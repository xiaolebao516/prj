#include <QtTest>

#include "mainwindow.h"
#include "calibrationdialog.h"
#include "reportwidget.h"
#include "ui_mainwindow.h"

#include <QAbstractButton>
#include <QCloseEvent>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QMessageBox>
#include <QPageLayout>
#include <QPageSize>
#include <QPainter>
#include <QPrinter>
#include <QTemporaryDir>
#include <QTimer>

namespace {

class FakeOpenSerialPort : public QSerialPort
{
public:
    explicit FakeOpenSerialPort(QObject* parent = nullptr)
        : QSerialPort(parent)
    {
    }

    void openForTest()
    {
        setOpenMode(QIODevice::ReadWrite);
    }
};

QByteArray frame(quint16 index, quint8 channel)
{
    const quint16 gain = 1024;
    const quint16 sample = 2048;
    QByteArray bytes;
    bytes.append(char(0xAA));
    bytes.append(char(0x55));
    bytes.append(char(gain & 0xFF));
    bytes.append(char((gain >> 8) & 0xFF));
    bytes.append(char(index & 0xFF));
    bytes.append(char((index >> 8) & 0xFF));
    bytes.append(char(channel));
    bytes.append(char(1));
    bytes.append(char(0));
    bytes.append(char(sample & 0xFF));
    bytes.append(char((sample >> 8) & 0xFF));
    bytes.append(char(0xEE));
    bytes.append(char(0xEE));
    return bytes;
}

PatientInfo samplePatient()
{
    PatientInfo patient;
    patient.id = QStringLiteral("patient-001");
    patient.name = QStringLiteral("测试患者");
    patient.gender = QStringLiteral("女");
    patient.birthDay = QStringLiteral("1990-01-02");
    patient.height = QStringLiteral("165");
    patient.weight = QStringLiteral("55");
    return patient;
}

MeasurementRecord sampleMeasurement()
{
    MeasurementRecord record;
    record.id = QStringLiteral("measurement-001");
    record.patientId = QStringLiteral("patient-001");
    record.measuredAt = QStringLiteral("2026-07-22T10:30:00");
    record.sos = QStringLiteral("4000.0");
    return record;
}

} // namespace

class MainWindowSafetyTests : public QObject
{
    Q_OBJECT

private slots:
    void invalidChannelsAreDiscarded();
    void incompleteFrameGroupsAreBounded();
    void fragmentedFrameReassemblesAtEveryByteBoundary();
    void parserResynchronizesAfterNoiseAndBadTail();
    void interleavedAndWrappedFrameIndexesStayIndependent();
    void speedSeriesKeepsOnlyRecentPoints();
    void disconnectedControlsAndPlaceholdersAreSafe();
    void patientMeasurementDisablesConflictingControls();
    void debugAutoDisablesConflictingNavigation();
    void patientFormsStayInsideAndCenteredAtSmallWindow();
    void pendingResultBlocksAnotherMeasurement();
    void samePatientReselectionPreservesPendingResult();
    void samePatientReselectionPreservesPartialRounds();
    void cancelledPatientSwitchPreservesPartialRounds();
    void cancelledQuickPatientCreationDoesNotWritePatient();
    void partialRoundsRequireCloseConfirmation();
    void activeFirstRoundClosePausesAndCanResume();
    void partialRoundsBlockCalibrationDialog();
    void futureBirthDateBlocksMeasurement();
    void pendingSaveValidatesTheRecordedPatient();
    void pendingTransactionBlocksSingleFileWrites();
    void patientMeasurementStartClearsSerialAssembly();
    void serialIoErrorsResetAcquisition_data();
    void serialIoErrorsResetAcquisition();
    void reportRenderingIsReadableAndArtifactFree();
    void invalidMeasurementDateDoesNotInventAge();
    void completedReportUsesProvidedMeasurement();
    void reportPdfCanBeCommitted();
    void capturePagesWhenRequested();
};

void MainWindowSafetyTests::invalidChannelsAreDiscarded()
{
    MainWindow window;
    window.hide();
    for (quint8 channel : {quint8(0), quint8(5), quint8(255)}) {
        window.rxBuffer.append(frame(channel, channel));
        window.parseIncomingData();
        QVERIFY(window.frameGroups.isEmpty());
        QVERIFY(window.frameGroupOrder.isEmpty());
        QVERIFY(window.rxBuffer.isEmpty());
    }
}

void MainWindowSafetyTests::incompleteFrameGroupsAreBounded()
{
    MainWindow window;
    window.hide();
    for (quint16 index = 0; index < 100; ++index) {
        window.rxBuffer.append(frame(index, 1));
        window.parseIncomingData();
    }
    QCOMPARE(window.frameGroups.size(), MainWindow::maxIncompleteFrameGroups);
    QCOMPARE(window.frameGroupOrder.size(), MainWindow::maxIncompleteFrameGroups);
    QCOMPARE(window.frameGroupOrder.head(), quint16(84));
}

void MainWindowSafetyTests::fragmentedFrameReassemblesAtEveryByteBoundary()
{
    const QByteArray completeFrame = frame(42, 3);
    for (int split = 1; split < completeFrame.size(); ++split) {
        MainWindow window;
        window.hide();

        window.rxBuffer.append(completeFrame.left(split));
        window.parseIncomingData();
        QVERIFY2(!window.frameGroups.contains(42),
                 qPrintable(QStringLiteral("frame completed before byte %1 arrived").arg(split)));

        window.rxBuffer.append(completeFrame.mid(split));
        window.parseIncomingData();
        QVERIFY2(window.frameGroups.contains(42),
                 qPrintable(QStringLiteral("frame did not reassemble at split %1").arg(split)));
        const WaveGroup group = window.frameGroups.value(42);
        QVERIFY(group.has[2]);
        QCOMPARE(group.ch[2], QVector<quint16>({2048}));
        QVERIFY(window.rxBuffer.isEmpty());
    }
}

void MainWindowSafetyTests::parserResynchronizesAfterNoiseAndBadTail()
{
    MainWindow window;
    window.hide();

    QByteArray badTail = frame(10, 1);
    badTail[badTail.size() - 1] = char(0x00);
    QByteArray invalidLength = frame(12, 4).left(9);
    invalidLength[7] = char(0x00);
    invalidLength[8] = char(0x00);

    window.rxBuffer =
        QByteArray::fromHex("010203aa00ff")
        + badTail
        + invalidLength
        + frame(11, 2);
    window.parseIncomingData();

    QVERIFY(!window.frameGroups.contains(10));
    QVERIFY(!window.frameGroups.contains(12));
    QVERIFY(window.frameGroups.contains(11));
    const WaveGroup group = window.frameGroups.value(11);
    QVERIFY(group.has[1]);
    QCOMPARE(group.ch[1], QVector<quint16>({2048}));
    QVERIFY(window.rxBuffer.isEmpty());
}

void MainWindowSafetyTests::interleavedAndWrappedFrameIndexesStayIndependent()
{
    MainWindow window;
    window.hide();

    window.rxBuffer =
        frame(65535, 3)
        + frame(0, 2)
        + frame(65535, 1)
        + frame(0, 4);
    window.parseIncomingData();

    QCOMPARE(window.frameGroups.size(), 2);
    QVERIFY(window.frameGroups.value(65535).has[0]);
    QVERIFY(window.frameGroups.value(65535).has[2]);
    QVERIFY(!window.frameGroups.value(65535).has[1]);
    QVERIFY(!window.frameGroups.value(65535).has[3]);
    QVERIFY(!window.frameGroups.value(0).has[0]);
    QVERIFY(window.frameGroups.value(0).has[1]);
    QVERIFY(!window.frameGroups.value(0).has[2]);
    QVERIFY(window.frameGroups.value(0).has[3]);
    QCOMPARE(window.frameGroupOrder.size(), 2);
    QCOMPARE(window.frameGroupOrder.at(0), quint16(65535));
    QCOMPARE(window.frameGroupOrder.at(1), quint16(0));
    QVERIFY(window.rxBuffer.isEmpty());
}

void MainWindowSafetyTests::speedSeriesKeepsOnlyRecentPoints()
{
    MainWindow window;
    window.hide();
    for (int index = 0; index < 1000; ++index) {
        window.appendSpeedPoint(3000.0 + index);
    }
    QCOMPARE(window.seriesSpeed->count(), 50);
    QCOMPARE(window.seriesSpeed->at(0).x(), 950.0);
    QCOMPARE(window.seriesSpeed->at(49).x(), 999.0);
}

void MainWindowSafetyTests::disconnectedControlsAndPlaceholdersAreSafe()
{
    MainWindow window;
    window.hide();
    QVERIFY(!window.serial->isOpen());
    QVERIFY(!window.ui->pushButton->isEnabled());
    QVERIFY(!window.ui->triggerButton->isEnabled());
    QVERIFY(!window.ui->btnStartMeasurement->isEnabled());
    QVERIFY(!window.ui->wifi_button->isEnabled());
    QCOMPARE(window.ui->grpWaveArea->title(), QStringLiteral("四通道波形"));
    QCOMPARE(window.ui->grpSpeedArea->title(), QStringLiteral("声速趋势"));
    QCOMPARE(window.ui->grpProcessArea->title(), QStringLiteral("检测过程"));
    QCOMPARE(window.ui->grpPartImageRight->title(), QStringLiteral("测量部位"));
    QCOMPARE(window.ui->dateBirth->maximumDate(), QDate::currentDate());
    QCOMPARE(window.ui->dBirth->maximumDate(), QDate::currentDate());
    QCOMPARE(window.ui->eBirth->maximumDate(), QDate::currentDate());
}

void MainWindowSafetyTests::patientMeasurementDisablesConflictingControls()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    window.updatePatientSelectionUi();

    QVERIFY(window.ui->btnStartMeasurement->isEnabled());
    QCOMPARE(window.ui->btnStartMeasurement->text(), QStringLiteral("停止检测"));
    QVERIFY(!window.ui->pushButton->isEnabled());
    QVERIFY(!window.ui->triggerButton->isEnabled());
    QVERIFY(!window.ui->btnReport->isEnabled());
    QVERIFY(!window.ui->pushButton_2->isEnabled());
    QVERIFY(!window.ui->btnArchive->isEnabled());
    QVERIFY(!window.gainSliderA->isEnabled());
    QVERIFY(!window.gainSliderB->isEnabled());
    QVERIFY(!window.gainSliderC->isEnabled());
    QVERIFY(!window.gainSliderD->isEnabled());
}

void MainWindowSafetyTests::debugAutoDisablesConflictingNavigation()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.autoRunning = true;
    window.autoTimer->start(80);
    window.updatePatientSelectionUi();

    QVERIFY(!window.ui->pushButton->isEnabled());
    QVERIFY(window.ui->triggerButton->isEnabled());
    QVERIFY(!window.ui->btnPatientInfo->isEnabled());
    QVERIFY(!window.ui->btnReport->isEnabled());
    QVERIFY(!window.ui->btnArchive->isEnabled());
}

void MainWindowSafetyTests::patientFormsStayInsideAndCenteredAtSmallWindow()
{
    MainWindow window;
    window.setAttribute(Qt::WA_DontShowOnScreen, true);
    window.showNormal();
    window.resize(1200, 760);
    QTest::qWait(50);

    const auto verifyPage = [&window](QWidget* page, const QList<QWidget*>& controls) {
        window.ui->stackedWidget->setCurrentWidget(page);
        QApplication::processEvents();
        if (page->layout()) page->layout()->activate();
        QApplication::processEvents();

        int left = page->width();
        int right = 0;
        for (QWidget* control : controls) {
            QVERIFY(control);
            QVERIFY(!control->isHidden());
            const QRect bounds(control->mapTo(page, QPoint(0, 0)), control->size());
            QVERIFY2(page->rect().contains(bounds),
                     qPrintable(QStringLiteral("%1 [%2,%3 %4x%5] escaped %6 [%7,%8 %9x%10]")
                                    .arg(control->objectName())
                                    .arg(bounds.x())
                                    .arg(bounds.y())
                                    .arg(bounds.width())
                                    .arg(bounds.height())
                                    .arg(page->objectName())
                                    .arg(page->rect().x())
                                    .arg(page->rect().y())
                                    .arg(page->rect().width())
                                    .arg(page->rect().height())));
            left = qMin(left, bounds.left());
            right = qMax(right, bounds.right());
        }
        const int contentCenter = (left + right) / 2;
        QVERIFY2(qAbs(contentCenter - page->rect().center().x()) <= 80,
                 qPrintable(QStringLiteral("%1 content is visibly off-center")
                                .arg(page->objectName())));
    };

    verifyPage(window.ui->pagePatientForm,
               {window.ui->label, window.ui->editName,
                window.ui->label_18, window.ui->editID,
                window.ui->label_2, window.ui->comboGender,
                window.ui->label_3, window.ui->dateBirth,
                window.ui->label_5, window.ui->editHeight,
                window.ui->label_6, window.ui->editWeight,
                window.ui->btnFormSave, window.ui->btnFormBack});
    verifyPage(window.ui->pagePatientDetail,
               {window.ui->label_16, window.ui->dName,
                window.ui->label_13, window.ui->dID,
                window.ui->label_14, window.ui->dGender,
                window.ui->label_17, window.ui->dBirth,
                window.ui->label_12, window.ui->dHeight,
                window.ui->label_10, window.ui->dWeight,
                window.ui->btnDetailSave, window.ui->btnDetailDelete,
                window.ui->btnDetailBack});
}

void MainWindowSafetyTests::pendingResultBlocksAnotherMeasurement()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;

    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.hasPendingMeasurement = true;
    window.pendingMeasurement = sampleMeasurement();
    window.roundSosList = {4001.0, 4002.0, 4003.0, 4004.0, 4005.0};
    const QList<double> roundsBefore = window.roundSosList;

    window.updatePatientSelectionUi();
    QVERIFY(!window.ui->btnStartMeasurement->isEnabled());
    window.on_btnStartMeasurement_clicked();

    QVERIFY(!window.patientMeasureRunning);
    QVERIFY(window.hasPendingMeasurement);
    QCOMPARE(window.pendingMeasurement.id, QStringLiteral("measurement-001"));
    QCOMPARE(window.roundSosList, roundsBefore);
}

void MainWindowSafetyTests::samePatientReselectionPreservesPendingResult()
{
    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    window.currentPatient = patient;
    window.hasPendingMeasurement = true;
    window.pendingMeasurement = sampleMeasurement();
    window.roundSosList = {4001.0, 4002.0};

    window.selectCurrentPatient(patient);

    QVERIFY(window.hasPendingMeasurement);
    QCOMPARE(window.pendingMeasurement.id, QStringLiteral("measurement-001"));
    QCOMPARE(window.roundSosList, QList<double>({4001.0, 4002.0}));
}

void MainWindowSafetyTests::samePatientReselectionPreservesPartialRounds()
{
    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    window.currentPatient = patient;
    window.roundSosList = {4001.0, 4002.0};
    window.roundAList = {3991.0, 3992.0};
    window.roundBList = {4011.0, 4012.0};

    QVERIFY(window.selectCurrentPatient(patient));

    QCOMPARE(window.currentPatient.id, patient.id);
    QCOMPARE(window.roundSosList, QList<double>({4001.0, 4002.0}));
    QCOMPARE(window.roundAList, QList<double>({3991.0, 3992.0}));
    QCOMPARE(window.roundBList, QList<double>({4011.0, 4012.0}));
}

void MainWindowSafetyTests::cancelledPatientSwitchPreservesPartialRounds()
{
    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    PatientInfo otherPatient = patient;
    otherPatient.id = QStringLiteral("patient-002");
    otherPatient.name = QStringLiteral("其他患者");
    window.currentPatient = patient;
    window.roundSosList = {4001.0, 4002.0};

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->done(QMessageBox::No);
        }
    });
    closer.start(0);
    QVERIFY(!window.selectCurrentPatient(otherPatient));
    closer.stop();

    QCOMPARE(window.currentPatient.id, patient.id);
    QCOMPARE(window.roundSosList, QList<double>({4001.0, 4002.0}));
}

void MainWindowSafetyTests::cancelledQuickPatientCreationDoesNotWritePatient()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());

    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    window.patientList = {patient};
    window.currentPatient = patient;
    window.patientDataWritable = true;
    window.xmlFilePath = directory.filePath(QStringLiteral("patients.xml"));
    window.measurementsFilePath = directory.filePath(QStringLiteral("measurements.xml"));
    window.hasPendingMeasurement = true;
    window.pendingMeasurement = sampleMeasurement();
    window.ui->eName->setText(QStringLiteral("新患者"));
    window.ui->eID->setText(QStringLiteral("patient-new"));
    window.ui->eGender->setCurrentText(QStringLiteral("男"));
    window.ui->eBirth->setDate(QDate(2000, 1, 1));
    window.ui->eHeight->setText(QStringLiteral("170"));
    window.ui->eWeight->setText(QStringLiteral("60"));

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->done(QMessageBox::No);
        }
    });
    closer.start(0);
    window.on_btnPatientNewSave_clicked();
    closer.stop();

    QCOMPARE(window.patientList.size(), 1);
    QCOMPARE(window.currentPatient.id, patient.id);
    QVERIFY(window.hasPendingMeasurement);
    QVERIFY(!QFileInfo::exists(window.xmlFilePath));
}

void MainWindowSafetyTests::partialRoundsRequireCloseConfirmation()
{
    MainWindow window;
    window.hide();
    window.currentPatient = samplePatient();
    window.roundSosList = {4001.0, 4002.0};

    QCloseEvent event;
    event.ignore();
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
    QCOMPARE(window.roundSosList, QList<double>({4001.0, 4002.0}));
}

void MainWindowSafetyTests::activeFirstRoundClosePausesAndCanResume()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.patientMeasureRunning = true;
    window.acquireMode = PatientMeasureMode;
    window.currentRoundSosList = {4001.0, 4002.0};
    window.autoTimer->start(80);
    window.updatePatientSelectionUi();

    QCloseEvent event;
    event.ignore();
    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            if (QAbstractButton* button = box->button(QMessageBox::Cancel)) {
                button->click();
            }
        }
    });
    closer.start(0);
    window.closeEvent(&event);
    closer.stop();

    QVERIFY(!event.isAccepted());
    QVERIFY(window.patientMeasureRunning);
    QCOMPARE(window.acquireMode, PatientMeasureMode);
    QVERIFY(window.autoTimer->isActive());
    QCOMPARE(window.currentRoundSosList, QList<double>({4001.0, 4002.0}));
}

void MainWindowSafetyTests::partialRoundsBlockCalibrationDialog()
{
    MainWindow window;
    window.hide();
    window.currentPatient = samplePatient();
    window.roundSosList = {4001.0, 4002.0};
    const double activeDBefore = window.signalProcessor.probeDistanceCD;
    bool calibrationDialogOpened = false;

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, [&calibrationDialogOpened]() {
        QWidget* modal = QApplication::activeModalWidget();
        if (auto* dialog = qobject_cast<CalibrationDialog*>(modal)) {
            calibrationDialogOpened = true;
            dialog->close();
        } else if (auto* box = qobject_cast<QMessageBox*>(modal)) {
            box->accept();
        }
    });
    closer.start(0);
    window.openCalibrationDialog();
    closer.stop();

    QVERIFY(!calibrationDialogOpened);
    QCOMPARE(window.signalProcessor.probeDistanceCD, activeDBefore);
    QCOMPARE(window.roundSosList, QList<double>({4001.0, 4002.0}));
}

void MainWindowSafetyTests::futureBirthDateBlocksMeasurement()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientDataWritable = true;
    window.currentPatient = samplePatient();
    window.currentPatient.birthDay =
        QDate::currentDate().addDays(1).toString(QStringLiteral("yyyy-MM-dd"));

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            if (QAbstractButton* button = box->button(QMessageBox::Ok)) {
                button->click();
            }
        }
    });
    closer.start(0);
    window.startPatientMeasurement(5);
    closer.stop();

    QVERIFY(!window.patientMeasureRunning);
    QVERIFY(!window.autoTimer->isActive());
    QCOMPARE(window.acquireMode, DebugAcquireMode);
}

void MainWindowSafetyTests::pendingSaveValidatesTheRecordedPatient()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());

    MainWindow window;
    window.hide();
    PatientInfo otherPatient = samplePatient();
    otherPatient.id = QStringLiteral("patient-002");
    otherPatient.name = QStringLiteral("其他患者");
    window.patientList = {otherPatient};
    window.currentPatient = otherPatient;
    window.patientDataWritable = true;
    window.measurementsFilePath = directory.filePath(QStringLiteral("measurements.xml"));
    window.hasPendingMeasurement = true;
    window.pendingMeasurement = sampleMeasurement();

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->accept();
        }
    });
    closer.start(0);
    QVERIFY(!window.trySavePendingMeasurement());
    closer.stop();
    QVERIFY(window.hasPendingMeasurement);
    QVERIFY(window.measurementList.isEmpty());
    QVERIFY(!QFileInfo::exists(window.measurementsFilePath));
}

void MainWindowSafetyTests::pendingTransactionBlocksSingleFileWrites()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());

    MainWindow window;
    window.hide();
    window.patientDataWritable = true;
    window.xmlFilePath = directory.filePath(QStringLiteral("patients.xml"));
    window.measurementsFilePath = directory.filePath(QStringLiteral("measurements.xml"));
    QFile marker(window.xmlFilePath + QStringLiteral(".txn"));
    QVERIFY(marker.open(QIODevice::WriteOnly));
    QCOMPARE(marker.write("<invalid-marker/>"), qint64(17));
    marker.close();

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->accept();
        }
    });
    closer.start(0);
    QVERIFY(!window.saveMeasurements({sampleMeasurement()}));
    closer.stop();

    QVERIFY(!window.patientDataWritable);
    QVERIFY(!QFileInfo::exists(window.measurementsFilePath));
}

void MainWindowSafetyTests::patientMeasurementStartClearsSerialAssembly()
{
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.currentPatient = samplePatient();
    window.rxBuffer = QByteArray::fromHex("aa55");
    window.frameGroups.insert(7, WaveGroup());
    window.frameGroupOrder.enqueue(7);
    window.samplesA = {1, 2, 3};
    window.samplesB = {4, 5, 6};
    window.chReceived[0] = true;
    window.chReceived[1] = true;

    window.startPatientMeasurement(5);

    QVERIFY(window.patientMeasureRunning);
    QVERIFY(window.rxBuffer.isEmpty());
    QVERIFY(window.frameGroups.isEmpty());
    QVERIFY(window.frameGroupOrder.isEmpty());
    QVERIFY(window.samplesA.isEmpty());
    QVERIFY(window.samplesB.isEmpty());
    for (bool received : window.chReceived) QVERIFY(!received);
    window.stopPatientMeasurement();
}

void MainWindowSafetyTests::serialIoErrorsResetAcquisition_data()
{
    QTest::addColumn<int>("error");
    QTest::newRow("resource-error") << int(QSerialPort::ResourceError);
    QTest::newRow("read-error") << int(QSerialPort::ReadError);
    QTest::newRow("write-error") << int(QSerialPort::WriteError);
}

void MainWindowSafetyTests::serialIoErrorsResetAcquisition()
{
    QFETCH(int, error);
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* serial = new FakeOpenSerialPort(&window);
    serial->openForTest();
    window.serial = serial;
    window.patientMeasureRunning = true;
    window.autoRunning = true;
    window.acquireMode = PatientMeasureMode;
    window.rxBuffer = QByteArray::fromHex("aa55");
    window.frameGroups.insert(9, WaveGroup());
    window.frameGroupOrder.enqueue(9);
    window.autoTimer->start(80);

    QTimer closer;
    closer.setSingleShot(true);
    QObject::connect(&closer, &QTimer::timeout, []() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            box->accept();
        }
    });
    closer.start(0);
    window.handleSerialError(static_cast<QSerialPort::SerialPortError>(error));
    closer.stop();

    QVERIFY(window.serialErrorHandled);
    QVERIFY(!window.serial->isOpen());
    QVERIFY(!window.patientMeasureRunning);
    QVERIFY(!window.autoRunning);
    QVERIFY(!window.autoTimer->isActive());
    QVERIFY(window.rxBuffer.isEmpty());
    QVERIFY(window.frameGroups.isEmpty());
    QVERIFY(window.frameGroupOrder.isEmpty());
}

void MainWindowSafetyTests::reportRenderingIsReadableAndArtifactFree()
{
    ReportData data;
    data.patientName = QStringLiteral("【测试】二十岁女性");
    data.patientId = QStringLiteral("W20");
    data.age = QStringLiteral("20");
    data.gender = QStringLiteral("女");
    data.birthDay = QStringLiteral("2006-01-01");
    data.measuredAt = QStringLiteral("2026-07-17 08:10");
    data.height = QStringLiteral("166 cm");
    data.weight = QStringLiteral("54 kg");
    data.part = QStringLiteral("桡骨");
    data.sos = QStringLiteral("4200.0 m/s");
    data.boneStrength = QStringLiteral("测试数据");
    data.diagnosis = QStringLiteral("仅用于年龄-SOS参考图演示，不提供诊断结论");
    data.operatorName = QStringLiteral("测试");

    ReportWidget report;
    report.setReportData(data);
    QImage image(QSize(795, 1124), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    report.renderReport(&painter, QRectF(QPointF(0, 0), image.size()));
    painter.end();

    QCOMPARE(image.pixelColor(173, 825), QColor(Qt::white));
    QVERIFY(image.pixelColor(794, 500).red() > 200);
    QCOMPARE(report.reportData().tScore, QString());
    QCOMPARE(report.reportData().zScore, QString());

    const QString captureDir = qEnvironmentVariable("BONE_UI_CAPTURE_DIR");
    if (!captureDir.isEmpty()) {
        QVERIFY(QDir().mkpath(captureDir));
        QVERIFY(image.save(QDir(captureDir).filePath(QStringLiteral("report.png"))));
    }
}

void MainWindowSafetyTests::invalidMeasurementDateDoesNotInventAge()
{
    MainWindow window;
    window.hide();
    PatientInfo patient = samplePatient();
    MeasurementRecord measurement = sampleMeasurement();
    measurement.patientAge.clear();
    measurement.patientBirthDay = patient.birthDay;
    measurement.measuredAt = QStringLiteral("invalid-date");

    const ReportData data = window.buildReportData(patient, measurement);

    QVERIFY(data.age.isEmpty());
    QCOMPARE(data.measuredAt, QStringLiteral("invalid-date"));
}

void MainWindowSafetyTests::completedReportUsesProvidedMeasurement()
{
    MainWindow window;
    window.hide();
    const PatientInfo patient = samplePatient();
    window.currentPatient = patient;

    MeasurementRecord futureRecord = sampleMeasurement();
    futureRecord.id = QStringLiteral("future-record");
    futureRecord.sos = QStringLiteral("9999.0");
    futureRecord.measuredAt = QStringLiteral("2099-01-01T00:00:00");
    window.measurementList = {futureRecord};

    MeasurementRecord completed = sampleMeasurement();
    completed.id = QStringLiteral("just-completed");
    completed.sos = QStringLiteral("4000.0");
    completed.measuredAt = QStringLiteral("2026-07-26T10:00:00");
    window.showPatientMeasureFinishedDialog(completed);

    QCOMPARE(window.reportWidget->reportData().sos, QStringLiteral("4000.0 m/s"));
    QCOMPARE(window.reportWidget->reportData().measuredAt,
             QStringLiteral("2026-07-26 10:00"));
}

void MainWindowSafetyTests::reportPdfCanBeCommitted()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString pdfPath = directory.filePath(QStringLiteral("report.pdf"));

    MainWindow window;
    window.hide();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(pdfPath);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setPageMargins(QMarginsF(8, 8, 8, 8), QPageLayout::Millimeter);

    QVERIFY(window.renderReportToPrinter(&printer));
    QFile pdf(pdfPath);
    QVERIFY(pdf.open(QIODevice::ReadOnly));
    QVERIFY(pdf.size() > 1000);
    QCOMPARE(pdf.read(4), QByteArray("%PDF"));
}

void MainWindowSafetyTests::capturePagesWhenRequested()
{
    const QString captureDir = qEnvironmentVariable("BONE_UI_CAPTURE_DIR");
    if (captureDir.isEmpty()) QSKIP("BONE_UI_CAPTURE_DIR is not set");
    QVERIFY(QDir().mkpath(captureDir));

    MainWindow window;
    window.showNormal();
    window.resize(1920, 1080);
    QTest::qWait(100);

    const auto capture = [&window, &captureDir](QWidget* page, const QString& name) {
        window.ui->stackedWidget->setCurrentWidget(page);
        window.scheduleResponsiveLayout();
        QTest::qWait(100);
        QVERIFY2(window.grab().save(QDir(captureDir).filePath(name)), qPrintable(name));
    };

    capture(window.ui->pageMain, QStringLiteral("main.png"));
    capture(window.ui->pageArchive, QStringLiteral("archive.png"));
    capture(window.ui->pagePatientForm, QStringLiteral("patient-form.png"));
    capture(window.ui->pagePatientDetail, QStringLiteral("patient-detail.png"));
}

QTEST_MAIN(MainWindowSafetyTests)
#include "mainwindow_safety_tests.moc"
