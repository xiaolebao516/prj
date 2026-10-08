// 被测者检测流程、逐帧处理、轮次与结果、校准采集、操作教学

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bonehealth.h"
#include "sosreference.h"
#include "agesoschartwidget.h"
#include "reportwidget.h"
#include "calibrationdialog.h"
#include "measurementguidedialog.h"
#include "utils.h"
#include "patientformdialog.h"
#include "databackup.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDoubleValidator>
#include <QResizeEvent>
#include <QCheckBox>
#include <QDateEdit>
#include <QHeaderView>
#include <QScreen>
#include <QSignalBlocker>
#include <QStyle>
#include <QtCharts/QValueAxis>
#include <QRegularExpression>
#include <QInputDialog>
#include <QDateTime>
#include <QCoreApplication>
#include <QDomElement>
#include <QFileDialog>
#include <QFileInfo>
#include <algorithm>
#include <utility>
#include <cmath> // 确保包含 math 头文件
#include <limits>
#include <QFrame>
#include <QGridLayout>
#include <QBoxLayout>
#include <QPointer>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTableWidget>
#include <QPushButton>
#include <QToolButton>
#include <QSettings>
#include <QSaveFile>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QUuid>
#include <QAction>
#include <QMenuBar>
#include <QSet>
#include <QScopeGuard>
#include <QJsonArray>
#include <QStandardPaths>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include "mainwindow_internal.h"

using namespace mainwindow_detail;


void MainWindow::openCalibrationDialog()
{
    if (patientMeasureRunning) {
        QMessageBox::information(this,
                                 QStringLiteral("正在检测"),
                                 QStringLiteral("请先停止当前检测，再进入探头校准。"));
        return;
    }

    if (hasIncompletePatientRounds()) {
        QMessageBox::information(
            this,
            QStringLiteral("检测尚未完成"),
            QStringLiteral("当前被测者已完成 %1/%2 次测量。请先完成本组检测，"
                           "避免同一组测量混用不同的校准参数。")
                .arg(roundSosList.size())
                .arg(normalMeasureRounds));
        return;
    }

    if (autoRunning) {
        autoTimer->stop();
        autoRunning = false;
        ui->triggerButton->setText(QStringLiteral("自动采集"));
        updatePatientSelectionUi();
    }

    CalibrationDialog dialog(&calibrationStore, currentAccount.username, this);
    calibrationDialog = &dialog;
    connect(&dialog, &CalibrationDialog::acquisitionStartRequested,
            this, &MainWindow::startCalibrationAcquisition);
    connect(&dialog, &CalibrationDialog::acquisitionStopRequested,
            this, &MainWindow::stopCalibrationAcquisition);
    connect(&dialog, &CalibrationDialog::activeDChanged, this, [this](double activeD) {
        signalProcessor.probeDistanceCD = activeD;
        calibrationSignalProcessor.probeDistanceCD = activeD;
    });
    dialog.exec();
    stopCalibrationAcquisition();
    calibrationDialog = nullptr;
}

void MainWindow::startCalibrationAcquisition(double processingD)
{
    if (!serial->isOpen()) {
        if (calibrationDialog) {
            calibrationDialog->notifyAcquisitionUnavailable(
                QStringLiteral("设备未连接。请先连接骨密度仪，再重新开始本次采集。"));
        }
        return;
    }

    rxBuffer.clear();
    clearFrameAssembly();
    samplesA.clear();
    samplesB.clear();
    samplesC.clear();
    samplesD.clear();
    chReceived[0] = chReceived[1] = chReceived[2] = chReceived[3] = false;
    serial->readAll();
    calibrationSignalProcessor.probeDistanceCD = processingD;
    acquireMode = CalibrationAcquireMode;
    autoTimer->start(80);
}

void MainWindow::stopCalibrationAcquisition()
{
    if (autoTimer && autoTimer->isActive() && acquireMode == CalibrationAcquireMode) {
        autoTimer->stop();
    }
    if (acquireMode == CalibrationAcquireMode) acquireMode = DebugAcquireMode;
    rxBuffer.clear();
    clearFrameAssembly();
    chReceived[0] = chReceived[1] = chReceived[2] = chReceived[3] = false;
    calibrationSignalProcessor.probeDistanceCD = calibrationStore.parameters().activeD;
}

bool MainWindow::hasCurrentPatient() const
{
    return !currentPatient.id.trimmed().isEmpty()
    && !currentPatient.name.trimmed().isEmpty();
}

void MainWindow::startPatientMeasurement(int targetRounds, bool offerFirstUseGuide)
{
    cancelPendingNextPatientRound();

    if (!hasCurrentPatient()) {
        pendingStartAfterPatientInfo = false;
        on_btnPatientInfo_clicked();
        return;
    }

    const QDate birthDate =
        QDate::fromString(currentPatient.birthDay, QStringLiteral("yyyy-MM-dd"));
    if (!birthDate.isValid() || birthDate > QDate::currentDate()) {
        QMessageBox::warning(
            this,
            QStringLiteral("日期异常"),
            QStringLiteral("被测者出生日期无效，或当前 Windows 系统日期早于出生日期。"
                           "请先核对档案和电脑日期，再开始检测。"));
        return;
    }

    if (!serial->isOpen()) {
        QMessageBox::warning(this, "串口未连接", "请先连接串口设备。");
        return;
    }

    if (offerFirstUseGuide && shouldOfferMeasurementGuide()
        && !runMeasurementGuide(true)) {
        return;
    }

    rxBuffer.clear();
    clearFrameAssembly();
    samplesA.clear();
    samplesB.clear();
    samplesC.clear();
    samplesD.clear();
    chReceived[0] = chReceived[1] = chReceived[2] = chReceived[3] = false;
    serial->readAll();

    if (roundSosList.size() >= normalMeasureRounds) {
        resetPatientMeasurementState(normalMeasureRounds);
    }
    if (roundSosList.size() >= normalMeasureRounds) {
        QMessageBox::information(this,
                                 "检测已完成",
                                 "当前被检者已经完成 5 次测量。");
        return;
    }

    // 关闭上一轮完成提示
    closeRoundFinishedTip();

    // 如果之前 trigger 正在调试采集，先停掉
    if (autoRunning) {
        autoTimer->stop();
        autoRunning = false;
        ui->triggerButton->setText("自动采集");
    }

    acquireMode = PatientMeasureMode;
    patientMeasureRunning = true;
    updatePatientSelectionUi();
    currentMeasureTargetRounds = qMax(1, targetRounds);

    // 只重置当前这一轮，不清空 roundSosList
    resetOneRoundMeasurementState();

    int nextRound = roundSosList.size() + 1;
    startExperimentLog();

    updatePatientSelectionUi();
    ui->btnPatientInfo->setStyleSheet("");  // 恢复默认样式，确保可点击

    ui->lblProcessStatus->setText(
        QString("当前第 %1/%2 轮")
            .arg(nextRound)
            .arg(normalMeasureRounds));

    ui->lblProcessStatus->setStyleSheet(
        "font-size: 14px; color: #1D5FA8; font-weight: bold;"
        );

    autoTimer->start(80);
    updatePatientSelectionUi();
}

double MainWindow::dualWindowAQuality(const QVector<double>& early, const QVector<double>& late,
                                     int onset, int lag, double* front, double* middle)
{
    // Quality only: both windows use the already selected A lag, never a new peak.
    const auto score = [&](int lo, int hi) {
        const qint64 begin=qint64(onset)+lo, end=qint64(onset)+hi;
        if (lag<=0 || begin<0 || end>=early.size() || end+lag>=late.size()) return 0.0;
        double ma=0,mb=0,aa=0,bb=0,ab=0;
        for (qint64 i=begin;i<=end;++i) {
            if (!std::isfinite(early[i]) || !std::isfinite(late[i+lag])) return 0.0;
            ma+=early[i]; mb+=late[i+lag];
        }
        ma/=end-begin+1; mb/=end-begin+1;
        for (qint64 i=begin;i<=end;++i) {
            const double a=early[i]-ma,b=late[i+lag]-mb;
            aa+=a*a; bb+=b*b; ab+=a*b;
        }
        if (aa<1e-12 || bb<1e-12) return 0.0;
        const double value=ab/std::sqrt(aa*bb);
        return std::isfinite(value) ? qBound(-1.0,value,1.0) : 0.0;
    };
    const double a=score(-20,30),b=score(0,60);
    if (front) *front=a;
    if (middle) *middle=b;
    return qMin(a,b);
}

void MainWindow::startExperimentLog()
{
#ifndef QT_NO_DEBUG
    experimentLogWarningShown = false;
    // Freeze the approved identity fields for the entire measurement, including
    // failed/retried rounds. A rename must not relabel an existing session.
    if (experimentSessionId.isEmpty() ||
        experimentSubjectSnapshot.value("archive_id").toString() != currentPatient.id) {
        experimentSessionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
        experimentSubjectSnapshot = QJsonObject{{"archive_id", currentPatient.id},
                                               {"name", currentPatient.name}};
    }
    QJsonArray previousRounds;
    for (double sos : roundSosList) previousRounds.append(sos);
#ifdef BONE_COMPLETE_B_PEAK_EXPERIMENT
    const char* implementation="b-peak-completion-20260908-v1";
#elif defined(BONE_RELOCK_PRESERVATION_EXPERIMENT)
    const char* implementation="relock-preservation-20260908-v1";
#elif defined(BONE_DUAL_WINDOW_A_EXPERIMENT)
    const char* implementation="dual-window-a078-20260906-v1";
#elif defined(BONE_OBSERVE_BEFORE_G_EXPERIMENT)
    const char* implementation="observe-before-g-20260906-v1";
#else
    const char* implementation="onset-consistency-20260915-v1";
#endif
    const QJsonObject config{
        {"recording_profile", "subject-linked-20260918-v1"},
        {"measurement_session_id", experimentSessionId},
        {"subject", experimentSubjectSnapshot},
        {"previous_accepted_rounds", previousRounds},
        {"implementation", implementation},
        {"B_onset_forward_limit", enforceBOnsetConsistency ? bOnsetForwardLimit : 0},
        {"B_clipped_peak_extension", completeTruncatedBPeak ? 15 : 0},
        {"partial_relock_retention_lag", deferPartialDiscardUntilRelock
            ? partialRelockRetentionTolerance : 0},
        {"build", __DATE__ " " __TIME__},
        {"round", roundSosList.size() + 1}, {"round_target", normalMeasureRounds},
        {"frame_target", processValidTarget}, {"round_cluster_tolerance", roundClusterTolerance},
        {"probe_distance_m", signalProcessor.probeDistanceCD},
        {"sample_period_s", signalProcessor.samplePeriod},
        {"B_only", useBOnlyForPatientSos}, {"SOS_offset", patientSosOffset},
        {"angle_gate_enabled", enablePatientAngleGate},
        {"frame_corr_A", mCfg.frameCorrAMin}, {"frame_corr_B", mCfg.frameCorrBMin},
        {"round_corr_A", mCfg.roundCorrAMin}, {"round_corr_B", mCfg.roundCorrBMin},
        {"D_min", mCfg.angleSignedDiffMin}, {"D_max", mCfg.angleSignedDiffMax},
        {"G_min", mCfg.anglePairMidGapMin}, {"G_max", mCfg.anglePairMidGapMax},
        {"warmup", mCfg.stableLagWarmupCount}, {"lock_need", mCfg.stableLagLockNeedCount},
        {"lag_tolerance", mCfg.stableLagTolerance}, {"unlock_count", mCfg.boneLagUnlockCount},
        {"window_size", stableLagWindowSize},
        {"gain_BC", gainSliderA->value()}, {"gain_BD", gainSliderB->value()},
        {"gain_AC", gainSliderC->value()}, {"gain_AD", gainSliderD->value()}};
    experimentLog.start(QCoreApplication::applicationDirPath() + "/measurement-experiments", config);
    checkExperimentLogError();
#endif
}

void MainWindow::checkExperimentLogError()
{
    if (experimentLogWarningShown || experimentLog.error().isEmpty()) return;
    experimentLogWarningShown = true;
    qWarning() << "Experiment recording unavailable:" << experimentLog.error();
    statusBar()->showMessage(QStringLiteral("实验记录未保存完整，请检查剩余磁盘空间和文件夹权限；检测本身不受此提示控制。"));
}

void MainWindow::stopPatientMeasurement()
{
    cancelPendingNextPatientRound();
    clearFeedbackReadings();

    if (experimentLog.active()) {
        experimentLog.write({{"event", "stop"}, {"partial_values", processValidCount},
                             {"accepted_rounds", roundSosList.size()}});
        experimentLog.close();
        checkExperimentLogError();
    }
    checkExperimentLogError();
    if (autoTimer->isActive()) {
        autoTimer->stop();
    }

    patientMeasureRunning = false;
    acquireMode = DebugAcquireMode;
    updatePatientSelectionUi();

    updatePatientSelectionUi();
}

void MainWindow::scheduleNextPatientRound(int finishedRounds)
{
    cancelPendingNextPatientRound();
    if (finishedRounds <= 0 ||
        finishedRounds >= normalMeasureRounds ||
        finishedRounds != roundSosList.size() ||
        patientMeasureRunning ||
        !patientDataWritable ||
        hasPendingMeasurement ||
        !hasCurrentPatient() ||
        !serial || !serial->isOpen()) {
        updatePatientSelectionUi();
        return;
    }

    pendingNextRoundPatientId = currentPatient.id;
    pendingNextRoundFinishedRounds = finishedRounds;
    nextRoundTimer.start(nextRoundDelayMs);
    updatePatientSelectionUi();
}

void MainWindow::cancelPendingNextPatientRound()
{
    if (nextRoundTimer.isActive()) nextRoundTimer.stop();
    pendingNextRoundPatientId.clear();
    pendingNextRoundFinishedRounds = 0;
}

// Keep the dedicated measurement button synchronized after the legacy reset.
void MainWindow::resetPatientMeasurementState(int targetRounds)
{
    currentMeasureTargetRounds = qMax(1, targetRounds);
    resetAllPatientMeasurementData();
}

void MainWindow::resetAllPatientMeasurementData()
{
    cancelPendingNextPatientRound();
    if (experimentLog.active()) {
        experimentLog.write({{"event", "reset_all"}});
        experimentLog.close();
        checkExperimentLogError();
    }
    experimentSessionId.clear();
    experimentSubjectSnapshot = QJsonObject();
    if (autoTimer && autoTimer->isActive()) {
        autoTimer->stop();
    }
    autoRunning = false;
    acquireMode = DebugAcquireMode;

    currentRoundSosList.clear();
    currentRoundAList.clear();
    currentRoundBList.clear();
    currentRoundCorrAList.clear();
    currentRoundCorrBList.clear();
    currentRoundPairMidGapList.clear();
    currentRoundSignedLagDiffList.clear();

    roundSosList.clear();
    roundAList.clear();
    roundBList.clear();
    candidateRoundList.clear();

    processValidCount = 0;

    // ✅ 新增：清空正式测量稳定 lag 窗口
    resetBoneLagStability();
    resetGateStats();
    lastFrameBJumpOk = false;
    lastFrameBoundaryOk = false;
    lastFrameDiffOk = false;
    lastFrameDirectionOk = false;
    lastFrameCorrOk = false;
    lastFrameAngleSignedDiffOk = false;
    lastFrameAnglePairMidGapOk = false;
    lastFrameAngleOk = false;
    lastFrameStableOk = false;
    lastFrameStableState = 0;

    closeRoundFinishedTip();

    ui->barPairA->setValue(500);
    ui->barPairB->setValue(500);


    ui->barMeasureProgress->setRange(0, processValidTarget);
    ui->barMeasureProgress->setValue(0);
    ui->barMeasureProgress->setFormat("有效值：%v / %m");
    ui->barMeasureProgress->setTextVisible(true);

    clearFeedbackReadings();
    ui->lblProcessStatus->setText("等待开始测量");
    ui->lblProcessStatus->setStyleSheet(
        "font-size: 12px; color: #606266;"
        );

    if (seriesSpeed) {
        seriesSpeed->clear();
    }

    speedPointIndex = 0;
    updatePatientSelectionUi();
}

void MainWindow::resetOneRoundMeasurementState()
{
    updatePatientSelectionUi();
    currentRoundSosList.clear();
    currentRoundAList.clear();
    currentRoundBList.clear();
    currentRoundCorrAList.clear();
    currentRoundCorrBList.clear();
    currentRoundPairMidGapList.clear();
    currentRoundSignedLagDiffList.clear();

    processValidCount = 0;

    // ✅ 新增：每一轮正式测量开始时，清空最近 lagB 稳定性窗口和门控统计
    resetBoneLagStability();
    resetGateStats();

    ui->barPairA->setValue(500);
    ui->barPairB->setValue(500);

    ui->barMeasureProgress->setRange(0, processValidTarget);
    ui->barMeasureProgress->setValue(0);
    ui->barMeasureProgress->setFormat("有效值：%v / %m");
    ui->barMeasureProgress->setTextVisible(true);

    clearFeedbackReadings();
    if (seriesSpeed) {
        seriesSpeed->clear();
    }

    speedPointIndex = 0;
}

// The caller may have stopped a measurement before resetting this round.
void MainWindow::showRoundFinishedTip(int finishedRounds,
                                      int totalRounds,
                                      bool accepted)
{
    closeRoundFinishedTip();

    measureTipBox = new QMessageBox(this);
    measureTipBox->setIcon(QMessageBox::Information);
    measureTipBox->setWindowTitle(
        accepted ? QStringLiteral("本轮测量完成")
                 : QStringLiteral("本轮未计入"));

    if (accepted) {
        measureTipBox->setText(
            QString("第 %1/%2 轮测量完成。\n\n"
                    "1 秒后将自动开始下一轮；也可点击「立即开始下一轮」。")
                .arg(finishedRounds)
                .arg(totalRounds));
    } else {
        measureTipBox->setText(
            QString("本轮数据未达到要求，未计入结果。\n\n"
                    "请根据界面提示调整后，再次点击「开始检测」。"));
    }

    // 关键 1：不放 OK 按钮，只作为提示窗口
    measureTipBox->setStandardButtons(QMessageBox::NoButton);

    // 关键 2：必须设置为非模态，否则它可能会挡住主窗口，导致你点不了"开始检测"
    measureTipBox->setModal(false);
    measureTipBox->setWindowModality(Qt::NonModal);

    // 关键 3：让它像一个小提示窗口，不要抢占整个应用
    measureTipBox->setWindowFlags(
        measureTipBox->windowFlags()
        | Qt::Tool
        | Qt::WindowStaysOnTopHint
        | Qt::WindowDoesNotAcceptFocus
        );

    measureTipBox->setAttribute(Qt::WA_DeleteOnClose);

    connect(measureTipBox, &QObject::destroyed, this, [this]() {
        measureTipBox = nullptr;
    });

    // 显示在主窗口中间偏上位置，避免挡住"开始检测"按钮
    measureTipBox->show();

    QPoint center = this->geometry().center();
    int x = center.x() - measureTipBox->width() / 2;
    int y = this->geometry().top() + 120;
    measureTipBox->move(x, y);
    ui->btnStartMeasurement->setFocus(Qt::OtherFocusReason);
}

void MainWindow::closeRoundFinishedTip()
{
    if (measureTipBox) {
        QMessageBox *box = measureTipBox;
        measureTipBox = nullptr;

        box->hide();
        box->deleteLater();
    }
}

void MainWindow::handlePatientMeasureValue(double sosA,
                                           double sosB,
                                           double sosAvg,
                                           int lagA,
                                           int lagB,
                                           int diffLag,
                                           double pairMidGap,
                                           double corrA,
                                           double corrB,
                                           bool strictValid)
{
    if (!patientMeasureRunning || acquireMode != PatientMeasureMode) {
        return;
    }

    updateCorrAFeedback(corrA);
    updateProcessPanel(
        sosA,
        sosB,
        lagA,
        lagB,
        diffLag,
        pairMidGap,
        strictValid
        );

    if (!strictValid) {
        return;
    }

    currentRoundSosList.append(sosAvg);
    currentRoundAList.append(sosA);
    currentRoundBList.append(sosB);

    currentRoundCorrAList.append(corrA);
    currentRoundCorrBList.append(corrB);

    // ✅ 新增：保存这一帧的姿态特征
    currentRoundPairMidGapList.append(pairMidGap);
    currentRoundSignedLagDiffList.append(lagA - lagB);

    if (kDebugPerFrame) {
        qDebug() << "Patient frame accepted:"
                 << "count =" << currentRoundSosList.size()
                 << "sosAvg =" << sosAvg
                 << "sosA =" << sosA
                 << "sosB =" << sosB
                 << "lagA =" << lagA
                 << "lagB =" << lagB
                 << "diffLag =" << diffLag
                 << "signedLagDiff =" << (lagA - lagB)
                 << "pairMidGap =" << pairMidGap
                 << "corrA =" << corrA
                 << "corrB =" << corrB;
    }

    if (currentRoundSosList.size() >= processValidTarget) {
        finishOnePatientRound();
    }
}

void MainWindow::finishOnePatientRound()
{
    double oneRoundSos = Utils::trimmedMeanValue(currentRoundSosList, 0.2);
    double oneRoundA   = Utils::trimmedMeanValue(currentRoundAList, 0.2);
    double oneRoundB   = Utils::trimmedMeanValue(currentRoundBList, 0.2);

    double oneRoundCorrA = Utils::trimmedMeanValue(currentRoundCorrAList, 0.2);
    double oneRoundCorrB = Utils::trimmedMeanValue(currentRoundCorrBList, 0.2);

    // ✅ 新增：这一轮的平均姿态特征
    double oneRoundPairMidGap =
        Utils::trimmedMeanValue(currentRoundPairMidGapList, 0.2);

    double oneRoundSignedLagDiff =
        Utils::trimmedMeanValue(currentRoundSignedLagDiffList, 0.2);

    bool oneRoundAngleSignedDiffOk =
        (oneRoundSignedLagDiff >= mCfg.angleSignedDiffMin &&
         oneRoundSignedLagDiff <= mCfg.angleSignedDiffMax);

    bool oneRoundPairMidGapOk =
        (oneRoundPairMidGap >= mCfg.anglePairMidGapMin &&
         oneRoundPairMidGap <= mCfg.anglePairMidGapMax);

    bool oneRoundAngleOk =
        (!enablePatientAngleGate) ||
        (oneRoundAngleSignedDiffOk && oneRoundPairMidGapOk);
    experimentLog.write({{"event", "round_summary"}, {"sos", oneRoundSos},
        {"corr_A", oneRoundCorrA}, {"corr_B", oneRoundCorrB},
        {"D", oneRoundSignedLagDiff}, {"G", oneRoundPairMidGap},
        {"quality_pass", oneRoundCorrB >= mCfg.roundCorrBMin &&
            oneRoundCorrA >= mCfg.roundCorrAMin && oneRoundAngleOk}});

    qDebug() << "One patient round candidate:"
             << "sos =" << oneRoundSos
             << "A =" << oneRoundA
             << "B =" << oneRoundB
             << "corrA =" << oneRoundCorrA
             << "corrB =" << oneRoundCorrB
             << "pairMidGap =" << oneRoundPairMidGap
             << "signedLagDiff =" << oneRoundSignedLagDiff
             << "oneRoundAngleSignedDiffOk =" << oneRoundAngleSignedDiffOk
             << "oneRoundPairMidGapOk =" << oneRoundPairMidGapOk
             << "oneRoundAngleOk =" << oneRoundAngleOk
             << "mCfg.roundCorrAMin =" << mCfg.roundCorrAMin
             << "mCfg.roundCorrBMin =" << mCfg.roundCorrBMin;

    // ======================================================
    // 新增：整轮质量门槛
    //
    // 目的：
    // 即使某些帧勉强通过了单帧 corrOk，
    // 如果整轮平均相关质量不够好，说明这一轮很可能是
    // "稳定但位置不准 / 波形质量差"的测量，直接丢弃。
    // ======================================================
    if (oneRoundCorrB < mCfg.roundCorrBMin ||
        oneRoundCorrA < mCfg.roundCorrAMin ||
        !oneRoundAngleOk) {
        qDebug() << "One patient round rejected by quality:"
                 << "sos =" << oneRoundSos
                 << "corrA =" << oneRoundCorrA
                 << "corrB =" << oneRoundCorrB;
        printGateStats();

        currentRoundSosList.clear();
        currentRoundAList.clear();
        currentRoundBList.clear();
        currentRoundCorrAList.clear();
        currentRoundCorrBList.clear();
        currentRoundPairMidGapList.clear();
        currentRoundSignedLagDiffList.clear();

        processValidCount = 0;
        ui->barMeasureProgress->setValue(0);
        ui->barMeasureProgress->setFormat("有效值：%v / %m");
        ui->barMeasureProgress->setTextVisible(true);

        // 本轮失败后，停止采集，让用户重新点"开始检测"并重新调整探头
        stopPatientMeasurement();

        const int rejectedRound = qMin(roundSosList.size() + 1,
                                       normalMeasureRounds);
        ui->lblProcessStatus->setText(
            QString("第 %1/%2 轮未计入｜数据稳定性不足，请按界面提示调整后重新测量")
                .arg(rejectedRound)
                .arg(normalMeasureRounds));
        ui->lblProcessStatus->setStyleSheet(
            "font-size: 14px; color: #9A5B00; font-weight: bold;"
            );

        showRoundFinishedTip(roundSosList.size(), normalMeasureRounds, false);
        return;
    }

    RoundCandidate cand;
    cand.sos = oneRoundSos;
    cand.a = oneRoundA;
    cand.b = oneRoundB;
    cand.corrA = oneRoundCorrA;
    cand.corrB = oneRoundCorrB;

    candidateRoundList.append(cand);

    // 从所有候选小测量中，重建"最大一致簇"。
    // 离群小测量不会进入 roundSosList。
    Utils::rebuildAcceptedRoundsFromCandidates(candidateRoundList, roundSosList,
        roundAList, roundBList, roundClusterTolerance);

    int finished = roundSosList.size();
    QJsonArray acceptedSos;
    for (double sos : roundSosList) acceptedSos.append(sos);
    experimentLog.write({{"event", "round_cluster"}, {"accepted_sos", acceptedSos},
                         {"candidate_count", candidateRoundList.size()}});
    if (finished >= normalMeasureRounds) {
        double sos=0, a=0, b=0;
        QVector<int> selected;
        if (computeFinalPatientRoundMeans(sos, a, b, &selected)) {
            QJsonArray indices;
            for (int index : selected) indices.append(index);
            experimentLog.write({{"event", "final_round_selection"},
                {"source_accepted_sos", acceptedSos}, {"selected_indices", indices},
                {"final_sos", sos}, {"final_A", a}, {"final_B", b}});
        }
    }

    qDebug() << "One patient round accepted:"
             << "acceptedRoundCount =" << finished
             << "candidateCount =" << candidateRoundList.size()
             << "sos =" << oneRoundSos
             << "A =" << oneRoundA
             << "B =" << oneRoundB
             << "corrA =" << oneRoundCorrA
             << "corrB =" << oneRoundCorrB;
    printGateStats();

    // 当前轮清零，但不清空 roundSosList / candidateRoundList
    currentRoundSosList.clear();
    currentRoundAList.clear();
    currentRoundBList.clear();
    currentRoundCorrAList.clear();
    currentRoundCorrBList.clear();
    currentRoundPairMidGapList.clear();
    currentRoundSignedLagDiffList.clear();

    processValidCount = 0;
    ui->barMeasureProgress->setValue(0);
    ui->barMeasureProgress->setFormat("有效值：%v / %m");

    // 本轮完成后立刻停止采集，恢复"开始检测"按钮
    stopPatientMeasurement();

    // 如果已经完成 5 次，直接生成最终结果
    if (finished >= normalMeasureRounds) {
        finishAllPatientRounds();
        return;
    }

    // 还没满 5 次：先明确停止采集，再留出 1 秒让操作者保持姿势。
    ui->lblProcessStatus->setText(
        QString("第 %1/%2 轮完成｜1 秒后自动开始下一轮")
            .arg(finished)
            .arg(normalMeasureRounds));

    ui->lblProcessStatus->setStyleSheet(
        "font-size: 14px; color: #1D5FA8; font-weight: bold;"
        );

    showRoundFinishedTip(finished, normalMeasureRounds);
    scheduleNextPatientRound(finished);
}

QVector<int> MainWindow::selectFinalRoundIndices(const QVector<double>& values, int target)
{
    if (target <= 0 || values.size() < target) return {};
    QVector<int> order;
    for (int i=0; i<values.size(); ++i) {
        if (!std::isfinite(values[i])) return {};
        order.append(i);
    }
    if (values.size() == target) return order;
    std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
        return values[a] < values[b];
    });
    int bestStart=-1;
    double bestRange=0, bestDeviation=0;
    for (int start=0; start+target<=order.size(); ++start) {
        const double range=values[order[start+target-1]]-values[order[start]];
        const double median=target%2 ? values[order[start+target/2]]
            : .5*(values[order[start+target/2-1]]+values[order[start+target/2]]);
        double deviation=0;
        for (int k=0; k<target; ++k) deviation+=std::abs(values[order[start+k]]-median);
        if (bestStart<0 || range<bestRange || (range==bestRange && deviation<bestDeviation)) {
            bestStart=start; bestRange=range; bestDeviation=deviation;
        }
    }
    auto selected=order.mid(bestStart,target);
    // Preserve acquisition order and the same companion-value indices.
    std::sort(selected.begin(),selected.end());
    return selected;
}

bool MainWindow::computeFinalPatientRoundMeans(double& sos, double& a, double& b,
                                              QVector<int>* selectedIndices) const
{
    if (roundAList.size()!=roundSosList.size() || roundBList.size()!=roundSosList.size()) return false;
    const auto selected=selectFinalRoundIndices(roundSosList,normalMeasureRounds);
    if (selected.size()!=normalMeasureRounds || selected.isEmpty()) return false;
    double sumSos=0,sumA=0,sumB=0;
    for (int index : selected) {
        if (!std::isfinite(roundAList[index]) || !std::isfinite(roundBList[index])) return false;
        sumSos+=roundSosList[index]; sumA+=roundAList[index]; sumB+=roundBList[index];
    }
    sos=sumSos/selected.size(); a=sumA/selected.size(); b=sumB/selected.size();
    if (selectedIndices) *selectedIndices=selected;
    return true;
}

void MainWindow::finishAllPatientRounds()
{
    closeRoundFinishedTip();

    if (hasPendingMeasurement) {
        stopPatientMeasurement();
        ui->lblProcessStatus->setText(
            QStringLiteral("上一次检测结果尚未保存，请先点击“保存结果”。"));
        ui->lblProcessStatus->setStyleSheet(
            "font-size: 14px; color: #9A5B00; font-weight: bold;");
        updatePatientSelectionUi();
        return;
    }

    if (roundSosList.size() < normalMeasureRounds) {
        ui->lblProcessStatus->setText(
            QString("有效小测量不足：%1/%2，请继续测量。")
                .arg(roundSosList.size())
                .arg(normalMeasureRounds)
            );
        ui->lblProcessStatus->setStyleSheet(
            "font-size: 14px; color: #9A5B00; font-weight: bold;"
            );
        return;
    }

    double finalSos=0, finalA=0, finalB=0;
    QVector<int> selectedIndices;
    if (!computeFinalPatientRoundMeans(finalSos, finalA, finalB, &selectedIndices)) {
        stopPatientMeasurement();
        ui->lblProcessStatus->setText(QStringLiteral("测量数据不完整，未生成结果，请重新检测。"));
        return;
    }

    stopPatientMeasurement();

    const QDate birthDate = QDate::fromString(currentPatient.birthDay, "yyyy-MM-dd");
    const int age = birthDate.isValid() ? BoneHealth::calcPatientAge(birthDate) : -1;
    const BoneHealth::DerivedResult derived =
        BoneHealth::deriveResult(finalSos, currentPatient.gender, age);

    currentPatient.checkDate = QDate::currentDate().toString("yyyy-MM-dd");
    currentPatient.speedOfSound = QString::number(finalSos, 'f', 1);
    currentPatient.diagprompt = derived.strength;

    pendingMeasurement = MeasurementRecord();
    pendingMeasurement.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    pendingMeasurement.patientId = currentPatient.id;
    pendingMeasurement.measuredAt = QDateTime::currentDateTime().toString(Qt::ISODate);
    pendingMeasurement.operatorName = currentAccount.username;
    pendingMeasurement.part = QString::fromUtf8("桡骨");
    pendingMeasurement.sos = QString::number(finalSos, 'f', 1);
    pendingMeasurement.tScore = derived.tScore;
    pendingMeasurement.zScore = derived.zScore;
    pendingMeasurement.diagnosis = derived.diagnosis;
    pendingMeasurement.patientName = currentPatient.name;
    pendingMeasurement.patientGender = currentPatient.gender;
    pendingMeasurement.patientBirthDay = currentPatient.birthDay;
    pendingMeasurement.patientHeight = currentPatient.height;
    pendingMeasurement.patientWeight = currentPatient.weight;
    pendingMeasurement.patientAge = age >= 0 ? QString::number(age) : QString();
    pendingMeasurement.boneStrength = derived.strength;
    pendingMeasurement.fractureRisk = derived.fractureRisk;
    pendingMeasurement.boneAge = derived.boneAge;
    hasPendingMeasurement = true;
    const MeasurementRecord completedMeasurement = pendingMeasurement;
    QList<MeasurementRecord> savedMeasurements = measurementList;
    savedMeasurements.append(pendingMeasurement);
    if (saveMeasurements(savedMeasurements)) {
        measurementList = savedMeasurements;
        hasPendingMeasurement = false;
        pendingMeasurement = MeasurementRecord();
    }
    refreshPatientDerivedViews();

    ui->lblProcessStatus->setText(
        QString("5 次测量完成：最终 SOS=%1 m/s")
            .arg(finalSos, 0, 'f', 1)
        );
    ui->lblProcessStatus->setStyleSheet(
        "font-size: 14px; color: #1B7A4B; font-weight: bold;"
        );

    showPatientMeasureFinishedDialog(completedMeasurement);

    qDebug() << "All patient rounds finished:"
             << "rounds =" << selectedIndices.size()
             << "sourceRounds =" << roundSosList.size()
             << "selectedIndices =" << selectedIndices
             << "finalSos =" << finalSos
             << "finalA =" << finalA
             << "finalB =" << finalB
             << "T =" << derived.tScore
             << "Z =" << derived.zScore
             << "strength =" << derived.strength
             << "risk =" << derived.fractureRisk
             << "boneAge =" << derived.boneAge;
}

void MainWindow::showPatientMeasureFinishedDialog(
    const MeasurementRecord& completedMeasurement)
{
    if (hasPendingMeasurement) {
        QMessageBox::warning(this, "结果保存失败",
                             "本次报表已经生成，但检测结果尚未保存，请返回主界面在“测量结果”中重试保存。");
    }
    showReportFrom(ui->pageMain, currentPatient, completedMeasurement);
}

void MainWindow::printRangeFormatted(const QString& name,
                                     const QVector<double>& data,
                                     int start, int end) const
{
    if (data.isEmpty()) return;

    int n = data.size();
    start = qMax(0, start);
    end   = qMin(end, n - 1);
    if (start > end) return;

    qDebug().noquote() << "";
    qDebug().noquote() << "======== " + name +
                              QString(" [%1..%2] ========").arg(start).arg(end);

    for (int base = start; base <= end; base += 25) {
        if ((base - start) % 120 == 0) {
            qDebug().noquote() << QString("---- [%1] ----").arg(base);
        }

        QString line = QString("[%1] ").arg(base, 4, 10, QLatin1Char('0'));
        int lineEnd = qMin(base + 24, end);

        for (int i = base; i <= lineEnd; ++i) {
            line += QString("%1 ").arg(data[i] + 2048.0, 8, 'f', 2);
        }

        qDebug().noquote() << line;
    }

    qDebug().noquote() << "======================================";
}



void MainWindow::appendAngleFeatureCsv(const QString& mode,
                                       double sosAvg,
                                       double sosA,
                                       double sosB,
                                       int lagA,
                                       int lagB,
                                       int diffLag,
                                       int signedLagDiff,
                                       double wB,
                                       double corrA,
                                       double corrB,
                                       int bcOnset,
                                       int bdOnset,
                                       int acOnset,
                                       int adOnset,
                                       int bcPeak,
                                       int bdPeak,
                                       int acPeak,
                                       int adPeak,
                                       int bcValley,
                                       int bdValley,
                                       int acValley,
                                       int adValley,
                                       int valleyLagB,
                                       int valleyLagA,
                                       double pairMidB,
                                       double pairMidA,
                                       double pairMidGap,
                                       double onsetMidB,
                                       double onsetMidA,
                                       double onsetMidGap,
                                       double peakMidB,
                                       double peakMidA,
                                       double peakMidGap,
                                       double valleyMidB,
                                       double valleyMidA,
                                       double valleyMidGap,
                                       double depthBC,
                                       double depthBD,
                                       double depthAC,
                                       double depthAD,
                                       double depthRatioBCBD,
                                       double depthRatioACAD,
                                       double depthRatioAB,
                                       double expectedOffset,
                                       double offsetResidual) const
{
    QString path = QCoreApplication::applicationDirPath() + "/angle_features.csv";

    QFile file(path);

    bool needHeader = true;

    if (file.exists() && file.size() > 0) {
        needHeader = false;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qDebug() << "AngleFeature CSV open failed:" << path;
        return;
    }

    QTextStream out(&file);

    if (needHeader) {
        out << "time,mode,"
            << "sosAvg,sosA,sosB,"
            << "lagA,lagB,diffLag,signedLagDiff,wB,corrA,corrB,"
            << "bcOnset,bdOnset,acOnset,adOnset,"
            << "bcPeak,bdPeak,acPeak,adPeak,"
            << "bcValley,bdValley,acValley,adValley,"
            << "valleyLagB,valleyLagA,"
            << "pairMidB,pairMidA,pairMidGap,"
            << "onsetMidB,onsetMidA,onsetMidGap,"
            << "peakMidB,peakMidA,peakMidGap,"
            << "valleyMidB,valleyMidA,valleyMidGap,"
            << "depthBC,depthBD,depthAC,depthAD,"
            << "depthRatioBCBD,depthRatioACAD,depthRatioAB,"
            << "expectedOffset,offsetResidual"
            << "\n";
    }

    out << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz") << ","
        << mode << ","
        << sosAvg << ","
        << sosA << ","
        << sosB << ","
        << lagA << ","
        << lagB << ","
        << diffLag << ","
        << signedLagDiff << ","
        << wB << ","
        << corrA << ","
        << corrB << ","
        << bcOnset << ","
        << bdOnset << ","
        << acOnset << ","
        << adOnset << ","
        << bcPeak << ","
        << bdPeak << ","
        << acPeak << ","
        << adPeak << ","
        << bcValley << ","
        << bdValley << ","
        << acValley << ","
        << adValley << ","
        << valleyLagB << ","
        << valleyLagA << ","
        << pairMidB << ","
        << pairMidA << ","
        << pairMidGap << ","
        << onsetMidB << ","
        << onsetMidA << ","
        << onsetMidGap << ","
        << peakMidB << ","
        << peakMidA << ","
        << peakMidGap << ","
        << valleyMidB << ","
        << valleyMidA << ","
        << valleyMidGap << ","
        << depthBC << ","
        << depthBD << ","
        << depthAC << ","
        << depthAD << ","
        << depthRatioBCBD << ","
        << depthRatioACAD << ","
        << depthRatioAB << ","
        << expectedOffset << ","
        << offsetResidual
        << "\n";

    file.close();
}

void MainWindow::printAngleFeatureDebug(const QVector<double>& filBC,
                                        const QVector<double>& filBD,
                                        const QVector<double>& filAC,
                                        const QVector<double>& filAD,
                                        const ArrivalResult& pickBC,
                                        const ArrivalResult& pickBD,
                                        const ArrivalResult& pickAC,
                                        const ArrivalResult& pickAD,
                                        const PairResult& bRes,
                                        const PairResult& aRes,
                                        double sosAvg,
                                        double wB) const
{
    int n = std::min(
        std::min(filBC.size(), filBD.size()),
        std::min(filAC.size(), filAD.size())
        );

    if (n <= 0) {
        return;
    }

    // ======================================================
    // 1. 四路统一找"第一个明显波谷"
    //
    // 注意：这里暂时只用于调试，不参与正式结果。
    // 搜索范围先用 620~1400，覆盖铜块、塑料、桡骨主要第一波区域。
    // ======================================================
    int valleySearchStart = 620;
    int valleySearchEnd = qMin(1400, n - 2);

    ValleyResult vBC = SignalProcessor::findFirstProminentValley(
        filBC,
        valleySearchStart,
        valleySearchEnd,
        25.0,
        0.25,
        20
        );

    ValleyResult vBD = SignalProcessor::findFirstProminentValley(
        filBD,
        valleySearchStart,
        valleySearchEnd,
        25.0,
        0.25,
        20
        );

    ValleyResult vAC = SignalProcessor::findFirstProminentValley(
        filAC,
        valleySearchStart,
        valleySearchEnd,
        25.0,
        0.25,
        20
        );

    ValleyResult vAD = SignalProcessor::findFirstProminentValley(
        filAD,
        valleySearchStart,
        valleySearchEnd,
        25.0,
        0.25,
        20
        );

    auto idxOrInvalid = [](const ValleyResult& v) -> int {
        return v.valid ? v.idx : -1;
    };

    auto depthOrZero = [](const ValleyResult& v) -> double {
        return v.valid ? v.depth : 0.0;
    };

    int bcValley = idxOrInvalid(vBC);
    int bdValley = idxOrInvalid(vBD);
    int acValley = idxOrInvalid(vAC);
    int adValley = idxOrInvalid(vAD);

    double depthBC = depthOrZero(vBC);
    double depthBD = depthOrZero(vBD);
    double depthAC = depthOrZero(vAC);
    double depthAD = depthOrZero(vAD);

    // ======================================================
    // 2. 当前算法实际使用的 pair 特征
    //
    // B_pair: early = BD, late = BC
    // A_pair: early = AD, late = AC
    //
    // bRes.earlyOnset / lateOnset 对 B 来说是 BD / BC
    // aRes.earlyOnset / lateOnset 对 A 来说是 AD / AC
    // ======================================================
    double pairMidB = 0.5 * (bRes.earlyOnset + bRes.lateOnset);
    double pairMidA = 0.5 * (aRes.earlyOnset + aRes.lateOnset);
    double pairMidGap = pairMidB - pairMidA;

    // ======================================================
    // 3. onset 中心关系
    // ======================================================
    int onsetLagB = pickBC.onset - pickBD.onset;
    int onsetLagA = pickAC.onset - pickAD.onset;

    double onsetMidB = 0.5 * (pickBD.onset + pickBC.onset);
    double onsetMidA = 0.5 * (pickAD.onset + pickAC.onset);
    double onsetMidGap = onsetMidB - onsetMidA;

    // ======================================================
    // 4. peak 中心关系
    // ======================================================
    int peakLagB = pickBC.peak - pickBD.peak;
    int peakLagA = pickAC.peak - pickAD.peak;

    double peakMidB = 0.5 * (pickBD.peak + pickBC.peak);
    double peakMidA = 0.5 * (pickAD.peak + pickAC.peak);
    double peakMidGap = peakMidB - peakMidA;

    // ======================================================
    // 5. valley 中心关系
    // ======================================================
    int valleyLagB = -999;
    int valleyLagA = -999;

    double valleyMidB = -999.0;
    double valleyMidA = -999.0;
    double valleyMidGap = -999.0;

    if (vBC.valid && vBD.valid) {
        valleyLagB = vBC.idx - vBD.idx;
        valleyMidB = 0.5 * (vBD.idx + vBC.idx);
    }

    if (vAC.valid && vAD.valid) {
        valleyLagA = vAC.idx - vAD.idx;
        valleyMidA = 0.5 * (vAD.idx + vAC.idx);
    }

    if (vBC.valid && vBD.valid && vAC.valid && vAD.valid) {
        valleyMidGap = valleyMidB - valleyMidA;
    }

    // ======================================================
    // 6. A/B lag 系统偏差
    //
    // 先用一个很粗的经验公式：
    // 塑料 lagB≈202, offset≈3
    // 铜块 lagB≈105, offset≈6
    // 粗略得到 expectedOffset ≈ 8.5 - 0.028 * lagB
    //
    // 这只是调试用，不要现在加入正式判定。
    // ======================================================
    int signedLagDiff = aRes.refinedLag - bRes.refinedLag;
    int diffLag = std::abs(signedLagDiff);

    double expectedOffset = 8.5 - 0.028 * bRes.refinedLag;
    double offsetResidual = signedLagDiff - expectedOffset;

    // ======================================================
    // 7. 深度比例
    // ======================================================
    double depthRatioBCBD = Utils::safeRatio(depthBC, depthBD);
    double depthRatioACAD = Utils::safeRatio(depthAC, depthAD);

    double bDepthMean = 0.5 * (depthBC + depthBD);
    double aDepthMean = 0.5 * (depthAC + depthAD);

    double depthRatioAB = Utils::safeRatio(aDepthMean, bDepthMean);

    QString mode = (acquireMode == PatientMeasureMode) ? "Patient" : "Debug";

    // ======================================================
    // 8. qDebug 输出
    // ======================================================
    qDebug() << "AngleFeature_PAIR:"
             << "mode =" << mode
             << "sosAvg =" << sosAvg
             << "sosA =" << aRes.sos
             << "sosB =" << bRes.sos
             << "lagA =" << aRes.refinedLag
             << "lagB =" << bRes.refinedLag
             << "signedLagDiff =" << signedLagDiff
             << "diffLag =" << diffLag
             << "wB =" << wB
             << "corrA =" << aRes.corr
             << "corrB =" << bRes.corr
             << "pairMidB =" << pairMidB
             << "pairMidA =" << pairMidA
             << "pairMidGap =" << pairMidGap;

    qDebug() << "AngleFeature_ONSET_PEAK:"
             << "BC.onset =" << pickBC.onset
             << "BD.onset =" << pickBD.onset
             << "AC.onset =" << pickAC.onset
             << "AD.onset =" << pickAD.onset
             << "onsetLagB =" << onsetLagB
             << "onsetLagA =" << onsetLagA
             << "onsetMidB =" << onsetMidB
             << "onsetMidA =" << onsetMidA
             << "onsetMidGap =" << onsetMidGap
             << "peakLagB =" << peakLagB
             << "peakLagA =" << peakLagA
             << "peakMidGap =" << peakMidGap;

    qDebug() << "AngleFeature_VALLEY:"
             << "BC.valley =" << bcValley
             << "BD.valley =" << bdValley
             << "AC.valley =" << acValley
             << "AD.valley =" << adValley
             << "valleyLagB =" << valleyLagB
             << "valleyLagA =" << valleyLagA
             << "valleyMidB =" << valleyMidB
             << "valleyMidA =" << valleyMidA
             << "valleyMidGap =" << valleyMidGap
             << "depthBC =" << depthBC
             << "depthBD =" << depthBD
             << "depthAC =" << depthAC
             << "depthAD =" << depthAD
             << "depthRatioBCBD =" << depthRatioBCBD
             << "depthRatioACAD =" << depthRatioACAD
             << "depthRatioAB =" << depthRatioAB;

    qDebug() << "AngleFeature_OFFSET:"
             << "lagB =" << bRes.refinedLag
             << "signedLagDiff =" << signedLagDiff
             << "expectedOffset =" << expectedOffset
             << "offsetResidual =" << offsetResidual;

    // ======================================================
    // 9. 保存到 CSV
    // ======================================================
    appendAngleFeatureCsv(
        mode,
        sosAvg,
        aRes.sos,
        bRes.sos,
        aRes.refinedLag,
        bRes.refinedLag,
        diffLag,
        signedLagDiff,
        wB,
        aRes.corr,
        bRes.corr,
        pickBC.onset,
        pickBD.onset,
        pickAC.onset,
        pickAD.onset,
        pickBC.peak,
        pickBD.peak,
        pickAC.peak,
        pickAD.peak,
        bcValley,
        bdValley,
        acValley,
        adValley,
        valleyLagB,
        valleyLagA,
        pairMidB,
        pairMidA,
        pairMidGap,
        onsetMidB,
        onsetMidA,
        onsetMidGap,
        peakMidB,
        peakMidA,
        peakMidGap,
        valleyMidB,
        valleyMidA,
        valleyMidGap,
        depthBC,
        depthBD,
        depthAC,
        depthAD,
        depthRatioBCBD,
        depthRatioACAD,
        depthRatioAB,
        expectedOffset,
        offsetResidual
        );
}

void MainWindow::detectAndPlotSpeed(const QVector<double>& filBC,
                                    const QVector<double>& filBD,
                                    const QVector<double>& filAC,
                                    const QVector<double>& filAD)
{
    const bool recording = patientMeasureRunning && acquireMode == PatientMeasureMode
                           && experimentLog.active();
    QElapsedTimer processingTimer;
    processingTimer.start();
    QJsonObject evidence{{"event", "frame"}, {"decision", "empty_filtered_input"}};
    const auto record = [&]() {
        if (!recording) return;
        evidence["processing_ms"] = processingTimer.elapsed();
        evidence["raw_BC"] = MeasurementExperimentLog::encodeRaw(samplesA);
        evidence["raw_BD"] = MeasurementExperimentLog::encodeRaw(samplesB);
        evidence["raw_AC"] = MeasurementExperimentLog::encodeRaw(samplesC);
        evidence["raw_AD"] = MeasurementExperimentLog::encodeRaw(samplesD);
        evidence["partial_values_before_accept"] = processValidCount;
        evidence["locked"] = boneLagLocked;
        evidence["locked_lag"] = lockedBoneLagCenter;
        experimentLog.write(evidence);
        checkExperimentLogError();
    };
    auto recordOnReturn = qScopeGuard(record);
    const auto pairEvidence = [](const PairResult& pair) {
        return QJsonObject{{"valid", pair.valid}, {"rough_lag", pair.roughLag},
            {"lag", pair.refinedLag}, {"sos", pair.sos}, {"corr", pair.corr},
            {"early_feature", pair.earlyOnset}, {"late_feature", pair.lateOnset}};
    };
    int n = std::min(
        std::min(filBC.size(), filBD.size()),
        std::min(filAC.size(), filAD.size())
        );

    if (n <= 0) {
        setSpeedDebugInvalid("滤波数据为空");

        if (patientMeasureRunning && acquireMode == PatientMeasureMode) {
            updateProcessInvalid("滤波数据为空");
        }

        return;
    }

    // ======================================================
    // 1. 首波粗定位参数
    // ======================================================
    int noiseStart = 560;
    int noiseEnd   = 615;

    int searchStart = 620;
    int searchEnd = qMin(n - 1, 1600);

    double kSigma = 4.0;
    int runLen = 8;
    int envWin = 20;
    double onsetRatio = 0.20;
    int peakLookAhead = 140;

    ArrivalResult pickBC = SignalProcessor::detectFirstArrivalSmart(
        filBC, noiseStart, noiseEnd, searchStart, searchEnd,
        kSigma, runLen, envWin, onsetRatio, peakLookAhead
        );

    ArrivalResult pickBD = SignalProcessor::detectFirstArrivalSmart(
        filBD, noiseStart, noiseEnd, searchStart, searchEnd,
        kSigma, runLen, envWin, onsetRatio, peakLookAhead
        );

    ArrivalResult pickAC = SignalProcessor::detectFirstArrivalSmart(
        filAC, noiseStart, noiseEnd, searchStart, searchEnd,
        kSigma, runLen, envWin, onsetRatio, peakLookAhead
        );

    ArrivalResult pickAD = SignalProcessor::detectFirstArrivalSmart(
        filAD, noiseStart, noiseEnd, searchStart, searchEnd,
        kSigma, runLen, envWin, onsetRatio, peakLookAhead
        );

    if (kDebugPerFrame) {
        qDebug() << "FirstArrivalSmart:"
                 << "BC.onset =" << pickBC.onset << "BC.peak =" << pickBC.peak
                 << "BD.onset =" << pickBD.onset << "BD.peak =" << pickBD.peak
                 << "AC.onset =" << pickAC.onset << "AC.peak =" << pickAC.peak
                 << "AD.onset =" << pickAD.onset << "AD.peak =" << pickAD.peak;
    }

    if (acquireMode == CalibrationAcquireMode) {
        processCalibrationFrame(filBC, filBD, filAC, filAD,
                                pickBC, pickBD, pickAC, pickAD);
        return;
    }

    const auto arrivalEvidence = [](const ArrivalResult& pick) {
        return QJsonObject{{"valid", pick.valid}, {"first_hit", pick.firstHit},
            {"onset", pick.onset}, {"peak", pick.peak}, {"threshold", pick.threshold},
            {"forward_shift", pick.valid ? QJsonValue(pick.onset-pick.firstHit) : QJsonValue()}};
    };
    evidence["B_arrivals"] = QJsonObject{{"BD", arrivalEvidence(pickBD)},
                                          {"BC", arrivalEvidence(pickBC)}};
    const bool inconsistentBOnset =
        (pickBD.valid && !pickBD.onsetConsistent(bOnsetForwardLimit)) ||
        (pickBC.valid && !pickBC.onsetConsistent(bOnsetForwardLimit));
    if (patientMeasureRunning && acquireMode == PatientMeasureMode &&
        enforceBOnsetConsistency && inconsistentBOnset) {
        evidence["decision"] = "B_onset_inconsistent";
        evidence["B_onset_forward_limit"] = bOnsetForwardLimit;
        const QString reason = QStringLiteral("首波定位不一致，本帧未计入；请重新贴合探头");
        setSpeedDebugInvalid(reason);
        updateProcessInvalid(reason);
        return;
    }

    double vMin = 1800.0;
    double vMax = 5000.0;

    // ======================================================
    // 2. 先算 B_pair：BD -> BC
    // ======================================================
    PairResult bRes = signalProcessor.estimatePairSpeed(
        filBD,
        filBC,
        pickBD,
        pickBC,
        vMin,
        vMax,
        "B_pair / BD->BC",
        -1,
        -1,
        completeTruncatedBPeak ? 15 : 0
        );
    evidence["B"] = pairEvidence(bRes);
    evidence["decision"] = "B_pair_invalid";

    if (!bRes.valid) {
        qDebug() << "Skip: B_pair invalid, because B_pair is the reference pair";

        setSpeedDebugInvalid("B_pair 无效");

        if (patientMeasureRunning && acquireMode == PatientMeasureMode) {
            updateProcessInvalid("B_pair 无效，无法作为参考");
        }

        return;
    }

    // ======================================================
    // B_pair 质量检查：防止从第一波粗定位跳到后面的波包
    // ======================================================
    int bLagJump = std::abs(bRes.refinedLag - bRes.roughLag);

    // 这次桡骨正确簇：B_jump 大约 10~34
    // 错误 2400 簇：B_jump 大约 109~115
    // 所以 70 是一个比较安全的分界
    int bLagJumpLimit = 70;

    if (bLagJump > bLagJumpLimit) {
        evidence["decision"] = "B_lag_jump";
        qDebug() << "Skip: B_pair jump too large"
                 << "roughLag =" << bRes.roughLag
                 << "refinedLag =" << bRes.refinedLag
                 << "jump =" << bLagJump
                 << "limit =" << bLagJumpLimit;

        setSpeedDebugInvalid(
            QString("B_pair 跳变过大 rough=%1 refined=%2")
                .arg(bRes.roughLag)
                .arg(bRes.refinedLag)
            );

        if (patientMeasureRunning && acquireMode == PatientMeasureMode) {
            updateProcessInvalid("B_pair 跳变过大，疑似选到后波包");
        }

        return;
    }

    // ======================================================
    // 3. 再算 A_pair：AD -> AC
    // ======================================================
    int lagToleranceAB = 20;

    PairResult aRes = signalProcessor.estimatePairSpeedByValley(
        filAD,
        filAC,
        pickAD,
        bRes.refinedLag,
        lagToleranceAB,
        "A_pair / AD->AC / valley"
        );
    evidence["A_feature_branch"] = aRes.valid ? "valley" : "envelope_fallback";
    if (useDualWindowAQuality && aRes.valid) {
        double front=0,middle=0;
        const double original=aRes.corr;
        aRes.corr=dualWindowAQuality(filAD,filAC,aRes.earlyOnset,aRes.refinedLag,&front,&middle);
        evidence["A_quality_trial"] = QJsonObject{{"original_corr",original},
            {"front_corr",front},{"middle_corr",middle},{"common_lag",aRes.refinedLag}};
    }

    if (!aRes.valid) {
        int forcedMin = qMax(1, bRes.refinedLag - lagToleranceAB);
        int forcedMax = bRes.refinedLag + lagToleranceAB;

        qDebug() << "A_pair valley failed, fallback to constrained corr"
                 << "forced range = [" << forcedMin << "," << forcedMax << "]";

        aRes = signalProcessor.estimatePairSpeed(
            filAD,
            filAC,
            pickAD,
            pickAC,
            vMin,
            vMax,
            "A_pair / AD->AC / constrained",
            forcedMin,
            forcedMax
            );
    }

    evidence["A"] = pairEvidence(aRes);
    evidence["decision"] = "A_pair_invalid";
    if (!aRes.valid) {
        qDebug() << "Skip: A_pair invalid";

        setSpeedDebugInvalid("A_pair 无效");

        if (patientMeasureRunning && acquireMode == PatientMeasureMode) {
            updateProcessInvalid("A_pair 无效");
        }

        return;
    }

    // ======================================================
    // 4. A/B 一致性判断
    // ======================================================
    int diffLag = std::abs(aRes.refinedLag - bRes.refinedLag);

    // ======================================================
    // 新增：提前计算 pairMidGap
    //
    // 注意：这里要放在 diffLag > lagToleranceAB 判断之前。
    // 因为即使 A/B 差异过大，我们仍然希望竖向平衡条能显示角度偏向。
    // ======================================================
    double pairMidB = 0.5 * (bRes.earlyOnset + bRes.lateOnset);
    double pairMidA = 0.5 * (aRes.earlyOnset + aRes.lateOnset);
    double pairMidGap = pairMidB - pairMidA;
    evidence["D"] = aRes.refinedLag - bRes.refinedLag;
    evidence["G"] = pairMidGap;

    if (diffLag > lagToleranceAB) {
        evidence["decision"] = "AB_difference";
        qDebug() << "Skip: A_pair and B_pair inconsistent"
                 << "lagA =" << aRes.refinedLag
                 << "lagB =" << bRes.refinedLag
                 << "diff =" << diffLag
                 << "tolerance =" << lagToleranceAB
                 << "pairMidGap =" << pairMidGap;

        setSpeedDebugInvalid(
            QString("A/B 差异过大，diff=%1").arg(diffLag)
            );

        // 调试模式：中间检查过程区保持不动
        // 病人检测模式：显示一高一低，但不计入有效值
        if (patientMeasureRunning && acquireMode == PatientMeasureMode) {
            rejectBoneLagCandidate();
            updateCorrAFeedback(aRes.corr);
            updateProcessPanel(
                aRes.sos,
                bRes.sos,
                aRes.refinedLag,
                bRes.refinedLag,
                diffLag,
                pairMidGap,
                false
                );
        }

        return;
    }

    // ======================================================
    // 5. 计算声速
    //
    // sosWeighted：原来的 A/B 加权值，保留用于调试观察。
    // sosPatient ：正式测量真正采用的值。
    //
    // 当前建议：
    // A 通道主要用于姿态门控；
    // B 通道作为正式 SOS 主测量值。
    // ======================================================
    double wB = 0.75;

    if (aRes.refinedLag > bRes.refinedLag + 3) {
        wB = 0.8;
    }

    if (std::abs(aRes.refinedLag - bRes.refinedLag) <= 2) {
        wB = 0.7;
    }

    double sosWeighted = wB * bRes.sos + (1.0 - wB) * aRes.sos;

    double sosPatient = sosWeighted;

    if (useBOnlyForPatientSos) {
        sosPatient = bRes.sos;
    }

    // 预留最终校准偏移，先默认为 0
    sosPatient += patientSosOffset;

    // 为了尽量少改后面的代码，继续用 sosAvg 这个变量名，
    // 但它现在表示"正式采用的 SOS"。
    double sosAvg = sosPatient;

    if (kDebugPerFrame) {
        qDebug() << "SpeedFinal:"
                 << "sosA =" << aRes.sos
                 << "sosB =" << bRes.sos
                 << "weighted =" << sosWeighted
                 << "patient =" << sosPatient
                 << "useBOnly =" << useBOnlyForPatientSos
                 << "offset =" << patientSosOffset
                 << "lagA =" << aRes.refinedLag
                 << "lagB =" << bRes.refinedLag
                 << "diffLag =" << diffLag
                 << "wB =" << wB
                 << "mode =" << (acquireMode == PatientMeasureMode ? "Patient" : "Debug");
    }

    // ======================================================
    // 角度/姿态特征调试输出（逐帧大流量，默认关闭）
    // 只用于研究 A/B 绝对位置关系，不影响正式测量结果。
    // ======================================================
    if (kDebugPerFrame) {
        printAngleFeatureDebug(
            filBC,
            filBD,
            filAC,
            filAD,
            pickBC,
            pickBD,
            pickAC,
            pickAD,
            bRes,
            aRes,
            sosAvg,
            wB
            );
    }

    // ======================================================
    // 6. 声速调试值显示：调试模式和病人模式都显示
    // ======================================================
    updateSpeedDebugPanel(
        aRes.sos,
        bRes.sos,
        sosAvg,
        aRes.refinedLag,
        bRes.refinedLag,
        diffLag,
        aRes.corr,
        bRes.corr
        );

    // ======================================================
    // 7. 声速曲线：调试模式和病人模式都显示
    // ======================================================
    appendSpeedPoint(sosAvg);

    // ======================================================
    // 8. 中间检查过程区：
    // 只有病人检测模式才更新
    // trigger / 获取波形调试模式下保持不动
    // ======================================================
    if (patientMeasureRunning && acquireMode == PatientMeasureMode) {

        // ======================================================
        // 1. 基础质量判定
        // ======================================================
        int bLagJump = std::abs(bRes.refinedLag - bRes.roughLag);

        bool bJumpOk = (bLagJump <= 70);
        bool notBoundary = (bRes.refinedLag < 260);

        int signedLagDiff = aRes.refinedLag - bRes.refinedLag;
        int diffLagForCheck = std::abs(signedLagDiff);

        bool diffOk = (diffLagForCheck <= 18);
        bool directionOk = (signedLagDiff >= -2);

        // 注意：
        // B_corr 现在只作为底线，不再作为主要姿态判据。
        bool corrOk = (bRes.corr >= mCfg.frameCorrBMin && aRes.corr >= mCfg.frameCorrAMin);


        // ======================================================
        // 2. 新增：A/B 绝对位置关系，也就是角度判定
        //
        // pairMidB = B组两个特征点中心
        // pairMidA = A组两个特征点中心
        // pairMidGap = pairMidB - pairMidA
        //
        // 这次 CSV 显示：
        // 3600 错误角度：signedLagDiff≈5,  pairMidGap≈27
        // 3800 正确角度：signedLagDiff≈10, pairMidGap≈6.5
        // 4000 错误角度：signedLagDiff≈1,  pairMidGap≈-4.5
        // ======================================================
        double pairMidB = 0.5 * (bRes.earlyOnset + bRes.lateOnset);
        double pairMidA = 0.5 * (aRes.earlyOnset + aRes.lateOnset);
        double pairMidGap = pairMidB - pairMidA;

        bool angleSignedDiffOk =
            (signedLagDiff >= mCfg.angleSignedDiffMin &&
             signedLagDiff <= mCfg.angleSignedDiffMax);

        bool anglePairMidGapOk =
            (pairMidGap >= mCfg.anglePairMidGapMin &&
             pairMidGap <= mCfg.anglePairMidGapMax);

        bool angleOk =
            (!enablePatientAngleGate) ||
            (angleSignedDiffOk && anglePairMidGapOk);


        // ======================================================
        // 3. 稳定 lag 簇判定
        //
        // 只有通过基础质量 + 姿态判定的帧，才允许进入稳定窗口。
        // 否则会污染 stableLag 窗口。
        // ======================================================
        int stableCenter = 0;
        int stableCount = 0;
        bool stableOk = false;

        // The isolated trial observes current lag stability before the G gate.
        // G still gates strictValid below; no G-failing frame enters results.
        const bool basePrechecksOk = bJumpOk && notBoundary && diffOk && directionOk &&
            corrOk && ((!enablePatientAngleGate) || angleSignedDiffOk);
        const bool stabilityInputOk = basePrechecksOk && (observeStabilityBeforeG || angleOk);
        if (stabilityInputOk) {

            stableOk = checkBoneLagStable(
                bRes.refinedLag,
                &stableCenter,
                &stableCount
                );

        } else {
            rejectBoneLagCandidate();
            if (kDebugPerFrame) {
                qDebug() << "BoneLagStable: current frame not added because pre-check failed"
                         << "bJumpOk =" << bJumpOk
                         << "notBoundary =" << notBoundary
                         << "diffOk =" << diffOk
                         << "directionOk =" << directionOk
                         << "corrOk =" << corrOk
                         << "angleOk =" << angleOk
                         << "angleSignedDiffOk =" << angleSignedDiffOk
                         << "anglePairMidGapOk =" << anglePairMidGapOk
                         << "signedLagDiff =" << signedLagDiff
                         << "pairMidGap =" << pairMidGap
                         << "corrA =" << aRes.corr
                         << "corrB =" << bRes.corr;
            }
        }


        // ======================================================
        // 4. 最终正式有效帧判定
        // ======================================================
        bool strictValid =
            bJumpOk &&
            notBoundary &&
            diffOk &&
            directionOk &&
            corrOk &&
            angleOk &&
            stableOk;
        evidence["decision"] = strictValid ? "accepted" : "rejected";
        evidence["sos_patient"] = sosAvg;
        evidence["gates"] = QJsonObject{{"B_jump", bJumpOk}, {"boundary", notBoundary},
            {"AB_diff", diffOk}, {"direction", directionOk},
            {"corr_A", aRes.corr >= mCfg.frameCorrAMin},
            {"corr_B", bRes.corr >= mCfg.frameCorrBMin},
            {"D", angleSignedDiffOk}, {"G", anglePairMidGapOk},
            {"stability_evaluated", stabilityInputOk},
            {"stable", stableOk}};
        if (observeStabilityBeforeG) {
            auto gates = evidence["gates"].toObject();
            gates["all_prechecks_passed"] = basePrechecksOk && angleOk;
            evidence["gates"] = gates;
        }


        if (kDebugPerFrame) {
            qDebug() << "PatientValidCheck:"
                     << "lagA =" << aRes.refinedLag
                     << "lagB =" << bRes.refinedLag
                     << "diffLag =" << diffLagForCheck
                     << "signedLagDiff =" << signedLagDiff
                     << "pairMidB =" << pairMidB
                     << "pairMidA =" << pairMidA
                     << "pairMidGap =" << pairMidGap
                     << "angleSignedDiffRange = ["
                     << mCfg.angleSignedDiffMin << "," << mCfg.angleSignedDiffMax << "]"
                     << "anglePairMidGapRange = ["
                     << mCfg.anglePairMidGapMin << "," << mCfg.anglePairMidGapMax << "]"
                     << "bRoughLag =" << bRes.roughLag
                     << "bJump =" << bLagJump
                     << "corrA =" << aRes.corr
                     << "corrB =" << bRes.corr
                     << "mCfg.frameCorrAMin =" << mCfg.frameCorrAMin
                 << "mCfg.frameCorrBMin =" << mCfg.frameCorrBMin
                 << "bJumpOk =" << bJumpOk
                 << "notBoundary =" << notBoundary
                 << "diffOk =" << diffOk
                 << "directionOk =" << directionOk
                 << "corrOk =" << corrOk
                 << "angleSignedDiffOk =" << angleSignedDiffOk
                 << "anglePairMidGapOk =" << anglePairMidGapOk
                 << "angleOk =" << angleOk
                 << "stableCenter =" << stableCenter
                 << "stableCount =" << stableCount
                 << "boneLagLocked =" << boneLagLocked
                 << "lockedBoneLagCenter =" << lockedBoneLagCenter
                 << "stableOk =" << stableOk
                 << "strictValid =" << strictValid;
        }


        // ======================================================
        // 门控拒绝率统计（不改检验逻辑，纯诊断用）
        // ======================================================
        // 脱耦过滤：corr<0.25 认为探头悬空/无耦合，不计入闸门统计
        if (bRes.corr < 0.25) {
            gateDecoupledFrames++;
        } else {
            gateTotalFrames++;

            // 跟踪 CorrA 分布（诊断用）
            if (gateTotalFrames == 1) {
                gateCorrAMin = aRes.corr;
                gateCorrAMax = aRes.corr;
                gateCorrASum = aRes.corr;
            } else {
                if (aRes.corr < gateCorrAMin) gateCorrAMin = aRes.corr;
                if (aRes.corr > gateCorrAMax) gateCorrAMax = aRes.corr;
                gateCorrASum += aRes.corr;
            }

            // 记录当前帧各闸门状态
            lastFrameBJumpOk = bJumpOk;
            lastFrameBoundaryOk = notBoundary;
            lastFrameDiffOk = diffOk;
            lastFrameDirectionOk = directionOk;
            lastFrameCorrOk = corrOk;
            lastFrameAngleSignedDiffOk = angleSignedDiffOk;
            lastFrameAnglePairMidGapOk = anglePairMidGapOk;
            lastFrameAngleOk = angleOk;
            lastFrameStableOk = stableOk;

            // 累加各闸失败计数
            if (!bJumpOk)             gateFailBJump++;
            if (!notBoundary)         gateFailBoundary++;
            if (!diffOk)              gateFailDiff++;
            if (!directionOk)         gateFailDirection++;
            if (bRes.corr < mCfg.frameCorrBMin) gateFailCorrB++;
            if (aRes.corr < mCfg.frameCorrAMin) gateFailCorrA++;
            if (!angleSignedDiffOk)   gateFailAngleSignedDiff++;
            if (!anglePairMidGapOk)   gateFailAnglePairMidGap++;

            // 稳定簇状态细分
            if (!stableOk) {
                if (boneLagLocked) {
                    gateFailStableOutOfLock++;
                    lastFrameStableState = 3;
                } else if (recentBoneLagBList.size() < mCfg.stableLagWarmupCount) {
                    gateFailStableWarmup++;
                    lastFrameStableState = 0;
                } else {
                    gateFailStableNotConcentrated++;
                    lastFrameStableState = 1;
                }
            } else {
                lastFrameStableState = 2;
            }
        }

        // 每 50 帧自动输出一次当前统计（不需要等有效帧攒满）
        if (gateTotalFrames % 50 == 0) {
            printGateStats();
        }

        // Write this decision before the last value can finish/close the round log.
        record();
        recordOnReturn.dismiss();
        handlePatientMeasureValue(
            aRes.sos,
            bRes.sos,
            sosAvg,
            aRes.refinedLag,
            bRes.refinedLag,
            diffLagForCheck,
            pairMidGap,
            aRes.corr,
            bRes.corr,
            strictValid
            );
    }
}

void MainWindow::processCalibrationFrame(const QVector<double>& filBC,
                                         const QVector<double>& filBD,
                                         const QVector<double>& filAC,
                                         const QVector<double>& filAD,
                                         const ArrivalResult& pickBC,
                                         const ArrivalResult& pickBD,
                                         const ArrivalResult& pickAC,
                                         const ArrivalResult& pickAD)
{
    if (!calibrationDialog) return;

    constexpr double vMin = 1800.0;
    constexpr double vMax = 5000.0;
    constexpr int lagToleranceAB = 20;

    CalibrationFrame frame;
    const PairResult bRes = calibrationSignalProcessor.estimatePairSpeed(
        filBD, filBC, pickBD, pickBC, vMin, vMax,
        QStringLiteral("Calibration B_pair / BD->BC"));

    if (!bRes.valid) {
        calibrationDialog->submitFrame(frame);
        return;
    }

    const int bLagJump = std::abs(bRes.refinedLag - bRes.roughLag);
    const int lagMin = qMax(1, static_cast<int>(std::floor(
        calibrationSignalProcessor.probeDistanceCD
        / (vMax * calibrationSignalProcessor.samplePeriod))) - 5);
    const int lagMax = qMax(lagMin + 1, static_cast<int>(std::ceil(
        calibrationSignalProcessor.probeDistanceCD
        / (vMin * calibrationSignalProcessor.samplePeriod))) + 5);

    frame.bValid = bLagJump <= 70 && bRes.corr >= mCfg.frameCorrBMin;
    frame.boundaryPeak = std::abs(bRes.refinedLag) <= lagMin + 2
        || std::abs(bRes.refinedLag) >= lagMax - 2;
    frame.sosB = bRes.sos;
    frame.corrB = bRes.corr;
    frame.lagB = bRes.refinedLag;

    PairResult aRes = calibrationSignalProcessor.estimatePairSpeedByValley(
        filAD, filAC, pickAD, bRes.refinedLag, lagToleranceAB,
        QStringLiteral("Calibration A_pair / AD->AC / valley"));
    if (!aRes.valid) {
        aRes = calibrationSignalProcessor.estimatePairSpeed(
            filAD, filAC, pickAD, pickAC, vMin, vMax,
            QStringLiteral("Calibration A_pair / AD->AC / constrained"),
            qMax(1, bRes.refinedLag - lagToleranceAB),
            bRes.refinedLag + lagToleranceAB);
    }
    frame.aValid = aRes.valid && aRes.corr >= 0.25;
    if (aRes.valid) {
        frame.sosA = aRes.sos;
        frame.corrA = aRes.corr;
        frame.lagA = aRes.refinedLag;
    }

    const PairResult auditB = signalProcessor.estimatePairSpeed(
        filBD, filBC, pickBD, pickBC, vMin, vMax,
        QStringLiteral("Calibration active-D peak audit"));
    frame.peakConsistent = auditB.valid
        && std::abs(auditB.refinedLag - bRes.refinedLag) <= 3;

    if (aRes.valid) {
        updateSpeedDebugPanel(aRes.sos, bRes.sos, bRes.sos,
                              aRes.refinedLag, bRes.refinedLag,
                              std::abs(aRes.refinedLag - bRes.refinedLag),
                              aRes.corr, bRes.corr);
    } else {
        updateSpeedDebugPanel(0.0, bRes.sos, bRes.sos,
                              0, bRes.refinedLag, 0, 0.0, bRes.corr);
    }
    calibrationDialog->submitFrame(frame);
}

void MainWindow::resetBoneLagStability()
{
    boneLagRejectedFrameCount = 0;
    recentBoneLagBList.clear();

    boneLagLocked = false;
    lockedBoneLagCenter = 0;

    boneLagOutOfLockCount = 0;
    partialRelockPending = false;
    partialPreviousLagCenter = 0;

    if (kDebugPerFrame) {
        qDebug() << "Bone lag stability reset.";
    }
}

void MainWindow::resetGateStats()
{
    gateTotalFrames = 0;
    gateFailBJump = 0;
    gateFailBoundary = 0;
    gateFailDiff = 0;
    gateFailDirection = 0;
    gateFailCorrA = 0;
    gateFailCorrB = 0;
    gateFailAngleSignedDiff = 0;
    gateFailAnglePairMidGap = 0;
    gateFailStableWarmup = 0;
    gateFailStableNotConcentrated = 0;
    gateFailStableOutOfLock = 0;
    gateDecoupledFrames = 0;
    gateCorrAMin = 0.0;
    gateCorrAMax = 0.0;
    gateCorrASum = 0.0;
}

void MainWindow::printGateStats() const
{
    if (gateTotalFrames <= 0) {
        qDebug() << "[闸门统计] 暂无数据";
        return;
    }

    auto pct = [&](int fail) -> QString {
        double r = (double)fail / (double)gateTotalFrames * 100.0;
        return QString("%1%").arg(r, 0, 'f', 1);
    };

    // 分两行短输出，避免 Qt Creator 截断长行
    qDebug().noquote()
        << QString("[闸门统计 共%1帧] BJump=%2 Boundary=%3 Diff=%4 Dir=%5")
               .arg(gateTotalFrames)
               .arg(pct(gateFailBJump))
               .arg(pct(gateFailBoundary))
               .arg(pct(gateFailDiff))
               .arg(pct(gateFailDirection));

    qDebug().noquote()
        << QString("[闸门统计] CorrA=%1 CorrB=%2 AngDiff=%3 AngGap=%4 StabWarm=%5 StabConc=%6 StabLock=%7")
               .arg(pct(gateFailCorrA))
               .arg(pct(gateFailCorrB))
               .arg(pct(gateFailAngleSignedDiff))
               .arg(pct(gateFailAnglePairMidGap))
               .arg(pct(gateFailStableWarmup))
               .arg(pct(gateFailStableNotConcentrated))
               .arg(pct(gateFailStableOutOfLock));

    if (gateDecoupledFrames > 0) {
        qDebug().noquote()
            << QString("  (另有 %1 帧因脱耦被忽略，corr<0.25)")
                   .arg(gateDecoupledFrames);
    }

    if (gateTotalFrames > 0) {
        double corrAMean = gateCorrASum / gateTotalFrames;
        qDebug().noquote()
            << QString("[CorrA分布] 最小=%1  平均=%2  最大=%3  门槛=%4")
                   .arg(gateCorrAMin, 0, 'f', 3)
                   .arg(corrAMean, 0, 'f', 3)
                   .arg(gateCorrAMax, 0, 'f', 3)
                   .arg(mCfg.frameCorrAMin, 0, 'f', 2);
    }
}

QString MainWindow::gateStatsSummary() const
{
    // 保留旧接口兼容，但调用方应改用 printGateStats()
    if (gateTotalFrames <= 0) return QString();

    auto pct = [&](int fail) -> QString {
        double r = (double)fail / (double)gateTotalFrames * 100.0;
        return QString("%1%").arg(r, 0, 'f', 1);
    };

    return QString(
        "BJump=%1 Boundary=%2 Diff=%3 Dir=%4 | "
        "CorrA=%5 CorrB=%6 | "
        "AngDiff=%7 AngGap=%8 | "
        "StabWarm=%9 StabConc=%10 StabLock=%11")
        .arg(pct(gateFailBJump))
        .arg(pct(gateFailBoundary))
        .arg(pct(gateFailDiff))
        .arg(pct(gateFailDirection))
        .arg(pct(gateFailCorrA))
        .arg(pct(gateFailCorrB))
        .arg(pct(gateFailAngleSignedDiff))
        .arg(pct(gateFailAnglePairMidGap))
        .arg(pct(gateFailStableWarmup))
        .arg(pct(gateFailStableNotConcentrated))
        .arg(pct(gateFailStableOutOfLock));
}

void MainWindow::discardPartialRound()
{
    experimentLog.write({{"event", "discard_partial"}, {"discarded_values", processValidCount}});
    // A new lock must never inherit samples measured at the previous position.
    currentRoundSosList.clear();
    currentRoundAList.clear();
    currentRoundBList.clear();
    currentRoundCorrAList.clear();
    currentRoundCorrBList.clear();
    currentRoundPairMidGapList.clear();
    currentRoundSignedLagDiffList.clear();
    processValidCount = 0;
    ui->barMeasureProgress->setValue(0);
}

void MainWindow::rejectBoneLagCandidate()
{
    if (!patientMeasureRunning || acquireMode != PatientMeasureMode) return;
    // Tolerate brief dropouts, but do not keep stale evidence indefinitely.
    // Reuse the existing sustained-loss count; no new tuning constant.
    if (++boneLagRejectedFrameCount >= mCfg.boneLagUnlockCount) {
        experimentLog.write({{"event", "sustained_precheck_loss"}});
        const bool deferDiscard = deferPartialDiscardUntilRelock && processValidCount > 0
            && (boneLagLocked || partialRelockPending);
        const int previousCenter = boneLagLocked ? lockedBoneLagCenter : partialPreviousLagCenter;
        resetBoneLagStability();
        if (deferDiscard) {
            partialRelockPending = true;
            partialPreviousLagCenter = previousCenter;
            experimentLog.write({{"event", "partial_discard_deferred"},
                                 {"previous_lag", previousCenter},
                                 {"preserved_values", processValidCount}});
        } else {
            discardPartialRound();
        }
    }
}

bool MainWindow::checkBoneLagStable(int lagB, int* centerOut, int* countOut)
{
    boneLagRejectedFrameCount = 0;
    // ======================================================
    // 1. 当前 lagB 加入候选窗口
    // ======================================================
    recentBoneLagBList.append(lagB);

    while (recentBoneLagBList.size() > stableLagWindowSize) {
        recentBoneLagBList.removeFirst();
    }

    // ======================================================
    // 2. 如果还没有锁定稳定簇，先进入 warmup 观察阶段
    // ======================================================
    if (!boneLagLocked) {

        if (recentBoneLagBList.size() < mCfg.stableLagWarmupCount) {
            if (centerOut) *centerOut = lagB;
            if (countOut) *countOut = recentBoneLagBList.size();

            if (kDebugPerFrame) {
                qDebug() << "BoneLagStable:"
                         << "lagB =" << lagB
                         << "recentCount =" << recentBoneLagBList.size()
                         << "warmupNeed =" << mCfg.stableLagWarmupCount
                         << "locked = false"
                         << "stable = false, warmup";
            }

            return false;
        }

        // 用最近 mCfg.stableLagWarmupCount 个候选的中位数作为锁定中心。
        // 注意：这里不再选择最小 lag，也就是不再追逐最高声速簇。
        QVector<int> sorted = recentBoneLagBList;
        std::sort(sorted.begin(), sorted.end());

        int center = sorted[sorted.size() / 2];

        int countAroundCenter = 0;
        for (int v : recentBoneLagBList) {
            if (std::abs(v - center) <= mCfg.stableLagTolerance) {
                countAroundCenter++;
            }
        }

        if (centerOut) *centerOut = center;
        if (countOut) *countOut = countAroundCenter;

        // 候选还不够集中，不锁定，不让进度条动。
        if (countAroundCenter < mCfg.stableLagLockNeedCount) {
            if (kDebugPerFrame) {
                qDebug() << "BoneLagStable:"
                         << "lagB =" << lagB
                         << "window =" << recentBoneLagBList
                         << "candidateCenter =" << center
                         << "countAroundCenter =" << countAroundCenter
                         << "need =" << mCfg.stableLagLockNeedCount
                         << "tolerance =" << mCfg.stableLagTolerance
                         << "locked = false"
                         << "stable = false, not concentrated";
            }

            return false;
        }

        // 锁定稳定簇
        boneLagLocked = true;
        lockedBoneLagCenter = center;
        boneLagOutOfLockCount = 0;

        if (partialRelockPending) {
            const int previousCenter = partialPreviousLagCenter;
            bool sameCluster = std::abs(center - previousCenter) <= partialRelockRetentionTolerance;
            for (double sosB : std::as_const(currentRoundBList)) {
                if (!std::isfinite(sosB) || sosB <= 0.0) {
                    sameCluster = false;
                    break;
                }
                const int sampleLag = qRound(signalProcessor.probeDistanceCD
                    / (sosB * signalProcessor.samplePeriod));
                if (std::abs(sampleLag - center) > partialRelockRetentionTolerance) {
                    sameCluster = false;
                    break;
                }
            }
            partialRelockPending = false;
            partialPreviousLagCenter = 0;
            experimentLog.write({{"event", sameCluster ? "partial_relock_retained" : "partial_relock_discarded"},
                                 {"previous_lag", previousCenter}, {"new_lag", center},
                                 {"preserved_values", processValidCount}});
            if (!sameCluster) {
                discardPartialRound();
            }
        }

        bool currentInCluster =
            (std::abs(lagB - lockedBoneLagCenter) <= mCfg.stableLagTolerance);

        if (kDebugPerFrame) {
            qDebug() << "BoneLagStable:"
                     << "LOCKED"
                     << "lockedCenter =" << lockedBoneLagCenter
                     << "countAroundCenter =" << countAroundCenter
                     << "currentLagB =" << lagB
                     << "currentInCluster =" << currentInCluster;
        }

        // 锁定这一帧，如果当前点也在簇内，就允许有效。
        return currentInCluster;
    }

    // ======================================================
    // 3. 已经锁定稳定簇：只接受锁定簇附近的帧
    // ======================================================
    bool currentInCluster =
        (std::abs(lagB - lockedBoneLagCenter) <= mCfg.stableLagTolerance);

    int countAroundLocked = 0;
    for (int v : recentBoneLagBList) {
        if (std::abs(v - lockedBoneLagCenter) <= mCfg.stableLagTolerance) {
            countAroundLocked++;
        }
    }

    if (centerOut) *centerOut = lockedBoneLagCenter;
    if (countOut) *countOut = countAroundLocked;

    if (currentInCluster) {
        boneLagOutOfLockCount = 0;

        if (kDebugPerFrame) {
            qDebug() << "BoneLagStable:"
                     << "lagB =" << lagB
                     << "lockedCenter =" << lockedBoneLagCenter
                     << "countAroundLocked =" << countAroundLocked
                     << "tolerance =" << mCfg.stableLagTolerance
                     << "currentInCluster = true"
                     << "stable = true";
        }

        return true;
    }

    // 当前帧偏离锁定簇，不计入。
    boneLagOutOfLockCount++;

    if (kDebugPerFrame) {
        qDebug() << "BoneLagStable:"
                 << "lagB =" << lagB
                 << "lockedCenter =" << lockedBoneLagCenter
                 << "countAroundLocked =" << countAroundLocked
                 << "tolerance =" << mCfg.stableLagTolerance
                 << "currentInCluster = false"
                 << "outOfLockCount =" << boneLagOutOfLockCount
                 << "unlockNeed =" << mCfg.boneLagUnlockCount
                 << "stable = false";
    }

    // 如果连续多帧都偏离锁定簇，说明探头已经移到别的位置了。
    // 重新寻找稳定簇时也丢弃旧位置的本轮样本，不能跨位置混合平均。
    if (boneLagOutOfLockCount >= mCfg.boneLagUnlockCount) {
        experimentLog.write({{"event", "cluster_lost"}, {"old_lag", lockedBoneLagCenter}});
        if (kDebugPerFrame) {
            qDebug() << "BoneLagStable:"
                     << "unlock because too many out-of-cluster frames"
                     << "oldLockedCenter =" << lockedBoneLagCenter;
        }

        resetBoneLagStability();
        discardPartialRound();
    }

    return false;
}

void MainWindow::on_btnStartMeasurement_clicked()
{
    // 这个按钮现在 UI 显示为"开始检测"或"停止检测"，objectName 仍然是 btnPatientInfo

    if (patientMeasureRunning) {
        // 正在检测中 → 停止检测
        stopPatientMeasurement();
        resetOneRoundMeasurementState();
        resetBoneLagStability();
        resetGateStats();

        ui->barPairA->setValue(500);
        ui->barPairB->setValue(500);
        ui->barMeasureProgress->setValue(0);
        ui->barMeasureProgress->setFormat("有效值：%v / %m");
        clearFeedbackReadings();
        ui->lblProcessStatus->setText("检测已手动停止");
        ui->lblProcessStatus->setStyleSheet(
            "font-size: 14px; color: #9A5B00; font-weight: bold;"
            );
        return;
    }

    if (hasPendingMeasurement) {
        statusBar()->showMessage(
            QStringLiteral("上一次检测结果尚未保存，请先在“测量结果”中点击“重试保存”。"),
            8000);
        updatePatientSelectionUi();
        return;
    }

    // 关键：如果上一轮测完后弹了提示框，点击"开始检测"时先自动关掉
    closeRoundFinishedTip();

    // 已完成上一组五轮测量时，开始按钮直接开启同一患者的新一组测量。
    if (roundSosList.size() >= normalMeasureRounds) {
        resetPatientMeasurementState(normalMeasureRounds);
    }

    // 没有当前病人信息：直接进入病人信息填写/导入页面
    // 不弹 OK 小窗口，也不自动开始测量
    if (!hasCurrentPatient()) {
        pendingStartAfterPatientInfo = false;
        on_btnPatientInfo_clicked();
        return;
    }

    // 每次点击"开始检测"只测 1 次；
    // 前面完成的第 1 次、第 2 次……保存在 roundSosList 里，不会清空。
    startPatientMeasurement(normalMeasureRounds, true);
}

void MainWindow::on_btnMeasurementGuide_clicked()
{
    if (patientMeasureRunning || (autoRunning && !patientMeasureRunning)) return;
    runMeasurementGuide(false);
}

bool MainWindow::shouldOfferMeasurementGuide() const
{
    return !measurementGuideSeenThisRun
        && !MeasurementGuideDialog::isCurrentVersionSeen(
            measurementGuideSettingsPath);
}

bool MainWindow::runMeasurementGuide(bool automatic)
{
    MeasurementGuideDialog dialog(
        automatic ? MeasurementGuideDialog::Mode::Automatic
                  : MeasurementGuideDialog::Mode::Manual,
        this, mCfg.frameCorrAMin);
    const bool accepted = dialog.exec() == QDialog::Accepted;

    if (automatic && accepted) {
        measurementGuideSeenThisRun = true;
        QString errorMessage;
        if (!MeasurementGuideDialog::markCurrentVersionSeen(
                measurementGuideSettingsPath, &errorMessage)) {
            statusBar()->showMessage(
                QStringLiteral("操作教学已完成，但首次提示状态未能保存：%1")
                    .arg(errorMessage),
                8000);
        }
    }

    if (ui->btnStartMeasurement->isEnabled()) {
        ui->btnStartMeasurement->setFocus(Qt::OtherFocusReason);
    }
    return accepted;
}



bool MainWindow::trySavePendingMeasurement()
{
    if (!hasPendingMeasurement) {
        return true;
    }
    bool patientExists = false;
    for (const PatientInfo& patient : patientList) {
        if (patient.id == pendingMeasurement.patientId) {
            patientExists = true;
            break;
        }
    }
    if (!patientExists) {
        QMessageBox::warning(this, "错误", "检测结果对应的档案已不存在，不能保存。");
        return false;
    }
    QList<MeasurementRecord> candidate = measurementList;
    candidate.append(pendingMeasurement);
    if (!saveMeasurements(candidate)) return false;
    measurementList = candidate;
    hasPendingMeasurement = false;
    pendingMeasurement = MeasurementRecord();
    refreshPatientDerivedViews();
    return true;
}

void MainWindow::on_btnSaveResult_clicked() {
    if (!hasPendingMeasurement) {
        QMessageBox::information(this, "提示", "当前没有可保存的新检测结果。");
        return;
    }
    if (!trySavePendingMeasurement()) return;
    statusBar()->showMessage(QStringLiteral("测量结果已保存。"), 5000);
}

bool MainWindow::hasIncompletePatientRounds() const
{
    return patientMeasureRunning
        || !currentRoundSosList.isEmpty()
        || (!roundSosList.isEmpty() && roundSosList.size() < normalMeasureRounds);
}
