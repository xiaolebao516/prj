// Raw-wave research only. Reuse audited signal helpers, not the legacy runner.
#define main legacy_landmark_main
#include "g_landmark_audit.cpp"
#undef main
#include <QSet>

static QJsonObject peak(const QVector<double>& e,const QVector<double>& l,int onset,int low,int high) {
    double best=-2;int lag=low;
    for(int k=low;k<=high;++k){const double c=corr(e,l,onset,k,-20,120);if(c>best){best=c;lag=k;}}
    return {{"lag",lag},{"corr",best},{"edge",lag==low||lag==high}};
}
static QJsonObject platform(const QVector<double>& e,const QVector<double>& l,int onset,int low,int high,const QJsonObject& pk) {
    const double alpha=qBound(0.0,(pk["lag"].toInt()-106.0)/99.0,1.0),ratio=.92+alpha*.03;
    QVector<int> band;int components=0,previous=-2;
    for(int k=low;k<=high;++k)if(corr(e,l,onset,k,-20,120)>=ratio*pk["corr"].toDouble()){
        if(k!=previous+1)++components;
        band.append(k);previous=k;
    }
    require(!band.isEmpty(),"Missing correlation platform");
    const int lag=band[qBound(0,int(std::round(alpha*(band.size()-1))),int(band.size()-1))];
    return {{"lag",lag},{"corr",corr(e,l,onset,lag,-20,120)},{"components",components}};
}
static QJsonArray candidates(const QVector<double>& wave,int start,int end) {
    const auto list=valleys(wave,start,end);double maximum=0;QJsonArray out;
    for(const auto& v:list)maximum=qMax(maximum,v.depth);
    for(const auto& v:list)out.append(QJsonObject{{"index",v.index},{"depth",v.depth},{"relative",v.depth/maximum}});
    return out;
}
int main(int argc,char** argv) {
    QCoreApplication app(argc,argv);qInstallMessageHandler([](QtMsgType,const QMessageLogContext&,const QString&){});
    try {
        require(argc==4 || (argc==5 && QString::fromLocal8Bit(argv[4])=="--all-afternoon-gates"),"Usage: targeted_wave_audit manifest.json input-directory output.json [--all-afternoon-gates]");
        const bool full=argc==5;
        QFile mf(QString::fromLocal8Bit(argv[1]));require(mf.open(QIODevice::ReadOnly),"Cannot read manifest");
        const auto manifest=QJsonDocument::fromJson(mf.readAll()).object();
        SignalProcessor p;p.designFIR(1250000,600000,62500000);
        const GateConfig ga{0,15,18,480,550},gb{20,105,115,500,560};QJsonArray output;
        for(const auto& item:manifest["files"].toArray()) {
            const auto entry=item.toObject();QSet<int> selected;
            if(full && entry["local"].toString()<"2026-09-07T16:00")continue;
            for(const auto& n:entry["sequences"].toArray())selected.insert(n.toInt());
            QFile f(QDir(QString::fromLocal8Bit(argv[2])).filePath(entry["file"].toString()));
            require(f.open(QIODevice::ReadOnly),"Cannot read source");
            require(QFileInfo(f).absoluteFilePath()!=QFileInfo(QString::fromLocal8Bit(argv[3])).absoluteFilePath(),"Output equals source");
            QCryptographicHash hash(QCryptographicHash::Sha256);
            require(hash.addData(&f),"Hash read failed");
            require(QString::fromLatin1(hash.result().toHex())==entry["sha256"].toString().toLower(),"Source hash changed");
            f.seek(0);QJsonArray frames;int found=0;QVector<double> previousAD,previousAC;int previousSequence=-1;QJsonObject previousRow;
            while(!f.atEnd()) {
                const auto line=f.readLine(1024*1024);
                if(line.contains('\0')||!line.endsWith('\n'))break;
                QJsonParseError error;auto row=QJsonDocument::fromJson(line,&error).object();
                require(error.error==QJsonParseError::NoError,"Invalid JSON");
                if(row["event"]=="start") {
                    const auto c=row["config"].toObject();p.probeDistanceCD=c["probe_distance_m"].toDouble();p.samplePeriod=c["sample_period_s"].toDouble();
                    require(c["implementation"]=="dual-window-a078-20260906-v1","Wrong profile");
                }
                int seq=row["sequence"].toInt();
                if(full ? !(row["event"]=="frame"&&row.contains("gates")) : !selected.contains(seq))continue;
                require(row["event"]=="frame"&&row.contains("gates"),"Selected frame missing gates");++found;
                const auto ad=p.applyFIRDouble(SignalProcessor::preprocessRawForFIR(decode(row,"raw_AD"),ga,"AD"));
                const auto ac=p.applyFIRDouble(SignalProcessor::preprocessRawForFIR(decode(row,"raw_AC"),ga,"AC"));
                const auto bd=p.applyFIRDouble(SignalProcessor::preprocessRawForFIR(decode(row,"raw_BD"),gb,"BD"));
                const auto bc=p.applyFIRDouble(SignalProcessor::preprocessRawForFIR(decode(row,"raw_BC"),gb,"BC"));
                const auto pick=[](const QVector<double>& x){return SignalProcessor::detectFirstArrivalSmart(x,560,615,620,qMin(int(x.size())-1,1600),4,8,20,.2,140);};
                const auto be=pick(bd),bl=pick(bc),ae=pick(ad);
                const auto b=p.estimatePairSpeed(bd,bc,be,bl,1800,5000,"auditB");
                const auto loggedB=row["B"].toObject(),loggedA=row["A"].toObject();
                require(b.valid&&b.refinedLag==loggedB["lag"].toInt()&&b.roughLag==loggedB["rough_lag"].toInt()&&b.earlyOnset==loggedB["early_feature"].toInt()&&b.lateOnset==loggedB["late_feature"].toInt()&&std::abs(b.corr-loggedB["corr"].toDouble())<1e-8,"B raw mismatch "+QString::number(seq));
                QJsonObject r{{"sequence",seq},{"elapsed_ms",row["elapsed_ms"]},{"decision",row["decision"]},{"G",row["G"]},{"D",row["D"]},{"gates",row["gates"]},{"A",loggedA},{"B",loggedB},{"branch",row["A_feature_branch"]}};
                int searchStart=620,searchEnd=qMin(1300,int(ad.size())-2);
                auto v=SignalProcessor::findFirstProminentValley(ad,searchStart,searchEnd,30,.30,20);
                if(!v.valid&&ae.valid){searchStart=ae.onset-60;searchEnd=ae.onset+150;}
                if(row["A_feature_branch"]=="valley") {
                    const auto a=p.estimatePairSpeedByValley(ad,ac,ae,b.refinedLag,20,"auditA");
                    require(a.valid&&a.earlyOnset==loggedA["early_feature"].toInt()&&a.lateOnset==loggedA["late_feature"].toInt()&&a.refinedLag==loggedA["lag"].toInt(),"A raw mismatch "+QString::number(seq));
                    const double q=qMin(corr(ad,ac,a.earlyOnset,a.refinedLag,-20,30),corr(ad,ac,a.earlyOnset,a.refinedLag,0,60));
                    require(std::abs(q-loggedA["corr"].toDouble())<1e-8,"A dual quality mismatch");
                    r["AD_candidates"]=candidates(ad,searchStart,searchEnd);
                    r["AD_arrival"]=ae.onset;
                    if(previousSequence==seq-1) {
                        const double adSimilarity=corr(previousAD,ad,0,0,620,1300);
                        const double acSimilarity=corr(previousAC,ac,0,0,620,1500);
                        r["AD_same_coordinate_corr"]=adSimilarity;r["AC_same_coordinate_corr"]=acSimilarity;
                        const auto oldA=previousRow["A"].toObject(),oldB=previousRow["B"].toObject();
                        const auto list=valleys(ad,searchStart,searchEnd);double maximum=0;for(const auto& c:list)maximum=qMax(maximum,c.depth);
                        int anchor=-1;double relative=0;int distance=4;
                        for(const auto& c:list)if(std::abs(c.index-oldA["early_feature"].toInt())<distance){distance=std::abs(c.index-oldA["early_feature"].toInt());anchor=c.index;relative=c.depth/maximum;}
                        if(anchor>=0&&anchor!=a.earlyOnset) {
                            const auto late=SignalProcessor::findFirstProminentValley(ac,anchor+b.refinedLag-20,anchor+b.refinedLag+20,25,.25,20);
                            if(late.valid) {
                                double originalQuality=0;const int vl=late.idx-anchor;
                                const int lag=SignalProcessor::refineLagByPositiveCrossCorrelation(ad,ac,anchor,vl,qMax(1,vl-15),vl+15,20,90,&originalQuality);
                                const double quality=qMin(corr(ad,ac,anchor,lag,-20,30),corr(ad,ac,anchor,lag,0,60));
                                const double g=.5*(b.earlyOnset+b.lateOnset-anchor-late.idx);const int d=lag-b.refinedLag;
                                const bool seed=previousRow["gates"].toObject()["all_prechecks_passed"].toBool();
                                const bool continuity=seed&&adSimilarity>=.995&&acSimilarity>=.995&&relative>=.28&&std::abs(b.refinedLag-oldB["lag"].toInt())<=1&&row["elapsed_ms"].toInt()-previousRow["elapsed_ms"].toInt()<=200;
                                r["one_step_trusted_anchor"]=QJsonObject{{"early",anchor},{"late",late.idx},{"relative",relative},{"A_lag",lag},{"A_quality",quality},{"D",d},{"G",g},{"previous_prechecks_passed",seed},{"continuity_eligible",continuity},{"A_D_G_pass",quality>=.78&&d>=5&&d<=15&&g>=-6&&g<=6}};
                            }
                        }
                    }
                }
                if(!full) {
                const int physicalLow=qMax(1,int(std::floor(p.probeDistanceCD/(5000*p.samplePeriod)))-5);
                const int physicalHigh=qMax(physicalLow+1,int(std::ceil(p.probeDistanceCD/(1800*p.samplePeriod)))+5);
                int low=physicalLow,high=physicalHigh;
                if(b.roughLag>=low&&b.roughLag<=high){int width=qBound(30,int(.25*std::abs(b.roughLag)),55);low=qMax(low,b.roughLag-width);high=qMin(high,b.roughLag+width);}
                else if(b.roughLag<low&&b.roughLag>=low-35&&b.roughLag>=60)high=qMin(high,b.roughLag+55);
                r["B_search_low"]=low;r["B_search_high"]=high;r["B_peak"]=peak(bd,bc,be.onset,low,high);
                if(r["B_peak"].toObject()["edge"].toBool()) {
                    const auto control=platform(bd,bc,be.onset,low,high,r["B_peak"].toObject());
                    require(control["lag"].toInt()==b.refinedLag&&std::abs(control["corr"].toDouble()-b.corr)<1e-8,"Independent B platform mismatch");
                    const int extendedLow=qMax(physicalLow,low-15),extendedHigh=qMin(physicalHigh,high+15);
                    const auto extended=peak(bd,bc,be.onset,extendedLow,extendedHigh);
                    r["B_peak_extended15"]=extended;
                    r["B_extended_same_platform"]=platform(bd,bc,be.onset,extendedLow,extendedHigh,extended);
                }
                }
                previousAD=ad;previousAC=ac;previousRow=row;previousSequence=seq;frames.append(r);
            }
            if(!full) require(found==selected.size(),"Missing selected frames");
            output.append(QJsonObject{{"file",entry["file"]},{"sha256",entry["sha256"]},{"kept_sequences",entry["kept_sequences"]},{"frames",frames}});
            std::fprintf(stderr,"Raw validated %d selected frames: %s\n",found,qPrintable(entry["file"].toString()));
        }
        QFile out(QString::fromLocal8Bit(argv[3]));require(out.open(QIODevice::WriteOnly|QIODevice::NewOnly),"Output must be new");
        const auto bytes=QJsonDocument(QJsonObject{{"files",output}}).toJson();require(out.write(bytes)==bytes.size()&&out.flush(),"Output write failed");
    }catch(const std::exception& e){std::fprintf(stderr,"Failed: %s\n",e.what());return 1;}
    return 0;
}
