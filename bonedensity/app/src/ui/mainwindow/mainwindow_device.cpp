// 串口连接、帧解析与设备响应提示

#include "mainwindow/mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogs/calibrationdialog.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QComboBox>
#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSettings>
#include <QSignalBlocker>
#include <QStatusBar>
#include "theme/theme.h"

void MainWindow::resetFrameAssembly()
{
    frameAssembler.clear();
    currentFrame.clear();
}

void MainWindow::restartFrameStream()
{
    resetFrameAssembly();
    serial->readAll();
}

void MainWindow::resetDisconnectedAcquisitionState()
{
    cancelPendingNextPatientRound();
    if (experimentLog.active()) {
        experimentLog.write({{"event", "disconnected"}});
        experimentLog.close();
        checkExperimentLogError();
    }
    if (autoTimer && autoTimer->isActive()) autoTimer->stop();
    autoRunning = false;
    patientMeasureRunning = false;
    acquireMode = DebugAcquireMode;
    resetFrameAssembly();
    awaitingDeviceFrame = false;
    deviceUnresponsive = false;
    ui->triggerButton->setText(QStringLiteral("自动采集"));
    updatePatientSelectionUi();
}

namespace {

// The lower unit is a Raspberry Pi Pico 2 running the Pico SDK USB serial
// (firmware 骨密度仪/hhhh111): VID 0x2E8A, PID 0x0009. Windows names every such
// port "USB 串行设备", so it is recognised by these IDs. The Raspberry Pi Debug
// Probe (PID 0x000C) and other Raspberry Pi devices do not match.
bool isDensitometerPort(const QSerialPortInfo& info)
{
    return info.hasVendorIdentifier() && info.vendorIdentifier() == 0x2E8A &&
           info.hasProductIdentifier() && info.productIdentifier() == 0x0009;
}

QString portLabel(const MainWindow::SerialPortChoice& port)
{
    if (port.densitometer) return QStringLiteral("骨密度仪（%1）").arg(port.name);
    if (port.description.isEmpty()) return port.name;
    return port.name + QStringLiteral(" - ") + port.description;
}

} // namespace

void MainWindow::scanPorts() {
    QList<SerialPortChoice> ports;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ports.append({info.portName(), info.description(), isDensitometerPort(info)});
    // The densitometer first, so it is the obvious choice in the drop-down.
    std::stable_partition(ports.begin(), ports.end(),
                          [](const SerialPortChoice& port) { return port.densitometer; });

    applyPortList(ports, preferredPort(ports));
    const QString target = nextAutoConnectPort(ports);
    if (!target.isEmpty()) autoConnect(target);
}

// The densitometer (the one used last time if several are plugged in);
// without one, the port that worked last time, then any USB serial port.
QString MainWindow::preferredPort(const QList<SerialPortChoice>& ports) const
{
    const QString remembered = rememberedPort();
    QString device;
    for (const SerialPortChoice& port : ports) {
        if (!port.densitometer) continue;
        if (port.name == remembered) return remembered;
        if (device.isEmpty()) device = port.name;
    }
    if (!device.isEmpty()) return device;
    for (const SerialPortChoice& port : ports) {
        if (port.name == remembered) return remembered;
    }
    for (const SerialPortChoice& port : ports) {
        if (port.description.contains(QStringLiteral("USB"), Qt::CaseInsensitive)) return port.name;
    }
    return QString();
}

QString MainWindow::nextAutoConnectPort(const QList<SerialPortChoice>& ports)
{
    QStringList devices;
    for (const SerialPortChoice& port : ports) {
        if (port.densitometer) devices.append(port.name);
    }
    // Two densitometers: the user has to choose.
    if (devices.size() != 1) {
        autoConnectCandidate.clear();
        return QString();
    }
    if (devices.first() != autoConnectCandidate) {
        // Newly plugged in. Windows may need a moment before the port opens.
        autoConnectCandidate = devices.first();
        autoConnectTriesLeft = 3;
    }
    if (!autoConnectEnabled || serial->isOpen() || autoConnectTriesLeft <= 0) return QString();
    --autoConnectTriesLeft;
    return autoConnectCandidate;
}

void MainWindow::autoConnect(const QString& portName)
{
    const int index = ui->comboPort->findData(portName);
    if (index >= 0) ui->comboPort->setCurrentIndex(index);
    if (openDevicePort(portName)) {
        statusBar()->showMessage(QStringLiteral("已自动连接骨密度仪（%1）。").arg(portName), 5000);
    } else if (autoConnectTriesLeft <= 0) {
        statusBar()->showMessage(
            QStringLiteral("骨密度仪（%1）自动连接失败，端口可能被其他程序占用。关闭该程序后点击“连接”。")
                .arg(portName), 10000);
    }
}

QString MainWindow::rememberedPort() const
{
    return QSettings(deviceSettingsPath, QSettings::IniFormat)
        .value(QStringLiteral("serial/lastPort")).toString();
}

void MainWindow::rememberPort(const QString& portName)
{
    QSettings settings(deviceSettingsPath, QSettings::IniFormat);
    settings.setValue(QStringLiteral("serial/lastPort"), portName);
}

// Rebuild the port list only when it actually changed and the user is not
// looking at the open drop-down, so the selection never flickers away.
void MainWindow::applyPortList(const QList<SerialPortChoice>& ports,
                               const QString& preferred)
{
    QComboBox* combo = ui->comboPort;
    bool unchanged = combo->count() == ports.size();
    for (int i = 0; unchanged && i < ports.size(); ++i) {
        unchanged = combo->itemData(i).toString() == ports[i].name &&
                    combo->itemText(i) == portLabel(ports[i]);
    }
    if (unchanged) return;
    if (combo->view() && combo->view()->isVisible()) return;

    const QString current = combo->currentData().toString();
    QSignalBlocker blocker(combo);
    combo->clear();
    for (const SerialPortChoice& port : ports) {
        // 文本：骨密度仪（COM11）或 COM3 - 蓝牙串口；data 只存端口名
        combo->addItem(portLabel(port), port.name);
        combo->setItemData(combo->count() - 1, port.description, Qt::ToolTipRole);
    }
    int idx = combo->findData(current);
    if (idx < 0 && !preferred.isEmpty()) idx = combo->findData(preferred);
    if (idx >= 0) combo->setCurrentIndex(idx);
}


void MainWindow::on_connectButton_clicked() {
    if (!serial->isOpen()) {

        QString portName = ui->comboPort->currentData().toString();
        if (portName.isEmpty()) {
            QMessageBox::warning(this, "无法连接设备", "请先选择骨密度仪端口，再点击“连接”。");
            return;
        }

        if (!openDevicePort(portName)) {
            QMessageBox::warning(this,
                                 "无法连接设备",
                                 "设备连接失败。\n请检查 USB 是否插好、设备是否被其他程序占用，然后重试。");
            return;
        }

    } else {
        serial->close();
        serialErrorHandled = false;
        autoConnectTriesLeft = 0;   // a manual disconnect stays disconnected
        ui->connectButton->setText("连接");
        resetDisconnectedAcquisitionState();
    }
}

bool MainWindow::openDevicePort(const QString& portName)
{
    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    serial->setReadBufferSize(0);              // 可选

    // ⭐⭐⭐ 关键一行：关闭文本模式的 LF ↔ CRLF 自动转换
    serial->setTextModeEnabled(false);   // ⭐⭐ 最关键！！！

    if (!serial->open(QIODevice::ReadWrite)) return false;

    rememberPort(portName);
    autoConnectTriesLeft = 0;
    serial->setDataTerminalReady(true); // 拉高 DTR，告诉 Pico "我准备好了"
    serial->setRequestToSend(false);     // 拉高 RTS (部分固件也需要这个)

    // ★★ 必须加的三行，解决 USB CDC 卡住的问题
    serial->clear(QSerialPort::AllDirections);
    serial->flush();
    serial->readAll();
    serial->waitForReadyRead(10);
    serial->readAll();

    resetFrameAssembly();
    serialErrorHandled = false;

    qDebug() << "Serial opened on" << portName;
    qDebug() << "Serial actually opened:" << serial->isOpen();
    qDebug() << "Error:" << serial->error();

    ui->connectButton->setText("断开连接");

    // 允许重复连接流程，但同一串口只保留一个 readyRead 连接。
    connect(serial, &QSerialPort::readyRead,
            this, &MainWindow::handleSerialReadyRead,
            Qt::UniqueConnection);
    updatePatientSelectionUi();
    return true;
}



void MainWindow::on_triggerButton_clicked()
{
    if (patientMeasureRunning) {
        stopPatientMeasurementManually();
        return;
    }

    acquireMode = DebugAcquireMode;

    if (!autoRunning) {
        autoTimer->start(80);
        ui->triggerButton->setText("停止自动采集");
        autoRunning = true;
    } else {
        autoTimer->stop();
        ui->triggerButton->setText("自动采集");
        autoRunning = false;
    }
    updatePatientSelectionUi();
}

void MainWindow::sendCmd() {
    if (!serial->isOpen()) return;

    serial->write(DeviceProtocol::acquireCommand(globalGain, nextFrameIdx++));
    noteCommandSent();
    // 注意：自动模式下不要加 waitForBytesWritten，会阻塞界面
    // 也不要在这里清空 frameAssembler，否则会把正在接收的数据清掉
}

//=====================发命令==============================================================
void MainWindow::on_btnAcquireWaveform_clicked()
{
    if (patientMeasureRunning) {
        stopPatientMeasurementManually();
        return;
    }

    acquireMode = DebugAcquireMode;

    if (!serial->isOpen()) {
        QMessageBox::warning(this, "设备未连接", "请先点击“连接”，连接设备后再获取波形。");
        return;
    }

    const QByteArray cmd = DeviceProtocol::acquireCommand(globalGain, nextFrameIdx++);
    resetFrameAssembly();
    serial->write(cmd);
    noteCommandSent();
    serial->waitForBytesWritten(50);

    qDebug() << "TX CMD" << cmd.toHex(' ');
}


void MainWindow::handleSerialReadyRead() {
    frameAssembler.append(serial->readAll());
    processReceivedFrames();
}

void MainWindow::processReceivedFrames()
{
    // Each frame's bytes are consumed before it is processed, so a nested
    // event loop (a message box) can safely read and process more frames.
    WaveFrame frame;
    while (frameAssembler.takeFrame(&frame)) {
        currentFrame = frame;
        noteDeviceFrameReceived();
        plotSamples();
    }
}

void MainWindow::handleSerialError(QSerialPort::SerialPortError error) {
    const bool communicationError =
        error == QSerialPort::ResourceError ||
        error == QSerialPort::ReadError ||
        error == QSerialPort::WriteError;
    if (!communicationError || serialErrorHandled) return;

    serialErrorHandled = true;

    if (acquireMode == CalibrationAcquireMode && calibrationDialog) {
        calibrationDialog->notifyAcquisitionUnavailable(
            QStringLiteral("设备通信已中断，本次独立测量已取消。请检查USB和设备电源，重新连接后再采集。"));
        stopCalibrationAcquisition();
    }
    serial->close();
    ui->connectButton->setText("连接");
    resetDisconnectedAcquisitionState();
    statusBar()->showMessage("设备通信已中断，请检查 USB 连接和设备电源。重新插入后会自动连接。", 10000);

    QMessageBox::warning(this,
                         "设备通信已中断",
                         "设备通信已中断，可能是 USB 被拔出、连接不稳定或读写失败。\n"
                         "请检查 USB 线和设备电源。重新插入后会自动连接；若没有连上，请点击“连接”。");
}

// ==================== 设备响应提示（只影响显示，不改变命令时序）====================

void MainWindow::noteCommandSent()
{
    // Measure from the first unanswered command; continuous 80 ms commands
    // must not keep restarting the clock.
    if (awaitingDeviceFrame) return;
    awaitingDeviceFrame = true;
    awaitingFrameSince.start();
}

void MainWindow::noteDeviceFrameReceived()
{
    awaitingDeviceFrame = false;
    if (!deviceUnresponsive) return;
    deviceUnresponsive = false;
    statusBar()->showMessage(QStringLiteral("设备已恢复响应。"), 4000);
    updatePatientSelectionUi();
}

void MainWindow::checkDeviceResponse()
{
    if (!serial || !serial->isOpen()) {
        awaitingDeviceFrame = false;
        deviceUnresponsive = false;
        return;
    }
    if (deviceUnresponsive || !awaitingDeviceFrame) return;
    if (awaitingFrameSince.elapsed() < deviceResponseTimeoutMs) return;
    deviceUnresponsive = true;
    statusBar()->showMessage(
        QStringLiteral("设备无响应：请确认选的是骨密度仪的端口、设备已通电、探头线已插好。"), 10000);
    updatePatientSelectionUi();
}
