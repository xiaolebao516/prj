// Research only. Compare earlier policies on the exact new recorded trajectory.
#define MEASUREMENT_REPLAY_LIBRARY
#include "offline_replay.cpp"

static QJsonArray inspectGTransitions(const QVector<QJsonObject>& rows) {
    SignalProcessor p; p.designFIR(1250000,600000,62500000);
    QJsonArray out;
    const GateConfig gate{0,15,18,480,550};
    const auto inspect=[&](const QJsonObject& f) {
        const auto ad=p.applyFIRDouble(SignalProcessor::preprocessRawForFIR(decode(f,"raw_AD"),gate,"AD"));
        const auto chosen=SignalProcessor::findFirstProminentValley(ad,620,qMin(1300,int(ad.size())-2),30,.30,20);
        const auto first=SignalProcessor::findFirstProminentValley(ad,620,qMin(1300,int(ad.size())-2),30,0,20);
        require(chosen.valid && chosen.idx==f["A"].toObject()["early_feature"].toInt(),"A valley transition reconstruction mismatch");
        double maxDepth=0;
        for(int i=620;i<=qMin(1300,int(ad.size())-2);++i)
            if(ad[i]<ad[i-1] && ad[i]<=ad[i+1]) maxDepth=qMax(maxDepth,-ad[i]);
        return QJsonObject{{"sequence",f["sequence"]},{"elapsed_ms",f["elapsed_ms"]},{"G",f["G"]},
            {"A",f["A"]},{"B",f["B"]},{"selected_AD_valley",chosen.idx},
            {"first_AD_valley",first.idx},{"first_depth",first.depth},{"required_depth",qMax(30.0,.30*maxDepth)}};
    };
    QJsonObject previous;
    for(const auto& f:rows) {
        if(f["event"]!="frame") continue;
        if(f.contains("gates") && previous.contains("gates") && f["A_feature_branch"]=="valley" &&
           previous["A_feature_branch"]=="valley" && std::abs(f["G"].toDouble()-previous["G"].toDouble())>40 &&
           f["B"].toObject()["lag"]==previous["B"].toObject()["lag"] && out.size()<6)
            out.append(QJsonObject{{"before",inspect(previous)},{"after",inspect(f)}});
        previous=f;
    }
    return out;
}

int main(int argc,char** argv) {
    QApplication app(argc,argv);
    qInstallMessageHandler([](QtMsgType,const QMessageLogContext&,const QString&){});
    try {
        require(argc>=3,"Usage: followup_replay input-dir output.json -platform offscreen");
        QDir input(QString::fromLocal8Bit(argv[1]));
        auto names=input.entryList({"round-*.jsonl"},QDir::Files,QDir::Name);
        require(names.size()==10,"Expected ten new records");
        QJsonArray results;
        for (const auto& name:names) {
            QFile source(input.filePath(name));require(source.open(QIODevice::ReadOnly),"Cannot read source");
            const auto hash=QString::fromLatin1(QCryptographicHash::hash(source.readAll(),QCryptographicHash::Sha256).toHex());
            auto rows=readRows(input.filePath(name));
            const Case baseline{"baseline",.78,.78,-6,6,14,10,true,true};
            auto raw=MainWindowSafetyTests::runFile(rows,baseline,true);
            require(equivalent(raw,MainWindowSafetyTests::runFile(rows,baseline,false)),"New baseline raw/cached mismatch");
            QJsonObject entry{{"file",name},{"sha256",hash},{"baseline",raw},{"baseline_match",true},
                {"G_transition_probes",inspectGTransitions(rows)}};
            // Restore original A quality from recorded diagnostics, not by changing the input waveform.
            for (auto& r:rows) if(r["event"]=="frame") {
                if(r.contains("A_quality_trial")) {
                    auto a=r["A"].toObject();
                    a["corr"]=r["A_quality_trial"].toObject()["original_corr"];r["A"]=a;
                }
                if(r.contains("gates")) {
                    auto g=r["gates"].toObject();g["corr_A"]=r["A"].toObject()["corr"].toDouble()>=.78;
                    bool pre=true;for(const auto* key:{"B_jump","boundary","AB_diff","direction","corr_A","corr_B","D"}) pre=pre&&g[key].toBool();
                    g["stability_evaluated"]=pre;g["all_prechecks_passed"]=pre&&g["G"].toBool();r["gates"]=g;
                }
            }
            for (const auto& trial:{Case{"previous_observe",.78,.80,-6,6,14,10,true,false},
                                    Case{"original",.78,.80,-6,6,14,10,false,false}}) {
                auto cached=MainWindowSafetyTests::runFile(rows,trial,false);
                auto actual=MainWindowSafetyTests::runFile(rows,trial,true);
                require(equivalent(cached,actual),"Old-policy raw/cached mismatch");
                entry[trial.name]=actual;
            }
            results.append(entry);
            std::fprintf(stderr,"Verified %d/10: %s\n",int(results.size()),qPrintable(name));
        }
        QFile out(QString::fromLocal8Bit(argv[2]));require(out.open(QIODevice::WriteOnly),"Cannot write output");
        const auto bytes=QJsonDocument(QJsonObject{{"files",results},{"scope","Fixed-trajectory per-attempt comparison; missing old-policy completion cannot predict subsequent user motion or time. No new threshold or product change."}}).toJson();
        require(out.write(bytes)==bytes.size(),"Incomplete output");
    } catch(const std::exception& e) {std::fprintf(stderr,"Failed: %s\n",e.what());return 1;}
    return 0;
}
