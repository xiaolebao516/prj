#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bonehealth.h"
#include "utils.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QtCharts/QValueAxis>
#include <QRegularExpression>
#include <QInputDialog>
#include <QDateTime>
#include <QCoreApplication>
#include <QDomElement>
#include <QFileDialog>
#include <algorithm>
#include <cmath> // 确保包含 math 头文件
#include <QFrame>
#include <QGridLayout>
#include <QBoxLayout>
#include <QPointer>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this))
{
    ui->setupUi(this);

    // ------------------- 开始美化代码 -------------------

    // 1. 设置主窗口背景色 (告别灰色)
    this->setStyleSheet("QMainWindow { background-color: #F5F7FA; }");

    // 2. 定义全局样式表 (建议放在单独的 QString 变量中，方便修改)
    QString qss = R"(
    /* 全局字体 */
    QWidget {
        font-family: 'Microsoft YaHei', 'Segoe UI';
        font-size: 14px;
        color: #333333;
    }

    /* ---------------- 按钮美化 ---------------- */
    QPushButton {
        background-color: #FFFFFF;
        border: 1px solid #DCDFE6;
        border-radius: 6px;       /* 圆角 */
        padding: 8px 16px;        /* 内边距，让按钮变胖一点 */
        color: #606266;
        font-weight: bold;
    }
    QPushButton:hover {
        background-color: #ECF5FF; /* 悬停变淡蓝 */
        color: #409EFF;
        border-color: #C6E2FF;
    }
    QPushButton:pressed {
        background-color: #409EFF; /* 按下变深蓝 */
        color: #FFFFFF;
        border-color: #409EFF;
    }

    /* 特殊按钮：比如“获取波形”、“保存”这种主要操作，可以单独设为蓝色背景 */
    /* 你需要在 UI 设计器里给这些按钮的 styleSheet 属性单独加，或者用 objectName 区分 */
    QPushButton#btnAcquireWaveform, QPushButton#btnLogin {
        background-color: #409EFF;
        color: white;
        border: none;
    }
    QPushButton#btnAcquireWaveform:hover, QPushButton#btnLogin:hover {
        background-color: #66B1FF;
    }

    /* ---------------- 输入框美化 ---------------- */
    QLineEdit, QDateEdit, QComboBox {
        border: 1px solid #DCDFE6;
        border-radius: 4px;
        padding: 5px;  /* 文字不要紧贴边框 */
        background-color: #FFFFFF;
        selection-background-color: #409EFF;
    }
    QLineEdit:focus, QDateEdit:focus, QComboBox:focus {
        border: 1px solid #409EFF; /* 选中时边框变蓝 */
    }

    /* ---------------- 表格美化 (TableWidget) ---------------- */
    QTableWidget {
        background-color: #FFFFFF;
        border: 1px solid #EBEEF5;
        gridline-color: #EBEEF5;
        selection-background-color: #ECF5FF; /* 选中行背景淡蓝 */
        selection-color: #606266;            /* 选中行文字颜色 */
    }
    QHeaderView::section {
        background-color: #F5F7FA;  /* 表头背景灰白 */
        border: none;
        border-bottom: 1px solid #EBEEF5;
        border-right: 1px solid #EBEEF5;
        padding: 8px;
        font-weight: bold;
    }

    /* ---------------- 标签 ---------------- */
    QLabel {
        color: #303133;
    }

    /* ---------------- 分组框 ---------------- */
    QGroupBox {
        border: 1px solid #DCDFE6;
        border-radius: 6px;
        margin-top: 10px; /* 给标题留位置 */
        padding-top: 10px;
    }
    QGroupBox::title {
        subcontrol-origin: margin;
        subcontrol-position: top left;
        padding: 0 5px;
        color: #409EFF; /* 标题蓝色 */
        font-weight: bold;
    }
)";

    // 应用样式表到整个应用程序窗口
    this->setStyleSheet(qss);

    // ------------------- 结束美化代码 -------------------

    this->setWindowTitle("骨密度仪APP");
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    //this->showFullScreen();
    this->showMaximized();

    // ✅ 默认显示主页面
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);

    connect(&scanTimer, &QTimer::timeout, this, &MainWindow::scanPorts);
    scanTimer.start(1000);
    scanPorts();

    //connect(serial, &QSerialPort::readyRead, this, &MainWindow::handleSerialReadyRead);
    connect(serial, &QSerialPort::errorOccurred, this, &MainWindow::handleSerialError);

    setupChart();
    setupSpeedChart(); // ✅ 初始化声速趋势图
    setupSpeedDebugPanel();  // ✅ 初始化声速调试显示区域
    initProcessPanel();       // 初始化检查过程示意区
    initLatestResultPanel();  // 初始化右侧最近一次测量结果区

    ui->btnPatientInfo->setText("开始检测");

    connect(ui->pushButton, &QPushButton::clicked,
            this, &MainWindow::on_btnAcquireWaveform_clicked);
    // ✅ 初始化滤波器
    signalProcessor.designFIR(1250000.0, 600000.0, 62500000.0);

    // ✅ 加载患者数据库
    xmlFilePath = QCoreApplication::applicationDirPath() + "/patients.xml";
    loadPatients();
    //ui->comboPart->setCurrentIndex(-1);
    //ui->dPartCombo->setCurrentIndex(-1);
    refreshTable(patientList);


    // ✅ 档案管理按钮连接

    ui->dateBirth->setDisplayFormat("yyyy-MM-dd");
    ui->dateCheck->setDisplayFormat("yyyy-MM-dd");
    ui->dBirth->setDisplayFormat("yyyy-MM-dd");
    ui->dCheck->setDisplayFormat("yyyy-MM-dd");

    initSearchControls(); // ✅ 初始化下拉框

    autoTimer = new QTimer(this);
    connect(autoTimer,&QTimer::timeout,this,&MainWindow::sendCmd);



}

//先登录===============================================================================
void MainWindow::on_btnLogin_clicked() {
    QString user = ui->editUsername->text().trimmed();
    QString pass = ui->editPassword->text();

    ui->lblLoginMsg->clear();

    // 简单验证，后续可以改成读文件或数据库
    if (user == "1" && pass == "1") {
        ui->stackedWidget->setCurrentWidget(ui->pageMain); // 登录成功进入主界面
    } else {
        ui->lblLoginMsg->setText("账号或密码错误，请重试");
    }
}
//====================================================================================

MainWindow::~MainWindow() {
    if (serial->isOpen()) serial->close();
    delete ui;
}

// ================= 串口扫描等原有代码 =======================================================================================
void MainWindow::scanPorts() {
    QString current = ui->comboPort->currentData().toString();
    ui->comboPort->blockSignals(true);
    ui->comboPort->clear();

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        // 文本：COM11 - USB Serial Device
        // data：COM11   ✅ 只存端口名
        ui->comboPort->addItem(info.portName() + " - " + info.description(),
                               info.portName());
    }

    int idx = ui->comboPort->findData(current);
    if (idx >= 0) ui->comboPort->setCurrentIndex(idx);
    ui->comboPort->blockSignals(false);
}


void MainWindow::on_connectButton_clicked() {
    if (!serial->isOpen()) {

        QString portName = ui->comboPort->currentData().toString();
        if (portName.isEmpty()) {
            QMessageBox::warning(this, "Error", "Select a serial port first");
            return;
        }

        serial->setPortName(portName);
        serial->setBaudRate(QSerialPort::Baud115200);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);

        serial->setReadBufferSize(0);              // 可选

        // ⭐⭐⭐ 关键一行：关闭文本模式的 LF ↔ CRLF 自动转换
        serial->setTextModeEnabled(false);   // ⭐⭐ 最关键！！！

        if (serial->open(QIODevice::ReadWrite)) {
            serial->setDataTerminalReady(true); // 拉高 DTR，告诉 Pico "我准备好了"
            serial->setRequestToSend(false);     // 拉高 RTS (部分固件也需要这个)




            // ★★ 必须加的三行，解决 USB CDC 卡住的问题
            serial->clear(QSerialPort::AllDirections);
            serial->flush();
            serial->readAll();
            serial->waitForReadyRead(10);

            rxBuffer.clear();
            chReceived[0] = chReceived[1] = chReceived[2] = chReceived[3] = false;

            qDebug() << "Serial opened on" << portName;
            qDebug() << "Serial actually opened:" << serial->isOpen();
            qDebug() << "Error:" << serial->error();

            ui->connectButton->setText("Disconnect");

            // ★★ 放在这里确保连接成功后 readyRead 才被连接
            connect(serial, &QSerialPort::readyRead,
                    this, &MainWindow::handleSerialReadyRead);

        } else {
            QMessageBox::critical(this, "Open failed", serial->errorString());
            return;
        }

    } else {
        serial->close();
        ui->connectButton->setText("Connect");
    }
}



void MainWindow::on_triggerButton_clicked()
{
    if (patientMeasureRunning) {
        QMessageBox::information(this,
                                 "正在进行病人检测",
                                 "当前处于病人检测流程中，请等待检测完成后再使用 trigger 调试。");
        return;
    }

    acquireMode = DebugAcquireMode;

    if (!autoRunning) {
        autoTimer->start(80);
        ui->triggerButton->setText("停止自动采集");
        autoRunning = true;
    } else {
        autoTimer->stop();
        ui->triggerButton->setText("trigger");
        autoRunning = false;
    }
}

void MainWindow::sendCmd() {
    if (!serial->isOpen()) return;

    quint16 idx16 = nextFrameIdx++;

    QByteArray cmd;
    cmd.append((char)0xA5);
    cmd.append((char)0x5A);

    // gain 小端
    cmd.append((char)(globalGain & 0xFF));
    cmd.append((char)((globalGain >> 8) & 0xFF));

    // frame_idx 小端（16bit）
    // ✅ 修复：原Lambda代码只发了1个字节，这里必须发2个字节
    cmd.append((char)(idx16 & 0xFF));
    cmd.append((char)((idx16 >> 8) & 0xFF));

    serial->write(cmd);
    // 注意：自动模式下不要加 waitForBytesWritten，会阻塞界面
    // 也不要在这里 clear() rxBuffer，否则会把正在接收的数据清掉
}

//=====================发命令==============================================================
void MainWindow::on_btnAcquireWaveform_clicked()
{
    if (patientMeasureRunning) {
        QMessageBox::information(this,
                                 "正在进行病人检测",
                                 "当前处于病人检测流程中，不能手动获取波形。");
        return;
    }

    acquireMode = DebugAcquireMode;

    if (!serial->isOpen()) {
        QMessageBox::warning(this, "Not connected", "请先连接串口");
        return;
    }

    quint16 idx16 = nextFrameIdx++;

    QByteArray cmd;
    cmd.append(char(0xA5));
    cmd.append(char(0x5A));

    cmd.append(char(globalGain & 0xFF));
    cmd.append(char((globalGain >> 8) & 0xFF));

    cmd.append(char(idx16 & 0xFF));
    cmd.append(char((idx16 >> 8) & 0xFF));

    rxBuffer.clear();
    serial->write(cmd);
    serial->waitForBytesWritten(50);

    qDebug() << "TX CMD" << cmd.toHex(' ');

    samplesA.clear();
    samplesB.clear();
    samplesC.clear();
    samplesD.clear();

    chReceived[0] = chReceived[1] = chReceived[2] = chReceived[3] = false;
}

bool MainWindow::hasCurrentPatient() const
{
    return !currentPatient.id.trimmed().isEmpty()
    && !currentPatient.name.trimmed().isEmpty();
}

void MainWindow::startPatientMeasurement(int targetRounds)
{
    if (!hasCurrentPatient()) {
        pendingStartAfterPatientInfo = false;

        clearNewForm();
        ui->stackedWidget->setCurrentWidget(ui->pagePatientSelect);
        return;
    }

    if (!serial->isOpen()) {
        QMessageBox::warning(this, "串口未连接", "请先连接串口设备。");
        return;
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
        ui->triggerButton->setText("trigger");
    }

    acquireMode = PatientMeasureMode;
    patientMeasureRunning = true;
    currentMeasureTargetRounds = qMax(1, targetRounds);

    // 只重置当前这一轮，不清空 roundSosList
    resetOneRoundMeasurementState();

    int nextRound = roundSosList.size() + 1;

    ui->btnPatientInfo->setText("检测中...");
    ui->btnPatientInfo->setEnabled(false);

    ui->lblProcessStatus->setText(
        QString("正在检测：第 %1/%2 次，请保持探头与桡骨平行")
            .arg(nextRound)
            .arg(normalMeasureRounds)
        );

    ui->lblProcessStatus->setStyleSheet(
        "font-size: 12px; color: #409EFF; font-weight: bold;"
        );

    autoTimer->start(80);
}

void MainWindow::stopPatientMeasurement()
{
    if (autoTimer->isActive()) {
        autoTimer->stop();
    }

    patientMeasureRunning = false;
    acquireMode = DebugAcquireMode;

    ui->btnPatientInfo->setText("开始检测");
    ui->btnPatientInfo->setEnabled(true);
}

void MainWindow::resetPatientMeasurementState(int targetRounds)
{
    currentMeasureTargetRounds = qMax(1, targetRounds);
    resetAllPatientMeasurementData();
}

void MainWindow::resetAllPatientMeasurementData()
{
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

    closeRoundFinishedTip();

    ui->barPairA->setValue(500);
    ui->barPairB->setValue(500);


    ui->barMeasureProgress->setRange(0, processValidTarget);
    ui->barMeasureProgress->setValue(0);
    ui->barMeasureProgress->setFormat("有效值：%v / %m");
    ui->barMeasureProgress->setTextVisible(true);

    ui->lblPairAValue->setText("D=--\n目标=10.0");
    ui->lblPairBValue->setText("G=--\n目标=0.0");

    ui->lblProcessStatus->setText("等待开始测量");
    ui->lblProcessStatus->setStyleSheet(
        "font-size: 12px; color: #606266;"
        );

    if (seriesSpeed) {
        seriesSpeed->clear();
    }

    speedPointIndex = 0;
}

void MainWindow::resetOneRoundMeasurementState()
{
    currentRoundSosList.clear();
    currentRoundAList.clear();
    currentRoundBList.clear();
    currentRoundCorrAList.clear();
    currentRoundCorrBList.clear();
    currentRoundPairMidGapList.clear();
    currentRoundSignedLagDiffList.clear();

    processValidCount = 0;

    // ✅ 新增：每一轮正式测量开始时，清空最近 lagB 稳定性窗口
    resetBoneLagStability();

    ui->barPairA->setValue(500);
    ui->barPairB->setValue(500);

    ui->barMeasureProgress->setRange(0, processValidTarget);
    ui->barMeasureProgress->setValue(0);
    ui->barMeasureProgress->setFormat("有效值：%v / %m");
    ui->barMeasureProgress->setTextVisible(true);

    ui->lblPairAValue->setText("D=--\n目标=10.0");
    ui->lblPairBValue->setText("G=--\n目标=0.0");

    if (seriesSpeed) {
        seriesSpeed->clear();
    }

    speedPointIndex = 0;
}

void MainWindow::showRoundFinishedTip(int finishedRounds, int totalRounds)
{
    closeRoundFinishedTip();

    measureTipBox = new QMessageBox(this);
    measureTipBox->setIcon(QMessageBox::Information);
    measureTipBox->setWindowTitle("本次测量完成");

    measureTipBox->setText(
        QString("第 %1/%2 次测量完成。\n\n请再次点击“开始检测”进行下一次测量。")
            .arg(finishedRounds)
            .arg(totalRounds)
        );

    // 关键 1：不放 OK 按钮，只作为提示窗口
    measureTipBox->setStandardButtons(QMessageBox::NoButton);

    // 关键 2：必须设置为非模态，否则它可能会挡住主窗口，导致你点不了“开始检测”
    measureTipBox->setModal(false);
    measureTipBox->setWindowModality(Qt::NonModal);

    // 关键 3：让它像一个小提示窗口，不要抢占整个应用
    measureTipBox->setWindowFlags(
        measureTipBox->windowFlags()
        | Qt::Tool
        | Qt::WindowStaysOnTopHint
        );

    measureTipBox->setAttribute(Qt::WA_DeleteOnClose);

    connect(measureTipBox, &QObject::destroyed, this, [this]() {
        measureTipBox = nullptr;
    });

    // 显示在主窗口中间偏上位置，避免挡住“开始检测”按钮
    measureTipBox->show();

    QPoint center = this->geometry().center();
    int x = center.x() - measureTipBox->width() / 2;
    int y = this->geometry().top() + 120;
    measureTipBox->move(x, y);
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
        (oneRoundSignedLagDiff >= angleSignedDiffMin &&
         oneRoundSignedLagDiff <= angleSignedDiffMax);

    bool oneRoundPairMidGapOk =
        (oneRoundPairMidGap >= anglePairMidGapMin &&
         oneRoundPairMidGap <= anglePairMidGapMax);

    bool oneRoundAngleOk =
        (!enablePatientAngleGate) ||
        (oneRoundAngleSignedDiffOk && oneRoundPairMidGapOk);

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
             << "roundCorrAMin =" << roundCorrAMin
             << "roundCorrBMin =" << roundCorrBMin;

    // ======================================================
    // 新增：整轮质量门槛
    //
    // 目的：
    // 即使某些帧勉强通过了单帧 corrOk，
    // 如果整轮平均相关质量不够好，说明这一轮很可能是
    // “稳定但位置不准 / 波形质量差”的测量，直接丢弃。
    // ======================================================
    if (oneRoundCorrB < roundCorrBMin ||
        oneRoundCorrA < roundCorrAMin ||
        !oneRoundAngleOk) {
        qDebug() << "One patient round rejected by quality:"
                 << "sos =" << oneRoundSos
                 << "corrA =" << oneRoundCorrA
                 << "corrB =" << oneRoundCorrB;

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

        // 本轮失败后，停止采集，让用户重新点“开始检测”并重新调整探头
        stopPatientMeasurement();

        ui->lblProcessStatus->setText(
            QString("本次小测量姿态/质量不足，已丢弃：corrA=%1，corrB=%2，pairMidGap=%3，lagA-lagB=%4。请重新调整探头。")
                .arg(oneRoundCorrA, 0, 'f', 2)
                .arg(oneRoundCorrB, 0, 'f', 2)
                .arg(oneRoundPairMidGap, 0, 'f', 1)
                .arg(oneRoundSignedLagDiff, 0, 'f', 1)
            );
        ui->lblProcessStatus->setStyleSheet(
            "font-size: 12px; color: #E6A23C; font-weight: bold;"
            );

        showRoundFinishedTip(roundSosList.size(), normalMeasureRounds);
        return;
    }

    RoundCandidate cand;
    cand.sos = oneRoundSos;
    cand.a = oneRoundA;
    cand.b = oneRoundB;
    cand.corrA = oneRoundCorrA;
    cand.corrB = oneRoundCorrB;

    candidateRoundList.append(cand);

    // 从所有候选小测量中，重建“最大一致簇”。
    // 离群小测量不会进入 roundSosList。
    Utils::rebuildAcceptedRoundsFromCandidates(candidateRoundList, roundSosList,
        roundAList, roundBList, roundClusterTolerance);

    int finished = roundSosList.size();

    qDebug() << "One patient round accepted:"
             << "acceptedRoundCount =" << finished
             << "candidateCount =" << candidateRoundList.size()
             << "sos =" << oneRoundSos
             << "A =" << oneRoundA
             << "B =" << oneRoundB
             << "corrA =" << oneRoundCorrA
             << "corrB =" << oneRoundCorrB;

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

    // 本轮完成后立刻停止采集，恢复“开始检测”按钮
    stopPatientMeasurement();

    // 如果已经完成 5 次，直接生成最终结果
    if (finished >= normalMeasureRounds) {
        finishAllPatientRounds();
        return;
    }

    // 还没满 5 次：停住，等待再次点击“开始检测”
    ui->lblProcessStatus->setText(
        QString("本次小测量完成。有效主簇：%1/%2，候选次数：%3。请再次点击“开始检测”。")
            .arg(finished)
            .arg(normalMeasureRounds)
            .arg(candidateRoundList.size())
        );

    ui->lblProcessStatus->setStyleSheet(
        "font-size: 12px; color: #409EFF; font-weight: bold;"
        );

    showRoundFinishedTip(finished, normalMeasureRounds);
}

void MainWindow::finishAllPatientRounds()
{
    closeRoundFinishedTip();

    if (roundSosList.size() < normalMeasureRounds) {
        ui->lblProcessStatus->setText(
            QString("有效小测量不足：%1/%2，请继续测量。")
                .arg(roundSosList.size())
                .arg(normalMeasureRounds)
            );
        ui->lblProcessStatus->setStyleSheet(
            "font-size: 12px; color: #E6A23C; font-weight: bold;"
            );
        return;
    }

    double finalSos = Utils::trimmedMeanValue(roundSosList, 0.2);
    double finalA   = Utils::trimmedMeanValue(roundAList, 0.2);
    double finalB   = Utils::trimmedMeanValue(roundBList, 0.2);

    stopPatientMeasurement();

    int age = BoneHealth::calcPatientAge(
        QDate::fromString(currentPatient.birthDay, "yyyy-MM-dd"));

    double youngMean = currentPatient.gender.contains("男") ? 4150.0 : 4137.0;
    double youngSd   = 115.2;

    double ageMean = BoneHealth::calcAgeReferenceMean(age);
    if (currentPatient.gender.contains("男")) {
        ageMean += 20.0;
    }

    double ageSd = 115.2;

    double tScore = (finalSos - youngMean) / youngSd;
    double zScore = (finalSos - ageMean) / ageSd;

    QString strength = BoneHealth::classifyBoneStrength(tScore);
    double risk = BoneHealth::calcRelativeFractureRisk(tScore);
    int boneAge = BoneHealth::estimateBoneAgeFromSos(finalSos, currentPatient.gender);

    currentPatient.checkDate = QDate::currentDate().toString("yyyy-MM-dd");
    currentPatient.speedOfSound = QString::number(finalSos, 'f', 1);
    currentPatient.diagprompt = strength;

    updateLatestResultPanel(finalSos, tScore, zScore, strength, risk, boneAge);

    ui->lblProcessStatus->setText(
        QString("5 次测量完成：最终 SOS=%1 m/s")
            .arg(finalSos, 0, 'f', 1)
        );
    ui->lblProcessStatus->setStyleSheet(
        "font-size: 12px; color: #67C23A; font-weight: bold;"
        );

    showPatientMeasureFinishedDialog(finalSos, tScore, zScore, strength, risk, boneAge);

    qDebug() << "All patient rounds finished:"
             << "rounds =" << roundSosList.size()
             << "finalSos =" << finalSos
             << "finalA =" << finalA
             << "finalB =" << finalB
             << "T =" << tScore
             << "Z =" << zScore
             << "strength =" << strength
             << "risk =" << risk
             << "boneAge =" << boneAge;
}


void MainWindow::initLatestResultPanel()
{
    ui->lblLatestPart->setText("桡骨");
    ui->lblLatestSOS->setText("--");
    ui->lblLatestT->setText("--");
    ui->lblLatestZ->setText("--");
    ui->lblLatestStrength->setText("--");
    ui->lblLatestRisk->setText("--");
    ui->lblLatestBoneAge->setText("--");
}

void MainWindow::updateLatestResultPanel(double sos,
                                         double tScore,
                                         double zScore,
                                         const QString& strength,
                                         double risk,
                                         int boneAge)
{
    ui->lblLatestPart->setText("桡骨");
    ui->lblLatestSOS->setText(QString("%1 m/s").arg(sos, 0, 'f', 1));
    ui->lblLatestT->setText(QString("%1").arg(tScore, 0, 'f', 2));
    ui->lblLatestZ->setText(QString("%1").arg(zScore, 0, 'f', 2));
    ui->lblLatestStrength->setText(strength);
    ui->lblLatestRisk->setText(QString("%1").arg(risk, 0, 'f', 1));
    ui->lblLatestBoneAge->setText(QString("%1 岁").arg(boneAge));
}

void MainWindow::showPatientMeasureFinishedDialog(double sos,
                                                  double tScore,
                                                  double zScore,
                                                  const QString& strength,
                                                  double risk,
                                                  int boneAge)
{
    QString msg;
    msg += "病人检测完成。\n\n";
    msg += QString("姓名：%1\n").arg(currentPatient.name);
    msg += QString("ID：%1\n").arg(currentPatient.id);
    msg += QString("测量部位：桡骨\n");
    msg += QString("测量次数：%1 次\n\n").arg(currentMeasureTargetRounds);

    msg += QString("SOS：%1 m/s\n").arg(sos, 0, 'f', 1);
    msg += QString("T值：%1\n").arg(tScore, 0, 'f', 2);
    msg += QString("Z值：%1\n").arg(zScore, 0, 'f', 2);
    msg += QString("骨强度：%1\n").arg(strength);
    msg += QString("相对骨折风险：%1\n").arg(risk, 0, 'f', 1);
    msg += QString("相对骨龄：%1 岁\n").arg(boneAge);

    QMessageBox::information(this, "检测结果", msg);

    // 等你后面在 .ui 里新增 pageReport 后，
    // 可以把这里改成：
    // updateReportPageUi();
    // ui->stackedWidget->setCurrentWidget(ui->pageReport);
}


void MainWindow::handleSerialReadyRead() {
    QByteArray chunk = serial->readAll();

    //qDebug() << "RX chunk" << chunk.size() << chunk.toHex(' ');

    rxBuffer.append(chunk);

    // 显示当前 buffer 前 40 字节
    //if (rxBuffer.size() > 0) {
    //    int show = qMin(40, rxBuffer.size());
    //    qDebug() << "RX buffer head" << show << ":"
    //             << rxBuffer.left(show).toHex(' ');
    //}

    parseIncomingData();
}

void MainWindow::parseIncomingData() {
    const QByteArray MAGIC = QByteArray::fromHex("AA55");
    const int headerSize = 9;

    while (true) {
        // 1. 查找 MAGIC
        int start = rxBuffer.indexOf(MAGIC);
        if (start < 0) {
            if (rxBuffer.size() > 4096)
                rxBuffer.remove(0, rxBuffer.size() - 2);
            return;
        }

        if (start > 0) {
            // ❌ 删除丢弃字节的打印
            // qDebug() << "Drop" << start << "bytes before frame head";
            rxBuffer.remove(0, start);
        }

        if (rxBuffer.size() < headerSize) {
            return; // 数据不够，直接返回不打印
        }

        const unsigned char *p =
            reinterpret_cast<const unsigned char*>(rxBuffer.constData());

        quint16 gain   = p[2] | (p[3] << 8);
        quint16 idx    = p[4] | (p[5] << 8);
        quint8  ch     = p[6];
        quint16 length = p[7] | (p[8] << 8);

        // ❌ 删除 Header 解析信息的打印
        /*
        qDebug() << "Header:" << "AA55"
                 << "gain=" << gain
                 << "idx=" << idx
                 << "ch=" << ch
                 << "len=" << length;
        */

        // 2. length 合法性
        if (length == 0 || length > 5000) {
            // ❌ 删除错误打印
            rxBuffer.remove(0, 1);
            continue;
        }

        int frameBytes = headerSize + length * 2 + 2;

        if (rxBuffer.size() < frameBytes) {
            return; // 数据不够
        }

        // 3. 校验尾巴 EE EE
        quint8 tail1 = p[headerSize + length * 2];
        quint8 tail2 = p[headerSize + length * 2 + 1];

        if (tail1 != 0xEE || tail2 != 0xEE) {
            // ❌ 删除校验错误打印
            rxBuffer.remove(0, 1);
            continue;
        }

        // 4. 解析 Payload
        // ❌ 删除 VALID FRAME 打印
        // qDebug() << ">>> VALID FRAME idx=" << idx << "ch=" << ch;

        const unsigned char *payload = p + headerSize;

        QVector<quint16> tmp;
        tmp.reserve(length);

        for (int i = 0; i < length; ++i) {
            quint16 raw = payload[2*i] | (payload[2*i+1] << 8);
            tmp.append(raw & 0x0FFF);
        }

        WaveGroup &g = frameGroups[idx];
        g.ch[ch - 1] = tmp;
        g.has[ch - 1] = true;

        // ❌ 删除 flags 打印
        /*
        QString flags;
        for (int i = 0; i < 4; ++i) flags += g.has[i] ? "1" : "0";
        qDebug() << "FrameGroup idx" << idx << "flags:" << flags;
        */

        if (g.has[0] && g.has[1] && g.has[2] && g.has[3]) {
            // ❌ 删除 DONE 打印
            // qDebug() << ">>> FULL GROUP DONE idx=" << idx;

            samplesA = g.ch[0];
            samplesB = g.ch[1];
            samplesC = g.ch[2];
            samplesD = g.ch[3];


            frameGroups.remove(idx);
            plotSamples();
        }

        rxBuffer.remove(0, frameBytes);
    }
}



void MainWindow::plotSamples()
{
    // ==========================================================
    // 当前实际通道映射：
    // samplesA = CH1 = B -> C
    // samplesB = CH2 = B -> D
    // samplesC = CH3 = A -> C
    // samplesD = CH4 = A -> D
    // ==========================================================

    // 1. 根据铜块未滤波原始数据设置假波抹除参数
    GateConfig cfgBC;  // samplesA = B->C
    cfgBC.baselineStart = 20;
    cfgBC.baselineEnd   = 105;
    cfgBC.eraseStart    = 115;
    cfgBC.eraseEnd      = 500;
    cfgBC.rampEnd       = 560;

    GateConfig cfgBD;  // samplesB = B->D
    cfgBD.baselineStart = 20;
    cfgBD.baselineEnd   = 105;
    cfgBD.eraseStart    = 115;
    cfgBD.eraseEnd      = 500;
    cfgBD.rampEnd       = 560;

    GateConfig cfgAC;  // samplesC = A->C
    cfgAC.baselineStart = 0;
    cfgAC.baselineEnd   = 15;
    cfgAC.eraseStart    = 18;
    cfgAC.eraseEnd      = 480;
    cfgAC.rampEnd       = 550;

    GateConfig cfgAD;  // samplesD = A->D
    cfgAD.baselineStart = 0;
    cfgAD.baselineEnd   = 15;
    cfgAD.eraseStart    = 18;
    cfgAD.eraseEnd      = 480;
    cfgAD.rampEnd       = 550;

    // 2. 先在原始 ADC 层面抹掉假波，并减掉各通道基线
    QVector<double> rawBC = SignalProcessor::preprocessRawForFIR(samplesA, cfgBC, "B->C");
    QVector<double> rawBD = SignalProcessor::preprocessRawForFIR(samplesB, cfgBD, "B->D");
    QVector<double> rawAC = SignalProcessor::preprocessRawForFIR(samplesC, cfgAC, "A->C");
    QVector<double> rawAD = SignalProcessor::preprocessRawForFIR(samplesD, cfgAD, "A->D");

    // 3. 再做 FIR 滤波
    QVector<double> filBC = signalProcessor.applyFIRDouble(rawBC);
    QVector<double> filBD = signalProcessor.applyFIRDouble(rawBD);
    QVector<double> filAC = signalProcessor.applyFIRDouble(rawAC);
    QVector<double> filAD = signalProcessor.applyFIRDouble(rawAD);

    int n = std::min(
        std::min(filBC.size(), filBD.size()),
        std::min(filAC.size(), filAD.size())
        );

    if (n <= 0) return;

    // 4. 打印滤波后数据。这里仍然加 2048 打印，方便看图
    printRangeFormatted("B->C / filBC", filBC, 0, 1499);
    printRangeFormatted("B->D / filBD", filBD, 0, 1499);
    printRangeFormatted("A->C / filAC", filAC, 0, 1499);
    printRangeFormatted("A->D / filAD", filAD, 0, 1499);

    // 5. 声速检测
    detectAndPlotSpeed(filBC, filBD, filAC, filAD);

    // 6. 画图
    QVector<QPointF> ptsA, ptsB, ptsC, ptsD;
    ptsA.reserve(n);
    ptsB.reserve(n);
    ptsC.reserve(n);
    ptsD.reserve(n);

    for (int i = 0; i < n; ++i) {
        ptsA.append(QPointF(i, filBC[i] + 2048.0));
        ptsB.append(QPointF(i, filBD[i] + 2048.0));
        ptsC.append(QPointF(i, filAC[i] + 2048.0));
        ptsD.append(QPointF(i, filAD[i] + 2048.0));
    }

    seriesA->replace(ptsA);
    seriesB->replace(ptsB);
    seriesC->replace(ptsC);
    seriesD->replace(ptsD);

    auto setFixedAxisY = [&](QChart *chart) {
        auto *axisY = qobject_cast<QValueAxis*>(chart->axisY());
        if (axisY) axisY->setRange(0, 4095);
    };

    setFixedAxisY(chartA);
    setFixedAxisY(chartB);
    setFixedAxisY(chartC);
    setFixedAxisY(chartD);

    auto updateAxisX = [&](QChart *chart, int count) {
        auto *axisX = qobject_cast<QValueAxis*>(chart->axisX());
        if (axisX) axisX->setRange(0, count - 1);
    };

    updateAxisX(chartA, n);
    updateAxisX(chartB, n);
    updateAxisX(chartC, n);
    updateAxisX(chartD, n);
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
    // 1. 四路统一找“第一个明显波谷”
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

void MainWindow::appendSpeedPoint(double speedAvg)
{
    seriesSpeed->append(speedPointIndex, speedAvg);
    speedPointIndex++;

    auto *axisX = qobject_cast<QValueAxis*>(chartSpeed->axisX());
    auto *axisY = qobject_cast<QValueAxis*>(chartSpeed->axisY());

    if (axisX) {
        if (speedPointIndex < 50) {
            axisX->setRange(0, 50);
        } else {
            axisX->setRange(speedPointIndex - 50, speedPointIndex);
        }

        axisX->setTickCount(6);
    }

    if (axisY) {
        axisY->setRange(2000, 5000);
        axisY->setTickCount(4);
        axisY->setLabelFormat("%.0f");
    }
}

void MainWindow::detectAndPlotSpeed(const QVector<double>& filBC,
                                    const QVector<double>& filBD,
                                    const QVector<double>& filAC,
                                    const QVector<double>& filAD)
{
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

    qDebug() << "FirstArrivalSmart:"
             << "BC.onset =" << pickBC.onset << "BC.peak =" << pickBC.peak
             << "BD.onset =" << pickBD.onset << "BD.peak =" << pickBD.peak
             << "AC.onset =" << pickAC.onset << "AC.peak =" << pickAC.peak
             << "AD.onset =" << pickAD.onset << "AD.peak =" << pickAD.peak;

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
        "B_pair / BD->BC"
        );

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

    if (diffLag > lagToleranceAB) {
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
    // 但它现在表示“正式采用的 SOS”。
    double sosAvg = sosPatient;

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

    // ======================================================
    // 新增：角度/姿态特征调试输出
    //
    // 只用于研究 A/B 绝对位置关系，不影响正式测量结果。
    // 后面我们要用这些数据判断：
    // 3600、3800、4000 三种桡骨角度状态到底有什么区别。
    // ======================================================
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
        bool corrOk = (bRes.corr >= frameCorrBMin && aRes.corr >= frameCorrAMin);


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
            (signedLagDiff >= angleSignedDiffMin &&
             signedLagDiff <= angleSignedDiffMax);

        bool anglePairMidGapOk =
            (pairMidGap >= anglePairMidGapMin &&
             pairMidGap <= anglePairMidGapMax);

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

        if (bJumpOk &&
            notBoundary &&
            diffOk &&
            directionOk &&
            corrOk &&
            angleOk) {

            stableOk = checkBoneLagStable(
                bRes.refinedLag,
                &stableCenter,
                &stableCount
                );

        } else {
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


        qDebug() << "PatientValidCheck:"
                 << "lagA =" << aRes.refinedLag
                 << "lagB =" << bRes.refinedLag
                 << "diffLag =" << diffLagForCheck
                 << "signedLagDiff =" << signedLagDiff
                 << "pairMidB =" << pairMidB
                 << "pairMidA =" << pairMidA
                 << "pairMidGap =" << pairMidGap
                 << "angleSignedDiffRange = ["
                 << angleSignedDiffMin << "," << angleSignedDiffMax << "]"
                 << "anglePairMidGapRange = ["
                 << anglePairMidGapMin << "," << anglePairMidGapMax << "]"
                 << "bRoughLag =" << bRes.roughLag
                 << "bJump =" << bLagJump
                 << "corrA =" << aRes.corr
                 << "corrB =" << bRes.corr
                 << "frameCorrAMin =" << frameCorrAMin
                 << "frameCorrBMin =" << frameCorrBMin
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

void MainWindow::resetBoneLagStability()
{
    recentBoneLagBList.clear();

    boneLagLocked = false;
    lockedBoneLagCenter = 0;

    boneLagOutOfLockCount = 0;

    qDebug() << "Bone lag stability reset.";
}

bool MainWindow::checkBoneLagStable(int lagB, int* centerOut, int* countOut)
{
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

        if (recentBoneLagBList.size() < stableLagWarmupCount) {
            if (centerOut) *centerOut = lagB;
            if (countOut) *countOut = recentBoneLagBList.size();

            qDebug() << "BoneLagStable:"
                     << "lagB =" << lagB
                     << "recentCount =" << recentBoneLagBList.size()
                     << "warmupNeed =" << stableLagWarmupCount
                     << "locked = false"
                     << "stable = false, warmup";

            return false;
        }

        // 用最近 stableLagWarmupCount 个候选的中位数作为锁定中心。
        // 注意：这里不再选择最小 lag，也就是不再追逐最高声速簇。
        QVector<int> sorted = recentBoneLagBList;
        std::sort(sorted.begin(), sorted.end());

        int center = sorted[sorted.size() / 2];

        int countAroundCenter = 0;
        for (int v : recentBoneLagBList) {
            if (std::abs(v - center) <= stableLagTolerance) {
                countAroundCenter++;
            }
        }

        if (centerOut) *centerOut = center;
        if (countOut) *countOut = countAroundCenter;

        // 候选还不够集中，不锁定，不让进度条动。
        if (countAroundCenter < stableLagLockNeedCount) {
            qDebug() << "BoneLagStable:"
                     << "lagB =" << lagB
                     << "window =" << recentBoneLagBList
                     << "candidateCenter =" << center
                     << "countAroundCenter =" << countAroundCenter
                     << "need =" << stableLagLockNeedCount
                     << "tolerance =" << stableLagTolerance
                     << "locked = false"
                     << "stable = false, not concentrated";

            return false;
        }

        // 锁定稳定簇
        boneLagLocked = true;
        lockedBoneLagCenter = center;
        boneLagOutOfLockCount = 0;

        bool currentInCluster =
            (std::abs(lagB - lockedBoneLagCenter) <= stableLagTolerance);

        qDebug() << "BoneLagStable:"
                 << "LOCKED"
                 << "lockedCenter =" << lockedBoneLagCenter
                 << "countAroundCenter =" << countAroundCenter
                 << "currentLagB =" << lagB
                 << "currentInCluster =" << currentInCluster;

        // 锁定这一帧，如果当前点也在簇内，就允许有效。
        return currentInCluster;
    }

    // ======================================================
    // 3. 已经锁定稳定簇：只接受锁定簇附近的帧
    // ======================================================
    bool currentInCluster =
        (std::abs(lagB - lockedBoneLagCenter) <= stableLagTolerance);

    int countAroundLocked = 0;
    for (int v : recentBoneLagBList) {
        if (std::abs(v - lockedBoneLagCenter) <= stableLagTolerance) {
            countAroundLocked++;
        }
    }

    if (centerOut) *centerOut = lockedBoneLagCenter;
    if (countOut) *countOut = countAroundLocked;

    if (currentInCluster) {
        boneLagOutOfLockCount = 0;

        qDebug() << "BoneLagStable:"
                 << "lagB =" << lagB
                 << "lockedCenter =" << lockedBoneLagCenter
                 << "countAroundLocked =" << countAroundLocked
                 << "tolerance =" << stableLagTolerance
                 << "currentInCluster = true"
                 << "stable = true";

        return true;
    }

    // 当前帧偏离锁定簇，不计入。
    boneLagOutOfLockCount++;

    qDebug() << "BoneLagStable:"
             << "lagB =" << lagB
             << "lockedCenter =" << lockedBoneLagCenter
             << "countAroundLocked =" << countAroundLocked
             << "tolerance =" << stableLagTolerance
             << "currentInCluster = false"
             << "outOfLockCount =" << boneLagOutOfLockCount
             << "unlockNeed =" << boneLagUnlockCount
             << "stable = false";

    // 如果连续多帧都偏离锁定簇，说明探头已经移到别的位置了。
    // 这时重新寻找稳定簇。注意：这里不清空已累计的有效值，
    // 只是停止让当前帧推动进度条；如果你希望更严格，也可以在这里清空当前轮。
    if (boneLagOutOfLockCount >= boneLagUnlockCount) {
        qDebug() << "BoneLagStable:"
                 << "unlock because too many out-of-cluster frames"
                 << "oldLockedCenter =" << lockedBoneLagCenter;

        recentBoneLagBList.clear();
        boneLagLocked = false;
        lockedBoneLagCenter = 0;
        boneLagOutOfLockCount = 0;
    }

    return false;
}

void MainWindow::handleSerialError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, "Serial Error", serial->errorString());
        serial->close();
        ui->connectButton->setText("Connect");
    }
}




void MainWindow::setupChart()
{
    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->setSpacing(2);
    vbox->setContentsMargins(2, 2, 2, 2);

    auto createChannel = [&](QLineSeries **seriesPtr,
                             QChart **chartPtr,
                             QChartView **viewPtr,
                             QSlider **sliderPtr) {
        // 1. 曲线
        *seriesPtr = new QLineSeries();

        QPen pen(QColor(0, 229, 255));
        pen.setWidth(2);
        (*seriesPtr)->setPen(pen);

        // 2. 图表
        *chartPtr = new QChart();
        (*chartPtr)->addSeries(*seriesPtr);
        (*chartPtr)->legend()->hide();

        // 关键：尽量压缩边距
        (*chartPtr)->setMargins(QMargins(0, 0, 0, 0));
        (*chartPtr)->layout()->setContentsMargins(0, 0, 0, 0);
        (*chartPtr)->setBackgroundRoundness(0);
        (*chartPtr)->setBackgroundBrush(QBrush(QColor(30, 30, 30)));

        // 3. X 轴：去掉 Sample Index 和数字标签，节省高度
        QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText("");
        axisX->setLabelFormat("%d");
        axisX->setLabelsVisible(false);       // 不显示横坐标数字
        axisX->setGridLineVisible(true);
        axisX->setGridLineColor(QColor(80, 80, 80));
        axisX->setLinePenColor(QColor(120, 120, 120));
        (*chartPtr)->setAxisX(axisX, *seriesPtr);

        // 4. Y 轴：去掉 Amplitude，只保留少量刻度
        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("");
        axisY->setRange(0, 4095);
        axisY->setTickCount(3);               // 只保留 0 / 中间 / 4095
        axisY->setLabelFormat("%.0f");
        axisY->setLabelsColor(Qt::white);
        axisY->setGridLineColor(QColor(80, 80, 80));
        axisY->setLinePenColor(QColor(120, 120, 120));
        (*chartPtr)->setAxisY(axisY, *seriesPtr);

        // 5. 图表视图
        *viewPtr = new QChartView(*chartPtr);
        (*viewPtr)->setRenderHint(QPainter::Antialiasing);
        (*viewPtr)->setStyleSheet("background: transparent;");
        (*viewPtr)->setMinimumHeight(90);
        (*viewPtr)->setMaximumHeight(125);

        // 6. 增益滑条
        *sliderPtr = new QSlider(Qt::Vertical);
        (*sliderPtr)->setRange(0, 1241);
        (*sliderPtr)->setValue(globalGain);
        (*sliderPtr)->setInvertedAppearance(false);
        (*sliderPtr)->setTickPosition(QSlider::NoTicks);
        (*sliderPtr)->setFixedWidth(24);
        (*sliderPtr)->setCursor(Qt::PointingHandCursor);

        QString sliderStyle = R"(
            QSlider:vertical {
                background: transparent;
                min-width: 24px;
            }
            QSlider::groove:vertical {
                background: #E0E0E0;
                width: 4px;
                border-radius: 2px;
                margin: 0px 10px;
            }
            QSlider::handle:vertical {
                background: #FFFFFF;
                border: 2px solid #409EFF;
                height: 14px;
                margin: 0 -5px;
                border-radius: 8px;
            }
            QSlider::handle:vertical:hover {
                background: #409EFF;
                border: 2px solid #409EFF;
            }
            QSlider::add-page:vertical {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                            stop:0 #409EFF, stop:1 #36D1DC);
                width: 4px;
                border-radius: 2px;
                margin: 0px 10px;
            }
            QSlider::sub-page:vertical {
                background: #E4E7ED;
                width: 4px;
                border-radius: 2px;
                margin: 0px 10px;
            }
        )";

        (*sliderPtr)->setStyleSheet(sliderStyle);

        connect(*sliderPtr,
                &QSlider::valueChanged,
                this,
                &MainWindow::onGainSliderChanged);

        // 7. 一行：左边图，右边滑条
        QHBoxLayout *hbox = new QHBoxLayout();
        hbox->setContentsMargins(2, 0, 2, 2);
        hbox->setSpacing(4);

        hbox->addWidget(*viewPtr, 1);
        hbox->addWidget(*sliderPtr, 0, Qt::AlignHCenter);

        vbox->addLayout(hbox);
    };

    // 注意：这里不再显示 A→C/A→D/B→C/B→D 文字，只创建四路图
    createChannel(&seriesA, &chartA, &viewA, &gainSliderA);
    createChannel(&seriesB, &chartB, &viewB, &gainSliderB);
    createChannel(&seriesC, &chartC, &viewC, &gainSliderC);
    createChannel(&seriesD, &chartD, &viewD, &gainSliderD);

    QWidget *container = new QWidget();
    container->setLayout(vbox);

    // 关键：不要再套 QScrollArea，避免右侧滚轮
    ui->verticalLayoutChart->setContentsMargins(0, 0, 0, 0);
    ui->verticalLayoutChart->setSpacing(0);
    ui->verticalLayoutChart->addWidget(container);
}


void MainWindow::onGainSliderChanged(int value)
{
    // 如果值没变，直接返回，避免多余信号
    if (globalGain == value)
        return;

    // 更新全局增益
    globalGain = static_cast<quint16>(value);

    // 四个滑条同步 —— 使用 QSignalBlocker 防止递归触发 valueChanged
    {
        QSignalBlocker b1(gainSliderA);
        QSignalBlocker b2(gainSliderB);
        QSignalBlocker b3(gainSliderC);
        QSignalBlocker b4(gainSliderD);

        gainSliderA->setValue(value);
        gainSliderB->setValue(value);
        gainSliderC->setValue(value);
        gainSliderD->setValue(value);
    }

    // 状态栏提示当前增益
    statusBar()->showMessage(
        QString("当前增益 = 0x%1 (%2)")
            .arg(globalGain, 4, 16, QLatin1Char('0'))
            .arg(globalGain),
        1500
        );
}



void MainWindow::on_btnPatientInfo_clicked()
{
    // 这个按钮现在 UI 显示为“开始检测”，objectName 仍然是 btnPatientInfo

    if (patientMeasureRunning) {
        // 正在检测中，不重复启动
        return;
    }

    // 关键：如果上一轮测完后弹了提示框，点击“开始检测”时先自动关掉
    closeRoundFinishedTip();

    // 没有当前病人信息：直接进入病人信息填写/导入页面
    // 不弹 OK 小窗口，也不自动开始测量
    if (!hasCurrentPatient()) {
        pendingStartAfterPatientInfo = false;

        clearNewForm();
        ui->stackedWidget->setCurrentWidget(ui->pagePatientSelect);
        return;
    }

    // 如果已经完成 5 次，再点开始检测就不继续测了
    if (roundSosList.size() >= normalMeasureRounds) {
        QMessageBox::information(this,
                                 "检测已完成",
                                 "当前被检者已经完成 5 次测量。\n"
                                 "如需重新测量，请在报表页面选择“重新测量”，或重新选择被检者。");
        return;
    }

    // 每次点击“开始检测”只测 1 次；
    // 前面完成的第 1 次、第 2 次……保存在 roundSosList 里，不会清空。
    startPatientMeasurement(normalMeasureRounds);
}

void MainWindow::on_btnPatientNewSave_clicked()
{
    PatientInfo p;
    p.name     = ui->eName->text().trimmed();
    p.id       = ui->eID->text().trimmed();
    p.gender   = ui->eGender->currentText();
    p.birthDay = ui->eBirth->date().toString("yyyy-MM-dd");
    p.height   = ui->eHeight->text().trimmed();
    p.weight   = ui->eWeight->text().trimmed();

    if (p.name.isEmpty() || p.id.isEmpty()) {
        QMessageBox::warning(this, "错误", "姓名和ID不能为空");
        return;
    }

    currentPatient = p;
    updateCurrentPatientUI();

    // 新病人开始，清空之前累计的 5 次测量数据
    resetAllPatientMeasurementData();

    pendingStartAfterPatientInfo = false;

    // 只回到主界面，等待你再次点击“开始检测”
    ui->stackedWidget->setCurrentWidget(ui->pageMain);
}

void MainWindow::on_btnBackToMain_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->pageMain);
}

void MainWindow::on_btnImportFromDB_clicked() {
    archiveMode = ImportMode;        // 标记为导入模式
    refreshTable(patientList);
    ui->stackedWidget->setCurrentWidget(ui->pageArchive);
}

void MainWindow::updateCurrentPatientUI() {
    ui->labelName->setText("姓名: " + currentPatient.name);
    ui->labelID->setText("ID: " + currentPatient.id);
    ui->labelGender->setText("性别: " + currentPatient.gender);
    ui->labelBirth->setText("出生日期: " + currentPatient.birthDay);
    ui->labelHeight->setText("身高: " + currentPatient.height);
    ui->labelWeight->setText("体重: " + currentPatient.weight);

}



void MainWindow::on_btnSaveResult_clicked() {
    if (currentPatient.id.isEmpty()) {
        QMessageBox::warning(this, "错误", "请先录入患者信息");
        return;
    }

    // ✅ 更新检查日期
    currentPatient.checkDate = QDate::currentDate().toString("yyyy-MM-dd");

    // ❗ 这里是未来你计算出声速后赋值的地方
    // currentPatient.speedOfSound = QString::number(calculatedSpeed);

    // ✅ 如果 ID 和检查部位都相同 → 覆盖，否则新建
    bool found = false;
    for (int i = 0; i < patientList.size(); ++i) {
        if (patientList[i].id == currentPatient.id) {
            patientList[i] = currentPatient; // 覆盖同一个人同一个部位
            found = true;
            break;
        }
    }
    if (!found) {
        patientList.push_back(currentPatient); // 新建另一行（相同人不同部位）
    }

    savePatients();
    QMessageBox::information(this, "成功", "测量结果已保存");
}

void MainWindow::setupSpeedChart()
{
    // ======================================================
    // 1. 创建声速曲线
    // ======================================================
    seriesSpeed = new QLineSeries();
    seriesSpeed->setName("声速趋势");

    QPen pen(QColor(255, 215, 0));   // 黄色线
    pen.setWidth(2);
    seriesSpeed->setPen(pen);

    // ======================================================
    // 2. 创建图表
    // ======================================================
    chartSpeed = new QChart();
    chartSpeed->addSeries(seriesSpeed);

    // 节省空间：不显示标题、不显示图例
    chartSpeed->setTitle("");
    chartSpeed->legend()->hide();

    // 压缩边距
    chartSpeed->setMargins(QMargins(2, 2, 2, 2));
    chartSpeed->setBackgroundRoundness(0);

    // 深色背景
    chartSpeed->setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    chartSpeed->setPlotAreaBackgroundBrush(QBrush(QColor(30, 30, 30)));
    chartSpeed->setPlotAreaBackgroundVisible(true);

    // ======================================================
    // 3. X轴：时间/次数
    // ======================================================
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("");
    axisX->setRange(0, 50);
    axisX->setTickCount(6);          // 0,10,20,30,40,50
    axisX->setLabelFormat("%d");

    axisX->setLabelsColor(Qt::white);
    axisX->setGridLineColor(QColor(80, 80, 80));
    axisX->setLinePenColor(QColor(120, 120, 120));

    QFont fontX = axisX->labelsFont();
    fontX.setPointSize(8);
    axisX->setLabelsFont(fontX);

    chartSpeed->addAxis(axisX, Qt::AlignBottom);
    seriesSpeed->attachAxis(axisX);

    // ======================================================
    // 4. Y轴：声速范围固定 2000~5000
    // ======================================================
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("");
    axisY->setRange(2000, 5000);
    axisY->setTickCount(4);          // 2000,3000,4000,5000
    axisY->setLabelFormat("%.0f");

    axisY->setLabelsColor(Qt::white);
    axisY->setGridLineColor(QColor(80, 80, 80));
    axisY->setLinePenColor(QColor(120, 120, 120));

    QFont fontY = axisY->labelsFont();
    fontY.setPointSize(8);
    axisY->setLabelsFont(fontY);

    chartSpeed->addAxis(axisY, Qt::AlignLeft);
    seriesSpeed->attachAxis(axisY);

    // ======================================================
    // 5. 绑定到 UI
    // ======================================================
    ui->chartViewSpeed->setChart(chartSpeed);
    ui->chartViewSpeed->setRenderHint(QPainter::Antialiasing);
    ui->chartViewSpeed->setStyleSheet("background: transparent;");
}

void MainWindow::setupSpeedDebugPanel()
{
    QWidget *host = ui->chartViewSpeed->parentWidget();

    if (!host) {
        return;
    }

    QFrame *panel = new QFrame(host);
    panel->setObjectName("speedDebugPanel");
    panel->setFrameShape(QFrame::StyledPanel);
    panel->setMaximumHeight(48);
    panel->setMinimumHeight(42);

    panel->setStyleSheet(R"(
        QFrame#speedDebugPanel {
            background-color: #FFFFFF;
            border: 1px solid #DCDFE6;
            border-radius: 6px;
        }
        QLabel {
            border: none;
            background: transparent;
            color: #303133;
        }
    )");

    QGridLayout *grid = new QGridLayout(panel);
    grid->setContentsMargins(6, 2, 6, 2);
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(1);

    QLabel *title = new QLabel("声速调试值");
    title->setStyleSheet("font-size: 11px; font-weight: bold; color: #409EFF;");

    lblSosA = new QLabel("A：-- m/s");
    lblSosB = new QLabel("B：-- m/s");
    lblSosAvg = new QLabel("平均：-- m/s");
    lblSosInfo = new QLabel("等待测量...");

    QString valueStyle = "font-size: 11px; font-weight: bold; color: #303133;";
    lblSosA->setStyleSheet(valueStyle);
    lblSosB->setStyleSheet(valueStyle);
    lblSosAvg->setStyleSheet(valueStyle);
    lblSosInfo->setStyleSheet("font-size: 10px; color: #606266;");

    grid->addWidget(title,     0, 0);
    grid->addWidget(lblSosA,   1, 0);
    grid->addWidget(lblSosB,   1, 1);
    grid->addWidget(lblSosAvg, 1, 2);
    grid->addWidget(lblSosInfo, 2, 0, 1, 3);

    // 优先插入到 chartViewSpeed 所在布局里
    QLayout *layout = host->layout();

    if (QBoxLayout *box = qobject_cast<QBoxLayout*>(layout)) {
        int idx = box->indexOf(ui->chartViewSpeed);

        if (idx >= 0) {
            box->insertWidget(idx, panel, 0);
        } else {
            box->insertWidget(0, panel, 0);
        }

        return;
    }

    // 如果父控件没有布局，退回固定位置，但注意 parent 是 chartViewSpeed 的父控件，
    // 不是 pageMain，所以不会再跑到窗口最上面。
    QRect oldRect = ui->chartViewSpeed->geometry();

    int panelHeight = 46;
    int gap = 4;

    panel->setGeometry(oldRect.x(),
                       oldRect.y(),
                       oldRect.width(),
                       panelHeight);

    ui->chartViewSpeed->setGeometry(oldRect.x(),
                                    oldRect.y() + panelHeight + gap,
                                    oldRect.width(),
                                    qMax(120, oldRect.height() - panelHeight - gap));

    panel->show();
}

void MainWindow::updateSpeedDebugPanel(double sosA,
                                       double sosB,
                                       double sosAvg,
                                       int lagA,
                                       int lagB,
                                       int diffLag,
                                       double corrA,
                                       double corrB)
{
    if (!lblSosA || !lblSosB || !lblSosAvg || !lblSosInfo) {
        return;
    }

    lblSosA->setText(QString("A：%1").arg(sosA, 0, 'f', 0));
    lblSosB->setText(QString("B：%1").arg(sosB, 0, 'f', 0));
    lblSosAvg->setText(QString("平均：%1 m/s").arg(sosAvg, 0, 'f', 0));

    lblSosInfo->setText(
        QString("lagA=%1  lagB=%2  diff=%3  corrA=%4  corrB=%5")
            .arg(lagA)
            .arg(lagB)
            .arg(diffLag)
            .arg(corrA, 0, 'f', 2)
            .arg(corrB, 0, 'f', 2)
        );

    lblSosInfo->setStyleSheet("font-size: 10px; color: #67C23A;");
}

void MainWindow::setSpeedDebugInvalid(const QString& reason)
{
    if (!lblSosA || !lblSosB || !lblSosAvg || !lblSosInfo) {
        return;
    }

    lblSosInfo->setText("无效：" + reason);
    lblSosInfo->setStyleSheet("font-size: 10px; color: #F56C6C;");
}

void MainWindow::initProcessPanel()
{
    processValidCount = 0;

    // ======================================================
    // 1. 两个竖向进度条
    // ======================================================
    ui->barPairA->setRange(0, 1000);
    ui->barPairB->setRange(0, 1000);

    // 平衡状态：两个都在 50%
    ui->barPairA->setValue(500);
    ui->barPairB->setValue(500);

    // 如果运行后发现绿色条方向反了，就把 false 改成 true
    ui->barPairA->setInvertedAppearance(false);
    ui->barPairB->setInvertedAppearance(false);

    // ======================================================
    // 2. 横向有效测量进度条
    // ======================================================
    ui->barMeasureProgress->setRange(0, processValidTarget);
    ui->barMeasureProgress->setValue(0);
    ui->barMeasureProgress->setFormat("有效值：%v / %m");
    ui->barMeasureProgress->setTextVisible(true);

    // ======================================================
    // 3. 文本初始化
    // ======================================================
    ui->lblPairAValue->setText("D=--\n目标=10.0");
    ui->lblPairBValue->setText("G=--\n目标=0.0");
    ui->lblProcessStatus->setText("等待开始测量");

    // ======================================================
    // 4. 进度条样式
    // ======================================================
    QString barStyle = R"(
        QProgressBar {
            border: 1px solid #BFCBD9;
            border-radius: 4px;
            background-color: #EEEEEE;
            text-align: center;
        }
        QProgressBar::chunk {
            background-color: #00C853;
            border-radius: 3px;
        }
    )";

    ui->barPairA->setStyleSheet(barStyle);
    ui->barPairB->setStyleSheet(barStyle);
    ui->barMeasureProgress->setStyleSheet(barStyle);

    // ======================================================
    // 5. 给两个竖条加 50% 中线
    // ======================================================
    QTimer::singleShot(0, this, [this]() {
        addMiddleLineToProgressBar(ui->barPairA);
        addMiddleLineToProgressBar(ui->barPairB);
    });
}

void MainWindow::addMiddleLineToProgressBar(QProgressBar *bar)
{
    if (!bar) {
        return;
    }

    // 防止重复添加中线
    if (bar->findChild<QFrame*>("middleLine")) {
        return;
    }

    QFrame *line = new QFrame(bar);
    line->setObjectName("middleLine");
    line->setFrameShape(QFrame::NoFrame);
    line->setStyleSheet("background-color: #E6A23C;");

    int w = bar->width();
    int h = bar->height();

    if (w <= 0 || h <= 0) {
        // 如果此时控件还没布局完成，稍后再试一次
        QPointer<QProgressBar> safeBar(bar);

        QTimer::singleShot(100, this, [this, safeBar]() {
            if (safeBar) {
                addMiddleLineToProgressBar(safeBar.data());
            }
        });

        return;
    }

    // 50% 中线
    line->setGeometry(0, h / 2 - 1, w, 2);
    line->raise();
    line->show();
}

void MainWindow::updateProcessPanel(double sosA,
                                    double sosB,
                                    int lagA,
                                    int lagB,
                                    int diffLag,
                                    double pairMidGap,
                                    bool countThisFrame)
{
    // ======================================================
    // 1. 两个竖条：显示探头角度 / 姿态平衡
    //

    auto mapToBar = [](double value,
                       double target,
                       double fullScale,
                       double maxOffset,
                       double power) -> int {
        double dev = value - target;
        double norm = std::abs(dev) / fullScale;
        norm = qBound(0.0, norm, 1.0);

        double curved = std::pow(norm, power);
        int offset = qRound(maxOffset * curved);
        offset = qBound(0, offset, 480);

        if (dev > 0.0) {
            return 500 + offset;
        } else if (dev < 0.0) {
            return 500 - offset;
        }

        return 500;
    };

    // 左竖条：D = lagA - lagB
    int dBar = mapToBar(
        lagA - lagB,
        angleSignedDiffTarget,
        6.0,
        320.0,
        1.4
        );

    // 右竖条：G = pairMidGap
    int gBar = mapToBar(
        pairMidGap,
        anglePairMidGapTarget,
        12.0,
        320.0,
        1.4
        );

    ui->barPairA->setValue(qBound(0, dBar, 1000));
    ui->barPairB->setValue(qBound(0, gBar, 1000));

    // ======================================================
    // 2. 标签显示
    // ======================================================
    int signedLagDiff = lagA - lagB;

    ui->lblPairAValue->setText(
        QString("D=%1\n目标=%2")
            .arg(signedLagDiff)
            .arg(angleSignedDiffTarget, 0, 'f', 1)
        );

    ui->lblPairBValue->setText(
        QString("G=%1\n目标=%2")
            .arg(pairMidGap, 0, 'f', 1)
            .arg(anglePairMidGapTarget, 0, 'f', 1)
        );

    // ======================================================
    // 3. 横向进度条：只有 strictValid=true 才前进
    // ======================================================
    if (countThisFrame && processValidCount < processValidTarget) {
        processValidCount++;
    }

    ui->barMeasureProgress->setValue(processValidCount);

    // ======================================================
    // 4. 计算当前姿态是否在允许范围内
    // ======================================================
    bool angleSignedDiffOk =
        (signedLagDiff >= angleSignedDiffMin &&
         signedLagDiff <= angleSignedDiffMax);

    bool anglePairMidGapOk =
        (pairMidGap >= anglePairMidGapMin &&
         pairMidGap <= anglePairMidGapMax);

    bool angleOk =
        (!enablePatientAngleGate) ||
        (angleSignedDiffOk && anglePairMidGapOk);

    // ======================================================
    // 5. 状态文字
    // ======================================================
    if (processValidCount >= processValidTarget) {
        ui->lblProcessStatus->setText(
            QString("本次测量完成：有效值已满 %1/%2")
                .arg(processValidTarget)
                .arg(processValidTarget)
            );

        ui->lblProcessStatus->setStyleSheet(
            "font-size: 12px; color: #67C23A; font-weight: bold;"
            );
        return;
    }

    if (countThisFrame) {
        ui->lblProcessStatus->setText(
            QString("姿态有效：Gap=%1，目标=%2，lagA-lagB=%3，进度 %4/%5")
                .arg(pairMidGap, 0, 'f', 1)
                .arg(anglePairMidGapTarget, 0, 'f', 1)
                .arg(signedLagDiff)
                .arg(processValidCount)
                .arg(processValidTarget)
            );

        ui->lblProcessStatus->setStyleSheet(
            "font-size: 12px; color: #67C23A; font-weight: bold;"
            );
    } else {
        QString guide;

        double gDev = pairMidGap - anglePairMidGapTarget;
        double dDev = signedLagDiff - angleSignedDiffTarget;

        if (gDev > 3.0) {
            guide = "G偏大：当前偏向低速/左侧方向，请向右侧轻压或反方向微调";
        } else if (gDev < -3.0) {
            guide = "G偏小：当前偏向高速/右侧方向，请向左侧轻压或反方向微调";
        } else if (dDev < -1.5) {
            guide = "D偏小：A/B延迟关系偏向高速姿态，请轻微回调角度";
        } else if (dDev > 2.0) {
            guide = "D偏大：A/B延迟关系偏向低速姿态，请轻微反向调整";
        } else {
            guide = "角度接近目标，请保持不动等待稳定";
        }
        QString reason;

        if (!anglePairMidGapOk) {
            reason += QString("Gap=%1 不在 [%2,%3]，目标=%4；")
                          .arg(pairMidGap, 0, 'f', 1)
                          .arg(anglePairMidGapMin, 0, 'f', 1)
                          .arg(anglePairMidGapMax, 0, 'f', 1)
                          .arg(anglePairMidGapTarget, 0, 'f', 1);
        }

        if (!angleSignedDiffOk) {
            reason += QString("lagA-lagB=%1 不在 [%2,%3]；")
                          .arg(signedLagDiff)
                          .arg(angleSignedDiffMin, 0, 'f', 1)
                          .arg(angleSignedDiffMax, 0, 'f', 1);
        }

        if (reason.isEmpty()) {
            reason = "角度基本合格，但稳定性/相关性/lag窗口还未满足；";
        }

        ui->lblProcessStatus->setText(
            QString("未累计：%1\n%2")
                .arg(reason)
                .arg(guide)
            );
        ui->lblProcessStatus->setStyleSheet(
            "font-size: 12px; color: #F56C6C; font-weight: bold;"
            );
    }
}

void MainWindow::updateProcessInvalid(const QString& reason)
{
    ui->lblProcessStatus->setText("无效：" + reason);
    ui->lblProcessStatus->setStyleSheet(
        "font-size: 12px; color: #F56C6C; font-weight: bold;"
        );
}

void MainWindow::on_btnShowResult_clicked() {
    // 可以在这里写跳转逻辑
    // 比如：ui->stackedWidget->setCurrentWidget(ui->pageResult);

    // 暂时先弹窗提示，证明按钮好使
    QMessageBox::information(this, "结果", "正在跳转到测量结果分析界面...\n(界面开发中)");

    // 如果你想测试跳转到已有页面（比如档案页），可以解开下面这行：
    // ui->stackedWidget->setCurrentWidget(ui->pageArchive);
}


// ==================== 档案管理 ===============================================================================================
void MainWindow::on_btnArchive_clicked() {
    refreshTable(patientList);   // ✅ 进入档案页时刷新表格
    ui->stackedWidget->setCurrentWidget(ui->pageArchive);
}

void MainWindow::on_btnBackFromArchive_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->pageMain);
    archiveMode = NormalMode;
}

void MainWindow::loadPatients() {
    patientList.clear();
    QFile file(xmlFilePath);
    if (!file.open(QIODevice::ReadOnly)) return;
    QDomDocument doc;
    if (!doc.setContent(&file)) { file.close(); return; }
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("patient");
    for (int i = 0; i < nodes.count(); ++i) {
        QDomElement e = nodes.at(i).toElement();
        PatientInfo p;
        p.id        = e.attribute("id");
        p.name      = e.attribute("name");
        p.gender    = e.attribute("gender");
        p.birthDay  = e.attribute("birth");
        p.checkDate = e.attribute("checkDate");
        //p.checkPart = e.attribute("part");
        p.height    = e.attribute("height");
        p.weight    = e.attribute("weight");
        p.diagprompt= e.attribute("diag");
        p.speedOfSound = e.attribute("sos");
        patientList << p;
    }
    file.close();
}

void MainWindow::savePatients() {
    QDomDocument doc;
    QDomElement root = doc.createElement("patients");
    doc.appendChild(root);
    for (auto &p : patientList) {
        QDomElement e = doc.createElement("patient");
        e.setAttribute("id", p.id);
        e.setAttribute("name", p.name);
        e.setAttribute("gender", p.gender);
        e.setAttribute("birth", p.birthDay);
        e.setAttribute("checkDate", p.checkDate);
        e.setAttribute("part", "桡骨");
        e.setAttribute("height", p.height);
        e.setAttribute("weight", p.weight);
        e.setAttribute("diag", p.diagprompt);
        e.setAttribute("sos", p.speedOfSound);
        root.appendChild(e);
    }
    QFile file(xmlFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        doc.save(out, 4);
        file.close();
    }
}

void MainWindow::refreshTable(const QList<PatientInfo> &list) {
    ui->table->clear();

    // ✅ 设置6列：ID, 姓名, 性别, 出生日期, 检查日期, 声速
    // 注意：复选框将直接附加在第一列(ID列)上
    ui->table->setColumnCount(6);
    QStringList headers;
    headers << "编号(ID)" << "姓名" << "性别" << "出生日期" << "检查日期" << "声速(m/s)";
    ui->table->setHorizontalHeaderLabels(headers);

    ui->table->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i) {
        const PatientInfo &p = list[i];

        // --- 第一列：ID + 复选框 ---
        QTableWidgetItem *itemID = new QTableWidgetItem(p.id);
        // ⭐⭐⭐ 关键：设置 CheckState 为 Unchecked (未选中) ⭐⭐⭐
        itemID->setCheckState(Qt::Unchecked);
        // 设置为不可编辑，但可勾选，可选择
        itemID->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        ui->table->setItem(i, 0, itemID);
        // -------------------------

        ui->table->setItem(i, 1, new QTableWidgetItem(p.name));
        ui->table->setItem(i, 2, new QTableWidgetItem(p.gender));
        ui->table->setItem(i, 3, new QTableWidgetItem(p.birthDay));
        ui->table->setItem(i, 4, new QTableWidgetItem(p.checkDate));

        QString sosStr = p.speedOfSound.isEmpty() ? "-" : p.speedOfSound;
        ui->table->setItem(i, 5, new QTableWidgetItem(sosStr));
    }

    // 列宽设置
    ui->table->setColumnWidth(0, 250);
    ui->table->setColumnWidth(1, 250);
    ui->table->setColumnWidth(2, 100);
    ui->table->setColumnWidth(3, 300);
    ui->table->setColumnWidth(4, 300);
    ui->table->setColumnWidth(5, 200);

    // ⭐⭐⭐ 关键：取消整行选中行为，否则点击复选框容易触发双击事件或选中干扰 ⭐⭐⭐
    // 如果你希望能同时点选和勾选，可以保留 SelectRows，
    // 但为了让勾选操作更明确，通常可以保留 SelectRows。
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table->horizontalHeader()->setStretchLastSection(true);
}


void MainWindow::on_btnShowAll_clicked() {
    refreshTable(patientList);
}

// 1. 姓名搜索
void MainWindow::on_btnSearchName_clicked() {
    QString key = ui->editSearchKeyword->text().trimmed(); // 从文本框获取
    if (key.isEmpty()) {
        QMessageBox::information(this, "提示", "请输入姓名关键字");
        return;
    }
    QList<PatientInfo> result;
    // 支持模糊搜索
    for (auto &p : patientList) {
        if (p.name.contains(key, Qt::CaseInsensitive)) {
            result << p;
        }
    }
    refreshTable(result);
    //QMessageBox::information(this, "搜索结果", QString("找到 %1 条记录").arg(result.size()));
}

// 2. ID 搜索
void MainWindow::on_btnSearchID_clicked() {
    QString key = ui->editSearchKeyword->text().trimmed(); // 从同一个文本框获取
    if (key.isEmpty()) {
        QMessageBox::information(this, "提示", "请输入ID关键字");
        return;
    }
    QList<PatientInfo> result;
    for (auto &p : patientList) {
        // ID通常是精确匹配，如果想模糊匹配用 contains，精确用 ==
        if (p.id.contains(key, Qt::CaseInsensitive)) {
            result << p;
        }
    }
    refreshTable(result);
    //QMessageBox::information(this, "搜索结果", QString("找到 %1 条记录").arg(result.size()));
}

// 3. 日期搜索 (组合年月日)
void MainWindow::on_btnSearchDate_clicked() {
    // 组装日期字符串，格式必须与 XML 里存的格式一致 (yyyy-MM-dd)
    int year = ui->comboYear->currentData().toInt();
    int month = ui->comboMonth->currentData().toInt();
    int day = ui->comboDay->currentData().toInt();

    // 格式化为 "2026-01-05" 这种标准格式
    // %1: 年份
    // %2: 月份 (填充0到2位)
    // %3: 日期 (填充0到2位)
    QString searchDate = QString("%1-%2-%3")
                             .arg(year)
                             .arg(month, 2, 10, QLatin1Char('0'))
                             .arg(day, 2, 10, QLatin1Char('0'));

    qDebug() << "Searching date:" << searchDate;

    QList<PatientInfo> result;
    for (auto &p : patientList) {
        // 假设你的 p.checkDate 也是 "yyyy-MM-dd" 格式
        if (p.checkDate == searchDate) {
            result << p;
        }
    }
    refreshTable(result);

    if (result.isEmpty()) {
        QMessageBox::information(this, "搜索结果", "该日期没有检查记录");
    }
}

void MainWindow::on_btnDeleteSelected_clicked()
{
    int rowCount = ui->table->rowCount();
    int checkedCount = 0;

    // 1. 先统计选中了多少个，如果没有选则提示
    for (int i = 0; i < rowCount; ++i) {
        QTableWidgetItem *item = ui->table->item(i, 0);
        if (item && item->checkState() == Qt::Checked) {
            checkedCount++;
        }
    }

    if (checkedCount == 0) {
        QMessageBox::information(this, "提示", "请先勾选需要删除的病人记录");
        return;
    }

    // 2. 二次确认
    QString msg = QString("确定要删除选中的 %1 条记录吗？\n此操作不可恢复！").arg(checkedCount);
    if (QMessageBox::question(this, "确认删除", msg) != QMessageBox::Yes) {
        return;
    }

    // 3. 开始删除
    // 我们采用“标记删除法”：先找出所有要删除的目标特征，再从列表中移除

    // 定义一个结构体来暂存要删除的人的特征
    struct DeleteTarget {
        QString id;
        QString checkDate;
        QString name;
    };
    QList<DeleteTarget> targets;

    for (int i = 0; i < rowCount; ++i) {
        QTableWidgetItem *itemID = ui->table->item(i, 0); // 第0列 ID

        // 只有打了勾的行才处理
        if (itemID && itemID->checkState() == Qt::Checked) {
            DeleteTarget t;
            t.id = itemID->text();
            // 第4列是检查日期，第1列是姓名
            if (ui->table->item(i, 4)) t.checkDate = ui->table->item(i, 4)->text();
            if (ui->table->item(i, 1)) t.name = ui->table->item(i, 1)->text();

            targets.append(t);
        }
    }

    // 4. 从总数据库(patientList)中移除
    // 使用倒序遍历，防止删除元素后索引错位
    int deletedRealCount = 0;

    for (int i = patientList.size() - 1; i >= 0; --i) {
        const PatientInfo &p = patientList[i];

        // 遍历我们要删除的目标列表
        for (int j = 0; j < targets.size(); ++j) {
            // ⭐⭐⭐ 核心修改：ID、检查日期、姓名 必须全部匹配才删除 ⭐⭐⭐
            // 这样即使 ID 重复，只要日期或姓名不一样，就不会误删
            if (p.id == targets[j].id &&
                p.checkDate == targets[j].checkDate &&
                p.name == targets[j].name) {

                patientList.removeAt(i); // 从数据库列表中移除
                targets.removeAt(j);     // 把这个目标标记为已处理，提高效率
                deletedRealCount++;
                break; // 跳出内层循环，继续检查下一条数据库记录
            }
        }
    }

    // 5. 保存并刷新界面
    savePatients(); // 保存到 XML

    // 刷新表格显示（为了保持当前的搜索状态，建议重新触发一次搜索，或者直接刷新全部）
    // 这里简单起见，直接调用当前的搜索逻辑，如果没搜索就是显示全部
    QString key = ui->editSearchKeyword->text();
    if (key.isEmpty()) {
        refreshTable(patientList); // 显示全部
    } else {
        // 如果用户正在搜索状态下删除，保持搜索结果刷新会比较友好
        // 简单处理：直接调用显示全部即可，用户体验差别不大
        refreshTable(patientList);
    }

    QMessageBox::information(this, "成功", QString("已成功删除 %1 条记录").arg(deletedRealCount));
}

void MainWindow::on_btnAdd_clicked() {
    clearNewForm();
    ui->stackedWidget->setCurrentWidget(ui->pagePatientForm);
}

void MainWindow::on_table_cellDoubleClicked(int row, int)
{
    if (row < 0 || row >= ui->table->rowCount()) return;

    QTableWidgetItem* idItem   = ui->table->item(row, 0);
    QTableWidgetItem* dateItem = ui->table->item(row, 4);

    if (!idItem) return;

    QString id = idItem->text();
    QString date = dateItem ? dateItem->text() : "";

    int idx = -1;

    // 优先匹配 ID + 检查日期
    for (int i = 0; i < patientList.size(); ++i) {
        if (patientList[i].id == id && patientList[i].checkDate == date) {
            idx = i;
            break;
        }
    }

    // 如果没找到，再只按 ID 匹配
    if (idx == -1) {
        for (int i = 0; i < patientList.size(); ++i) {
            if (patientList[i].id == id) {
                idx = i;
                break;
            }
        }
    }

    if (idx == -1) {
        QMessageBox::warning(this, "错误", "未找到该行对应的内部数据");
        return;
    }

    switch (archiveMode) {
    case ImportMode:
        currentPatient = patientList[idx];
        updateCurrentPatientUI();

        // 导入新病人后，清空之前累计的测量数据
        resetAllPatientMeasurementData();

        pendingStartAfterPatientInfo = false;

        ui->stackedWidget->setCurrentWidget(ui->pageMain);
        archiveMode = NormalMode;

        // 注意：这里不再自动 startPatientMeasurement()
        // 等你回到主界面后，再手动点击“开始检测”
        break;

    case PrintMode:
        currentPatient = patientList[idx];
        QMessageBox::information(this, "打印", "选择了病人：" + currentPatient.name);
        ui->stackedWidget->setCurrentWidget(ui->pageMain);
        archiveMode = NormalMode;
        break;

    case NormalMode:
    default:
        fillDetailPage(patientList[idx], idx);
        ui->stackedWidget->setCurrentWidget(ui->pagePatientDetail);
        break;
    }
}




void MainWindow::clearNewForm() {
    ui->editName->clear();
    ui->editID->clear();
    ui->comboGender->setCurrentIndex(0);
    ui->dateBirth->setDate(QDate::currentDate());
    ui->dateCheck->setDate(QDate::currentDate());
    //ui->comboPart->setCurrentIndex(-1);
    ui->editHeight->clear();
    ui->editWeight->clear();
    ui->editDiag->clear();

}

void MainWindow::on_btnFormBack_clicked() {
    editingIndex = -1; // 返回时也重置状态
    ui->stackedWidget->setCurrentWidget(ui->pageArchive);
}

void MainWindow::on_btnFormSave_clicked() {
    // 1) 从新增表单页面采集数据（注意这些控件名都是 pagePatientForm 上的）
    PatientInfo p;
    p.name      = ui->editName->text().trimmed();
    p.id        = ui->editID->text().trimmed();
    p.gender    = ui->comboGender->currentText();
    p.birthDay  = ui->dateBirth->date().toString("yyyy-MM-dd");
    p.checkDate = ui->dateCheck->date().toString("yyyy-MM-dd");
    p.height    = ui->editHeight->text().trimmed();
    p.weight    = ui->editWeight->text().trimmed();
    p.diagprompt= ui->editDiag->text().trimmed();


    // 2) 基本校验
    if (p.name.isEmpty() || p.id.isEmpty()) {
        QMessageBox::warning(this, "缺少信息", "姓名和编号(ID)不能为空");
        return;
    }

    // 3) 新增 / 编辑 分支
    if (editingIndex == -1) {
        // 新增：校验 ID 唯一
        for (const auto &it : patientList) {
            if (it.id == p.id) {
                QMessageBox::warning(this, "重复ID", "该编号(ID)已存在，请更换。");
                return;
            }
        }
        patientList.push_back(p);
    } else {
        // 编辑
        patientList[editingIndex] = p;
    }

    // 4) 落盘并刷新
    savePatients();
    refreshTable(patientList);

    // 5) 复位状态并返回档案页
    editingIndex = -1;
    ui->stackedWidget->setCurrentWidget(ui->pageArchive);
}



void MainWindow::fillDetailPage(const PatientInfo& p, int index) {
    editingIndex = index;

    ui->dName->setText(p.name);
    ui->dID->setText(p.id);
    ui->dGender->setCurrentText(p.gender);
    ui->dBirth->setDate(QDate::fromString(p.birthDay, "yyyy-MM-dd"));
    ui->dCheck->setDate(QDate::fromString(p.checkDate, "yyyy-MM-dd"));
    ui->dHeight->setText(p.height);
    ui->dWeight->setText(p.weight);
    ui->dDiag->setText(p.diagprompt);
}

void MainWindow::on_btnDetailBack_clicked() {
    editingIndex = -1;
    ui->stackedWidget->setCurrentWidget(ui->pageArchive);
}

void MainWindow::on_btnDetailSave_clicked() {
    if (editingIndex < 0 || editingIndex >= patientList.size()) {
        QMessageBox::warning(this, "错误", "没有选中的病人记录");
        return;
    }

    // 读回编辑后的内容
    PatientInfo& p = patientList[editingIndex];
    QString newId = ui->dID->text().trimmed();
    if (ui->dName->text().trimmed().isEmpty() || newId.isEmpty()) {
        QMessageBox::warning(this, "缺少信息", "姓名和编号(ID)不能为空");
        return;
    }
    // 如果 ID 改了，检查是否与其他记录冲突
    if (newId != p.id) {
        for (int i=0; i<patientList.size(); ++i) {
            if (i != editingIndex && patientList[i].id == newId) {
                QMessageBox::warning(this, "重复ID", "该编号(ID)已存在，请更换");
                return;
            }
        }
    }

    p.name      = ui->dName->text().trimmed();
    p.id        = newId;
    p.gender    = ui->dGender->currentText();
    p.birthDay  = ui->dBirth->date().toString("yyyy-MM-dd");
    p.checkDate = ui->dCheck->date().toString("yyyy-MM-dd");
    p.height    = ui->dHeight->text().trimmed();
    p.weight    = ui->dWeight->text().trimmed();
    p.diagprompt= ui->dDiag->text().trimmed();


    // 你可以选择不改检查时间，也可以在每次保存时更新“最后修改”：
    // p.checkTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    savePatients();
    refreshTable(patientList);
    QMessageBox::information(this, "成功", "已保存修改");
}

void MainWindow::on_btnDetailDelete_clicked() {
    if (editingIndex < 0 || editingIndex >= patientList.size()) return;

    if (QMessageBox::question(this, "确认删除", "确定删除该病人信息？此操作不可恢复") != QMessageBox::Yes)
        return;

    patientList.removeAt(editingIndex);
    savePatients();
    refreshTable(patientList);

    editingIndex = -1;
    ui->stackedWidget->setCurrentWidget(ui->pageArchive);
}

void MainWindow::initSearchControls()
{
    // 1. 初始化年份 (从今年往前推到1900年)
    int currentYear = QDate::currentDate().year();
    ui->comboYear->clear();
    // 添加一个 "不限" 选项，方便用户只搜月份（可选）
    // ui->comboYear->addItem("不限", 0);

    for (int y = currentYear; y >= 1900; --y) {
        ui->comboYear->addItem(QString::number(y), y);
    }

    // 2. 初始化月份 (1-12)
    ui->comboMonth->clear();
    for (int m = 1; m <= 12; ++m) {
        ui->comboMonth->addItem(QString::number(m), m);
    }
    // 设置默认为当前月份
    ui->comboMonth->setCurrentIndex(QDate::currentDate().month() - 1);

    // 3. 初始化日 (联动)
    // 连接信号：当年份或月份改变时，重新计算天数
    connect(ui->comboYear, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::updateDayCombo);
    connect(ui->comboMonth, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::updateDayCombo);

    // 先手动触发一次以填充天数
    updateDayCombo();

    // 默认选中当天
    int currentDay = QDate::currentDate().day();
    int idx = ui->comboDay->findText(QString::number(currentDay));
    if (idx != -1) ui->comboDay->setCurrentIndex(idx);
}

void MainWindow::updateDayCombo()
{
    // 获取当前选中的年月
    int year = ui->comboYear->currentData().toInt();
    int month = ui->comboMonth->currentData().toInt();

    // 记录之前选中的天数，防止刷新后跳变
    QString currentDayText = ui->comboDay->currentText();

    ui->comboDay->clear();

    // 计算该月有多少天
    int daysInMonth = QDate(year, month, 1).daysInMonth();

    for (int d = 1; d <= daysInMonth; ++d) {
        ui->comboDay->addItem(QString::number(d), d);
    }

    // 尝试恢复之前选中的天数（如果新月份也有这一天）
    int idx = ui->comboDay->findText(currentDayText);
    if (idx != -1) {
        ui->comboDay->setCurrentIndex(idx);
    }
}
