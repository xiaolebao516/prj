// 主题、各页面布局、结果卡与状态提示、登录页

#include "mainwindow/mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindow/mainwindow_internal.h"
#include "theme/theme.h"
#include "widgets/uikit.h"

#include <QAction>
#include <QCheckBox>
#include <QDateEdit>
#include <QEvent>
#include <QFile>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QPainter>
#include <QPainterPath>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QStyle>
#include <QTableWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <algorithm>

using namespace mainwindow_detail;
using Icons::Glyph;

// ================= 串口扫描等原有代码 =======================================================================================
void MainWindow::scheduleResponsiveLayout()
{
    // Layouts own all geometry; only the aspect-driven reference height and
    // the scaled part image need a pass after Qt has settled the sizes.
    QTimer::singleShot(0, this, [this]() {
        if (!ui) return;
        fitReferenceChartHeight();
        updatePartImage();
    });
}


void MainWindow::initLatestResultPanel()
{
    updateResultPanel();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Resize &&
        (watched == barCorrA || watched == ui->barPairA || watched == ui->barPairB)) {
        addMiddleLineToProgressBar(qobject_cast<QProgressBar*>(watched));
    }
    if (event->type() == QEvent::Resize) {
        if (watched == mainBlock || watched == ui->grpReferenceCurveArea) fitReferenceChartHeight();
        else if (watched == ui->label_32) updatePartImage();
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::updatePatientSelectionUi()
{
    const Theme::Tokens& tokens = Theme::tokens();
    const bool selected = hasCurrentPatient();
    const bool connected = serial && serial->isOpen();
    const bool debugAcquisitionRunning = autoRunning && !patientMeasureRunning;
    const bool nextRoundPending = nextRoundTimer.isActive();
    const bool navigationLocked = patientMeasureRunning || debugAcquisitionRunning || nextRoundPending;
    const bool hasSavedResult = selected && latestMeasurementForPatient(currentPatient.id);

    ui->btnPatientInfo->setText(selected ? QStringLiteral("更换") : QStringLiteral("从档案选择"));
    ui->btnPatientInfo->setEnabled(patientDataWritable && !navigationLocked);
    if (btnNewPatient) btnNewPatient->setEnabled(patientDataWritable && !navigationLocked);

    ui->btnStartMeasurement->setEnabled(
        patientDataWritable &&
        (patientMeasureRunning || (!hasPendingMeasurement && selected && connected)));
    ui->btnStartMeasurement->setText(patientMeasureRunning
        ? QStringLiteral("停止检测")
        : nextRoundPending ? QStringLiteral("立即开始下一轮")
                           : QStringLiteral("开始检测"));
    ui->btnStartMeasurement->setProperty("variant", patientMeasureRunning
        ? QStringLiteral("danger") : QStringLiteral("primary"));
    ui->btnStartMeasurement->setIcon(patientMeasureRunning
        ? Icons::icon(Glyph::Stop, tokens.bad)
        : Icons::icon(Glyph::Play, Qt::white, tokens.ink300));

    QString hint;
    if (patientMeasureRunning) hint = QStringLiteral("检测中不能更换被测者；5 次完成后自动保存。");
    else if (nextRoundPending) hint = QStringLiteral("1 秒后自动开始下一次，也可以立即开始。");
    else if (!connected && !selected) hint = QStringLiteral("请先连接设备，并从档案选择或新建被测者。");
    else if (!connected) hint = QStringLiteral("请先在上方连接设备。");
    else if (!selected) hint = QStringLiteral("请先从档案选择或新建被测者。");
    else if (!patientDataWritable) hint = QStringLiteral("档案处于只读保护，不能开始检测。");
    else if (hasPendingMeasurement) hint = QStringLiteral("上次结果尚未保存，请先在“测量结果”中重试保存。");
    else hint = QStringLiteral("共 5 次，每次约 10 秒；完成一次后 1 秒自动开始下一次。");
    if (lblStartHint) lblStartHint->setText(hint);
    ui->btnStartMeasurement->setToolTip(ui->btnStartMeasurement->isEnabled() ? QString() : hint);

    ui->btnMeasurementGuide->setEnabled(!navigationLocked);
    ui->pushButton->setEnabled(connected && !navigationLocked);
    ui->triggerButton->setEnabled(connected && !patientMeasureRunning && !nextRoundPending);
    const QString deviceHint = connected ? QString() : QStringLiteral("请先连接设备");
    ui->pushButton->setToolTip(connected ? QStringLiteral("单次读取四通道波形，不计入检测") : deviceHint);
    ui->triggerButton->setToolTip(connected ? QStringLiteral("每 80 ms 连续采集，不计入检测") : deviceHint);

    ui->btnReport->setEnabled(!navigationLocked && hasSavedResult);
    if (navigationLocked) ui->btnReport->setToolTip(QStringLiteral("检测或采集进行中，暂不能打开报表"));
    else if (!selected) ui->btnReport->setToolTip(QStringLiteral("请先选择被测者"));
    else if (!hasSavedResult) ui->btnReport->setToolTip(QStringLiteral("当前被测者还没有已保存的检测结果"));
    else ui->btnReport->setToolTip(QStringLiteral("打开最近一次检测的报表"));

    ui->pushButton_2->setEnabled(!patientMeasureRunning && !nextRoundPending);
    for (QSlider* slider : {gainSliderA, gainSliderB, gainSliderC, gainSliderD}) {
        if (slider) slider->setEnabled(!patientMeasureRunning && !nextRoundPending);
    }
    // 正常完成时结果已自动保存；仅在自动保存失败时出现重试入口。
    ui->btnSaveResult->setVisible(hasPendingMeasurement);
    ui->btnSaveResult->setEnabled(patientDataWritable && !nextRoundPending);
    ui->btnArchive->setEnabled(!navigationLocked);
    ui->btnArchive->setToolTip(navigationLocked ? QStringLiteral("检测或采集进行中，暂不能打开档案") : QString());
    ui->btnAdd->setEnabled(patientDataWritable && !nextRoundPending);
    if (btnAccount) btnAccount->setEnabled(!navigationLocked);
    if (QAction* action = findChild<QAction*>(QStringLiteral("manageAccountsAction"))) {
        action->setEnabled(!navigationLocked);
    }

    if (lblDeviceStatus) {
        const bool silent = connected && deviceUnresponsive;
        lblDeviceStatus->setText(!connected ? QStringLiteral("● 未连接")
                                 : silent ? QStringLiteral("● 设备无响应")
                                          : QStringLiteral("● 已连接"));
        lblDeviceStatus->setProperty("state", !connected ? QStringLiteral("warn")
                                              : silent ? QStringLiteral("bad") : QStringLiteral("ok"));
        lblDeviceStatus->setToolTip(silent
            ? QStringLiteral("已发送采集命令，但 %1 秒内没有收到完整波形。\n"
                             "请确认选的是骨密度仪的端口、设备已通电、探头线已插好。")
                  .arg(deviceResponseTimeoutMs / 1000.0, 0, 'f', 1)
            : QString());
    }
    ui->connectButton->setProperty("variant", connected ? QString() : QStringLiteral("primary"));
    for (QWidget* widget : {static_cast<QWidget*>(ui->btnStartMeasurement),
                            static_cast<QWidget*>(ui->connectButton),
                            static_cast<QWidget*>(lblDeviceStatus)}) {
        Theme::repolish(widget);
    }
    updateRunStateUi();
    updateArchiveSelectionBar();
}

void MainWindow::updateRunStateUi()
{
    const bool nextRoundPending = nextRoundTimer.isActive();
    const bool debugAcquisitionRunning = autoRunning && !patientMeasureRunning;
    const int finished = int(session.roundSos.size());
    const int current = qMin(finished + 1, mCfg.roundsPerMeasurement);
    if (roundProgress) {
        roundProgress->setProgress(finished, patientMeasureRunning, mCfg.roundsPerMeasurement);
        roundProgress->setVisible(patientMeasureRunning || nextRoundPending || hasIncompletePatientRounds());
    }
    if (!lblRunState) return;
    QString text;
    Theme::Tone tone = Theme::Tone::Info;
    if (patientMeasureRunning) {
        text = QStringLiteral("正在检测 · 第 %1 / %2 轮").arg(current).arg(mCfg.roundsPerMeasurement);
    } else if (nextRoundPending) {
        text = QStringLiteral("即将开始第 %1 / %2 轮").arg(current).arg(mCfg.roundsPerMeasurement);
    } else if (debugAcquisitionRunning) {
        text = QStringLiteral("连续采集中 · 不计入检测");
    } else if (hasPendingMeasurement) {
        text = QStringLiteral("本次结果尚未保存");
        tone = Theme::Tone::Warn;
    }
    lblRunState->setText(text);
    lblRunState->setVisible(!text.isEmpty());
    Theme::setTone(lblRunState, tone);
}

// ==================== 主题与布局（静谧仪器）====================

void MainWindow::applyTheme()
{
    Theme::installApplicationStyle();
    setStyleSheet(Theme::styleSheet());
}

namespace {

QLabel* captionLabel(const QString& text, QWidget* parent, const QString& role = QStringLiteral("caption"))
{
    auto* label = new QLabel(text, parent);
    label->setProperty("role", role);
    return label;
}

QFrame* separator(QWidget* parent)
{
    auto* line = new QFrame(parent);
    line->setObjectName(QStringLiteral("toolbarSeparator"));
    line->setFixedSize(1, 24);
    return line;
}

void decorate(QPushButton* button, Glyph glyph, const QString& variant, const QColor& iconColor)
{
    button->setProperty("variant", variant);
    button->setIcon(Icons::icon(glyph, iconColor));
    button->setIconSize(QSize(16, 16));
}

// Card header: title, optional muted subtitle, then the caller's trailing widgets.
QHBoxLayout* addCardHeader(QBoxLayout* cardLayout, QWidget* card, const QString& title,
                           const QString& subtitle = QString())
{
    auto* row = new QHBoxLayout;
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(10);
    row->addWidget(captionLabel(title, card, QStringLiteral("cardTitle")));
    if (!subtitle.isEmpty()) row->addWidget(captionLabel(subtitle, card, QStringLiteral("cardSubtitle")));
    row->addStretch(1);
    cardLayout->addLayout(row);
    return row;
}

QFrame* metricCell(QWidget* parent, QLabel* caption, QLabel* value, bool divider)
{
    auto* frame = new QFrame(parent);
    frame->setProperty("role", QStringLiteral("cell"));
    frame->setProperty("divider", divider);
    auto* layout = new QVBoxLayout(frame);
    layout->setContentsMargins(divider ? 14 : 0, 10, 4, 10);
    layout->setSpacing(2);
    caption->setParent(frame);
    value->setParent(frame);
    layout->addWidget(caption);
    layout->addWidget(value);
    return frame;
}

QPixmap pixmapFor(const QIcon& icon, int size)
{
    return icon.pixmap(QSize(size, size), 2.0);
}

} // namespace

void MainWindow::setupToolbar()
{
    const Theme::Tokens& tokens = Theme::tokens();
    auto* toolbar = new QFrame(ui->pageMain);
    toolbar->setObjectName(QStringLiteral("mainToolbar"));
    toolbar->setFixedHeight(64);
    auto* row = new QHBoxLayout(toolbar);
    row->setContentsMargins(24, 0, 20, 0);
    row->setSpacing(8);

    row->addWidget(new BrandMark(28, toolbar));
    row->addSpacing(2);
    auto* title = new QLabel(QStringLiteral("超声骨密度仪"), toolbar);
    title->setObjectName(QStringLiteral("appTitle"));
    row->addWidget(title);
    row->addSpacing(12);
    row->addWidget(separator(toolbar));
    row->addSpacing(12);

    ui->comboPort->setMinimumWidth(240);
    ui->comboPort->setFixedHeight(36);
    auto* portIcon = new QLabel(ui->comboPort);
    portIcon->setObjectName(QStringLiteral("portIcon"));
    portIcon->setPixmap(pixmapFor(Icons::icon(Glyph::Plug, tokens.ink500), 16));
    portIcon->setFixedSize(16, 16);
    portIcon->move(12, 10);
    portIcon->setAttribute(Qt::WA_TransparentForMouseEvents);
    row->addWidget(ui->comboPort);
    row->addWidget(ui->connectButton);
    lblDeviceStatus = new QLabel(toolbar);
    lblDeviceStatus->setObjectName(QStringLiteral("deviceStatus"));
    row->addWidget(lblDeviceStatus, 0, Qt::AlignVCenter);
    row->addSpacing(12);
    row->addWidget(separator(toolbar));
    row->addSpacing(8);

    row->addWidget(captionLabel(QStringLiteral("调试"), toolbar, QStringLiteral("toolbarCaption")));
    ui->pushButton->setText(QStringLiteral("获取波形"));
    ui->triggerButton->setText(QStringLiteral("自动采集"));
    decorate(ui->pushButton, Glyph::Activity, QStringLiteral("ghost"), tokens.ink700);
    decorate(ui->triggerButton, Glyph::Repeat, QStringLiteral("ghost"), tokens.ink700);
    row->addWidget(ui->pushButton);
    row->addWidget(ui->triggerButton);
    row->addSpacing(8);
    row->addWidget(separator(toolbar));
    row->addSpacing(8);

    ui->btnArchive->setText(QStringLiteral("档案"));
    ui->btnReport->setText(QStringLiteral("报表"));
    ui->pushButton_2->setText(QStringLiteral("校准"));
    decorate(ui->btnArchive, Glyph::Folder, QStringLiteral("ghost"), tokens.ink700);
    decorate(ui->btnReport, Glyph::FileText, QStringLiteral("ghost"), tokens.ink700);
    decorate(ui->pushButton_2, Glyph::Target, QStringLiteral("ghost"), tokens.ink700);
    row->addWidget(ui->btnArchive);
    row->addWidget(ui->btnReport);
    row->addWidget(ui->pushButton_2);
    row->addStretch(1);

    lblRunState = new QLabel(toolbar);
    lblRunState->setObjectName(QStringLiteral("runState"));
    lblRunState->hide();
    row->addWidget(lblRunState, 0, Qt::AlignVCenter);
    row->addSpacing(10);

    btnAccount = new QToolButton(toolbar);
    btnAccount->setObjectName(QStringLiteral("accountButton"));
    btnAccount->setPopupMode(QToolButton::InstantPopup);
    btnAccount->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnAccount->setIconSize(QSize(28, 28));
    auto* accountMenu = new QMenu(btnAccount);
    accountMenu->setObjectName(QStringLiteral("accountMenu"));
    actManageAccounts = accountMenu->addAction(Icons::icon(Glyph::Users, tokens.ink700),
                                               QStringLiteral("账号管理…"), this, &MainWindow::manageAccounts);
    accountMenu->addAction(Icons::icon(Glyph::Folder, tokens.ink700),
                           QStringLiteral("打开数据文件夹"), this, &MainWindow::openDataFolder);
    accountMenu->addSeparator();
    accountMenu->addAction(Icons::icon(Glyph::LogOut, tokens.ink700),
                           QStringLiteral("切换账号"), this, &MainWindow::switchAccount);
    btnAccount->setMenu(accountMenu);
    row->addWidget(btnAccount);
    updateAccountUi();

    for (QPushButton* button : {ui->connectButton, ui->pushButton, ui->triggerButton,
                                ui->btnArchive, ui->btnReport, ui->pushButton_2}) {
        button->setMinimumHeight(36);
        button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    // The legacy absolute-positioned container is now empty.
    ui->layoutWidget_2->hide();
    static_cast<QVBoxLayout*>(ui->pageMain->layout())->addWidget(toolbar);
}

void MainWindow::setupRightColumn()
{
    const Theme::Tokens& tokens = Theme::tokens();

    // ---- 被测者 ----
    QGroupBox* patientCard = ui->grpPatientInfoRight;
    patientCard->setTitle(QStringLiteral("被测者"));
    patientCard->setProperty("card", true);
    auto* patientLayout = new QVBoxLayout(patientCard);
    patientLayout->setContentsMargins(20, 16, 20, 18);
    patientLayout->setSpacing(14);
    QHBoxLayout* header = addCardHeader(patientLayout, patientCard, QStringLiteral("被测者"));
    header->setSpacing(4);
    btnNewPatient = new QPushButton(QStringLiteral("新建档案"), patientCard);
    btnNewPatient->setObjectName(QStringLiteral("btnNewPatient"));
    connect(btnNewPatient, &QPushButton::clicked, this, [this]() { openNewPatientDialog(true); });
    for (const auto& [button, glyph] : {std::pair<QPushButton*, Glyph>{ui->btnPatientInfo, Glyph::Swap},
                                        std::pair<QPushButton*, Glyph>{btnNewPatient, Glyph::Plus}}) {
        decorate(button, glyph, QStringLiteral("ghost"), tokens.ink700);
        button->setProperty("btnSize", QStringLiteral("small"));
        button->setIconSize(QSize(14, 14));
        header->addWidget(button);
    }

    auto* identity = new QHBoxLayout;
    identity->setSpacing(14);
    patientAvatar = new AvatarBadge(52, patientCard);
    identity->addWidget(patientAvatar, 0, Qt::AlignVCenter);
    auto* nameColumn = new QVBoxLayout;
    nameColumn->setSpacing(6);
    ui->labelName->setProperty("role", QStringLiteral("personName"));
    ui->labelName->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    nameColumn->addWidget(ui->labelName);
    lblPatientMeta = captionLabel(QString(), patientCard, QStringLiteral("tag"));
    auto* metaRow = new QHBoxLayout;
    metaRow->setContentsMargins(0, 0, 0, 0);
    metaRow->addWidget(lblPatientMeta);
    metaRow->addStretch(1);
    nameColumn->addLayout(metaRow);
    identity->addLayout(nameColumn, 1);
    patientLayout->addLayout(identity);

    auto* details = new QFrame(patientCard);
    details->setObjectName(QStringLiteral("patientDetails"));
    auto* detailGrid = new QGridLayout(details);
    detailGrid->setContentsMargins(0, 12, 0, 12);
    detailGrid->setHorizontalSpacing(16);
    detailGrid->setVerticalSpacing(10);
    const QList<QPair<QString, QLabel*>> detailCells = {
        {QStringLiteral("编号"), ui->labelID}, {QStringLiteral("出生日期"), ui->labelBirth},
        {QStringLiteral("身高"), ui->labelHeight}, {QStringLiteral("体重"), ui->labelWeight}};
    for (int i = 0; i < detailCells.size(); ++i) {
        auto* cellLayout = new QVBoxLayout;
        cellLayout->setSpacing(2);
        cellLayout->addWidget(captionLabel(detailCells[i].first, details, QStringLiteral("fieldLabel")));
        QLabel* value = detailCells[i].second;
        value->setParent(details);
        value->setProperty("role", QStringLiteral("detail"));
        value->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        cellLayout->addWidget(value);
        detailGrid->addLayout(cellLayout, i / 2, i % 2);
    }
    detailGrid->setColumnStretch(0, 1);
    detailGrid->setColumnStretch(1, 1);
    patientLayout->addWidget(details);
    ui->labelGender->hide();
    ui->label_27->hide();
    ui->label_28->hide();

    ui->btnStartMeasurement->setProperty("btnSize", QStringLiteral("large"));
    ui->btnStartMeasurement->setIconSize(QSize(14, 14));
    ui->btnStartMeasurement->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* startBlock = new QVBoxLayout;
    startBlock->setSpacing(8);
    startBlock->addWidget(ui->btnStartMeasurement);
    lblStartHint = captionLabel(QString(), patientCard, QStringLiteral("hint"));
    lblStartHint->setWordWrap(true);
    startBlock->addWidget(lblStartHint);
    patientLayout->addLayout(startBlock);

    // ---- 测量结果 ----
    QGroupBox* resultCard = ui->grpLatestResultRight;
    resultCard->setTitle(QStringLiteral("测量结果"));
    resultCard->setProperty("card", true);
    auto* resultLayout = new QVBoxLayout(resultCard);
    resultLayout->setContentsMargins(20, 16, 20, 16);
    resultLayout->setSpacing(10);
    QHBoxLayout* resultHeader = addCardHeader(resultLayout, resultCard, QStringLiteral("测量结果"));
    lblResultNote = captionLabel(QString(), resultCard);
    lblResultNote->setObjectName(QStringLiteral("resultNote"));
    resultHeader->addWidget(lblResultNote);

    auto* sosRow = new QHBoxLayout;
    sosRow->setSpacing(8);
    ui->lblLatestSOS->setProperty("role", QStringLiteral("hero"));
    ui->lblLatestPart->setProperty("role", QStringLiteral("caption"));
    ui->lblLatestStrength->setProperty("role", QStringLiteral("chip"));
    sosRow->addWidget(ui->lblLatestSOS, 0, Qt::AlignBottom);
    ui->lblLatestPart->setContentsMargins(0, 0, 0, 7);   // sit on the number's baseline
    sosRow->addWidget(ui->lblLatestPart, 0, Qt::AlignBottom);
    sosRow->addStretch();
    sosRow->addWidget(ui->lblLatestStrength, 0, Qt::AlignVCenter);
    resultLayout->addLayout(sosRow);

    tScoreGauge = new TScoreGauge(resultCard);
    tScoreGauge->setObjectName(QStringLiteral("tScoreGauge"));
    tScoreGauge->setToolTip(QStringLiteral("T 值位置：≥ −1 正常；−2.5 ~ −1 不足；≤ −2.5 严重不足"));
    resultLayout->addWidget(tScoreGauge);

    auto* metricsFrame = new QFrame(resultCard);
    metricsFrame->setObjectName(QStringLiteral("metricsFrame"));
    auto* metrics = new QHBoxLayout(metricsFrame);
    metrics->setContentsMargins(0, 0, 0, 0);
    metrics->setSpacing(0);
    const QList<QPair<QLabel*, QLabel*>> metricPairs = {
        {ui->label_35, ui->lblLatestT}, {ui->label_36, ui->lblLatestZ},
        {ui->label_38, ui->lblLatestRisk}, {ui->label_39, ui->lblLatestBoneAge}};
    const QStringList metricNames = {QStringLiteral("T 值"), QStringLiteral("Z 值"),
                                     QStringLiteral("骨折风险"), QStringLiteral("相对骨龄")};
    for (int i = 0; i < metricPairs.size(); ++i) {
        metricPairs[i].first->setText(metricNames[i]);
        metricPairs[i].first->setProperty("role", QStringLiteral("fieldLabel"));
        metricPairs[i].second->setProperty("role", QStringLiteral("metric"));
        metrics->addWidget(metricCell(metricsFrame, metricPairs[i].first, metricPairs[i].second, i > 0), 1);
    }
    for (QLabel* unused : {ui->label_33, ui->label_34, ui->label_37}) unused->hide();
    resultLayout->addWidget(metricsFrame);

    ui->btnSaveResult->setText(QStringLiteral("重试保存"));
    ui->btnSaveResult->setProperty("variant", QStringLiteral("primary"));
    resultLayout->addWidget(ui->btnSaveResult);
    lblRecentHistory = captionLabel(QString(), resultCard, QStringLiteral("history"));
    lblRecentHistory->setObjectName(QStringLiteral("recentHistory"));
    lblRecentHistory->setTextFormat(Qt::PlainText);
    resultLayout->addWidget(lblRecentHistory);

    // ---- 测量部位 ----
    QGroupBox* partCard = ui->grpPartImageRight;
    partCard->setTitle(QStringLiteral("测量部位"));
    auto* partLayout = new QVBoxLayout(partCard);
    partLayout->setContentsMargins(20, 16, 20, 18);
    partLayout->setSpacing(10);
    addCardHeader(partLayout, partCard, QStringLiteral("测量部位"), QStringLiteral("桡骨远端"));
    ui->label_32->setScaledContents(false);
    ui->label_32->setAlignment(Qt::AlignCenter);
    ui->label_32->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->label_32->setMinimumHeight(90);
    ui->label_32->setObjectName(QStringLiteral("label_32"));
    partLayout->addWidget(ui->label_32, 1);
    ui->label_32->installEventFilter(this);
    updateCurrentPatientUI();
}

void MainWindow::setupArchivePage()
{
    const Theme::Tokens& tokens = Theme::tokens();
    QWidget* page = ui->pageArchive;
    delete page->layout();
    page->setFont(font());

    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto* toolbar = new QFrame(page);
    toolbar->setObjectName(QStringLiteral("mainToolbar"));
    toolbar->setFixedHeight(64);
    auto* top = new QHBoxLayout(toolbar);
    top->setContentsMargins(20, 0, 24, 0);
    top->setSpacing(8);
    ui->btnBackFromArchive->setText(QStringLiteral("主界面"));
    decorate(ui->btnBackFromArchive, Glyph::ChevronLeft, QStringLiteral("ghost"), tokens.ink700);
    top->addWidget(ui->btnBackFromArchive);
    top->addWidget(separator(toolbar));
    top->addSpacing(4);
    auto* title = new QLabel(QStringLiteral("档案"), toolbar);
    title->setObjectName(QStringLiteral("appTitle"));
    top->addWidget(title);
    top->addSpacing(16);
    ui->editSearchKeyword->setPlaceholderText(QStringLiteral("姓名或编号"));
    ui->editSearchKeyword->setMinimumWidth(280);
    ui->editSearchKeyword->setClearButtonEnabled(true);
    ui->editSearchKeyword->addAction(Icons::icon(Glyph::Search, tokens.ink400), QLineEdit::LeadingPosition);
    connect(ui->editSearchKeyword, &QLineEdit::returnPressed, this, &MainWindow::on_btnSearchName_clicked);
    top->addWidget(ui->editSearchKeyword);
    chkDateFilter = new QCheckBox(QStringLiteral("检测日期"), toolbar);
    chkDateFilter->setObjectName(QStringLiteral("chkDateFilter"));
    dateFilter = new QDateEdit(QDate::currentDate(), toolbar);
    dateFilter->setObjectName(QStringLiteral("dateFilter"));
    dateFilter->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    dateFilter->setCalendarPopup(true);
    dateFilter->setEnabled(false);
    connect(chkDateFilter, &QCheckBox::toggled, dateFilter, &QWidget::setEnabled);
    top->addSpacing(8);
    top->addWidget(chkDateFilter);
    top->addWidget(dateFilter);
    ui->btnSearchName->setText(QStringLiteral("查找"));
    ui->btnSearchName->setProperty("variant", QStringLiteral("dark"));
    ui->btnShowAll->setText(QStringLiteral("清除"));
    ui->btnShowAll->setProperty("variant", QStringLiteral("link"));
    top->addWidget(ui->btnSearchName);
    top->addWidget(ui->btnShowAll);
    top->addStretch();
    ui->btnAdd->setText(QStringLiteral("新建档案"));
    decorate(ui->btnAdd, Glyph::Plus, QStringLiteral("primary"), Qt::white);
    top->addWidget(ui->btnAdd);
    for (QWidget* control : {static_cast<QWidget*>(ui->btnBackFromArchive), static_cast<QWidget*>(ui->editSearchKeyword),
                             static_cast<QWidget*>(dateFilter), static_cast<QWidget*>(ui->btnSearchName),
                             static_cast<QWidget*>(ui->btnAdd)}) {
        control->setFixedHeight(36);
    }
    root->addWidget(toolbar);

    auto* body = new CardCanvas(page);
    body->setObjectName(QStringLiteral("archiveBody"));
    auto* bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(20, 20, 20, 20);
    bodyLayout->setSpacing(16);

    auto* tableCard = new QFrame(body);
    tableCard->setObjectName(QStringLiteral("tableCard"));
    auto* tableLayout = new QVBoxLayout(tableCard);
    tableLayout->setContentsMargins(1, 1, 1, 1);
    tableLayout->setSpacing(0);
    ui->table->verticalHeader()->hide();
    ui->table->verticalHeader()->setDefaultSectionSize(48);
    ui->table->horizontalHeader()->setMinimumHeight(44);
    ui->table->horizontalHeader()->setHighlightSections(false);
    ui->table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->table->setShowGrid(false);
    ui->table->setAlternatingRowColors(false);
    ui->table->setFrameShape(QFrame::NoFrame);
    ui->table->setMouseTracking(true);
    ui->table->setItemDelegateForColumn(ArchiveNameColumn, new NameAvatarDelegate(
        [this](const QModelIndex& index) {
            const QModelIndex idIndex = index.sibling(index.row(), ArchiveIdColumn);
            return hasCurrentPatient() && idIndex.data().toString() == currentPatient.id;
        }, ui->table));
    tableLayout->addWidget(ui->table, 1);
    auto* tableFooter = captionLabel(QStringLiteral("单击行选中，双击设为当前被测者；勾选框用于批量删除和导出"),
                                     tableCard);
    tableFooter->setObjectName(QStringLiteral("tableFooter"));
    tableLayout->addWidget(tableFooter);
    bodyLayout->addWidget(tableCard, 1);

    auto* actionBar = new QFrame(body);
    actionBar->setObjectName(QStringLiteral("actionBar"));
    auto* actions = new QHBoxLayout(actionBar);
    actions->setContentsMargins(20, 12, 20, 12);
    actions->setSpacing(10);
    actions->addWidget(captionLabel(QStringLiteral("当前选中"), actionBar));
    auto* selectionLabel = new ElidingLabel(actionBar);
    selectionLabel->setFullText(QStringLiteral("未选择"));
    lblArchiveSelection = selectionLabel;
    lblArchiveSelection->setObjectName(QStringLiteral("archiveSelection"));
    actions->addWidget(lblArchiveSelection);
    actions->addSpacing(8);
    ui->btnSelectPatient->setText(QStringLiteral("设为当前被测者"));
    ui->btnSelectPatient->setProperty("variant", QStringLiteral("primary"));
    ui->btnViewHistory->setText(QStringLiteral("检测历史 / 报表"));
    decorate(ui->btnViewHistory, Glyph::Clock, QString(), tokens.ink700);
    btnEditPatient = new QPushButton(QStringLiteral("编辑资料"), actionBar);
    btnEditPatient->setObjectName(QStringLiteral("btnEditPatient"));
    decorate(btnEditPatient, Glyph::Edit, QString(), tokens.ink700);
    connect(btnEditPatient, &QPushButton::clicked, this, [this]() {
        const QString id = selectedArchivePatientId();
        if (!id.isEmpty()) openEditPatientDialog(id);
    });
    actions->addWidget(ui->btnSelectPatient);
    actions->addWidget(ui->btnViewHistory);
    actions->addWidget(btnEditPatient);
    actions->addStretch();
    lblCheckedCount = captionLabel(QString(), actionBar);
    lblCheckedCount->setObjectName(QStringLiteral("checkedCount"));
    actions->addWidget(lblCheckedCount);
    btnExport = new QPushButton(QStringLiteral("导出全部"), actionBar);
    btnExport->setObjectName(QStringLiteral("btnExport"));
    decorate(btnExport, Glyph::Download, QString(), tokens.ink700);
    btnExport->setToolTip(QStringLiteral("把检测记录导出为 CSV 表格（可用 Excel 打开）。"
                                         "勾选了档案时只导出勾选的人。"));
    connect(btnExport, &QPushButton::clicked, this, &MainWindow::exportMeasurements);
    actions->addWidget(btnExport);
    ui->btnDeleteSelected->setText(QStringLiteral("删除勾选项"));
    decorate(ui->btnDeleteSelected, Glyph::Trash, QStringLiteral("dangerOutline"), tokens.bad);
    actions->addWidget(ui->btnDeleteSelected);
    for (QPushButton* button : {ui->btnSelectPatient, ui->btnViewHistory, btnEditPatient, btnExport,
                                ui->btnDeleteSelected}) {
        button->setMinimumHeight(36);
    }
    bodyLayout->addWidget(actionBar);
    body->addCard(tableCard);
    body->addCard(actionBar);
    root->addWidget(body, 1);

    ui->btnSelectPatient->show();
    ui->btnViewHistory->show();
    connect(ui->table, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
        if (item && item->column() == ArchiveIdColumn) updateArchiveSelectionBar();
    });
    connect(ui->table, &QTableWidget::itemSelectionChanged, this, &MainWindow::updateArchiveSelectionBar);
}

void MainWindow::setupMainLayout()
{
    QWidget* page = ui->pageMain;
    auto* pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);
    setupToolbar();

    auto* body = new CardCanvas(page);
    body->setObjectName(QStringLiteral("mainBody"));
    auto* bodyLayout = new QHBoxLayout(body);
    bodyLayout->setContentsMargins(20, 20, 20, 20);
    bodyLayout->setSpacing(16);

    mainBlock = new QWidget(body);
    mainBlock->setObjectName(QStringLiteral("mainBlock"));
    auto* grid = new QGridLayout(mainBlock);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(16);
    grid->addWidget(ui->grpWaveArea, 0, 0);
    grid->addWidget(ui->grpReferenceCurveArea, 0, 1);
    grid->addWidget(ui->grpSpeedArea, 1, 0);
    grid->addWidget(ui->grpProcessArea, 1, 1);
    grid->setColumnStretch(0, 5);
    grid->setColumnStretch(1, 4);
    grid->setRowStretch(0, 0);
    grid->setRowStretch(1, 1);

    for (QGroupBox* group : {ui->grpWaveArea, ui->grpReferenceCurveArea, ui->grpSpeedArea,
                             ui->grpProcessArea, ui->grpPatientInfoRight, ui->grpLatestResultRight,
                             ui->grpPartImageRight}) {
        // The title stays as the card's accessible name; theme.qss does not
        // paint it because each card draws a richer header row itself.
        group->setProperty("card", true);
        Theme::repolish(group);              // the theme is already applied; re-evaluate selectors
        group->setMinimumSize(0, 0);
        group->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        body->addCard(group);
    }

    ui->grpWaveArea->setTitle(QStringLiteral("四通道波形"));
    ui->grpReferenceCurveArea->setTitle(QStringLiteral("年龄 – SOS 参考"));
    ui->grpSpeedArea->setTitle(QStringLiteral("声速趋势"));
    ui->grpProcessArea->setTitle(QStringLiteral("检测过程"));
    auto* waveLayout = new QVBoxLayout(ui->grpWaveArea);
    waveLayout->setContentsMargins(20, 16, 20, 18);
    waveLayout->setSpacing(12);
    QHBoxLayout* waveHeader = addCardHeader(waveLayout, ui->grpWaveArea, QStringLiteral("四通道波形"),
                                            QStringLiteral("原始回波 · 0 – 4095"));
    waveHeader->setObjectName(QStringLiteral("waveHeader"));
    ui->layoutWidget_3->setMinimumSize(0, 0);
    waveLayout->addWidget(ui->layoutWidget_3, 1);

    auto* speedLayout = new QVBoxLayout(ui->grpSpeedArea);
    speedLayout->setContentsMargins(20, 16, 20, 16);
    speedLayout->setSpacing(10);
    addCardHeader(speedLayout, ui->grpSpeedArea, QStringLiteral("声速趋势"), QStringLiteral("最近 50 帧"));
    ui->chartViewSpeed->setMinimumSize(0, 80);
    speedLayout->addWidget(ui->chartViewSpeed, 1);

    auto* referenceLayout = new QVBoxLayout(ui->grpReferenceCurveArea);
    referenceLayout->setContentsMargins(16, 16, 16, 12);
    referenceLayout->setSpacing(6);
    QHBoxLayout* referenceHeader = addCardHeader(referenceLayout, ui->grpReferenceCurveArea,
                                                 QStringLiteral("年龄 – SOS 参考"));
    referenceHeader->setContentsMargins(4, 0, 4, 0);
    const Theme::Tokens& tokens = Theme::tokens();
    auto legend = [&](const QString& text, const QString& swatchCss) {
        auto* item = new QLabel(QStringLiteral("<span style='%1'>%2</span>&nbsp;%3")
                                    .arg(swatchCss, QStringLiteral("●"), text),
                                ui->grpReferenceCurveArea);
        item->setTextFormat(Qt::RichText);
        item->setProperty("role", QStringLiteral("caption"));
        referenceHeader->addWidget(item);
    };
    legend(QStringLiteral("本次"), QStringLiteral("color:%1;").arg(tokens.ink900.name()));
    legend(QStringLiteral("历史"), QStringLiteral("color:%1;").arg(tokens.ink400.name()));
    legend(QStringLiteral("同龄均值 ±1SD"), QStringLiteral("color:%1;").arg(tokens.accent.name()));
    ui->chartViewReference->setMinimumSize(0, 0);
    referenceLayout->addWidget(ui->chartViewReference, 1);
    ui->grpReferenceCurveArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* rightColumn = new QWidget(body);
    rightColumn->setObjectName(QStringLiteral("rightColumn"));
    auto* rightLayout = new QVBoxLayout(rightColumn);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(16);
    setupRightColumn();
    ui->grpPatientInfoRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    ui->grpLatestResultRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    rightLayout->addWidget(ui->grpPatientInfoRight);
    rightLayout->addWidget(ui->grpLatestResultRight);
    rightLayout->addWidget(ui->grpPartImageRight, 1);
    rightColumn->setMinimumWidth(330);

    bodyLayout->addWidget(mainBlock, 25);
    bodyLayout->addWidget(rightColumn, 8);
    pageLayout->addWidget(body, 1);
    ui->mainBodyWidget->hide();

    mainBlock->installEventFilter(this);
    ui->grpReferenceCurveArea->installEventFilter(this);
    setupArchivePage();
    if (ui->menubar) ui->menubar->hide();
}

void MainWindow::fitReferenceChartHeight()
{
    if (!mainBlock || !ui->grpReferenceCurveArea || !ui->grpReferenceCurveArea->layout()) return;
    QWidget* group = ui->grpReferenceCurveArea;
    auto* groupLayout = static_cast<QBoxLayout*>(group->layout());
    const QMargins margins = groupLayout->contentsMargins();
    const int chartWidth = group->width() - margins.left() - margins.right();
    if (chartWidth <= 0) return;
    // Everything above the chart (the header row) plus the layout spacing.
    int headerHeight = 0;
    for (int i = 0; i < groupLayout->count(); ++i) {
        QLayoutItem* item = groupLayout->itemAt(i);
        if (item->widget() == ui->chartViewReference) break;
        headerHeight += item->sizeHint().height() + groupLayout->spacing();
    }
    const int imageMargin = 2 * AgeSosChartWidget::imageMargin;
    int desired = qRound((chartWidth - imageMargin) * ui->chartViewReference->imageAspectRatio())
                  + imageMargin + margins.top() + margins.bottom() + headerHeight;
    // The bottom row must keep the height it needs at its current width (the
    // process card has word-wrapped text, so that is more than its minimum
    // size hint); otherwise this fixed-height row overflows into it.
    int processMinimum = 250;
    for (QWidget* bottom : {static_cast<QWidget*>(ui->grpProcessArea), static_cast<QWidget*>(ui->grpSpeedArea)}) {
        int need = bottom->minimumSizeHint().height();
        if (bottom->hasHeightForWidth() && bottom->width() > 0)
            need = qMax(need, bottom->heightForWidth(bottom->width()));
        processMinimum = qMax(processMinimum, need);
    }
    const int rowGap = mainBlock->layout() ? static_cast<QGridLayout*>(mainBlock->layout())->verticalSpacing() : 16;
    if (mainBlock->height() > 0) {
        desired = qMin(desired, mainBlock->height() - processMinimum - rowGap);
    }
    desired = qMax(desired, 200);
    // The waveform card shares this row, so both use the same height and the
    // row boundary stays aligned across the first two columns.
    for (QWidget* rowWidget : {group, static_cast<QWidget*>(ui->grpWaveArea)}) {
        if (rowWidget->minimumHeight() != desired || rowWidget->maximumHeight() != desired) {
            rowWidget->setFixedHeight(desired);
        }
    }
}

void MainWindow::updatePartImage()
{
    if (!ui->label_32) return;
    static const QPixmap source(QStringLiteral(":/images/Radius.png"));
    const QSize area = ui->label_32->size();
    if (source.isNull() || area.width() <= 4 || area.height() <= 4) return;
    // The illustration sits on its own dark backdrop; extend that backdrop to
    // the whole frame and round the corners so it reads as one panel.
    const qreal dpr = ui->label_32->devicePixelRatioF();
    QPixmap framed(area * dpr);
    framed.setDevicePixelRatio(dpr);
    framed.fill(Qt::transparent);
    const QColor backdrop = source.toImage().pixelColor(2, 2);
    QPainter painter(&framed);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QPainterPath clip;
    clip.addRoundedRect(QRectF(QPointF(0, 0), QSizeF(area)), 10, 10);
    painter.fillPath(clip, backdrop);
    painter.setClipPath(clip);
    const QPixmap scaled = source.scaled(area * dpr, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    const QSizeF logical = QSizeF(scaled.size()) / dpr;
    painter.drawPixmap(QRectF(QPointF((area.width() - logical.width()) / 2.0,
                                      (area.height() - logical.height()) / 2.0), logical),
                       scaled, QRectF(scaled.rect()));
    painter.end();
    ui->label_32->setPixmap(framed);
}

void MainWindow::updateResultPanel()
{
    if (!lblResultNote) return;
    const MeasurementRecord* shown = nullptr;
    bool unsaved = false;
    if (hasPendingMeasurement && hasCurrentPatient() && pendingMeasurement.patientId == currentPatient.id) {
        shown = &pendingMeasurement;
        unsaved = true;
    } else if (hasCurrentPatient()) {
        shown = latestMeasurementForPatient(currentPatient.id);
    }
    MeasurementRecord shownRecord;
    if (shown) {
        shownRecord = withCurrentReference(*shown, currentPatient);
        shown = &shownRecord;
    }

    const auto text = [](const QString& value) {
        return value.trimmed().isEmpty() ? QStringLiteral("--") : value.trimmed();
    };
    QString strength;
    double tValue = std::numeric_limits<double>::quiet_NaN();
    if (!shown) {
        for (QLabel* label : {ui->lblLatestSOS, ui->lblLatestT, ui->lblLatestZ,
                              ui->lblLatestRisk, ui->lblLatestBoneAge}) {
            label->setText(QStringLiteral("--"));
        }
        strength = QStringLiteral("--");
        lblResultNote->setText(hasCurrentPatient() ? QStringLiteral("暂无结果") : QStringLiteral("未选择被测者"));
        lblResultNote->setProperty("state", QString());
    } else {
        ui->lblLatestSOS->setText(text(shown->sos));
        ui->lblLatestT->setText(text(shown->tScore));
        ui->lblLatestZ->setText(text(shown->zScore));
        ui->lblLatestRisk->setText(text(shown->fractureRisk));
        ui->lblLatestBoneAge->setText(shown->boneAge.trimmed().isEmpty()
                                          ? QStringLiteral("--")
                                          : QStringLiteral("%1 岁").arg(shown->boneAge.trimmed()));
        bool tOk = false;
        const double parsedT = shown->tScore.trimmed().toDouble(&tOk);
        if (tOk && std::isfinite(parsedT)) tValue = parsedT;
        strength = text(shown->boneStrength.isEmpty() ? shown->diagnosis : shown->boneStrength);
        const QDateTime time = parsedMeasurementDateTime(shown->measuredAt);
        const QString when = time.isValid() ? time.toString(QStringLiteral("yyyy-MM-dd HH:mm"))
                                            : shown->measuredAt;
        if (unsaved) {
            lblResultNote->setText(QStringLiteral("本次 · %1 · 未保存").arg(when));
            lblResultNote->setProperty("state", QStringLiteral("bad"));
        } else {
            const bool today = time.isValid() && time.date() == QDate::currentDate();
            lblResultNote->setText(QStringLiteral("%1 · %2 · 已保存")
                                       .arg(today ? QStringLiteral("本次") : QStringLiteral("最近一次"), when));
            lblResultNote->setProperty("state", today ? QStringLiteral("ok") : QString());
        }
    }
    if (tScoreGauge) tScoreGauge->setTScore(tValue);
    ui->lblLatestPart->setText(QStringLiteral("m/s · 桡骨"));
    ui->lblLatestStrength->setText(strength == QStringLiteral("--")
                                       ? QStringLiteral("骨强度 --")
                                       : QStringLiteral("骨强度 %1").arg(strength));
    ui->lblLatestStrength->setProperty("state",
        strength == QStringLiteral("正常") ? QStringLiteral("ok")
        : strength == QStringLiteral("不足") ? QStringLiteral("warn")
        : strength == QStringLiteral("严重不足") ? QStringLiteral("bad") : QString());

    QStringList lines;
    if (hasCurrentPatient()) {
        QList<MeasurementRecord> records = measurementsForPatient(currentPatient.id);
        std::sort(records.begin(), records.end(), [](const MeasurementRecord& a, const MeasurementRecord& b) {
            return a.measuredAt > b.measuredAt;
        });
        for (const MeasurementRecord& record : records) {
            if (shown && !unsaved && sameMeasurement(record, *shown)) continue;
            const QDateTime time = parsedMeasurementDateTime(record.measuredAt);
            const QString day = time.isValid() ? time.date().toString(QStringLiteral("yyyy-MM-dd"))
                                               : record.measuredAt.left(10);
            const MeasurementRecord current = withCurrentReference(record, currentPatient);
            const QString recordStrength = current.boneStrength.isEmpty() ? current.diagnosis : current.boneStrength;
            lines << QStringLiteral("%1　%2 m/s　%3").arg(day, text(record.sos), text(recordStrength));
            if (lines.size() >= 3) break;
        }
    }
    lblRecentHistory->setText(lines.isEmpty() ? QStringLiteral("以往记录：暂无")
                                              : QStringLiteral("以往记录\n") + lines.join(QLatin1Char('\n')));
    ui->btnSaveResult->setVisible(hasPendingMeasurement);
    for (QWidget* widget : {static_cast<QWidget*>(lblResultNote), static_cast<QWidget*>(ui->lblLatestStrength)}) {
        Theme::repolish(widget);
    }
    // The patient's sex/age profile selects a reference bitmap with its own aspect ratio.
    fitReferenceChartHeight();
}

void MainWindow::refreshPatientDerivedViews()
{
    refreshTable(patientList);
    updateCurrentPatientUI();
    updateAgeSosReference();
    updateResultPanel();
    updatePatientSelectionUi();
}

// ==================== 登录页 ====================

void MainWindow::setupLoginPage()
{
    QWidget* page = ui->pageLogin;
    delete page->layout();

    auto* root = new QHBoxLayout(page);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ---- brand panel ----
    auto* brand = new BrandPanel(page);
    brand->setObjectName(QStringLiteral("loginBrand"));
    brand->setMinimumWidth(360);
    brand->setMaximumWidth(760);
    brand->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    auto* brandLayout = new QVBoxLayout(brand);
    brandLayout->setContentsMargins(56, 56, 40, 44);
    brandLayout->setSpacing(14);
    brandLayout->addWidget(new BrandMark(36, brand), 0, Qt::AlignLeft);
    brandLayout->addStretch(3);
    auto* productName = new QLabel(QStringLiteral("超声骨密度仪"), brand);
    productName->setProperty("role", QStringLiteral("brandTitle"));
    auto* tagline = new QLabel(QStringLiteral("定量超声 · 桡骨声速检测"), brand);
    tagline->setProperty("role", QStringLiteral("brandTagline"));
    brandLayout->addWidget(productName);
    brandLayout->addWidget(tagline);
    brandLayout->addStretch(5);
    auto* version = new QLabel(QStringLiteral("数据保存在本机 · 结果仅供参考 · 版本 %1").arg(QStringLiteral(APP_VERSION)),
                               brand);
    version->setObjectName(QStringLiteral("loginVersion"));
    version->setProperty("role", QStringLiteral("brandFoot"));
    version->setWordWrap(true);
    brandLayout->addWidget(version);
    root->addWidget(brand, 2);

    // ---- form ----
    auto* formArea = new QWidget(page);
    formArea->setObjectName(QStringLiteral("loginFormArea"));
    auto* formColumn = new QVBoxLayout(formArea);
    formColumn->setContentsMargins(32, 32, 32, 32);
    formColumn->addStretch(1);
    auto* card = new QWidget(formArea);
    card->setObjectName(QStringLiteral("loginCard"));
    card->setMinimumWidth(280);
    card->setMaximumWidth(380);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto* form = new QVBoxLayout(card);
    form->setContentsMargins(0, 0, 0, 0);
    form->setSpacing(8);

    auto* title = new QLabel(QStringLiteral("登录"), card);
    title->setObjectName(QStringLiteral("loginTitle"));
    auto* subtitle = new QLabel(QStringLiteral("使用本机账号登录后开始工作"), card);
    subtitle->setObjectName(QStringLiteral("loginSubtitle"));
    form->addWidget(title);
    form->addWidget(subtitle);
    form->addSpacing(28);

    ui->label_19->setText(QStringLiteral("账号"));
    ui->label_20->setText(QStringLiteral("密码"));
    for (QLabel* label : {ui->label_19, ui->label_20}) label->setProperty("role", QStringLiteral("formLabel"));
    ui->editUsername->setPlaceholderText(QStringLiteral("请输入账号"));
    ui->editPassword->setPlaceholderText(QStringLiteral("请输入密码"));
    ui->editPassword->setEchoMode(QLineEdit::Password);
    for (QLineEdit* edit : {ui->editUsername, ui->editPassword}) {
        edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    form->addWidget(ui->label_19);
    form->addWidget(ui->editUsername);
    form->addSpacing(10);
    form->addWidget(ui->label_20);
    form->addWidget(ui->editPassword);

    ui->lblLoginMsg->setStyleSheet(QString());
    ui->lblLoginMsg->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->lblLoginMsg->setProperty("role", QStringLiteral("error"));
    ui->lblLoginMsg->setWordWrap(true);
    ui->lblLoginMsg->setMinimumHeight(24);
    form->addWidget(ui->lblLoginMsg);

    ui->btnLogin->setText(QStringLiteral("登 录"));
    ui->btnLogin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->btnLogin->setProperty("variant", QStringLiteral("primary"));
    ui->btnLogin->setProperty("btnSize", QStringLiteral("large"));
    form->addWidget(ui->btnLogin);

    auto* centered = new QHBoxLayout;
    centered->addStretch(1);
    centered->addWidget(card, 4);
    centered->addStretch(1);
    formColumn->addLayout(centered);
    formColumn->addStretch(1);
    root->addWidget(formArea, 3);

    for (QWidget* w : {static_cast<QWidget*>(ui->lblLoginMsg), static_cast<QWidget*>(ui->btnLogin),
                       static_cast<QWidget*>(ui->label_19), static_cast<QWidget*>(ui->label_20)}) {
        Theme::repolish(w);
    }
}
