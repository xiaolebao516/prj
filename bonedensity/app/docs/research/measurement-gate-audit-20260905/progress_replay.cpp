// Progress-first bounded research; never filters or fits SOS to a reference band.
#define MEASUREMENT_REPLAY_LIBRARY
#include "offline_replay.cpp"

static QJsonObject jsonFile(const QString& path){QFile f(path);require(f.open(QIODevice::ReadOnly),"Cannot read "+path);QJsonParseError e;auto d=QJsonDocument::fromJson(f.readAll(),&e);require(e.error==QJsonParseError::NoError&&d.isObject(),"Bad JSON");return d.object();}
int main(int argc,char** argv){
    QApplication app(argc,argv);qInstallMessageHandler([](QtMsgType,const QMessageLogContext&,const QString&){});
    try {
        require(argc>=6,"Usage: progress_replay G-audit.json A-audit.json old-dual-results.json G-audit-cwd output.json -platform offscreen");
        const auto gFiles=jsonFile(QString::fromLocal8Bit(argv[1]))["files"].toArray();
        QMap<QString,QJsonObject> aFiles,previousDual;
        for(const auto& v:jsonFile(QString::fromLocal8Bit(argv[2]))["files"].toArray()){const auto x=v.toObject();aFiles[x["file"].toString()]=x;}
        for(const auto& v:jsonFile(QString::fromLocal8Bit(argv[3]))["files"].toArray()){const auto x=v.toObject();previousDual[x["file"].toString()]=x;}
        require(gFiles.size()==19&&aFiles.size()==9&&previousDual.size()==9,"Unexpected manifest count");
        QJsonArray results;
        for(const auto& v:gFiles){const auto gEntry=v.toObject();const auto name=gEntry["file"].toString();
            const auto path=QDir(QString::fromLocal8Bit(argv[4])).filePath(gEntry["source"].toString());
            QFile f(path);require(f.open(QIODevice::ReadOnly),"Cannot read source");
            require(QString::fromLatin1(QCryptographicHash::hash(f.readAll(),QCryptographicHash::Sha256).toHex())==gEntry["sha256"].toString(),"Input changed");
            const auto original=readRows(path);const auto config=original.first()["config"].toObject();
            const bool dual=config["implementation"]=="dual-window-a078-20260906-v1";
            const bool observe=dual||config["implementation"]=="observe-before-g-20260906-v1";
            const Case recorded{"baseline",.78,dual?.78:.8,-6,6,14,10,observe,dual};
            const auto sourceBaseline=MainWindowSafetyTests::runFile(original,recorded,false);
            auto current=original;QMap<int,QJsonObject> aProbes,gProbes;
            if(!dual){require(aFiles.contains(name)&&previousDual.contains(name),"Missing old evidence");
                require(aFiles[name]["sha256"]==gEntry["sha256"]&&previousDual[name]["sha256"]==gEntry["sha256"],"Old manifest differs");
                for(const auto& x:aFiles[name]["frames"].toArray())aProbes[x.toObject()["sequence"].toInt()]=x.toObject();
            }
            for(const auto& x:gEntry["frames"].toArray())gProbes[x.toObject()["sequence"].toInt()]=x.toObject();
            for(auto& row:current){if(row["event"]!="frame"||!row.contains("gates"))continue;
                auto a=row["A"].toObject();
                if(!dual&&row["A_feature_branch"]=="valley"){
                    const auto probe=aProbes.value(row["sequence"].toInt());require(!probe.isEmpty()&&equivalent(probe["A"],a)&&equivalent(probe["B"],row["B"]),"A probe identity differs");
                    a["corr"]=qMin(probe["front"].toObject()["fixed_lag_corr"].toDouble(),probe["middle"].toObject()["fixed_lag_corr"].toDouble());row["A"]=a;
                }
                auto gates=row["gates"].toObject();gates["corr_A"]=a["corr"].toDouble()>=.78;
                bool beforeG=true;for(const auto* key:{"B_jump","boundary","AB_diff","direction","corr_A","corr_B","D"})beforeG=beforeG&&gates[key].toBool();
                gates["stability_evaluated"]=beforeG;row["gates"]=gates;
            }
            const Case dualCase{"current",.78,.78,-6,6,14,10,true,true};
            const auto control=MainWindowSafetyTests::runFile(current,dualCase,false);
            require(equivalent(control,dual?sourceBaseline:previousDual[name]["dual_window_078"].toObject()),"Current dual control differs from verified earlier evidence");
            QJsonObject report{{"file",name},{"sha256",gEntry["sha256"]},{"source_baseline",sourceBaseline}};
            for(const bool corrected:{false,true})for(const int limit:{6,8,10}){
                if(corrected&&limit==8)continue;
                auto rows=current;int changed=0;
                if(corrected)for(auto& row:rows){if(row["event"]!="frame"||!gProbes.contains(row["sequence"].toInt()))continue;
                    const auto candidate=gProbes[row["sequence"].toInt()]["quarter"].toObject();
                    if(candidate["late_valid"].toBool()&&candidate["quality_at_unchanged_A_lag"].toDouble()>=.78){if(row["G"]!=candidate["G"])++changed;row["G"]=candidate["G"];}
                }
                auto variant=dualCase;variant.name=QString(corrected?"corrected_G%1":"original_G%1").arg(limit);variant.gMin=-limit;variant.gMax=limit;
                auto outcome=MainWindowSafetyTests::runFile(rows,variant,false,true);outcome["changed_G_frames"]=changed;report[variant.name]=outcome;
                if(!corrected&&limit!=6){
                    const auto direct=MainWindowSafetyTests::runFile(rows,variant,true);
                    const auto cached=MainWindowSafetyTests::runFile(rows,variant,false);
                    require(equivalent(direct,cached),"G candidate actual raw and cached results differ");
                    report[QString("G%1_raw_candidate_matches").arg(limit)]=true;
                }
            }
            results.append(report);std::fprintf(stderr,"Progress comparison %d/19 verified\n",int(results.size()));
        }
        QFile output(QString::fromLocal8Bit(argv[5]));require(output.open(QIODevice::WriteOnly),"Cannot write output");
        const auto bytes=QJsonDocument(QJsonObject{{"files",results},{"scope","Current dual0.78 baseline verified against logged new10 and earlier raw-verified old9. Four G-only research controls use raw-verified landmarks and actual product stability/round aggregation. Unchanged per-frame SOS/lag/A score. Accepted traces include later discarded values; fixed trajectories, not prospective timing or accuracy."}}).toJson();require(output.write(bytes)==bytes.size(),"Short output write");
    }catch(const std::exception& e){std::fprintf(stderr,"Failed: %s\n",e.what());return 1;}return 0;
}
