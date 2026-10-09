// 实时波形、声速趋势、检测过程面板

#include "mainwindow/mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QFrame>
#include <QGraphicsLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSlider>
#include <QStyle>
#include <QVBoxLayout>
#include <cmath>

void MainWindow::plotSamples()
{
    const FilteredFrame filtered = FrameAnalyzer::filter(signalProcessor, currentFrame);
    processFilteredFrame(filtered);

    const int n = filtered.length();
    if (n <= 0) return;
    const QVector<double>& filBC = filtered.bc;
    const QVector<double>& filBD = filtered.bd;
    const QVector<double>& filAC = filtered.ac;
    const QVector<double>& filAD = filtered.ad;

    // 画图（限速，避免每 80 ms 重绘四张图）
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
    // Same four QChart/QLineSeries channels as before; only the light theme,
    // channel labels and gain read-outs are new. Antialiasing stays off (SC-44).
    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->setSpacing(6);
    vbox->setContentsMargins(0, 0, 0, 0);

    struct ChannelStyle { QString name; QColor color; };
    const ChannelStyle styles[4] = {
        {QStringLiteral("A"), QColor(0x1D, 0x5F, 0xA8)},
        {QStringLiteral("B"), QColor(0x1B, 0x7A, 0x4B)},
        {QStringLiteral("C"), QColor(0x9A, 0x5B, 0x00)},
        {QStringLiteral("D"), QColor(0x6B, 0x3F, 0xA0)}};
    const QColor gridColor(0xEE, 0xF1, 0xF5);
    const QColor axisColor(0xD5, 0xDB, 0xE3);
    const QColor labelColor(0x5B, 0x65, 0x73);
    int channelIndex = 0;

    auto createChannel = [&](QLineSeries **seriesPtr,
                             QChart **chartPtr,
                             QChartView **viewPtr,
                             QSlider **sliderPtr) {
        const ChannelStyle& style = styles[channelIndex++];

        // 1. 曲线
        *seriesPtr = new QLineSeries();
        QPen pen(style.color);
        pen.setWidth(2);
        (*seriesPtr)->setPen(pen);

        // 2. 图表
        *chartPtr = new QChart();
        (*chartPtr)->addSeries(*seriesPtr);
        (*chartPtr)->legend()->hide();
        (*chartPtr)->setMargins(QMargins(0, 0, 0, 0));
        (*chartPtr)->layout()->setContentsMargins(0, 0, 0, 0);
        (*chartPtr)->setBackgroundRoundness(0);
        (*chartPtr)->setBackgroundBrush(QBrush(Qt::white));
        (*chartPtr)->setPlotAreaBackgroundBrush(QBrush(QColor(0xF7, 0xF9, 0xFB)));
        (*chartPtr)->setPlotAreaBackgroundVisible(true);

        // 3. X 轴：不显示横坐标数字，节省高度
        QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText("");
        axisX->setLabelFormat("%d");
        axisX->setLabelsVisible(false);
        axisX->setGridLineVisible(true);
        axisX->setGridLineColor(gridColor);
        axisX->setLinePenColor(axisColor);
        (*chartPtr)->addAxis(axisX, Qt::AlignBottom);
        (*seriesPtr)->attachAxis(axisX);

        // 4. Y 轴：只保留 0 / 中间 / 4095
        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("");
        axisY->setRange(0, 4095);
        axisY->setTickCount(3);
        axisY->setLabelFormat("%.0f");
        axisY->setLabelsColor(labelColor);
        QFont axisFont = axisY->labelsFont();
        axisFont.setPointSize(7);
        axisY->setLabelsFont(axisFont);
        axisY->setGridLineColor(gridColor);
        axisY->setLinePenColor(axisColor);
        (*chartPtr)->addAxis(axisY, Qt::AlignLeft);
        (*seriesPtr)->attachAxis(axisY);

        // 5. 图表视图
        *viewPtr = new QChartView(*chartPtr);
        (*viewPtr)->setRenderHint(QPainter::Antialiasing, false);
        (*viewPtr)->setStyleSheet("background: transparent;");
        (*viewPtr)->setMinimumHeight(44);
        (*viewPtr)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // 6. 增益滑条（四条联动，共用一个增益）
        *sliderPtr = new QSlider(Qt::Vertical);
        (*sliderPtr)->setRange(0, 1241);
        (*sliderPtr)->setValue(globalGain);
        (*sliderPtr)->setInvertedAppearance(false);
        (*sliderPtr)->setTickPosition(QSlider::NoTicks);
        (*sliderPtr)->setFixedWidth(24);
        (*sliderPtr)->setMinimumHeight(24);
        (*sliderPtr)->setCursor(Qt::PointingHandCursor);
        (*sliderPtr)->setObjectName(QStringLiteral("gainSlider%1").arg(style.name));
        (*sliderPtr)->setToolTip(QStringLiteral("增益（四个通道联动）"));
        connect(*sliderPtr, &QSlider::valueChanged, this, &MainWindow::onGainSliderChanged);

        auto* channelLabel = new QLabel(style.name);
        channelLabel->setObjectName(QStringLiteral("channelLabel%1").arg(style.name));
        channelLabel->setFixedWidth(18);
        channelLabel->setAlignment(Qt::AlignCenter);
        channelLabel->setStyleSheet(QStringLiteral("font-weight:bold; font-size:14px; color:%1;")
                                        .arg(style.color.name()));

        auto* gainValue = new QLabel(QString::number(globalGain));
        gainValue->setProperty("role", QStringLiteral("tiny"));
        gainValue->setAlignment(Qt::AlignCenter);
        gainValueLabels.append(gainValue);
        auto* gainColumn = new QVBoxLayout();
        gainColumn->setSpacing(0);
        gainColumn->addWidget(*sliderPtr, 1, Qt::AlignHCenter);
        gainColumn->addWidget(gainValue);

        // 7. 一行：通道名 + 图 + 增益
        QHBoxLayout *hbox = new QHBoxLayout();
        hbox->setContentsMargins(0, 0, 0, 0);
        hbox->setSpacing(6);
        hbox->addWidget(channelLabel);
        hbox->addWidget(*viewPtr, 1);
        hbox->addLayout(gainColumn);
        vbox->addLayout(hbox, 1);
    };

    createChannel(&seriesA, &chartA, &viewA, &gainSliderA);
    createChannel(&seriesB, &chartB, &viewB, &gainSliderB);
    createChannel(&seriesC, &chartC, &viewC, &gainSliderC);
    createChannel(&seriesD, &chartD, &viewD, &gainSliderD);

    QWidget *container = new QWidget();
    container->setLayout(vbox);

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

    QPen pen(QColor(0x1D, 0x5F, 0xA8));
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

    chartSpeed->setBackgroundBrush(QBrush(Qt::white));
    chartSpeed->setPlotAreaBackgroundBrush(QBrush(QColor(0xF7, 0xF9, 0xFB)));
    chartSpeed->setPlotAreaBackgroundVisible(true);

    // ======================================================
    // 3. X轴：时间/次数
    // ======================================================
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("");
    axisX->setRange(0, 50);
    axisX->setTickCount(6);          // 0,10,20,30,40,50
    axisX->setLabelFormat("%d");

    axisX->setLabelsColor(QColor(0x5B, 0x65, 0x73));
    axisX->setGridLineColor(QColor(0xEE, 0xF1, 0xF5));
    axisX->setLinePenColor(QColor(0xD5, 0xDB, 0xE3));

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

    axisY->setLabelsColor(QColor(0x5B, 0x65, 0x73));
    axisY->setGridLineColor(QColor(0xEE, 0xF1, 0xF5));
    axisY->setLinePenColor(QColor(0xD5, 0xDB, 0xE3));

    QFont fontY = axisY->labelsFont();
    fontY.setPointSize(8);
    axisY->setLabelsFont(fontY);

    chartSpeed->addAxis(axisY, Qt::AlignLeft);
    seriesSpeed->attachAxis(axisY);

    // ======================================================
    // 5. 绑定到 UI
    // ======================================================
    ui->chartViewSpeed->setChart(chartSpeed);
    ui->chartViewSpeed->setRenderHint(QPainter::Antialiasing, false);
    ui->chartViewSpeed->setStyleSheet("background: transparent;");
}

void MainWindow::setupSpeedDebugPanel()
{
    QWidget *host = ui->chartViewSpeed->parentWidget();
    if (!host) return;

    QFrame *panel = new QFrame(host);
    panel->setObjectName("speedDebugPanel");
    auto *grid = new QGridLayout(panel);
    grid->setContentsMargins(12, 6, 12, 6);
    grid->setHorizontalSpacing(18);
    grid->setVerticalSpacing(0);

    lblSosA = new QLabel("--");
    lblSosB = new QLabel("--");
    lblSosAvg = new QLabel("--");
    lblSosInfo = new QLabel("等待测量...");
    lblSosInfo->setObjectName("speedDebugInfo");
    const QStringList captions = {QStringLiteral("通道 A"), QStringLiteral("通道 B（输出）"),
                                  QStringLiteral("平均 m/s")};
    const QList<QLabel*> values = {lblSosA, lblSosB, lblSosAvg};
    for (int i = 0; i < values.size(); ++i) {
        auto *caption = new QLabel(captions[i]);
        caption->setProperty("role", QStringLiteral("caption"));
        values[i]->setProperty("role", QStringLiteral("statValue"));
        grid->addWidget(caption, 0, i);
        grid->addWidget(values[i], 1, i);
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
    // This layout replaces only the approved process area, not the reference chart.
    ui->lblProcessTitle->hide();
    ui->lblGateStats->hide();
    auto* outer = new QVBoxLayout(ui->grpProcessArea);
    outer->setContentsMargins(14, 38, 14, 12);
    outer->addWidget(ui->widgetBalanceArea);
    auto* root = new QVBoxLayout(ui->widgetBalanceArea);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(7);
    ui->widgetBalanceArea->setStyleSheet(QStringLiteral(
        "QLabel { color:#1A2330; font-size:13px; background:transparent; }"
        "QFrame#corrACard { background:white; border:1.5px solid #9CC3EE; border-radius:8px; }"
        "QFrame#gCard { background:white; border:1px solid #E1E6EC; border-radius:8px; }"));
    auto* header = new QHBoxLayout;
    ui->lblProcessStatus->setWordWrap(true);
    ui->lblProcessStatus->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    header->addWidget(ui->lblProcessStatus, 1);
    ui->btnMeasurementGuide->setMinimumSize(82, 28);
    ui->btnMeasurementGuide->setStyleSheet(QStringLiteral(
        "QPushButton { color:#1D5FA8; background:#E7F0FA; border:none;"
        " border-radius:8px; padding:4px 12px; font-size:13px; font-weight:bold; }"
        "QPushButton:disabled { color:#A3ACB8; background:#F2F4F7; }"));
    header->addWidget(ui->btnMeasurementGuide);
    root->addLayout(header);
    ui->barMeasureProgress->setFixedHeight(20);
    root->addWidget(ui->barMeasureProgress);

    auto label = [](const QString& text, const QString& name) {
        auto* value = new QLabel(text);
        value->setObjectName(name);
        value->setWordWrap(true);
        return value;
    };
    auto* cards = new QHBoxLayout;
    cards->setSpacing(8);
    auto* aCard = new QFrame;
    aCard->setObjectName(QStringLiteral("corrACard"));
    auto* aLayout = new QVBoxLayout(aCard);
    aLayout->setContentsMargins(10, 9, 10, 9);
    aLayout->setSpacing(5);
    auto* aTitle = label(QStringLiteral("① 优先调整  corrA"), QStringLiteral("corrATitle"));
    aTitle->setStyleSheet(QStringLiteral("color:#1D5FA8; font-size:14px; font-weight:bold;"));
    aLayout->addWidget(aTitle);
    auto* aBody = new QHBoxLayout;
    aBody->setSpacing(10);
    auto* scale = new QVBoxLayout;
    scale->setSpacing(2);
    scale->addWidget(label(QStringLiteral("1.00"), QStringLiteral("corrAScaleTop")), 0, Qt::AlignHCenter);
    barCorrA = new QProgressBar;
    barCorrA->setObjectName(QStringLiteral("barCorrA"));
    barCorrA->setOrientation(Qt::Vertical);
    barCorrA->setRange(0, 1000);
    barCorrA->setTextVisible(false);
    barCorrA->setFixedWidth(40);
    barCorrA->setMinimumHeight(82);
    scale->addWidget(barCorrA, 1);
    scale->addWidget(label(QStringLiteral("0.00"), QStringLiteral("corrAScaleBottom")), 0, Qt::AlignHCenter);
    aBody->addLayout(scale);
    auto* aValues = new QVBoxLayout;
    aValues->setSpacing(5);
    aValues->addStretch();
    lblCorrAValue = label(QStringLiteral("—"), QStringLiteral("lblCorrAValue"));
    lblCorrAValue->setStyleSheet(QStringLiteral("font-size:28px; font-weight:bold;"));
    lblCorrAValue->setWordWrap(false);
    lblCorrAStatus = label(QStringLiteral("等待信号"), QStringLiteral("lblCorrAStatus"));
    lblCorrAThreshold = label(QString(), QStringLiteral("lblCorrAThreshold"));
    aValues->addWidget(lblCorrAValue);
    aValues->addWidget(lblCorrAStatus);
    aValues->addWidget(lblCorrAThreshold);
    aValues->addWidget(label(QStringLiteral("达到要求即可\n不必追求满格"), QStringLiteral("corrAHelp")));
    aValues->addStretch();
    aBody->addLayout(aValues, 1);
    aLayout->addLayout(aBody, 1);
    ui->lblPositionGuide->setText(QStringLiteral("先调整探头长轴方向"));
    ui->lblPositionGuide->setWordWrap(true);
    ui->lblPositionGuide->setStyleSheet(QStringLiteral("font-size:13px; font-weight:bold; color:#1D5FA8;"));
    aLayout->addWidget(ui->lblPositionGuide);
    aLayout->addWidget(label(QStringLiteral("沿桡骨方向小幅旋转，观察 corrA。"), QStringLiteral("corrAAction")));
    cards->addWidget(aCard, 3);

    auto* gCard = new QFrame;
    gCard->setObjectName(QStringLiteral("gCard"));
    auto* gLayout = new QVBoxLayout(gCard);
    gLayout->setContentsMargins(10, 9, 10, 9);
    gLayout->setSpacing(5);
    ui->lblBPairTitle->setText(QStringLiteral("② 辅助调整  G"));
    ui->lblBPairTitle->setWordWrap(true);
    ui->lblBPairTitle->setStyleSheet(QStringLiteral("font-size:14px; font-weight:bold;"));
    gLayout->addWidget(ui->lblBPairTitle);
    auto* gBody = new QHBoxLayout;
    gBody->setSpacing(8);
    ui->barPairB->setFixedWidth(28);
    ui->barPairB->setMinimumHeight(100);
    ui->barPairB->setTextVisible(false);
    gBody->addWidget(ui->barPairB);
    auto* gValues = new QVBoxLayout;
    gValues->addStretch();
    ui->lblPairBValue->setStyleSheet(QStringLiteral("font-size:17px; font-weight:bold;"));
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
    auxiliary->setSpacing(8);
    ui->lblAPairTitle->setText(QStringLiteral("辅助 D"));
    ui->lblAPairTitle->setStyleSheet(QStringLiteral("font-size:12px;"));
    ui->barPairA->setOrientation(Qt::Horizontal);
    ui->barPairA->setFixedSize(64, 10);
    ui->barPairA->setTextVisible(false);
    ui->lblPairAValue->setStyleSheet(QStringLiteral("font-size:12px;"));
    lblDStatus = label(QStringLiteral("等待信号"), QStringLiteral("lblDStatus"));
    auxiliary->addWidget(ui->lblAPairTitle);
    auxiliary->addWidget(ui->barPairA);
    auxiliary->addWidget(ui->lblPairAValue);
    auxiliary->addWidget(lblDStatus);
    auxiliary->addStretch();
    root->addLayout(auxiliary);
    ui->lblPositionGuideNote->setText(QStringLiteral(
        "优先让 corrA 达到要求；连续计数后保持稳定。提示仅供参考，以有效值计数为准。"));
    ui->lblPositionGuideNote->setWordWrap(true);
    ui->lblPositionGuideNote->setStyleSheet(QStringLiteral(
        "color:#174D8A; background:#EEF4FB; border-radius:8px; padding:8px 12px; font-size:13px;"));
    root->addWidget(ui->lblPositionGuideNote);

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
    ui->barMeasureProgress->setRange(0, mCfg.framesPerRound);
    ui->barMeasureProgress->setValue(0);
    ui->barMeasureProgress->setFormat("有效值：%v / %m");
    ui->barMeasureProgress->setTextVisible(true);

    // ======================================================
    // 3. 文本初始化
    // ======================================================
    ui->lblPairAValue->setText("D=--");
    ui->lblPairBValue->setText("G=--");
    ui->lblProcessStatus->setText("等待开始测量");
    ui->lblProcessStatus->setWordWrap(true);
    ui->lblProcessStatus->setMinimumHeight(0);
    ui->lblGateStats->setText("");
    ui->lblGateStats->setWordWrap(true);

    // ======================================================
    // 4. 进度条样式
    // ======================================================
    QString barStyle = R"(
        QProgressBar {
            border: none;
            border-radius: 6px;
            background-color: #EEF1F5;
            color: #1A2330;
            text-align: center;
        }
        QProgressBar::chunk {
            background-color: #168368;
            border-radius: 6px;
        }
    )";

    ui->barPairA->setStyleSheet(QString(barStyle).replace("#168368", "#1D5FA8"));
    ui->barPairB->setStyleSheet(QString(barStyle).replace("#168368", "#1D5FA8"));
    ui->barMeasureProgress->setStyleSheet(barStyle);
    barCorrA->setStyleSheet(barStyle);
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
        line->setStyleSheet("background-color: #63758a;");
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
    const QString statusStyle = QStringLiteral("color:%1; font-size:12px;")
        .arg(!available ? "#738194" : meets ? "#168368" : "#a66b14");
    if (lblCorrAStatus->styleSheet() != statusStyle) lblCorrAStatus->setStyleSheet(statusStyle);
    const QString barStyle = QStringLiteral(
        "QProgressBar {border:1px solid #cad4df; border-radius:4px; background:#e8edf3;}"
        "QProgressBar::chunk {background:%1; border-radius:3px;}")
        .arg(!available ? "#bdc7d2" : meets ? "#168368" : "#d4a34b");
    if (barCorrA->styleSheet() != barStyle) barCorrA->setStyleSheet(barStyle);
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

void MainWindow::showPostureFeedback(int lagA, int lagB, double pairMidGap)
{
    ui->barPairA->setEnabled(true);
    ui->barPairB->setEnabled(true);

    // 两个条：显示探头角度 / 姿态平衡，偏离目标越多越靠边（中线附近刻度更细）
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

    const int signedLagDiff = lagA - lagB;
    // D = lagA - lagB；G = pairMidGap
    const int dBar = mapToBar(signedLagDiff, mCfg.angleSignedDiffTarget, 6.0, 320.0, 1.4);
    const int gBar = mapToBar(pairMidGap, mCfg.anglePairMidGapTarget, 12.0, 320.0, 1.4);
    ui->barPairA->setValue(qBound(0, dBar, 1000));
    ui->barPairB->setValue(qBound(0, gBar, 1000));

    ui->lblPairAValue->setText(QString("D=%1").arg(signedLagDiff));
    ui->lblPairBValue->setText(QString("G=%1").arg(pairMidGap, 0, 'f', 1));

    // 文字只说明 D/G 是否在允许范围内，不随每帧给出操作指令。
    const bool dOk = signedLagDiff >= mCfg.angleSignedDiffMin &&
                     signedLagDiff <= mCfg.angleSignedDiffMax;
    const bool gOk = pairMidGap >= mCfg.anglePairMidGapMin &&
                     pairMidGap <= mCfg.anglePairMidGapMax;
    lblDStatus->setText(dOk ? QStringLiteral("满足要求") : QStringLiteral("未满足"));
    lblGStatus->setText(gOk ? QStringLiteral("满足要求") : QStringLiteral("未满足"));
    ui->lblGateStats->setText("");
}

void MainWindow::updateMeasureProgress()
{
    ui->barMeasureProgress->setValue(session.validCount());
}
