// 串口连接、帧解析与设备响应提示

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
#include "theme.h"

using namespace mainwindow_detail;


void MainWindow::clearFrameAssembly()
{
    frameGroups.clear();
    frameGroupOrder.clear();
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
    rxBuffer.clear();
    samplesA.clear();
    samplesB.clear();
    samplesC.clear();
    samplesD.clear();
    chReceived[0] = chReceived[1] = chReceived[2] = chReceived[3] = false;
    clearFrameAssembly();
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

            rxBuffer.clear();
            clearFrameAssembly();
            chReceived[0] = chReceived[1] = chReceived[2] = chReceived[3] = false;
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
        // 检测中点击“获取波形”按钮 = 停止检测
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
        Theme::setTone(ui->lblProcessStatus, Theme::Tone::Warn);
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
    noteCommandSent();
    // 注意：自动模式下不要加 waitForBytesWritten，会阻塞界面
    // 也不要在这里 clear() rxBuffer，否则会把正在接收的数据清掉
}

//=====================发命令==============================================================
void MainWindow::on_btnAcquireWaveform_clicked()
{
    if (patientMeasureRunning) {
        // 检测中点击"获取波形" = 停止检测
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
        Theme::setTone(ui->lblProcessStatus, Theme::Tone::Warn);
        return;
    }

    acquireMode = DebugAcquireMode;

    if (!serial->isOpen()) {
        QMessageBox::warning(this, "设备未连接", "请先点击“连接”，连接设备后再获取波形。");
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
    noteCommandSent();
    serial->waitForBytesWritten(50);

    qDebug() << "TX CMD" << cmd.toHex(' ');

    samplesA.clear();
    samplesB.clear();
    samplesC.clear();
    samplesD.clear();

    chReceived[0] = chReceived[1] = chReceived[2] = chReceived[3] = false;
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

        // p[2..3] carries the frame's gain; it is not used on this side.
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

        if (ch < 1 || ch > 4) {
            rxBuffer.remove(0, frameBytes);
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

        if (!frameGroups.contains(idx)) {
            while (frameGroupOrder.size() >= maxIncompleteFrameGroups) {
                frameGroups.remove(frameGroupOrder.dequeue());
            }
            frameGroupOrder.enqueue(idx);
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
            frameGroupOrder.removeAll(idx);
            noteDeviceFrameReceived();
            plotSamples();
        }

        rxBuffer.remove(0, frameBytes);
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
