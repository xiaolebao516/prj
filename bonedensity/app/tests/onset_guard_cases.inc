// Included after the MainWindowSafetyTests declaration; uses its existing friend access.
namespace {
QVector<QJsonObject> onsetReadRows(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) qFatal("Cannot open replay input");
    QVector<QJsonObject> rows;
    for (const auto& line:file.readAll().split('\n')) {
        if (line.trimmed().isEmpty()) continue;
        QJsonParseError error;
        auto doc=QJsonDocument::fromJson(line,&error);
        if(error.error!=QJsonParseError::NoError || !doc.isObject()) qFatal("Invalid replay JSON");
        rows.append(doc.object());
    }
    return rows;
}
QVector<quint16> onsetRaw(const QJsonObject& row,const char* key) {
    const auto data=QByteArray::fromBase64Encoding(row[key].toString().toLatin1(),QByteArray::AbortOnBase64DecodingErrors);
    if (!data || data.decoded.size()!=4000) qFatal("Invalid raw waveform");
    QVector<quint16> raw(2000);
    for(int i=0;i<2000;++i) raw[i]=qFromLittleEndian<quint16>(data.decoded.constData()+2*i);
    return raw;
}
bool onsetJsonEqual(const QJsonValue& a,const QJsonValue& b) {
    if(a.type()!=b.type()) return false;
    if(a.isDouble()) return std::abs(a.toDouble()-b.toDouble())<1e-8;
    if(a.isObject()) {
        const auto x=a.toObject(),y=b.toObject();
        if(x.keys()!=y.keys())return false;
        for(const auto& k:x.keys())if(!onsetJsonEqual(x[k],y[k]))return false;
        return true;
    }
    if(a.isArray()) {
        const auto x=a.toArray(),y=b.toArray();if(x.size()!=y.size())return false;
        for(int i=0;i<x.size();++i)if(!onsetJsonEqual(x[i],y[i]))return false;
        return true;
    }
    return a==b;
}
}

void MainWindowSafetyTests::onsetGuardRejectsRecordedLowFrame() {
    const auto rows=onsetReadRows(QFINDTESTDATA("../testdata/onset-consistency/low-1984.jsonl"));
    const auto r=rows.last();QTemporaryDir dir;QVERIFY(dir.isValid());
    MainWindow w;w.patientMeasureRunning=true;w.acquireMode=PatientMeasureMode;
    w.profile.enforceBOnsetConsistency=true; // the guard itself, also in trial builds that disable it
    QVERIFY(w.experimentLog.start(dir.path(),{}));const QString log=w.experimentLog.path();
    w.currentFrame.bc=onsetRaw(r,"raw_BC");w.currentFrame.bd=onsetRaw(r,"raw_BD");
    w.currentFrame.ac=onsetRaw(r,"raw_AC");w.currentFrame.ad=onsetRaw(r,"raw_AD");
    for(int i=0;i<15;++i)w.plotSamples();
    QCOMPARE(w.session.validCount(),0);QVERIFY(w.session.currentRound.sos.isEmpty());
    w.stopPatientMeasurement();
    const auto actual=onsetReadRows(log);int frames=0;
    for(const auto& x:actual)if(x["event"]=="frame") {
        ++frames;QCOMPARE(x["decision"].toString(),QString("B_onset_inconsistent"));
        const auto bc=x["B_arrivals"].toObject()["BC"].toObject();
        QCOMPARE(bc["first_hit"].toInt(),1227);QCOMPARE(bc["onset"].toInt(),1333);
        QCOMPARE(bc["forward_shift"].toInt(),106);
    }
    QCOMPARE(frames,15);
    QVERIFY(w.lblCorrAValue->toolTip().isEmpty() || !w.lblCorrAValue->toolTip().contains("0.958"));
}

void MainWindowSafetyTests::onsetGuardMatchesRecordedBatch() {
    const QString source=qEnvironmentVariable("BONE_ONSET_REPLAY_INPUT");
    const QString expectedRoot=qEnvironmentVariable("BONE_ONSET_REPLAY_EXPECTED");
    if(source.isEmpty() || expectedRoot.isEmpty())QSKIP("Set onset replay input/expected paths for 75-log acceptance");
    int files=0,frames=0;QJsonArray evidence;
    for(const QString& date:{QString("20260914"),QString("20260915")}) {
        QFile expectedFile(QDir(expectedRoot).filePath("wave-audit-"+date+"/state-replay.json"));
        QVERIFY(expectedFile.open(QIODevice::ReadOnly));
        auto expected=QJsonDocument::fromJson(expectedFile.readAll()).object();
        const auto baseline=expected["baseline"].toArray();
        const auto candidate=expected["scenarios"].toObject()["40"].toObject()["cases"].toArray();
        for(int f=0;f<baseline.size();++f) {
            const QString name=baseline[f].toObject()["file"].toString();
            const auto rows=onsetReadRows(QDir(source).filePath(name));
            const auto c=rows.first()["config"].toObject();
            for(bool guard:{false,true}) {
                QTemporaryDir dir;QVERIFY(dir.isValid());MainWindow w;
                w.profile.enforceBOnsetConsistency=guard;
                w.mCfg.frameCorrAMin=c["frame_corr_A"].toDouble();
                w.mCfg.frameCorrBMin=c["frame_corr_B"].toDouble();
                w.mCfg.roundCorrAMin=c["round_corr_A"].toDouble();
                w.mCfg.roundCorrBMin=c["round_corr_B"].toDouble();
                w.mCfg.anglePairMidGapMin=c["G_min"].toDouble();w.mCfg.anglePairMidGapMax=c["G_max"].toDouble();
                w.signalProcessor.probeDistanceCD=c["probe_distance_m"].toDouble();
                w.signalProcessor.samplePeriod=c["sample_period_s"].toDouble();
                w.patientMeasureRunning=true;w.acquireMode=PatientMeasureMode;
                QVERIFY(w.experimentLog.start(dir.path(),{}));const QString path=w.experimentLog.path();
                QVector<QJsonObject> originals;
                for(const auto& r:rows) {
                    if(r["event"]!="frame")continue;
                    QVERIFY2(w.patientMeasureRunning,qPrintable(name+" premature completion"));
                    originals.append(r);
                    w.currentFrame.bc=onsetRaw(r,"raw_BC");w.currentFrame.bd=onsetRaw(r,"raw_BD");
                    w.currentFrame.ac=onsetRaw(r,"raw_AC");w.currentFrame.ad=onsetRaw(r,"raw_AD");
                    w.plotSamples();
                }
                w.stopPatientMeasurement();w.closeRoundFinishedTip();
                const auto actual=onsetReadRows(path);int i=0;QJsonArray accepted;QJsonObject summary;
                qint64 completed=-1;
                for(const auto& r:actual) {
                    if(r["event"]=="frame") {
                        QVERIFY(i<originals.size());const auto original=originals[i++];
                        if(!guard) {
                            for(const QString& key:{QString("decision"),QString("B"),QString("A"),QString("D"),QString("G"),QString("gates"),QString("locked"),QString("locked_lag"),QString("partial_values_before_accept"),QString("sos_patient")})
                                QVERIFY2(onsetJsonEqual(r[key],original[key]),qPrintable(name+" seq "+QString::number(original["sequence"].toInt())+" "+key));
                        }
                        if(r["decision"]=="accepted") accepted.append(QJsonObject{{"sequence",original["sequence"]},{"sos",r["sos_patient"]}});
                        completed=original["elapsed_ms"].toInteger();
                    } else if(r["event"]=="round_summary") {
                        for(const QString& k:{QString("sos"),QString("corr_A"),QString("corr_B"),QString("D"),QString("G"),QString("quality_pass")}) summary[k]=r[k];
                    }
                }
                QCOMPARE(i,originals.size());
                const auto target=(guard?candidate[f]:baseline[f]).toObject();
                QVERIFY2(onsetJsonEqual(accepted,target["accepted"]),qPrintable(name+" acceptance sequence"));
                QVERIFY2(onsetJsonEqual(summary.isEmpty()?QJsonValue(QJsonValue::Null):QJsonValue(summary),target["summary"]),qPrintable(name+" summary"));
                if(!summary.isEmpty())QCOMPARE(completed,target["completion_ms"].toInteger());
                if(guard){++files;frames+=i;evidence.append(QJsonObject{{"file",name},{"frames",i},{"accepted",accepted.size()},{"summary",summary}});}
            }
        }
    }
    QCOMPARE(files,75);QCOMPARE(frames,18362);
    const QString output=qEnvironmentVariable("BONE_ONSET_REPLAY_OUTPUT");
    if(!output.isEmpty()){QFile f(output);QVERIFY(f.open(QIODevice::WriteOnly));QVERIFY(f.write(QJsonDocument(evidence).toJson())>0);}
}
