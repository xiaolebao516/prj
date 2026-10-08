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
