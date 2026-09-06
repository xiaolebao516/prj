#include <QtTest>

#include "mainwindow.h"
#include "calibrationdialog.h"
#include "measurementguidedialog.h"
#include "reportwidget.h"
#include "ui_mainwindow.h"

#include <QAbstractButton>
#include <QCloseEvent>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QMessageBox>
#include <QPageLayout>
#include <QPageSize>
#include <QPainter>
#include <QPrinter>
#include <QTemporaryDir>
#include <QTimer>
#include <QStackedWidget>
#include <cmath>
#include <limits>

namespace {

class FakeOpenSerialPort : public QSerialPort
{
public:
    explicit FakeOpenSerialPort(QObject* parent = nullptr)
        : QSerialPort(parent)
    {
    }

    void openForTest()
    {
        setOpenMode(QIODevice::ReadWrite);
    }
};

QByteArray frame(quint16 index, quint8 channel)
{
    const quint16 gain = 1024;
    const quint16 sample = 2048;
    QByteArray bytes;
    bytes.append(char(0xAA));
    bytes.append(char(0x55));
    bytes.append(char(gain & 0xFF));
    bytes.append(char((gain >> 8) & 0xFF));
    bytes.append(char(index & 0xFF));
    bytes.append(char((index >> 8) & 0xFF));
    bytes.append(char(channel));
    bytes.append(char(1));
    bytes.append(char(0));
    bytes.append(char(sample & 0xFF));
    bytes.append(char((sample >> 8) & 0xFF));
    bytes.append(char(0xEE));
    bytes.append(char(0xEE));
    return bytes;
}

PatientInfo samplePatient()
{
    PatientInfo patient;
    patient.id = QStringLiteral("patient-001");
    patient.name = QStringLiteral("测试患者");
    patient.gender = QStringLiteral("女");
    patient.birthDay = QStringLiteral("1990-01-02");
    patient.height = QStringLiteral("165");
    patient.weight = QStringLiteral("55");
    return patient;
}

MeasurementRecord sampleMeasurement()
{
    MeasurementRecord record;
    record.id = QStringLiteral("measurement-001");
    record.patientId = QStringLiteral("patient-001");
    record.measuredAt = QStringLiteral("2026-07-22T10:30:00");
    record.sos = QStringLiteral("4000.0");
    return record;
}

} // namespace

class MainWindowSafetyTests : public QObject
{
    Q_OBJECT

private slots:
    void dualWindowQualityRequiresBothAtCommonLag();
    void trialRoundQualityStillRejectsBelowFloor();
    void observeBeforeGPreservesAcceptanceAndExpiry();
    void experimentBuildIdentity();
    void rejectedFramesExpirePatientStability();
    void clusterLossDiscardsPartialRound();
    void transientRejectionPreservesProgressAndSteadySequence();
    void experimentRecordingIsBoundedAndUnique();
    void experimentRecordingIncludesEarlyFailuresAndRawInput();
    void precheckFailuresExpireStateThroughActualPipeline();
    void experimentRecordingCoversFeatureDecisions();
    void experimentRecordingThroughput();
    void invalidChannelsAreDiscarded();
    void incompleteFrameGroupsAreBounded();
    void fragmentedFrameReassemblesAtEveryByteBoundary();
    void parserResynchronizesAfterNoiseAndBadTail();
    void interleavedAndWrappedFrameIndexesStayIndependent();
    void speedSeriesKeepsOnlyRecentPoints();
    void disconnectedControlsAndPlaceholdersAreSafe();
    void positionGuideTracksExistingBarsWithoutChangingThem();
    void measurementStatusIsVisibleAndOperatorFacing();
    void measurementGuideHasThreeApprovedPagesAndPortableMarker();
    void measurementGuideFirstUseAndSpaceContinue();
    void patientMeasurementDisablesConflictingControls();
    void debugAutoDisablesConflictingNavigation();
    void patientFormsStayInsideAndCenteredAtSmallWindow();
    void pendingResultBlocksAnotherMeasurement();
    void samePatientReselectionPreservesPendingResult();
    void samePatientReselectionPreservesPartialRounds();
    void cancelledPatientSwitchPreservesPartialRounds();
    void cancelledQuickPatientCreationDoesNotWritePatient();
    void partialRoundsRequireCloseConfirmation();
    void activeFirstRoundClosePausesAndCanResume();
    void partialRoundsBlockCalibrationDialog();
    void futureBirthDateBlocksMeasurement();
    void pendingSaveValidatesTheRecordedPatient();
    void pendingTransactionBlocksSingleFileWrites();
    void patientMeasurementStartClearsSerialAssembly();
    void serialIoErrorsResetAcquisition_data();
    void serialIoErrorsResetAcquisition();
    void reportRenderingIsReadableAndArtifactFree();
    void invalidMeasurementDateDoesNotInventAge();
    void completedReportUsesProvidedMeasurement();
    void reportPdfCanBeCommitted();
    void capturePagesWhenRequested();
};

void MainWindowSafetyTests::dualWindowQualityRequiresBothAtCommonLag()
{
    QVector<double> early(240), late(260);
    const int onset=80, lag=17;
    for (int i=0;i<early.size();++i) {
        early[i]=std::sin(i*.37)+.4*std::cos(i*.83);
        late[i+lag]=3*early[i]+4;
    }
    double front=0,middle=0;
    QCOMPARE(MainWindow::dualWindowAQuality(early,late,onset,lag,&front,&middle),1.0);
    QVERIFY(front>.999 && middle>.999);
    // The second window cannot independently choose a better lag.
    QVERIFY(MainWindow::dualWindowAQuality(early,late,onset,lag+4)<.78);
    for (int i=onset+31;i<=onset+60;++i) late[i+lag]=-20*early[i];
    const double badMiddle=MainWindow::dualWindowAQuality(early,late,onset,lag,&front,&middle);
    QVERIFY(front>.999); QVERIFY(middle<.78); QCOMPARE(badMiddle,qMin(front,middle));
    for (int i=onset-20;i<onset;++i) late[i+lag]=-40*early[i];
    MainWindow::dualWindowAQuality(early,late,onset,lag,&front,&middle);
    QVERIFY(front<.78);
    QVERIFY(MainWindow::dualWindowAQuality(early,late,10,lag)<=0.0);
    QCOMPARE(MainWindow::dualWindowAQuality(early,late,220,lag),0.0);
    QCOMPARE(MainWindow::dualWindowAQuality(early,late,onset,-1),0.0);
    QCOMPARE(MainWindow::dualWindowAQuality(QVector<double>(240,1),late,onset,lag),0.0);
    early[onset]=std::numeric_limits<double>::quiet_NaN();
    QCOMPARE(MainWindow::dualWindowAQuality(early,late,onset,lag),0.0);
}

void MainWindowSafetyTests::trialRoundQualityStillRejectsBelowFloor()
{
    // Exercise actual aggregation on either build, without changing its configured floor.
    for (int scenario=0;scenario<4;++scenario) {
        QTemporaryDir directory;
        QVERIFY(directory.isValid());
        MainWindow window;
        window.patientMeasureRunning=true;
        window.acquireMode=PatientMeasureMode;
        QVERIFY(window.experimentLog.start(directory.path(),{}));
        const QString path=window.experimentLog.path();
        const double a=scenario==0 ? .779 : .799;
        const double b=scenario==2 ? .549 : .95;
        const double g=scenario==3 ? 7 : 0;
        for (int i=0;i<30;++i)
            window.handlePatientMeasureValue(3500,3828,3828,137,128,9,g,a,b,true);
        window.stopPatientMeasurement();
        QFile file(path); QVERIFY(file.open(QIODevice::ReadOnly));
        bool found=false;
        for (const auto& line:file.readAll().split('\n')) {
            const auto row=QJsonDocument::fromJson(line).object();
            if (row["event"]!="round_summary") continue;
            found=true;
            QCOMPARE(row["quality_pass"].toBool(),scenario==1 && window.useDualWindowAQuality);
        }
        QVERIFY(found);
        window.closeRoundFinishedTip();
    }
}

void MainWindowSafetyTests::observeBeforeGPreservesAcceptanceAndExpiry()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const auto wave = [](double center) {
        QVector<double> signal(1800);
        for (int i=0;i<signal.size();++i) {
            const double t=i-center;
            if (std::abs(t)<=100)
                signal[i]=1000*std::exp(-t*t/(2*35.0*35.0))*std::cos(2*3.141592653589793*t/50);
        }
        return signal;
    };
    const auto bc=wave(928),bd=wave(800);
    MainWindow probe;
    probe.useDualWindowAQuality=false;
    probe.observeStabilityBeforeG=false;
    probe.patientMeasureRunning=true;
    probe.acquireMode=PatientMeasureMode;
    QVERIFY(probe.experimentLog.start(directory.path(),{}));
    const auto scanPath=probe.experimentLog.path();
    for (int shift=-50;shift<=20;++shift)
        probe.detectAndPlotSpeed(bc,bd,wave(937+shift),wave(800+shift));
    probe.stopPatientMeasurement();
    QFile scan(scanPath); QVERIFY(scan.open(QIODevice::ReadOnly));
    int shift=-50,goodShift=1000,badGShift=1000;
    for (const auto& line:scan.readAll().trimmed().split('\n')) {
        const auto row=QJsonDocument::fromJson(line).object();
        if (row.value("event")!="frame") continue;
        const auto gates=row.value("gates").toObject();
        if (!gates.isEmpty()) {
            bool base=true;
            for (const char* key:{"B_jump","boundary","AB_diff","direction","corr_A","corr_B","D"})
                base=base && gates.value(key).toBool();
            if (base && gates.value("G").toBool()) goodShift=shift;
            if (base && !gates.value("G").toBool()) badGShift=shift;
        }
        ++shift;
    }
    QVERIFY(goodShift!=1000 && badGShift!=1000);
    for (bool experimental:{false,true}) {
        MainWindow window;
        window.observeStabilityBeforeG=experimental;
        window.useDualWindowAQuality=false;
        window.patientMeasureRunning=true;
        window.acquireMode=PatientMeasureMode;
        const auto feed=[&](int offset) {
            window.detectAndPlotSpeed(bc,bd,wave(937+offset),wave(800+offset));
        };
        for (int i=0;i<20;++i) feed(badGShift);
        QCOMPARE(window.processValidCount,0);
        QVERIFY(window.currentRoundSosList.isEmpty());
        QCOMPARE(window.boneLagLocked,experimental);
        feed(goodShift);
        QCOMPARE(window.processValidCount,experimental ? 1 : 0);
        if (!experimental) continue;
        for (int i=0;i<20;++i) feed(badGShift);
        QCOMPARE(window.processValidCount,1);
        QVERIFY(window.boneLagLocked);
        for (int i=0;i<window.mCfg.boneLagUnlockCount;++i)
            window.detectAndPlotSpeed(wave(948),bd,wave(957+badGShift),wave(800+badGShift));
        QCOMPARE(window.processValidCount,0);
        QVERIFY(window.currentRoundSosList.isEmpty());
        QVERIFY(!window.boneLagLocked);
        for (int i=0;i<20;++i) feed(badGShift);
        feed(goodShift);
        QCOMPARE(window.processValidCount,1);
        for (int i=0;i<window.mCfg.boneLagUnlockCount;++i)
            window.detectAndPlotSpeed(QVector<double>(1800),QVector<double>(1800),QVector<double>(1800),QVector<double>(1800));
        QCOMPARE(window.processValidCount,0);
        QVERIFY(window.currentRoundSosList.isEmpty());
        QVERIFY(!window.boneLagLocked);
        feed(goodShift);
        QCOMPARE(window.processValidCount,0);
    }
}

void MainWindowSafetyTests::experimentBuildIdentity()
{
    MainWindow window;
#ifdef BONE_DUAL_WINDOW_A_EXPERIMENT
    QVERIFY(window.useDualWindowAQuality);
    QVERIFY(window.observeStabilityBeforeG);
    QCOMPARE(window.mCfg.roundCorrAMin,.78);
    QVERIFY(window.windowTitle().contains(QStringLiteral("双段评分试测版")));
#elif defined(BONE_OBSERVE_BEFORE_G_EXPERIMENT)
    QVERIFY(!window.useDualWindowAQuality);
    QCOMPARE(window.mCfg.roundCorrAMin,.80);
    QVERIFY(window.observeStabilityBeforeG);
    QVERIFY(window.windowTitle().contains(QStringLiteral("试测版")));
#else
    QVERIFY(!window.useDualWindowAQuality);
    QCOMPARE(window.mCfg.roundCorrAMin,.80);
    QVERIFY(!window.observeStabilityBeforeG);
    QVERIFY(!window.windowTitle().contains(QStringLiteral("试测版")));
#endif
#ifndef QT_NO_DEBUG
    window.startExperimentLog();
    QVERIFY(window.experimentLog.active());
    const auto path=window.experimentLog.path();
    window.stopPatientMeasurement();
    QFile log(path); QVERIFY(log.open(QIODevice::ReadOnly));
    const auto config=QJsonDocument::fromJson(log.readLine()).object().value("config").toObject();
    QCOMPARE(config.value("round_corr_A").toDouble(),window.mCfg.roundCorrAMin);
    QCOMPARE(config.value("frame_corr_A").toDouble(),.78);
    QCOMPARE(config.value("implementation").toString(), window.useDualWindowAQuality
        ? QStringLiteral("dual-window-a078-20260906-v1") : window.observeStabilityBeforeG
        ? QStringLiteral("observe-before-g-20260906-v1") : QStringLiteral("state-repair-20260905-v1"));
#endif
}

void MainWindowSafetyTests::rejectedFramesExpirePatientStability()
{
    MainWindow window;
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    for (int i = 0; i < window.mCfg.stableLagWarmupCount - 1; ++i)
        QVERIFY(!window.checkBoneLagStable(128));
    for (int i = 0; i < window.mCfg.boneLagUnlockCount; ++i)
        window.updateProcessInvalid("synthetic invalid frame");
    QVERIFY(window.recentBoneLagBList.isEmpty());
    QVERIFY(!window.checkBoneLagStable(128));
    QVERIFY(!window.boneLagLocked);
}

void MainWindowSafetyTests::clusterLossDiscardsPartialRound()
{
    MainWindow window;
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    auto feed = [&](int lag) {
        const bool accepted = window.checkBoneLagStable(lag);
        window.handlePatientMeasureValue(490000.0 / (lag + 9), 490000.0 / lag,
            490000.0 / lag, lag + 9, lag, 9, 0, .95, .95, accepted);
    };
    for (int i = 0; i < 28; ++i) feed(120);
    QCOMPARE(window.currentRoundSosList.size(), 15);
    for (int i = 0; i < window.mCfg.boneLagUnlockCount; ++i) feed(140);
    QVERIFY(window.currentRoundSosList.isEmpty());
    QVERIFY(window.currentRoundAList.isEmpty());
    QVERIFY(window.currentRoundBList.isEmpty());
    QVERIFY(window.currentRoundCorrAList.isEmpty());
    QVERIFY(window.currentRoundCorrBList.isEmpty());
    QVERIFY(window.currentRoundPairMidGapList.isEmpty());
    QVERIFY(window.currentRoundSignedLagDiffList.isEmpty());
    QCOMPARE(window.processValidCount, 0);
    QVERIFY(window.patientMeasureRunning);
    for (int i = 0; i < 28; ++i) feed(140);
    QVERIFY(window.candidateRoundList.isEmpty());
    QCOMPARE(window.currentRoundSosList.size(), 15);
    for (int i = 0; i < 15; ++i) feed(140);
    QCOMPARE(window.candidateRoundList.size(), 1);
    QVERIFY(qAbs(window.candidateRoundList.first().sos - 3500.0) < .001);
    window.closeRoundFinishedTip();
}

void MainWindowSafetyTests::transientRejectionPreservesProgressAndSteadySequence()
{
    MainWindow window;
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    for (int i = 1; i <= window.mCfg.stableLagWarmupCount; ++i)
        QCOMPARE(window.checkBoneLagStable(128), i == window.mCfg.stableLagWarmupCount);
    window.handlePatientMeasureValue(3500, 3828, 3828, 137, 128, 9, 0, .95, .95, true);
    for (int i = 0; i < window.mCfg.boneLagUnlockCount - 1; ++i)
        window.updateProcessInvalid("transient");
    QVERIFY(window.boneLagLocked);
    QCOMPARE(window.currentRoundSosList.size(), 1);
    QVERIFY(window.checkBoneLagStable(128));
    window.updateProcessInvalid("one more transient after recovery");
    QVERIFY(window.boneLagLocked);
    for (int i = 1; i < window.mCfg.boneLagUnlockCount; ++i)
        window.updateProcessInvalid("sustained loss");
    QVERIFY(!window.boneLagLocked);
    QVERIFY(window.currentRoundSosList.isEmpty());
    QCOMPARE(window.processValidCount, 0);
}

void MainWindowSafetyTests::experimentRecordingIsBoundedAndUnique()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MeasurementExperimentLog log;
    QVERIFY(log.start(directory.path(), {{"test", true}}, 4096, 16384));
    const QString firstPath = log.path();
    QCOMPARE(MeasurementExperimentLog::encodeRaw({0, 256, 65535}),
             QString::fromLatin1(QByteArray::fromHex("00000001ffff").toBase64()));
    QVERIFY(log.write({{"event", "frame"}, {"valid", false}}));
    QVERIFY(log.close());
    QFile first(firstPath);
    QVERIFY(first.open(QIODevice::ReadOnly));
    const QByteArray original = first.readAll();
    QVERIFY(log.start(directory.path(), {}, 4096, 16384));
    QVERIFY(log.path() != firstPath);
    QVERIFY(!log.write({{"oversized", QString(8192, 'x')}}));
    QVERIFY(!log.active());
    QVERIFY(!log.error().isEmpty());
    QVERIFY(QFileInfo(log.path()).size() <= 4096);
    first.seek(0);
    QCOMPARE(first.readAll(), original);
    QVERIFY(!log.start(directory.path(), {}, 4096, original.size()));
    // A file where a directory is required simulates an unavailable destination.
    QVERIFY(!log.start(firstPath + "/child", {}));
}

void MainWindowSafetyTests::experimentRecordingIncludesEarlyFailuresAndRawInput()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    window.currentPatient = samplePatient();
    window.currentPatient.name = "DO_NOT_LOG_NAME";
    window.currentPatient.id = "DO_NOT_LOG_ID";
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    QVERIFY(window.experimentLog.start(directory.path(), {}));
    const QString path = window.experimentLog.path();
    window.samplesA = {0, 256, 65535};
    window.samplesB = {2};
    window.samplesC = {3};
    window.samplesD = {4};
    window.detectAndPlotSpeed({}, {}, {}, {});
    QVector<double> flat(1800, 0.0);
    window.detectAndPlotSpeed(flat, flat, flat, flat);
    window.stopPatientMeasurement();
    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly));
    const QByteArray bytes = file.readAll();
    QVERIFY(!bytes.contains("DO_NOT_LOG"));
    const auto rows = bytes.trimmed().split('\n');
    QCOMPARE(rows.size(), 4); // start, two rejected frames, stop
    const QJsonObject emptyFrame = QJsonDocument::fromJson(rows[1]).object();
    QCOMPARE(emptyFrame.value("decision").toString(), QString("empty_filtered_input"));
    QCOMPARE(emptyFrame.value("raw_BC").toString(),
             MeasurementExperimentLog::encodeRaw(window.samplesA));
    const QJsonObject invalidFrame = QJsonDocument::fromJson(rows[2]).object();
    QCOMPARE(invalidFrame.value("decision").toString(), QString("B_pair_invalid"));
    QVERIFY(!invalidFrame.value("B").toObject().value("valid").toBool());
    QVERIFY(invalidFrame.value("elapsed_ms").toInteger() >= emptyFrame.value("elapsed_ms").toInteger());
    QCOMPARE(window.processValidCount, 0);
    // Logging failure must not grant/reject a valid candidate or stop acquisition.
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    QVERIFY(!window.experimentLog.start(path + "/blocked", {}));
    window.checkExperimentLogError();
    QVERIFY(window.experimentLogWarningShown);
    QVERIFY(window.statusBar()->currentMessage().contains(QStringLiteral("实验记录")));
    for (int i = 1; i <= window.mCfg.stableLagWarmupCount; ++i)
        QCOMPARE(window.checkBoneLagStable(128), i == window.mCfg.stableLagWarmupCount);
    QVERIFY(window.patientMeasureRunning);
}

void MainWindowSafetyTests::precheckFailuresExpireStateThroughActualPipeline()
{
    MainWindow window;
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    for (int i = 0; i < 14; ++i) window.checkBoneLagStable(128);
    window.handlePatientMeasureValue(3500, 3828, 3828, 137, 128, 9, 0, .95, .95, true);
    QVector<double> flat(1800, 0.0);
    for (int i = 0; i < window.mCfg.boneLagUnlockCount; ++i)
        window.detectAndPlotSpeed(flat, flat, flat, flat);
    QVERIFY(!window.boneLagLocked);
    QVERIFY(window.currentRoundSosList.isEmpty());
    QVERIFY(!window.ui->barPairA->isEnabled());
    QVERIFY(!window.ui->barPairB->isEnabled());
    QCOMPARE(window.ui->lblPairAValue->text(), QString("D=--"));
    window.updateProcessPanel(3500, 3828, 137, 128, 9, 0, false);
    QVERIFY(window.ui->barPairA->isEnabled());
    QVERIFY(window.ui->barPairB->isEnabled());
    QCOMPARE(window.processValidCount, 0);
}

void MainWindowSafetyTests::experimentRecordingCoversFeatureDecisions()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    // This existing fixture explicitly locks down default-flow behavior.
    window.observeStabilityBeforeG = false;
    window.useDualWindowAQuality = false;
    window.mCfg.roundCorrAMin = .80;
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    QVERIFY(window.experimentLog.start(directory.path(), {}));
    const QString path = window.experimentLog.path();
    // Exactly delayed synthetic wave packets: tests logger coverage, not accuracy.
    const auto wave = [](double center) {
        QVector<double> signal(1800);
        for (int i = 0; i < signal.size(); ++i) {
            const double t = i - center;
            // Finite packet support: zero-noise Gaussian tails otherwise trigger
            // the onset detector long before the intended synthetic packet.
            if (std::abs(t) > 100.0) continue;
            signal[i] = 1000.0 * std::exp(-t*t / (2.0*35.0*35.0))
                * std::cos(2.0*3.141592653589793*t/50.0);
        }
        return signal;
    };
    const auto bc = wave(928), bd = wave(800);
    for (int shift = -50; shift <= 20; ++shift)
        window.detectAndPlotSpeed(bc, bd, wave(937 + shift), wave(800 + shift));
    window.stopPatientMeasurement();
    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly));
    const auto rows = file.readAll().trimmed().split('\n');
    int frames = 0, features = 0, gates = 0;
    int usableShift = 1000;
    for (const QByteArray& line : rows) {
        const auto row = QJsonDocument::fromJson(line).object();
        if (row.value("event") != "frame") continue;
        ++frames;
        if (row.contains("G")) {
            ++features;
            QVERIFY(row.contains("A_feature_branch"));
            const auto a = row.value("A").toObject();
            const auto b = row.value("B").toObject();
            QCOMPARE(row.value("D").toInt(), a.value("lag").toInt() - b.value("lag").toInt());
            QCOMPARE(row.value("G").toDouble(), .5 *
                (b.value("early_feature").toInt() + b.value("late_feature").toInt() -
                 a.value("early_feature").toInt() - a.value("late_feature").toInt()));
        }
        if (row.contains("gates")) {
            ++gates;
            const auto checks = row.value("gates").toObject();
            QVERIFY(checks.contains("stability_evaluated"));
            QVERIFY(checks.contains("corr_A"));
            QVERIFY(row.value("decision") == "accepted" || row.value("decision") == "rejected");
            if (checks.value("stability_evaluated").toBool()) usableShift = frames - 51;
        }
    }
    QCOMPARE(frames, 71);
    QVERIFY(features > 0);
    QVERIFY(gates > 0);
    QVERIFY(usableShift != 1000);
    window.resetOneRoundMeasurementState();
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    QVERIFY(window.experimentLog.start(directory.path(), {}));
    const QString completePath = window.experimentLog.path();
    const int neededFrames = window.mCfg.stableLagWarmupCount - 1 + window.processValidTarget;
    for (int i = 0; i < neededFrames; ++i)
        window.detectAndPlotSpeed(bc, bd, wave(937 + usableShift), wave(800 + usableShift));
    QCOMPARE(window.candidateRoundList.size(), 1);
    QVERIFY(!window.patientMeasureRunning);
    QVERIFY(!window.experimentLog.active());
    QFile complete(completePath);
    QVERIFY(complete.open(QIODevice::ReadOnly));
    int accepted = 0, summaries = 0, frameCount = 0;
    QString lastEvent;
    for (const auto& line : complete.readAll().trimmed().split('\n')) {
        const auto row = QJsonDocument::fromJson(line).object();
        lastEvent = row.value("event").toString();
        if (lastEvent == "frame") {
            ++frameCount;
            if (row.value("decision") == "accepted") ++accepted;
        }
        if (lastEvent == "round_summary") {
            ++summaries;
            QVERIFY(row.value("quality_pass").toBool());
            QCOMPARE(row.value("sos").toDouble(), window.candidateRoundList.first().sos);
        }
    }
    QCOMPARE(accepted, window.processValidTarget);
    QCOMPARE(frameCount, neededFrames);
    QCOMPARE(summaries, 1);
    QCOMPARE(lastEvent, QString("stop"));
    window.closeRoundFinishedTip();
}

void MainWindowSafetyTests::experimentRecordingThroughput()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MeasurementExperimentLog log;
    QVERIFY(log.start(directory.path(), {}));
    const QVector<quint16> raw(1800, 2048);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 500; ++i) {
        QVERIFY(log.write({{"event", "frame"},
            {"raw_BC", MeasurementExperimentLog::encodeRaw(raw)},
            {"raw_BD", MeasurementExperimentLog::encodeRaw(raw)},
            {"raw_AC", MeasurementExperimentLog::encodeRaw(raw)},
            {"raw_AD", MeasurementExperimentLog::encodeRaw(raw)}}));
    }
    QVERIFY(log.close());
    qInfo() << "Synthetic recording: 500 four-channel x 1800-sample frames in"
            << timer.elapsed() << "ms including encoding and final flush; local disk only";
    QVERIFY(QFileInfo(log.path()).size() > 9000000);
}

void MainWindowSafetyTests::invalidChannelsAreDiscarded()
{
    MainWindow window;
    window.hide();
    for (quint8 channel : {quint8(0), quint8(5), quint8(255)}) {
        window.rxBuffer.append(frame(channel, channel));
        window.parseIncomingData();
        QVERIFY(window.frameGroups.isEmpty());
        QVERIFY(window.frameGroupOrder.isEmpty());
        QVERIFY(window.rxBuffer.isEmpty());
    }
}

void MainWindowSafetyTests::incompleteFrameGroupsAreBounded()
{
    MainWindow window;
    window.hide();
    for (quint16 index = 0; index < 100; ++index) {
        window.rxBuffer.append(frame(index, 1));
        window.parseIncomingData();
    }
    QCOMPARE(window.frameGroups.size(), MainWindow::maxIncompleteFrameGroups);
    QCOMPARE(window.frameGroupOrder.size(), MainWindow::maxIncompleteFrameGroups);
    QCOMPARE(window.frameGroupOrder.head(), quint16(84));
}

void MainWindowSafetyTests::positionGuideTracksExistingBarsWithoutChangingThem()
{
    MainWindow window;
    window.hide();

    QCOMPARE(window.ui->barPairB->geometry(), QRect(30, 30, 81, 201));
    QCOMPARE(window.ui->barPairA->geometry(), QRect(370, 30, 81, 201));
    QCOMPARE(window.ui->lblBPairTitle->text(), QStringLiteral("G 倾角平衡"));
    QCOMPARE(window.ui->lblAPairTitle->text(), QStringLiteral("D 位置平衡"));
    QCOMPARE(window.ui->lblPositionGuide->text(),
             QStringLiteral("先调右侧 D：空间位置\n再调左侧 G：左右倾角"));
    QCOMPARE(window.ui->lblPositionGuideNote->text(),
             QStringLiteral("最终使右侧 D、左侧 G 均稳定在中线"));
    QVERIFY(window.ui->lblPositionGuide->styleSheet().contains(
        QStringLiteral("font-weight: bold")));
    QVERIFY(!window.ui->lblPositionGuide->geometry().intersects(
        window.ui->barPairB->geometry()));
    QVERIFY(!window.ui->lblPositionGuide->geometry().intersects(
        window.ui->barPairA->geometry()));
    QVERIFY(!window.ui->lblPositionGuideNote->geometry().intersects(
        window.ui->barPairB->geometry()));
    QVERIFY(!window.ui->lblPositionGuideNote->geometry().intersects(
        window.ui->barPairA->geometry()));
    QCOMPARE(window.ui->btnMeasurementGuide->text(),
             QStringLiteral("操作教学"));
    QVERIFY(!window.ui->btnMeasurementGuide->geometry().intersects(
        window.ui->barPairB->geometry()));
    QVERIFY(!window.ui->btnMeasurementGuide->geometry().intersects(
        window.ui->barPairA->geometry()));
    QVERIFY(!window.ui->btnMeasurementGuide->geometry().intersects(
        window.ui->lblPositionGuide->geometry()));
    QVERIFY(!window.ui->btnMeasurementGuide->geometry().intersects(
        window.ui->lblPositionGuideNote->geometry()));

    const QString fixedGuide = window.ui->lblPositionGuide->text();
    const QString fixedNote = window.ui->lblPositionGuideNote->text();
    window.updateProcessPanel(0.0, 0.0, 105, 100, 5, 8.0, false);
    window.updateProcessPanel(0.0, 0.0, 107, 100, 7, 4.0, true);
    window.updateProcessInvalid(QStringLiteral("测试无效帧"));
    QCOMPARE(window.ui->lblPositionGuide->text(), fixedGuide);
    QCOMPARE(window.ui->lblPositionGuideNote->text(), fixedNote);
    QCOMPARE(window.ui->barMeasureProgress->value(), 1);
}

void MainWindowSafetyTests::measurementStatusIsVisibleAndOperatorFacing()
{
    MainWindow window;
    window.hide();

    QVERIFY(window.ui->widgetBalanceArea->rect().contains(
        window.ui->lblProcessStatus->geometry()));

    const auto hasDeveloperWording = [](const QString& text) {
        return text.contains(QStringLiteral("Gap"), Qt::CaseInsensitive) ||
               text.contains(QStringLiteral("lag"), Qt::CaseInsensitive) ||
               text.contains(QStringLiteral("Corr"), Qt::CaseInsensitive) ||
               text.contains(QStringLiteral("pair"), Qt::CaseInsensitive) ||
               text.contains(QStringLiteral("Qt Creator"), Qt::CaseInsensitive) ||
               text.contains(QStringLiteral("控制台"));
    };

    window.ui->lblProcessStatus->setText(QStringLiteral("当前第 1/5 轮"));
    const QString roundStatus = window.ui->lblProcessStatus->text();
    window.updateProcessPanel(0.0, 0.0, 109, 100, 9, 0.0, true);
    QCOMPARE(window.ui->lblProcessStatus->text(), roundStatus);
    QCOMPARE(window.ui->barMeasureProgress->value(), 1);
    QVERIFY(!hasDeveloperWording(window.ui->lblProcessStatus->text()));

    window.lastFrameAngleSignedDiffOk = false;
    window.lastFrameAnglePairMidGapOk = true;
    window.updateProcessPanel(0.0, 0.0, 103, 100, 3, 8.0, false);
    QCOMPARE(window.ui->lblProcessStatus->text(), roundStatus);
    QVERIFY(!hasDeveloperWording(window.ui->lblProcessStatus->text()));

    window.updateProcessInvalid(QStringLiteral("B_pair 无效，无法作为参考"));
    QCOMPARE(window.ui->lblProcessStatus->text(), roundStatus);
    QVERIFY(!hasDeveloperWording(window.ui->lblProcessStatus->text()));

    window.showRoundFinishedTip(0, 5, false);
    QVERIFY(window.measureTipBox);
    QCOMPARE(window.measureTipBox->windowTitle(), QStringLiteral("本轮未计入"));
    QVERIFY(window.measureTipBox->text().contains(QStringLiteral("未达到要求")));
    QVERIFY(!window.measureTipBox->text().contains(QStringLiteral("测量完成")));
    window.closeRoundFinishedTip();
}

void MainWindowSafetyTests::measurementGuideHasThreeApprovedPagesAndPortableMarker()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString settingsPath =
        directory.filePath(QStringLiteral("measurement-guide.ini"));

    QVERIFY(!MeasurementGuideDialog::isCurrentVersionSeen(settingsPath));
    QString errorMessage;
    QVERIFY2(MeasurementGuideDialog::markCurrentVersionSeen(
                 settingsPath, &errorMessage),
             qPrintable(errorMessage));
    QVERIFY(MeasurementGuideDialog::isCurrentVersionSeen(settingsPath));

    MeasurementGuideDialog dialog(MeasurementGuideDialog::Mode::Automatic);
    dialog.setAttribute(Qt::WA_DontShowOnScreen, true);
    dialog.resize(900, 620);
    QStackedWidget* pages = dialog.findChild<QStackedWidget*>(
        QStringLiteral("measurementGuidePages"));
    QVERIFY(pages);
    QCOMPARE(pages->count(), 3);
    QVERIFY(pages->widget(0)->findChild<QLabel*>(
        QStringLiteral("guideBody"))->text().contains(
        QStringLiteral("右侧 D")));
    QVERIFY(pages->widget(1)->findChild<QLabel*>(
        QStringLiteral("guideHeading"))->text().contains(
        QStringLiteral("右侧 D")));
    QVERIFY(pages->widget(2)->findChild<QLabel*>(
        QStringLiteral("guideHeading"))->text().contains(
        QStringLiteral("左侧 G")));
    for (int index = 1; index <= 3; ++index) {
        QVERIFY(dialog.findChild<QWidget*>(
            QStringLiteral("measurementGuideIllustration%1").arg(index)));
    }

    QPushButton* next = dialog.findChild<QPushButton*>(
        QStringLiteral("guideNextButton"));
    QVERIFY(next);
    QCOMPARE(next->text(), QStringLiteral("下一步"));

    const QString captureDir = qEnvironmentVariable("BONE_UI_CAPTURE_DIR");
    const auto capture = [&dialog, &captureDir](const QString& name) {
        if (captureDir.isEmpty()) return;
        QVERIFY(QDir().mkpath(captureDir));
        dialog.show();
        QTest::qWait(50);
        QVERIFY2(dialog.grab().save(QDir(captureDir).filePath(name)),
                 qPrintable(name));
    };
    capture(QStringLiteral("measurement-guide-1.png"));
    next->click();
    capture(QStringLiteral("measurement-guide-2.png"));
    next->click();
    capture(QStringLiteral("measurement-guide-3.png"));
    QCOMPARE(pages->currentIndex(), 2);
    QCOMPARE(next->text(), QStringLiteral("知道了，开始检测"));
}

void MainWindowSafetyTests::measurementGuideFirstUseAndSpaceContinue()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());

    MainWindow window;
    window.setAttribute(Qt::WA_DontShowOnScreen, true);
    window.show();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.measurementGuideSettingsPath =
        directory.filePath(QStringLiteral("measurement-guide.ini"));
    window.updatePatientSelectionUi();

    QTimer::singleShot(0, []() {
        auto* dialog = qobject_cast<MeasurementGuideDialog*>(
            QApplication::activeModalWidget());
        QVERIFY(dialog);
        QPushButton* skip = dialog->findChild<QPushButton*>(
            QStringLiteral("guideSkipButton"));
        QVERIFY(skip);
        skip->click();
    });
    window.startPatientMeasurement(5, true);
    QVERIFY(window.patientMeasureRunning);
    QVERIFY(MeasurementGuideDialog::isCurrentVersionSeen(
        window.measurementGuideSettingsPath));
    window.stopPatientMeasurement();

    window.roundSosList = {4000.0};
    window.updatePatientSelectionUi();
    QVERIFY(window.ui->btnStartMeasurement->isEnabled());
    window.ui->btnStartMeasurement->setFocus(Qt::OtherFocusReason);
    QTest::keyClick(window.ui->btnStartMeasurement, Qt::Key_Space);
    QVERIFY(window.patientMeasureRunning);
    QCOMPARE(window.ui->lblProcessStatus->text(),
             QStringLiteral("当前第 2/5 轮"));
    window.stopPatientMeasurement();

    QFile::remove(window.measurementGuideSettingsPath);
    window.measurementGuideSeenThisRun = false;
    QTimer::singleShot(0, []() {
        if (auto* dialog = qobject_cast<MeasurementGuideDialog*>(
                QApplication::activeModalWidget())) {
            dialog->reject();
        }
    });
    window.startPatientMeasurement(5, true);
    QVERIFY(!window.patientMeasureRunning);
    QVERIFY(!QFileInfo::exists(window.measurementGuideSettingsPath));

    QTimer::singleShot(0, []() {
        if (auto* dialog = qobject_cast<MeasurementGuideDialog*>(
                QApplication::activeModalWidget())) {
            QCOMPARE(dialog->findChild<QPushButton*>(
                         QStringLiteral("guideSkipButton"))->text(),
                     QStringLiteral("关闭"));
            dialog->reject();
        }
    });
    window.on_btnMeasurementGuide_clicked();
    QVERIFY(!window.patientMeasureRunning);
    QVERIFY(!QFileInfo::exists(window.measurementGuideSettingsPath));
}

void MainWindowSafetyTests::fragmentedFrameReassemblesAtEveryByteBoundary()
{
    const QByteArray completeFrame = frame(42, 3);
    for (int split = 1; split < completeFrame.size(); ++split) {
        MainWindow window;
        window.hide();

        window.rxBuffer.append(completeFrame.left(split));
        window.parseIncomingData();
        QVERIFY2(!window.frameGroups.contains(42),
                 qPrintable(QStringLiteral("frame completed before byte %1 arrived").arg(split)));

        window.rxBuffer.append(completeFrame.mid(split));
        window.parseIncomingData();
        QVERIFY2(window.frameGroups.contains(42),
                 qPrintable(QStringLiteral("frame did not reassemble at split %1").arg(split)));
        const WaveGroup group = window.frameGroups.value(42);
        QVERIFY(group.has[2]);
        QCOMPARE(group.ch[2], QVector<quint16>({2048}));
        QVERIFY(window.rxBuffer.isEmpty());
    }
}

void MainWindowSafetyTests::parserResynchronizesAfterNoiseAndBadTail()
{
    MainWindow window;
    window.hide();

    QByteArray badTail = frame(10, 1);
    badTail[badTail.size() - 1] = char(0x00);
    QByteArray invalidLength = frame(12, 4).left(9);
    invalidLength[7] = char(0x00);
    invalidLength[8] = char(0x00);

    window.rxBuffer =
        QByteArray::fromHex("010203aa00ff")
        + badTail
        + invalidLength
        + frame(11, 2);
    window.parseIncomingData();

    QVERIFY(!window.frameGroups.contains(10));
    QVERIFY(!window.frameGroups.contains(12));
    QVERIFY(window.frameGroups.contains(11));
    const WaveGroup group = window.frameGroups.value(11);
    QVERIFY(group.has[1]);
    QCOMPARE(group.ch[1], QVector<quint16>({2048}));
    QVERIFY(window.rxBuffer.isEmpty());
}

void MainWindowSafetyTests::interleavedAndWrappedFrameIndexesStayIndependent()
{
    MainWindow window;
    window.hide();

    window.rxBuffer =
        frame(65535, 3)
        + frame(0, 2)
        + frame(65535, 1)
        + frame(0, 4);
    window.parseIncomingData();

    QCOMPARE(window.frameGroups.size(), 2);
    QVERIFY(window.frameGroups.value(65535).has[0]);
    QVERIFY(window.frameGroups.value(65535).has[2]);
    QVERIFY(!window.frameGroups.value(65535).has[1]);
    QVERIFY(!window.frameGroups.value(65535).has[3]);
    QVERIFY(!window.frameGroups.value(0).has[0]);
    QVERIFY(window.frameGroups.value(0).has[1]);
    QVERIFY(!window.frameGroups.value(0).has[2]);
    QVERIFY(window.frameGroups.value(0).has[3]);
    QCOMPARE(window.frameGroupOrder.size(), 2);
    QCOMPARE(window.frameGroupOrder.at(0), quint16(65535));
    QCOMPARE(window.frameGroupOrder.at(1), quint16(0));
    QVERIFY(window.rxBuffer.isEmpty());
}

void MainWindowSafetyTests::speedSeriesKeepsOnlyRecentPoints()
{
    MainWindow window;
    window.hide();
    for (int index = 0; index < 1000; ++index) {
        window.appendSpeedPoint(3000.0 + index);
    }
    QCOMPARE(window.seriesSpeed->count(), 50);
    QCOMPARE(window.seriesSpeed->at(0).x(), 950.0);
    QCOMPARE(window.seriesSpeed->at(49).x(), 999.0);
}

void MainWindowSafetyTests::disconnectedControlsAndPlaceholdersAreSafe()
{
    MainWindow window;
    window.hide();
    QVERIFY(!window.serial->isOpen());
    QVERIFY(!window.ui->pushButton->isEnabled());
    QVERIFY(!window.ui->triggerButton->isEnabled());
    QVERIFY(!window.ui->btnStartMeasurement->isEnabled());
    QVERIFY(!window.ui->wifi_button->isEnabled());
    QCOMPARE(window.ui->grpWaveArea->title(), QStringLiteral("四通道波形"));
    QCOMPARE(window.ui->grpSpeedArea->title(), QStringLiteral("声速趋势"));
    QCOMPARE(window.ui->grpProcessArea->title(), QStringLiteral("检测过程"));
    QCOMPARE(window.ui->grpPartImageRight->title(), QStringLiteral("测量部位"));
    QCOMPARE(window.ui->dateBirth->maximumDate(), QDate::currentDate());
    QCOMPARE(window.ui->dBirth->maximumDate(), QDate::currentDate());
    QCOMPARE(window.ui->eBirth->maximumDate(), QDate::currentDate());
}

void MainWindowSafetyTests::patientMeasurementDisablesConflictingControls()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    window.updatePatientSelectionUi();

    QVERIFY(window.ui->btnStartMeasurement->isEnabled());
    QCOMPARE(window.ui->btnStartMeasurement->text(), QStringLiteral("停止检测"));
    QVERIFY(!window.ui->pushButton->isEnabled());
    QVERIFY(!window.ui->triggerButton->isEnabled());
    QVERIFY(!window.ui->btnReport->isEnabled());
    QVERIFY(!window.ui->pushButton_2->isEnabled());
    QVERIFY(!window.ui->btnArchive->isEnabled());
    QVERIFY(!window.ui->btnMeasurementGuide->isEnabled());
    QVERIFY(!window.gainSliderA->isEnabled());
    QVERIFY(!window.gainSliderB->isEnabled());
    QVERIFY(!window.gainSliderC->isEnabled());
    QVERIFY(!window.gainSliderD->isEnabled());
}

void MainWindowSafetyTests::debugAutoDisablesConflictingNavigation()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.autoRunning = true;
    window.autoTimer->start(80);
    window.updatePatientSelectionUi();

    QVERIFY(!window.ui->pushButton->isEnabled());
    QVERIFY(window.ui->triggerButton->isEnabled());
    QVERIFY(!window.ui->btnPatientInfo->isEnabled());
    QVERIFY(!window.ui->btnReport->isEnabled());
    QVERIFY(!window.ui->btnArchive->isEnabled());
    QVERIFY(!window.ui->btnMeasurementGuide->isEnabled());
}

void MainWindowSafetyTests::patientFormsStayInsideAndCenteredAtSmallWindow()
{
    MainWindow window;
    window.setAttribute(Qt::WA_DontShowOnScreen, true);
    window.showNormal();
    window.resize(1200, 760);
    QTest::qWait(50);

    const auto verifyPage = [&window](QWidget* page, const QList<QWidget*>& controls) {
        window.ui->stackedWidget->setCurrentWidget(page);
        QApplication::processEvents();
        if (page->layout()) page->layout()->activate();
        QApplication::processEvents();

        int left = page->width();
        int right = 0;
        for (QWidget* control : controls) {
            QVERIFY(control);
            QVERIFY(!control->isHidden());
            const QRect bounds(control->mapTo(page, QPoint(0, 0)), control->size());
            QVERIFY2(page->rect().contains(bounds),
                     qPrintable(QStringLiteral("%1 [%2,%3 %4x%5] escaped %6 [%7,%8 %9x%10]")
                                    .arg(control->objectName())
                                    .arg(bounds.x())
                                    .arg(bounds.y())
                                    .arg(bounds.width())
                                    .arg(bounds.height())
                                    .arg(page->objectName())
                                    .arg(page->rect().x())
                                    .arg(page->rect().y())
                                    .arg(page->rect().width())
                                    .arg(page->rect().height())));
            left = qMin(left, bounds.left());
            right = qMax(right, bounds.right());
        }
        const int contentCenter = (left + right) / 2;
        QVERIFY2(qAbs(contentCenter - page->rect().center().x()) <= 80,
                 qPrintable(QStringLiteral("%1 content is visibly off-center")
                                .arg(page->objectName())));
    };

    verifyPage(window.ui->pagePatientForm,
               {window.ui->label, window.ui->editName,
                window.ui->label_18, window.ui->editID,
                window.ui->label_2, window.ui->comboGender,
                window.ui->label_3, window.ui->dateBirth,
                window.ui->label_5, window.ui->editHeight,
                window.ui->label_6, window.ui->editWeight,
                window.ui->btnFormSave, window.ui->btnFormBack});
    verifyPage(window.ui->pagePatientDetail,
               {window.ui->label_16, window.ui->dName,
                window.ui->label_13, window.ui->dID,
                window.ui->label_14, window.ui->dGender,
                window.ui->label_17, window.ui->dBirth,
                window.ui->label_12, window.ui->dHeight,
                window.ui->label_10, window.ui->dWeight,
                window.ui->btnDetailSave, window.ui->btnDetailDelete,
                window.ui->btnDetailBack});
}

void MainWindowSafetyTests::pendingResultBlocksAnotherMeasurement()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;

    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.hasPendingMeasurement = true;
    window.pendingMeasurement = sampleMeasurement();
    window.roundSosList = {4001.0, 4002.0, 4003.0, 4004.0, 4005.0};
    const QList<double> roundsBefore = window.roundSosList;

    window.updatePatientSelectionUi();
    QVERIFY(!window.ui->btnStartMeasurement->isEnabled());
    window.on_btnStartMeasurement_clicked();

    QVERIFY(!window.patientMeasureRunning);
    QVERIFY(window.hasPendingMeasurement);
    QCOMPARE(window.pendingMeasurement.id, QStringLiteral("measurement-001"));
    QCOMPARE(window.roundSosList, roundsBefore);
}

void MainWindowSafetyTests::samePatientReselectionPreservesPendingResult()
{
    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    window.currentPatient = patient;
    window.hasPendingMeasurement = true;
    window.pendingMeasurement = sampleMeasurement();
    window.roundSosList = {4001.0, 4002.0};

    window.selectCurrentPatient(patient);

    QVERIFY(window.hasPendingMeasurement);
    QCOMPARE(window.pendingMeasurement.id, QStringLiteral("measurement-001"));
    QCOMPARE(window.roundSosList, QList<double>({4001.0, 4002.0}));
}

void MainWindowSafetyTests::samePatientReselectionPreservesPartialRounds()
{
    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    window.currentPatient = patient;
    window.roundSosList = {4001.0, 4002.0};
    window.roundAList = {3991.0, 3992.0};
    window.roundBList = {4011.0, 4012.0};

    QVERIFY(window.selectCurrentPatient(patient));

    QCOMPARE(window.currentPatient.id, patient.id);
    QCOMPARE(window.roundSosList, QList<double>({4001.0, 4002.0}));
    QCOMPARE(window.roundAList, QList<double>({3991.0, 3992.0}));
    QCOMPARE(window.roundBList, QList<double>({4011.0, 4012.0}));
}

void MainWindowSafetyTests::cancelledPatientSwitchPreservesPartialRounds()
{
    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    PatientInfo otherPatient = patient;
    otherPatient.id = QStringLiteral("patient-002");
    otherPatient.name = QStringLiteral("其他患者");
    window.currentPatient = patient;
    window.roundSosList = {4001.0, 4002.0};

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->done(QMessageBox::No);
        }
    });
    closer.start(0);
    QVERIFY(!window.selectCurrentPatient(otherPatient));
    closer.stop();

    QCOMPARE(window.currentPatient.id, patient.id);
    QCOMPARE(window.roundSosList, QList<double>({4001.0, 4002.0}));
}

void MainWindowSafetyTests::cancelledQuickPatientCreationDoesNotWritePatient()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());

    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    window.patientList = {patient};
    window.currentPatient = patient;
    window.patientDataWritable = true;
    window.xmlFilePath = directory.filePath(QStringLiteral("patients.xml"));
    window.measurementsFilePath = directory.filePath(QStringLiteral("measurements.xml"));
    window.hasPendingMeasurement = true;
    window.pendingMeasurement = sampleMeasurement();
    window.ui->eName->setText(QStringLiteral("新患者"));
    window.ui->eID->setText(QStringLiteral("patient-new"));
    window.ui->eGender->setCurrentText(QStringLiteral("男"));
    window.ui->eBirth->setDate(QDate(2000, 1, 1));
    window.ui->eHeight->setText(QStringLiteral("170"));
    window.ui->eWeight->setText(QStringLiteral("60"));

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->done(QMessageBox::No);
        }
    });
    closer.start(0);
    window.on_btnPatientNewSave_clicked();
    closer.stop();

    QCOMPARE(window.patientList.size(), 1);
    QCOMPARE(window.currentPatient.id, patient.id);
    QVERIFY(window.hasPendingMeasurement);
    QVERIFY(!QFileInfo::exists(window.xmlFilePath));
}

void MainWindowSafetyTests::partialRoundsRequireCloseConfirmation()
{
    MainWindow window;
    window.hide();
    window.currentPatient = samplePatient();
    window.roundSosList = {4001.0, 4002.0};

    QCloseEvent event;
    event.ignore();
    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->done(QMessageBox::Cancel);
        }
    });
    closer.start(0);
    window.closeEvent(&event);
    closer.stop();

    QVERIFY(!event.isAccepted());
    QCOMPARE(window.roundSosList, QList<double>({4001.0, 4002.0}));
}

void MainWindowSafetyTests::activeFirstRoundClosePausesAndCanResume()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    window.currentRoundSosList = {4001.0, 4002.0};
    window.autoTimer->start(80);
    window.updatePatientSelectionUi();

    QCloseEvent event;
    event.ignore();
    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            if (QAbstractButton* button = box->button(QMessageBox::Cancel)) {
                button->click();
            }
        }
    });
    closer.start(0);
    window.closeEvent(&event);
    closer.stop();

    QVERIFY(!event.isAccepted());
    QVERIFY(window.patientMeasureRunning);
    QCOMPARE(window.acquireMode, PatientMeasureMode);
    QVERIFY(window.autoTimer->isActive());
    QCOMPARE(window.currentRoundSosList, QList<double>({4001.0, 4002.0}));
}

void MainWindowSafetyTests::partialRoundsBlockCalibrationDialog()
{
    MainWindow window;
    window.hide();
    window.currentPatient = samplePatient();
    window.roundSosList = {4001.0, 4002.0};
    const double activeDBefore = window.signalProcessor.probeDistanceCD;
    bool calibrationDialogOpened = false;

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, [&calibrationDialogOpened]() {
        QWidget* modal = QApplication::activeModalWidget();
        if (auto* dialog = qobject_cast<CalibrationDialog*>(modal)) {
            calibrationDialogOpened = true;
            dialog->close();
        } else if (auto* box = qobject_cast<QMessageBox*>(modal)) {
            box->accept();
        }
    });
    closer.start(0);
    window.openCalibrationDialog();
    closer.stop();

    QVERIFY(!calibrationDialogOpened);
    QCOMPARE(window.signalProcessor.probeDistanceCD, activeDBefore);
    QCOMPARE(window.roundSosList, QList<double>({4001.0, 4002.0}));
}

void MainWindowSafetyTests::futureBirthDateBlocksMeasurement()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.currentPatient.birthDay =
        QDate::currentDate().addDays(1).toString(QStringLiteral("yyyy-MM-dd"));

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            if (QAbstractButton* button = box->button(QMessageBox::Ok)) {
                button->click();
            }
        }
    });
    closer.start(0);
    window.startPatientMeasurement(5);
    closer.stop();

    QVERIFY(!window.patientMeasureRunning);
    QVERIFY(!window.autoTimer->isActive());
    QCOMPARE(window.acquireMode, DebugAcquireMode);
}

void MainWindowSafetyTests::pendingSaveValidatesTheRecordedPatient()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());

    MainWindow window;
    window.hide();
    PatientInfo otherPatient = samplePatient();
    otherPatient.id = QStringLiteral("patient-002");
    otherPatient.name = QStringLiteral("其他患者");
    window.patientList = {otherPatient};
    window.currentPatient = otherPatient;
    window.patientDataWritable = true;
    window.measurementsFilePath = directory.filePath(QStringLiteral("measurements.xml"));
    window.hasPendingMeasurement = true;
    window.pendingMeasurement = sampleMeasurement();

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->accept();
        }
    });
    closer.start(0);
    QVERIFY(!window.trySavePendingMeasurement());
    closer.stop();
    QVERIFY(window.hasPendingMeasurement);
    QVERIFY(window.measurementList.isEmpty());
    QVERIFY(!QFileInfo::exists(window.measurementsFilePath));
}

void MainWindowSafetyTests::pendingTransactionBlocksSingleFileWrites()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());

    MainWindow window;
    window.hide();
    window.patientDataWritable = true;
    window.xmlFilePath = directory.filePath(QStringLiteral("patients.xml"));
    window.measurementsFilePath = directory.filePath(QStringLiteral("measurements.xml"));
    QFile marker(window.xmlFilePath + QStringLiteral(".txn"));
    QVERIFY(marker.open(QIODevice::WriteOnly));
    QCOMPARE(marker.write("<invalid-marker/>"), qint64(17));
    marker.close();

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->accept();
        }
    });
    closer.start(0);
    QVERIFY(!window.saveMeasurements({sampleMeasurement()}));
    closer.stop();

    QVERIFY(!window.patientDataWritable);
    QVERIFY(!QFileInfo::exists(window.measurementsFilePath));
}

void MainWindowSafetyTests::patientMeasurementStartClearsSerialAssembly()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.currentPatient = samplePatient();
    window.rxBuffer = QByteArray::fromHex("aa55");
    window.frameGroups.insert(7, WaveGroup());
    window.frameGroupOrder.enqueue(7);
    window.samplesA = {1, 2, 3};
    window.samplesB = {4, 5, 6};
    window.chReceived[0] = true;
    window.chReceived[1] = true;

    window.startPatientMeasurement(5);

    QVERIFY(window.patientMeasureRunning);
#ifndef QT_NO_DEBUG
    QVERIFY2(window.experimentLog.active(), qPrintable(window.experimentLog.error()));
    const QString logPath = window.experimentLog.path();
    QVERIFY(logPath.startsWith(QCoreApplication::applicationDirPath()));
#else
    QVERIFY(!window.experimentLog.active());
#endif
    QVERIFY(window.rxBuffer.isEmpty());
    QVERIFY(window.frameGroups.isEmpty());
    QVERIFY(window.frameGroupOrder.isEmpty());
    QVERIFY(window.samplesA.isEmpty());
    QVERIFY(window.samplesB.isEmpty());
    for (bool received : window.chReceived) QVERIFY(!received);
    window.stopPatientMeasurement();
#ifndef QT_NO_DEBUG
    QFile log(logPath);
    QVERIFY(log.open(QIODevice::ReadOnly));
    const auto config = QJsonDocument::fromJson(log.readLine()).object().value("config").toObject();
    QCOMPARE(config.value("D_min").toDouble(), window.mCfg.angleSignedDiffMin);
    QCOMPARE(config.value("G_max").toDouble(), window.mCfg.anglePairMidGapMax);
    QVERIFY(!config.contains("patient"));
#endif
}

void MainWindowSafetyTests::serialIoErrorsResetAcquisition_data()
{
    QTest::addColumn<int>("error");
    QTest::newRow("resource-error") << int(QSerialPort::ResourceError);
    QTest::newRow("read-error") << int(QSerialPort::ReadError);
    QTest::newRow("write-error") << int(QSerialPort::WriteError);
}

void MainWindowSafetyTests::serialIoErrorsResetAcquisition()
{
    QFETCH(int, error);
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientMeasureRunning = true;
    window.autoRunning = true;
    window.acquireMode = PatientMeasureMode;
    window.rxBuffer = QByteArray::fromHex("aa55");
    window.frameGroups.insert(9, WaveGroup());
    window.frameGroupOrder.enqueue(9);
    window.autoTimer->start(80);

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->accept();
        }
    });
    closer.start(0);
    window.handleSerialError(static_cast<QSerialPort::SerialPortError>(error));
    closer.stop();

    QVERIFY(window.serialErrorHandled);
    QVERIFY(!window.serial->isOpen());
    QVERIFY(!window.patientMeasureRunning);
    QVERIFY(!window.autoRunning);
    QVERIFY(!window.autoTimer->isActive());
    QVERIFY(window.rxBuffer.isEmpty());
    QVERIFY(window.frameGroups.isEmpty());
    QVERIFY(window.frameGroupOrder.isEmpty());
}

void MainWindowSafetyTests::reportRenderingIsReadableAndArtifactFree()
{
    ReportData data;
    data.patientName = QStringLiteral("【测试】二十岁女性");
    data.patientId = QStringLiteral("W20");
    data.age = QStringLiteral("20");
    data.gender = QStringLiteral("女");
    data.birthDay = QStringLiteral("2006-01-01");
    data.measuredAt = QStringLiteral("2026-07-17 08:10");
    data.height = QStringLiteral("166 cm");
    data.weight = QStringLiteral("54 kg");
    data.part = QStringLiteral("桡骨");
    data.sos = QStringLiteral("4200.0 m/s");
    data.boneStrength = QStringLiteral("测试数据");
    data.diagnosis = QStringLiteral("仅用于年龄-SOS参考图演示，不提供诊断结论");
    data.operatorName = QStringLiteral("测试");

    ReportWidget report;
    report.setReportData(data);
    QImage image(QSize(795, 1124), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    report.renderReport(&painter, QRectF(QPointF(0, 0), image.size()));
    painter.end();

    QCOMPARE(image.pixelColor(173, 825), QColor(Qt::white));
    QVERIFY(image.pixelColor(794, 500).red() > 200);
    QCOMPARE(report.reportData().tScore, QString());
    QCOMPARE(report.reportData().zScore, QString());

    const QString captureDir = qEnvironmentVariable("BONE_UI_CAPTURE_DIR");
    if (!captureDir.isEmpty()) {
        QVERIFY(QDir().mkpath(captureDir));
        QVERIFY(image.save(QDir(captureDir).filePath(QStringLiteral("report.png"))));
    }
}

void MainWindowSafetyTests::invalidMeasurementDateDoesNotInventAge()
{
    MainWindow window;
    window.hide();
    PatientInfo patient = samplePatient();
    MeasurementRecord measurement = sampleMeasurement();
    measurement.patientAge.clear();
    measurement.patientBirthDay = patient.birthDay;
    measurement.measuredAt = QStringLiteral("invalid-date");

    const ReportData data = window.buildReportData(patient, measurement);

    QVERIFY(data.age.isEmpty());
    QCOMPARE(data.measuredAt, QStringLiteral("invalid-date"));
}

void MainWindowSafetyTests::completedReportUsesProvidedMeasurement()
{
    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    window.currentPatient = patient;

    MeasurementRecord futureRecord = sampleMeasurement();
    futureRecord.id = QStringLiteral("future-record");
    futureRecord.sos = QStringLiteral("9999.0");
    futureRecord.measuredAt = QStringLiteral("2099-01-01T00:00:00");
    window.measurementList = {futureRecord};

    MeasurementRecord completed = sampleMeasurement();
    completed.id = QStringLiteral("just-completed");
    completed.sos = QStringLiteral("4000.0");
    completed.measuredAt = QStringLiteral("2026-07-26T10:00:00");
    window.showPatientMeasureFinishedDialog(completed);

    QCOMPARE(window.reportWidget->reportData().sos, QStringLiteral("4000.0 m/s"));
    QCOMPARE(window.reportWidget->reportData().measuredAt,
             QStringLiteral("2026-07-26 10:00"));
}

void MainWindowSafetyTests::reportPdfCanBeCommitted()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString pdfPath = directory.filePath(QStringLiteral("report.pdf"));

    MainWindow window;
    window.hide();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(pdfPath);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setPageMargins(QMarginsF(8, 8, 8, 8), QPageLayout::Millimeter);

    QVERIFY(window.renderReportToPrinter(&printer));
    QFile pdf(pdfPath);
    QVERIFY(pdf.open(QIODevice::ReadOnly));
    QVERIFY(pdf.size() > 1000);
    QCOMPARE(pdf.read(4), QByteArray("%PDF"));
}

void MainWindowSafetyTests::capturePagesWhenRequested()
{
    const QString captureDir = qEnvironmentVariable("BONE_UI_CAPTURE_DIR");
    if (captureDir.isEmpty()) QSKIP("BONE_UI_CAPTURE_DIR is not set");
    QVERIFY(QDir().mkpath(captureDir));

    MainWindow window;
    window.showNormal();
    window.resize(1920, 1080);
    QTest::qWait(100);

    const auto capture = [&window, &captureDir](QWidget* page, const QString& name) {
        window.ui->stackedWidget->setCurrentWidget(page);
        window.scheduleResponsiveLayout();
        QTest::qWait(100);
        QVERIFY2(window.grab().save(QDir(captureDir).filePath(name)), qPrintable(name));
    };

    capture(window.ui->pageMain, QStringLiteral("main.png"));
    capture(window.ui->pageArchive, QStringLiteral("archive.png"));
    capture(window.ui->pagePatientForm, QStringLiteral("patient-form.png"));
    capture(window.ui->pagePatientDetail, QStringLiteral("patient-detail.png"));
}

QTEST_MAIN(MainWindowSafetyTests)
#include "mainwindow_safety_tests.moc"
