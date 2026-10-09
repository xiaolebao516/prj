#include <QtTest>

#include "mainwindow.h"
#include "utils.h"
#include "calibrationdialog.h"
#include "measurementguidedialog.h"
#include "reportwidget.h"
#include "patientformdialog.h"
#include "patientstore.h"
#include "legacyimport.h"
#include "parametergroup.h"
#include "datalocation.h"
#include "databackup.h"
#include "sosreference.h"
#include "bonehealth.h"
#include "agesoschartwidget.h"
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
#include <QGroupBox>
#include <QMenu>
#include <QToolButton>
#include <QStyleOption>
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

    QTemporaryDir dataRoot;

private slots:
    // Every test runs against a throw-away data folder, never app/BoneDensityData.
    void initTestCase() { QVERIFY(dataRoot.isValid()); qputenv("BONE_DATA_DIR", dataRoot.path().toUtf8()); }
    void experimentSubjectSnapshotAndSessions();
    void onsetConsistencyBoundaries();
    void onsetGuardRejectsRecordedLowFrame();
    void onsetGuardMatchesRecordedBatch();
    void clippedBPeakSearchCanBeCompletedWithoutFullRangeScan();
    void partialRoundWaitsForRelockBeforeDiscarding();
    void finalResultUsesExactlyFiveRecordedRounds();
    void finalRoundSelectionPreservesCompanionsAndBoundaries();
    void finalRoundSelectionRecordsOriginalPool();
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
    void liveChartRenderingRejectsUnsafeInputs();
    void liveChartPaintStress();
    void disconnectedControlsAndPlaceholdersAreSafe();
    void positionGuideTracksExistingBarsWithoutChangingThem();
    void corrAFeedbackBindingAndResponsiveLayout();
    void measurementStatusIsVisibleAndOperatorFacing();
    void measurementGuideHasThreeApprovedPagesAndPortableMarker();
    void measurementGuideFirstUseAndSpaceContinue();
    void automaticNextRoundIsGuardedAndCancelable();
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
    void ageSosHistoryUsesStoredAgeProfileAndReportCutoff();
    void mainAgeSosChartHighlightsLatestValidMeasurement();
    void invalidMeasurementDateDoesNotInventAge();
    void completedReportUsesProvidedMeasurement();
    void reportPdfCanBeCommitted();
    void patientDialogStartsWithoutDefaults();
    void patientDialogEditKeepsIdReadOnly();
    void suggestedPatientIdIsUnique();
    void selectingPatientShowsLatestSavedResult();
    void retrySaveRefreshesResultArchiveAndChart();
    void deletingRecordRefreshesViews();
    void editingCurrentPatientRefreshesViews();
    void enterSubmitsLogin();
    void closeCancelKeepsPendingNextRound();
    void batchDeleteConfirmationListsNamesAndCounts();
    void archiveDoubleClickSetsCurrentPatient();
    void toolbarAndResultCardStructure();
    void accountDialogFillsWidthAndStaysOpen();
    void themedDialogGroupTitlesStayClearOfContent();
    void loginPageIsUsableAndShowsVersion();
    void portListRefreshKeepsSelectionWithoutRebuilding();
    void deviceWatchdogReportsMissingFrames();
    void sosReferenceMatchesPublishedTable();
    void patientResultUsesReferenceAndSkipsChildren();
    void adultAgeSosChartIsDrawnFromReference();
    void adultChartPointsMatchComputedScores();
    void savedRecordsAreShownWithCurrentReference();
    void boneAgeAndChildChartStayConsistent();
    void mainLayoutFitsCommonWindowSizes();
    void dataBackupSnapshotsOnceAndPrunes();
    void deletionsAreBackedUpFirst();
    void accountMenuSwitchesOperatorSafely();
    void serialPortSelectionIsRemembered();
    void measurementCsvMatchesScreenValues();
    void archiveActionBarKeepsButtonLabels();
    void dataFolderIsSharedByEveryBuild();
    void parameterGroupsDescribeTheirData();
    void resultsRecordTheirParameterGroup();
    void legacyDataIsMergedWithoutMixingPeople();
    void capturePagesWhenRequested();
};

void MainWindowSafetyTests::clippedBPeakSearchCanBeCompletedWithoutFullRangeScan()
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

void MainWindowSafetyTests::partialRoundWaitsForRelockBeforeDiscarding()
{
    const auto seedPartial=[](MainWindow& window) {
        window.patientMeasureRunning=true;
        window.acquireMode=PatientMeasureMode;
        window.deferPartialDiscardUntilRelock=true;
        for(int i=0;i<window.mCfg.stableLagWarmupCount;++i)
            window.checkBoneLagStable(128);
        QVERIFY(window.boneLagLocked);
        for(int i=0;i<5;++i) {
            window.currentRoundSosList.append(3900+i);
            window.currentRoundAList.append(3800+i);
            window.currentRoundBList.append(3900+i);
            window.currentRoundCorrAList.append(.9);
            window.currentRoundCorrBList.append(.9);
            window.currentRoundPairMidGapList.append(0);
            window.currentRoundSignedLagDiffList.append(9);
        }
        window.processValidCount=5;
    };
    MainWindow same;
    seedPartial(same);
    for(int i=0;i<same.mCfg.boneLagUnlockCount;++i)same.rejectBoneLagCandidate();
    QVERIFY(!same.boneLagLocked);
    QCOMPARE(same.currentRoundSosList.size(),5);
    for(int i=0;i<same.mCfg.stableLagWarmupCount;++i)same.checkBoneLagStable(128);
    QVERIFY(same.boneLagLocked);
    QCOMPARE(same.currentRoundSosList.size(),5);

    MainWindow moved;
    seedPartial(moved);
    for(int i=0;i<moved.mCfg.boneLagUnlockCount;++i)moved.rejectBoneLagCandidate();
    QCOMPARE(moved.currentRoundSosList.size(),5);
    for(int i=0;i<moved.mCfg.stableLagWarmupCount;++i)moved.checkBoneLagStable(150);
    QVERIFY(moved.boneLagLocked);
    QVERIFY(moved.currentRoundSosList.isEmpty());
    QCOMPARE(moved.processValidCount,0);
}

void MainWindowSafetyTests::finalResultUsesExactlyFiveRecordedRounds()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    window.hide();
    window.xmlFilePath = directory.filePath("patients.xml");
    window.measurementsFilePath = directory.filePath("measurements.xml");
    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.patientList = {window.currentPatient};
    window.measurementList.clear();
    window.roundSosList = {3910.048533906161, 3956.9673922545667,
        3946.3440860215032, 3951.612903225805, 4152.542372881357, 3983.7398373983733};
    window.roundAList = {10,20,30,40,5000,60};
    window.roundBList = {100,200,300,400,50000,600};
    const auto original = window.roundSosList;
    window.finishAllPatientRounds();
    QCOMPARE(window.currentPatient.speedOfSound, QStringLiteral("3949.7"));
    QCOMPARE(window.measurementList.size(), 1);
    QCOMPARE(window.measurementList.last().sos, QStringLiteral("3949.7"));
    QCOMPARE(window.roundSosList, original);
}

void MainWindowSafetyTests::finalRoundSelectionPreservesCompanionsAndBoundaries()
{
    MainWindow window;
    window.hide();
    window.roundSosList = {3910,3957,3946,3952,4153,3984};
    window.roundAList = {10,20,30,40,5000,60};
    window.roundBList = {100,200,300,400,50000,600};
    const auto original = window.roundSosList;
    double sos=-1, a=-1, b=-1;
    QVector<int> selected;
    QVERIFY(window.computeFinalPatientRoundMeans(sos,a,b,&selected));
    QCOMPARE(selected, QVector<int>({0,1,2,3,5}));
    QCOMPARE(sos,3949.8);
    QCOMPARE(a,32.0);
    QCOMPARE(b,320.0);
    QCOMPARE(window.roundSosList,original);
    window.roundSosList = {3984,3910,3946,3957,3952};
    window.roundAList = {10,20,30,40,60};
    window.roundBList = {100,200,300,400,600};
    QVERIFY(window.computeFinalPatientRoundMeans(sos,a,b,&selected));
    QCOMPARE(selected,QVector<int>({0,1,2,3,4}));
    QCOMPARE(sos,Utils::trimmedMeanValue(window.roundSosList,0.2));
    QCOMPARE(a,Utils::trimmedMeanValue(window.roundAList,0.2));
    QCOMPARE(b,Utils::trimmedMeanValue(window.roundBList,0.2));
    QCOMPARE(MainWindow::selectFinalRoundIndices({1,2,3,4,5,6},5),QVector<int>({0,1,2,3,4}));
    QCOMPARE(MainWindow::selectFinalRoundIndices({5,5,5,5,5,5},5),QVector<int>({0,1,2,3,4}));
    QVERIFY(MainWindow::selectFinalRoundIndices({1,2,3,4},5).isEmpty());
    QVERIFY(MainWindow::selectFinalRoundIndices({1,2,3,4,5},0).isEmpty());
    QVERIFY(MainWindow::selectFinalRoundIndices({1,2,3,4,std::numeric_limits<double>::quiet_NaN()},5).isEmpty());
    window.roundAList.removeLast();
    sos=a=b=-1;
    QVERIFY(!window.computeFinalPatientRoundMeans(sos,a,b));
    QCOMPARE(sos,-1.0);
    QCOMPARE(a,-1.0);
    QCOMPARE(b,-1.0);
}

void MainWindowSafetyTests::finalRoundSelectionRecordsOriginalPool()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    window.hide();
    window.xmlFilePath=directory.filePath("patients.xml");
    window.measurementsFilePath=directory.filePath("measurements.xml");
    window.patientDataWritable=true;
    window.currentPatient=samplePatient();
    window.patientList={window.currentPatient};
    window.measurementList.clear();
    for(double sos : {3910.,3957.,3946.,3952.,4153.})
        window.candidateRoundList.append(RoundCandidate{sos,sos-100,sos,.95,.95});
    Utils::rebuildAcceptedRoundsFromCandidates(window.candidateRoundList,window.roundSosList,
        window.roundAList,window.roundBList,window.roundClusterTolerance);
    QCOMPARE(window.roundSosList.size(),4);
    QVERIFY(window.experimentLog.start(directory.filePath("logs"),{}));
    const QString path=window.experimentLog.path();
    window.patientMeasureRunning=true;
    window.acquireMode=PatientMeasureMode;
    for(int i=0;i<30;++i)
        window.handlePatientMeasureValue(3884,3984,3984,132,123,9,0,.95,.95,true);
    QVERIFY(!window.patientMeasureRunning);
    QCOMPARE(window.candidateRoundList.size(),6);
    QCOMPARE(window.roundSosList.size(),6);
    QCOMPARE(window.measurementList.size(),1);
    QCOMPARE(window.measurementList.last().sos,QStringLiteral("3949.8"));
    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly));
    int found=0;
    while(!file.atEnd()) {
        const auto row=QJsonDocument::fromJson(file.readLine()).object();
        if(row["event"]!="final_round_selection")continue;
        ++found;
        QCOMPARE(row["source_accepted_sos"].toArray(),QJsonArray({3910,3957,3946,3952,4153,3984}));
        QCOMPARE(row["selected_indices"].toArray(),QJsonArray({0,1,2,3,5}));
        QCOMPARE(row["final_sos"].toDouble(),3949.8);
        QCOMPARE(row["final_A"].toDouble(),3849.8);
        QCOMPARE(row["final_B"].toDouble(),3949.8);
    }
    QCOMPARE(found,1);
}

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
    // Isolate the pre-existing stability flow from the new onset rejection.
    // This zero-noise synthetic packet has an artificially early threshold hit.
    probe.enforceBOnsetConsistency=false;
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
        // This regression isolates the original/observe-before-G flows.
        window.enforceBOnsetConsistency=false;
        window.deferPartialDiscardUntilRelock=false;
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
#ifdef BONE_COMPLETE_B_PEAK_EXPERIMENT
    QVERIFY(window.useDualWindowAQuality);
    QVERIFY(window.observeStabilityBeforeG);
    QVERIFY(window.completeTruncatedBPeak);
    QCOMPARE(window.mCfg.roundCorrAMin,.78);
    QVERIFY(window.windowTitle().contains(QStringLiteral("B峰补全试测版")));
#elif defined(BONE_RELOCK_PRESERVATION_EXPERIMENT)
    QVERIFY(window.useDualWindowAQuality);
    QVERIFY(window.observeStabilityBeforeG);
    QVERIFY(!window.completeTruncatedBPeak);
    QVERIFY(window.deferPartialDiscardUntilRelock);
    QCOMPARE(window.partialRelockRetentionTolerance,2);
    QCOMPARE(window.mCfg.roundCorrAMin,.78);
    QVERIFY(window.windowTitle().contains(QStringLiteral("稳定簇续接试测版")));
#elif defined(BONE_DUAL_WINDOW_A_EXPERIMENT)
    QVERIFY(window.useDualWindowAQuality);
    QVERIFY(window.observeStabilityBeforeG);
    QVERIFY(!window.completeTruncatedBPeak);
    QVERIFY(!window.deferPartialDiscardUntilRelock);
    QCOMPARE(window.mCfg.roundCorrAMin,.78);
    QVERIFY(window.windowTitle().contains(QStringLiteral("双段评分试测版")));
#elif defined(BONE_OBSERVE_BEFORE_G_EXPERIMENT)
    QVERIFY(!window.useDualWindowAQuality);
    QVERIFY(!window.completeTruncatedBPeak);
    QVERIFY(!window.deferPartialDiscardUntilRelock);
    QCOMPARE(window.mCfg.roundCorrAMin,.80);
    QVERIFY(window.observeStabilityBeforeG);
    QVERIFY(window.windowTitle().contains(QStringLiteral("试测版")));
#else
    QVERIFY(window.useDualWindowAQuality);
    QVERIFY(!window.completeTruncatedBPeak);
    QVERIFY(window.deferPartialDiscardUntilRelock);
    QCOMPARE(window.partialRelockRetentionTolerance,2);
    QCOMPARE(window.mCfg.roundCorrAMin,.78);
    QCOMPARE(window.mCfg.anglePairMidGapMin,-12.0);
    QCOMPARE(window.mCfg.anglePairMidGapMax,0.0);
    QVERIFY(window.observeStabilityBeforeG);
    QCOMPARE(window.windowTitle(),QStringLiteral("超声骨密度仪"));
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
    QCOMPARE(config.value("B_clipped_peak_extension").toInt(),window.completeTruncatedBPeak ? 15 : 0);
    QCOMPARE(config.value("partial_relock_retention_lag").toInt(),
             window.deferPartialDiscardUntilRelock ? 2 : 0);
#ifdef BONE_COMPLETE_B_PEAK_EXPERIMENT
    const QString expectedImplementation=QStringLiteral("b-peak-completion-20260908-v1");
#elif defined(BONE_RELOCK_PRESERVATION_EXPERIMENT)
    const QString expectedImplementation=QStringLiteral("relock-preservation-20260908-v1");
#elif defined(BONE_DUAL_WINDOW_A_EXPERIMENT)
    const QString expectedImplementation=QStringLiteral("dual-window-a078-20260906-v1");
#elif defined(BONE_OBSERVE_BEFORE_G_EXPERIMENT)
    const QString expectedImplementation=QStringLiteral("observe-before-g-20260906-v1");
#else
    const QString expectedImplementation=QStringLiteral("onset-consistency-20260915-v1");
#endif
    QCOMPARE(config.value("implementation").toString(),expectedImplementation);
    QCOMPARE(config.value("B_onset_forward_limit").toInt(),window.enforceBOnsetConsistency ? 40 : 0);
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
    // The separate relock test covers the experimental deferred-discard path.
    window.deferPartialDiscardUntilRelock=false;
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
    // Keep this as the default immediate-expiry regression in every build profile.
    window.deferPartialDiscardUntilRelock=false;
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
    // Zero-noise synthetic packets exercise legacy feature logging separately.
    window.enforceBOnsetConsistency=false;
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
    QVector<QString> displayedCorrA;
    for (int shift = -50; shift <= 20; ++shift) {
        window.detectAndPlotSpeed(bc, bd, wave(937 + shift), wave(800 + shift));
        displayedCorrA.append(window.lblCorrAValue->toolTip());
    }
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
            bool displayed = false;
            const double actualDisplay = displayedCorrA.at(frames - 1).toDouble(&displayed);
            QVERIFY(displayed);
            QVERIFY(std::abs(actualDisplay - a.value("corr").toDouble()) < 1e-10);
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

    QVERIFY2(window.findChild<QProgressBar*>(QStringLiteral("barCorrA")),
             "The approved primary corrA meter must exist in the actual Qt window");

    QCOMPARE(window.ui->barPairB->orientation(), Qt::Vertical);
    QCOMPARE(window.ui->barPairA->orientation(), Qt::Horizontal);
    QCOMPARE(window.ui->lblBPairTitle->text(), QStringLiteral("② 辅助调整  G"));
    QCOMPARE(window.ui->lblAPairTitle->text(), QStringLiteral("辅助 D"));
    QCOMPARE(window.ui->lblPositionGuide->text(),
             QStringLiteral("先调整探头长轴方向"));
    QCOMPARE(window.ui->lblPositionGuideNote->text(),
             QStringLiteral("优先让 corrA 达到要求；连续计数后保持稳定。提示仅供参考，以有效值计数为准。"));
    QVERIFY(window.ui->lblPositionGuide->styleSheet().contains(
        QStringLiteral("font-weight:bold")));
    QCOMPARE(window.ui->btnMeasurementGuide->text(),
             QStringLiteral("操作教学"));

    const QString fixedGuide = window.ui->lblPositionGuide->text();
    const QString fixedNote = window.ui->lblPositionGuideNote->text();
    window.updateProcessPanel(0.0, 0.0, 105, 100, 5, 8.0, false);
    window.updateProcessPanel(0.0, 0.0, 107, 100, 7, 4.0, true);
    window.updateProcessInvalid(QStringLiteral("测试无效帧"));
    QCOMPARE(window.ui->lblPositionGuide->text(), fixedGuide);
    QCOMPARE(window.ui->lblPositionGuideNote->text(), fixedNote);
    QCOMPARE(window.ui->barMeasureProgress->value(), 1);
}

void MainWindowSafetyTests::corrAFeedbackBindingAndResponsiveLayout()
{
    MainWindow window;
    window.setAttribute(Qt::WA_DontShowOnScreen);
    window.ui->stackedWidget->setCurrentWidget(window.ui->pageMain);
    window.show();
    auto* corrA = window.findChild<QProgressBar*>(QStringLiteral("barCorrA"));
    QVERIFY(corrA);
    const auto mapped = [](double value, double target, double scale) {
        const int offset = qRound(320 * std::pow(qMin(std::abs(value-target)/scale, 1.0), 1.4));
        return 500 + (value > target ? offset : value < target ? -offset : 0);
    };
    for (const auto g : {-60., -12., -6., 0., 6., 12., 20.}) {
        window.updateProcessPanel(0, 0, 107, 100, 7, g, false);
        QCOMPARE(window.ui->barPairA->value(), mapped(7, window.mCfg.angleSignedDiffTarget, 6));
        QCOMPARE(window.ui->barPairB->value(), mapped(g, window.mCfg.anglePairMidGapTarget, 12));
    }
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    window.handlePatientMeasureValue(3900, 3900, 3900, 109, 100, 9, 0, .7799, .98, false);
    QCOMPARE(window.lblCorrAStatus->text(), QStringLiteral("未达标"));
    QCOMPARE(window.processValidCount, 0);
    window.handlePatientMeasureValue(3900, 3900, 3900, 109, 100, 9, 0, .846, .98, false);
    QCOMPARE(corrA->value(), 846);
    QCOMPARE(window.lblCorrAValue->text(), QStringLiteral("0.846"));
    QCOMPARE(window.lblCorrAStatus->text(), QStringLiteral("已达标"));
    QCOMPARE(window.processValidCount, 0); // corrA alone never admits a value
    window.updateCorrAFeedback(window.mCfg.frameCorrAMin);
    QCOMPARE(window.lblCorrAStatus->text(), QStringLiteral("已达标"));
    window.updateCorrAFeedback(-.2);
    QCOMPARE(corrA->value(), 0);
    QCOMPARE(window.lblCorrAValue->text(), QStringLiteral("-0.200"));
    window.updateCorrAFeedback(1);
    QCOMPARE(corrA->value(), 1000);
    const double threshold = window.mCfg.frameCorrAMin;
    window.mCfg.frameCorrAMin = .81;
    window.updateCorrAFeedback(.8);
    QVERIFY(window.lblCorrAThreshold->text().contains(QStringLiteral("0.81")));
    QCOMPARE(window.lblCorrAStatus->text(), QStringLiteral("未达标"));
    QCOMPARE(window.mCfg.frameCorrAMin, .81);
    window.mCfg.frameCorrAMin = threshold;
    window.updateCorrAFeedback(.846);
    window.updateProcessPanel(0, 0, 109, 100, 9, 0, true);
    window.ui->lblProcessStatus->setText(QStringLiteral("当前第 2/5 轮"));
    const QString captureDir = qEnvironmentVariable("BONE_UI_CAPTURE_DIR");
    for (const auto size : {QSize(1920,1080), QSize(1366,768)}) {
        window.resize(size);
        window.scheduleResponsiveLayout();
        QTest::qWait(100);
        auto* area = window.ui->grpProcessArea;
        auto* aCard = window.findChild<QFrame*>(QStringLiteral("corrACard"));
        auto* gCard = window.findChild<QFrame*>(QStringLiteral("gCard"));
        QVERIFY(aCard && gCard);
        QVERIFY(aCard->width() > gCard->width());
        for (auto* child : {static_cast<QWidget*>(corrA), static_cast<QWidget*>(window.ui->barPairB),
                           static_cast<QWidget*>(window.ui->barPairA),
                           static_cast<QWidget*>(window.ui->lblPositionGuideNote),
                           static_cast<QWidget*>(window.ui->lblProcessStatus)}) {
            QVERIFY2(area->rect().contains(QRect(child->mapTo(area,QPoint()),child->size())),
                     qPrintable(child->objectName()));
        }
        QVERIFY(!area->geometry().intersects(window.ui->grpReferenceCurveArea->geometry()));
        auto* line = corrA->findChild<QFrame*>(QStringLiteral("middleLine"));
        QVERIFY(line);
        QCOMPARE(line->y(), qRound((corrA->height()-2)*(1-threshold)));
        auto* dLine = window.ui->barPairA->findChild<QFrame*>(QStringLiteral("middleLine"));
        QVERIFY(dLine);
        QCOMPARE(dLine->width(), 2);
        if (!captureDir.isEmpty()) {
            QVERIFY(QDir().mkpath(captureDir));
            QVERIFY(window.grab().save(QDir(captureDir).filePath(QStringLiteral("feedback-%1.png").arg(size.width()))));
            if (size.width() == 1920)
                QVERIFY(area->grab().save(QDir(captureDir).filePath(QStringLiteral("feedback-panel.png"))));
        }
    }
    window.updateProcessInvalid(QStringLiteral("测试无效帧"));
    QCOMPARE(window.ui->barMeasureProgress->value(), 1);
    QVERIFY(!corrA->isEnabled());
    QCOMPARE(corrA->value(), 0);
    QCOMPARE(window.lblCorrAValue->text(), QStringLiteral("—"));
    QCOMPARE(window.ui->lblPairAValue->text(), QStringLiteral("D=--"));
    for (int action = 0; action < 3; ++action) {
        window.updateCorrAFeedback(.9);
        if (action == 0) window.stopPatientMeasurement();
        if (action == 1) window.resetOneRoundMeasurementState();
        if (action == 2) window.resetAllPatientMeasurementData();
        QVERIFY(!corrA->isEnabled());
        QCOMPARE(window.lblCorrAValue->text(), QStringLiteral("—"));
    }
}

void MainWindowSafetyTests::measurementStatusIsVisibleAndOperatorFacing()
{
    MainWindow window;
    window.hide();

    QVERIFY(window.ui->lblProcessStatus->wordWrap());

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
    QFile oldMarker(settingsPath);
    QVERIFY(oldMarker.open(QIODevice::WriteOnly));
    oldMarker.write("version=1\n");
    oldMarker.close();
    QVERIFY(MeasurementGuideDialog::currentGuideVersion() > 1);
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
        QStringLiteral("贴合")));
    QVERIFY(pages->widget(1)->findChild<QLabel*>(
        QStringLiteral("guideHeading"))->text().contains(
        QStringLiteral("corrA")));
    QVERIFY(pages->widget(2)->findChild<QLabel*>(
        QStringLiteral("guideHeading"))->text().contains(
        QStringLiteral("倾角")));
    QVERIFY(pages->widget(2)->findChild<QLabel*>(
        QStringLiteral("guideBody"))->text().contains(
        QStringLiteral("等待 1 秒自动进入下一轮")));
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
    auto* back = dialog.findChild<QPushButton*>(QStringLiteral("guideBackButton"));
    back->click();
    QCOMPARE(pages->currentIndex(), 1);
    dialog.findChild<QPushButton*>(QStringLiteral("guideStep1"))->click();
    QCOMPARE(pages->currentIndex(), 0);
    dialog.resize(760, 560);
    capture(QStringLiteral("measurement-guide-small.png"));
    dialog.findChild<QPushButton*>(QStringLiteral("guideSkipButton"))->click();
    QCOMPARE(dialog.result(), int(QDialog::Accepted));
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

void MainWindowSafetyTests::automaticNextRoundIsGuardedAndCancelable()
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
    window.measurementGuideSeenThisRun = true;
    window.nextRoundDelayMs = 100;
    window.updatePatientSelectionUi();

    QSignalSpy nextRoundTimeout(&window.nextRoundTimer, &QTimer::timeout);

    window.startPatientMeasurement(5);
    for (int i = 0; i < window.processValidTarget; ++i) {
        window.handlePatientMeasureValue(
            3884, 3984, 3984, 132, 123, 9, 0, .95, .95, true);
    }
    QCOMPARE(window.roundSosList.size(), 1);
    QVERIFY(!window.patientMeasureRunning);
    QVERIFY(window.nextRoundTimer.isActive());
    QVERIFY(window.ui->lblProcessStatus->text().contains(QStringLiteral("1 秒后自动")));
    QCOMPARE(window.ui->btnStartMeasurement->text(), QStringLiteral("立即开始下一轮"));
    QVERIFY(window.ui->btnStartMeasurement->isEnabled());
    QVERIFY(!window.ui->btnPatientInfo->isEnabled());
    QVERIFY(!window.ui->btnMeasurementGuide->isEnabled());

    window.ui->btnStartMeasurement->setFocus(Qt::OtherFocusReason);
    QTest::keyClick(window.ui->btnStartMeasurement, Qt::Key_Space);
    QVERIFY(window.patientMeasureRunning);
    QVERIFY(!window.nextRoundTimer.isActive());
    QTest::qWait(150);
    QCOMPARE(nextRoundTimeout.count(), 0);
    QCOMPARE(window.ui->lblProcessStatus->text(), QStringLiteral("当前第 2/5 轮"));
    window.stopPatientMeasurement();

    window.nextRoundDelayMs = 20;
    window.scheduleNextPatientRound(1);
    QVERIFY(window.nextRoundTimer.isActive());
    QTRY_VERIFY_WITH_TIMEOUT(window.patientMeasureRunning, 500);
    QCOMPARE(nextRoundTimeout.count(), 1);
    QCOMPARE(window.ui->lblProcessStatus->text(), QStringLiteral("当前第 2/5 轮"));
    window.stopPatientMeasurement();

    window.startPatientMeasurement(5);
    for (int i = 0; i < window.processValidTarget; ++i) {
        window.handlePatientMeasureValue(
            3884, 3984, 3984, 132, 123, 9, 0, .10, .10, true);
    }
    QVERIFY(!window.patientMeasureRunning);
    QVERIFY(!window.nextRoundTimer.isActive());
    QCOMPARE(nextRoundTimeout.count(), 1);
    window.closeRoundFinishedTip();

    window.roundSosList = {3900, 3901, 3902, 3903, 3904};
    window.scheduleNextPatientRound(5);
    QVERIFY(!window.nextRoundTimer.isActive());

    window.roundSosList = {3900};
    window.scheduleNextPatientRound(1);
    QVERIFY(window.nextRoundTimer.isActive());
    window.resetDisconnectedAcquisitionState();
    QVERIFY(!window.nextRoundTimer.isActive());
    QTest::qWait(50);
    QCOMPARE(nextRoundTimeout.count(), 1);
    QVERIFY(!window.patientMeasureRunning);
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

void MainWindowSafetyTests::liveChartRenderingRejectsUnsafeInputs()
{
    MainWindow window;
    window.hide();

    QVERIFY(!(window.viewA->renderHints() & QPainter::Antialiasing));
    QVERIFY(!(window.viewB->renderHints() & QPainter::Antialiasing));
    QVERIFY(!(window.viewC->renderHints() & QPainter::Antialiasing));
    QVERIFY(!(window.viewD->renderHints() & QPainter::Antialiasing));
    QVERIFY(!(window.ui->chartViewSpeed->renderHints() & QPainter::Antialiasing));

    window.liveWaveformRenderTimer.invalidate();
    QVERIFY(window.shouldRefreshLiveWaveforms());
    QVERIFY(!window.shouldRefreshLiveWaveforms());
    QTest::qWait(MainWindow::liveWaveformRefreshIntervalMs + 10);
    QVERIFY(window.shouldRefreshLiveWaveforms());

    const int initialCount = window.seriesSpeed->count();
    const int initialIndex = window.speedPointIndex;
    window.appendSpeedPoint(std::numeric_limits<double>::quiet_NaN());
    window.appendSpeedPoint(std::numeric_limits<double>::infinity());
    QCOMPARE(window.seriesSpeed->count(), initialCount);
    QCOMPARE(window.speedPointIndex, initialIndex);
}

void MainWindowSafetyTests::liveChartPaintStress()
{
    MainWindow window;
    window.hide();

    const QVector<QLineSeries*> series = {
        window.seriesA, window.seriesB, window.seriesC, window.seriesD
    };
    const QVector<QChartView*> views = {
        window.viewA, window.viewB, window.viewC, window.viewD
    };
    for (QChartView* view : views) {
        view->resize(640, 125);
    }

    QVector<QPointF> points;
    points.reserve(2000);
    for (int sample = 0; sample < 2000; ++sample) {
        points.append(QPointF(sample, 2048.0));
    }

    QImage target(640, 125, QImage::Format_ARGB32_Premultiplied);
    target.fill(Qt::black);
    constexpr int tenMinuteEquivalentUpdates = 2400;
    for (int update = 0; update < tenMinuteEquivalentUpdates; ++update) {
        const double phase = update * 0.03;
        for (int sample = 0; sample < points.size(); ++sample) {
            points[sample].setY(2048.0 + 900.0 * std::sin(sample * 0.025 + phase));
        }
        for (QLineSeries* line : series) {
            line->replace(points);
        }
        if ((update % 4) == 0) {
            QPainter painter(&target);
            views[(update / 4) % views.size()]->render(&painter);
        }
        if ((update % 60) == 0) {
            QCoreApplication::processEvents();
        }
    }

    for (QLineSeries* line : series) {
        QCOMPARE(line->count(), 2000);
    }
}

void MainWindowSafetyTests::disconnectedControlsAndPlaceholdersAreSafe()
{
    MainWindow window;
    window.hide();
    QVERIFY(!window.serial->isOpen());
    QVERIFY(!window.ui->pushButton->isEnabled());
    QVERIFY(!window.ui->triggerButton->isEnabled());
    QVERIFY(!window.ui->btnStartMeasurement->isEnabled());
    QVERIFY(!window.findChild<QPushButton*>(QStringLiteral("wifi_button")));
    QCOMPARE(window.ui->grpWaveArea->title(), QStringLiteral("四通道波形"));
    QCOMPARE(window.ui->grpSpeedArea->title(), QStringLiteral("声速趋势"));
    QCOMPARE(window.ui->grpProcessArea->title(), QStringLiteral("检测过程"));
    QCOMPARE(window.ui->grpPartImageRight->title(), QStringLiteral("测量部位"));
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

    for (const auto mode : {PatientFormDialog::Mode::Create, PatientFormDialog::Mode::Edit}) {
        PatientFormDialog dialog(mode, &window);
        dialog.setAttribute(Qt::WA_DontShowOnScreen, true);
        if (mode == PatientFormDialog::Mode::Edit) dialog.setPatient(samplePatient());
        dialog.show();
        QApplication::processEvents();
        dialog.layout()->activate();
        QVERIFY(dialog.width() <= window.width());
        QVERIFY(dialog.height() <= window.height());
        for (QWidget* control : QList<QWidget*>{dialog.nameEdit, dialog.idEdit, dialog.maleButton,
                                                dialog.femaleButton, dialog.yearEdit, dialog.monthEdit,
                                                dialog.dayEdit, dialog.heightEdit, dialog.weightEdit,
                                                dialog.saveButton, dialog.cancelButton}) {
            QVERIFY(control);
            QVERIFY(!control->isHidden());
            const QRect bounds(control->mapTo(&dialog, QPoint(0, 0)), control->size());
            QVERIFY2(dialog.rect().contains(bounds), qPrintable(control->objectName()));
        }
        dialog.close();
    }
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
    PatientInfo newPatient;
    newPatient.name = QStringLiteral("新患者");
    newPatient.id = QStringLiteral("patient-new");
    newPatient.gender = QStringLiteral("男");
    newPatient.birthDay = QStringLiteral("2000-01-01");
    newPatient.height = QStringLiteral("170");
    newPatient.weight = QStringLiteral("60");

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->done(QMessageBox::No);
        }
    });
    closer.start(0);
    QVERIFY(!window.createPatient(newPatient, true));
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
    QVERIFY(logPath.startsWith(DataLocation::experimentsRoot()));
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
    data.ageSosChart.hasPatient = true;
    data.ageSosChart.hasMeasurementRecords = true;
    data.ageSosChart.gender = QStringLiteral("女");
    data.ageSosChart.focalAge = 47;
    AgeSosMeasurementPoint historyPoint;
    historyPoint.age = 41;
    historyPoint.sos = 3980.0;
    historyPoint.measuredAt = QStringLiteral("2020-08-16T09:00:00");
    data.ageSosChart.points.append(historyPoint);
    historyPoint.age = 44;
    historyPoint.sos = 4035.0;
    historyPoint.measuredAt = QStringLiteral("2023-07-09T09:00:00");
    data.ageSosChart.points.append(historyPoint);
    historyPoint.age = 47;
    historyPoint.sos = 4090.0;
    historyPoint.measuredAt = QStringLiteral("2026-09-22T10:26:00");
    historyPoint.highlighted = true;
    data.ageSosChart.points.append(historyPoint);

    ReportWidget report;
    report.setReportData(data);
    QImage image(QSize(795, 1124), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    report.renderReport(&painter, QRectF(QPointF(0, 0), image.size()));
    painter.end();

    int redPixels = 0;
    int grayPixels = 0;
    for (int y = 410; y < 850; ++y) {
        for (int x = 40; x < 755; ++x) {
            const QColor color = image.pixelColor(x, y);
            if (color.red() > 210 && color.green() < 150 && color.blue() < 150) ++redPixels;
            if (qAbs(color.red() - color.green()) < 8 &&
                qAbs(color.green() - color.blue()) < 8 &&
                color.red() >= 90 && color.red() <= 180) ++grayPixels;
        }
    }
    QVERIFY(redPixels > 20);
    QVERIFY(grayPixels > 20);
    QCOMPARE(report.reportData().tScore, QString());
    QCOMPARE(report.reportData().zScore, QString());

    const QString captureDir = qEnvironmentVariable("BONE_UI_CAPTURE_DIR");
    if (!captureDir.isEmpty()) {
        QVERIFY(QDir().mkpath(captureDir));
        QVERIFY(image.save(QDir(captureDir).filePath(QStringLiteral("report.png"))));
    }
}

void MainWindowSafetyTests::ageSosHistoryUsesStoredAgeProfileAndReportCutoff()
{
    MainWindow window;
    window.hide();
    PatientInfo patient = samplePatient();

    auto record = [&patient](const QString& id, const QString& measuredAt,
                             int age, double sos) {
        MeasurementRecord value;
        value.id = id;
        value.patientId = patient.id;
        value.patientName = patient.name;
        value.patientGender = patient.gender;
        value.patientBirthDay = patient.birthDay;
        value.patientAge = QString::number(age);
        value.measuredAt = measuredAt;
        value.sos = QString::number(sos, 'f', 1);
        return value;
    };

    const MeasurementRecord child = record(QStringLiteral("child"),
                                            QStringLiteral("2008-05-01T09:00:00"),
                                            18, 3900.0);
    const MeasurementRecord adultPast = record(QStringLiteral("adult-past"),
                                                QStringLiteral("2011-05-01T09:00:00"),
                                                21, 4000.0);
    const MeasurementRecord focal = record(QStringLiteral("adult-focal"),
                                            QStringLiteral("2014-05-01T09:00:00"),
                                            24, 4050.0);
    const MeasurementRecord future = record(QStringLiteral("adult-future"),
                                             QStringLiteral("2017-05-01T09:00:00"),
                                             27, 4100.0);
    const MeasurementRecord invalid = record(QStringLiteral("invalid"),
                                              QStringLiteral("2013-05-01T09:00:00"),
                                              23, 9999.0);
    window.measurementList = {child, adultPast, focal, future, invalid};

    const ReportData data = window.buildReportData(patient, focal);
    QCOMPARE(data.ageSosChart.focalAge, 24);
    QCOMPARE(data.ageSosChart.omittedOtherProfileCount, 1);
    QCOMPARE(data.ageSosChart.points.size(), 2);
    QCOMPARE(data.ageSosChart.points.at(0).age, 21);
    QVERIFY(!data.ageSosChart.points.at(0).highlighted);
    QCOMPARE(data.ageSosChart.points.at(1).age, 24);
    QVERIFY(data.ageSosChart.points.at(1).highlighted);
}

void MainWindowSafetyTests::mainAgeSosChartHighlightsLatestValidMeasurement()
{
    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    window.currentPatient = patient;

    MeasurementRecord first = sampleMeasurement();
    first.id = QStringLiteral("first");
    first.patientAge = QStringLiteral("35");
    first.patientGender = QStringLiteral("女");
    first.measuredAt = QStringLiteral("2025-01-01T10:00:00");
    first.sos = QStringLiteral("3980");
    MeasurementRecord latest = first;
    latest.id = QStringLiteral("latest");
    latest.patientAge = QStringLiteral("36");
    latest.measuredAt = QStringLiteral("2026-01-01T10:00:00");
    latest.sos = QStringLiteral("4020");
    MeasurementRecord invalidLatest = latest;
    invalidLatest.id = QStringLiteral("invalid-latest");
    invalidLatest.measuredAt = QStringLiteral("2027-01-01T10:00:00");
    invalidLatest.sos = QStringLiteral("9000");
    window.measurementList = {first, latest, invalidLatest};

    window.updateAgeSosReference();
    const AgeSosChartData& chart = window.ui->chartViewReference->chartData();
    QCOMPARE(chart.points.size(), 2);
    QVERIFY(!chart.points.at(0).highlighted);
    QVERIFY(chart.points.at(1).highlighted);
    QCOMPARE(chart.points.at(1).age, 36);
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
    const QString captureDir = qEnvironmentVariable("BONE_UI_CAPTURE_DIR");
    if (!captureDir.isEmpty()) QVERIFY(QDir().mkpath(captureDir));
    const QString pdfPath = captureDir.isEmpty()
        ? directory.filePath(QStringLiteral("report.pdf"))
        : QDir(captureDir).filePath(QStringLiteral("report.pdf"));

    MainWindow window;
    window.hide();
    ReportData data;
    data.patientName = QStringLiteral("【测试】二十岁女性");
    data.patientId = QStringLiteral("W20");
    data.age = QStringLiteral("47");
    data.gender = QStringLiteral("女");
    data.birthDay = QStringLiteral("1979-04-18");
    data.measuredAt = QStringLiteral("2026-09-22 10:26");
    data.height = QStringLiteral("164 cm");
    data.weight = QStringLiteral("56 kg");
    data.part = QStringLiteral("桡骨");
    data.sos = QStringLiteral("4090.0 m/s");
    data.boneStrength = QStringLiteral("正常");
    data.tScore = QStringLiteral("-0.8");
    data.zScore = QStringLiteral("-0.3");
    data.diagnosis = QStringLiteral("匿名测试数据，仅用于报表渲染验证");
    data.operatorName = QStringLiteral("测试");
    data.ageSosChart.hasPatient = true;
    data.ageSosChart.hasMeasurementRecords = true;
    data.ageSosChart.gender = QStringLiteral("女");
    data.ageSosChart.focalAge = 47;
    data.ageSosChart.points.append(
        {41, 3980.0, QStringLiteral("2020-08-16T09:00:00"), false});
    data.ageSosChart.points.append(
        {44, 4035.0, QStringLiteral("2023-07-09T09:00:00"), false});
    data.ageSosChart.points.append(
        {47, 4090.0, QStringLiteral("2026-09-22T10:26:00"), true});
    window.reportWidget->setReportData(data);
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

    capture(window.ui->pageLogin, QStringLiteral("login.png"));
    capture(window.ui->pageMain, QStringLiteral("main.png"));

    // In-memory anonymous sample data only; nothing is written to disk.
    PatientInfo patient = samplePatient();
    PatientInfo other = samplePatient();
    other.id = QStringLiteral("patient-002");
    other.name = QStringLiteral("示例被测者");
    other.gender = QStringLiteral("男");
    other.birthDay = QStringLiteral("1958-04-12");
    MeasurementRecord older = sampleMeasurement();
    older.id = QStringLiteral("measurement-older");
    older.measuredAt = QStringLiteral("2025-03-12T14:05:00");
    older.sos = QStringLiteral("4031.5");
    older.boneStrength = QStringLiteral("正常");
    MeasurementRecord latest = sampleMeasurement();
    latest.tScore = QStringLiteral("-0.85");
    latest.zScore = QStringLiteral("-0.40");
    latest.boneStrength = QStringLiteral("正常");
    latest.fractureRisk = QStringLiteral("1.4");
    latest.boneAge = QStringLiteral("38");
    window.patientList = {patient, other};
    window.measurementList = {older, latest};
    QVERIFY(window.selectCurrentPatient(patient));
    window.refreshTable(window.patientList);
    window.ui->table->selectRow(0);
    window.ui->table->item(1, MainWindow::ArchiveIdColumn)->setCheckState(Qt::Checked);

    capture(window.ui->pageArchive, QStringLiteral("archive.png"));
    for (const QSize& size : {QSize(1366, 768), QSize(1600, 900), QSize(1920, 1080), QSize(1920, 1013)}) {
        window.resize(size);
        QTest::qWait(100);
        capture(window.ui->pageMain, QStringLiteral("main-%1x%2.png").arg(size.width()).arg(size.height()));
    }
    PatientFormDialog dialog(PatientFormDialog::Mode::Create, &window);
    dialog.setAttribute(Qt::WA_DontShowOnScreen, true);
    dialog.show();
    QTest::qWait(50);
    QVERIFY(dialog.grab().save(QDir(captureDir).filePath(QStringLiteral("patient-form.png"))));
}

#include "onset_guard_cases.inc"
#include "subject_recording_cases.inc"
#include "ui_refresh_cases.inc"
#include "sos_reference_cases.inc"
#include "data_backup_cases.inc"
#include "workflow_cases.inc"
#include "data_location_cases.inc"

QTEST_MAIN(MainWindowSafetyTests)
#include "mainwindow_safety_tests.moc"
