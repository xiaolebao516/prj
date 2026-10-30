// Data backup cases: rolling copies of the runtime XML files.

namespace {

void writeBackupTestFile(const QString& path, const QByteArray& content)
{
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
    file.write(content);
}

QByteArray readBackupTestFile(const QString& path)
{
    QFile file(path);
    return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}

} // namespace

void MainWindowSafetyTests::dataBackupSnapshotsOnceAndPrunes()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString patients = directory.filePath(QStringLiteral("patients.xml"));
    const QString measurements = directory.filePath(QStringLiteral("measurements.xml"));
    const QString missing = directory.filePath(QStringLiteral("missing.xml"));
    const QString root = directory.filePath(QStringLiteral("backups"));
    writeBackupTestFile(patients, "p1");
    writeBackupTestFile(measurements, "m1");

    // Nothing to back up yet: no folder is created.
    const DataBackup::Result empty = DataBackup::snapshot(root, {missing}, QStringLiteral("2026-10-01"));
    QVERIFY(empty.ok && !empty.created);
    QVERIFY(!QFileInfo::exists(QDir(root).filePath(QStringLiteral("2026-10-01"))));

    const DataBackup::Result first =
        DataBackup::snapshot(root, {patients, measurements, missing}, QStringLiteral("2026-10-01"));
    QVERIFY(first.ok && first.created);
    QCOMPARE(readBackupTestFile(QDir(first.folder).filePath(QStringLiteral("patients.xml"))), QByteArray("p1"));
    QCOMPARE(readBackupTestFile(QDir(first.folder).filePath(QStringLiteral("measurements.xml"))), QByteArray("m1"));
    QVERIFY(!QFileInfo::exists(QDir(first.folder).filePath(QStringLiteral("missing.xml"))));

    // A label is taken once: later changes never overwrite that copy.
    writeBackupTestFile(patients, "p2");
    const DataBackup::Result again = DataBackup::snapshot(root, {patients}, QStringLiteral("2026-10-01"));
    QVERIFY(again.ok && !again.created);
    QCOMPARE(readBackupTestFile(QDir(first.folder).filePath(QStringLiteral("patients.xml"))), QByteArray("p1"));

    QCOMPARE(DataBackup::dailyLabel(QDate(2026, 10, 9)), QStringLiteral("2026-10-09"));
    QCOMPARE(DataBackup::beforeDeleteLabel(QDateTime(QDate(2026, 10, 9), QTime(14, 30, 15))),
             QStringLiteral("2026-10-09_143015_before-delete"));

    // Pruning keeps the newest of each kind, drops interrupted copies and
    // leaves folders it did not create alone.
    for (int day = 1; day <= 5; ++day) {
        QVERIFY(DataBackup::snapshot(root, {patients}, DataBackup::dailyLabel(QDate(2026, 9, day))).created);
        QVERIFY(DataBackup::snapshot(root, {patients},
                                     DataBackup::beforeDeleteLabel(QDateTime(QDate(2026, 9, day), QTime(8, 0))))
                    .created);
    }
    QDir rootDir(root);
    QVERIFY(rootDir.mkpath(QStringLiteral("keep-me")));
    QVERIFY(rootDir.mkpath(QStringLiteral("2026-10-02.partial")));
    DataBackup::prune(root, 3, 2);
    QStringList remaining = rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    remaining.sort();
    QCOMPARE(remaining, QStringList({QStringLiteral("2026-09-04"),
                                     QStringLiteral("2026-09-04_080000_before-delete"),
                                     QStringLiteral("2026-09-05"),
                                     QStringLiteral("2026-09-05_080000_before-delete"),
                                     QStringLiteral("2026-10-01"),
                                     QStringLiteral("keep-me")}));
}

void MainWindowSafetyTests::deletionsAreBackedUpFirst()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    PREPARE_ARCHIVE_WINDOW(window, directory);
    const PatientInfo patient = samplePatient();
    const MeasurementRecord record = sampleMeasurement();
    window.patientList = {patient};
    QVERIFY(window.savePatients(window.patientList));
    QVERIFY(window.saveMeasurements({record}));
    window.measurementList = {record};

    // The daily copy lives next to the data files, not next to the executable.
    window.backupDataDaily();
    const QDir backups(window.backupRoot());
    QCOMPARE(QFileInfo(window.backupRoot()).absolutePath(), QFileInfo(directory.path()).absoluteFilePath());
    const QString daily = backups.filePath(DataBackup::dailyLabel(QDate::currentDate()));
    QVERIFY(QFileInfo::exists(QDir(daily).filePath(QStringLiteral("patients.xml"))));
    QVERIFY(QFileInfo::exists(QDir(daily).filePath(QStringLiteral("measurements.xml"))));

    // Deleting a record first snapshots the data as it was.
    QVERIFY(window.deleteMeasurementRecord(record.id));
    QVERIFY(window.measurementList.isEmpty());
    const QStringList snapshots =
        backups.entryList({QStringLiteral("*_before-delete")}, QDir::Dirs | QDir::NoDotAndDotDot);
    QCOMPARE(snapshots.size(), 1);
    const QByteArray before = readBackupTestFile(
        QDir(backups.filePath(snapshots.first())).filePath(QStringLiteral("measurements.xml")));
    QVERIFY(before.contains(record.id.toUtf8()));
    QVERIFY(!readBackupTestFile(window.measurementsFilePath).contains(record.id.toUtf8()));
}
