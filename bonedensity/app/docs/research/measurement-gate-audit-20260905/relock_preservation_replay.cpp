// Research only: defer deleting accepted values until the replacement B-lag cluster is known.
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
        require(argc==4,"Usage: relock_preservation_replay triage.json input-directory output.json");
        QFile manifest(QString::fromLocal8Bit(argv[1]));require(manifest.open(QIODevice::ReadOnly),"Cannot read manifest");
        const auto files=QJsonDocument::fromJson(manifest.readAll()).object()["files"].toArray();
        require(files.size()==73,"Expected complete triage manifest");QJsonArray output;int count=0;
        const Case baseline{"baseline",.78,.78,-6,6,14,10,true,true,false};
        const Case candidate{"relock-preservation",.78,.78,-6,6,14,10,true,true,true};
        for(const auto& value:files) {
            const auto entry=value.toObject();
            if(entry["file"].toString().mid(15,2).toInt()<8)continue;
            bool damaged=false;const auto rows=prefixRows(QDir(QString::fromLocal8Bit(argv[2])).filePath(entry["file"].toString()),entry["sha256"].toString(),damaged);
            output.append(QJsonObject{{"file",entry["file"]},{"sha256",entry["sha256"]},{"damaged_prefix_only",damaged},
                {"baseline",MainWindowSafetyTests::runFile(rows,baseline,false,true)},
                {"candidate",MainWindowSafetyTests::runFile(rows,candidate,false,true)}});
            std::fprintf(stderr,"Relock replay %d/63\n",++count);
        }
        require(count==63,"Expected63 afternoon logs");
        QFile out(QString::fromLocal8Bit(argv[3]));require(out.open(QIODevice::WriteOnly|QIODevice::NewOnly),"Output must be new");
        const auto bytes=QJsonDocument(QJsonObject{{"files",output},{"scope","Recorded-feature counterfactual through actual stability and round aggregation. Accepted values are retained across sustained precheck loss only when the next independently established B-lag cluster matches the previous center; different centers discard. No gate/threshold/SOS estimator change or prospective claim."}}).toJson();
        require(out.write(bytes)==bytes.size()&&out.flush(),"Output failed");
    }catch(const std::exception& e){std::fprintf(stderr,"Failed: %s\n",e.what());return 1;}
    return 0;
}
