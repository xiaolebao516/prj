// 被测者检测流程：逐帧结果交给测量核心、轮次与结果、校准采集、操作教学

#include "mainwindow/mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogs/calibrationdialog.h"
#include "dialogs/measurementguidedialog.h"
#include "health/bonehealth.h"
#include "measurement/parametergroup.h"
#include "storage/datalocation.h"

#include <QtCharts/QLineSeries>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QMessageBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QUuid>

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
                .arg(session.roundSos.size())
                .arg(mCfg.roundsPerMeasurement));
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

    restartFrameStream();
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
    resetFrameAssembly();
    calibrationSignalProcessor.probeDistanceCD = calibrationStore.parameters().activeD;
}

bool MainWindow::hasCurrentPatient() const
{
    return !currentPatient.id.trimmed().isEmpty()
    && !currentPatient.name.trimmed().isEmpty();
}

void MainWindow::startPatientMeasurement(bool offerFirstUseGuide)
{
    cancelPendingNextPatientRound();

    if (!hasCurrentPatient()) {
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

    restartFrameStream();

    // 上一组已完成：开始同一被测者的新一组测量。
    if (session.measurementComplete()) resetAllPatientMeasurementData();

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

    // 只重置当前这一轮，已完成的轮次保留
    resetOneRoundMeasurementState();

    const int nextRound = session.roundSos.size() + 1;
    startExperimentLog();

    ui->btnPatientInfo->setStyleSheet("");  // 恢复默认样式，确保可点击

    ui->lblProcessStatus->setText(
        QString("当前第 %1/%2 轮")
            .arg(nextRound)
            .arg(mCfg.roundsPerMeasurement));

    ui->lblProcessStatus->setStyleSheet(
        "font-size: 14px; color: #1D5FA8; font-weight: bold;"
        );

    autoTimer->start(80);
    updatePatientSelectionUi();
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
    for (double sos : session.roundSos) previousRounds.append(sos);
    QJsonObject config = measurementParameters(mCfg, profile, signalProcessor);
    config.insert("recording_profile", "subject-linked-20260918-v1");
    config.insert("measurement_session_id", experimentSessionId);
    config.insert("subject", experimentSubjectSnapshot);
    config.insert("previous_accepted_rounds", previousRounds);
    config.insert("build", __DATE__ " " __TIME__);
    config.insert("round", session.roundSos.size() + 1);
    config.insert("gain_BC", gainSliderA->value());
    config.insert("gain_BD", gainSliderB->value());
    config.insert("gain_AC", gainSliderC->value());
    config.insert("gain_AD", gainSliderD->value());
    config.insert("parameter_group", ParameterGroup::id(config));
    // Logs measured with the same parameters share one folder that also
    // describes those parameters (see ParameterGroup).
    QString folderError;
    const QString folder = ParameterGroup::prepareFolder(
        DataLocation::experimentsRoot(), config, QDateTime::currentDateTime(),
        true, &folderError);
    if (folder.isEmpty()) {
        statusBar()->showMessage(QStringLiteral("实验记录未开启：%1").arg(folderError), 8000);
        return;
    }
    experimentLog.start(folder, config);
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
        experimentLog.write({{"event", "stop"}, {"partial_values", session.validCount()},
                             {"accepted_rounds", session.roundSos.size()}});
        experimentLog.close();
    }
    checkExperimentLogError();
    if (autoTimer->isActive()) {
        autoTimer->stop();
    }

    patientMeasureRunning = false;
    acquireMode = DebugAcquireMode;
    updatePatientSelectionUi();
}

void MainWindow::scheduleNextPatientRound(int finishedRounds)
{
    cancelPendingNextPatientRound();
    if (finishedRounds <= 0 ||
        finishedRounds >= mCfg.roundsPerMeasurement ||
        finishedRounds != session.roundSos.size() ||
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

    session.resetAll();
    closeRoundFinishedTip();

    ui->barPairA->setValue(500);
    ui->barPairB->setValue(500);
    ui->barMeasureProgress->setRange(0, mCfg.framesPerRound);
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

// Each round starts with an empty stability window and fresh gate statistics.
void MainWindow::resetOneRoundMeasurementState()
{
    session.resetRound();

    ui->barPairA->setValue(500);
    ui->barPairB->setValue(500);
    ui->barMeasureProgress->setRange(0, mCfg.framesPerRound);
    ui->barMeasureProgress->setValue(0);
    ui->barMeasureProgress->setFormat("有效值：%v / %m");
    ui->barMeasureProgress->setTextVisible(true);

    clearFeedbackReadings();
    if (seriesSpeed) {
        seriesSpeed->clear();
    }

    speedPointIndex = 0;
    updatePatientSelectionUi();
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

    // 只作为提示窗口：不放按钮、非模态、不抢焦点，避免挡住"开始检测"
    measureTipBox->setStandardButtons(QMessageBox::NoButton);
    measureTipBox->setModal(false);
    measureTipBox->setWindowModality(Qt::NonModal);
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

// One filtered frame: analysis (FrameAnalyzer), then in a patient measurement
// the gates and stability (MeasurementSession), the experiment record and the
// value. Debug acquisition only shows the SOS read-outs.
void MainWindow::processFilteredFrame(const FilteredFrame& filtered)
{
    if (acquireMode == CalibrationAcquireMode) {
        processCalibrationFrame(filtered);
        return;
    }

    const bool patient = patientMeasurementActive();
    const bool recording = patient && experimentLog.active();
    QElapsedTimer processingTimer;
    processingTimer.start();
    const FrameAnalysis frame = frameAnalyzer.analyze(filtered, patient);

    // Written after the stability update and before the value can complete
    // (and close) the round log.
    const auto record = [&](const FrameVerdict* verdict) {
        if (!recording) return;
        QJsonObject evidence = frameEvidence(frame, mCfg, profile, verdict);
        evidence["processing_ms"] = processingTimer.elapsed();
        evidence["raw_BC"] = MeasurementExperimentLog::encodeRaw(currentFrame.bc);
        evidence["raw_BD"] = MeasurementExperimentLog::encodeRaw(currentFrame.bd);
        evidence["raw_AC"] = MeasurementExperimentLog::encodeRaw(currentFrame.ac);
        evidence["raw_AD"] = MeasurementExperimentLog::encodeRaw(currentFrame.ad);
        evidence["partial_values_before_accept"] = session.validCount();
        evidence["locked"] = session.isLocked();
        evidence["locked_lag"] = session.lockedLag();
        experimentLog.write(evidence);
        checkExperimentLogError();
    };

    switch (frame.decision) {
    case FrameDecision::EmptyInput:
        setSpeedDebugInvalid("滤波数据为空");
        if (patient) rejectPatientFrame();
        record(nullptr);
        return;
    case FrameDecision::BOnsetInconsistent:
        setSpeedDebugInvalid(QStringLiteral("首波定位不一致，本帧未计入；请重新贴合探头"));
        rejectPatientFrame();
        record(nullptr);
        return;
    case FrameDecision::BPairInvalid:
        setSpeedDebugInvalid("B_pair 无效");
        if (patient) rejectPatientFrame();
        record(nullptr);
        return;
    case FrameDecision::BLagJump:
        setSpeedDebugInvalid(QString("B_pair 跳变过大 rough=%1 refined=%2")
                                 .arg(frame.b.roughLag)
                                 .arg(frame.b.refinedLag));
        if (patient) rejectPatientFrame();
        record(nullptr);
        return;
    case FrameDecision::APairInvalid:
        setSpeedDebugInvalid("A_pair 无效");
        if (patient) rejectPatientFrame();
        record(nullptr);
        return;
    case FrameDecision::ABDifference:
        setSpeedDebugInvalid(QString("A/B 差异过大，diff=%1").arg(frame.diffLag()));
        // 被测者检测：显示姿态偏向，但不计入有效值
        if (patient) {
            session.rejectLagCandidate();
            updateCorrAFeedback(frame.a.corr);
            showPostureFeedback(frame.a.refinedLag, frame.b.refinedLag, frame.pairMidGap);
            updateMeasureProgress();
        }
        record(nullptr);
        return;
    case FrameDecision::Measured:
        break;
    }

    updateSpeedDebugPanel(frame.a.sos, frame.b.sos, frame.sos,
                          frame.a.refinedLag, frame.b.refinedLag, frame.diffLag(),
                          frame.a.corr, frame.b.corr);
    appendSpeedPoint(frame.sos);
    if (!patient) return;

    const FrameVerdict verdict = session.evaluate(frame);
    record(&verdict);
    handlePatientMeasureValue(frame.a.sos, frame.b.sos, frame.sos,
                              frame.a.refinedLag, frame.b.refinedLag, frame.diffLag(),
                              frame.pairMidGap, frame.a.corr, frame.b.corr,
                              verdict.accepted);
}

void MainWindow::processCalibrationFrame(const FilteredFrame& filtered)
{
    if (filtered.length() <= 0) {
        setSpeedDebugInvalid("滤波数据为空");
        return;
    }
    const FrameArrivals arrivals = FrameAnalyzer::detectArrivals(filtered);
    if (!calibrationDialog) return;

    PairResult a;
    PairResult b;
    const CalibrationFrame frame = FrameAnalyzer::calibrationFrame(
        calibrationSignalProcessor, signalProcessor, mCfg.frameCorrBMin,
        filtered, arrivals, &a, &b);
    if (b.valid) {
        if (a.valid) {
            updateSpeedDebugPanel(a.sos, b.sos, b.sos, a.refinedLag, b.refinedLag,
                                  std::abs(a.refinedLag - b.refinedLag), a.corr, b.corr);
        } else {
            updateSpeedDebugPanel(0.0, b.sos, b.sos, 0, b.refinedLag, 0, 0.0, b.corr);
        }
    }
    calibrationDialog->submitFrame(frame);
}

void MainWindow::rejectPatientFrame()
{
    if (patientMeasurementActive()) session.rejectLagCandidate();
    clearFeedbackReadings();
    updateMeasureProgress();
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
    Q_UNUSED(diffLag);
    if (!patientMeasurementActive()) return;

    updateCorrAFeedback(corrA);
    showPostureFeedback(lagA, lagB, pairMidGap);
    // 进度只在 strictValid 时前进
    if (strictValid) session.addValue(sosAvg, sosA, sosB, corrA, corrB, pairMidGap, lagA - lagB);
    updateMeasureProgress();

    if (strictValid && session.roundFull()) finishOnePatientRound();
}

void MainWindow::finishOnePatientRound()
{
    const RoundOutcome outcome = session.completeRound();
    const int totalRounds = mCfg.roundsPerMeasurement;
    ui->barMeasureProgress->setValue(0);
    ui->barMeasureProgress->setFormat("有效值：%v / %m");

    if (!outcome.accepted) {
        ui->barMeasureProgress->setTextVisible(true);

        // 本轮失败后，停止采集，让用户重新点"开始检测"并重新调整探头
        stopPatientMeasurement();

        const int rejectedRound = qMin(outcome.acceptedRounds + 1, totalRounds);
        ui->lblProcessStatus->setText(
            QString("第 %1/%2 轮未计入｜数据稳定性不足，请按界面提示调整后重新测量")
                .arg(rejectedRound)
                .arg(totalRounds));
        ui->lblProcessStatus->setStyleSheet(
            "font-size: 14px; color: #9A5B00; font-weight: bold;"
            );

        showRoundFinishedTip(outcome.acceptedRounds, totalRounds, false);
        return;
    }

    // 本轮完成后立刻停止采集，恢复"开始检测"按钮
    stopPatientMeasurement();

    const int finished = outcome.acceptedRounds;
    if (finished >= totalRounds) {
        finishAllPatientRounds();
        return;
    }

    // 还没满 5 次：先明确停止采集，再留出 1 秒让操作者保持姿势。
    ui->lblProcessStatus->setText(
        QString("第 %1/%2 轮完成｜1 秒后自动开始下一轮")
            .arg(finished)
            .arg(totalRounds));

    ui->lblProcessStatus->setStyleSheet(
        "font-size: 14px; color: #1D5FA8; font-weight: bold;"
        );

    showRoundFinishedTip(finished, totalRounds);
    scheduleNextPatientRound(finished);
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

    if (!session.measurementComplete()) {
        ui->lblProcessStatus->setText(
            QString("有效小测量不足：%1/%2，请继续测量。")
                .arg(session.roundSos.size())
                .arg(mCfg.roundsPerMeasurement)
            );
        ui->lblProcessStatus->setStyleSheet(
            "font-size: 14px; color: #9A5B00; font-weight: bold;"
            );
        return;
    }

    double finalSos=0, finalA=0, finalB=0;
    QVector<int> selectedIndices;
    if (!session.finalMeans(finalSos, finalA, finalB, &selectedIndices)) {
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
    {
        // Record which parameter group produced this result and keep that
        // group's description in the data folder, also in builds without logs.
        const QJsonObject parameters = measurementParameters(mCfg, profile, signalProcessor);
        pendingMeasurement.parameterGroup = ParameterGroup::id(parameters);
        ParameterGroup::prepareFolder(DataLocation::experimentsRoot(), parameters,
                                      QDateTime::currentDateTime(), false);
    }
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
             << "sourceRounds =" << session.roundSos.size()
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

// 检测中点击"停止检测"/"获取波形"/"自动采集"：停止并清空本轮进度（已完成的轮次保留）
void MainWindow::stopPatientMeasurementManually()
{
    stopPatientMeasurement();
    resetOneRoundMeasurementState();
    ui->lblProcessStatus->setText("检测已手动停止");
    ui->lblProcessStatus->setStyleSheet(
        "font-size: 14px; color: #9A5B00; font-weight: bold;"
        );
}

void MainWindow::on_btnStartMeasurement_clicked()
{
    if (patientMeasureRunning) {
        stopPatientMeasurementManually();
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

    // 已完成上一组五轮测量时，开始按钮直接开启同一被测者的新一组测量。
    if (session.measurementComplete()) resetAllPatientMeasurementData();

    // 没有当前被测者：直接进入档案选择，不自动开始测量
    if (!hasCurrentPatient()) {
        on_btnPatientInfo_clicked();
        return;
    }

    // 每次点击只测 1 轮；已完成的轮次保存在 session.roundSos 里，不会清空。
    startPatientMeasurement(true);
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
    return patientMeasureRunning || session.hasIncompleteRounds();
}
