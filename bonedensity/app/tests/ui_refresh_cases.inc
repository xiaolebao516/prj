// UI-REFRESH-001 focused cases (LOOP SC-49 .. SC-56).

namespace {

QRect boundsIn(const QWidget* widget, const QWidget* ancestor)
{
    return QRect(widget->mapTo(ancestor, QPoint(0, 0)), widget->size());
}


} // namespace

// Isolated temporary data files; never the real executable directory.
#define PREPARE_ARCHIVE_WINDOW(window, directory) \
    do { \
        (window).hide(); \
        (window).patientDataWritable = true; \
        (window).xmlFilePath = (directory).filePath(QStringLiteral("patients.xml")); \
        (window).measurementsFilePath = (directory).filePath(QStringLiteral("measurements.xml")); \
    } while (false)

void MainWindowSafetyTests::patientDialogStartsWithoutDefaults()
{
    PatientFormDialog dialog(PatientFormDialog::Mode::Create);
    dialog.setSuggestedId(QStringLiteral("P20261008-001"));

    QVERIFY(!dialog.maleButton->isChecked());
    QVERIFY(!dialog.femaleButton->isChecked());
    QVERIFY(dialog.yearEdit->text().isEmpty());
    QVERIFY(dialog.monthEdit->text().isEmpty());
    QVERIFY(dialog.dayEdit->text().isEmpty());
    QCOMPARE(dialog.idEdit->text(), QStringLiteral("P20261008-001"));
    QVERIFY(!dialog.idEdit->isReadOnly());

    dialog.nameEdit->setText(QStringLiteral("王秀英"));
    QVERIFY(!dialog.validate());
    QVERIFY(!dialog.sexError->isHidden());

    dialog.femaleButton->setChecked(true);
    QVERIFY(!dialog.validate());

    dialog.yearEdit->setText(QStringLiteral("1958"));
    dialog.monthEdit->setText(QStringLiteral("4"));
    dialog.dayEdit->setText(QStringLiteral("12"));
    QVERIFY(dialog.validate());
    const int age = PatientFormDialog::ageOn(QDate(1958, 4, 12), QDate::currentDate());
    QVERIFY(dialog.ageLabel->text().contains(QString::number(age)));

    const PatientInfo patient = dialog.patient();
    QCOMPARE(patient.name, QStringLiteral("王秀英"));
    QCOMPARE(patient.gender, QStringLiteral("女"));
    QCOMPARE(patient.birthDay, QStringLiteral("1958-04-12"));
    QCOMPARE(patient.id, QStringLiteral("P20261008-001"));

    dialog.yearEdit->setText(QString::number(QDate::currentDate().year() - 10));
    QVERIFY(dialog.ageHint->text().contains(QStringLiteral("儿童")));

    dialog.yearEdit->setText(QString::number(QDate::currentDate().year() + 1));
    QVERIFY(!dialog.validate());

    dialog.yearEdit->setText(QStringLiteral("1958"));
    dialog.heightEdit->setText(QStringLiteral("1000"));
    QVERIFY(!dialog.validate());
    dialog.heightEdit->setText(QStringLiteral("156"));
    QVERIFY(dialog.validate());

    dialog.setExistingIds({QStringLiteral("P20261008-001")});
    QVERIFY(!dialog.validate());
}

void MainWindowSafetyTests::patientDialogEditKeepsIdReadOnly()
{
    const PatientInfo original = samplePatient();
    PatientFormDialog dialog(PatientFormDialog::Mode::Edit);
    dialog.setPatient(original);
    dialog.setExistingIds({original.id});

    QVERIFY(dialog.idEdit->isReadOnly());
    QVERIFY(dialog.femaleButton->isChecked());
    QCOMPARE(dialog.yearEdit->text(), QStringLiteral("1990"));
    QCOMPARE(dialog.monthEdit->text(), QStringLiteral("1"));
    QCOMPARE(dialog.dayEdit->text(), QStringLiteral("2"));
    QVERIFY(dialog.validate());

    dialog.nameEdit->setText(QStringLiteral("改名患者"));
    const PatientInfo edited = dialog.patient();
    QCOMPARE(edited.id, original.id);
    QCOMPARE(edited.name, QStringLiteral("改名患者"));
}

void MainWindowSafetyTests::suggestedPatientIdIsUnique()
{
    PatientInfo first = samplePatient();
    first.id = QStringLiteral("P20261008-001");
    PatientInfo second = samplePatient();
    second.id = QStringLiteral("P20261008-002");
    QCOMPARE(PatientFormDialog::suggestId({first, second}, QDate(2026, 10, 8)),
             QStringLiteral("P20261008-003"));
    QCOMPARE(PatientFormDialog::suggestId({}, QDate(2026, 10, 9)),
             QStringLiteral("P20261009-001"));
}

void MainWindowSafetyTests::selectingPatientShowsLatestSavedResult()
{
    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    MeasurementRecord older = sampleMeasurement();
    older.id = QStringLiteral("measurement-old");
    older.measuredAt = QStringLiteral("2025-01-05T09:00:00");
    older.sos = QStringLiteral("3950.0");
    MeasurementRecord latest = sampleMeasurement();
    latest.tScore = QStringLiteral("-1.20");
    latest.boneStrength = QStringLiteral("不足");
    window.patientList = {patient};
    window.measurementList = {older, latest};

    QVERIFY(window.selectCurrentPatient(patient));
    QCOMPARE(window.ui->lblLatestSOS->text(), QStringLiteral("4000.0"));
    QVERIFY(window.lblResultNote->text().contains(QStringLiteral("2026-07-22")));
    QVERIFY(window.lblRecentHistory->text().contains(QStringLiteral("2025-01-05")));
    QVERIFY(window.ui->btnSaveResult->isHidden());
}

void MainWindowSafetyTests::retrySaveRefreshesResultArchiveAndChart()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    PREPARE_ARCHIVE_WINDOW(window, directory);
    const PatientInfo patient = samplePatient();
    window.patientList = {patient};
    QVERIFY(window.savePatients(window.patientList));
    QVERIFY(window.selectCurrentPatient(patient));
    QVERIFY(window.ui->chartViewReference->chartData().points.isEmpty());

    window.pendingMeasurement = sampleMeasurement();
    window.hasPendingMeasurement = true;
    window.updateResultPanel();
    QVERIFY(!window.ui->btnSaveResult->isHidden());
    QVERIFY(window.lblResultNote->text().contains(QStringLiteral("未保存")));

    QVERIFY(window.trySavePendingMeasurement());
    QVERIFY(window.ui->btnSaveResult->isHidden());
    QCOMPARE(window.ui->lblLatestSOS->text(), QStringLiteral("4000.0"));
    QVERIFY(window.lblResultNote->text().contains(QStringLiteral("2026-07-22")));
    QCOMPARE(window.ui->table->rowCount(), 1);
    QCOMPARE(window.ui->table->item(0, MainWindow::ArchiveLatestColumn)->text(),
             QStringLiteral("2026-07-22"));
    QVERIFY(!window.ui->chartViewReference->chartData().points.isEmpty());
}

void MainWindowSafetyTests::deletingRecordRefreshesViews()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    PREPARE_ARCHIVE_WINDOW(window, directory);
    const PatientInfo patient = samplePatient();
    const MeasurementRecord record = sampleMeasurement();
    window.patientList = {patient};
    window.measurementList = {record};
    QVERIFY(window.savePatientData(window.patientList, window.measurementList));
    QVERIFY(window.selectCurrentPatient(patient));
    QCOMPARE(window.ui->lblLatestSOS->text(), QStringLiteral("4000.0"));

    QVERIFY(window.deleteMeasurementRecord(record.id));
    QVERIFY(window.measurementList.isEmpty());
    QCOMPARE(window.ui->lblLatestSOS->text(), QStringLiteral("--"));
    QVERIFY(window.ui->chartViewReference->chartData().points.isEmpty());
    QCOMPARE(window.ui->table->item(0, MainWindow::ArchiveLatestColumn)->text(),
             QStringLiteral("--"));
}

void MainWindowSafetyTests::editingCurrentPatientRefreshesViews()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    PREPARE_ARCHIVE_WINDOW(window, directory);
    PatientInfo patient = samplePatient();
    window.patientList = {patient};
    QVERIFY(window.savePatients(window.patientList));
    QVERIFY(window.selectCurrentPatient(patient));

    patient.name = QStringLiteral("改名患者");
    patient.weight = QStringLiteral("57");
    QVERIFY(window.updatePatient(patient));
    QCOMPARE(window.currentPatient.name, QStringLiteral("改名患者"));
    QVERIFY(window.ui->labelName->text().contains(QStringLiteral("改名患者")));
    QCOMPARE(window.ui->table->item(0, MainWindow::ArchiveNameColumn)->text(),
             QStringLiteral("改名患者"));
}

void MainWindowSafetyTests::enterSubmitsLogin()
{
    MainWindow window;
    window.hide();
    window.ui->editUsername->setText(QStringLiteral("no-such-user"));
    window.ui->editPassword->setText(QStringLiteral("wrong"));
    QTest::keyClick(window.ui->editPassword, Qt::Key_Return);
    QCOMPARE(window.ui->lblLoginMsg->text(), QStringLiteral("账号或密码错误，请重试"));
}

void MainWindowSafetyTests::closeCancelKeepsPendingNextRound()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.nextRoundDelayMs = 5000;
    window.roundSosList = {3900.0};
    window.scheduleNextPatientRound(1);
    QVERIFY(window.nextRoundTimer.isActive());

    QCloseEvent event;
    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->done(QMessageBox::Cancel);
        }
    });
    closer.start(0);
    window.closeEvent(&event);
    closer.stop();

    QVERIFY(!event.isAccepted());
    QVERIFY(window.nextRoundTimer.isActive());
    QCOMPARE(window.pendingNextRoundFinishedRounds, 1);
    QCOMPARE(window.ui->btnStartMeasurement->text(), QStringLiteral("立即开始下一轮"));
}

void MainWindowSafetyTests::batchDeleteConfirmationListsNamesAndCounts()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    PREPARE_ARCHIVE_WINDOW(window, directory);
    PatientInfo first = samplePatient();
    PatientInfo second = samplePatient();
    second.id = QStringLiteral("patient-002");
    second.name = QStringLiteral("另一位");
    MeasurementRecord a = sampleMeasurement();
    MeasurementRecord b = sampleMeasurement();
    b.id = QStringLiteral("measurement-002");
    window.patientList = {first, second};
    window.measurementList = {a, b};
    window.refreshTable(window.patientList);
    for (int row = 0; row < window.ui->table->rowCount(); ++row) {
        window.ui->table->item(row, MainWindow::ArchiveIdColumn)->setCheckState(Qt::Checked);
    }
    QVERIFY(window.lblCheckedCount->text().contains(QStringLiteral("2")));

    QString shownText;
    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, [&shownText]() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            shownText = box->text() + box->informativeText();
            box->done(QMessageBox::No);
        }
    });
    closer.start(0);
    window.on_btnDeleteSelected_clicked();
    closer.stop();

    QVERIFY(shownText.contains(QStringLiteral("测试患者（2 条检测记录）")));
    QVERIFY(shownText.contains(QStringLiteral("另一位（0 条检测记录）")));
    QCOMPARE(window.patientList.size(), 2);
    QCOMPARE(window.measurementList.size(), 2);
}

void MainWindowSafetyTests::archiveDoubleClickSetsCurrentPatient()
{
    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    window.patientList = {patient};
    window.refreshTable(window.patientList);
    window.ui->stackedWidget->setCurrentWidget(window.ui->pageArchive);

    window.on_table_cellDoubleClicked(0, MainWindow::ArchiveNameColumn);
    QCOMPARE(window.currentPatient.id, patient.id);
    QCOMPARE(window.ui->stackedWidget->currentWidget(), window.ui->pageMain);
    QVERIFY(!window.ui->btnSelectPatient->isHidden());
    QVERIFY(!window.ui->btnViewHistory->isHidden());
}

void MainWindowSafetyTests::toolbarAndResultCardStructure()
{
    MainWindow window;
    window.hide();
    QVERIFY(!window.findChild<QPushButton*>(QStringLiteral("wifi_button")));
    QVERIFY(window.ui->grpLatestResultRight->isAncestorOf(window.ui->btnSaveResult));
    QVERIFY(window.ui->btnSaveResult->isHidden());
    QVERIFY(!window.windowTitle().contains(QStringLiteral("试测")));

    QVERIFY(!window.ui->btnStartMeasurement->isEnabled());
    QVERIFY(window.lblStartHint->text().contains(QStringLiteral("连接")));
    QVERIFY(!window.ui->btnReport->isEnabled());
    QVERIFY(!window.ui->btnReport->toolTip().isEmpty());
    QVERIFY(window.lblDeviceStatus->text().contains(QStringLiteral("未连接")));

    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientDataWritable = true;
    window.updatePatientSelectionUi();
    QVERIFY(window.lblDeviceStatus->text().contains(QStringLiteral("已连接")));
    QVERIFY(window.lblStartHint->text().contains(QStringLiteral("被测者")));
}

void MainWindowSafetyTests::accountDialogFillsWidthAndStaysOpen()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    window.hide();
    QString error;
    QVERIFY(window.accountStore.loadOrInitialize(directory.filePath(QStringLiteral("accounts.xml")), &error));
    QVERIFY(window.accountStore.createUser(QStringLiteral("zhang"), QStringLiteral("pass-1234"), &error));
    window.currentAccount.username = QStringLiteral("admin");
    window.currentAccount.role = QStringLiteral("admin");

    bool inspected = false;
    QTimer inspector;
    inspector.setSingleShot(true);
    QObject::connect(&inspector, &QTimer::timeout, [&inspected]() {
        auto* dialog = qobject_cast<QDialog*>(QApplication::activeModalWidget());
        if (!dialog || dialog->objectName() != QStringLiteral("accountDialog")) return;
        auto* table = dialog->findChild<QTableWidget*>(QStringLiteral("accountTable"));
        if (table) {
            QApplication::processEvents();
            inspected = !table->verticalHeader()->isVisible() &&
                        table->horizontalHeader()->sectionResizeMode(0) == QHeaderView::Stretch &&
                        table->rowCount() == 2 &&
                        table->item(0, 2)->text() == QStringLiteral("管理员") &&
                        table->item(1, 2)->text() == QStringLiteral("普通账号");
            const QString captureDir = qEnvironmentVariable("BONE_UI_CAPTURE_DIR");
            if (!captureDir.isEmpty() && QDir().mkpath(captureDir))
                dialog->grab().save(QDir(captureDir).filePath(QStringLiteral("accounts.png")));
        }
        dialog->accept();
    });
    inspector.start(50);
    window.manageAccounts();
    QVERIFY(inspected);
}

void MainWindowSafetyTests::themedDialogGroupTitlesStayClearOfContent()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    window.setAttribute(Qt::WA_DontShowOnScreen, true);
    window.showNormal();
    QString error;
    QVERIFY(window.calibrationStore.loadOrInitialize(directory.filePath(QStringLiteral("calibration.xml")), &error));

    CalibrationDialog dialog(&window.calibrationStore, QStringLiteral("admin"), &window);
    dialog.setAttribute(Qt::WA_DontShowOnScreen, true);
    dialog.resize(960, 760);
    dialog.show();
    QTest::qWait(50);

    // The main-window card rule (title inside the frame) must not leak into dialogs.
    auto* pages = dialog.findChild<QStackedWidget*>(QStringLiteral("calibrationPages"));
    QVERIFY(pages);
    const QList<QGroupBox*> groups = pages->currentWidget()->findChildren<QGroupBox*>();
    QVERIFY(!groups.isEmpty());
    for (QGroupBox* group : groups) {
        QStyleOptionGroupBox option;
        option.initFrom(group);
        option.text = group->title();
        option.subControls = QStyle::SC_GroupBoxLabel | QStyle::SC_GroupBoxFrame;
        const QRect label = group->style()->subControlRect(QStyle::CC_GroupBox, &option,
                                                           QStyle::SC_GroupBoxLabel, group);
        for (QWidget* child : group->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly)) {
            if (!child->isVisible()) continue;
            QVERIFY2(child->geometry().top() >= label.bottom(),
                     qPrintable(group->title() + QStringLiteral(" title overlaps ") + child->objectName()));
        }
    }
    const QString captureDir = qEnvironmentVariable("BONE_UI_CAPTURE_DIR");
    if (!captureDir.isEmpty() && QDir().mkpath(captureDir)) {
        dialog.grab().save(QDir(captureDir).filePath(QStringLiteral("calibration-1.png")));
        if (auto* next = dialog.findChild<QPushButton*>(QStringLiteral("calibrationNextButton"))) {
            next->click();
            QTest::qWait(50);
            dialog.grab().save(QDir(captureDir).filePath(QStringLiteral("calibration-2.png")));
        }
        MeasurementGuideDialog guide(MeasurementGuideDialog::Mode::Manual, &window, window.mCfg.frameCorrAMin);
        guide.setAttribute(Qt::WA_DontShowOnScreen, true);
        guide.show();
        QTest::qWait(50);
        guide.grab().save(QDir(captureDir).filePath(QStringLiteral("guide-themed.png")));
        guide.close();
    }
    dialog.close();
}

void MainWindowSafetyTests::loginPageIsUsableAndShowsVersion()
{
    MainWindow window;
    window.setAttribute(Qt::WA_DontShowOnScreen, true);
    window.showNormal();
    window.resize(1366, 768);
    window.ui->stackedWidget->setCurrentWidget(window.ui->pageLogin);
    QTest::qWait(50);
    QApplication::processEvents();

    QVERIFY(window.ui->editUsername->width() >= 260);
    QVERIFY(window.ui->editPassword->width() >= 260);
    QCOMPARE(window.ui->editPassword->echoMode(), QLineEdit::Password);
    QVERIFY(window.ui->btnLogin->width() >= 260);
    auto* title = window.findChild<QLabel*>(QStringLiteral("loginTitle"));
    QVERIFY(title && title->isVisible());
    auto* version = window.findChild<QLabel*>(QStringLiteral("loginVersion"));
    QVERIFY(version && version->text().contains(QStringLiteral(APP_VERSION)));
    QVERIFY(!QStringLiteral(APP_VERSION).isEmpty());
}

void MainWindowSafetyTests::portListRefreshKeepsSelectionWithoutRebuilding()
{
    MainWindow window;
    window.hide();
    const QList<QPair<QString, QString>> ports = {
        {QStringLiteral("COM3"), QStringLiteral("蓝牙串口")},
        {QStringLiteral("COM5"), QStringLiteral("USB 串行设备")}};
    window.applyPortList(ports);
    QCOMPARE(window.ui->comboPort->count(), 2);
    window.ui->comboPort->setCurrentIndex(1);

    QSignalSpy removed(window.ui->comboPort->model(), &QAbstractItemModel::rowsRemoved);
    window.applyPortList(ports);
    QCOMPARE(removed.count(), 0);
    QCOMPARE(window.ui->comboPort->currentData().toString(), QStringLiteral("COM5"));

    window.applyPortList({{QStringLiteral("COM5"), QStringLiteral("USB 串行设备")},
                          {QStringLiteral("COM7"), QStringLiteral("新设备")}});
    QCOMPARE(window.ui->comboPort->count(), 2);
    QCOMPARE(window.ui->comboPort->currentData().toString(), QStringLiteral("COM5"));
}

void MainWindowSafetyTests::deviceWatchdogReportsMissingFrames()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.deviceResponseTimeoutMs = 0;
    window.updatePatientSelectionUi();
    QVERIFY(window.lblDeviceStatus->text().contains(QStringLiteral("已连接")));

    window.checkDeviceResponse();
    QVERIFY(!window.deviceUnresponsive);   // nothing requested yet

    window.sendCmd();
    window.checkDeviceResponse();
    QVERIFY(window.deviceUnresponsive);
    QVERIFY(window.lblDeviceStatus->text().contains(QStringLiteral("无响应")));
    QVERIFY(!window.lblDeviceStatus->toolTip().isEmpty());

    window.noteDeviceFrameReceived();
    QVERIFY(!window.deviceUnresponsive);
    QVERIFY(window.lblDeviceStatus->text().contains(QStringLiteral("已连接")));

    window.sendCmd();
    window.checkDeviceResponse();
    QVERIFY(window.deviceUnresponsive);
    window.resetDisconnectedAcquisitionState();
    QVERIFY(!window.deviceUnresponsive);
    QVERIFY(!window.awaitingDeviceFrame);
}

void MainWindowSafetyTests::mainLayoutFitsCommonWindowSizes()
{
    MainWindow window;
    window.setAttribute(Qt::WA_DontShowOnScreen, true);
    window.showNormal();
    window.ui->stackedWidget->setCurrentWidget(window.ui->pageMain);
    window.currentPatient = samplePatient();
    window.updateAgeSosReference();

    for (const QSize& size : {QSize(1366, 768), QSize(1366, 705), QSize(1600, 900), QSize(1920, 1080), QSize(1920, 1013), QSize(1920, 960)}) {
        window.resize(size);
        QTest::qWait(80);
        QApplication::processEvents();
        QWidget* page = window.ui->pageMain;
        QVERIFY2(page->width() >= size.width() - 40,
                 qPrintable(QStringLiteral("window did not reach %1x%2").arg(size.width()).arg(size.height())));

        const QList<QWidget*> regions = {
            window.ui->grpWaveArea, window.ui->grpReferenceCurveArea,
            window.ui->grpSpeedArea, window.ui->grpProcessArea,
            window.ui->grpPatientInfoRight, window.ui->grpLatestResultRight,
            window.ui->grpPartImageRight};
        for (QWidget* region : regions) {
            QVERIFY(region->isVisible());
            const QRect bounds = boundsIn(region, page);
            QVERIFY2(page->rect().contains(bounds),
                     qPrintable(QStringLiteral("%1 clipped at %2x%3").arg(region->objectName())
                                    .arg(size.width()).arg(size.height())));
            QVERIFY2(bounds.height() >= 60, qPrintable(region->objectName()));
            // Card style: no outer title margin (the title is drawn inside the card).
            QVERIFY2(region->contentsRect().top() <= 2,
                     qPrintable(region->objectName() + QStringLiteral(" lost the card style")));
        }
        for (int i = 0; i < regions.size(); ++i) {
            for (int j = i + 1; j < regions.size(); ++j) {
                QVERIFY2(!boundsIn(regions[i], page).intersects(boundsIn(regions[j], page)),
                         qPrintable(regions[i]->objectName() + QStringLiteral(" overlaps ")
                                    + regions[j]->objectName()
                                    + QStringLiteral(" at %1x%2 (block %3, wave %4, process min %5)")
                                          .arg(size.width()).arg(size.height())
                                          .arg(window.mainBlock->height())
                                          .arg(window.ui->grpWaveArea->height())
                                          .arg(window.ui->grpProcessArea->minimumSizeHint().height())
                                    + QStringLiteral(" wave y %1-%2 speed y %3 speed min %4 ref h %5")
                                          .arg(window.ui->grpWaveArea->y()).arg(window.ui->grpWaveArea->geometry().bottom())
                                          .arg(window.ui->grpSpeedArea->y())
                                          .arg(window.ui->grpSpeedArea->minimumSizeHint().height())
                                          .arg(window.ui->grpReferenceCurveArea->height())));
            }
        }

        const QRect wave = boundsIn(window.ui->grpWaveArea, page);
        const QRect ref = boundsIn(window.ui->grpReferenceCurveArea, page);
        const QRect speed = boundsIn(window.ui->grpSpeedArea, page);
        const QRect process = boundsIn(window.ui->grpProcessArea, page);
        QVERIFY(qAbs(wave.top() - ref.top()) <= 1);
        QVERIFY(qAbs(wave.bottom() - ref.bottom()) <= 1);
        QVERIFY(qAbs(speed.top() - process.top()) <= 1);
        QVERIFY(qAbs(speed.bottom() - process.bottom()) <= 1);

        QWidget* chart = window.ui->chartViewReference;
        const double aspect = window.ui->chartViewReference->imageAspectRatio();
        QVERIFY(aspect > 0.5 && aspect < 1.2);
        QVERIFY2(chart->height() <= qRound(chart->width() * aspect) + 16,
                 qPrintable(QStringLiteral("age-SOS chart has extra vertical space at %1").arg(size.width())));
        QVERIFY(window.ui->grpProcessArea->height() >= 220);
    }
}
