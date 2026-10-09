// Anonymous identities only; the test executable lives in an isolated directory.
void MainWindowSafetyTests::experimentSubjectSnapshotAndSessions()
{
#ifdef QT_NO_DEBUG
    QSKIP("Experiment recording is Debug-only");
#else
    MainWindow window;
    window.hide();
    delete window.serial;
    auto* port = new FakeOpenSerialPort(&window);
    port->openForTest();
    window.serial = port;
    window.currentPatient = samplePatient();
    window.currentPatient.name = QStringLiteral("试测甲/同名\"\n");
    const QString originalName = window.currentPatient.name;
    const auto readConfig = [](const QString& path) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) return QJsonObject();
        return QJsonDocument::fromJson(file.readLine()).object()["config"].toObject();
    };

    window.startPatientMeasurement(false);
    QVERIFY(window.experimentLog.active());
    const QString firstPath = window.experimentLog.path();
    window.stopPatientMeasurement();
    const auto first = readConfig(firstPath);
    QCOMPARE(first["recording_profile"].toString(), QStringLiteral("subject-linked-20260918-v1"));
    const auto subject = first["subject"].toObject();
    QCOMPARE(subject["archive_id"].toString(), window.currentPatient.id);
    QCOMPARE(subject["name"].toString(), originalName);
    QCOMPARE(subject.size(), 2); // No birth date, gender, body measurements or accounts.
    const QString session = first["measurement_session_id"].toString();
    QVERIFY(!QUuid(session).isNull());
    QVERIFY(!QFileInfo(firstPath).fileName().contains(QStringLiteral("试测甲")));

    // Retry the interrupted first round: same session and frozen identity.
    window.currentPatient.name = QStringLiteral("试测甲已更名");
    window.startPatientMeasurement(false);
    const QString retryPath = window.experimentLog.path();
    window.stopPatientMeasurement();
    const auto retry = readConfig(retryPath);
    QCOMPARE(retry["measurement_session_id"].toString(), session);
    QCOMPARE(retry["subject"].toObject(), subject);
    QVERIFY(retryPath != firstPath);

    // Exercise the real automatic next-round timer, without real serial hardware.
    window.session.roundSos = {3800.0};
    window.patientDataWritable = true;
    window.scheduleNextPatientRound(1);
    window.nextRoundTimer.setInterval(1);
    QTRY_VERIFY(window.patientMeasureRunning);
    const QString nextPath = window.experimentLog.path();
    window.stopPatientMeasurement();
    const auto next = readConfig(nextPath);
    QCOMPARE(next["round"].toInt(), 2);
    QCOMPARE(next["measurement_session_id"].toString(), session);
    QCOMPARE(next["subject"].toObject(), subject);

    // A new measurement after five completed rounds gets a new snapshot/ID.
    window.session.roundSos = {3800,3800,3800,3800,3800};
    window.startPatientMeasurement(false);
    const QString completedPath = window.experimentLog.path();
    window.stopPatientMeasurement();
    const auto completed = readConfig(completedPath);
    const QString secondSession = completed["measurement_session_id"].toString();
    QVERIFY(!QUuid(secondSession).isNull());
    QVERIFY(secondSession != session);
    QCOMPARE(completed["subject"].toObject()["name"].toString(), window.currentPatient.name);

    // Explicit restart and switching to a different archive with the same name.
    window.resetAllPatientMeasurementData();
    window.startPatientMeasurement(false);
    const QString resetPath = window.experimentLog.path();
    window.stopPatientMeasurement();
    const QString resetSession = readConfig(resetPath)["measurement_session_id"].toString();
    QVERIFY(!QUuid(resetSession).isNull());
    QVERIFY(resetSession != secondSession);
    PatientInfo other = window.currentPatient;
    other.id = QStringLiteral("patient-002");
    QVERIFY(window.selectCurrentPatient(other));
    window.startPatientMeasurement(false);
    const QString otherPath = window.experimentLog.path();
    window.stopPatientMeasurement();
    const auto otherConfig = readConfig(otherPath);
    QVERIFY(otherConfig["measurement_session_id"].toString() != resetSession);
    QCOMPARE(otherConfig["subject"].toObject()["archive_id"].toString(), other.id);
    QCOMPARE(otherConfig["subject"].toObject()["name"].toString(), other.name);
    QCOMPARE(readConfig(firstPath), first); // Later renames cannot relabel old logs.
    QCOMPARE(first["implementation"].toString(), QStringLiteral("onset-consistency-20260915-v1"));
    QCOMPARE(first["G_min"].toDouble(), -12.0);
    QCOMPARE(first["G_max"].toDouble(), 0.0);
#endif
}
