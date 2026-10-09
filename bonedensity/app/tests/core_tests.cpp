#include <QtTest>

#include "device/deviceprotocol.h"
#include "measurement/frameanalyzer.h"
#include "measurement/measurementprofile.h"
#include "measurement/measurementsession.h"
#include "measurement/parametergroup.h"
#include "measurement/signalprocessor.h"
#include "measurement/utils.h"
#include "testframes.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <cmath>
#include <limits>

// Core-only tests: built with Qt Core/Xml/Test only (see core_tests.pro).
class CoreTests : public QObject
{
    Q_OBJECT

private slots:
    void acquireCommandEncoding();
    void completeFrameIsReturnedOnceWithTwelveBitSamples();
    void framesQueuedTogetherAreReturnedInOrderUntilCleared();
    void invalidChannelsAreDiscarded();
    void incompleteFrameGroupsAreBounded();
    void fragmentedFrameReassemblesAtEveryByteBoundary();
    void parserResynchronizesAfterNoiseAndBadTail();
    void interleavedAndWrappedFrameIndexesStayIndependent();
    void bufferWithoutFrameStartIsCapped();

    // Measurement core
    void parameterGroupOfTheFormalProfileIsUnchanged();
    void frameGatesCombineLikeTheOriginalChecks();
    void onsetConsistencyBoundaries();
    void clippedBPeakSearchCanBeCompletedWithoutFullRangeScan();
    void dualWindowQualityRequiresBothAtCommonLag();
    void partialRoundWaitsForRelockBeforeDiscarding();
    void finalRoundSelectionPreservesCompanionsAndBoundaries();
    void roundQualityGateAndClusteringEmitEvents();
};

void CoreTests::acquireCommandEncoding()
{
    QCOMPARE(DeviceProtocol::acquireCommand(0x1234, 0xABCD), QByteArray::fromHex("a55a3412cdab"));
    QCOMPARE(DeviceProtocol::acquireCommand(1024, 0), QByteArray::fromHex("a55a00040000"));
}

void CoreTests::completeFrameIsReturnedOnceWithTwelveBitSamples()
{
    FrameAssembler assembler;
    assembler.append(channelFrame(5, 3, 3) + channelFrame(5, 1, 0xF123)
                     + channelFrame(5, 4, 4) + channelFrame(5, 2, 2));
    WaveFrame frame;
    QVERIFY(assembler.takeFrame(&frame));
    QCOMPARE(frame.bc, QVector<quint16>({0x123}));
    QCOMPARE(frame.bd, QVector<quint16>({2}));
    QCOMPARE(frame.ac, QVector<quint16>({3}));
    QCOMPARE(frame.ad, QVector<quint16>({4}));
    QVERIFY(assembler.pendingFrames().isEmpty());
    QVERIFY(assembler.pendingOrder().isEmpty());
    QVERIFY(assembler.buffer().isEmpty());
    QVERIFY(!assembler.takeFrame(&frame));
}

void CoreTests::framesQueuedTogetherAreReturnedInOrderUntilCleared()
{
    FrameAssembler assembler;
    for (quint16 index : {quint16(1), quint16(2), quint16(3)}) {
        for (quint8 channel = 1; channel <= 4; ++channel)
            assembler.append(channelFrame(index, channel, index));
    }
    WaveFrame frame;
    QVERIFY(assembler.takeFrame(&frame));
    QCOMPARE(frame.bc, QVector<quint16>({1}));
    QVERIFY(assembler.takeFrame(&frame));
    QCOMPARE(frame.ad, QVector<quint16>({2}));
    // A reset while a frame is being processed drops whatever is still queued.
    assembler.clear();
    QVERIFY(!assembler.takeFrame(&frame));
    QVERIFY(assembler.buffer().isEmpty());
}

void CoreTests::invalidChannelsAreDiscarded()
{
    FrameAssembler assembler;
    WaveFrame frame;
    for (quint8 channel : {quint8(0), quint8(5), quint8(255)}) {
        assembler.append(channelFrame(channel, channel));
        QVERIFY(!assembler.takeFrame(&frame));
        QVERIFY(assembler.pendingFrames().isEmpty());
        QVERIFY(assembler.pendingOrder().isEmpty());
        QVERIFY(assembler.buffer().isEmpty());
    }
}

void CoreTests::incompleteFrameGroupsAreBounded()
{
    FrameAssembler assembler;
    WaveFrame frame;
    for (quint16 index = 0; index < 100; ++index) {
        assembler.append(channelFrame(index, 1));
        QVERIFY(!assembler.takeFrame(&frame));
    }
    QCOMPARE(assembler.pendingFrames().size(), FrameAssembler::maxPendingFrames);
    QCOMPARE(assembler.pendingOrder().size(), FrameAssembler::maxPendingFrames);
    QCOMPARE(assembler.pendingOrder().head(), quint16(84));
}

void CoreTests::fragmentedFrameReassemblesAtEveryByteBoundary()
{
    const QByteArray completeFrame = channelFrame(42, 3);
    for (int split = 1; split < completeFrame.size(); ++split) {
        FrameAssembler assembler;
        WaveFrame frame;
        assembler.append(completeFrame.left(split));
        QVERIFY(!assembler.takeFrame(&frame));
        QVERIFY2(!assembler.pendingFrames().contains(42),
                 qPrintable(QStringLiteral("frame completed before byte %1 arrived").arg(split)));

        assembler.append(completeFrame.mid(split));
        QVERIFY(!assembler.takeFrame(&frame));
        QVERIFY2(assembler.pendingFrames().contains(42),
                 qPrintable(QStringLiteral("frame did not reassemble at split %1").arg(split)));
        const WaveGroup group = assembler.pendingFrames().value(42);
        QVERIFY(group.has[2]);
        QCOMPARE(group.ch[2], QVector<quint16>({2048}));
        QVERIFY(assembler.buffer().isEmpty());
    }
}

void CoreTests::parserResynchronizesAfterNoiseAndBadTail()
{
    QByteArray badTail = channelFrame(10, 1);
    badTail[badTail.size() - 1] = char(0x00);
    QByteArray invalidLength = channelFrame(12, 4).left(9);
    invalidLength[7] = char(0x00);
    invalidLength[8] = char(0x00);

    FrameAssembler assembler;
    assembler.append(QByteArray::fromHex("010203aa00ff") + badTail + invalidLength
                     + channelFrame(11, 2));
    WaveFrame frame;
    QVERIFY(!assembler.takeFrame(&frame));

    QVERIFY(!assembler.pendingFrames().contains(10));
    QVERIFY(!assembler.pendingFrames().contains(12));
    QVERIFY(assembler.pendingFrames().contains(11));
    const WaveGroup group = assembler.pendingFrames().value(11);
    QVERIFY(group.has[1]);
    QCOMPARE(group.ch[1], QVector<quint16>({2048}));
    QVERIFY(assembler.buffer().isEmpty());
}

void CoreTests::interleavedAndWrappedFrameIndexesStayIndependent()
{
    FrameAssembler assembler;
    assembler.append(channelFrame(65535, 3) + channelFrame(0, 2)
                     + channelFrame(65535, 1) + channelFrame(0, 4));
    WaveFrame frame;
    QVERIFY(!assembler.takeFrame(&frame));

    const auto& groups = assembler.pendingFrames();
    QCOMPARE(groups.size(), 2);
    QVERIFY(groups.value(65535).has[0]);
    QVERIFY(groups.value(65535).has[2]);
    QVERIFY(!groups.value(65535).has[1]);
    QVERIFY(!groups.value(65535).has[3]);
    QVERIFY(!groups.value(0).has[0]);
    QVERIFY(groups.value(0).has[1]);
    QVERIFY(!groups.value(0).has[2]);
    QVERIFY(groups.value(0).has[3]);
    QCOMPARE(assembler.pendingOrder().size(), 2);
    QCOMPARE(assembler.pendingOrder().at(0), quint16(65535));
    QCOMPARE(assembler.pendingOrder().at(1), quint16(0));
    QVERIFY(assembler.buffer().isEmpty());
}

void CoreTests::bufferWithoutFrameStartIsCapped()
{
    FrameAssembler assembler;
    WaveFrame frame;
    assembler.append(QByteArray(4000, char(0x11)));
    QVERIFY(!assembler.takeFrame(&frame));
    QCOMPARE(assembler.buffer().size(), 4000);
    assembler.append(QByteArray(200, char(0x11)));
    QVERIFY(!assembler.takeFrame(&frame));
    // Only the last two bytes are kept: they may begin the next frame header.
    QCOMPARE(assembler.buffer().size(), 2);
}

void CoreTests::parameterGroupOfTheFormalProfileIsUnchanged()
{
#if defined(BONE_COMPLETE_B_PEAK_EXPERIMENT) || defined(BONE_RELOCK_PRESERVATION_EXPERIMENT) \
    || defined(BONE_DUAL_WINDOW_A_EXPERIMENT) || defined(BONE_OBSERVE_BEFORE_G_EXPERIMENT)
    QSKIP("Pinned for the formal profile only");
#else
    // Saved results and experiment folders are keyed by this id: a change of
    // any recorded parameter (or of its JSON form) must be deliberate.
    SignalProcessor processor;
    processor.probeDistanceCD = 7.84e-3;
    processor.samplePeriod = 16e-9;
    const QJsonObject parameters =
        measurementParameters(MeasureConfig(), MeasurementProfile::current(), processor);
    QCOMPARE(QJsonDocument(parameters).toJson(QJsonDocument::Compact),
             QByteArray(R"({"B_clipped_peak_extension":0,"B_only":true,"B_onset_forward_limit":40,)"
                        R"("D_max":15,"D_min":5,"G_max":0,"G_min":-12,"SOS_offset":0,)"
                        R"("angle_gate_enabled":true,"frame_corr_A":0.78,"frame_corr_B":0.55,)"
                        R"("frame_target":30,"implementation":"onset-consistency-20260915-v1",)"
                        R"("lag_tolerance":5,"lock_need":10,"partial_relock_retention_lag":2,)"
                        R"("probe_distance_m":0.00784,"round_cluster_tolerance":180,)"
                        R"("round_corr_A":0.78,"round_corr_B":0.55,"round_target":5,)"
                        R"("sample_period_s":1.6e-08,"unlock_count":10,"warmup":14,)"
                        R"("window_size":20})"));
    QCOMPARE(ParameterGroup::id(parameters), QStringLiteral("onset-consistency-20260915-v1_5a0ddeea"));
#endif
}

void CoreTests::frameGatesCombineLikeTheOriginalChecks()
{
    for (int mask = 0; mask < 256; ++mask) {
        FrameGates g;
        g.bJump = mask & 1; g.boundary = mask & 2; g.abDiff = mask & 4; g.direction = mask & 8;
        g.corrA = mask & 16; g.corrB = mask & 32; g.d = mask & 64; g.g = mask & 128;
        for (bool gate : {false, true}) {
            // The expressions of the former per-frame code.
            const bool angleOk = !gate || (g.d && g.g);
            const bool corrOk = g.corrB && g.corrA;
            const bool basePrechecksOk = g.bJump && g.boundary && g.abDiff && g.direction &&
                corrOk && (!gate || g.d);
            const bool strictValidWhenStable = g.bJump && g.boundary && g.abDiff && g.direction &&
                corrOk && angleOk;
            QCOMPARE(g.posture(gate), angleOk);
            QCOMPARE(g.prechecks(gate), basePrechecksOk);
            QCOMPARE(g.all(gate), strictValidWhenStable);
            QCOMPARE(g.all(gate), basePrechecksOk && angleOk);
        }
    }
}

void CoreTests::onsetConsistencyBoundaries()
{
    ArrivalResult a;a.valid=true;a.firstHit=100;a.onset=140;
    QVERIFY(a.onsetConsistent(40));
    a.onset=141;QVERIFY(!a.onsetConsistent(40));
    a.onset=99;QVERIFY(a.onsetConsistent(40));
    a.valid=false;QVERIFY(!a.onsetConsistent(40));
    a.valid=true;a.firstHit=-1;QVERIFY(!a.onsetConsistent(40));
}

void CoreTests::clippedBPeakSearchCanBeCompletedWithoutFullRangeScan()
{
    QVector<double> early(600), late(600);
    for (int i=80; i<=220; ++i) {
        const double x=(i-150)/3.0;
        early[i]=std::exp(-.5*x*x);
        late[i+140]=early[i];
    }
    double oldCorr=0, newCorr=0;
    const int oldLag=SignalProcessor::refineLagByPositiveCrossCorrelation(
        early,late,150,105,92,250,20,70,&oldCorr);
    const int newLag=SignalProcessor::refineLagByPositiveCrossCorrelation(
        early,late,150,105,92,250,20,70,&newCorr,15);
    QVERIFY(oldLag<=135);
    QCOMPARE(newLag,140);
    QVERIFY(newCorr>oldCorr);
}

void CoreTests::dualWindowQualityRequiresBothAtCommonLag()
{
    QVector<double> early(240), late(260);
    const int onset=80, lag=17;
    for (int i=0;i<early.size();++i) {
        early[i]=std::sin(i*.37)+.4*std::cos(i*.83);
        late[i+lag]=3*early[i]+4;
    }
    double front=0,middle=0;
    QCOMPARE(FrameAnalyzer::dualWindowAQuality(early,late,onset,lag,&front,&middle),1.0);
    QVERIFY(front>.999 && middle>.999);
    // The second window cannot independently choose a better lag.
    QVERIFY(FrameAnalyzer::dualWindowAQuality(early,late,onset,lag+4)<.78);
    for (int i=onset+31;i<=onset+60;++i) late[i+lag]=-20*early[i];
    const double badMiddle=FrameAnalyzer::dualWindowAQuality(early,late,onset,lag,&front,&middle);
    QVERIFY(front>.999); QVERIFY(middle<.78); QCOMPARE(badMiddle,qMin(front,middle));
    for (int i=onset-20;i<onset;++i) late[i+lag]=-40*early[i];
    FrameAnalyzer::dualWindowAQuality(early,late,onset,lag,&front,&middle);
    QVERIFY(front<.78);
    QVERIFY(FrameAnalyzer::dualWindowAQuality(early,late,10,lag)<=0.0);
    QCOMPARE(FrameAnalyzer::dualWindowAQuality(early,late,220,lag),0.0);
    QCOMPARE(FrameAnalyzer::dualWindowAQuality(early,late,onset,-1),0.0);
    QCOMPARE(FrameAnalyzer::dualWindowAQuality(QVector<double>(240,1),late,onset,lag),0.0);
    early[onset]=std::numeric_limits<double>::quiet_NaN();
    QCOMPARE(FrameAnalyzer::dualWindowAQuality(early,late,onset,lag),0.0);
}

void CoreTests::partialRoundWaitsForRelockBeforeDiscarding()
{
    SignalProcessor processor;
    MeasureConfig config;
    MeasurementProfile profile = MeasurementProfile::current();
    profile.deferPartialDiscardUntilRelock = true;
    const auto seedPartial = [&config](MeasurementSession& session) {
        for (int i=0;i<config.stableLagWarmupCount;++i) session.checkLagStable(128);
        QVERIFY(session.isLocked());
        for (int i=0;i<5;++i) session.currentRound.append(3900+i, 3800+i, 3900+i, .9, .9, 0, 9);
    };
    MeasurementSession same(processor, config, profile);
    seedPartial(same);
    for (int i=0;i<config.boneLagUnlockCount;++i) same.rejectLagCandidate();
    QVERIFY(!same.isLocked());
    QCOMPARE(same.currentRound.size(),5);
    for (int i=0;i<config.stableLagWarmupCount;++i) same.checkLagStable(128);
    QVERIFY(same.isLocked());
    QCOMPARE(same.currentRound.size(),5);

    MeasurementSession moved(processor, config, profile);
    seedPartial(moved);
    for (int i=0;i<config.boneLagUnlockCount;++i) moved.rejectLagCandidate();
    QCOMPARE(moved.currentRound.size(),5);
    for (int i=0;i<config.stableLagWarmupCount;++i) moved.checkLagStable(150);
    QVERIFY(moved.isLocked());
    QVERIFY(moved.currentRound.isEmpty());
    QCOMPARE(moved.validCount(),0);
}

void CoreTests::finalRoundSelectionPreservesCompanionsAndBoundaries()
{
    SignalProcessor processor;
    MeasureConfig config;
    const MeasurementProfile profile = MeasurementProfile::current();
    MeasurementSession session(processor, config, profile);
    session.roundSos = {3910,3957,3946,3952,4153,3984};
    session.roundA = {10,20,30,40,5000,60};
    session.roundB = {100,200,300,400,50000,600};
    const auto original = session.roundSos;
    double sos=-1, a=-1, b=-1;
    QVector<int> selected;
    QVERIFY(session.finalMeans(sos,a,b,&selected));
    QCOMPARE(selected, QVector<int>({0,1,2,3,5}));
    QCOMPARE(sos,3949.8);
    QCOMPARE(a,32.0);
    QCOMPARE(b,320.0);
    QCOMPARE(session.roundSos,original);
    session.roundSos = {3984,3910,3946,3957,3952};
    session.roundA = {10,20,30,40,60};
    session.roundB = {100,200,300,400,600};
    QVERIFY(session.finalMeans(sos,a,b,&selected));
    QCOMPARE(selected,QVector<int>({0,1,2,3,4}));
    QCOMPARE(sos,Utils::trimmedMeanValue(session.roundSos,0.2));
    QCOMPARE(a,Utils::trimmedMeanValue(session.roundA,0.2));
    QCOMPARE(b,Utils::trimmedMeanValue(session.roundB,0.2));
    QCOMPARE(MeasurementSession::selectFinalRoundIndices({1,2,3,4,5,6},5),QVector<int>({0,1,2,3,4}));
    QCOMPARE(MeasurementSession::selectFinalRoundIndices({5,5,5,5,5,5},5),QVector<int>({0,1,2,3,4}));
    QVERIFY(MeasurementSession::selectFinalRoundIndices({1,2,3,4},5).isEmpty());
    QVERIFY(MeasurementSession::selectFinalRoundIndices({1,2,3,4,5},0).isEmpty());
    QVERIFY(MeasurementSession::selectFinalRoundIndices({1,2,3,4,std::numeric_limits<double>::quiet_NaN()},5).isEmpty());
    session.roundA.removeLast();
    sos=a=b=-1;
    QVERIFY(!session.finalMeans(sos,a,b));
    QCOMPARE(sos,-1.0);
    QCOMPARE(a,-1.0);
    QCOMPARE(b,-1.0);
}

void CoreTests::roundQualityGateAndClusteringEmitEvents()
{
    SignalProcessor processor;
    MeasureConfig config;
    const MeasurementProfile profile = MeasurementProfile::current();
    MeasurementSession session(processor, config, profile);
    QVector<QJsonObject> events;
    session.log = [&events](const QJsonObject& event) { events.append(event); };
    const auto fillRound = [&](double sos, double corrA) {
        for (int i = 0; i < config.framesPerRound; ++i) {
            QVERIFY(!session.roundFull());
            session.addValue(sos, sos - 100, sos, corrA, .95, -3, 9);
        }
        QVERIFY(session.roundFull());
    };

    // Below the round corrA floor: rejected, nothing kept.
    fillRound(3900, .5);
    RoundOutcome outcome = session.completeRound();
    QVERIFY(!outcome.accepted);
    QVERIFY(!outcome.summary.qualityPass);
    QVERIFY(session.currentRound.isEmpty());
    QVERIFY(session.candidates.isEmpty());
    QCOMPARE(events.size(), 1);
    QCOMPARE(events.last()["event"].toString(), QStringLiteral("round_summary"));
    QVERIFY(!events.last()["quality_pass"].toBool());

    // Five consistent rounds and one outlier: the outlier stays a candidate only.
    for (double sos : {3910., 3957., 3946., 4400., 3952., 3984.}) {
        fillRound(sos, .95);
        outcome = session.completeRound();
        QVERIFY(outcome.accepted);
        QCOMPARE(outcome.acceptedRounds, session.roundSos.size());
    }
    QCOMPARE(session.candidates.size(), 6);
    QCOMPARE(session.roundSos, QVector<double>({3910, 3957, 3946, 3952, 3984}));
    QVERIFY(session.measurementComplete());
    QVERIFY(!session.hasIncompleteRounds());
    QCOMPARE(events.last()["event"].toString(), QStringLiteral("final_round_selection"));
    QCOMPARE(events.last()["selected_indices"].toArray(), QJsonArray({0, 1, 2, 3, 4}));

    session.resetAll();
    QVERIFY(session.roundSos.isEmpty());
    QVERIFY(session.candidates.isEmpty());
    QVERIFY(!session.isLocked());
}

QTEST_GUILESS_MAIN(CoreTests)
#include "core_tests.moc"
