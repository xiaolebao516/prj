// Isolated research comparison. No product quality formula is changed here.
#define MEASUREMENT_REPLAY_LIBRARY
#include "offline_replay.cpp"

int main(int argc,char** argv) {
    QApplication app(argc,argv);
    qInstallMessageHandler([](QtMsgType,const QMessageLogContext&,const QString&){});
    try {
        require(argc>=5,"Usage: dual_window_replay audit.json old-log-dir failed.jsonl output.json -platform offscreen");
        QFile auditFile(QString::fromLocal8Bit(argv[1]));require(auditFile.open(QIODevice::ReadOnly),"Cannot read audit");
        const auto audit=QJsonDocument::fromJson(auditFile.readAll()).object()["files"].toArray();
        require(audit.size()==9,"Expected nine audited files");
        QJsonArray results; QMap<QString,QVector<RoundCandidate>> candidates;
        for (int index=0;index<audit.size();++index) {
            const auto entry=audit[index].toObject();
            const QString path=index==8 ? QString::fromLocal8Bit(argv[3]) :
                QDir(QString::fromLocal8Bit(argv[2])).filePath(entry["file"].toString());
            QFile source(path);require(source.open(QIODevice::ReadOnly),"Cannot read source");
            const auto hash=QString::fromLatin1(QCryptographicHash::hash(source.readAll(),QCryptographicHash::Sha256).toHex());
            require(hash==entry["sha256"].toString(),"Input changed since raw audit");
            auto rows=readRows(path);
            const bool experimental=rows.first()["config"].toObject()["implementation"]=="observe-before-g-20260906-v1";
            Case baseline{"baseline",.78,.80,-6,6,14,10,experimental};
            const auto raw=MainWindowSafetyTests::runFile(rows,baseline,true);
            const auto cached=MainWindowSafetyTests::runFile(rows,baseline,false);
            require(equivalent(raw,cached),"Recorded raw/feature baseline differs");
            const Case control{"observe_before_G",.78,.80,-6,6,14,10,true};
            const auto unchanged=MainWindowSafetyTests::runFile(rows,control,false);
            QMap<int,QJsonObject> scores;
            for (const auto& v:entry["frames"].toArray()) {auto x=v.toObject();scores.insert(x["sequence"].toInt(),x);}
            int replaced=0;
            for (auto& frame:rows) {
                if (frame["event"]!="frame" || !frame.contains("gates") || frame["A_feature_branch"]!="valley") continue;
                const int seq=frame["sequence"].toInt();require(scores.contains(seq),"Missing validated score");
                const auto score=scores[seq];auto a=frame["A"].toObject();
                require(equivalent(a,score["A"]),"Audited feature identity mismatch");
                const double front=score["front"].toObject()["fixed_lag_corr"].toDouble(-2);
                const double middle=score["middle"].toObject()["fixed_lag_corr"].toDouble(-2);
                require(std::isfinite(front) && std::isfinite(middle),"Invalid window score");
                a["corr"]=qMin(front,middle);frame["A"]=a;
                // These counterfactual gates are only guards for feature replay.
                auto gates=frame["gates"].toObject();gates["corr_A"]=a["corr"].toDouble()>=.78;
                bool pre=true;for (const auto* k:{"B_jump","boundary","AB_diff","direction","corr_A","corr_B","D"}) pre=pre && gates[k].toBool();
                gates["stability_evaluated"]=pre;gates["all_prechecks_passed"]=pre && gates["G"].toBool();
                frame["gates"]=gates;++replaced;
            }
            const auto candidate=MainWindowSafetyTests::runFile(rows,Case{"dual_window",.78,.80,-6,6,14,10,true},false);
            const Case implementedCase{"dual_window_078",.78,.78,-6,6,14,10,true,true};
            const auto implemented=MainWindowSafetyTests::runFile(rows,implementedCase,true);
            const auto expectedTrial=MainWindowSafetyTests::runFile(rows,implementedCase,false);
            require(equivalent(implemented,expectedTrial),"Implemented raw dual0.78 differs from independent cached candidate");
            QJsonObject result{{"file",entry["file"]},{"sha256",hash},{"raw_feature_baseline_match",true},
                {"implemented_raw_candidate_match",true},{"dual_window_078",implemented},
                {"replaced_valley_scores",replaced},{"recorded_baseline",raw},{"observe_before_G",unchanged},{"dual_window",candidate}};
            // Only the original eight logs are a multi-attempt session. Never mix subjects/sessions.
            if (index<8) for (const auto& name:{QString("observe_before_G"),QString("dual_window"),QString("dual_window_078")}) {
                const auto summary=result[name].toObject()["round_summary"].toObject();
                if (summary["quality_pass"].toBool()) {RoundCandidate c;c.sos=summary["sos"].toDouble();candidates[name].append(c);}
                QVector<double> sos,a,b;Utils::rebuildAcceptedRoundsFromCandidates(candidates[name],sos,a,b,180.0);
                result[name+"_conditional_rounds"]=array(sos);
                if (sos.size()>=5) result[name+"_conditional_final"]=Utils::trimmedMeanValue(sos,.2);
            }
            results.append(result);
            std::fprintf(stderr,"Verified and compared %d/9: %s\n",index+1,qPrintable(entry["file"].toString()));
        }
        QFile output(QString::fromLocal8Bit(argv[4]));require(output.open(QIODevice::WriteOnly),"Cannot write output");
        QJsonObject report{{"scope","Validated raw/feature recorded baselines and implemented raw dual0.78 versus independently scored cached candidate. Non-valley fallback unchanged. Each attempt stops at first30. This is not prospective accuracy or positioning acceptance."},
            {"candidate","A=min(corr[-20,+30],corr[0,+60]) at unchanged A lag. Frame0.78/round0.78; observe-before-G. Previous round0.80 candidate retained as control."},{"files",results}};
        const auto bytes=QJsonDocument(report).toJson();require(output.write(bytes)==bytes.size(),"Incomplete write");
    } catch (const std::exception& e) {std::fprintf(stderr,"Comparison failed: %s\n",e.what());return 1;}
    return 0;
}
