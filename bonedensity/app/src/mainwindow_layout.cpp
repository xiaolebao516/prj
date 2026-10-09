// 主题、各页面布局、结果卡与状态提示、登录页

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
        if (!widget) continue;
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
    }
    updateArchiveSelectionBar();
}

// ==================== UI-REFRESH-001：主题与布局 ====================

void MainWindow::applyTheme()
{
    QFile file(QStringLiteral(":/theme.qss"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(QString::fromUtf8(file.readAll()));
    }
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
    line->setFixedSize(1, 26);
    return line;
}

} // namespace

void MainWindow::setupToolbar()
{
    auto* toolbar = new QFrame(ui->pageMain);
    toolbar->setObjectName(QStringLiteral("mainToolbar"));
    auto* row = new QHBoxLayout(toolbar);
    row->setContentsMargins(18, 10, 18, 10);
    row->setSpacing(8);

    auto* title = new QLabel(QStringLiteral("超声骨密度仪"), toolbar);
    title->setObjectName(QStringLiteral("appTitle"));
    row->addWidget(title);
    row->addSpacing(16);

    row->addWidget(captionLabel(QStringLiteral("设备"), toolbar));
    ui->comboPort->setMinimumWidth(200);
    row->addWidget(ui->comboPort);
    row->addWidget(ui->connectButton);
    lblDeviceStatus = new QLabel(toolbar);
    lblDeviceStatus->setObjectName(QStringLiteral("deviceStatus"));
    row->addWidget(lblDeviceStatus);
    row->addSpacing(12);
    row->addWidget(separator(toolbar));
    row->addSpacing(12);

    row->addWidget(captionLabel(QStringLiteral("调试"), toolbar));
    ui->pushButton->setText(QStringLiteral("获取波形"));
    ui->triggerButton->setText(QStringLiteral("自动采集"));
    row->addWidget(ui->pushButton);
    row->addWidget(ui->triggerButton);
    row->addSpacing(12);
    row->addWidget(separator(toolbar));
    row->addSpacing(12);

    ui->btnArchive->setText(QStringLiteral("档案"));
    ui->btnReport->setText(QStringLiteral("报表"));
    ui->pushButton_2->setText(QStringLiteral("校准"));
    row->addWidget(ui->btnArchive);
    row->addWidget(ui->btnReport);
    row->addWidget(ui->pushButton_2);
    row->addStretch(1);

    btnAccount = new QToolButton(toolbar);
    btnAccount->setObjectName(QStringLiteral("accountButton"));
    btnAccount->setPopupMode(QToolButton::InstantPopup);
    btnAccount->setToolButtonStyle(Qt::ToolButtonTextOnly);
    auto* accountMenu = new QMenu(btnAccount);
    accountMenu->setObjectName(QStringLiteral("accountMenu"));
    actManageAccounts = accountMenu->addAction(QStringLiteral("账号管理…"), this, &MainWindow::manageAccounts);
    accountMenu->addAction(QStringLiteral("打开数据文件夹"), this, &MainWindow::openDataFolder);
    accountMenu->addSeparator();
    accountMenu->addAction(QStringLiteral("切换账号"), this, &MainWindow::switchAccount);
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
    // ---- 被测者 ----
    QGroupBox* patientCard = ui->grpPatientInfoRight;
    patientCard->setTitle(QString());
    patientCard->setProperty("card", true);
    auto* patientLayout = new QVBoxLayout(patientCard);
    patientLayout->setContentsMargins(16, 12, 16, 14);
    patientLayout->setSpacing(8);
    auto* header = new QHBoxLayout;
    header->addWidget(captionLabel(QStringLiteral("被测者"), patientCard, QStringLiteral("cardTitle")));
    header->addStretch();
    ui->btnPatientInfo->setProperty("variant", QStringLiteral("link"));
    btnNewPatient = new QPushButton(QStringLiteral("新建档案"), patientCard);
    btnNewPatient->setObjectName(QStringLiteral("btnNewPatient"));
    btnNewPatient->setProperty("variant", QStringLiteral("link"));
    connect(btnNewPatient, &QPushButton::clicked, this, [this]() { openNewPatientDialog(true); });
    header->addWidget(ui->btnPatientInfo);
    header->addWidget(btnNewPatient);
    patientLayout->addLayout(header);

    auto* nameRow = new QHBoxLayout;
    nameRow->setSpacing(10);
    ui->labelName->setProperty("role", QStringLiteral("personName"));
    lblPatientMeta = captionLabel(QString(), patientCard);
    nameRow->addWidget(ui->labelName);
    nameRow->addWidget(lblPatientMeta, 0, Qt::AlignBottom);
    nameRow->addStretch();
    patientLayout->addLayout(nameRow);

    auto* details = new QGridLayout;
    details->setHorizontalSpacing(16);
    details->setVerticalSpacing(4);
    details->addWidget(ui->labelID, 0, 0);
    details->addWidget(ui->labelBirth, 0, 1);
    details->addWidget(ui->labelHeight, 1, 0);
    details->addWidget(ui->labelWeight, 1, 1);
    for (QLabel* label : {ui->labelID, ui->labelBirth, ui->labelHeight, ui->labelWeight}) {
        label->setProperty("role", QStringLiteral("detail"));
        label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    }
    patientLayout->addLayout(details);
    ui->labelGender->hide();
    ui->label_27->hide();
    ui->label_28->hide();

    ui->btnStartMeasurement->setMinimumHeight(46);
    ui->btnStartMeasurement->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    patientLayout->addSpacing(4);
    patientLayout->addWidget(ui->btnStartMeasurement);
    lblStartHint = captionLabel(QString(), patientCard, QStringLiteral("hint"));
    lblStartHint->setWordWrap(true);
    patientLayout->addWidget(lblStartHint);

    // ---- 测量结果 ----
    QGroupBox* resultCard = ui->grpLatestResultRight;
    resultCard->setTitle(QString());
    resultCard->setProperty("card", true);
    auto* resultLayout = new QVBoxLayout(resultCard);
    resultLayout->setContentsMargins(16, 12, 16, 14);
    resultLayout->setSpacing(8);
    auto* resultHeader = new QHBoxLayout;
    resultHeader->addWidget(captionLabel(QStringLiteral("测量结果"), resultCard, QStringLiteral("cardTitle")));
    resultHeader->addStretch();
    lblResultNote = captionLabel(QString(), resultCard);
    lblResultNote->setObjectName(QStringLiteral("resultNote"));
    resultHeader->addWidget(lblResultNote);
    resultLayout->addLayout(resultHeader);

    auto* sosRow = new QHBoxLayout;
    sosRow->setSpacing(8);
    ui->lblLatestSOS->setProperty("role", QStringLiteral("bigValue"));
    ui->lblLatestPart->setProperty("role", QStringLiteral("caption"));
    ui->lblLatestStrength->setProperty("role", QStringLiteral("chip"));
    sosRow->addWidget(ui->lblLatestSOS, 0, Qt::AlignBottom);
    sosRow->addWidget(ui->lblLatestPart, 0, Qt::AlignBottom);
    sosRow->addStretch();
    sosRow->addWidget(ui->lblLatestStrength, 0, Qt::AlignVCenter);
    resultLayout->addLayout(sosRow);

    auto* metrics = new QGridLayout;
    metrics->setHorizontalSpacing(12);
    metrics->setVerticalSpacing(2);
    const QList<QPair<QLabel*, QLabel*>> metricPairs = {
        {ui->label_35, ui->lblLatestT}, {ui->label_36, ui->lblLatestZ},
        {ui->label_38, ui->lblLatestRisk}, {ui->label_39, ui->lblLatestBoneAge}};
    const QStringList metricNames = {QStringLiteral("T 值"), QStringLiteral("Z 值"),
                                     QStringLiteral("骨折风险"), QStringLiteral("相对骨龄")};
    for (int i = 0; i < metricPairs.size(); ++i) {
        metricPairs[i].first->setText(metricNames[i]);
        metricPairs[i].first->setProperty("role", QStringLiteral("caption"));
        metricPairs[i].second->setProperty("role", QStringLiteral("metric"));
        metrics->addWidget(metricPairs[i].first, 0, i);
        metrics->addWidget(metricPairs[i].second, 1, i);
    }
    for (QLabel* unused : {ui->label_33, ui->label_34, ui->label_37}) unused->hide();
    auto* metricsFrame = new QFrame(resultCard);
    metricsFrame->setObjectName(QStringLiteral("metricsFrame"));
    metricsFrame->setLayout(metrics);
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
    auto* partLayout = new QVBoxLayout(partCard);
    partLayout->setContentsMargins(16, 40, 16, 14);
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
    QWidget* page = ui->pageArchive;
    delete page->layout();
    page->setFont(font());

    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto* toolbar = new QFrame(page);
    toolbar->setObjectName(QStringLiteral("mainToolbar"));
    auto* top = new QHBoxLayout(toolbar);
    top->setContentsMargins(18, 10, 18, 10);
    top->setSpacing(8);
    ui->btnBackFromArchive->setText(QStringLiteral("‹ 主界面"));
    top->addWidget(ui->btnBackFromArchive);
    auto* title = new QLabel(QStringLiteral("档案"), toolbar);
    title->setObjectName(QStringLiteral("appTitle"));
    top->addWidget(title);
    top->addSpacing(20);
    ui->editSearchKeyword->setPlaceholderText(QStringLiteral("姓名或编号"));
    ui->editSearchKeyword->setMinimumWidth(220);
    ui->editSearchKeyword->setClearButtonEnabled(true);
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
    ui->btnAdd->setProperty("variant", QStringLiteral("primary"));
    top->addWidget(ui->btnAdd);
    root->addWidget(toolbar);

    auto* body = new QWidget(page);
    body->setObjectName(QStringLiteral("archiveBody"));
    auto* bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(18, 16, 18, 18);
    bodyLayout->setSpacing(14);

    auto* tableCard = new QFrame(body);
    tableCard->setObjectName(QStringLiteral("tableCard"));
    auto* tableLayout = new QVBoxLayout(tableCard);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(0);
    ui->table->verticalHeader()->hide();
    ui->table->setShowGrid(false);
    ui->table->setAlternatingRowColors(false);
    ui->table->setFrameShape(QFrame::NoFrame);
    tableLayout->addWidget(ui->table, 1);
    auto* tableFooter = captionLabel(QStringLiteral("单击行选中，双击设为当前被测者；勾选框用于批量删除"),
                                     tableCard);
    tableFooter->setObjectName(QStringLiteral("tableFooter"));
    tableLayout->addWidget(tableFooter);
    bodyLayout->addWidget(tableCard, 1);

    auto* actionBar = new QFrame(body);
    actionBar->setObjectName(QStringLiteral("actionBar"));
    auto* actions = new QHBoxLayout(actionBar);
    actions->setContentsMargins(18, 12, 18, 12);
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
    btnEditPatient = new QPushButton(QStringLiteral("编辑资料"), actionBar);
    btnEditPatient->setObjectName(QStringLiteral("btnEditPatient"));
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
    btnExport->setToolTip(QStringLiteral("把检测记录导出为 CSV 表格（可用 Excel 打开）。"
                                         "勾选了档案时只导出勾选的人。"));
    connect(btnExport, &QPushButton::clicked, this, &MainWindow::exportMeasurements);
    actions->addWidget(btnExport);
    ui->btnDeleteSelected->setText(QStringLiteral("删除勾选项"));
    ui->btnDeleteSelected->setProperty("variant", QStringLiteral("dangerOutline"));
    actions->addWidget(ui->btnDeleteSelected);
    bodyLayout->addWidget(actionBar);
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

    auto* body = new QWidget(page);
    body->setObjectName(QStringLiteral("mainBody"));
    auto* bodyLayout = new QHBoxLayout(body);
    bodyLayout->setContentsMargins(16, 14, 16, 16);
    bodyLayout->setSpacing(14);

    mainBlock = new QWidget(body);
    mainBlock->setObjectName(QStringLiteral("mainBlock"));
    auto* grid = new QGridLayout(mainBlock);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(14);
    grid->addWidget(ui->grpWaveArea, 0, 0);
    grid->addWidget(ui->grpReferenceCurveArea, 0, 1);
    grid->addWidget(ui->grpSpeedArea, 1, 0);
    grid->addWidget(ui->grpProcessArea, 1, 1);
    grid->setColumnStretch(0, 5);
    grid->setColumnStretch(1, 4);
    grid->setRowStretch(0, 0);
    grid->setRowStretch(1, 1);

    ui->grpReferenceCurveArea->setTitle(QStringLiteral("年龄 – SOS 参考"));
    for (QGroupBox* group : {ui->grpWaveArea, ui->grpReferenceCurveArea, ui->grpSpeedArea,
                             ui->grpProcessArea, ui->grpPatientInfoRight, ui->grpLatestResultRight,
                             ui->grpPartImageRight}) {
        group->setProperty("card", true);   // theme.qss: title inside the card
        group->style()->unpolish(group);    // the theme is already applied; re-evaluate selectors
        group->style()->polish(group);
        group->setMinimumSize(0, 0);
        group->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    auto* waveLayout = new QVBoxLayout(ui->grpWaveArea);
    waveLayout->setContentsMargins(12, 38, 12, 10);
    ui->layoutWidget_3->setMinimumSize(0, 0);
    waveLayout->addWidget(ui->layoutWidget_3, 1);

    auto* speedLayout = new QVBoxLayout(ui->grpSpeedArea);
    speedLayout->setContentsMargins(14, 38, 14, 12);
    speedLayout->setSpacing(8);
    ui->chartViewSpeed->setMinimumSize(0, 80);
    speedLayout->addWidget(ui->chartViewSpeed, 1);

    auto* referenceLayout = new QVBoxLayout(ui->grpReferenceCurveArea);
    referenceLayout->setContentsMargins(14, 38, 14, 12);
    ui->chartViewReference->setMinimumSize(0, 0);
    referenceLayout->addWidget(ui->chartViewReference, 1);
    ui->grpReferenceCurveArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* rightColumn = new QWidget(body);
    rightColumn->setObjectName(QStringLiteral("rightColumn"));
    auto* rightLayout = new QVBoxLayout(rightColumn);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(14);
    setupRightColumn();
    ui->grpPatientInfoRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    ui->grpLatestResultRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    rightLayout->addWidget(ui->grpPatientInfoRight);
    rightLayout->addWidget(ui->grpLatestResultRight);
    rightLayout->addWidget(ui->grpPartImageRight, 1);
    rightColumn->setMinimumWidth(320);

    bodyLayout->addWidget(mainBlock, 25);
    bodyLayout->addWidget(rightColumn, 9);
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
    const QMargins margins = group->layout()->contentsMargins();
    const int chartWidth = group->width() - margins.left() - margins.right();
    if (chartWidth <= 0) return;
    const int imageMargin = 2 * AgeSosChartWidget::imageMargin;
    int desired = qRound((chartWidth - imageMargin) * ui->chartViewReference->imageAspectRatio())
                  + imageMargin + margins.top() + margins.bottom();
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
    if (mainBlock->height() > 0) {
        desired = qMin(desired, mainBlock->height() - processMinimum - 14);
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
    ui->label_32->setPixmap(source.scaled(area, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
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

    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(24, 24, 24, 18);
    root->addStretch(3);

    auto* card = new QFrame(page);
    card->setObjectName(QStringLiteral("loginCard"));
    card->setFixedWidth(400);
    auto* form = new QVBoxLayout(card);
    form->setContentsMargins(36, 32, 36, 30);
    form->setSpacing(8);

    auto* title = new QLabel(QStringLiteral("超声骨密度仪"), card);
    title->setObjectName(QStringLiteral("loginTitle"));
    auto* subtitle = new QLabel(QStringLiteral("请登录后使用"), card);
    subtitle->setProperty("role", QStringLiteral("caption"));
    form->addWidget(title);
    form->addWidget(subtitle);
    form->addSpacing(18);

    ui->label_19->setText(QStringLiteral("账号"));
    ui->label_20->setText(QStringLiteral("密码"));
    for (QLabel* label : {ui->label_19, ui->label_20}) label->setProperty("role", QStringLiteral("fieldLabel"));
    ui->editUsername->setPlaceholderText(QStringLiteral("请输入账号"));
    ui->editPassword->setPlaceholderText(QStringLiteral("请输入密码"));
    ui->editPassword->setEchoMode(QLineEdit::Password);
    for (QLineEdit* edit : {ui->editUsername, ui->editPassword}) {
        edit->setMinimumHeight(40);
        edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    form->addWidget(ui->label_19);
    form->addWidget(ui->editUsername);
    form->addSpacing(6);
    form->addWidget(ui->label_20);
    form->addWidget(ui->editPassword);

    ui->lblLoginMsg->setStyleSheet(QString());
    ui->lblLoginMsg->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->lblLoginMsg->setProperty("role", QStringLiteral("error"));
    ui->lblLoginMsg->setWordWrap(true);
    ui->lblLoginMsg->setMinimumHeight(20);
    form->addWidget(ui->lblLoginMsg);

    ui->btnLogin->setText(QStringLiteral("登 录"));
    ui->btnLogin->setMinimumHeight(44);
    ui->btnLogin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->btnLogin->setProperty("variant", QStringLiteral("primary"));
    form->addWidget(ui->btnLogin);

    root->addWidget(card, 0, Qt::AlignHCenter);
    root->addStretch(4);

    auto* version = new QLabel(QStringLiteral("版本 %1 · 数据保存在本机 · 结果仅供参考").arg(QStringLiteral(APP_VERSION)),
                               page);
    version->setObjectName(QStringLiteral("loginVersion"));
    version->setProperty("role", QStringLiteral("caption"));
    root->addWidget(version, 0, Qt::AlignHCenter);

    for (QWidget* w : {static_cast<QWidget*>(ui->lblLoginMsg), static_cast<QWidget*>(ui->btnLogin),
                       static_cast<QWidget*>(ui->label_19), static_cast<QWidget*>(ui->label_20)}) {
        w->style()->unpolish(w);
        w->style()->polish(w);
    }
}
