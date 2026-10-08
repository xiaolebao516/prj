// ======================================================
// 逐帧调试开关：改为 true 可恢复每帧详细 qDebug 输出
// （波形数据、首波定位、角度特征、稳定性状态等）
// 正常使用时保持 false，只看每轮结束的闸门统计汇总
// ======================================================
static constexpr bool kDebugPerFrame = false;

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
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {

// A label that gives up its width first: the text is shortened with "…" and
// the full text stays in the tooltip, so neighbouring buttons keep their labels.
class ElidingLabel : public QLabel
{
public:
    using QLabel::QLabel;
    void setFullText(const QString& text)
    {
        fullText_ = text;
        setToolTip(text);
        updateGeometry();
        refresh();
    }
    const QString& fullText() const { return fullText_; }
    QSize minimumSizeHint() const override
    {
        return {fontMetrics().horizontalAdvance(QStringLiteral("测试…")), QLabel::minimumSizeHint().height()};
    }
    QSize sizeHint() const override
    {
        return {fontMetrics().horizontalAdvance(fullText_) + 4, QLabel::sizeHint().height()};
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QLabel::resizeEvent(event);
        refresh();
    }

private:
    void refresh()
    {
        QLabel::setText(fontMetrics().elidedText(fullText_, Qt::ElideRight, qMax(0, width())));
    }
    QString fullText_;
};

int ageOnDate(const QString& birthDay, const QDate& date)
{
    const QDate birth = QDate::fromString(birthDay, QStringLiteral("yyyy-MM-dd"));
    if (!birth.isValid() || !date.isValid() || date < birth) return -1;

    int age = date.year() - birth.year();
    if (birth.addYears(age) > date) --age;
    return age;
}

QDateTime parsedMeasurementDateTime(const QString& value)
{
    QDateTime dateTime = QDateTime::fromString(value, Qt::ISODate);
    if (!dateTime.isValid())
        dateTime = QDateTime::fromString(value, QStringLiteral("yyyy-MM-dd HH:mm"));
    return dateTime;
}

int measurementAge(const MeasurementRecord& record, const PatientInfo& patient)
{
    bool ok = false;
    const int storedAge = record.patientAge.trimmed().toInt(&ok);
    if (ok && storedAge >= 0 && storedAge <= 100) return storedAge;

    const QDateTime measuredAt = parsedMeasurementDateTime(record.measuredAt);
    const QString birthDay = record.patientBirthDay.trimmed().isEmpty()
        ? patient.birthDay : record.patientBirthDay;
    return ageOnDate(birthDay, measuredAt.date());
}

QString measurementGender(const MeasurementRecord& record, const PatientInfo& patient)
{
    return record.patientGender.trimmed().isEmpty()
        ? patient.gender : record.patientGender;
}

// Saved records keep the values computed when they were measured; screens and
// reports show them re-derived from the stored SOS with the current reference,
// so the numbers always agree with the age-SOS chart.
MeasurementRecord withCurrentReference(const MeasurementRecord& record, const PatientInfo& patient)
{
    bool ok = false;
    const double sos = record.sos.trimmed().toDouble(&ok);
    if (!ok || !std::isfinite(sos) || sos <= 0.0) return record;

    const BoneHealth::DerivedResult derived = BoneHealth::deriveResult(
        sos, measurementGender(record, patient), measurementAge(record, patient));
    MeasurementRecord shown = record;
    // A diagnosis that is just the generated verdict is replaced; free text is kept.
    if (record.diagnosis.trimmed().isEmpty() || record.diagnosis == record.boneStrength)
        shown.diagnosis = derived.diagnosis;
    shown.tScore = derived.tScore;
    shown.zScore = derived.zScore;
    shown.boneStrength = derived.strength;
    shown.fractureRisk = derived.fractureRisk;
    shown.boneAge = derived.boneAge;
    return shown;
}

bool sameMeasurement(const MeasurementRecord& left, const MeasurementRecord& right)
{
    if (!left.id.trimmed().isEmpty() && !right.id.trimmed().isEmpty())
        return left.id == right.id;
    return left.patientId == right.patientId &&
           left.measuredAt == right.measuredAt &&
           left.sos == right.sos;
}

bool sameSexProfile(AgeSosChartWidget::Profile left, AgeSosChartWidget::Profile right)
{
    const bool leftFemale = left == AgeSosChartWidget::Profile::Girl ||
                            left == AgeSosChartWidget::Profile::Woman;
    const bool rightFemale = right == AgeSosChartWidget::Profile::Girl ||
                             right == AgeSosChartWidget::Profile::Woman;
    const bool leftMale = left == AgeSosChartWidget::Profile::Boy ||
                          left == AgeSosChartWidget::Profile::Man;
    const bool rightMale = right == AgeSosChartWidget::Profile::Boy ||
                           right == AgeSosChartWidget::Profile::Man;
    return (leftFemale && rightFemale) || (leftMale && rightMale);
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this))
{
    ui->setupUi(this);
    accountsFilePath = QCoreApplication::applicationDirPath() + "/accounts.xml";
    deviceSettingsPath = QCoreApplication::applicationDirPath() + "/device.ini";
    measurementGuideSettingsPath =
        QCoreApplication::applicationDirPath() + "/measurement-guide.ini";
    QString accountError;
    if (!accountStore.loadOrInitialize(accountsFilePath, &accountError)) {
        QMessageBox::warning(this, "账号初始化失败", accountError);
    }
    calibrationFilePath = QCoreApplication::applicationDirPath() + "/calibration.xml";
    QString calibrationError;
    if (!calibrationStore.loadOrInitialize(calibrationFilePath, &calibrationError)) {
        QMessageBox::warning(this, QStringLiteral("校准参数加载失败"), calibrationError);
    } else if (!calibrationError.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("校准参数已恢复"), calibrationError);
    }
    signalProcessor.probeDistanceCD = calibrationStore.parameters().activeD;
    calibrationSignalProcessor.probeDistanceCD = calibrationStore.parameters().activeD;
    QAction* manageAccountsAction = ui->menubar->addAction("账号管理");
    manageAccountsAction->setObjectName("manageAccountsAction");
    manageAccountsAction->setVisible(false);
    connect(manageAccountsAction, &QAction::triggered, this, &MainWindow::manageAccounts);

    applyTheme();

#if defined(BONE_COMPLETE_B_PEAK_EXPERIMENT) || defined(BONE_RELOCK_PRESERVATION_EXPERIMENT) || defined(BONE_DUAL_WINDOW_A_EXPERIMENT)
    if (useDualWindowAQuality) mCfg.roundCorrAMin = 0.78;
#endif
#ifdef BONE_COMPLETE_B_PEAK_EXPERIMENT
    this->setWindowTitle(QStringLiteral("骨密度仪 · B峰补全试测版（仅研发验证）"));
#elif defined(BONE_RELOCK_PRESERVATION_EXPERIMENT)
    this->setWindowTitle(QStringLiteral("骨密度仪 · 稳定簇续接试测版（仅研发验证）"));
#elif defined(BONE_DUAL_WINDOW_A_EXPERIMENT)
    this->setWindowTitle(QStringLiteral("骨密度仪 · 双段评分试测版（仅研发验证）"));
#elif defined(BONE_OBSERVE_BEFORE_G_EXPERIMENT)
    this->setWindowTitle(QStringLiteral("骨密度仪 · 姿态流程试测版（仅研发验证）"));
#else
    this->setWindowTitle(QStringLiteral("超声骨密度仪"));
#endif
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    //this->showFullScreen();
    this->showMaximized();

    // ✅ 默认显示主页面
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);

    deviceWatchdog.setInterval(500);
    connect(&deviceWatchdog, &QTimer::timeout, this, &MainWindow::checkDeviceResponse);
    deviceWatchdog.start();
    connect(&scanTimer, &QTimer::timeout, this, &MainWindow::scanPorts);
    scanTimer.start(1000);
    scanPorts();

    //connect(serial, &QSerialPort::readyRead, this, &MainWindow::handleSerialReadyRead);
    connect(serial, &QSerialPort::errorOccurred, this, &MainWindow::handleSerialError);

    setupLoginPage();
    setupMainLayout();
    setupChart();
    setupSpeedChart(); // ✅ 初始化声速趋势图
    setupSpeedDebugPanel();  // ✅ 初始化声速调试显示区域
    initProcessPanel();       // 初始化检查过程示意区
    initLatestResultPanel();  // 初始化右侧最近一次测量结果区
    setupReportPage();

    updatePatientSelectionUi();

    connect(ui->pushButton, &QPushButton::clicked,
            this, &MainWindow::on_btnAcquireWaveform_clicked);
    connect(ui->btnReport, &QPushButton::clicked, this, [this]() {
        if (!hasCurrentPatient()) {
            QMessageBox::information(this, "报表", "请先选择被测者。");
            return;
        }
        const MeasurementRecord* latest = latestMeasurementForPatient(currentPatient.id);
        if (!latest) {
            QMessageBox::information(this, "报表", "当前被测者还没有已保存的检测结果。");
            return;
        }
        showReportFrom(ui->pageMain, currentPatient, *latest);
    });
    connect(ui->editUsername, &QLineEdit::returnPressed, this, &MainWindow::on_btnLogin_clicked);
    connect(ui->editPassword, &QLineEdit::returnPressed, this, &MainWindow::on_btnLogin_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked,
            this, &MainWindow::openCalibrationDialog);
    // ✅ 初始化滤波器
    signalProcessor.designFIR(1250000.0, 600000.0, 62500000.0);

    // ✅ 加载患者数据库
    xmlFilePath = QCoreApplication::applicationDirPath() + "/patients.xml";
    measurementsFilePath = QCoreApplication::applicationDirPath() + "/measurements.xml";
    loadPatients();
    //ui->comboPart->setCurrentIndex(-1);
    //ui->dPartCombo->setCurrentIndex(-1);
    refreshTable(patientList);
    updatePatientSelectionUi();



    autoTimer = new QTimer(this);
    connect(autoTimer,&QTimer::timeout,this,&MainWindow::sendCmd);

    nextRoundTimer.setSingleShot(true);
    connect(&nextRoundTimer, &QTimer::timeout, this, [this]() {
        const bool stillEligible =
            !patientMeasureRunning &&
            patientDataWritable &&
            !hasPendingMeasurement &&
            hasCurrentPatient() &&
            currentPatient.id == pendingNextRoundPatientId &&
            pendingNextRoundFinishedRounds > 0 &&
            pendingNextRoundFinishedRounds == roundSosList.size() &&
            pendingNextRoundFinishedRounds < normalMeasureRounds &&
            serial && serial->isOpen();
        if (!stillEligible) {
            cancelPendingNextPatientRound();
            updatePatientSelectionUi();
            return;
        }

        closeRoundFinishedTip();
        startPatientMeasurement(normalMeasureRounds, false);
    });

    // 自适应屏幕分辨率：设计稿 1920x1080，按屏幕等比缩放
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeo = screen->availableGeometry();
        int sw = screenGeo.width();
        int sh = screenGeo.height();
        if (sw > 0 && sh > 0) {
            double scaleW = (double)sw / 1920.0;
            double scaleH = (double)sh / 1080.0;
            double scale = qMin(scaleW, scaleH);
            if (scale < 1.0) {
                this->resize(qRound(1920 * scale), qRound(1080 * scale));
            }
            if (scale < 1.0 || sw > 1920) {
                this->move(screenGeo.x() + (sw - this->width()) / 2,
                           screenGeo.y() + (sh - this->height()) / 2);
            }
        }
    }

    scheduleResponsiveLayout();
}

//先登录===============================================================================
void MainWindow::on_btnLogin_clicked() {
    QString user = ui->editUsername->text().trimmed();
    QString pass = ui->editPassword->text();

    ui->lblLoginMsg->clear();

    AccountInfo account;
    if (accountStore.authenticate(user, pass, &account)) {
        currentAccount = account;
        ui->editPassword->clear();
        if (QAction* action = findChild<QAction*>("manageAccountsAction")) {
            action->setVisible(currentAccount.role == "admin");
        }
        updateAccountUi();
        ui->stackedWidget->setCurrentWidget(ui->pageMain);
        scheduleResponsiveLayout();
        backupDataDaily();
        return;
    }
    ui->lblLoginMsg->setText("账号或密码错误，请重试");
}
//====================================================================================

MainWindow::~MainWindow() {
    if (experimentLog.active()) {
        experimentLog.write({{"event", "application_closed"}});
        experimentLog.close();
    }
    if (serial->isOpen()) serial->close();
    delete ui;
}

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

void MainWindow::closeEvent(QCloseEvent *event)
{
    const bool nextRoundWasPending = nextRoundTimer.isActive();
    const int pendingFinishedRounds = pendingNextRoundFinishedRounds;
    cancelPendingNextPatientRound();
    const bool patientMeasurementWasRunning = patientMeasureRunning;
    const bool patientTimerWasActive =
        patientMeasurementWasRunning && autoTimer && autoTimer->isActive();
    const bool incompleteMeasurementWasPresent = hasIncompletePatientRounds();

    if (patientMeasurementWasRunning) {
        experimentLog.write({{"event", "close_confirmation_pause"}});
        if (autoTimer && autoTimer->isActive()) autoTimer->stop();
        patientMeasureRunning = false;
        acquireMode = DebugAcquireMode;
        updatePatientSelectionUi();
    }

    const auto resumePatientMeasurement = [this,
                                           patientMeasurementWasRunning,
                                           patientTimerWasActive]() {
        if (!patientMeasurementWasRunning || !serial || !serial->isOpen()) return;
        rxBuffer.clear();
        clearFrameAssembly();
        samplesA.clear();
        samplesB.clear();
        samplesC.clear();
        samplesD.clear();
        chReceived[0] = chReceived[1] = chReceived[2] = chReceived[3] = false;
        serial->readAll();
        acquireMode = PatientMeasureMode;
        patientMeasureRunning = true;
        experimentLog.write({{"event", "close_cancelled_resume"}});
        if (patientTimerWasActive) autoTimer->start(80);
        updatePatientSelectionUi();
    };
    // Cancelling the close must not lose the guarded 1 s automatic next round.
    const auto restoreAfterCancel = [this, &resumePatientMeasurement,
                                     nextRoundWasPending, pendingFinishedRounds]() {
        resumePatientMeasurement();
        if (nextRoundWasPending) scheduleNextPatientRound(pendingFinishedRounds);
    };

    if (hasPendingMeasurement) {
        const QMessageBox::StandardButton choice = QMessageBox::warning(
            this,
            QStringLiteral("检测结果尚未保存"),
            QStringLiteral("本次检测结果尚未保存。请选择“重试”再次保存，或明确放弃后退出。"),
            QMessageBox::Retry | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Retry);
        if (choice == QMessageBox::Retry) {
            if (trySavePendingMeasurement()) event->accept();
            else {
                event->ignore();
                restoreAfterCancel();
            }
            return;
        }
        if (choice == QMessageBox::Discard) {
            event->accept();
            return;
        }
        event->ignore();
        restoreAfterCancel();
        return;
    }

    if (incompleteMeasurementWasPresent) {
        const QMessageBox::StandardButton choice = QMessageBox::warning(
            this,
            QStringLiteral("检测尚未完成"),
            QStringLiteral("当前被测者已完成 %1/%2 次测量，本组检测尚未完成。"
                           "退出会丢失本组进度，是否仍要退出？")
                .arg(roundSosList.size())
                .arg(normalMeasureRounds),
            QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Cancel);
        if (choice == QMessageBox::Discard) event->accept();
        else {
            event->ignore();
            restoreAfterCancel();
        }
        return;
    }

    event->accept();
}

void MainWindow::manageAccounts()
{
    if (currentAccount.role != "admin") return;

    QDialog dialog(this);
    dialog.setObjectName(QStringLiteral("accountDialog"));
    dialog.setWindowTitle(QStringLiteral("账号管理"));
    dialog.resize(620, 420);
    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(22, 18, 22, 18);
    layout->setSpacing(12);

    auto* header = new QHBoxLayout;
    auto* title = new QLabel(QStringLiteral("账号管理"), &dialog);
    title->setProperty("role", QStringLiteral("dialogTitle"));
    auto* note = new QLabel(QStringLiteral("账号用于区分操作人；密码只以加密摘要保存在本机"), &dialog);
    note->setProperty("role", QStringLiteral("caption"));
    header->addWidget(title);
    header->addStretch();
    header->addWidget(note, 0, Qt::AlignBottom);
    layout->addLayout(header);

    auto* table = new QTableWidget(&dialog);
    table->setObjectName(QStringLiteral("accountTable"));
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({QStringLiteral("账号"), QStringLiteral("状态"), QStringLiteral("角色")});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->verticalHeader()->hide();
    table->setShowGrid(false);
    table->setFrameShape(QFrame::NoFrame);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    auto* tableCard = new QFrame(&dialog);
    tableCard->setObjectName(QStringLiteral("tableCard"));
    auto* tableLayout = new QVBoxLayout(tableCard);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->addWidget(table);
    layout->addWidget(tableCard, 1);

    auto* buttons = new QHBoxLayout;
    buttons->setSpacing(8);
    auto* add = new QPushButton(QStringLiteral("新建账号"), &dialog);
    add->setProperty("variant", QStringLiteral("primary"));
    auto* toggle = new QPushButton(QStringLiteral("停用"), &dialog);
    auto* reset = new QPushButton(QStringLiteral("重置密码"), &dialog);
    auto* remove = new QPushButton(QStringLiteral("删除"), &dialog);
    remove->setProperty("variant", QStringLiteral("dangerOutline"));
    auto* close = new QPushButton(QStringLiteral("关闭"), &dialog);
    buttons->addWidget(add);
    buttons->addWidget(toggle);
    buttons->addWidget(reset);
    buttons->addWidget(remove);
    buttons->addStretch();
    buttons->addWidget(close);
    layout->addLayout(buttons);

    const auto selected = [table]() {
        const int row = table->currentRow();
        return row >= 0 && table->item(row, 0) && table->selectionModel()->isRowSelected(row, QModelIndex())
            ? table->item(row, 0)->text() : QString();
    };
    const auto updateButtons = [this, selected, toggle, reset, remove]() {
        const QString name = selected();
        bool enabled = true;
        for (const AccountInfo& account : accountStore.accounts()) {
            if (account.username == name) enabled = account.enabled;
        }
        toggle->setText(enabled ? QStringLiteral("停用") : QStringLiteral("启用"));
        for (QPushButton* button : {toggle, reset, remove}) button->setEnabled(!name.isEmpty());
    };
    // Actions keep the dialog open and refresh the list in place.
    const auto reload = [this, table, updateButtons](const QString& keepSelected) {
        const QList<AccountInfo>& accounts = accountStore.accounts();
        table->setRowCount(accounts.size());
        for (int i = 0; i < accounts.size(); ++i) {
            table->setItem(i, 0, new QTableWidgetItem(accounts[i].username));
            auto* state = new QTableWidgetItem(accounts[i].enabled ? QStringLiteral("启用") : QStringLiteral("已停用"));
            state->setForeground(accounts[i].enabled ? QColor(0x1B, 0x7A, 0x4B) : QColor(0x9A, 0x5B, 0x00));
            table->setItem(i, 1, state);
            table->setItem(i, 2, new QTableWidgetItem(accounts[i].role == QStringLiteral("admin")
                                                          ? QStringLiteral("管理员") : QStringLiteral("普通账号")));
            if (accounts[i].username == keepSelected) table->selectRow(i);
        }
        updateButtons();
    };
    reload(QString());
    connect(table, &QTableWidget::itemSelectionChanged, &dialog, updateButtons);
    connect(close, &QPushButton::clicked, &dialog, &QDialog::accept);

    connect(add, &QPushButton::clicked, &dialog, [this, &dialog, reload]() {
        bool ok = false;
        const QString name = QInputDialog::getText(&dialog, QStringLiteral("新建账号"), QStringLiteral("账号："),
                                                   QLineEdit::Normal, QString(), &ok);
        if (!ok) return;
        const QString pass = QInputDialog::getText(&dialog, QStringLiteral("新建账号"), QStringLiteral("密码："),
                                                   QLineEdit::Password, QString(), &ok);
        if (!ok) return;
        QString error;
        if (!accountStore.createUser(name, pass, &error)) {
            QMessageBox::warning(&dialog, QStringLiteral("新建失败"), error);
            return;
        }
        reload(name.trimmed());
    });
    connect(toggle, &QPushButton::clicked, &dialog, [this, &dialog, selected, reload]() {
        const QString name = selected();
        if (name.isEmpty()) return;
        for (const AccountInfo& account : accountStore.accounts()) {
            if (account.username != name) continue;
            QString error;
            if (!accountStore.setEnabled(name, !account.enabled, &error)) {
                QMessageBox::warning(&dialog, QStringLiteral("操作失败"), error);
                return;
            }
            reload(name);
            return;
        }
    });
    connect(reset, &QPushButton::clicked, &dialog, [this, &dialog, selected, reload]() {
        const QString name = selected();
        if (name.isEmpty()) return;
        bool ok = false;
        const QString pass = QInputDialog::getText(&dialog, QStringLiteral("重置密码"),
                                                   QStringLiteral("账号 %1 的新密码：").arg(name),
                                                   QLineEdit::Password, QString(), &ok);
        if (!ok) return;
        QString error;
        if (!accountStore.resetPassword(name, pass, &error)) {
            QMessageBox::warning(&dialog, QStringLiteral("重置失败"), error);
            return;
        }
        reload(name);
        QMessageBox::information(&dialog, QStringLiteral("重置密码"), QStringLiteral("密码已重置。"));
    });
    connect(remove, &QPushButton::clicked, &dialog, [this, &dialog, selected, reload]() {
        const QString name = selected();
        if (name.isEmpty()) return;
        if (QMessageBox::question(&dialog, QStringLiteral("确认删除"),
                                  QStringLiteral("确定删除账号“%1”？").arg(name)) != QMessageBox::Yes) {
            return;
        }
        QString error;
        if (!accountStore.deleteUser(name, &error)) {
            QMessageBox::warning(&dialog, QStringLiteral("删除失败"), error);
            return;
        }
        reload(QString());
    });
    dialog.exec();
}

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
        ui->lblProcessStatus->setStyleSheet(
            "font-size: 14px; color: #9A5B00; font-weight: bold;"
            );
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
        ui->lblProcessStatus->setStyleSheet(
            "font-size: 14px; color: #9A5B00; font-weight: bold;"
            );
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


void MainWindow::initLatestResultPanel()
{
    updateResultPanel();
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

void MainWindow::setupReportPage()
{
    QVBoxLayout* pageLayout = new QVBoxLayout(ui->pageReport);
    pageLayout->setContentsMargins(16, 12, 16, 12);
    pageLayout->setSpacing(10);

    QHBoxLayout* toolbar = new QHBoxLayout();
    QPushButton* backButton = new QPushButton("返回", ui->pageReport);
    backButton->setObjectName(QStringLiteral("reportBackButton"));
    QPushButton* exportButton = new QPushButton("导出 PDF", ui->pageReport);
    QPushButton* printButton = new QPushButton("打印", ui->pageReport);
    toolbar->addWidget(backButton);
    toolbar->addStretch();
    toolbar->addWidget(exportButton);
    toolbar->addWidget(printButton);
    pageLayout->addLayout(toolbar);

    reportWidget = new ReportWidget(ui->pageReport);
    pageLayout->addWidget(reportWidget, 1);

    connect(backButton, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(reportReturnPage ? reportReturnPage : ui->pageMain);
        scheduleResponsiveLayout();
    });
    connect(printButton, &QPushButton::clicked, this, [this]() {
        QPrinter printer(QPrinter::HighResolution);
        printer.setPageSize(QPageSize(QPageSize::A4));
        printer.setPageOrientation(QPageLayout::Portrait);
        printer.setPageMargins(QMarginsF(8, 8, 8, 8), QPageLayout::Millimeter);
        QPrintDialog dialog(&printer, this);
        dialog.setWindowTitle("打印检测报表");
        if (dialog.exec() == QDialog::Accepted && !renderReportToPrinter(&printer)) {
            QMessageBox::warning(this, "打印失败", "无法生成打印内容，请检查打印机设置后重试。");
        }
    });
    connect(exportButton, &QPushButton::clicked, this, [this]() {
        const ReportData& data = reportWidget->reportData();
        QString fileName = QString("%1_%2_%3.pdf")
                               .arg(data.patientName, data.patientId,
                                    data.measuredAt.left(10));
        fileName.replace(QRegularExpression(R"([\\/:*?\"<>|])"), "_");
        const QString defaultPath = QStandardPaths::writableLocation(
                                        QStandardPaths::DocumentsLocation)
                                    + "/" + fileName;
        QString outputPath = QFileDialog::getSaveFileName(
            this, "导出检测报表", defaultPath, "PDF 文件 (*.pdf)");
        if (outputPath.isEmpty()) return;
        if (!outputPath.endsWith(".pdf", Qt::CaseInsensitive)) outputPath += ".pdf";

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(outputPath);
        printer.setPageSize(QPageSize(QPageSize::A4));
        printer.setPageOrientation(QPageLayout::Portrait);
        printer.setPageMargins(QMarginsF(8, 8, 8, 8), QPageLayout::Millimeter);
        if (!renderReportToPrinter(&printer)) {
            QMessageBox::warning(this, "导出失败", "无法写入 PDF 文件，请检查保存位置后重试。");
            return;
        }
        QMessageBox::information(this, "导出完成", "检测报表已保存为 PDF 文件。");
    });
}

ReportData MainWindow::buildReportData(const PatientInfo& patient,
                                       const MeasurementRecord& savedMeasurement) const
{
    const MeasurementRecord measurement = withCurrentReference(savedMeasurement, patient);
    auto fallback = [](const QString& snapshot, const QString& current) {
        return snapshot.trimmed().isEmpty() ? current : snapshot;
    };
    auto withUnit = [](const QString& value, const QString& unit) {
        return value.trimmed().isEmpty() ? QString() : value + " " + unit;
    };

    ReportData data;
    data.patientName = fallback(measurement.patientName, patient.name);
    data.patientId = measurement.patientId.isEmpty() ? patient.id : measurement.patientId;
    data.gender = fallback(measurement.patientGender, patient.gender);
    data.birthDay = fallback(measurement.patientBirthDay, patient.birthDay);
    data.height = withUnit(fallback(measurement.patientHeight, patient.height), "cm");
    data.weight = withUnit(fallback(measurement.patientWeight, patient.weight), "kg");
    data.part = measurement.part;
    data.sos = withUnit(measurement.sos, "m/s");
    data.tScore = measurement.tScore;
    data.zScore = measurement.zScore;
    data.boneStrength = measurement.boneStrength.isEmpty()
        ? measurement.diagnosis : measurement.boneStrength;
    data.operatorName = measurement.operatorName;

    const QDateTime measuredDateTime = QDateTime::fromString(measurement.measuredAt, Qt::ISODate);
    data.measuredAt = measuredDateTime.isValid()
        ? measuredDateTime.toString("yyyy-MM-dd HH:mm") : measurement.measuredAt;

    data.age = measurement.patientAge;
    if (data.age.isEmpty() && measuredDateTime.isValid()) {
        const QDate birthDate = QDate::fromString(data.birthDay, "yyyy-MM-dd");
        const int age = ageOnDate(data.birthDay, measuredDateTime.date());
        if (birthDate.isValid() && age >= 0) {
            data.age = QString::number(age);
        }
    }

    QStringList diagnosisParts;
    if (!data.boneStrength.isEmpty()) {
        diagnosisParts << "骨强度评估：" + data.boneStrength;
    }
    diagnosisParts << "相对骨折风险："
                          + (measurement.fractureRisk.trimmed().isEmpty()
                                 ? QStringLiteral("--")
                                 : measurement.fractureRisk);
    diagnosisParts << (measurement.boneAge.isEmpty()
                           ? QStringLiteral("相对骨龄：--")
                           : QStringLiteral("相对骨龄：%1 岁").arg(measurement.boneAge));
    if (!measurement.diagnosis.isEmpty() && measurement.diagnosis != data.boneStrength) {
        diagnosisParts << measurement.diagnosis;
    }
    data.diagnosis = diagnosisParts.join("；");
    data.ageSosChart = buildAgeSosChartData(patient, measurement, true);
    return data;
}

AgeSosChartData MainWindow::buildAgeSosChartData(
    const PatientInfo& patient,
    const MeasurementRecord& focalMeasurement,
    bool cutoffAtFocal) const
{
    AgeSosChartData data;
    data.hasPatient = !patient.id.trimmed().isEmpty();
    data.gender = measurementGender(focalMeasurement, patient);
    data.focalAge = measurementAge(focalMeasurement, patient);
    const AgeSosChartWidget::Profile focalProfile =
        AgeSosChartWidget::profileFor(data.gender, data.focalAge);
    const QDateTime focalDateTime = parsedMeasurementDateTime(focalMeasurement.measuredAt);

    QList<MeasurementRecord> candidates = measurementsForPatient(patient.id);
    bool focalPresent = false;
    for (const MeasurementRecord& record : std::as_const(candidates)) {
        if (sameMeasurement(record, focalMeasurement)) {
            focalPresent = true;
            break;
        }
    }
    if (!focalPresent) candidates.append(focalMeasurement);

    for (const MeasurementRecord& record : std::as_const(candidates)) {
        const bool isFocal = sameMeasurement(record, focalMeasurement);
        const QDateTime recordDateTime = parsedMeasurementDateTime(record.measuredAt);
        if (cutoffAtFocal) {
            if (focalDateTime.isValid()) {
                if (!isFocal && (!recordDateTime.isValid() || recordDateTime > focalDateTime))
                    continue;
            } else if (!isFocal) {
                continue;
            }
        }

        data.hasMeasurementRecords = true;
        const int age = measurementAge(record, patient);
        const QString gender = measurementGender(record, patient);
        const AgeSosChartWidget::Profile recordProfile =
            AgeSosChartWidget::profileFor(gender, age);
        bool sosOk = false;
        const double sos = record.sos.trimmed().toDouble(&sosOk);
        if (!sosOk || !std::isfinite(sos) || recordProfile == AgeSosChartWidget::Profile::None)
            continue;

        if (recordProfile != focalProfile) {
            if (sameSexProfile(recordProfile, focalProfile) &&
                AgeSosChartWidget::supportsPoint(gender, age, sos)) {
                ++data.omittedOtherProfileCount;
            }
            continue;
        }
        if (!AgeSosChartWidget::supportsPoint(gender, age, sos)) continue;
        data.points.append({age, sos, record.measuredAt, isFocal});
    }

    std::stable_sort(data.points.begin(), data.points.end(),
                     [](const AgeSosMeasurementPoint& left,
                        const AgeSosMeasurementPoint& right) {
        const QDateTime leftTime = parsedMeasurementDateTime(left.measuredAt);
        const QDateTime rightTime = parsedMeasurementDateTime(right.measuredAt);
        if (leftTime.isValid() && rightTime.isValid()) return leftTime < rightTime;
        if (leftTime.isValid() != rightTime.isValid()) return leftTime.isValid();
        return left.measuredAt < right.measuredAt;
    });
    return data;
}

void MainWindow::showReport(const PatientInfo& patient,
                            const MeasurementRecord& measurement)
{
    reportWidget->setReportData(buildReportData(patient, measurement));
    ui->stackedWidget->setCurrentWidget(ui->pageReport);
}

bool MainWindow::renderReportToPrinter(QPrinter* printer)
{
    if (!printer || !reportWidget) return false;
    QPainter painter;
    if (!painter.begin(printer)) return false;
    const QRect pageRect = printer->pageLayout().paintRectPixels(printer->resolution());
    reportWidget->renderReport(&painter, pageRect);
    return painter.end();
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



void MainWindow::on_btnPatientInfo_clicked()
{
    if (patientMeasureRunning) return;
    on_btnArchive_clicked();
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

void MainWindow::updateCurrentPatientUI() {
    const QString empty = QStringLiteral("--");
    const auto value = [&empty](const QString& text, const QString& unit = QString()) {
        return text.trimmed().isEmpty() ? empty : text + unit;
    };
    ui->labelName->setText(currentPatient.name.isEmpty() ? QStringLiteral("未选择") : currentPatient.name);
    QStringList meta;
    if (!currentPatient.gender.isEmpty()) meta << currentPatient.gender;
    const int age = ageOnDate(currentPatient.birthDay, QDate::currentDate());
    if (age >= 0) meta << QStringLiteral("%1 岁").arg(age);
    if (lblPatientMeta) lblPatientMeta->setText(meta.join(QStringLiteral(" · ")));
    ui->labelID->setText(QStringLiteral("编号　%1").arg(value(currentPatient.id)));
    ui->labelGender->setText(QStringLiteral("性别　%1").arg(value(currentPatient.gender)));
    ui->labelBirth->setText(QStringLiteral("出生　%1").arg(value(currentPatient.birthDay)));
    ui->labelHeight->setText(QStringLiteral("身高　%1").arg(value(currentPatient.height, QStringLiteral(" cm"))));
    ui->labelWeight->setText(QStringLiteral("体重　%1").arg(value(currentPatient.weight, QStringLiteral(" kg"))));
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
    processValidCount = 0;
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

// ==================== 档案管理 ===============================================================================================
void MainWindow::on_btnArchive_clicked() {
    if (patientMeasureRunning) {
        QMessageBox::warning(this, "检测进行中", "检测进行中不能编辑或删除档案。");
        return;
    }
    refreshTable(patientList);
    ui->stackedWidget->setCurrentWidget(ui->pageArchive);
    ui->editSearchKeyword->setFocus(Qt::OtherFocusReason);
}

void MainWindow::on_btnBackFromArchive_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->pageMain);
    scheduleResponsiveLayout();
}

void MainWindow::loadPatients() {
    patientDataWritable = false;
    patientDataLoadError.clear();
    QString errorMessage;
    if (!patientStore.load(xmlFilePath, measurementsFilePath,
                           &patientList, &measurementList, &errorMessage)) {
        patientDataLoadError = errorMessage;
        QMessageBox::warning(
            this,
            "档案加载失败，已进入只读保护",
            errorMessage + "\n\n为避免覆盖原档案，本次运行已禁止新增、修改、删除和检测保存。"
                           "请保留提示中的异常副本，修复数据文件后重新启动软件。");
        updatePatientSelectionUi();
        return;
    }
    patientDataWritable = true;
    updatePatientSelectionUi();
}

QList<MeasurementRecord> MainWindow::measurementsForPatient(const QString& patientId) const
{
    QList<MeasurementRecord> records;
    for (const MeasurementRecord& record : measurementList) {
        if (record.patientId == patientId) records.append(record);
    }
    return records;
}

const MeasurementRecord* MainWindow::latestMeasurementForPatient(const QString& patientId) const
{
    const MeasurementRecord* latest = nullptr;
    for (const MeasurementRecord& record : measurementList) {
        if (record.patientId == patientId &&
            (!latest || record.measuredAt > latest->measuredAt)) {
            latest = &record;
        }
    }
    return latest;
}

bool MainWindow::ensurePatientDataWritable()
{
    if (QFileInfo::exists(xmlFilePath + QStringLiteral(".txn"))) {
        patientDataWritable = false;
        patientDataLoadError = QStringLiteral(
            "检测到未完成的档案保存事务。为避免覆盖可恢复数据，本次运行已进入只读保护。"
            "请关闭并重新启动软件，让程序先完成档案恢复。");
        updatePatientSelectionUi();
    }
    if (patientDataWritable) return true;

    const QString detail = patientDataLoadError.trimmed().isEmpty()
        ? QStringLiteral("档案加载或恢复未成功。")
        : patientDataLoadError;
    QMessageBox::warning(this, "档案处于只读保护",
                         detail + QStringLiteral("\n本次运行禁止新增、修改、删除和检测保存。"));
    return false;
}

bool MainWindow::savePatients(const QList<PatientInfo>& patients)
{
    if (!ensurePatientDataWritable()) return false;
    QString errorMessage;
    if (!patientStore.savePatients(xmlFilePath, patients, &errorMessage)) {
        QMessageBox::warning(this, "档案保存失败", errorMessage);
        return false;
    }
    return true;
}

bool MainWindow::saveMeasurements(const QList<MeasurementRecord>& measurements)
{
    if (!ensurePatientDataWritable()) return false;
    QString errorMessage;
    if (!patientStore.saveMeasurements(measurementsFilePath, measurements, &errorMessage)) {
        QMessageBox::warning(this, "结果保存失败", errorMessage);
        return false;
    }
    return true;
}

bool MainWindow::savePatientData(const QList<PatientInfo>& patients,
                                 const QList<MeasurementRecord>& measurements)
{
    if (!ensurePatientDataWritable()) return false;
    QString errorMessage;
    if (!patientStore.savePatientData(xmlFilePath, measurementsFilePath,
                                      patients, measurements, &errorMessage)) {
        if (QFileInfo::exists(xmlFilePath + QStringLiteral(".txn"))) {
            patientDataWritable = false;
            patientDataLoadError = QStringLiteral(
                "档案保存事务未能完成。为避免覆盖可恢复数据，本次运行已进入只读保护。"
                "请关闭并重新启动软件完成恢复。");
            updatePatientSelectionUi();
            errorMessage += QStringLiteral("\n\n") + patientDataLoadError;
        }
        QMessageBox::critical(this, "档案保存失败", errorMessage);
        return false;
    }
    return true;
}

bool MainWindow::hasIncompletePatientRounds() const
{
    return patientMeasureRunning
        || !currentRoundSosList.isEmpty()
        || (!roundSosList.isEmpty() && roundSosList.size() < normalMeasureRounds);
}

bool MainWindow::confirmPatientChange(const QString& targetPatientId)
{
    if (!currentPatient.id.isEmpty() && currentPatient.id == targetPatientId) {
        return true;
    }

    if (hasPendingMeasurement) {
        return QMessageBox::question(
                   this,
                   QStringLiteral("放弃未保存结果"),
                   QStringLiteral("当前结果尚未保存，是否放弃并更换被测者？"))
            == QMessageBox::Yes;
    }

    if (hasIncompletePatientRounds()) {
        return QMessageBox::question(
                   this,
                   QStringLiteral("放弃未完成检测"),
                   QStringLiteral("当前被测者已完成 %1/%2 次测量。更换被测者会清空本组进度，"
                                  "是否仍要更换？")
                       .arg(roundSosList.size())
                       .arg(normalMeasureRounds))
            == QMessageBox::Yes;
    }
    return true;
}

void MainWindow::applyCurrentPatient(const PatientInfo& patient)
{
    currentPatient = patient;
    hasPendingMeasurement = false;
    pendingMeasurement = MeasurementRecord();
    pendingStartAfterPatientInfo = false;
    resetAllPatientMeasurementData();
    updateCurrentPatientUI();
    updatePatientSelectionUi();
    updateAgeSosReference();
    updateResultPanel();
}

bool MainWindow::selectCurrentPatient(const PatientInfo& patient)
{
    if (patientMeasureRunning) return false;
    if (!currentPatient.id.isEmpty() && currentPatient.id == patient.id) {
        currentPatient = patient;
        updateCurrentPatientUI();
        updatePatientSelectionUi();
        updateAgeSosReference();
        updateResultPanel();
        return true;
    }
    if (!confirmPatientChange(patient.id)) return false;
    applyCurrentPatient(patient);
    return true;
}

void MainWindow::clearCurrentPatient()
{
    currentPatient = PatientInfo();
    hasPendingMeasurement = false;
    pendingMeasurement = MeasurementRecord();
    pendingStartAfterPatientInfo = false;
    resetAllPatientMeasurementData();
    updateCurrentPatientUI();
    updatePatientSelectionUi();
    updateAgeSosReference();
    updateResultPanel();
}

void MainWindow::updateAgeSosReference()
{
    if (!ui || !ui->chartViewReference) return;
    if (!hasCurrentPatient()) {
        ui->chartViewReference->clearReferenceData();
        return;
    }

    const MeasurementRecord* latest = nullptr;
    QDateTime latestTime;
    for (const MeasurementRecord& record : measurementList) {
        if (record.patientId != currentPatient.id) continue;
        const int age = measurementAge(record, currentPatient);
        const QString gender = measurementGender(record, currentPatient);
        bool sosOk = false;
        const double sos = record.sos.trimmed().toDouble(&sosOk);
        if (!sosOk || !AgeSosChartWidget::supportsPoint(gender, age, sos)) continue;
        const QDateTime time = parsedMeasurementDateTime(record.measuredAt);
        if (!latest || (time.isValid() && (!latestTime.isValid() || time > latestTime)) ||
            (!time.isValid() && !latestTime.isValid() && record.measuredAt > latest->measuredAt)) {
            latest = &record;
            latestTime = time;
        }
    }
    if (!latest) {
        const int age = ageOnDate(currentPatient.birthDay, QDate::currentDate());
        AgeSosChartData data;
        data.hasPatient = true;
        data.hasMeasurementRecords = !measurementsForPatient(currentPatient.id).isEmpty();
        data.gender = currentPatient.gender;
        data.focalAge = age;
        ui->chartViewReference->setChartData(data);
        return;
    }
    ui->chartViewReference->setChartData(
        buildAgeSosChartData(currentPatient, *latest, false));
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

void MainWindow::showPatientHistory(const QString& patientId)
{
    QDialog dialog(this);
    dialog.setObjectName(QStringLiteral("historyDialog"));
    PatientInfo historyPatient;
    for (const PatientInfo& patient : patientList) {
        if (patient.id == patientId) historyPatient = patient;
    }
    const QString patientName = historyPatient.name;
    dialog.setWindowTitle(QStringLiteral("检测历史 · %1").arg(patientName));
    dialog.resize(900, 460);
    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(12);
    auto* title = new QLabel(dialog.windowTitle(), &dialog);
    title->setProperty("role", QStringLiteral("dialogTitle"));
    layout->addWidget(title);

    auto* table = new QTableWidget(&dialog);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->verticalHeader()->hide();
    table->setShowGrid(false);
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({QStringLiteral("检测时间"), QStringLiteral("SOS (m/s)"),
                                      QStringLiteral("T 值"), QStringLiteral("Z 值"),
                                      QStringLiteral("骨强度"), QStringLiteral("操作账号")});
    QList<MeasurementRecord> records = measurementsForPatient(patientId);
    std::sort(records.begin(), records.end(), [](const MeasurementRecord& a, const MeasurementRecord& b) {
        return a.measuredAt > b.measuredAt;
    });
    table->setRowCount(records.size());
    for (int row = 0; row < records.size(); ++row) {
        const MeasurementRecord record = withCurrentReference(records[row], historyPatient);
        const QDateTime measuredAt = parsedMeasurementDateTime(record.measuredAt);
        auto* timeItem = new QTableWidgetItem(measuredAt.isValid()
            ? measuredAt.toString(QStringLiteral("yyyy-MM-dd HH:mm")) : record.measuredAt);
        timeItem->setData(Qt::UserRole, record.id);
        table->setItem(row, 0, timeItem);
        table->setItem(row, 1, new QTableWidgetItem(record.sos));
        table->setItem(row, 2, new QTableWidgetItem(record.tScore));
        table->setItem(row, 3, new QTableWidgetItem(record.zScore));
        table->setItem(row, 4, new QTableWidgetItem(record.boneStrength.isEmpty()
                                                        ? record.diagnosis : record.boneStrength));
        table->setItem(row, 5, new QTableWidgetItem(record.operatorName));
    }
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if (table->rowCount() > 0) table->selectRow(0);
    layout->addWidget(table, 1);

    auto* buttons = new QHBoxLayout();
    auto* reportButton = new QPushButton(QStringLiteral("查看报表"), &dialog);
    reportButton->setProperty("variant", QStringLiteral("primary"));
    auto* deleteButton = new QPushButton(QStringLiteral("删除本条记录"), &dialog);
    deleteButton->setProperty("variant", QStringLiteral("dangerOutline"));
    deleteButton->setEnabled(patientDataWritable);
    auto* closeButton = new QPushButton(QStringLiteral("关闭"), &dialog);
    buttons->addWidget(reportButton);
    buttons->addWidget(deleteButton);
    buttons->addStretch();
    buttons->addWidget(closeButton);
    layout->addLayout(buttons);
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    auto openSelectedReport = [this, table, patientId, &dialog]() {
        const int row = table->currentRow();
        if (row < 0 || !table->item(row, 0)) {
            QMessageBox::information(&dialog, "提示", "请先选择一条检测记录。");
            return;
        }
        const QString recordId = table->item(row, 0)->data(Qt::UserRole).toString();
        const PatientInfo* patient = nullptr;
        const MeasurementRecord* measurement = nullptr;
        for (const PatientInfo& item : patientList) {
            if (item.id == patientId) {
                patient = &item;
                break;
            }
        }
        for (const MeasurementRecord& item : measurementList) {
            if (item.id == recordId) {
                measurement = &item;
                break;
            }
        }
        if (!patient || !measurement) {
            QMessageBox::warning(&dialog, "报表", "无法找到这条检测记录对应的报表数据。");
            return;
        }
        const PatientInfo patientCopy = *patient;
        const MeasurementRecord measurementCopy = *measurement;
        dialog.accept();
        showReportFrom(ui->pageArchive, patientCopy, measurementCopy);
    };
    connect(reportButton, &QPushButton::clicked, &dialog, openSelectedReport);
    connect(table, &QTableWidget::cellDoubleClicked, &dialog,
            [openSelectedReport](int, int) { openSelectedReport(); });
    connect(deleteButton, &QPushButton::clicked, &dialog, [this, table, &dialog]() {
        const int row = table->currentRow();
        if (row < 0 || !table->item(row, 0)) return;
        const QString recordId = table->item(row, 0)->data(Qt::UserRole).toString();
        if (QMessageBox::question(&dialog, "确认删除", "确定删除这条检测记录？删除后无法恢复。")
            != QMessageBox::Yes) {
            return;
        }
        if (deleteMeasurementRecord(recordId)) table->removeRow(row);
    });
    dialog.exec();
}

QString MainWindow::selectedArchivePatientId() const
{
    const int row = ui->table->currentRow();
    if (row < 0 || !ui->table->item(row, ArchiveIdColumn)) return QString();
    if (!ui->table->selectionModel() || !ui->table->selectionModel()->isRowSelected(row, QModelIndex()))
        return QString();
    return ui->table->item(row, ArchiveIdColumn)->text();
}

void MainWindow::on_btnSelectPatient_clicked()
{
    if (patientMeasureRunning) return;
    const QString id = selectedArchivePatientId();
    if (id.isEmpty()) {
        QMessageBox::information(this, "提示", "请先在表格中选中一位被测者。");
        return;
    }
    for (const PatientInfo& patient : patientList) {
        if (patient.id == id) {
            if (!selectCurrentPatient(patient)) return;
            ui->stackedWidget->setCurrentWidget(ui->pageMain);
            scheduleResponsiveLayout();
            return;
        }
    }
}

void MainWindow::on_btnViewHistory_clicked()
{
    if (patientMeasureRunning) return;
    const QString id = selectedArchivePatientId();
    if (id.isEmpty()) {
        QMessageBox::information(this, "提示", "请先在表格中选中一位被测者。");
        return;
    }
    showPatientHistory(id);
}

void MainWindow::refreshTable(const QList<PatientInfo> &list) {
    const QString previouslySelected = ui->table->currentRow() >= 0 && ui->table->item(ui->table->currentRow(), ArchiveIdColumn)
        ? ui->table->item(ui->table->currentRow(), ArchiveIdColumn)->text() : QString();
    QSignalBlocker blocker(ui->table);
    ui->table->clear();
    ui->table->setColumnCount(ArchiveColumnCount);
    ui->table->setHorizontalHeaderLabels({QStringLiteral("编号"), QStringLiteral("姓名"),
                                          QStringLiteral("性别"), QStringLiteral("出生日期（年龄）"),
                                          QStringLiteral("最近检测"), QStringLiteral("最近 SOS"),
                                          QStringLiteral("次数")});
    ui->table->setRowCount(list.size());
    int rowToSelect = -1;
    for (int i = 0; i < list.size(); ++i) {
        const PatientInfo &p = list[i];
        auto* itemID = new QTableWidgetItem(p.id);
        itemID->setCheckState(Qt::Unchecked);
        itemID->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->table->setItem(i, ArchiveIdColumn, itemID);

        const int age = ageOnDate(p.birthDay, QDate::currentDate());
        const QString birth = p.birthDay.isEmpty() ? QStringLiteral("--")
            : age >= 0 ? QStringLiteral("%1（%2）").arg(p.birthDay).arg(age) : p.birthDay;
        const MeasurementRecord* latest = latestMeasurementForPatient(p.id);
        const QDateTime latestTime = latest ? parsedMeasurementDateTime(latest->measuredAt) : QDateTime();
        const QString date = !latest ? QStringLiteral("--")
            : latestTime.isValid() ? latestTime.date().toString(QStringLiteral("yyyy-MM-dd"))
                                   : latest->measuredAt.left(10);
        const QStringList values = {p.name, p.gender.isEmpty() ? QStringLiteral("--") : p.gender, birth, date,
                                    latest ? latest->sos : QStringLiteral("--"),
                                    QString::number(measurementsForPatient(p.id).size())};
        for (int column = ArchiveNameColumn; column < ArchiveColumnCount; ++column) {
            auto* item = new QTableWidgetItem(values.at(column - ArchiveNameColumn));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            ui->table->setItem(i, column, item);
        }
        if (p.id == previouslySelected) rowToSelect = i;
    }

    QHeaderView* header = ui->table->horizontalHeader();
    header->setStretchLastSection(false);
    header->setSectionResizeMode(QHeaderView::Stretch);
    for (int column : {ArchiveIdColumn, ArchiveGenderColumn, ArchiveSosColumn, ArchiveCountColumn})
        header->setSectionResizeMode(column, QHeaderView::ResizeToContents);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    if (rowToSelect >= 0) ui->table->selectRow(rowToSelect);
    blocker.unblock();
    updateArchiveSelectionBar();
    updateAgeSosReference();
}

void MainWindow::on_btnShowAll_clicked() {
    ui->editSearchKeyword->clear();
    if (chkDateFilter) chkDateFilter->setChecked(false);
    refreshTable(patientList);
}

// 姓名/编号关键字，可选叠加检测日期
void MainWindow::on_btnSearchName_clicked() {
    const QString key = ui->editSearchKeyword->text().trimmed();
    const bool byDate = chkDateFilter && chkDateFilter->isChecked();
    const QString date = byDate ? dateFilter->date().toString(QStringLiteral("yyyy-MM-dd")) : QString();
    QList<PatientInfo> result;
    for (const PatientInfo& p : patientList) {
        if (!key.isEmpty() && !p.name.contains(key, Qt::CaseInsensitive) &&
            !p.id.contains(key, Qt::CaseInsensitive)) {
            continue;
        }
        if (byDate) {
            bool measuredThatDay = false;
            for (const MeasurementRecord& record : measurementsForPatient(p.id)) {
                const QDateTime time = parsedMeasurementDateTime(record.measuredAt);
                const QString day = time.isValid() ? time.date().toString(QStringLiteral("yyyy-MM-dd"))
                                                   : record.measuredAt.left(10);
                if (day == date) {
                    measuredThatDay = true;
                    break;
                }
            }
            if (!measuredThatDay) continue;
        }
        result << p;
    }
    refreshTable(result);
    if (result.isEmpty() && (!key.isEmpty() || byDate)) {
        statusBar()->showMessage(QStringLiteral("没有找到符合条件的档案。"), 5000);
    }
}

void MainWindow::on_btnDeleteSelected_clicked()
{
    if (patientMeasureRunning) {
        QMessageBox::warning(this, "检测进行中", "检测进行中不能删除档案。");
        return;
    }
    QStringList orderedIds;
    QSet<QString> patientIds;
    for (int i = 0; i < ui->table->rowCount(); ++i) {
        QTableWidgetItem *item = ui->table->item(i, ArchiveIdColumn);
        if (item && item->checkState() == Qt::Checked && !patientIds.contains(item->text())) {
            patientIds.insert(item->text());
            orderedIds << item->text();
        }
    }

    if (patientIds.isEmpty()) {
        QMessageBox::information(this, "提示", "请先勾选需要删除的档案。");
        return;
    }
    if (hasPendingMeasurement && patientIds.contains(pendingMeasurement.patientId)) {
        QMessageBox::warning(this, "存在未保存结果",
                             "当前被测者还有未保存的检测结果，请先重试保存后再删除档案。");
        return;
    }

    QStringList summaries;
    for (const QString& id : orderedIds) {
        QString name = id;
        for (const PatientInfo& patient : patientList) {
            if (patient.id == id) name = patient.name;
        }
        summaries << QStringLiteral("%1（%2 条检测记录）").arg(name).arg(measurementsForPatient(id).size());
    }
    QMessageBox box(QMessageBox::Warning, QStringLiteral("确认删除"),
                    QStringLiteral("确定删除选中的 %1 份档案吗？").arg(orderedIds.size()),
                    QMessageBox::Yes | QMessageBox::No, this);
    box.setInformativeText(summaries.join(QStringLiteral("、")) +
                           QStringLiteral("\n这些人的全部检测记录会一起删除。"
                                          "删除前会自动在软件文件夹的 backups 里留一份备份。"));
    box.setDefaultButton(QMessageBox::No);
    box.button(QMessageBox::Yes)->setText(QStringLiteral("删除"));
    box.button(QMessageBox::No)->setText(QStringLiteral("取消"));
    if (box.exec() != QMessageBox::Yes) return;

    QList<PatientInfo> patientCandidate;
    for (const PatientInfo& patient : patientList) {
        if (!patientIds.contains(patient.id)) patientCandidate.append(patient);
    }
    QList<MeasurementRecord> measurementCandidate;
    for (const MeasurementRecord& record : measurementList) {
        if (!patientIds.contains(record.patientId)) measurementCandidate.append(record);
    }
    if (!backupBeforeDelete()) return;
    if (!savePatientData(patientCandidate, measurementCandidate)) return;

    const bool removedCurrent = patientIds.contains(currentPatient.id);
    patientList = patientCandidate;
    measurementList = measurementCandidate;
    if (removedCurrent) clearCurrentPatient();
    refreshPatientDerivedViews();
    statusBar()->showMessage(QStringLiteral("已删除 %1 份档案。").arg(patientIds.size()), 5000);
}

void MainWindow::on_btnAdd_clicked() {
    if (patientMeasureRunning) {
        QMessageBox::warning(this, "检测进行中", "检测进行中不能新建档案。");
        return;
    }
    openNewPatientDialog(false);
}

void MainWindow::on_table_cellDoubleClicked(int row, int)
{
    if (patientMeasureRunning) {
        QMessageBox::warning(this, "检测进行中", "检测进行中不能更换被测者。");
        return;
    }
    if (row < 0 || row >= ui->table->rowCount()) return;
    QTableWidgetItem* idItem = ui->table->item(row, ArchiveIdColumn);
    if (!idItem) return;
    for (const PatientInfo& patient : patientList) {
        if (patient.id == idItem->text()) {
            if (!selectCurrentPatient(patient)) return;
            ui->stackedWidget->setCurrentWidget(ui->pageMain);
            scheduleResponsiveLayout();
            return;
        }
    }
    QMessageBox::warning(this, "错误", "未找到该行对应的档案。");
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
    const int processMinimum = 250;
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

void MainWindow::updateArchiveSelectionBar()
{
    if (!lblArchiveSelection || !lblCheckedCount) return;
    const QString id = selectedArchivePatientId();
    QString name;
    for (const PatientInfo& patient : patientList) {
        if (patient.id == id) name = patient.name;
    }
    static_cast<ElidingLabel*>(lblArchiveSelection)->setFullText(id.isEmpty() ? QStringLiteral("未选择")
                                              : QStringLiteral("%1（%2）").arg(name, id));
    int checked = 0;
    for (int row = 0; row < ui->table->rowCount(); ++row) {
        if (ui->table->item(row, ArchiveIdColumn) &&
            ui->table->item(row, ArchiveIdColumn)->checkState() == Qt::Checked) {
            ++checked;
        }
    }
    lblCheckedCount->setText(QStringLiteral("已勾选 %1 人").arg(checked));
    if (btnExport) btnExport->setText(checked > 0 ? QStringLiteral("导出勾选") : QStringLiteral("导出全部"));
    const bool nextRoundPending = nextRoundTimer.isActive();
    const bool rowActionsAllowed = !id.isEmpty() && !patientMeasureRunning;
    ui->btnSelectPatient->setEnabled(rowActionsAllowed && !nextRoundPending);
    ui->btnViewHistory->setEnabled(rowActionsAllowed);
    if (btnEditPatient) btnEditPatient->setEnabled(rowActionsAllowed && patientDataWritable && !nextRoundPending);
    ui->btnDeleteSelected->setEnabled(checked > 0 && patientDataWritable && !nextRoundPending &&
                                      !patientMeasureRunning);
}

void MainWindow::openNewPatientDialog(bool makeCurrent)
{
    if (patientMeasureRunning) return;
    if (!ensurePatientDataWritable()) return;
    PatientFormDialog dialog(PatientFormDialog::Mode::Create, this);
    dialog.setSuggestedId(PatientFormDialog::suggestId(patientList, QDate::currentDate()));
    QSet<QString> ids;
    for (const PatientInfo& patient : patientList) ids.insert(patient.id);
    dialog.setExistingIds(ids);
    if (makeCurrent) dialog.setSaveButtonText(QStringLiteral("保存并设为当前被测者"));
    if (dialog.exec() != QDialog::Accepted) return;
    const PatientInfo patient = dialog.patient();
    if (!createPatient(patient, makeCurrent)) return;
    if (makeCurrent) {
        ui->stackedWidget->setCurrentWidget(ui->pageMain);
        scheduleResponsiveLayout();
        return;
    }
    for (int row = 0; row < ui->table->rowCount(); ++row) {
        if (ui->table->item(row, ArchiveIdColumn) &&
            ui->table->item(row, ArchiveIdColumn)->text() == patient.id) {
            ui->table->selectRow(row);
            break;
        }
    }
}

void MainWindow::openEditPatientDialog(const QString& patientId)
{
    if (patientMeasureRunning) return;
    for (const PatientInfo& existing : patientList) {
        if (existing.id != patientId) continue;
        PatientFormDialog dialog(PatientFormDialog::Mode::Edit, this);
        dialog.setPatient(existing);
        QSet<QString> ids;
        for (const PatientInfo& patient : patientList) ids.insert(patient.id);
        dialog.setExistingIds(ids);
        if (dialog.exec() == QDialog::Accepted) updatePatient(dialog.patient());
        return;
    }
}

bool MainWindow::createPatient(const PatientInfo& patient, bool makeCurrent)
{
    if (patientMeasureRunning) return false;
    if (patient.name.trimmed().isEmpty() || patient.id.trimmed().isEmpty()) {
        QMessageBox::warning(this, "缺少信息", "姓名和编号不能为空。");
        return false;
    }
    for (const PatientInfo& existing : patientList) {
        if (existing.id == patient.id) {
            QMessageBox::warning(this, "重复编号", "该编号已存在，请从档案中选择被测者。");
            return false;
        }
    }
    if (makeCurrent && !confirmPatientChange(patient.id)) return false;

    QList<PatientInfo> candidate = patientList;
    candidate.append(patient);
    if (!savePatients(candidate)) return false;
    patientList = candidate;
    if (makeCurrent) applyCurrentPatient(patient);
    refreshPatientDerivedViews();
    return true;
}

bool MainWindow::updatePatient(const PatientInfo& patient)
{
    if (patientMeasureRunning) {
        QMessageBox::warning(this, "检测进行中", "检测进行中不能编辑档案。");
        return false;
    }
    int index = -1;
    for (int i = 0; i < patientList.size(); ++i) {
        if (patientList[i].id == patient.id) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        QMessageBox::warning(this, "错误", "没有找到这份档案。");
        return false;
    }
    if (patient.name.trimmed().isEmpty()) {
        QMessageBox::warning(this, "缺少信息", "姓名不能为空。");
        return false;
    }
    QList<PatientInfo> candidate = patientList;
    candidate[index] = patient;
    if (!savePatients(candidate)) return false;
    patientList = candidate;
    if (currentPatient.id == patient.id) {
        // Only the editable archive fields; session-only fields stay untouched.
        currentPatient.name = patient.name;
        currentPatient.gender = patient.gender;
        currentPatient.birthDay = patient.birthDay;
        currentPatient.height = patient.height;
        currentPatient.weight = patient.weight;
    }
    refreshPatientDerivedViews();
    statusBar()->showMessage(QStringLiteral("档案已保存。"), 4000);
    return true;
}

bool MainWindow::deleteMeasurementRecord(const QString& recordId)
{
    for (int i = 0; i < measurementList.size(); ++i) {
        if (measurementList[i].id != recordId) continue;
        QList<MeasurementRecord> candidate = measurementList;
        candidate.removeAt(i);
        if (!backupBeforeDelete()) return false;
        if (!saveMeasurements(candidate)) return false;
        measurementList = candidate;
        refreshPatientDerivedViews();
        return true;
    }
    return false;
}

void MainWindow::showReportFrom(QWidget* returnPage, const PatientInfo& patient,
                                const MeasurementRecord& measurement)
{
    reportReturnPage = returnPage;
    showReport(patient, measurement);
}

// ==================== 导出检测记录 ====================

namespace {

QString csvField(const QString& value)
{
    if (!value.contains(QLatin1Char(',')) && !value.contains(QLatin1Char('"')) &&
        !value.contains(QLatin1Char('\n')) && !value.contains(QLatin1Char('\r'))) {
        return value;
    }
    QString quoted = value;
    quoted.replace(QLatin1Char('"'), QStringLiteral("\"\""));
    return QLatin1Char('"') + quoted + QLatin1Char('"');
}

} // namespace

QString MainWindow::measurementsCsv(const QList<PatientInfo>& patients,
                                    const QList<MeasurementRecord>& measurements)
{
    const QStringList header = {
        QStringLiteral("档案编号"), QStringLiteral("姓名"), QStringLiteral("性别"),
        QStringLiteral("出生日期"), QStringLiteral("检测时间"), QStringLiteral("检测时年龄"),
        QStringLiteral("部位"), QStringLiteral("SOS(m/s)"), QStringLiteral("T值"),
        QStringLiteral("Z值"), QStringLiteral("骨强度"), QStringLiteral("相对骨折风险"),
        QStringLiteral("相对骨龄"), QStringLiteral("诊断提示"), QStringLiteral("操作账号"),
        QStringLiteral("身高(cm)"), QStringLiteral("体重(kg)")};
    const QString lineEnd = QStringLiteral("\r\n");
    QString csv = header.join(QLatin1Char(',')) + lineEnd;

    QList<MeasurementRecord> rows;
    for (const MeasurementRecord& record : measurements) {
        for (const PatientInfo& patient : patients) {
            if (patient.id == record.patientId) {
                rows.append(record);
                break;
            }
        }
    }
    std::stable_sort(rows.begin(), rows.end(), [](const MeasurementRecord& a, const MeasurementRecord& b) {
        return a.patientId != b.patientId ? a.patientId < b.patientId : a.measuredAt < b.measuredAt;
    });

    const auto snapshotOr = [](const QString& snapshot, const QString& current) {
        return snapshot.trimmed().isEmpty() ? current : snapshot;
    };
    for (const MeasurementRecord& saved : rows) {
        PatientInfo patient;
        for (const PatientInfo& candidate : patients) {
            if (candidate.id == saved.patientId) patient = candidate;
        }
        const MeasurementRecord record = withCurrentReference(saved, patient);
        const QDateTime time = parsedMeasurementDateTime(record.measuredAt);
        const int age = measurementAge(record, patient);
        const QStringList fields = {
            record.patientId,
            snapshotOr(record.patientName, patient.name),
            measurementGender(record, patient),
            snapshotOr(record.patientBirthDay, patient.birthDay),
            time.isValid() ? time.toString(QStringLiteral("yyyy-MM-dd HH:mm")) : record.measuredAt,
            age >= 0 ? QString::number(age) : QString(),
            record.part,
            record.sos,
            record.tScore,
            record.zScore,
            record.boneStrength,
            record.fractureRisk,
            record.boneAge,
            record.diagnosis == record.boneStrength ? QString() : record.diagnosis,
            record.operatorName,
            snapshotOr(record.patientHeight, patient.height),
            snapshotOr(record.patientWeight, patient.weight)};
        QStringList escaped;
        for (const QString& field : fields) escaped << csvField(field.trimmed());
        csv += escaped.join(QLatin1Char(',')) + lineEnd;
    }
    return csv;
}

QStringList MainWindow::checkedArchivePatientIds() const
{
    QStringList ids;
    for (int row = 0; row < ui->table->rowCount(); ++row) {
        const QTableWidgetItem* item = ui->table->item(row, ArchiveIdColumn);
        if (item && item->checkState() == Qt::Checked && !ids.contains(item->text())) ids << item->text();
    }
    return ids;
}

void MainWindow::exportMeasurements()
{
    const QStringList checkedIds = checkedArchivePatientIds();
    QList<PatientInfo> patients;
    for (const PatientInfo& patient : patientList) {
        if (checkedIds.isEmpty() || checkedIds.contains(patient.id)) patients.append(patient);
    }
    int recordCount = 0;
    for (const MeasurementRecord& record : measurementList) {
        for (const PatientInfo& patient : patients) {
            if (patient.id == record.patientId) {
                ++recordCount;
                break;
            }
        }
    }
    if (recordCount == 0) {
        QMessageBox::information(this, QStringLiteral("导出数据"),
                                 checkedIds.isEmpty() ? QStringLiteral("还没有任何检测记录。")
                                                      : QStringLiteral("勾选的档案还没有检测记录。"));
        return;
    }

    const QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString defaultPath = QDir(documents).filePath(
        QStringLiteral("骨密度检测记录_%1.csv").arg(QDate::currentDate().toString(QStringLiteral("yyyyMMdd"))));
    const QString path = QFileDialog::getSaveFileName(
        this, checkedIds.isEmpty() ? QStringLiteral("导出全部检测记录") : QStringLiteral("导出勾选档案的检测记录"),
        defaultPath, QStringLiteral("CSV 表格 (*.csv)"));
    if (path.isEmpty()) return;

    QSaveFile file(path);
    QByteArray bytes("\xEF\xBB\xBF");   // UTF-8 BOM so Excel shows Chinese correctly
    bytes += measurementsCsv(patients, measurementList).toUtf8();
    if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) {
        QMessageBox::warning(this, QStringLiteral("导出失败"),
                             QStringLiteral("无法写入文件，请换一个保存位置后重试。"));
        return;
    }
    statusBar()->showMessage(QStringLiteral("已导出 %1 人、%2 条检测记录。").arg(patients.size()).arg(recordCount), 6000);
}

// ==================== 账号菜单 ====================

void MainWindow::updateAccountUi()
{
    if (!btnAccount) return;
    const bool loggedIn = !currentAccount.username.isEmpty();
    btnAccount->setText(loggedIn ? QStringLiteral("账号 %1").arg(currentAccount.username)
                                 : QStringLiteral("未登录"));
    if (actManageAccounts) actManageAccounts->setVisible(currentAccount.role == QStringLiteral("admin"));
}

void MainWindow::openDataFolder()
{
    const QString folder = QFileInfo(xmlFilePath).absolutePath();
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(folder))) {
        QMessageBox::information(this, QStringLiteral("数据文件夹"),
                                 QStringLiteral("数据保存在：\n%1").arg(QDir::toNativeSeparators(folder)));
    }
}

void MainWindow::switchAccount()
{
    // The operator is written into every result, so a running or unsaved
    // measurement has to be finished under the account that started it.
    if (patientMeasureRunning || hasIncompletePatientRounds() || nextRoundTimer.isActive()) {
        QMessageBox::information(this, QStringLiteral("检测进行中"),
                                 QStringLiteral("请先完成或停止当前检测，再切换账号。"));
        return;
    }
    if (hasPendingMeasurement) {
        QMessageBox::warning(this, QStringLiteral("检测结果尚未保存"),
                             QStringLiteral("本次检测结果尚未保存，请先保存后再切换账号。"));
        return;
    }
    if (autoRunning) on_triggerButton_clicked();   // stop debug auto acquisition

    // The next operator starts without the previous person's selection.
    clearCurrentPatient();
    currentAccount = AccountInfo();
    updateAccountUi();
    ui->editUsername->clear();
    ui->editPassword->clear();
    ui->lblLoginMsg->clear();
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    ui->editUsername->setFocus();
}

// ==================== 数据备份 ====================

QString MainWindow::backupRoot() const
{
    return QFileInfo(xmlFilePath).absoluteDir().filePath(QStringLiteral("backups"));
}

QStringList MainWindow::dataFilePaths() const
{
    return {xmlFilePath, measurementsFilePath, accountsFilePath, calibrationFilePath};
}

void MainWindow::backupDataDaily()
{
    const DataBackup::Result result = DataBackup::snapshot(
        backupRoot(), dataFilePaths(), DataBackup::dailyLabel(QDate::currentDate()));
    if (!result.ok) {
        statusBar()->showMessage(
            QStringLiteral("今日自动备份失败：%1。数据仍可正常使用。").arg(result.error), 10000);
        return;
    }
    if (result.created) DataBackup::prune(backupRoot());
}

bool MainWindow::backupBeforeDelete()
{
    const DataBackup::Result result = DataBackup::snapshot(
        backupRoot(), dataFilePaths(), DataBackup::beforeDeleteLabel(QDateTime::currentDateTime()));
    if (result.ok) {
        DataBackup::prune(backupRoot());
        return true;
    }
    return QMessageBox::warning(
               this, QStringLiteral("删除前备份失败"),
               QStringLiteral("%1。\n\n仍要删除吗？删除后将无法从备份恢复。").arg(result.error),
               QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
        == QMessageBox::Yes;
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
