// 实时波形、声速趋势、检测过程面板

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
#include "uikit.h"

using namespace mainwindow_detail;




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

    if (n <= 0) {
        detectAndPlotSpeed(filBC, filBD, filAC, filAD);
        return;
    }

    // 4. 打印滤波后数据（逐帧大流量，默认关闭）
    // if (kDebugPerFrame) {
    //     printRangeFormatted("B->C / filBC", filBC, 0, 1499);
    //     printRangeFormatted("B->D / filBD", filBD, 0, 1499);
    //     printRangeFormatted("A->C / filAC", filAC, 0, 1499);
    //     printRangeFormatted("A->D / filAD", filAD, 0, 1499);
    // }

    // 5. 声速检测
    detectAndPlotSpeed(filBC, filBD, filAC, filAD);

    // 6. 画图
    if (!shouldRefreshLiveWaveforms()) {
        return;
    }

    QVector<QPointF> ptsA, ptsB, ptsC, ptsD;
    ptsA.reserve(n);
    ptsB.reserve(n);
    ptsC.reserve(n);
    ptsD.reserve(n);

    for (int i = 0; i < n; ++i) {
        if (!std::isfinite(filBC[i]) || !std::isfinite(filBD[i])
            || !std::isfinite(filAC[i]) || !std::isfinite(filAD[i])) {
            return;
        }
        ptsA.append(QPointF(i, filBC[i] + 2048.0));
        ptsB.append(QPointF(i, filBD[i] + 2048.0));
        ptsC.append(QPointF(i, filAC[i] + 2048.0));
        ptsD.append(QPointF(i, filAD[i] + 2048.0));
    }

    seriesA->replace(ptsA);
    seriesB->replace(ptsB);
    seriesC->replace(ptsC);
    seriesD->replace(ptsD);

    auto updateAxisX = [&](QChart *chart, int count) {
        auto *axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).value(0));
        const qreal maximum = count - 1;
        if (axisX && (axisX->min() != 0.0 || axisX->max() != maximum)) {
            axisX->setRange(0, maximum);
        }
    };

    updateAxisX(chartA, n);
    updateAxisX(chartB, n);
    updateAxisX(chartC, n);
    updateAxisX(chartD, n);
}

bool MainWindow::shouldRefreshLiveWaveforms()
{
    if (liveWaveformRenderTimer.isValid()
        && liveWaveformRenderTimer.elapsed() < liveWaveformRefreshIntervalMs) {
        return false;
    }

    liveWaveformRenderTimer.restart();
    return true;
}

void MainWindow::appendSpeedPoint(double speedAvg)
{
    if (!std::isfinite(speedAvg) || !seriesSpeed || !chartSpeed) {
        return;
    }

    seriesSpeed->append(speedPointIndex, speedAvg);
    const int excessPoints = seriesSpeed->count() - 50;
    if (excessPoints > 0) seriesSpeed->removePoints(0, excessPoints);
    speedPointIndex++;

    auto *axisX = qobject_cast<QValueAxis*>(chartSpeed->axes(Qt::Horizontal).value(0));
    if (axisX) {
        qreal minimum = 0.0;
        qreal maximum = 50.0;
        if (speedPointIndex < 50) {
            minimum = 0.0;
        } else {
            minimum = speedPointIndex - 50;
            maximum = speedPointIndex;
        }

        if (axisX->min() != minimum || axisX->max() != maximum) {
            axisX->setRange(minimum, maximum);
        }
    }
}




void MainWindow::setupChart()
{
    // Same four QChart/QLineSeries channels as before; only the theme, the
    // channel chips and the single shared gain control are new. Antialiasing
    // stays off (SC-44).
    const Theme::Tokens& tokens = Theme::tokens();
    QWidget *container = new QWidget();
    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->setSpacing(8);
    vbox->setContentsMargins(0, 0, 0, 0);

    const QString names[4] = {QStringLiteral("A"), QStringLiteral("B"), QStringLiteral("C"), QStringLiteral("D")};
    int channelIndex = 0;

    auto createChannel = [&](QLineSeries **seriesPtr,
                             QChart **chartPtr,
                             QChartView **viewPtr,
                             QSlider **sliderPtr) {
        const int index = channelIndex++;
        const QString& name = names[index];

        // 1. 曲线
        *seriesPtr = new QLineSeries();
        QPen pen(tokens.channel[index]);
        pen.setWidth(2);
        (*seriesPtr)->setPen(pen);

        // 2. 图表
        *chartPtr = new QChart();
        (*chartPtr)->addSeries(*seriesPtr);
        (*chartPtr)->setMargins(QMargins(0, 0, 0, 0));
        (*chartPtr)->layout()->setContentsMargins(0, 0, 0, 0);
        Theme::styleChart(*chartPtr);

        // 3. X 轴：不显示横坐标数字，节省高度
        QValueAxis *axisX = new QValueAxis();
        axisX->setLabelFormat("%d");
        Theme::styleValueAxis(axisX, false);
        (*chartPtr)->addAxis(axisX, Qt::AlignBottom);
        (*seriesPtr)->attachAxis(axisX);

        // 4. Y 轴：只保留 0 / 中间 / 4095
        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0, 4095);
        axisY->setTickCount(3);
        axisY->setLabelFormat("%.0f");
        Theme::styleValueAxis(axisY, true);
        // Slightly smaller than other charts so all three ticks still fit when
        // a channel is only ~50 px tall (1366 x 768).
        axisY->setLabelsFont(Theme::numberFont(9, QFont::Normal));
        (*chartPtr)->addAxis(axisY, Qt::AlignLeft);
        (*seriesPtr)->attachAxis(axisY);

        // 5. 图表视图
        *viewPtr = new QChartView(*chartPtr);
        (*viewPtr)->setRenderHint(QPainter::Antialiasing, false);
        (*viewPtr)->setStyleSheet("background: transparent;");
        (*viewPtr)->setFrameShape(QFrame::NoFrame);
        (*viewPtr)->setMinimumHeight(44);
        (*viewPtr)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // 6. 增益滑条：四条仍然联动、共用一个增益，界面只在标题栏显示 A 一条
        *sliderPtr = new QSlider(index == 0 ? Qt::Horizontal : Qt::Vertical, container);
        (*sliderPtr)->setRange(0, 1241);
        (*sliderPtr)->setValue(globalGain);
        (*sliderPtr)->setTickPosition(QSlider::NoTicks);
        (*sliderPtr)->setObjectName(QStringLiteral("gainSlider%1").arg(name));
        (*sliderPtr)->setToolTip(QStringLiteral("增益（四个通道共用）"));
        connect(*sliderPtr, &QSlider::valueChanged, this, &MainWindow::onGainSliderChanged);
        if (index != 0) (*sliderPtr)->hide();

        auto* channelLabel = new QLabel(name);
        channelLabel->setObjectName(QStringLiteral("channelLabel%1").arg(name));
        channelLabel->setProperty("role", QStringLiteral("channelChip"));
        channelLabel->setProperty("channel", name);
        channelLabel->setFixedSize(26, 26);
        channelLabel->setAlignment(Qt::AlignCenter);

        // 7. 一行：通道标签 + 图
        QHBoxLayout *hbox = new QHBoxLayout();
        hbox->setContentsMargins(0, 0, 0, 0);
        hbox->setSpacing(10);
        hbox->addWidget(channelLabel, 0, Qt::AlignVCenter);
        hbox->addWidget(*viewPtr, 1);
        vbox->addLayout(hbox, 1);
    };

    createChannel(&seriesA, &chartA, &viewA, &gainSliderA);
    createChannel(&seriesB, &chartB, &viewB, &gainSliderB);
    createChannel(&seriesC, &chartC, &viewC, &gainSliderC);
    createChannel(&seriesD, &chartD, &viewD, &gainSliderD);

    container->setLayout(vbox);

    // The shared gain lives in the card header next to the title.
    QHBoxLayout* header = ui->grpWaveArea->layout()
        ? ui->grpWaveArea->layout()->findChild<QHBoxLayout*>(QStringLiteral("waveHeader")) : nullptr;
    if (header) {
        auto* gainCaption = new QLabel(QStringLiteral("增益"), ui->grpWaveArea);
        gainCaption->setProperty("role", QStringLiteral("caption"));
        header->addWidget(gainCaption);
        gainSliderA->setParent(ui->grpWaveArea);
        gainSliderA->setFixedWidth(150);
        gainSliderA->show();
        header->addWidget(gainSliderA);
        auto* gainValue = new QLabel(QString::number(globalGain), ui->grpWaveArea);
        gainValue->setObjectName(QStringLiteral("gainValue"));
        gainValue->setAlignment(Qt::AlignCenter);
        gainValue->setMinimumWidth(48);
        gainValue->setToolTip(QStringLiteral("当前增益"));
        gainValueLabels.append(gainValue);
        header->addWidget(gainValue);
    }

    // 不套 QScrollArea，避免出现滚动条
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
    for (QLabel* label : gainValueLabels) label->setText(QString::number(value));

    // 状态栏提示当前增益
    statusBar()->showMessage(
        QString("当前增益 = 0x%1 (%2)")
            .arg(globalGain, 4, 16, QLatin1Char('0'))
            .arg(globalGain),
        1500
        );
}

void MainWindow::setupSpeedChart()
{
    // ======================================================
    // 1. 创建声速曲线
    // ======================================================
    seriesSpeed = new QLineSeries();
    seriesSpeed->setName("声速趋势");

    QPen pen(Theme::tokens().accent);
    pen.setWidth(2);
    seriesSpeed->setPen(pen);

    // ======================================================
    // 2. 创建图表
    // ======================================================
    chartSpeed = new QChart();
    chartSpeed->addSeries(seriesSpeed);

    // 节省空间：不显示标题、不显示图例
    chartSpeed->setTitle("");
    chartSpeed->setMargins(QMargins(0, 2, 2, 0));
    chartSpeed->layout()->setContentsMargins(0, 0, 0, 0);
    Theme::styleChart(chartSpeed);

    // ======================================================
    // 3. X轴：时间/次数
    // ======================================================
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(0, 50);
    axisX->setTickCount(6);          // 0,10,20,30,40,50
    axisX->setLabelFormat("%d");
    Theme::styleValueAxis(axisX, true);
    chartSpeed->addAxis(axisX, Qt::AlignBottom);
    seriesSpeed->attachAxis(axisX);

    // ======================================================
    // 4. Y轴：声速范围固定 2000~5000
    // ======================================================
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(2000, 5000);
    axisY->setTickCount(4);          // 2000,3000,4000,5000
    axisY->setLabelFormat("%.0f");
    Theme::styleValueAxis(axisY, true);
    chartSpeed->addAxis(axisY, Qt::AlignLeft);
    seriesSpeed->attachAxis(axisY);

    // ======================================================
    // 5. 绑定到 UI
    // ======================================================
    ui->chartViewSpeed->setChart(chartSpeed);
    ui->chartViewSpeed->setRenderHint(QPainter::Antialiasing, false);
    ui->chartViewSpeed->setStyleSheet("background: transparent;");
    ui->chartViewSpeed->setFrameShape(QFrame::NoFrame);
}

void MainWindow::setupSpeedDebugPanel()
{
    QWidget *host = ui->chartViewSpeed->parentWidget();
    if (!host) return;

    QFrame *panel = new QFrame(host);
    panel->setObjectName("speedDebugPanel");
    auto *row = new QHBoxLayout(panel);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(0);

    lblSosA = new QLabel("--");
    lblSosB = new QLabel("--");
    lblSosAvg = new QLabel("--");
    lblSosInfo = new QLabel("等待测量...");
    lblSosInfo->setObjectName("speedDebugInfo");
    const QStringList captions = {QStringLiteral("通道 A"), QStringLiteral("通道 B"),
                                  QStringLiteral("平均 m/s")};
    const QList<QLabel*> values = {lblSosA, lblSosB, lblSosAvg};
    for (int i = 0; i < values.size(); ++i) {
        auto *cell = new QFrame(panel);
        cell->setProperty("role", QStringLiteral("cell"));
        cell->setProperty("divider", i > 0);
        auto *cellLayout = new QVBoxLayout(cell);
        cellLayout->setContentsMargins(16, 9, 16, 9);
        cellLayout->setSpacing(1);
        auto *captionRow = new QHBoxLayout;
        captionRow->setSpacing(6);
        auto *caption = new QLabel(captions[i], cell);
        caption->setProperty("role", QStringLiteral("caption"));
        captionRow->addWidget(caption);
        if (values[i] == lblSosB) {
            // B is the channel the result is computed from.
            auto *badge = new QLabel(QStringLiteral("输出"), cell);
            badge->setProperty("role", QStringLiteral("badge"));
            captionRow->addWidget(badge);
            lblSosB->setProperty("accent", true);
        }
        captionRow->addStretch(1);
        cellLayout->addLayout(captionRow);
        values[i]->setParent(cell);
        values[i]->setProperty("role", QStringLiteral("statValue"));
        cellLayout->addWidget(values[i]);
        row->addWidget(cell, 1);
    }

    if (QBoxLayout *box = qobject_cast<QBoxLayout*>(host->layout())) {
        const int idx = box->indexOf(ui->chartViewSpeed);
        box->insertWidget(idx >= 0 ? idx : 0, panel, 0);
        box->insertWidget((idx >= 0 ? idx : 0) + 1, lblSosInfo, 0);
        return;
    }
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

    lblSosA->setText(QString::number(sosA, 'f', 0));
    lblSosB->setText(QString::number(sosB, 'f', 0));
    lblSosAvg->setText(QString::number(sosAvg, 'f', 0));

    lblSosInfo->setText(
        QString("lagA=%1  lagB=%2  diff=%3  corrA=%4  corrB=%5")
            .arg(lagA)
            .arg(lagB)
            .arg(diffLag)
            .arg(corrA, 0, 'f', 2)
            .arg(corrB, 0, 'f', 2)
        );

    lblSosInfo->setProperty("state", QStringLiteral("ok"));
    lblSosInfo->style()->unpolish(lblSosInfo);
    lblSosInfo->style()->polish(lblSosInfo);
}

void MainWindow::setSpeedDebugInvalid(const QString& reason)
{
    if (!lblSosA || !lblSosB || !lblSosAvg || !lblSosInfo) {
        return;
    }

    lblSosInfo->setText("无效：" + reason);
    lblSosInfo->setProperty("state", QStringLiteral("bad"));
    lblSosInfo->style()->unpolish(lblSosInfo);
    lblSosInfo->style()->polish(lblSosInfo);
}

void MainWindow::initProcessPanel()
{
    const Theme::Tokens& tokens = Theme::tokens();
    processValidCount = 0;
    // This layout replaces only the approved process area, not the reference chart.
    ui->lblProcessTitle->hide();
    ui->lblGateStats->hide();
    auto* outer = new QVBoxLayout(ui->grpProcessArea);
    outer->setContentsMargins(20, 16, 20, 16);
    outer->setSpacing(10);
    auto* cardHeader = new QHBoxLayout;
    cardHeader->setSpacing(10);
    auto* cardTitle = new QLabel(QStringLiteral("检测过程"), ui->grpProcessArea);
    cardTitle->setProperty("role", QStringLiteral("cardTitle"));
    cardHeader->addWidget(cardTitle);
    cardHeader->addStretch(1);
    ui->btnMeasurementGuide->setStyleSheet(QString());
    ui->btnMeasurementGuide->setProperty("variant", QStringLiteral("soft"));
    ui->btnMeasurementGuide->setProperty("btnSize", QStringLiteral("small"));
    ui->btnMeasurementGuide->setIcon(Icons::icon(Icons::Glyph::Help, tokens.accentInk));
    ui->btnMeasurementGuide->setIconSize(QSize(15, 15));
    ui->btnMeasurementGuide->setMinimumSize(96, 30);
    cardHeader->addWidget(ui->btnMeasurementGuide);
    outer->addLayout(cardHeader);
    outer->addWidget(ui->widgetBalanceArea, 1);

    auto* root = new QVBoxLayout(ui->widgetBalanceArea);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(10);
    ui->widgetBalanceArea->setStyleSheet(QString());
    auto* header = new QHBoxLayout;
    header->setSpacing(10);
    ui->lblProcessStatus->setWordWrap(true);
    ui->lblProcessStatus->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->lblProcessStatus->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    header->addWidget(ui->lblProcessStatus, 1);
    roundProgress = new RoundProgress(ui->widgetBalanceArea);
    roundProgress->setObjectName(QStringLiteral("roundProgress"));
    roundProgress->setToolTip(QStringLiteral("5 轮检测进度"));
    roundProgress->hide();
    header->addWidget(roundProgress, 0, Qt::AlignVCenter);
    root->addLayout(header);
    ui->barMeasureProgress->setFixedHeight(24);
    root->addWidget(ui->barMeasureProgress);

    auto label = [](const QString& text, const QString& name) {
        auto* value = new QLabel(text);
        value->setObjectName(name);
        value->setWordWrap(true);
        return value;
    };
    auto* cards = new QHBoxLayout;
    cards->setSpacing(10);
    auto* aCard = new QFrame;
    aCard->setObjectName(QStringLiteral("corrACard"));
    auto* aLayout = new QVBoxLayout(aCard);
    aLayout->setContentsMargins(14, 12, 14, 12);
    aLayout->setSpacing(6);
    auto* aHeader = new QHBoxLayout;
    aHeader->setSpacing(8);
    auto* aTitle = label(QStringLiteral("① 优先调整  corrA"), QStringLiteral("corrATitle"));
    aTitle->setWordWrap(false);
    aHeader->addWidget(aTitle, 1);
    lblCorrAStatus = label(QStringLiteral("等待信号"), QStringLiteral("lblCorrAStatus"));
    lblCorrAStatus->setWordWrap(false);
    aHeader->addWidget(lblCorrAStatus, 0, Qt::AlignVCenter);
    aLayout->addLayout(aHeader);
    auto* aBody = new QHBoxLayout;
    aBody->setSpacing(14);
    auto* scale = new QVBoxLayout;
    scale->setSpacing(3);
    scale->addWidget(label(QStringLiteral("1.00"), QStringLiteral("corrAScaleTop")), 0, Qt::AlignHCenter);
    barCorrA = new QProgressBar;
    barCorrA->setObjectName(QStringLiteral("barCorrA"));
    barCorrA->setOrientation(Qt::Vertical);
    barCorrA->setRange(0, 1000);
    barCorrA->setTextVisible(false);
    barCorrA->setFixedWidth(14);
    barCorrA->setMinimumHeight(82);
    scale->addWidget(barCorrA, 1, Qt::AlignHCenter);
    scale->addWidget(label(QStringLiteral("0.00"), QStringLiteral("corrAScaleBottom")), 0, Qt::AlignHCenter);
    aBody->addLayout(scale);
    auto* aValues = new QVBoxLayout;
    aValues->setSpacing(4);
    aValues->addStretch();
    lblCorrAValue = label(QStringLiteral("—"), QStringLiteral("lblCorrAValue"));
    lblCorrAValue->setWordWrap(false);
    lblCorrAThreshold = label(QString(), QStringLiteral("lblCorrAThreshold"));
    aValues->addWidget(lblCorrAValue);
    aValues->addWidget(lblCorrAThreshold);
    aValues->addWidget(label(QStringLiteral("达到要求即可，不必追求满格"), QStringLiteral("corrAHelp")));
    aValues->addStretch();
    aBody->addLayout(aValues, 1);
    aLayout->addLayout(aBody, 1);
    ui->lblPositionGuide->setText(QStringLiteral("先调整探头长轴方向"));
    ui->lblPositionGuide->setWordWrap(true);
    ui->lblPositionGuide->setStyleSheet(QStringLiteral("font-weight:bold;"));
    aLayout->addWidget(ui->lblPositionGuide);
    aLayout->addWidget(label(QStringLiteral("沿桡骨方向小幅旋转，观察 corrA。"), QStringLiteral("corrAAction")));
    cards->addWidget(aCard, 3);

    auto* gCard = new QFrame;
    gCard->setObjectName(QStringLiteral("gCard"));
    auto* gLayout = new QVBoxLayout(gCard);
    gLayout->setContentsMargins(14, 12, 14, 12);
    gLayout->setSpacing(6);
    ui->lblBPairTitle->setText(QStringLiteral("② 辅助调整  G"));
    ui->lblBPairTitle->setWordWrap(true);
    ui->lblBPairTitle->setStyleSheet(QString());
    gLayout->addWidget(ui->lblBPairTitle);
    auto* gBody = new QHBoxLayout;
    gBody->setSpacing(14);
    ui->barPairB->setFixedWidth(14);
    ui->barPairB->setMinimumHeight(100);
    ui->barPairB->setTextVisible(false);
    gBody->addWidget(ui->barPairB, 0, Qt::AlignHCenter);
    auto* gValues = new QVBoxLayout;
    gValues->setSpacing(4);
    gValues->addStretch();
    ui->lblPairBValue->setStyleSheet(QString());
    ui->lblPairBValue->setWordWrap(false);
    gValues->addWidget(ui->lblPairBValue);
    lblGStatus = label(QStringLiteral("等待信号"), QStringLiteral("lblGStatus"));
    gValues->addWidget(lblGStatus);
    gValues->addStretch();
    gBody->addLayout(gValues, 1);
    gLayout->addLayout(gBody, 1);
    auto* gAction = label(QStringLiteral("未计数，再微调倾角"), QStringLiteral("gAction"));
    gAction->setStyleSheet(QStringLiteral("font-weight:bold;"));
    gLayout->addWidget(gAction);
    gLayout->addWidget(label(QStringLiteral("保持已找到的位置与长轴方向。"), QStringLiteral("gHelp")));
    cards->addWidget(gCard, 2);
    root->addLayout(cards, 1);

    auto* auxiliary = new QHBoxLayout;
    auxiliary->setSpacing(10);
    ui->lblAPairTitle->setText(QStringLiteral("辅助 D"));
    ui->lblAPairTitle->setStyleSheet(QString());
    ui->barPairA->setOrientation(Qt::Horizontal);
    ui->barPairA->setFixedSize(72, 6);
    ui->barPairA->setTextVisible(false);
    ui->lblPairAValue->setStyleSheet(QString());
    lblDStatus = label(QStringLiteral("等待信号"), QStringLiteral("lblDStatus"));
    auxiliary->addWidget(ui->lblAPairTitle);
    auxiliary->addWidget(ui->barPairA, 0, Qt::AlignVCenter);
    auxiliary->addWidget(ui->lblPairAValue);
    auxiliary->addWidget(lblDStatus);
    auxiliary->addStretch();
    root->addLayout(auxiliary);

    auto* note = new QFrame(ui->widgetBalanceArea);
    note->setObjectName(QStringLiteral("guideNote"));
    auto* noteLayout = new QHBoxLayout(note);
    noteLayout->setContentsMargins(12, 9, 12, 9);
    noteLayout->setSpacing(8);
    auto* noteIcon = new QLabel(note);
    noteIcon->setPixmap(Icons::icon(Icons::Glyph::Info, tokens.accentInk).pixmap(QSize(15, 15), 2.0));
    noteIcon->setFixedSize(15, 15);
    noteLayout->addWidget(noteIcon, 0, Qt::AlignTop);
    ui->lblPositionGuideNote->setText(QStringLiteral(
        "优先让 corrA 达到要求；连续计数后保持稳定。提示仅供参考，以有效值计数为准。"));
    ui->lblPositionGuideNote->setWordWrap(true);
    ui->lblPositionGuideNote->setStyleSheet(QString());
    noteLayout->addWidget(ui->lblPositionGuideNote, 1);
    root->addWidget(note);

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
    ui->lblPairAValue->setText("D=--");
    ui->lblPairBValue->setText("G=--");
    ui->lblProcessStatus->setText("等待开始测量");
    ui->lblProcessStatus->setStyleSheet(QString());
    Theme::setTone(ui->lblProcessStatus, Theme::Tone::Muted);
    ui->lblProcessStatus->setWordWrap(true);
    ui->lblProcessStatus->setMinimumHeight(0);
    ui->lblGateStats->setText("");
    ui->lblGateStats->setWordWrap(true);

    // ======================================================
    // 4. 进度条样式：全部在 theme.qss（按 objectName 与 tone）
    // ======================================================
    for (auto* bar : {ui->barPairA, ui->barPairB, ui->barMeasureProgress}) {
        bar->setStyleSheet(QString());
    }
    for (auto* bar : {ui->barPairA, ui->barPairB, barCorrA}) {
        bar->installEventFilter(this);
    }
    clearFeedbackReadings();

    // ======================================================
    // 5. 给两个竖条加 50% 中线
    // ======================================================
    QTimer::singleShot(0, this, [this]() {
        addMiddleLineToProgressBar(ui->barPairA);
        addMiddleLineToProgressBar(ui->barPairB);
        addMiddleLineToProgressBar(barCorrA);
    });
}

void MainWindow::addMiddleLineToProgressBar(QProgressBar *bar)
{
    if (!bar) {
        return;
    }

    QFrame *line = bar->findChild<QFrame*>("middleLine");
    if (!line) {
        line = new QFrame(bar);
        line->setObjectName("middleLine");
        line->setFrameShape(QFrame::NoFrame);
        line->setStyleSheet(QStringLiteral("background-color: %1;").arg(Theme::tokens().ink900.name()));
        line->setAttribute(Qt::WA_TransparentForMouseEvents);
    }

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

    const double position = bar == barCorrA ? qBound(0.0, mCfg.frameCorrAMin, 1.0) : 0.5;
    if (bar->orientation() == Qt::Vertical)
        line->setGeometry(0, qBound(0, qRound((h - 2) * (1.0 - position)), h - 2), w, 2);
    else
        line->setGeometry(qRound((w - 2) * position), 0, 2, h);
    line->raise();
    line->show();
}

void MainWindow::updateCorrAFeedback(double corrA)
{
    if (!barCorrA) return;
    lblCorrAThreshold->setText(QStringLiteral("要求 ≥ %1").arg(mCfg.frameCorrAMin, 0, 'f', 2));
    addMiddleLineToProgressBar(barCorrA);
    const bool available = std::isfinite(corrA);
    barCorrA->setEnabled(available);
    barCorrA->setValue(available ? qRound(qBound(0.0, corrA, 1.0) * 1000) : 0);
    lblCorrAValue->setText(available ? QString::number(corrA, 'f', 3) : QStringLiteral("—"));
    lblCorrAValue->setToolTip(available ? QString::number(corrA, 'g', 12) : QString());
    const bool meets = available && corrA >= mCfg.frameCorrAMin;
    lblCorrAStatus->setText(!available ? QStringLiteral("等待信号")
                                     : meets ? QStringLiteral("已达标") : QStringLiteral("未达标"));
    const Theme::Tone tone = !available ? Theme::Tone::Muted : meets ? Theme::Tone::Ok : Theme::Tone::Warn;
    Theme::setTone(lblCorrAStatus, tone);
    Theme::setTone(barCorrA, tone);
}

void MainWindow::clearFeedbackReadings()
{
    if (!barCorrA) return;
    updateCorrAFeedback(qQNaN());
    ui->barPairA->setEnabled(false);
    ui->barPairB->setEnabled(false);
    ui->barPairA->setValue(0);
    ui->barPairB->setValue(0);
    ui->lblPairAValue->setText(QStringLiteral("D=--"));
    ui->lblPairBValue->setText(QStringLiteral("G=--"));
    lblGStatus->setText(QStringLiteral("等待信号"));
    lblDStatus->setText(QStringLiteral("等待信号"));
}

void MainWindow::updateProcessPanel(double sosA,
                                    double sosB,
                                    int lagA,
                                    int lagB,
                                    int diffLag,
                                    double pairMidGap,
                                    bool countThisFrame)
{
    ui->barPairA->setEnabled(true);
    ui->barPairB->setEnabled(true);
    Q_UNUSED(sosA);
    Q_UNUSED(sosB);
    Q_UNUSED(diffLag);

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

    // 右竖条：D = lagA - lagB
    int dBar = mapToBar(
        lagA - lagB,
        mCfg.angleSignedDiffTarget,
        6.0,
        320.0,
        1.4
        );

    // 左竖条：G = pairMidGap
    int gBar = mapToBar(
        pairMidGap,
        mCfg.anglePairMidGapTarget,
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
        QString("D=%1")
            .arg(signedLagDiff)
        );

    ui->lblPairBValue->setText(
        QString("G=%1")
            .arg(pairMidGap, 0, 'f', 1)
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
        (signedLagDiff >= mCfg.angleSignedDiffMin &&
         signedLagDiff <= mCfg.angleSignedDiffMax);

    bool anglePairMidGapOk =
        (pairMidGap >= mCfg.anglePairMidGapMin &&
         pairMidGap <= mCfg.anglePairMidGapMax);

    bool angleOk =
        (!enablePatientAngleGate) ||
        (angleSignedDiffOk && anglePairMidGapOk);

    // 姿态计算保留不变；文字不再随每帧跳变。
    Q_UNUSED(angleOk);
    lblDStatus->setText(angleSignedDiffOk ? QStringLiteral("满足要求") : QStringLiteral("未满足"));
    lblGStatus->setText(anglePairMidGapOk ? QStringLiteral("满足要求") : QStringLiteral("未满足"));
    ui->lblGateStats->setText("");
}

void MainWindow::updateProcessInvalid(const QString& reason)
{
    Q_UNUSED(reason);
    rejectBoneLagCandidate();
    clearFeedbackReadings();
}
