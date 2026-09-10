#define MEASUREMENT_REPLAY_LIBRARY
#include "offline_replay.cpp"

int main(int argc,char** argv) {
    QApplication app(argc,argv);qInstallMessageHandler([](QtMsgType,const QMessageLogContext&,const QString&){});
    try {
        require(argc>=4,"Usage: g_landmark_replay audit.json output.json audit-working-directory -platform offscreen");
        QFile input(QString::fromLocal8Bit(argv[1]));require(input.open(QIODevice::ReadOnly),"Cannot read audit");
        const auto files=QJsonDocument::fromJson(input.readAll()).object()["files"].toArray();
        require(files.size()==19,"Expected19 recordings");QJsonArray results;
        for(const auto& value:files) {
            const auto entry=value.toObject();const auto path=QDir(QString::fromLocal8Bit(argv[3])).filePath(entry["source"].toString());
            QFile source(path);require(source.open(QIODevice::ReadOnly),"Cannot read source");
            require(QString::fromLatin1(QCryptographicHash::hash(source.readAll(),QCryptographicHash::Sha256).toHex())==entry["sha256"].toString(),"Changed input");
            const auto original=readRows(path);const auto profile=original.first()["config"].toObject()["implementation"].toString();
            const bool dual=profile=="dual-window-a078-20260906-v1";
            const bool observe=dual||profile=="observe-before-g-20260906-v1";
            const Case baseline{"baseline",.78,dual?.78:.80,-6,6,14,10,observe,dual};
            QJsonObject report{{"file",entry["file"]},{"sha256",entry["sha256"]},
                {"baseline",MainWindowSafetyTests::runFile(original,baseline,false)}};
            QMap<int,QJsonObject> probes;for(const auto& v:entry["frames"].toArray())probes.insert(v.toObject()["sequence"].toInt(),v.toObject());
            for(const QString name:{QString("quarter"),QString("tracked")}) {
                auto rows=original;int changed=0,unusable=0;
                for(auto& f:rows) {
                    if(f["event"]!="frame"||!probes.contains(f["sequence"].toInt()))continue;
                    const auto probe=probes[f["sequence"].toInt()],candidate=probe[name].toObject();
                    require(equivalent(probe["A"],f["A"])&&equivalent(probe["B"],f["B"]),"Feature identity differs");
                    // Diagnostic fallback: no justified new landmark -> preserve original G.
                    // Do not weaken original A/B/D/stability or reselect lag/SOS.
                    if(!candidate["late_valid"].toBool()||candidate["quality_at_unchanged_A_lag"].toDouble()<.78){++unusable;continue;}
                    if(candidate["G"]==f["G"])continue;
                    f["G"]=candidate["G"];++changed;
                    auto gates=f["gates"].toObject();gates["G"]=std::abs(f["G"].toDouble())<=6;f["gates"]=gates;
                }
                auto trial=baseline;trial.name=name;
                report[name]=MainWindowSafetyTests::runFile(rows,trial,false);
                report[name+"_changed"]=changed;report[name+"_unusable_preserved"]=unusable;
            }
            results.append(report);std::fprintf(stderr,"Compared %d/19\n",int(results.size()));
        }
        QFile out(QString::fromLocal8Bit(argv[2]));require(out.open(QIODevice::WriteOnly),"Cannot write output");
        const auto bytes=QJsonDocument(QJsonObject{{"files",results},{"scope","Research-only cached replay with raw-reconstructed G landmarks, using actual product stability and aggregation. Recorded baseline validates every decision/state/summary. Product raw candidate not implemented or deployed; source-specific A quality/sequence retained. Unusable proposed landmark falls back to original G. No accuracy or prospective time claim."}}).toJson();
        require(out.write(bytes)==bytes.size(),"Short write");
    }catch(const std::exception& e){std::fprintf(stderr,"Failed: %s\n",e.what());return 1;}return 0;
}
