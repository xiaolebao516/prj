#include <QtTest>

#include "patientstore.h"

#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

namespace {

bool writeBytes(const QString& path, const QByteArray& data)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return false;
    return file.write(data) == data.size();
}

QByteArray readBytes(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return {};
    return file.readAll();
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
    record.operatorName = QStringLiteral("tester");
    record.part = QStringLiteral("桡骨");
    record.sos = QStringLiteral("4000.0");
    record.tScore = QStringLiteral("0.10");
    record.zScore = QStringLiteral("0.20");
    record.diagnosis = QStringLiteral("测试");
    return record;
}

bool writeTransactionMarker(const QString& path,
                            bool patientsExisted,
                            bool measurementsExisted)
{
    QDomDocument document;
    QDomElement root = document.createElement(QStringLiteral("patientDataTransaction"));
    root.setAttribute(QStringLiteral("version"), QStringLiteral("1"));
    root.setAttribute(QStringLiteral("patientsExisted"), patientsExisted ? 1 : 0);
    root.setAttribute(QStringLiteral("measurementsExisted"), measurementsExisted ? 1 : 0);
    document.appendChild(root);
    return writeBytes(path, document.toByteArray(4));
}

void verifyDocumentRoot(const QString& path,
                        const QString& expectedRoot,
                        const QString& expectedVersion)
{
    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QDomDocument document;
    QVERIFY(static_cast<bool>(document.setContent(&file)));
    QCOMPARE(document.documentElement().tagName(), expectedRoot);
    QCOMPARE(document.documentElement().attribute(QStringLiteral("version")), expectedVersion);
}

} // namespace

class PatientStoreTests : public QObject
{
    Q_OBJECT

private slots:
    void corruptPatientsAreCopiedWithoutChangingSource();
    void corruptMeasurementsAreCopiedWithoutChangingSource();
    void semanticCorruptionIsRejected_data();
    void semanticCorruptionIsRejected();
    void pairedSaveAndDeleteKeepDocumentsConsistent();
    void versionlessLegacyStillMigrates();
    void unfinishedTransactionIsRecoveredBeforeLoad();
    void missingTransactionBackupRefusesLoad();
    void invalidTransactionBackupDoesNotOverwriteLiveFiles_data();
    void invalidTransactionBackupDoesNotOverwriteLiveFiles();
    void transactionRecoveryHandlesOriginalExistenceMatrix_data();
    void transactionRecoveryHandlesOriginalExistenceMatrix();
};

void PatientStoreTests::corruptPatientsAreCopiedWithoutChangingSource()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString patientsPath = directory.filePath(QStringLiteral("patients.xml"));
    const QString measurementsPath = directory.filePath(QStringLiteral("measurements.xml"));
    const QByteArray damaged("<patients><patient id=\"broken\"></patients>");
    QVERIFY(writeBytes(patientsPath, damaged));
    QVERIFY(writeBytes(measurementsPath, "<measurements version=\"1\"/>") );

    PatientStore store;
    QList<PatientInfo> patients;
    QList<MeasurementRecord> measurements;
    QString error;
    QVERIFY(!store.load(patientsPath, measurementsPath, &patients, &measurements, &error));
    const QString firstBackup = patientsPath + QStringLiteral(".bad.bak");
    QCOMPARE(readBytes(patientsPath), damaged);
    QCOMPARE(readBytes(firstBackup), damaged);
    QVERIFY2(error.contains(firstBackup), qPrintable(error));

    error.clear();
    QVERIFY(!store.load(patientsPath, measurementsPath, &patients, &measurements, &error));
    const QString secondBackup = patientsPath + QStringLiteral(".bad.1.bak");
    QCOMPARE(readBytes(patientsPath), damaged);
    QCOMPARE(readBytes(secondBackup), damaged);
    QVERIFY2(error.contains(secondBackup), qPrintable(error));
}

void PatientStoreTests::corruptMeasurementsAreCopiedWithoutChangingSource()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString patientsPath = directory.filePath(QStringLiteral("patients.xml"));
    const QString measurementsPath = directory.filePath(QStringLiteral("measurements.xml"));
    const QByteArray damaged("<measurements><measurement id=\"broken\"></measurements>");
    QVERIFY(writeBytes(patientsPath, "<patients version=\"2\"/>") );
    QVERIFY(writeBytes(measurementsPath, damaged));

    PatientStore store;
    QList<PatientInfo> patients;
    QList<MeasurementRecord> measurements;
    QString error;
    QVERIFY(!store.load(patientsPath, measurementsPath, &patients, &measurements, &error));
    const QString backupPath = measurementsPath + QStringLiteral(".bad.bak");
    QCOMPARE(readBytes(measurementsPath), damaged);
    QCOMPARE(readBytes(backupPath), damaged);
    QVERIFY2(error.contains(backupPath), qPrintable(error));
}

void PatientStoreTests::semanticCorruptionIsRejected_data()
{
    QTest::addColumn<QByteArray>("patientsBytes");
    QTest::addColumn<QByteArray>("measurementsBytes");
    QTest::addColumn<QString>("badFileName");

    const QByteArray validPatients(
        "<patients version=\"2\"><patient id=\"patient-001\" name=\"测试患者\"/></patients>");

    QTest::newRow("unknown-patient-version")
        << QByteArray("<patients version=\"999\"/>")
        << QByteArray("<measurements version=\"1\"/>")
        << QStringLiteral("patients.xml");
    QTest::newRow("unknown-measurement-version")
        << validPatients
        << QByteArray("<measurements version=\"999\"/>")
        << QStringLiteral("measurements.xml");
    QTest::newRow("empty-patient-id")
        << QByteArray("<patients version=\"2\"><patient id=\"\"/></patients>")
        << QByteArray("<measurements version=\"1\"/>")
        << QStringLiteral("patients.xml");
    QTest::newRow("duplicate-patient-id")
        << QByteArray("<patients version=\"2\"><patient id=\"patient-001\"/>"
                      "<patient id=\"patient-001\"/></patients>")
        << QByteArray("<measurements version=\"1\"/>")
        << QStringLiteral("patients.xml");
    QTest::newRow("empty-measurement-id")
        << validPatients
        << QByteArray("<measurements version=\"1\"><measurement id=\"\" "
                      "patientId=\"patient-001\"/></measurements>")
        << QStringLiteral("measurements.xml");
    QTest::newRow("duplicate-measurement-id")
        << validPatients
        << QByteArray("<measurements version=\"1\">"
                      "<measurement id=\"measurement-001\" patientId=\"patient-001\"/>"
                      "<measurement id=\"measurement-001\" patientId=\"patient-001\"/>"
                      "</measurements>")
        << QStringLiteral("measurements.xml");
    QTest::newRow("orphan-measurement")
        << validPatients
        << QByteArray("<measurements version=\"1\"><measurement "
                      "id=\"measurement-001\" patientId=\"missing-patient\"/></measurements>")
        << QStringLiteral("measurements.xml");
}

void PatientStoreTests::semanticCorruptionIsRejected()
{
    QFETCH(QByteArray, patientsBytes);
    QFETCH(QByteArray, measurementsBytes);
    QFETCH(QString, badFileName);

    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString patientsPath = directory.filePath(QStringLiteral("patients.xml"));
    const QString measurementsPath = directory.filePath(QStringLiteral("measurements.xml"));
    QVERIFY(writeBytes(patientsPath, patientsBytes));
    QVERIFY(writeBytes(measurementsPath, measurementsBytes));

    PatientStore store;
    QList<PatientInfo> patients = {samplePatient()};
    QList<MeasurementRecord> measurements = {sampleMeasurement()};
    QString error;
    QVERIFY(!store.load(patientsPath, measurementsPath, &patients, &measurements, &error));

    const QString damagedPath = directory.filePath(badFileName);
    const QByteArray original = badFileName == QStringLiteral("patients.xml")
        ? patientsBytes : measurementsBytes;
    QCOMPARE(readBytes(damagedPath), original);
    QCOMPARE(readBytes(damagedPath + QStringLiteral(".bad.bak")), original);
    QVERIFY2(error.contains(QStringLiteral(".bad.bak")), qPrintable(error));
    QCOMPARE(patients.size(), 1);
    QCOMPARE(measurements.size(), 1);
}

void PatientStoreTests::pairedSaveAndDeleteKeepDocumentsConsistent()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString patientsPath = directory.filePath(QStringLiteral("patients.xml"));
    const QString measurementsPath = directory.filePath(QStringLiteral("measurements.xml"));
    PatientStore store;
    QString error;

    QVERIFY2(store.savePatientData(patientsPath, measurementsPath,
                                   {samplePatient()}, {sampleMeasurement()}, &error),
             qPrintable(error));
    verifyDocumentRoot(patientsPath, QStringLiteral("patients"), QStringLiteral("2"));
    verifyDocumentRoot(measurementsPath, QStringLiteral("measurements"), QStringLiteral("1"));

    QList<PatientInfo> patients;
    QList<MeasurementRecord> measurements;
    QVERIFY2(store.load(patientsPath, measurementsPath, &patients, &measurements, &error),
             qPrintable(error));
    QCOMPARE(patients.size(), 1);
    QCOMPARE(measurements.size(), 1);
    QCOMPARE(measurements.first().patientId, patients.first().id);

    QVERIFY2(store.savePatientData(patientsPath, measurementsPath, {}, {}, &error),
             qPrintable(error));
    patients.clear();
    measurements.clear();
    QVERIFY2(store.load(patientsPath, measurementsPath, &patients, &measurements, &error),
             qPrintable(error));
    QVERIFY(patients.isEmpty());
    QVERIFY(measurements.isEmpty());
    verifyDocumentRoot(patientsPath, QStringLiteral("patients"), QStringLiteral("2"));
    verifyDocumentRoot(measurementsPath, QStringLiteral("measurements"), QStringLiteral("1"));
    QVERIFY(!QFileInfo::exists(patientsPath + QStringLiteral(".txn")));
    QVERIFY(!QFileInfo::exists(patientsPath + QStringLiteral(".txn.bak")));
    QVERIFY(!QFileInfo::exists(measurementsPath + QStringLiteral(".txn.bak")));
}

void PatientStoreTests::versionlessLegacyStillMigrates()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString patientsPath = directory.filePath(QStringLiteral("patients.xml"));
    const QString measurementsPath = directory.filePath(QStringLiteral("measurements.xml"));
    const QByteArray legacyBytes(
        "<patients><patient id=\"patient-001\" name=\"旧档案\" gender=\"女\" "
        "birth=\"1990-01-02\" checkDate=\"2026-07-01\" sos=\"3999.9\" "
        "diag=\"旧记录\"/></patients>");
    QVERIFY(writeBytes(patientsPath, legacyBytes));
    QVERIFY(writeBytes(measurementsPath, "<measurements version=\"1\"/>"));

    PatientStore store;
    QList<PatientInfo> patients;
    QList<MeasurementRecord> measurements;
    QString error;
    QVERIFY2(store.load(patientsPath, measurementsPath, &patients, &measurements, &error),
             qPrintable(error));

    QCOMPARE(patients.size(), 1);
    QCOMPARE(measurements.size(), 1);
    QCOMPARE(measurements.first().patientId, QStringLiteral("patient-001"));
    QCOMPARE(measurements.first().sos, QStringLiteral("3999.9"));
    QCOMPARE(measurements.first().diagnosis, QStringLiteral("旧记录"));
    verifyDocumentRoot(patientsPath, QStringLiteral("patients"), QStringLiteral("2"));
    verifyDocumentRoot(measurementsPath, QStringLiteral("measurements"), QStringLiteral("1"));
    QCOMPARE(readBytes(patientsPath + QStringLiteral(".bak")), legacyBytes);
}

void PatientStoreTests::unfinishedTransactionIsRecoveredBeforeLoad()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString patientsPath = directory.filePath(QStringLiteral("patients.xml"));
    const QString measurementsPath = directory.filePath(QStringLiteral("measurements.xml"));
    const QString markerPath = patientsPath + QStringLiteral(".txn");
    PatientStore store;
    QString error;
    QVERIFY2(store.savePatientData(patientsPath, measurementsPath,
                                   {samplePatient()}, {sampleMeasurement()}, &error),
             qPrintable(error));
    const QByteArray originalPatients = readBytes(patientsPath);
    const QByteArray originalMeasurements = readBytes(measurementsPath);
    QVERIFY(QFile::copy(patientsPath, patientsPath + QStringLiteral(".txn.bak")));
    QVERIFY(QFile::copy(measurementsPath, measurementsPath + QStringLiteral(".txn.bak")));
    QVERIFY(writeTransactionMarker(markerPath, true, true));

    QVERIFY(writeBytes(patientsPath, "<patients version=\"2\"/>") );
    QList<PatientInfo> patients;
    QList<MeasurementRecord> measurements;
    QVERIFY2(store.load(patientsPath, measurementsPath, &patients, &measurements, &error),
             qPrintable(error));
    QCOMPARE(readBytes(patientsPath), originalPatients);
    QCOMPARE(readBytes(measurementsPath), originalMeasurements);
    QCOMPARE(patients.size(), 1);
    QCOMPARE(measurements.size(), 1);
    QVERIFY(!QFileInfo::exists(markerPath));
    QVERIFY(!QFileInfo::exists(patientsPath + QStringLiteral(".txn.bak")));
    QVERIFY(!QFileInfo::exists(measurementsPath + QStringLiteral(".txn.bak")));
}

void PatientStoreTests::missingTransactionBackupRefusesLoad()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString patientsPath = directory.filePath(QStringLiteral("patients.xml"));
    const QString measurementsPath = directory.filePath(QStringLiteral("measurements.xml"));
    const QString markerPath = patientsPath + QStringLiteral(".txn");
    PatientStore store;
    QString error;
    QVERIFY2(store.savePatientData(patientsPath, measurementsPath,
                                   {samplePatient()}, {sampleMeasurement()}, &error),
             qPrintable(error));
    QVERIFY(QFile::copy(patientsPath, patientsPath + QStringLiteral(".txn.bak")));
    QVERIFY(writeTransactionMarker(markerPath, true, true));
    const QByteArray patientsBeforeLoad = readBytes(patientsPath);
    const QByteArray measurementsBeforeLoad = readBytes(measurementsPath);

    QList<PatientInfo> patients = {samplePatient()};
    QList<MeasurementRecord> measurements = {sampleMeasurement()};
    QVERIFY(!store.load(patientsPath, measurementsPath, &patients, &measurements, &error));
    QVERIFY2(error.contains(QStringLiteral("检测记录事务备份")), qPrintable(error));
    QCOMPARE(readBytes(patientsPath), patientsBeforeLoad);
    QCOMPARE(readBytes(measurementsPath), measurementsBeforeLoad);
    QCOMPARE(patients.size(), 1);
    QCOMPARE(measurements.size(), 1);
    QVERIFY(QFileInfo::exists(markerPath));
    QVERIFY(QFileInfo::exists(patientsPath + QStringLiteral(".txn.bak")));
}

void PatientStoreTests::invalidTransactionBackupDoesNotOverwriteLiveFiles_data()
{
    QTest::addColumn<QString>("damagedBackupName");
    QTest::addColumn<QByteArray>("damagedBytes");
    QTest::newRow("truncated-patients-backup")
        << QStringLiteral("patients.xml.txn.bak")
        << QByteArray("<truncated");
    QTest::newRow("truncated-measurements-backup")
        << QStringLiteral("measurements.xml.txn.bak")
        << QByteArray("<truncated");
    QTest::newRow("duplicate-patient-id-backup")
        << QStringLiteral("patients.xml.txn.bak")
        << QByteArray("<patients version=\"2\"><patient id=\"backup\"/>"
                      "<patient id=\"backup\"/></patients>");
    QTest::newRow("orphan-measurement-backup")
        << QStringLiteral("measurements.xml.txn.bak")
        << QByteArray("<measurements version=\"1\"><measurement id=\"backup-record\" "
                      "patientId=\"missing\"/></measurements>");
}

void PatientStoreTests::invalidTransactionBackupDoesNotOverwriteLiveFiles()
{
    QFETCH(QString, damagedBackupName);
    QFETCH(QByteArray, damagedBytes);

    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString patientsPath = directory.filePath(QStringLiteral("patients.xml"));
    const QString measurementsPath = directory.filePath(QStringLiteral("measurements.xml"));
    const QString markerPath = patientsPath + QStringLiteral(".txn");
    const QByteArray livePatients("<patients version=\"2\"><patient id=\"live\"/></patients>");
    const QByteArray liveMeasurements(
        "<measurements version=\"1\"><measurement id=\"live-record\" patientId=\"live\"/>"
        "</measurements>");
    const QByteArray backupPatients(
        "<patients version=\"2\"><patient id=\"backup\"/></patients>");
    const QByteArray backupMeasurements(
        "<measurements version=\"1\"><measurement id=\"backup-record\" "
        "patientId=\"backup\"/></measurements>");

    QVERIFY(writeBytes(patientsPath, livePatients));
    QVERIFY(writeBytes(measurementsPath, liveMeasurements));
    QVERIFY(writeBytes(patientsPath + QStringLiteral(".txn.bak"), backupPatients));
    QVERIFY(writeBytes(measurementsPath + QStringLiteral(".txn.bak"), backupMeasurements));
    QVERIFY(writeBytes(directory.filePath(damagedBackupName), damagedBytes));
    QVERIFY(writeTransactionMarker(markerPath, true, true));

    const QByteArray damagedBackup = readBytes(directory.filePath(damagedBackupName));
    PatientStore store;
    QList<PatientInfo> patients;
    QList<MeasurementRecord> measurements;
    QString error;
    QVERIFY(!store.load(patientsPath, measurementsPath, &patients, &measurements, &error));

    QCOMPARE(readBytes(patientsPath), livePatients);
    QCOMPARE(readBytes(measurementsPath), liveMeasurements);
    QCOMPARE(readBytes(directory.filePath(damagedBackupName)), damagedBackup);
    QVERIFY(QFileInfo::exists(markerPath));
    QVERIFY(QFileInfo::exists(patientsPath + QStringLiteral(".txn.bak")));
    QVERIFY(QFileInfo::exists(measurementsPath + QStringLiteral(".txn.bak")));
    QVERIFY2(error.contains(QStringLiteral("事务备份")), qPrintable(error));
}

void PatientStoreTests::transactionRecoveryHandlesOriginalExistenceMatrix_data()
{
    QTest::addColumn<bool>("patientsExisted");
    QTest::addColumn<bool>("measurementsExisted");
    QTest::newRow("both-existed") << true << true;
    QTest::newRow("patients-only") << true << false;
    QTest::newRow("measurements-only") << false << true;
    QTest::newRow("neither-existed") << false << false;
}

void PatientStoreTests::transactionRecoveryHandlesOriginalExistenceMatrix()
{
    QFETCH(bool, patientsExisted);
    QFETCH(bool, measurementsExisted);

    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString patientsPath = directory.filePath(QStringLiteral("patients.xml"));
    const QString measurementsPath = directory.filePath(QStringLiteral("measurements.xml"));
    const QString markerPath = patientsPath + QStringLiteral(".txn");

    QVERIFY(writeBytes(patientsPath, "<patients version=\"2\"/>"));
    QVERIFY(writeBytes(measurementsPath, "<measurements version=\"1\"/>"));
    if (patientsExisted) {
        QVERIFY(writeBytes(patientsPath + QStringLiteral(".txn.bak"),
                           "<patients version=\"2\"><patient id=\"patient-001\"/></patients>"));
    }
    if (measurementsExisted) {
        const QByteArray backup = patientsExisted
            ? QByteArray("<measurements version=\"1\"><measurement "
                         "id=\"measurement-001\" patientId=\"patient-001\"/></measurements>")
            : QByteArray("<measurements version=\"1\"/>");
        QVERIFY(writeBytes(measurementsPath + QStringLiteral(".txn.bak"), backup));
    }
    QVERIFY(writeTransactionMarker(markerPath, patientsExisted, measurementsExisted));

    PatientStore store;
    QList<PatientInfo> patients;
    QList<MeasurementRecord> measurements;
    QString error;
    QVERIFY2(store.load(patientsPath, measurementsPath, &patients, &measurements, &error),
             qPrintable(error));

    QCOMPARE(QFileInfo::exists(patientsPath), patientsExisted);
    QCOMPARE(QFileInfo::exists(measurementsPath), measurementsExisted);
    QCOMPARE(patients.size(), patientsExisted ? 1 : 0);
    QCOMPARE(measurements.size(), patientsExisted && measurementsExisted ? 1 : 0);
    QVERIFY(!QFileInfo::exists(markerPath));
    QVERIFY(!QFileInfo::exists(patientsPath + QStringLiteral(".txn.bak")));
    QVERIFY(!QFileInfo::exists(measurementsPath + QStringLiteral(".txn.bak")));
}

QTEST_MAIN(PatientStoreTests)
#include "patientstore_tests.moc"
