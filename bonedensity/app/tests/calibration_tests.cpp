#include <QtTest>

#include "calibration.h"
#include "calibrationdialog.h"
#include "calibrationstore.h"

#include <QAbstractButton>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSignalSpy>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTimer>

namespace {

CalibrationSetup formalSetup()
{
    CalibrationSetup setup;
    setup.mode = CalibrationMode::Formal;
    setup.operatorName = QStringLiteral("tester");
    setup.probeId = QStringLiteral("probe-001");
    setup.phantomModel = QStringLiteral("phantom-A");
    setup.phantomSerial = QStringLiteral("serial-001");
    setup.certificateId = QStringLiteral("cert-001");
    setup.referenceTemperature = 23.0;
    setup.referenceSos = 2500.0;
    return setup;
}

CalibrationFrame validFrame(double sosB,
                            bool aValid = true,
                            bool peakConsistent = true)
{
    CalibrationFrame frame;
    frame.bValid = true;
    frame.aValid = aValid;
    frame.peakConsistent = peakConsistent;
    frame.sosA = 2490.0;
    frame.sosB = sosB;
    frame.corrA = 0.85;
    frame.corrB = 0.75;
    frame.lagA = 198;
    frame.lagB = 200;
    return frame;
}

void completeMeasurement(CalibrationSession& session,
                         CalibrationPhase phase,
                         double sosB,
                         bool aValid = true)
{
    QString error;
    QVERIFY2(session.beginMeasurement(phase, 23.0, &error), qPrintable(error));
    for (int frame = 0; frame < CalibrationSession::RequiredFramesPerMeasurement; ++frame) {
        const CalibrationFrameResult result = session.addFrame(validFrame(sosB, aValid), &error);
        if (frame + 1 < CalibrationSession::RequiredFramesPerMeasurement) {
            QCOMPARE(result, CalibrationFrameResult::Accepted);
        } else {
            QCOMPARE(result, CalibrationFrameResult::MeasurementCompleted);
        }
    }
}

CalibrationRecord passingRecord()
{
    CalibrationSession session;
    QString error;
    const CalibrationSetup setup = formalSetup();
    if (!session.configure(setup, 7.84e-3, &error)) return {};
    for (int i = 0; i < 6; ++i) completeMeasurement(session, CalibrationPhase::Calibration, 2450.0);
    for (int i = 0; i < 6; ++i) completeMeasurement(session, CalibrationPhase::Validation, 2500.0);
    return session.record();
}

} // namespace

class CalibrationTests : public QObject
{
    Q_OBJECT

private slots:
    void candidateFormula();
    void setupAndTemperatureValidation();
    void measurementHierarchyAndBOnlyCandidate();
    void validationThresholds();
    void developmentModeCannotActivate();
    void invalidAndBoundaryFramesAreRejected();
    void peakConsistencyRejectsValidationFrame();
    void storeActivationReloadAndRestore();
    void corruptStoreBacksUpAndRecovers();
    void failedWriteDoesNotChangeMemory();
    void partialCalibrationCloseCanBeCancelled();
    void collectingCalibrationClosePausesAndResumesOnCancel();
    void unsavedCompletedCalibrationCannotCloseSilently();
    void dialogWorkflowAndGuidance();
};

void CalibrationTests::candidateFormula()
{
    const double result = CalibrationSession::candidateD(7.84e-3, 2500.0, 2450.0);
    QVERIFY(std::abs(result - 8.0e-3) < 1e-12);
    QCOMPARE(CalibrationSession::candidateD(0.0, 2500.0, 2450.0), 0.0);
    QCOMPARE(CalibrationSession::candidateD(7.84e-3, -1.0, 2450.0), 0.0);
}

void CalibrationTests::setupAndTemperatureValidation()
{
    CalibrationSession session;
    CalibrationSetup setup = formalSetup();
    QString error;

    setup.certificateId.clear();
    QVERIFY(!session.configure(setup, 7.84e-3, &error));
    QVERIFY(error.contains(QStringLiteral("证书编号")));

    setup = formalSetup();
    QVERIFY(session.configure(setup, 7.84e-3, &error));
    QVERIFY(session.beginMeasurement(CalibrationPhase::Calibration, 24.0, &error));
    session.cancelCurrentMeasurement();
    QVERIFY(!session.beginMeasurement(CalibrationPhase::Calibration, 24.01, &error));
    QVERIFY(error.contains(QStringLiteral("±1 C")));
}

void CalibrationTests::measurementHierarchyAndBOnlyCandidate()
{
    CalibrationSession session;
    QString error;
    QVERIFY(session.configure(formalSetup(), 7.84e-3, &error));

    for (int measurement = 0; measurement < 5; ++measurement) {
        completeMeasurement(session, CalibrationPhase::Calibration, 2450.0, false);
    }
    QVERIFY(!session.calibrationComplete());
    QCOMPARE(session.candidateD(), 0.0);

    completeMeasurement(session, CalibrationPhase::Calibration, 2450.0, false);
    QVERIFY(session.calibrationComplete());
    QVERIFY(std::abs(session.candidateD() - 8.0e-3) < 1e-12);
    QCOMPARE(session.record().calibrationMeasurements.size(), 6);
    QVERIFY(!session.record().calibrationMeasurements.first().aDiagnosticAvailable);
}

void CalibrationTests::validationThresholds()
{
    CalibrationSession passing;
    QString error;
    QVERIFY(passing.configure(formalSetup(), 7.84e-3, &error));
    for (int i = 0; i < 6; ++i) completeMeasurement(passing, CalibrationPhase::Calibration, 2450.0);
    for (int i = 0; i < 6; ++i) completeMeasurement(passing, CalibrationPhase::Validation, 2550.0);
    QVERIFY(passing.validationComplete());
    QVERIFY(passing.record().evaluation.passed);
    QVERIFY(std::abs(passing.record().evaluation.relativeErrorPercent - 2.0) < 1e-9);

    CalibrationSession errorFailure;
    QVERIFY(errorFailure.configure(formalSetup(), 7.84e-3, &error));
    for (int i = 0; i < 6; ++i) completeMeasurement(errorFailure, CalibrationPhase::Calibration, 2450.0);
    for (int i = 0; i < 6; ++i) completeMeasurement(errorFailure, CalibrationPhase::Validation, 2550.1);
    QVERIFY(!errorFailure.record().evaluation.passed);
    QVERIFY(errorFailure.record().evaluation.failureReason.contains(QStringLiteral("±2%")));

    CalibrationSession repeatabilityFailure;
    QVERIFY(repeatabilityFailure.configure(formalSetup(), 7.84e-3, &error));
    for (int i = 0; i < 6; ++i) completeMeasurement(repeatabilityFailure, CalibrationPhase::Calibration, 2450.0);
    const double values[] = {2450.0, 2550.0, 2450.0, 2550.0, 2450.0, 2550.0};
    for (double value : values) completeMeasurement(repeatabilityFailure, CalibrationPhase::Validation, value);
    QVERIFY(!repeatabilityFailure.record().evaluation.passed);
    QVERIFY(repeatabilityFailure.record().evaluation.repeatabilityCvPercent > 1.0);
}

void CalibrationTests::developmentModeCannotActivate()
{
    CalibrationSetup setup = formalSetup();
    setup.mode = CalibrationMode::Development;
    setup.phantomModel.clear();
    setup.phantomSerial.clear();
    setup.certificateId.clear();

    CalibrationSession session;
    QString error;
    QVERIFY(session.configure(setup, 7.84e-3, &error));
    for (int i = 0; i < 6; ++i) completeMeasurement(session, CalibrationPhase::Calibration, 2450.0);
    for (int i = 0; i < 6; ++i) completeMeasurement(session, CalibrationPhase::Validation, 2500.0);
    QVERIFY(session.record().evaluation.passed);
    QVERIFY(!session.canActivate());
}

void CalibrationTests::invalidAndBoundaryFramesAreRejected()
{
    CalibrationSession session;
    QString error;
    QVERIFY(session.configure(formalSetup(), 7.84e-3, &error));
    QVERIFY(session.beginMeasurement(CalibrationPhase::Calibration, 23.0, &error));

    CalibrationFrame invalid = validFrame(2500.0);
    invalid.bValid = false;
    QCOMPARE(session.addFrame(invalid, &error), CalibrationFrameResult::Rejected);
    QVERIFY(error.contains(QStringLiteral("B通道")));

    CalibrationFrame boundary = validFrame(2500.0);
    boundary.boundaryPeak = true;
    QCOMPARE(session.addFrame(boundary, &error), CalibrationFrameResult::Rejected);
    QVERIFY(error.contains(QStringLiteral("搜索边界")));
    QCOMPARE(session.currentValidFrameCount(), 0);
    QCOMPARE(session.currentRejectedFrameCount(), 2);
}

void CalibrationTests::peakConsistencyRejectsValidationFrame()
{
    CalibrationSession session;
    QString error;
    QVERIFY(session.configure(formalSetup(), 7.84e-3, &error));
    for (int i = 0; i < 6; ++i) completeMeasurement(session, CalibrationPhase::Calibration, 2450.0);
    QVERIFY(session.beginMeasurement(CalibrationPhase::Validation, 23.0, &error));

    const CalibrationFrameResult result = session.addFrame(validFrame(2500.0, true, false), &error);
    QCOMPARE(result, CalibrationFrameResult::Rejected);
    QCOMPARE(session.currentValidFrameCount(), 0);
    QCOMPARE(session.currentRejectedFrameCount(), 1);
    QVERIFY(error.contains(QStringLiteral("相关峰不一致")));
}

void CalibrationTests::storeActivationReloadAndRestore()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("calibration.xml"));

    CalibrationStore store;
    QString error;
    QVERIFY2(store.loadOrInitialize(path, &error), qPrintable(error));
    QCOMPARE(store.parameters().activeD, CalibrationStore::FactoryDefaultD);

    const CalibrationRecord record = passingRecord();
    QVERIFY2(store.saveCompletedSession(record, true, &error), qPrintable(error));
    QVERIFY(std::abs(store.parameters().activeD - 8.0e-3) < 1e-12);
    QVERIFY(std::abs(store.parameters().previousD - 7.84e-3) < 1e-12);
    QCOMPARE(store.history().size(), 1);
    QCOMPARE(store.history().last().status, QStringLiteral("ACTIVATED"));

    CalibrationStore reloaded;
    QVERIFY2(reloaded.loadOrInitialize(path, &error), qPrintable(error));
    QVERIFY(std::abs(reloaded.parameters().activeD - 8.0e-3) < 1e-12);
    QCOMPARE(reloaded.history().size(), 1);

    QVERIFY2(reloaded.restorePrevious(QStringLiteral("tester"), &error), qPrintable(error));
    QVERIFY(std::abs(reloaded.parameters().activeD - 7.84e-3) < 1e-12);
    QVERIFY(std::abs(reloaded.parameters().previousD - 8.0e-3) < 1e-12);
    QVERIFY2(reloaded.restoreDefault(QStringLiteral("tester"), &error), qPrintable(error));
    QCOMPARE(reloaded.parameters().activeD, CalibrationStore::FactoryDefaultD);
}

void CalibrationTests::corruptStoreBacksUpAndRecovers()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("calibration.xml"));
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("<CalibrationData><broken>");
    file.close();

    CalibrationStore store;
    QString warning;
    QVERIFY(store.loadOrInitialize(path, &warning));
    QVERIFY(!warning.isEmpty());
    QVERIFY(QFile::exists(store.lastBackupPath()));
    QCOMPARE(store.parameters().activeD, CalibrationStore::FactoryDefaultD);

    CalibrationStore reloaded;
    QString error;
    QVERIFY2(reloaded.loadOrInitialize(path, &error), qPrintable(error));
    QCOMPARE(reloaded.parameters().activeD, CalibrationStore::FactoryDefaultD);
}

void CalibrationTests::failedWriteDoesNotChangeMemory()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("calibration.xml"));

    CalibrationStore store;
    QString error;
    QVERIFY(store.loadOrInitialize(path, &error));
    const double before = store.parameters().activeD;

    QVERIFY(QFile::remove(path));
    QVERIFY(QDir().mkdir(path));
    QVERIFY(!store.saveCompletedSession(passingRecord(), true, &error));
    QCOMPARE(store.parameters().activeD, before);
}

void CalibrationTests::partialCalibrationCloseCanBeCancelled()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    CalibrationStore store;
    QString error;
    QVERIFY(store.loadOrInitialize(directory.filePath(QStringLiteral("calibration.xml")), &error));

    CalibrationDialog dialog(&store, QStringLiteral("tester"));
    QVERIFY(dialog.session_.configure(formalSetup(), store.parameters().activeD, &error));
    completeMeasurement(dialog.session_, CalibrationPhase::Calibration, 2450.0);

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
    dialog.closeEvent(&event);
    closer.stop();

    QVERIFY(!event.isAccepted());
    QCOMPARE(dialog.session_.completedMeasurementCount(CalibrationPhase::Calibration), 1);
}

void CalibrationTests::collectingCalibrationClosePausesAndResumesOnCancel()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    CalibrationStore store;
    QString error;
    QVERIFY(store.loadOrInitialize(directory.filePath(QStringLiteral("calibration.xml")), &error));

    CalibrationDialog dialog(&store, QStringLiteral("tester"));
    QVERIFY(dialog.session_.configure(formalSetup(), store.parameters().activeD, &error));
    QVERIFY(dialog.session_.beginMeasurement(CalibrationPhase::Calibration, 23.0, &error));
    QSignalSpy stopSpy(&dialog, &CalibrationDialog::acquisitionStopRequested);
    QSignalSpy startSpy(&dialog, &CalibrationDialog::acquisitionStartRequested);

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
    dialog.closeEvent(&event);
    closer.stop();

    QVERIFY(!event.isAccepted());
    QVERIFY(dialog.session_.isCollecting());
    QCOMPARE(stopSpy.count(), 1);
    QCOMPARE(startSpy.count(), 1);
    QCOMPARE(startSpy.first().first().toDouble(), dialog.session_.processingD());
}

void CalibrationTests::unsavedCompletedCalibrationCannotCloseSilently()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("calibration.xml"));
    CalibrationStore store;
    QString error;
    QVERIFY(store.loadOrInitialize(path, &error));

    CalibrationDialog dialog(&store, QStringLiteral("tester"));
    QVERIFY(dialog.session_.configure(formalSetup(), store.parameters().activeD, &error));
    for (int i = 0; i < 6; ++i) {
        completeMeasurement(dialog.session_, CalibrationPhase::Calibration, 2450.0);
    }
    for (int i = 0; i < 6; ++i) {
        completeMeasurement(dialog.session_, CalibrationPhase::Validation, 2500.0);
    }
    QVERIFY(dialog.session_.validationComplete());
    QVERIFY(QFile::remove(path));
    QVERIFY(QDir().mkdir(path));

    QCloseEvent event;
    event.ignore();
    QTimer closer;
    closer.setInterval(0);
    QObject::connect(&closer, &QTimer::timeout, [&closer]() {
        auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
        if (!box) return;
        if (box->windowTitle() == QStringLiteral("保存校准结果失败")) {
            if (QAbstractButton* button = box->button(QMessageBox::Ok)) {
                button->click();
            }
            return;
        }
        if (box->windowTitle() == QStringLiteral("校准结果尚未保存")) {
            closer.stop();
            if (QAbstractButton* button = box->button(QMessageBox::Cancel)) {
                button->click();
            }
        }
    });
    closer.start();
    dialog.closeEvent(&event);
    closer.stop();

    QVERIFY(!event.isAccepted());
    QVERIFY(!dialog.recordSaved_);
    QVERIFY(dialog.session_.validationComplete());
}

void CalibrationTests::dialogWorkflowAndGuidance()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    CalibrationStore store;
    QString error;
    QVERIFY2(store.loadOrInitialize(directory.filePath(QStringLiteral("calibration.xml")), &error),
             qPrintable(error));

    CalibrationDialog dialog(&store, QStringLiteral("tester"));
    dialog.resize(820, 650);
    dialog.show();
    QTest::qWait(20);

    QString allText;
    for (QLabel* label : dialog.findChildren<QLabel*>()) allText += label->text();
    for (QCheckBox* check : dialog.findChildren<QCheckBox*>()) allText += check->text();
    QVERIFY(allText.contains(QStringLiteral("软件不验证证书真实性")));
    QVERIFY(allText.contains(QStringLiteral("同一种耦合剂")));
    QVERIFY(allText.contains(QStringLiteral("待温度稳定")));
    QVERIFY(allText.contains(QStringLiteral("±2%")));
    QVERIFY(allText.contains(QStringLiteral("变异系数不超过 <b>1%</b>")));

    auto* pages = dialog.findChild<QStackedWidget*>(QStringLiteral("calibrationPages"));
    auto* next = dialog.findChild<QPushButton*>(QStringLiteral("calibrationNextButton"));
    QVERIFY(pages);
    QVERIFY(next);
    QCOMPARE(pages->currentIndex(), 0);
    QTest::mouseClick(next, Qt::LeftButton);
    QCOMPARE(pages->currentIndex(), 1);

    auto* mode = dialog.findChild<QComboBox*>(QStringLiteral("calibrationModeCombo"));
    auto* operatorEdit = dialog.findChild<QLineEdit*>(QStringLiteral("calibrationOperatorEdit"));
    auto* probe = dialog.findChild<QLineEdit*>(QStringLiteral("calibrationProbeIdEdit"));
    auto* phantomModel = dialog.findChild<QLineEdit*>(QStringLiteral("calibrationPhantomModelEdit"));
    auto* phantomSerial = dialog.findChild<QLineEdit*>(QStringLiteral("calibrationPhantomSerialEdit"));
    auto* certificate = dialog.findChild<QLineEdit*>(QStringLiteral("calibrationCertificateEdit"));
    auto* certificateConfirmed = dialog.findChild<QCheckBox*>(QStringLiteral("calibrationCertificateConfirmed"));
    auto* responsibilityConfirmed = dialog.findChild<QCheckBox*>(QStringLiteral("calibrationResponsibilityConfirmed"));
    QVERIFY(mode && operatorEdit && probe && phantomModel && phantomSerial && certificate);
    QVERIFY(certificateConfirmed && responsibilityConfirmed);
    QVERIFY(operatorEdit->isReadOnly());
    QCOMPARE(operatorEdit->text(), QStringLiteral("tester"));

    mode->setCurrentIndex(1);
    QVERIFY(!certificate->isEnabled());
    mode->setCurrentIndex(0);
    QVERIFY(certificate->isEnabled());
    probe->setText(QStringLiteral("probe-001"));
    phantomModel->setText(QStringLiteral("phantom-A"));
    phantomSerial->setText(QStringLiteral("serial-001"));
    certificate->setText(QStringLiteral("cert-001"));
    certificateConfirmed->setChecked(true);
    responsibilityConfirmed->setChecked(true);
    QTest::mouseClick(next, Qt::LeftButton);
    QCOMPARE(pages->currentIndex(), 2);

    QSignalSpy startSpy(&dialog, &CalibrationDialog::acquisitionStartRequested);
    QSignalSpy stopSpy(&dialog, &CalibrationDialog::acquisitionStopRequested);
    auto completeUiMeasurement = [&](const QString& operationName,
                                     const QString& startName,
                                     double sosB) {
        auto* operation = dialog.findChild<QCheckBox*>(operationName);
        auto* start = dialog.findChild<QPushButton*>(startName);
        QVERIFY(operation && start);
        operation->setChecked(true);
        QTest::mouseClick(start, Qt::LeftButton);
        for (int frame = 0; frame < CalibrationSession::RequiredFramesPerMeasurement; ++frame) {
            dialog.submitFrame(validFrame(sosB));
        }
    };

    for (int measurement = 0; measurement < 6; ++measurement) {
        completeUiMeasurement(QStringLiteral("calibrationOperationConfirmed"),
                              QStringLiteral("calibrationStartButton"),
                              2450.0);
    }
    QCOMPARE(startSpy.count(), 6);
    QCOMPARE(stopSpy.count(), 6);
    auto* calibrationTable = dialog.findChild<QTableWidget*>(QStringLiteral("calibrationMeasurementTable"));
    QVERIFY(calibrationTable);
    QCOMPARE(calibrationTable->item(5, 3)->text(), QStringLiteral("2450.0"));
    QVERIFY(next->isEnabled());

    QTest::mouseClick(next, Qt::LeftButton);
    QCOMPARE(pages->currentIndex(), 3);
    auto* reposition = dialog.findChild<QCheckBox*>(QStringLiteral("calibrationRepositionConfirmed"));
    QVERIFY(reposition);
    reposition->setChecked(true);
    QTest::mouseClick(next, Qt::LeftButton);
    QCOMPARE(pages->currentIndex(), 4);

    for (int measurement = 0; measurement < 6; ++measurement) {
        completeUiMeasurement(QStringLiteral("validationOperationConfirmed"),
                              QStringLiteral("validationStartButton"),
                              2500.0);
    }
    QCOMPARE(startSpy.count(), 12);
    QCOMPARE(stopSpy.count(), 12);
    QCOMPARE(store.history().size(), 1);
    QCOMPARE(store.parameters().activeD, CalibrationStore::FactoryDefaultD);

    QTest::mouseClick(next, Qt::LeftButton);
    QCOMPARE(pages->currentIndex(), 5);
    auto* result = dialog.findChild<QLabel*>(QStringLiteral("calibrationResultLabel"));
    auto* activate = dialog.findChild<QPushButton*>(QStringLiteral("calibrationActivateButton"));
    QVERIFY(result && activate);
    QVERIFY(result->text().contains(QStringLiteral("工程验证通过")));
    QVERIFY(result->text().contains(QStringLiteral("候选D：8.0000 mm")));
    QVERIFY(activate->isVisible());
    QVERIFY(activate->isEnabled());

    const QRect dialogRect = dialog.rect();
    for (QWidget* child : dialog.findChildren<QWidget*>()) {
        if (!child->isVisible() || child->isWindow()) continue;
        const QPoint topLeft = child->mapTo(&dialog, QPoint(0, 0));
        const QRect childRect(topLeft, child->size());
        QVERIFY2(dialogRect.contains(childRect), qPrintable(child->objectName()));
    }
}

QTEST_MAIN(CalibrationTests)
#include "calibration_tests.moc"
