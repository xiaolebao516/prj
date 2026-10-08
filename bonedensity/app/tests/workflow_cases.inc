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

void MainWindowSafetyTests::measurementCsvMatchesScreenValues()
{
    PatientInfo first = samplePatient();                      // 女, born 1990-01-02
    first.name = QStringLiteral("张\"三,测试");                 // needs CSV quoting
    PatientInfo second = samplePatient();
    second.id = QStringLiteral("patient-002");
    second.name = QStringLiteral("李四");
    second.gender = QStringLiteral("男");
    PatientInfo unlisted = samplePatient();
    unlisted.id = QStringLiteral("patient-003");

    MeasurementRecord later = sampleMeasurement();             // 4000.0 on 2026-07-22
    later.id = QStringLiteral("m-2");
    MeasurementRecord earlier = sampleMeasurement();
    earlier.id = QStringLiteral("m-1");
    earlier.measuredAt = QStringLiteral("2025-03-12T09:00:00");
    earlier.sos = QStringLiteral("4031.5");
    earlier.tScore = QStringLiteral("-0.90");                  // stale stored value
    MeasurementRecord man = sampleMeasurement();
    man.id = QStringLiteral("m-3");
    man.patientId = second.id;
    man.sos = QStringLiteral("4100.0");
    MeasurementRecord hidden = sampleMeasurement();
    hidden.id = QStringLiteral("m-4");
    hidden.patientId = unlisted.id;

    const QString csv = MainWindow::measurementsCsv({first, second}, {later, man, earlier, hidden});
    QVERIFY(csv.endsWith(QStringLiteral("\r\n")));
    const QStringList lines = csv.split(QStringLiteral("\r\n"), Qt::SkipEmptyParts);
    QCOMPARE(lines.size(), 4);   // header + three rows; the unlisted patient is left out
    QVERIFY(lines.at(0).startsWith(QStringLiteral("档案编号,姓名,性别,出生日期,检测时间,检测时年龄")));

    // Sorted by patient, then time. Values are the ones shown on screen.
    const BoneHealth::DerivedResult earlierShown = BoneHealth::deriveResult(4031.5, QStringLiteral("女"), 35);
    QVERIFY(lines.at(1).startsWith(
        QStringLiteral("patient-001,\"张\"\"三,测试\",女,1990-01-02,2025-03-12 09:00,35,,4031.5,")));
    QVERIFY(lines.at(1).contains(QStringLiteral(",%1,%2,").arg(earlierShown.tScore, earlierShown.zScore)));
    QVERIFY(!lines.at(1).contains(QStringLiteral("-0.90")));
    QVERIFY(lines.at(2).startsWith(QStringLiteral("patient-001,")));
    QVERIFY(lines.at(2).contains(QStringLiteral(",2026-07-22 10:30,36,,4000.0,-1.23,")));
    const BoneHealth::DerivedResult manShown = BoneHealth::deriveResult(4100.0, QStringLiteral("男"), 36);
    QVERIFY(lines.at(3).startsWith(QStringLiteral("patient-002,李四,男,")));
    QVERIFY(lines.at(3).contains(QStringLiteral(",%1,%2,").arg(manShown.tScore, manShown.zScore)));

    // The button says what will be exported.
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    PREPARE_ARCHIVE_WINDOW(window, directory);
    window.patientList = {first, second};
    window.refreshTable(window.patientList);
    window.updateArchiveSelectionBar();
    QCOMPARE(window.btnExport->text(), QStringLiteral("导出全部"));
    window.ui->table->item(0, MainWindow::ArchiveIdColumn)->setCheckState(Qt::Checked);
    QCOMPARE(window.btnExport->text(), QStringLiteral("导出勾选"));
    QCOMPARE(window.checkedArchivePatientIds().size(), 1);
}

void MainWindowSafetyTests::archiveActionBarKeepsButtonLabels()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    PREPARE_ARCHIVE_WINDOW(window, directory);
    window.setAttribute(Qt::WA_DontShowOnScreen, true);
    window.showNormal();
    PatientInfo patient = samplePatient();
    patient.name = QStringLiteral("一个名字特别长的被测者用于检查省略显示");
    window.patientList = {patient};
    window.refreshTable(window.patientList);
    window.ui->stackedWidget->setCurrentWidget(window.ui->pageArchive);
    window.ui->table->selectRow(0);
    window.updateArchiveSelectionBar();

    const QList<QPushButton*> buttons = {window.ui->btnSelectPatient, window.ui->btnViewHistory,
                                         window.btnEditPatient, window.btnExport,
                                         window.ui->btnDeleteSelected};
    for (const QSize& size : {QSize(800, 700), QSize(1366, 768)}) {
        window.resize(size);
        QTest::qWait(30);
        QApplication::processEvents();
        for (QPushButton* button : buttons) {
            QVERIFY2(button->width() >= button->sizeHint().width(),
                     qPrintable(QStringLiteral("%1 squeezed to %2 px at %3 px")
                                    .arg(button->text()).arg(button->width()).arg(window.width())));
        }
    }
    // The long selection text gives way instead, keeping the full text as a tooltip.
    QVERIFY(window.lblArchiveSelection->toolTip().contains(patient.name));
}
