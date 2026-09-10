// Research only: actual raw B candidate through dependent product flow.
#define MEASUREMENT_REPLAY_LIBRARY
#include "offline_replay.cpp"

static QVector<QJsonObject> verifiedRows(const QString& path,const QString& expectedHash)
{
    QFile file(path);require(file.open(QIODevice::ReadOnly),"Cannot read source");
    QCryptographicHash hash(QCryptographicHash::Sha256);require(hash.addData(&file),"Hash failed");
    require(QString::fromLatin1(hash.result().toHex())==expectedHash,"Changed source");
    file.seek(0);QVector<QJsonObject> rows;
    while(!file.atEnd()) {
        const auto line=file.readLine(1024*1024);
        require(!line.contains('\0')&&line.endsWith('\n'),"Damaged input must be excluded");
        QJsonParseError error;const auto doc=QJsonDocument::fromJson(line,&error);
        require(error.error==QJsonParseError::NoError&&doc.isObject(),"Bad JSON");
        rows.append(doc.object());
    }
    require(!rows.isEmpty()&&rows.first()["event"]=="start","Missing start");
    return rows;
}

int main(int argc,char** argv)
{
    QApplication app(argc,argv);qInstallMessageHandler([](QtMsgType,const QMessageLogContext&,const QString&){});
    try {
        require(argc==4,"Usage: b_peak_full_replay triage.json input-directory output.json");
        QFile manifest(QString::fromLocal8Bit(argv[1]));require(manifest.open(QIODevice::ReadOnly),"Cannot read manifest");
        const auto files=QJsonDocument::fromJson(manifest.readAll()).object()["files"].toArray();
        require(files.size()==73,"Expected complete triage manifest");QJsonArray output;int skipped=0;
        const Case baseline{"baseline",.78,.78,-6,6,14,10,true,true};
        for(const auto& value:files) {
            const auto entry=value.toObject();
            if(entry["file"].toString().mid(15,2).toInt()<8)continue;
            if(entry["tail_bytes"].toDouble()>0){++skipped;output.append(QJsonObject{{"file",entry["file"]},{"sha256",entry["sha256"]},{"damaged_excluded",true}});continue;}
            const auto rows=verifiedRows(QDir(QString::fromLocal8Bit(argv[2])).filePath(entry["file"].toString()),entry["sha256"].toString());
            const auto base=MainWindowSafetyTests::runFile(rows,baseline,false,true);
            const auto candidate=MainWindowSafetyTests::runRawCompact(rows);
            output.append(QJsonObject{{"file",entry["file"]},{"sha256",entry["sha256"]},
                {"damaged_excluded",false},{"baseline",base},{"candidate",candidate}});
            std::fprintf(stderr,"Raw B replay %d/61\n",int(output.size())-skipped);
        }
        require(skipped==2,QString("Expected two separately excluded damaged logs, got %1").arg(skipped));
        QFile out(QString::fromLocal8Bit(argv[3]));require(out.open(QIODevice::WriteOnly|QIODevice::NewOnly),"Output must be new");
        const auto bytes=QJsonDocument(QJsonObject{{"files",output},{"scope","Actual raw B peak-completion profile through dependent A, D/G, stability and round aggregation. Existing logs are anonymous; two damaged files excluded. No target-SOS fitting or prospective claim."}}).toJson();
        require(out.write(bytes)==bytes.size()&&out.flush(),"Output failed");
    }catch(const std::exception& e){std::fprintf(stderr,"Failed: %s\n",e.what());return 1;}
    return 0;
}
