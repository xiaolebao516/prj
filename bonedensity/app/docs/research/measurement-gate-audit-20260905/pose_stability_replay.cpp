// Research only: observe B-lag stability before both pose gates while keeping
// the original D/G requirements on every accepted value.
#define MEASUREMENT_REPLAY_LIBRARY
#include "offline_replay.cpp"

static QVector<QJsonObject> prefixRows(const QString& path,const QString& expectedHash,bool& damaged)
{
    QFile file(path);require(file.open(QIODevice::ReadOnly),"Cannot read source");
    QCryptographicHash hash(QCryptographicHash::Sha256);require(hash.addData(&file),"Hash failed");
    require(QString::fromLatin1(hash.result().toHex())==expectedHash,"Changed source");
    file.seek(0);QVector<QJsonObject> rows;damaged=false;
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
    QApplication app(argc,argv);qInstallMessageHandler([](QtMsgType,const QMessageLogContext&,const QString&){});
    try {
        require(argc==4,"Usage: pose_stability_replay triage.json input-directory output.json");
        QFile manifest(QString::fromLocal8Bit(argv[1]));require(manifest.open(QIODevice::ReadOnly),"Cannot read manifest");
        const auto files=QJsonDocument::fromJson(manifest.readAll()).object()["files"].toArray();
        require(files.size()==73,"Expected complete triage manifest");QJsonArray output;int count=0;
        const Case baseline{"baseline",.78,.78,-6,6,14,10,true,true,false,false,0};
        const Case poseOnly{"pose-independent-stability",.78,.78,-6,6,14,10,true,true,false,true,0};
        const Case poseAndRelock{"pose-and-relock",.78,.78,-6,6,14,10,true,true,true,true,0};
        const Case dGrace2{"compatible-D-grace-2",.78,.78,-6,6,14,10,true,true,false,false,2};
        const Case dGrace5{"compatible-D-grace-5",.78,.78,-6,6,14,10,true,true,false,false,5};
        const Case dGrace10{"compatible-D-grace-10",.78,.78,-6,6,14,10,true,true,false,false,10};
        const Case dGrace20{"compatible-D-grace-20",.78,.78,-6,6,14,10,true,true,false,false,20};
        for(const auto& value:files) {
            const auto entry=value.toObject();
            if(entry["file"].toString().mid(15,2).toInt()<8)continue;
            bool damaged=false;const auto rows=prefixRows(QDir(QString::fromLocal8Bit(argv[2])).filePath(entry["file"].toString()),entry["sha256"].toString(),damaged);
            output.append(QJsonObject{{"file",entry["file"]},{"sha256",entry["sha256"]},{"damaged_prefix_only",damaged},
                {"baseline",MainWindowSafetyTests::runFile(rows,baseline,false,true)},
                {"pose_only",MainWindowSafetyTests::runFile(rows,poseOnly,false,true)},
                {"pose_and_relock",MainWindowSafetyTests::runFile(rows,poseAndRelock,false,true)},
                {"d_grace_2",MainWindowSafetyTests::runFile(rows,dGrace2,false,true)},
                {"d_grace_5",MainWindowSafetyTests::runFile(rows,dGrace5,false,true)},
                {"d_grace_10",MainWindowSafetyTests::runFile(rows,dGrace10,false,true)},
                {"d_grace_20",MainWindowSafetyTests::runFile(rows,dGrace20,false,true)}});
            std::fprintf(stderr,"Pose stability replay %d/63\n",++count);
        }
        require(count==63,"Expected63 afternoon logs");
        QFile out(QString::fromLocal8Bit(argv[3]));require(out.open(QIODevice::WriteOnly|QIODevice::NewOnly),"Output must be new");
        const auto bytes=QJsonDocument(QJsonObject{{"files",output},{"scope","Recorded-feature counterfactual through actual stability and round aggregation. Pose candidate observes B-lag stability before D/G. Bounded D-grace candidates do not add D-failing frames and postpone rejection by only 2/5/10/20 consecutive frames when every other gate passes and lagB remains within the existing D-qualified cluster. Every accepted value still passes original D[5,15], G[-6,6], correlation, lag and round gates. Thresholds and SOS estimator unchanged. Fixed trajectories, no paired truth or prospective timing claim."}}).toJson();
        require(out.write(bytes)==bytes.size()&&out.flush(),"Output failed");
    }catch(const std::exception& e){std::fprintf(stderr,"Failed: %s\n",e.what());return 1;}
    return 0;
}
