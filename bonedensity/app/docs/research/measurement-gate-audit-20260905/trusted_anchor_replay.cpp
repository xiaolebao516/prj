// Research only: independently reconstructed A candidate, original B and product gates.
#define MEASUREMENT_REPLAY_LIBRARY
#include "offline_replay.cpp"

static QVector<QJsonObject> prefixRows(const QString& path, const QString& expectedHash, bool& damaged)
{
    QFile file(path); require(file.open(QIODevice::ReadOnly),"Cannot read source");
    QCryptographicHash hash(QCryptographicHash::Sha256);
    require(hash.addData(&file),"Hash failed");
    require(QString::fromLatin1(hash.result().toHex())==expectedHash,"Changed input");
    file.seek(0); QVector<QJsonObject> rows; damaged=false;
    while(!file.atEnd()) {
        const auto line=file.readLine(1024*1024);
        if(line.contains('\0')||!line.endsWith('\n')){damaged=true;break;}
        QJsonParseError error;auto row=QJsonDocument::fromJson(line,&error).object();
        require(error.error==QJsonParseError::NoError&&!row.isEmpty(),"Bad complete JSON line");
        for(const auto* key:{"raw_AD","raw_AC","raw_BD","raw_BC"})row.remove(key);
        rows.append(row);
    }
    require(!rows.isEmpty()&&rows.first()["event"]=="start","Missing start");
    return rows;
}

int main(int argc,char** argv)
{
    QApplication app(argc,argv);
    qInstallMessageHandler([](QtMsgType,const QMessageLogContext&,const QString&){});
    try {
        require(argc==4,"Usage: trusted_anchor_replay full-raw-audit.json input-directory output.json (QT_QPA_PLATFORM=offscreen)");
        QFile input(QString::fromLocal8Bit(argv[1])); require(input.open(QIODevice::ReadOnly),"Cannot read audit");
        const auto files=QJsonDocument::fromJson(input.readAll()).object()["files"].toArray();
        require(files.size()==63,"Expected all63 afternoon logs"); QJsonArray output;
        const Case baseline{"baseline",.78,.78,-6,6,14,10,true,true};
        auto trial=baseline;trial.name="trusted-one-step";
        for(const auto& value:files) {
            const auto entry=value.toObject(); bool damaged=false;
            const auto original=prefixRows(QDir(QString::fromLocal8Bit(argv[2])).filePath(entry["file"].toString()),entry["sha256"].toString(),damaged);
            QMap<int,QJsonObject> probes;
            for(const auto& v:entry["frames"].toArray())probes.insert(v.toObject()["sequence"].toInt(),v.toObject());
            QJsonObject result{{"file",entry["file"]},{"sha256",entry["sha256"]},{"damaged_prefix_only",damaged},
                {"baseline",MainWindowSafetyTests::runFile(original,baseline,false,true)}};
            auto rows=original;int checked=0;QJsonArray changes;
            for(auto& row:rows) {
                if(row["event"]!="frame"||!row.contains("gates"))continue;
                const int seq=row["sequence"].toInt();require(probes.contains(seq),"Missing raw-reconstructed gate frame");
                const auto probe=probes[seq];++checked;
                require(equivalent(row["A"],probe["A"])&&equivalent(row["B"],probe["B"])&&equivalent(row["G"],probe["G"]),"Cached/raw identity mismatch");
                const auto candidate=probe["one_step_trusted_anchor"].toObject();
                // Preserve the normal passing path. Rescued frames are never used as future seeds:
                // every probe was computed from the previous ORIGINAL frame, not this modified copy.
                if(row["gates"].toObject()["all_prechecks_passed"].toBool() ||
                   !candidate["continuity_eligible"].toBool() || candidate["A_quality"].toDouble()<.78)continue;
                auto a=row["A"].toObject();const auto b=row["B"].toObject();
                const int lag=candidate["A_lag"].toInt();require(lag>0,"Invalid candidate lag");
                a["lag"]=lag;a["early_feature"]=candidate["early"];a["late_feature"]=candidate["late"];
                a["rough_lag"]=candidate["late"].toInt()-candidate["early"].toInt();
                a["corr"]=candidate["A_quality"];
                const auto config=original.first()["config"].toObject();
                a["sos"]=config["probe_distance_m"].toDouble()/(lag*config["sample_period_s"].toDouble());
                row["A"]=a;row["D"]=candidate["D"];row["G"]=candidate["G"];
                auto gates=row["gates"].toObject();const int d=lag-b["lag"].toInt();
                require(d==candidate["D"].toInt(),"Candidate D inconsistency");
                gates["corr_A"]=a["corr"].toDouble()>=.78;gates["AB_diff"]=std::abs(d)<=18;
                gates["direction"]=d>=-2;gates["D"]=d>=5&&d<=15;
                gates["G"]=row["G"].toDouble()>=-6&&row["G"].toDouble()<=6;
                bool beforeG=true;
                for(const auto* key:{"B_jump","boundary","AB_diff","direction","corr_A","corr_B","D"})beforeG=beforeG&&gates[key].toBool();
                gates["stability_evaluated"]=beforeG;gates["all_prechecks_passed"]=beforeG&&gates["G"].toBool();row["gates"]=gates;
                require(equivalent(row["B"],probe["B"]),"B changed");
                changes.append(QJsonObject{{"sequence",seq},{"elapsed_ms",row["elapsed_ms"]},{"candidate",candidate},{"new_prechecks",gates["all_prechecks_passed"]},{"sos",row["sos_patient"]}});
            }
            require(checked==probes.size(),"Unused raw gate frames");
            result["gate_frames_verified"]=checked;result["changes"]=changes;
            result["candidate"]=MainWindowSafetyTests::runFile(rows,trial,false,true);
            output.append(result);std::fprintf(stderr,"Replayed %d/63: %d candidate feature changes\n",int(output.size()),int(changes.size()));
        }
        QFile out(QString::fromLocal8Bit(argv[3]));require(out.open(QIODevice::WriteOnly|QIODevice::NewOnly),"Output must be new");
        const auto bytes=QJsonDocument(QJsonObject{{"files",output},{"scope","Offline recorded trajectories only; actual product per-round state/gates; unchanged B, no rescue chaining, original passing path preserved; corrupted files prefix only. No prospective speed or accuracy claim."}}).toJson();
        require(out.write(bytes)==bytes.size()&&out.flush(),"Output failed");
    }catch(const std::exception& e){std::fprintf(stderr,"Failed: %s\n",e.what());return 1;}
    return 0;
}
