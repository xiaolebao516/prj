// Unified data folder, parameter groups and import of data kept next to older programs.

namespace {

void writeDataTestFile(const QString& path, const QByteArray& content)
{
    QDir().mkpath(QFileInfo(path).absolutePath());
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
    file.write(content);
}

QJsonObject sampleLogConfig(double probeDistance, int gain)
{
    return QJsonObject{{"implementation", "onset-consistency-20260915-v1"},
                       {"probe_distance_m", probeDistance}, {"frame_corr_A", 0.78},
                       {"G_max", 0.0}, {"round_target", 5},
                       {"gain_BC", gain}, {"measurement_session_id", "s-1"},
                       {"subject", QJsonObject{{"archive_id", "P1"}, {"name", "测试"}}}};
}

QByteArray sampleLog(const QJsonObject& config, const QString& utc)
{
    return QJsonDocument(QJsonObject{{"event", "start"}, {"schema", 1}, {"config", config},
                                     {"utc", utc}}).toJson(QJsonDocument::Compact)
        + "\n{\"event\":\"frame\"}\n";
}

} // namespace

void MainWindowSafetyTests::dataFolderIsSharedByEveryBuild()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QDir base(directory.path());
    writeDataTestFile(base.filePath("app/BoneDensity.pro"), "TEMPLATE = app\n");
    QVERIFY(base.mkpath("app/build/debug/debug"));
    QVERIFY(base.mkpath("app/build/Desktop_Qt_6_5_3_MinGW_64_bit_Debug/debug"));
    const QString shared = QDir::cleanPath(base.filePath("app/BoneDensityData"));

    // Script build and Qt Creator build land on the same folder.
    QCOMPARE(DataLocation::resolve(base.filePath("app/build/debug/debug"), QString()), shared);
    QCOMPARE(DataLocation::resolve(base.filePath("app/build/Desktop_Qt_6_5_3_MinGW_64_bit_Debug/debug"),
                                   QString()), shared);
    // An installed package keeps its data next to the program.
    QVERIFY(base.mkpath("package/BoneDensityData"));
    QCOMPARE(DataLocation::resolve(base.filePath("package"), QString()),
             QDir::cleanPath(base.filePath("package/BoneDensityData")));
    QCOMPARE(DataLocation::resolve(base.filePath("elsewhere"), QString()),
             QDir::cleanPath(base.filePath("elsewhere/BoneDensityData")));
    QCOMPARE(DataLocation::resolve(base.filePath("app/build/debug/debug"), base.filePath("custom")),
             QDir::cleanPath(base.filePath("custom")));

    // This test run uses its own throw-away folder.
    QCOMPARE(DataLocation::root(), QDir::cleanPath(dataRoot.path()));
    MainWindow window;
    window.hide();
    QCOMPARE(QFileInfo(window.xmlFilePath).absolutePath(), QDir::cleanPath(dataRoot.path()));
    QCOMPARE(QFileInfo(window.accountsFilePath).absolutePath(), QDir::cleanPath(dataRoot.path()));
    QVERIFY(window.deviceSettingsPath.startsWith(QDir(dataRoot.path()).filePath("settings")));
}

void MainWindowSafetyTests::parameterGroupsDescribeTheirData()
{
    const QJsonObject base = sampleLogConfig(0.0049, 1024);
    // Gains, patient and session do not split groups; D does.
    QCOMPARE(ParameterGroup::id(base), ParameterGroup::id(sampleLogConfig(0.0049, 600)));
    QVERIFY(ParameterGroup::id(base) != ParameterGroup::id(sampleLogConfig(0.0050, 1024)));
    QVERIFY(ParameterGroup::id(base).startsWith("onset-consistency-20260915-v1_"));
    QVERIFY(!ParameterGroup::parameters(base).contains("gain_BC"));
    QVERIFY(!ParameterGroup::parameters(base).contains("subject"));

    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QDateTime later(QDate(2026, 10, 9), QTime(15, 0));
    const QDateTime earlier(QDate(2026, 9, 21), QTime(9, 30));
    const QString folder = ParameterGroup::prepareFolder(directory.path(), base, later, true);
    QVERIFY(!folder.isEmpty());
    QCOMPARE(ParameterGroup::prepareFolder(directory.path(), sampleLogConfig(0.0049, 7), earlier, true), folder);
    QJsonObject anotherRound = sampleLogConfig(0.0049, 7);
    anotherRound.insert("measurement_session_id", "s-2");
    QCOMPARE(ParameterGroup::prepareFolder(directory.path(), anotherRound, earlier, true), folder);
    QCOMPARE(ParameterGroup::prepareFolder(directory.path(), base, later, false), folder);

    QFile json(QDir(folder).filePath("parameters.json"));
    QVERIFY(json.open(QIODevice::ReadOnly));
    const QJsonObject info = QJsonDocument::fromJson(json.readAll()).object();
    QCOMPARE(info.value("group_id").toString(), ParameterGroup::id(base));
    // Three round logs from two measurement sessions (s-1 twice, s-2 once).
    QCOMPARE(info.value("sessions").toInt(), 2);
    QCOMPARE(info.value("logs").toInt(), 3);
    QCOMPARE(info.value("first_used_utc").toString(), earlier.toUTC().toString(Qt::ISODate));
    QCOMPARE(info.value("last_used_utc").toString(), later.toUTC().toString(Qt::ISODate));
    QCOMPARE(info.value("parameters").toObject().value("probe_distance_m").toDouble(), 0.0049);

    QFile text(QDir(folder).filePath("参数说明.txt"));
    QVERIFY(text.open(QIODevice::ReadOnly));
    const QString description = QString::fromUtf8(text.readAll());
    QVERIFY(description.contains("探头距离 D (m)：0.0049"));
    QVERIFY(description.contains("算法版本：onset-consistency-20260915-v1"));
    QVERIFY(description.contains("检测次数：2（实验日志 3 份"));
}

void MainWindowSafetyTests::resultsRecordTheirParameterGroup()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    MainWindow window;
    PREPARE_ARCHIVE_WINDOW(window, directory);
    window.currentPatient = samplePatient();
    window.patientList = {window.currentPatient};
    QVERIFY(window.savePatients(window.patientList));
    window.session.roundSos = {3910.0, 3956.9, 3946.3, 3951.6, 3983.7};
    window.session.roundA = {10, 20, 30, 40, 60};
    window.session.roundB = {100, 200, 300, 400, 600};
    window.finishAllPatientRounds();
    QCOMPARE(window.measurementList.size(), 1);
    const QString group = ParameterGroup::id(measurementParameters(window.mCfg, window.profile, window.signalProcessor));
    QCOMPARE(window.measurementList.first().parameterGroup, group);
    QVERIFY(QFileInfo::exists(QDir(DataLocation::experimentsRoot()).filePath(group + "/参数说明.txt")));

    // The group id survives saving and loading.
    PatientStore store;
    QList<PatientInfo> patients;
    QList<MeasurementRecord> records;
    QVERIFY(store.load(window.xmlFilePath, window.measurementsFilePath, &patients, &records));
    QCOMPARE(records.size(), 1);
    QCOMPARE(records.first().parameterGroup, group);
}

void MainWindowSafetyTests::legacyDataIsMergedWithoutMixingPeople()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QDir base(directory.path());
    const QString legacy = base.filePath("old-program");
    const QString root = base.filePath("BoneDensityData");
    QVERIFY(base.mkpath("old-program"));
    QVERIFY(base.mkpath("BoneDensityData"));
    PatientStore store;

    // Data folder: already holds P2 = 王某.
    PatientInfo kept = samplePatient();
    kept.id = "P2";
    kept.name = "王某";
    MeasurementRecord keptRecord = sampleMeasurement();
    keptRecord.id = "r-root";
    keptRecord.patientId = "P2";
    QVERIFY(store.savePatientData(QDir(root).filePath("patients.xml"), QDir(root).filePath("measurements.xml"),
                                  {kept}, {keptRecord}));
    writeDataTestFile(QDir(root).filePath("calibration.xml"), "<root-calibration/>");

    // Old folder: P1 (new), P2 = a different person, P3 identical to nothing.
    PatientInfo p1 = samplePatient();
    p1.id = "P1";
    p1.name = "李某";
    PatientInfo other = samplePatient();
    other.id = "P2";
    other.name = "张某";
    MeasurementRecord r1 = sampleMeasurement();
    r1.id = "r-1";
    r1.patientId = "P1";
    MeasurementRecord r2 = sampleMeasurement();
    r2.id = "r-2";
    r2.patientId = "P2";
    QVERIFY(store.savePatientData(QDir(legacy).filePath("patients.xml"),
                                  QDir(legacy).filePath("measurements.xml"), {p1, other}, {r1, r2}));
    writeDataTestFile(QDir(legacy).filePath("accounts.xml"), "<accounts/>");
    writeDataTestFile(QDir(legacy).filePath("calibration.xml"), "<old-calibration/>");
    writeDataTestFile(QDir(legacy).filePath("measurement-guide.ini"), "[guide]\n");
    const QJsonObject config = sampleLogConfig(0.0049, 1024);
    writeDataTestFile(QDir(legacy).filePath("measurement-experiments/round-a.jsonl"),
                      sampleLog(config, "2026-09-20T03:00:00.000Z"));
    writeDataTestFile(QDir(legacy).filePath("measurement-experiments/notes.zip"), "zip");

    QVERIFY(LegacyImport::hasLegacyData(legacy));
    const LegacyImport::Result result = LegacyImport::importFrom(legacy, root);
    QVERIFY2(result.ok, qPrintable(result.error + result.notes.join('\n')));
    QVERIFY(result.performed);
    QCOMPARE(result.patientsAdded, 1);
    QCOMPARE(result.patientConflicts, 1);
    QCOMPARE(result.measurementsAdded, 1);
    QCOMPARE(result.measurementsLeftOut, 1);
    QCOMPARE(result.logsCopied, 1);
    QVERIFY(result.notes.join('\n').contains("P2"));
    QVERIFY(result.notes.join('\n').contains("校准参数"));

    QList<PatientInfo> patients;
    QList<MeasurementRecord> records;
    QVERIFY(store.load(QDir(root).filePath("patients.xml"), QDir(root).filePath("measurements.xml"),
                       &patients, &records));
    QCOMPARE(patients.size(), 2);
    for (const PatientInfo& patient : patients) {
        if (patient.id == "P2") QCOMPARE(patient.name, QStringLiteral("王某"));   // not overwritten
    }
    QStringList ids;
    for (const MeasurementRecord& record : records) ids << record.id;
    ids.sort();
    QCOMPARE(ids, QStringList({"r-1", "r-root"}));   // 张某's record stays behind

    QVERIFY(QFileInfo::exists(QDir(root).filePath("accounts.xml")));
    QCOMPARE(readBackupTestFile(QDir(root).filePath("calibration.xml")), QByteArray("<root-calibration/>"));
    QVERIFY(QFileInfo::exists(QDir(root).filePath("settings/measurement-guide.ini")));
    const QString group = QDir(root).filePath("experiments/" + ParameterGroup::id(config));
    QVERIFY(QFileInfo::exists(group + "/round-a.jsonl"));
    QVERIFY(QFileInfo::exists(group + "/参数说明.txt"));
    QVERIFY(QFileInfo::exists(QDir(root).filePath("experiments/未分组/notes.zip")));
    QVERIFY(QFileInfo::exists(result.reportPath));

    // The old folder is left in place, marked, and not imported again.
    QVERIFY(QFileInfo::exists(QDir(legacy).filePath("patients.xml")));
    QVERIFY(QFileInfo::exists(QDir(legacy).filePath(LegacyImport::kMarkerName)));
    QVERIFY(!LegacyImport::hasLegacyData(legacy));
    QVERIFY(!LegacyImport::importFrom(legacy, root).performed);
}
