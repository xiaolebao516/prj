// Everyday workflow cases: account menu (switch operator, data folder).

namespace {

// Closes the next modal message box and remembers its text.
void dismissNextMessageBox(QString* text)
{
    QTimer::singleShot(0, [text]() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            if (text) *text = box->text();
            box->reject();
        }
    });
}

QAction* menuAction(QToolButton* button, const QString& text)
{
    if (!button || !button->menu()) return nullptr;
    for (QAction* action : button->menu()->actions()) {
        if (action->text() == text) return action;
    }
    return nullptr;
}

} // namespace

void MainWindowSafetyTests::accountMenuSwitchesOperatorSafely()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    PREPARE_ARCHIVE_WINDOW(window, directory);
    QVERIFY(window.btnAccount);
    QCOMPARE(window.btnAccount->text(), QStringLiteral("未登录"));
    QVERIFY(menuAction(window.btnAccount, QStringLiteral("打开数据文件夹")));
    QVERIFY(menuAction(window.btnAccount, QStringLiteral("切换账号")));

    window.currentAccount.username = QStringLiteral("admin");
    window.currentAccount.role = QStringLiteral("admin");
    window.updateAccountUi();
    QCOMPARE(window.btnAccount->text(), QStringLiteral("账号 admin"));
    QVERIFY(window.actManageAccounts->isVisible());
    window.currentAccount.username = QStringLiteral("zhang");
    window.currentAccount.role = QStringLiteral("user");
    window.updateAccountUi();
    QVERIFY(!window.actManageAccounts->isVisible());

    const PatientInfo patient = samplePatient();
    window.patientList = {patient};
    QVERIFY(window.savePatients(window.patientList));
    QVERIFY(window.selectCurrentPatient(patient));
    window.ui->stackedWidget->setCurrentWidget(window.ui->pageMain);

    // An unsaved result keeps the operator logged in.
    window.pendingMeasurement = sampleMeasurement();
    window.hasPendingMeasurement = true;
    QString message;
    dismissNextMessageBox(&message);
    window.switchAccount();
    QVERIFY(message.contains(QStringLiteral("尚未保存")));
    QCOMPARE(window.currentAccount.username, QStringLiteral("zhang"));
    QCOMPARE(window.ui->stackedWidget->currentWidget(), window.ui->pageMain);
    window.hasPendingMeasurement = false;
    window.pendingMeasurement = MeasurementRecord();

    // So does a measurement in progress.
    window.patientMeasureRunning = true;
    message.clear();
    dismissNextMessageBox(&message);
    window.switchAccount();
    QVERIFY(message.contains(QStringLiteral("切换账号")));
    QCOMPARE(window.currentAccount.username, QStringLiteral("zhang"));
    window.patientMeasureRunning = false;

    // Otherwise the next operator starts at the login page with no selection.
    window.ui->editUsername->setText(QStringLiteral("zhang"));
    window.switchAccount();
    QVERIFY(window.currentAccount.username.isEmpty());
    QVERIFY(window.currentPatient.id.isEmpty());
    QCOMPARE(window.ui->stackedWidget->currentWidget(), window.ui->pageLogin);
    QVERIFY(window.ui->editUsername->text().isEmpty());
    QCOMPARE(window.btnAccount->text(), QStringLiteral("未登录"));
    QCOMPARE(window.patientList.size(), 1);   // data is untouched
}

void MainWindowSafetyTests::serialPortSelectionIsRemembered()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    window.hide();
    window.deviceSettingsPath = directory.filePath(QStringLiteral("device.ini"));
    QVERIFY(window.rememberedPort().isEmpty());
    window.rememberPort(QStringLiteral("COM7"));
    QCOMPARE(window.rememberedPort(), QStringLiteral("COM7"));

    // At start-up nothing is selected yet, so the remembered port wins even
    // when it is not first in the list.
    window.ui->comboPort->clear();
    const QList<QPair<QString, QString>> ports = {
        {QStringLiteral("COM1"), QStringLiteral("通信端口")},
        {QStringLiteral("COM7"), QStringLiteral("USB 串行设备")}};
    window.applyPortList(ports, window.rememberedPort());
    QCOMPARE(window.ui->comboPort->currentData().toString(), QStringLiteral("COM7"));

    // A port the user picked is kept when another device is plugged in.
    window.ui->comboPort->setCurrentIndex(window.ui->comboPort->findData(QStringLiteral("COM1")));
    window.applyPortList({ports.at(0), ports.at(1),
                          {QStringLiteral("COM9"), QStringLiteral("USB 串行设备")}},
                         QStringLiteral("COM7"));
    QCOMPARE(window.ui->comboPort->count(), 3);
    QCOMPARE(window.ui->comboPort->currentData().toString(), QStringLiteral("COM1"));
}
