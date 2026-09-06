// Research-only runner. Links unchanged product sources, never opens the device.
// Raw baseline first; only threshold comparisons may reuse validated features.
#include "mainwindow.h"
#include "utils.h"
#include <QApplication>
#include <QCryptographicHash>
#include <QJsonArray>
#include <QTemporaryDir>
#include <QtEndian>
#include <cmath>
#include <cstdio>
#include <stdexcept>

static void require(bool ok, const QString& message) {
    if (!ok) throw std::runtime_error(message.toStdString());
}
static QVector<QJsonObject> readRows(const QString& path) {
    QFile file(path);
    require(file.open(QIODevice::ReadOnly), "Cannot read " + path);
    QVector<QJsonObject> rows;
    while (!file.atEnd()) {
        const auto line = file.readLine();
        if (line.trimmed().isEmpty()) continue;
        QJsonParseError error;
        const auto doc = QJsonDocument::fromJson(line, &error);
        require(error.error == QJsonParseError::NoError && doc.isObject(), "Invalid JSON: " + path);
        rows.append(doc.object());
    }
    return rows;
}
static bool equivalent(const QJsonValue& a, const QJsonValue& b) {
    if (a.type() != b.type()) return false;
    if (a.isDouble()) return std::abs(a.toDouble() - b.toDouble()) <= 1e-8;
    if (a.isObject()) {
        const auto x=a.toObject(), y=b.toObject();
        if (x.keys()!=y.keys()) return false;
        for (const auto& key : x.keys()) if (!equivalent(x[key],y[key])) return false;
        return true;
    }
    if (a.isArray()) {
        const auto x=a.toArray(), y=b.toArray();
        if (x.size()!=y.size()) return false;
        for (int i=0;i<x.size();++i) if (!equivalent(x[i],y[i])) return false;
        return true;
    }
    return a==b;
}
static QJsonObject normalized(QJsonObject row) {
    for (const auto* key : {"elapsed_ms","processing_ms","sequence",
                           "raw_BC","raw_BD","raw_AC","raw_AD"}) row.remove(key);
    return row;
}
static QVector<quint16> decode(const QJsonObject& frame, const char* key) {
    const auto decoded=QByteArray::fromBase64Encoding(frame[key].toString().toLatin1(),
                                                    QByteArray::AbortOnBase64DecodingErrors);
    require(bool(decoded) && !decoded.decoded.isEmpty() && decoded.decoded.size()%2==0,
            QString("Invalid raw channel ") + key);
    QVector<quint16> result(decoded.decoded.size()/2);
    for (qsizetype i=0;i<result.size();++i)
        result[i]=qFromLittleEndian<quint16>(decoded.decoded.constData()+2*i);
    return result;
}
static QJsonArray array(const QVector<double>& values) {
    QJsonArray result;
    for (double value : values) result.append(value);
    return result;
}
struct Case {
    QString name;
    double frameA=.78, roundA=.80, gMin=-6, gMax=6;
    int warmup=14, lossCount=10;
    bool stabilityBeforeG=false;
    bool dualWindow=false;
};

// Independent observation of the existing valley correlation window, not a new estimator.
static double windowCorrelation(const QVector<double>& early, const QVector<double>& late,
                                int onset, int lag) {
    const int n=qMin(early.size(),late.size());
    const int start=qMax(0,onset-20), end=qMin(n-1,onset+90);
    double mx=0,my=0; int count=0;
    for (int i=start;i<=end;++i) if (i+lag>=0 && i+lag<n) {
        mx+=early[i]; my+=late[i+lag]; ++count;
    }
    if (count<10) return -2;
    mx/=count; my/=count;
    double num=0,dx=0,dy=0;
    for (int i=start;i<=end;++i) if (i+lag>=0 && i+lag<n) {
        const double x=early[i]-mx,y=late[i+lag]-my;
        num+=x*y; dx+=x*x; dy+=y*y;
    }
    return dx<1e-12 || dy<1e-12 ? -2 : num/std::sqrt(dx*dy);
}

static QJsonArray inspectFeatures(const QVector<QJsonObject>& rows) {
    SignalProcessor processor;
    processor.designFIR(1250000.0,600000.0,62500000.0);
    const GateConfig bGate{20,105,115,500,560}, aGate{0,15,18,480,550};
    QJsonArray observations;
    for (const auto& frame : rows) {
        if (frame["event"]!="frame" || !frame.contains("gates")) continue;
        const auto filter=[&](const char* key,const GateConfig& gate) {
            return processor.applyFIRDouble(SignalProcessor::preprocessRawForFIR(decode(frame,key),gate,key));
        };
        const auto bc=filter("raw_BC",bGate),bd=filter("raw_BD",bGate);
        const auto ac=filter("raw_AC",aGate),ad=filter("raw_AD",aGate);
        const auto pick=[](const QVector<double>& wave) {
            return SignalProcessor::detectFirstArrivalSmart(wave,560,615,620,qMin(int(wave.size())-1,1600),4,8,20,.2,140);
        };
        const auto pbc=pick(bc),pbd=pick(bd),pac=pick(ac),pad=pick(ad);
        const auto a=frame["A"].toObject(),b=frame["B"].toObject();
        require(pbc.valid && pbd.valid && pbd.onset==b["early_feature"].toInt() &&
                pbc.onset==b["late_feature"].toInt(),"B onset reconstruction mismatch");
        QJsonObject out{{"sequence",frame["sequence"]},{"elapsed_ms",frame["elapsed_ms"]},
            {"G",frame["G"]},{"sos",frame["sos_patient"]},{"decision",frame["decision"]},
            {"branch",frame["A_feature_branch"]},{"gates",frame["gates"]},
            {"A_corr",a["corr"]},{"B_corr",b["corr"]}};
        if (pac.valid && pad.valid) {
            const double onsetG=.5*(pbc.onset+pbd.onset-pac.onset-pad.onset);
            out["same_onset_G"]=onsetG;
            out["A_onset_mid_minus_used_mid"]=.5*(pac.onset+pad.onset-a["early_feature"].toInt()-a["late_feature"].toInt());
        }
        if (frame["A_feature_branch"]=="valley") {
            const auto observed=processor.estimatePairSpeedByValley(ad,ac,pad,b["lag"].toInt(),20,"research A");
            require(observed.valid && observed.earlyOnset==a["early_feature"].toInt() &&
                    observed.lateOnset==a["late_feature"].toInt() && observed.refinedLag==a["lag"].toInt() &&
                    std::abs(observed.corr-a["corr"].toDouble())<=1e-8,"A valley reconstruction mismatch");
            const double selected=windowCorrelation(ad,ac,observed.earlyOnset,observed.refinedLag);
            require(std::abs(selected-observed.corr)<=1e-8,"Independent correlation mismatch");
            double peak=-1; int peakLag=0;
            for (int lag=qMax(1,observed.roughLag-15);lag<=observed.roughLag+15;++lag) {
                const double value=windowCorrelation(ad,ac,observed.earlyOnset,lag);
                if (value>peak) { peak=value; peakLag=lag; }
            }
            out["A_peak_corr"]=peak; out["A_peak_lag"]=peakLag;
            out["A_selected_lag"]=observed.refinedLag;
        }
        observations.append(out);
    }
    return observations;
}

class MainWindowSafetyTests {
    static bool featureFrame(MainWindow& w, const QJsonObject& frame, bool validate, bool stabilityBeforeG) {
        const QString decision=frame["decision"].toString();
        if (!frame.contains("gates")) {
            require(decision=="B_pair_invalid" || decision=="A_pair_invalid" ||
                    decision=="B_lag_jump" || decision=="AB_difference" ||
                    decision=="empty_filtered_input", "Unknown early-return path");
            w.updateProcessInvalid("offline rejected frame");
            return false;
        }
        const auto a=frame["A"].toObject(), b=frame["B"].toObject();
        const int lagA=a["lag"].toInt(), lagB=b["lag"].toInt();
        const int d=lagA-lagB;
        const double g=frame["G"].toDouble();
        const bool beforeG=std::abs(lagB-b["rough_lag"].toInt())<=70 && lagB<260 &&
            std::abs(d)<=18 && d>=-2 && a["corr"].toDouble()>=w.mCfg.frameCorrAMin &&
            b["corr"].toDouble()>=w.mCfg.frameCorrBMin &&
            d>=w.mCfg.angleSignedDiffMin && d<=w.mCfg.angleSignedDiffMax;
        const bool pre=beforeG && g>=w.mCfg.anglePairMidGapMin && g<=w.mCfg.anglePairMidGapMax;
        const bool observe=stabilityBeforeG ? beforeG : pre;
        const bool stable=observe ? w.checkBoneLagStable(lagB) : false;
        const bool accepted=pre && stable;
        if (!observe) w.rejectBoneLagCandidate();
        if (stabilityBeforeG && accepted)
            require(frame["gates"].toObject()["stability_evaluated"].toBool(),"New flow accepted an originally precheck-failing frame");
        if (validate) {
            require(observe==frame["gates"].toObject()["stability_evaluated"].toBool(), "Feature precheck mismatch");
            require(accepted==(decision=="accepted"), "Feature accepted-frame mismatch");
            require(w.boneLagLocked==frame["locked"].toBool() &&
                    w.lockedBoneLagCenter==frame["locked_lag"].toInt() &&
                    w.processValidCount==frame["partial_values_before_accept"].toInt(), "Feature state mismatch");
        }
        w.handlePatientMeasureValue(a["sos"].toDouble(),b["sos"].toDouble(),frame["sos_patient"].toDouble(),
                                    lagA,lagB,std::abs(d),g,a["corr"].toDouble(),b["corr"].toDouble(),accepted);
        return accepted;
    }
public:
    static QJsonObject runFile(const QVector<QJsonObject>& rows, const Case& variant, bool raw) {
        MainWindow w;
        w.hide();
        w.observeStabilityBeforeG=variant.stabilityBeforeG;
        w.useDualWindowAQuality=variant.dualWindow;
        const auto config=rows.first()["config"].toObject();
        require(config["implementation"]=="state-repair-20260905-v1" ||
                config["implementation"]=="observe-before-g-20260906-v1", "Unsupported input version");
        require(config["B_only"].toBool() && config["angle_gate_enabled"].toBool() &&
                config["SOS_offset"].toDouble()==0 && config["frame_target"].toInt()==30 &&
                config["D_min"].toDouble()==5 && config["D_max"].toDouble()==15 &&
                config["frame_corr_A"].toDouble()==.78 && config["round_corr_A"].toDouble()==.8 &&
                config["frame_corr_B"].toDouble()==.55 && config["round_corr_B"].toDouble()==.55 &&
                config["G_min"].toDouble()==-6 && config["G_max"].toDouble()==6 &&
                config["warmup"].toInt()==14 && config["lock_need"].toInt()==10 &&
                config["lag_tolerance"].toInt()==5 && config["unlock_count"].toInt()==10 &&
                config["window_size"].toInt()==20 && config["round_cluster_tolerance"].toDouble()==180 &&
                config["probe_distance_m"].toDouble()==w.signalProcessor.probeDistanceCD &&
                config["sample_period_s"].toDouble()==w.signalProcessor.samplePeriod,
                "Input configuration differs from this audited replay contract");
        w.resetOneRoundMeasurementState();
        w.patientMeasureRunning=true;
        w.acquireMode=PatientMeasureMode;
        w.mCfg.frameCorrAMin=variant.frameA;
        w.mCfg.roundCorrAMin=variant.roundA;
        w.mCfg.anglePairMidGapMin=variant.gMin;
        w.mCfg.anglePairMidGapMax=variant.gMax;
        w.mCfg.stableLagWarmupCount=variant.warmup;
        w.mCfg.boneLagUnlockCount=variant.lossCount;
        QTemporaryDir scratch;
        require(scratch.isValid() && w.experimentLog.start(scratch.path(),{}), "Cannot open isolated log");
        const QString path=w.experimentLog.path();
        QVector<QJsonObject> expected;
        QVector<qint64> recordedTimes;
        QVector<double> newlyAccepted;
        qint64 completionMs=0, firstAcceptedMs=-1;
        int consumed=0;
        const bool baseline=variant.name=="baseline";
        for (const auto& frame : rows) {
            if (frame["event"]!="frame") continue;
            if (!w.patientMeasureRunning) break;
            ++consumed;
            expected.append(normalized(frame));
            recordedTimes.append(frame["elapsed_ms"].toInteger());
            bool accepted=false;
            if (raw) {
                w.samplesA=decode(frame,"raw_BC"); w.samplesB=decode(frame,"raw_BD");
                w.samplesC=decode(frame,"raw_AC"); w.samplesD=decode(frame,"raw_AD");
                require(w.samplesA.size()==w.samplesB.size() && w.samplesA.size()==w.samplesC.size() &&
                        w.samplesA.size()==w.samplesD.size(), "Raw channel sizes differ");
                w.plotSamples();
            } else {
                accepted=featureFrame(w,frame,baseline,variant.stabilityBeforeG);
                if (accepted && frame["decision"]!="accepted") newlyAccepted.append(frame["sos_patient"].toDouble());
                if (accepted && firstAcceptedMs<0) firstAcceptedMs=frame["elapsed_ms"].toInteger();
            }
            completionMs=frame["elapsed_ms"].toInteger();
        }
        const bool reachedTarget=!w.patientMeasureRunning;
        w.stopPatientMeasurement();
        const auto actual=readRows(path);
        QVector<QJsonObject> actualFrames;
        QJsonObject summary, originalSummary;
        int discarded=0;
        for (const auto& row : actual) {
            if (row["event"]=="frame") actualFrames.append(normalized(row));
            if (row["event"]=="round_summary") summary=normalized(row);
            if (row["event"]=="discard_partial") discarded+=row["discarded_values"].toInt();
        }
        for (const auto& row : rows) if (row["event"]=="round_summary") originalSummary=normalized(row);
        if (raw) {
            require(expected.size()==actualFrames.size(), "Raw frame count mismatch");
            for (int i=0;i<expected.size();++i) {
                if (baseline)
                    require(equivalent(expected[i],actualFrames[i]), QString("Raw frame mismatch at index %1").arg(i));
                if (variant.dualWindow && expected[i].contains("gates")) {
                    // Cached A score was computed independently from the raw-wave audit.
                    for (const auto* key:{"A","B","D","G","sos_patient"})
                        require(equivalent(expected[i][key],actualFrames[i][key]),
                            QString("Dual raw feature mismatch: %1 at %2").arg(key).arg(i));
                }
                if (actualFrames[i]["decision"]=="accepted") {
                    if (firstAcceptedMs<0) firstAcceptedMs=recordedTimes[i];
                    if (expected[i]["decision"]!="accepted") newlyAccepted.append(actualFrames[i]["sos_patient"].toDouble());
                }
            }
        }
        if (baseline) {
            int recordedCount=0;
            for (const auto& row : rows) if (row["event"]=="frame") ++recordedCount;
            require(consumed==recordedCount && reachedTarget==!originalSummary.isEmpty(), "Baseline completion boundary mismatch");
            require(equivalent(summary,originalSummary), "Baseline round summary mismatch");
        }
        w.closeRoundFinishedTip();
        return {{"consumed_frames",consumed},{"completion_ms_on_recorded_timeline",completionMs},
            {"first_accepted_ms",firstAcceptedMs},{"reached_30",reachedTarget},{"round_summary",summary},
            {"discarded_values",discarded},{"newly_accepted_sos",array(newlyAccepted)}};
    }
};

#ifndef MEASUREMENT_REPLAY_LIBRARY
int main(int argc, char** argv) {
    QApplication app(argc,argv);
    qInstallMessageHandler([](QtMsgType, const QMessageLogContext&, const QString&){});
    try {
        require(argc>=3, "Arguments: input directory, output JSON, -platform offscreen");
        const QDir input(QString::fromLocal8Bit(argv[1]));
        const bool firstWait=app.arguments().contains("--first-wait");
        const bool combinations=app.arguments().contains("--combinations");
        const bool conservative=app.arguments().contains("--conservative-combination");
        const bool observeBeforeG=app.arguments().contains("--observe-before-g");
        require(!observeBeforeG || (!firstWait && !combinations),"Separate flow and threshold comparisons");
        require(!conservative || combinations,"Conservative follow-up requires combination mode");
        require(!(firstWait && combinations),"Select only one research comparison mode");
        require(input.absolutePath()!=QCoreApplication::applicationDirPath(), "Must run in isolated build directory");
        const auto files=input.entryList({"round-*.jsonl"},QDir::Files,QDir::Name);
        require(!files.isEmpty(), "No recordings");
        QVector<QVector<QJsonObject>> recordings;
        QJsonArray manifest;
        for (const auto& name : files) {
            QFile f(input.filePath(name)); require(f.open(QIODevice::ReadOnly), "Read failed");
            manifest.append(QJsonObject{{"file",name},{"sha256",QString::fromLatin1(QCryptographicHash::hash(f.readAll(),QCryptographicHash::Sha256).toHex())}});
            auto rows=readRows(input.filePath(name));
            require(!rows.isEmpty() && rows.first()["event"]=="start" && rows.last()["event"]=="stop", "Incomplete log");
            recordings.append(rows);
        }
        QJsonArray rawBaseline;
        for (int i=0;i<files.size();++i) {
            auto result=MainWindowSafetyTests::runFile(recordings[i],Case{"baseline"},true);
            result["file"]=files[i]; rawBaseline.append(result);
            std::fprintf(stderr,"raw baseline %d/%d verified\n",i+1,int(files.size()));
        }
        // Candidate ranges are research comparisons, never product defaults.
        const QVector<Case> variants=observeBeforeG ? QVector<Case>{{"baseline"},
            {"observe_before_G",.78,.80,-6,6,14,10,true}}
            : conservative ? QVector<Case>{{"baseline"},
            {"G8_warmup12",.78,.80,-8,8,12,10}}
            : combinations ? QVector<Case>{{"baseline"},
            {"G8_warmup10",.78,.80,-8,8,10,10},{"G12_warmup10",.78,.80,-12,12,10,10},
            {"A075_G8",.75,.80,-8,8},{"A075_G12",.75,.80,-12,12},
            {"A075_G8_warmup10",.75,.80,-8,8,10,10},{"A075_G12_warmup10",.75,.80,-12,12,10,10}}
            : firstWait ? QVector<Case>{{"baseline"},
            {"warmup_12",.78,.80,-6,6,12,10},{"warmup_10",.78,.80,-6,6,10,10},
            {"loss_count_15",.78,.80,-6,6,14,15},{"loss_count_20",.78,.80,-6,6,14,20}}
            : QVector<Case>{{"baseline"},{"roundA_078",.78,.78},
            {"frameA_075",.75,.80},{"frameA_080",.80,.80},
            {"G_pm8",.78,.80,-8,8},{"G_pm10",.78,.80,-10,10},
            {"G_pm12_stress",.78,.80,-12,12},{"roundA078_Gpm8",.78,.78,-8,8}};
        QJsonArray scenarios;
        QJsonArray candidateRawChecks;
        for (const auto& variant : variants) {
            QJsonArray attempts;
            QVector<RoundCandidate> candidates;
            QVector<double> sos,a,b;
            QJsonObject firstFive;
            for (int i=0;i<files.size();++i) {
                auto result=MainWindowSafetyTests::runFile(recordings[i],variant,false);
                if (variant.stabilityBeforeG || ((variant.name=="roundA_078" || variant.name=="G_pm8" || variant.name=="G_pm12_stress" ||
                     ((firstWait || combinations) && variant.name!="baseline"))
                    && (i==0 || i==7))) {
                    const auto direct=MainWindowSafetyTests::runFile(recordings[i],variant,true);
                    require(equivalent(result,direct), "Candidate raw/feature mismatch: " + variant.name);
                    candidateRawChecks.append(QJsonObject{{"scenario",variant.name},{"file",files[i]},{"matched",true}});
                }
                result["file"]=files[i];
                const auto summary=result["round_summary"].toObject();
                if (summary["quality_pass"].toBool()) {
                    RoundCandidate c; c.sos=summary["sos"].toDouble();
                    candidates.append(c);
                    Utils::rebuildAcceptedRoundsFromCandidates(candidates,sos,a,b,180.0);
                }
                result["conditional_accepted_rounds"]=array(sos);
                if (sos.size()>=5 && firstFive.isEmpty())
                    firstFive={{"attempt",i+1},{"sos",Utils::trimmedMeanValue(sos,.2)},{"rounds",array(sos)}};
                attempts.append(result);
            }
            scenarios.append(QJsonObject{{"name",variant.name},{"frame_A_min",variant.frameA},
                {"round_A_min",variant.roundA},{"G_min",variant.gMin},{"G_max",variant.gMax},
                {"warmup",variant.warmup},{"loss_count",variant.lossCount},
                {"observe_stability_before_G",variant.stabilityBeforeG},
                {"attempts",attempts},{"conditional_first_five",firstFive}});
            std::fprintf(stderr,"scenario %s complete\n",variant.name.toLocal8Bit().constData());
        }
        QJsonObject output{{"manifest",manifest},{"raw_baseline_verified",true},
            {"feature_baseline_verified",true},{"raw_baseline",rawBaseline},{"scenarios",scenarios},
            {"candidate_raw_checks",candidateRawChecks},
            {"scope","Each attempt stops at its first 30-value round. Reuses recorded hand trajectories and input timing; not prospective hardware time/accuracy. No paired truth."}};
        if (firstWait) {
            QJsonArray featureFiles;
            for (int i=0;i<files.size();++i) {
                featureFiles.append(QJsonObject{{"file",files[i]},{"frames",inspectFeatures(recordings[i])}});
                std::fprintf(stderr,"raw feature inspection %d/%d verified\n",i+1,int(files.size()));
            }
            output["feature_inspection"]=featureFiles;
            output["feature_scope"]="Same-onset G is a diagnostic alternative with no calibrated zero or authorized threshold. Peak correlation is diagnostic only; selected lag/SOS are unchanged. loss_count changes both precheck expiry and cluster loss, not just warmup.";
        }
        if (observeBeforeG) output["flow_scope"]="Experimental product-flow raw replay cross-checked against independent research feature sequencing for all eight recordings. G-failing frames never accepted; original numeric thresholds and round checks unchanged. Not a hardware benefit or accuracy test.";
        QFile outputFile(QString::fromLocal8Bit(argv[2]));
        require(outputFile.open(QIODevice::WriteOnly), "Cannot write aggregate results");
        require(outputFile.write(QJsonDocument(output).toJson())>0, "Write failed");
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr,"Replay failed: %s\n",e.what()); return 1;
    }
}
#endif
