// Diagnostic landmark comparisons only. Product lag/SOS and source stay unchanged.
#include "signalprocessor.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtEndian>
#include <cmath>
#include <cstdio>
#include <stdexcept>

static void require(bool ok,const QString& text) {if(!ok)throw std::runtime_error(text.toStdString());}
static QVector<quint16> decode(const QJsonObject& row,const char* key) {
    const auto d=QByteArray::fromBase64Encoding(row[key].toString().toLatin1(),QByteArray::AbortOnBase64DecodingErrors);
    require(bool(d)&&!d.decoded.isEmpty()&&d.decoded.size()%2==0,"Bad raw data");
    QVector<quint16> out(d.decoded.size()/2);
    for(qsizetype i=0;i<out.size();++i)out[i]=qFromLittleEndian<quint16>(d.decoded.constData()+2*i);
    return out;
}
struct Valley {int index;double depth;};
static QVector<Valley> valleys(const QVector<double>& x,int start,int end) {
    QVector<Valley> raw,merged;
    start=qBound(1,start,int(x.size())-2);end=qBound(1,end,int(x.size())-2);
    for(int i=start;i<=end;++i)
        if(x[i]<x[i-1]&&x[i]<=x[i+1]&&-x[i]>=30)raw.append({i,-x[i]});
    for(int i=0;i<raw.size();) {
        auto best=raw[i];int j=i+1;
        while(j<raw.size()&&raw[j].index-raw[j-1].index<=20) {if(raw[j].depth>best.depth)best=raw[j];++j;}
        merged.append(best);i=j;
    }
    return merged;
}
static double corr(const QVector<double>& a,const QVector<double>& b,int onset,int lag,int lo,int hi) {
    if(onset+lo<0||onset+hi>=a.size()||onset+lo+lag<0||onset+hi+lag>=b.size())return -2;
    double ma=0,mb=0,aa=0,bb=0,ab=0;const int n=hi-lo+1;
    for(int i=onset+lo;i<=onset+hi;++i){ma+=a[i];mb+=b[i+lag];}ma/=n;mb/=n;
    for(int i=onset+lo;i<=onset+hi;++i){double x=a[i]-ma,y=b[i+lag]-mb;aa+=x*x;bb+=y*y;ab+=x*y;}
    return aa<1e-12||bb<1e-12?-2:ab/std::sqrt(aa*bb);
}
int main(int argc,char** argv) {
    QCoreApplication app(argc,argv);qInstallMessageHandler([](QtMsgType,const QMessageLogContext&,const QString&){});
    try {
        require(argc>=3,"Usage: g_landmark_audit output.json input-dirs...");
        SignalProcessor p;p.designFIR(1250000,600000,62500000);
        const GateConfig gate{0,15,18,480,550};QJsonArray files;
        for(int arg=2;arg<argc;++arg) {
            QDir directory(QString::fromLocal8Bit(argv[arg]));
            for(const auto& name:directory.entryList({"round-*.jsonl"},QDir::Files,QDir::Name)) {
                QFile file(directory.filePath(name));require(file.open(QIODevice::ReadOnly),"Cannot read log");
                const auto data=file.readAll();QJsonArray frames;QJsonObject config;
                int held=-1,misses=0;double previousBMid=0;
                for(const auto& line:data.split('\n')) {
                    if(line.trimmed().isEmpty())continue;
                    const auto row=QJsonDocument::fromJson(line).object();
                    if(row["event"]=="start")config=row["config"].toObject();
                    if(row["event"]!="frame")continue;
                    if(!row.contains("gates")||row["A_feature_branch"]!="valley") {if(++misses>=10)held=-1;continue;}
                    const auto ad=p.applyFIRDouble(SignalProcessor::preprocessRawForFIR(decode(row,"raw_AD"),gate,"AD"));
                    const auto ac=p.applyFIRDouble(SignalProcessor::preprocessRawForFIR(decode(row,"raw_AC"),gate,"AC"));
                    int searchStart=620,searchEnd=qMin(1300,int(ad.size())-2);
                    auto original=SignalProcessor::findFirstProminentValley(ad,searchStart,searchEnd,30,.30,20);
                    if(!original.valid) {
                        const auto pick=SignalProcessor::detectFirstArrivalSmart(ad,560,615,620,qMin(int(ad.size())-1,1600),4,8,20,.2,140);
                        require(pick.valid,"Fallback onset invalid");searchStart=pick.onset-60;searchEnd=pick.onset+150;
                        original=SignalProcessor::findFirstProminentValley(ad,searchStart,searchEnd,30,.30,20);
                    }
                    const auto list=valleys(ad,searchStart,searchEnd);require(!list.isEmpty(),"No AD valleys");
                    double maxDepth=0;for(const auto& v:list)maxDepth=qMax(maxDepth,v.depth);
                    require(original.valid&&original.idx==row["A"].toObject()["early_feature"].toInt(),"Original AD feature mismatch");
                    int originalCopy=-1,quarter=-1;
                    QJsonArray candidates;
                    for(const auto& v:list) {
                        candidates.append(QJsonObject{{"idx",v.index},{"depth",v.depth},{"ratio",v.depth/maxDepth}});
                        if(originalCopy<0&&v.depth>=qMax(30.0,.30*maxDepth))originalCopy=v.index;
                        if(quarter<0&&v.depth>=qMax(30.0,.25*maxDepth))quarter=v.index;
                    }
                    require(originalCopy==original.idx&&quarter>=0,"Independent valley merge mismatch");
                    const auto a=row["A"].toObject(),b=row["B"].toObject();
                    const double bmid=.5*(b["early_feature"].toInt()+b["late_feature"].toInt());
                    int tracked=original.idx;
                    if(held>=0) {
                        const double predicted=held+bmid-previousBMid;double best=21;
                        for(const auto& v:list) if(v.depth>=qMax(30.0,.25*maxDepth)&&std::abs(v.index-predicted)<best) {
                            tracked=v.index;best=std::abs(v.index-predicted);
                        }
                    }
                    held=tracked;previousBMid=bmid;misses=0;
                    const auto proposal=[&](int early) {
                        const int pred=early+b["lag"].toInt();
                        auto late=SignalProcessor::findFirstProminentValley(ac,pred-20,pred+20,25,.25,20);
                        double quality=qMin(corr(ad,ac,early,a["lag"].toInt(),-20,30),corr(ad,ac,early,a["lag"].toInt(),0,60));
                        QJsonObject out{{"early",early},{"late_valid",late.valid},{"quality_at_unchanged_A_lag",quality}};
                        if(late.valid){out["late"]=late.idx;out["G"]=bmid-.5*(early+late.idx);}
                        return out;
                    };
                    frames.append(QJsonObject{{"sequence",row["sequence"]},{"elapsed_ms",row["elapsed_ms"]},
                        {"G",row["G"]},{"sos",row["sos_patient"]},{"decision",row["decision"]},{"gates",row["gates"]},
                        {"A",a},{"B",b},{"AD_candidates",candidates},{"depth_required",qMax(30.0,.30*maxDepth)},
                        {"quarter",proposal(quarter)},{"tracked",proposal(tracked)}});
                }
                files.append(QJsonObject{{"file",name},{"source",directory.filePath(name)},
                    {"sha256",QString::fromLatin1(QCryptographicHash::hash(data,QCryptographicHash::Sha256).toHex())},
                    {"config",config},{"frames",frames}});
                std::fprintf(stderr,"Audited %s: %d valley frames\n",qPrintable(name),int(frames.size()));
            }
        }
        QFile out(QString::fromLocal8Bit(argv[1]));require(out.open(QIODevice::WriteOnly),"Cannot write diagnostic output");
        const auto bytes=QJsonDocument(QJsonObject{{"files",files},{"scope","Research probes only. quarter uses AD depth0.25; tracked retains a nearby previous valley with B-midpoint shift, within20 samples and depth0.25, otherwise current0.30 choice; clears after10 unavailable frames. Neither uses G centering or target SOS to select valleys. Existing A lag/SOS are unchanged; proposals must also pass their local quality before counterfactual gating."}}).toJson();
        require(out.write(bytes)==bytes.size(),"Short write");
    }catch(const std::exception& e){std::fprintf(stderr,"Failed: %s\n",e.what());return 1;}return 0;
}
