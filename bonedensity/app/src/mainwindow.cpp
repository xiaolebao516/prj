// 主窗口：构造与析构、登录、关闭保护、账号菜单

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
#include "uikit.h"

using namespace mainwindow_detail;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this))
{
    ui->setupUi(this);
    accountsFilePath = DataLocation::filePath("accounts.xml");
    deviceSettingsPath = DataLocation::settingsFile("device.ini");
    measurementGuideSettingsPath = DataLocation::settingsFile("measurement-guide.ini");
    QString accountError;
    if (!accountStore.loadOrInitialize(accountsFilePath, &accountError)) {
        QMessageBox::warning(this, "账号初始化失败", accountError);
    }
    calibrationFilePath = DataLocation::filePath("calibration.xml");
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
    xmlFilePath = DataLocation::filePath("patients.xml");
    measurementsFilePath = DataLocation::filePath("measurements.xml");
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

// ==================== 账号菜单 ====================

void MainWindow::updateAccountUi()
{
    if (!btnAccount) return;
    const bool loggedIn = !currentAccount.username.isEmpty();
    btnAccount->setText(loggedIn ? currentAccount.username : QStringLiteral("未登录"));
    btnAccount->setIcon(QIcon(AvatarBadge::render(loggedIn ? currentAccount.username : QString(),
                                                  28, 2.0, false)));
    btnAccount->setToolTip(loggedIn ? QStringLiteral("当前账号：%1").arg(currentAccount.username) : QString());
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
