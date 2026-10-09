// 串口连接、帧解析与设备响应提示

#include "mainwindow/mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogs/calibrationdialog.h"

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

void MainWindow::scanPorts() {
    QList<QPair<QString, QString>> ports;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) ports.append({info.portName(), info.description()});

    // Pre-select the port that worked last time; otherwise the Raspberry Pi
    // Pico (VID 0x2E8A) the device is built on, then any USB serial port.
    QString preferred;
    const QString remembered = rememberedPort();
    for (const QSerialPortInfo &info : infos) {
        if (info.portName() == remembered) preferred = remembered;
    }
    for (const QSerialPortInfo &info : infos) {
        if (preferred.isEmpty() && info.hasVendorIdentifier() && info.vendorIdentifier() == 0x2E8A)
            preferred = info.portName();
    }
    for (const QSerialPortInfo &info : infos) {
        if (preferred.isEmpty() && info.description().contains(QStringLiteral("USB"), Qt::CaseInsensitive))
            preferred = info.portName();
    }
    applyPortList(ports, preferred);
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
void MainWindow::applyPortList(const QList<QPair<QString, QString>>& ports,
                               const QString& preferred)
{
    QComboBox* combo = ui->comboPort;
    bool unchanged = combo->count() == ports.size();
    for (int i = 0; unchanged && i < ports.size(); ++i) {
        unchanged = combo->itemData(i).toString() == ports[i].first &&
                    combo->itemText(i) == ports[i].first + " - " + ports[i].second;
    }
    if (unchanged) return;
    if (combo->view() && combo->view()->isVisible()) return;

    const QString current = combo->currentData().toString();
    QSignalBlocker blocker(combo);
    combo->clear();
    for (const auto& port : ports) {
        // 文本：COM11 - USB Serial Device；data 只存端口名
        combo->addItem(port.first + " - " + port.second, port.first);
    }
    int idx = combo->findData(current);
    if (idx < 0 && !preferred.isEmpty()) idx = combo->findData(preferred);
    if (idx >= 0) combo->setCurrentIndex(idx);
}


void MainWindow::on_connectButton_clicked() {
    if (!serial->isOpen()) {

        QString portName = ui->comboPort->currentData().toString();
        if (portName.isEmpty()) {
            QMessageBox::warning(this, "无法连接设备", "请先选择设备端口，再点击“连接”。");
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
            rememberPort(portName);
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

        } else {
            QMessageBox::warning(this,
                                 "无法连接设备",
                                 "设备连接失败。\n请检查 USB 是否插好、设备是否被其他程序占用，然后重试。");
            return;
        }

    } else {
        serial->close();
        serialErrorHandled = false;
        ui->connectButton->setText("连接");
        resetDisconnectedAcquisitionState();
    }
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
    statusBar()->showMessage("设备通信已中断，请检查 USB 连接和设备电源。重新插入后点击“连接”。", 10000);

    QMessageBox::warning(this,
                         "设备通信已中断",
                         "设备通信已中断，可能是 USB 被拔出、连接不稳定或读写失败。\n"
                         "请检查 USB 线和设备电源，重新插入后点击“连接”。");
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
