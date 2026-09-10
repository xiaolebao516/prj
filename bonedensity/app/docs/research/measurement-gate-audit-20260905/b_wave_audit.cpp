// Isolated B-lag diagnostics, not a candidate product estimator.
#include "signalprocessor.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
#include <QtEndian>
#include <cmath>
#include <cstdio>
#include <stdexcept>

static void require(bool ok,const QString& why) {if(!ok)throw std::runtime_error(why.toStdString());}
static QVector<quint16> decode(const QJsonObject& row,const char* key) {
    const auto b=QByteArray::fromBase64Encoding(row[key].toString().toLatin1(),QByteArray::AbortOnBase64DecodingErrors);
    require(bool(b)&&!b.decoded.isEmpty()&&b.decoded.size()%2==0,"Invalid raw data");
    QVector<quint16> v(b.decoded.size()/2);
    for(qsizetype i=0;i<v.size();++i)v[i]=qFromLittleEndian<quint16>(b.decoded.constData()+i*2);
    return v;
}
static double corr(const QVector<double>& a,const QVector<double>& b,int onset,int lag,int lo,int hi) {
    double ma=0,mb=0,aa=0,bb=0,ab=0;int count=0;
    const int n=qMin(a.size(),b.size()),start=qMax(0,onset+lo),end=qMin(n-1,onset+hi);
    for(int i=start;i<=end;++i)if(i+lag>=0&&i+lag<n){ma+=a[i];mb+=b[i+lag];++count;}
    if(count<10)return -2;
    ma/=count;mb/=count;
    for(int i=start;i<=end;++i)if(i+lag>=0&&i+lag<n){double x=a[i]-ma,y=b[i+lag]-mb;aa+=x*x;bb+=y*y;ab+=x*y;}
    return aa<1e-12||bb<1e-12?-2:ab/std::sqrt(aa*bb);
}
static QJsonObject scan(const QVector<double>& a,const QVector<double>& b,int onset,int low,int high,int lo,int hi) {
    QVector<double> values;double peak=-2;int lag=low;
    for(int k=low;k<=high;++k){const double c=corr(a,b,onset,k,lo,hi);values.append(c);if(c>peak){peak=c;lag=k;}}
    QJsonArray maxima;
    for(int i=0;i<values.size();++i)if(values[i]>0&&(i==0||values[i]>values[i-1])&&(i+1==values.size()||values[i]>=values[i+1]))
        maxima.append(QJsonObject{{"lag",low+i},{"corr",values[i]}});
    int left=lag,right=lag;
    if(peak>0){while(left>low&&values[left-1-low]>=.95*peak)--left;while(right<high&&values[right+1-low]>=.95*peak)++right;}
    return {{"peak_lag",lag},{"peak_corr",peak},{"edge",lag==low||lag==high},
        {"width95",right-left+1},{"local_maxima",maxima}};
}
int main(int argc,char** argv) {
    QCoreApplication app(argc,argv);qInstallMessageHandler([](QtMsgType,const QMessageLogContext&,const QString&){});
    try {
        require(argc>=3,"Usage: b_wave_audit output.json input-dirs...");
        SignalProcessor p;p.designFIR(1250000,600000,62500000);
        const GateConfig gate{20,105,115,500,560};QJsonArray files;
        for(int arg=2;arg<argc;++arg){QDir dir(QString::fromLocal8Bit(argv[arg]));
            for(const auto& name:dir.entryList({"round-*.jsonl"},QDir::Files,QDir::Name)){
                QFile file(dir.filePath(name));require(file.open(QIODevice::ReadOnly),"Cannot read log");
                require(QFileInfo(file).absoluteFilePath()!=QFileInfo(QString::fromLocal8Bit(argv[1])).absoluteFilePath(),"Output equals input");
                const auto bytes=file.readAll();QVector<QJsonObject> rows;QSet<int> kept;int seq=0;QJsonObject config;
                for(const auto& line:bytes.split('\n')){if(line.trimmed().isEmpty())continue;
                    QJsonParseError error;const auto doc=QJsonDocument::fromJson(line,&error);
                    require(error.error==QJsonParseError::NoError&&doc.isObject(),"Invalid JSON");
                    const auto r=doc.object();require(r["sequence"].toInt()==++seq,"Sequence gap");rows.append(r);
                    if(r["event"]=="start")config=r["config"].toObject();
                    if(r["event"]=="discard_partial"){require(r["discarded_values"].toInt()==kept.size(),"Discard count mismatch");kept.clear();}
                    if(r["event"]=="frame"&&r["decision"]=="accepted")kept.insert(seq);
                }
                require(!rows.isEmpty()&&rows.first()["event"]=="start"&&rows.last()["event"]=="stop","Incomplete log");
                p.probeDistanceCD=config["probe_distance_m"].toDouble();p.samplePeriod=config["sample_period_s"].toDouble();
                require(p.probeDistanceCD>0&&p.samplePeriod>0,"Bad physics config");
                const int physicalMin=qMax(1,int(std::floor(p.probeDistanceCD/(5000*p.samplePeriod)))-5);
                const int physicalMax=qMax(physicalMin+1,int(std::ceil(p.probeDistanceCD/(1800*p.samplePeriod)))+5);
                QJsonArray frames;int invalid=0;
                for(const auto& r:rows){if(r["event"]!="frame")continue;
                    const auto bd=p.applyFIRDouble(SignalProcessor::preprocessRawForFIR(decode(r,"raw_BD"),gate,"BD"));
                    const auto bc=p.applyFIRDouble(SignalProcessor::preprocessRawForFIR(decode(r,"raw_BC"),gate,"BC"));
                    const auto pick=[](const QVector<double>& x){return SignalProcessor::detectFirstArrivalSmart(x,560,615,620,qMin(int(x.size())-1,1600),4,8,20,.2,140);};
                    const auto early=pick(bd),late=pick(bc);
                    const auto b=p.estimatePairSpeed(bd,bc,early,late,1800,5000,"audit B");
                    const auto logged=r["B"].toObject();require(b.valid==logged["valid"].toBool(),"B validity mismatch: "+name+":"+QString::number(r["sequence"].toInt()));
                    if(!b.valid){++invalid;continue;}
                    require(b.earlyOnset==logged["early_feature"].toInt()&&b.lateOnset==logged["late_feature"].toInt()&&b.roughLag==logged["rough_lag"].toInt()&&b.refinedLag==logged["lag"].toInt()&&std::abs(b.corr-logged["corr"].toDouble())<1e-8&&std::abs(b.sos-logged["sos"].toDouble())<1e-8,"B raw reconstruction mismatch");
                    int low=physicalMin,high=physicalMax;
                    if(b.roughLag>=low&&b.roughLag<=high){int range=qBound(30,int(.25*std::abs(b.roughLag)),55);low=qMax(low,b.roughLag-range);high=qMin(high,b.roughLag+range);}
                    else if(b.roughLag<low&&b.roughLag>=low-35&&b.roughLag>=60)high=qMin(high,b.roughLag+55);
                    const auto full=scan(bd,bc,early.onset,low,high,-20,120);
                    const int peak=full["peak_lag"].toInt();double alpha=qBound(0.0,(peak-106.0)/99.0,1.0),ratio=.92+alpha*.03;
                    QVector<int> plateau;int components=0,previous=-2;
                    for(int k=low;k<=high;++k)if(corr(bd,bc,early.onset,k,-20,120)>=ratio*full["peak_corr"].toDouble()){
                        if(k!=previous+1)++components;
                        previous=k;plateau.append(k);
                    }
                    require(!plateau.isEmpty(),"Missing positive plateau");
                    const int selected=plateau[qBound(0,int(std::round(alpha*(plateau.size()-1))),int(plateau.size()-1))];
                    require(selected==b.refinedLag&&std::abs(corr(bd,bc,early.onset,selected,-20,120)-b.corr)<1e-8,"Independent plateau mismatch");
                    QJsonObject out{{"sequence",r["sequence"]},{"elapsed_ms",r["elapsed_ms"]},{"kept",kept.contains(r["sequence"].toInt())},
                        {"decision",r["decision"]},{"gates",r["gates"]},{"B",logged},{"G",r["G"]},{"D",r["D"]},
                        {"early_peak",early.peak},{"late_peak",late.peak},{"search_min",low},{"search_max",high},
                        {"full",full},{"plateau_first",plateau.first()},{"plateau_last",plateau.last()},{"plateau_components",components},
                        {"front",scan(bd,bc,early.onset,low,high,-20,60)},{"tail",scan(bd,bc,early.onset,low,high,40,120)},
                        {"long",scan(bd,bc,early.onset,low,high,-20,200)},
                        {"extended15",scan(bd,bc,early.onset,qMax(physicalMin,low-15),qMin(physicalMax,high+15),-20,120)},
                        {"physical",scan(bd,bc,early.onset,physicalMin,physicalMax,-20,120)},
                        {"onset_minus10",scan(bd,bc,early.onset-10,low,high,-20,120)},
                        {"onset_plus10",scan(bd,bc,early.onset+10,low,high,-20,120)}};
                    // Sensitivity only: identical filtered waves and early window;
                    // perturb only the rough-lag hint. Not a physical-pose simulation.
                    for(int delta:{-5,5}){
                        double quality=0;
                        const int lag=SignalProcessor::refineLagByPositiveCrossCorrelation(
                            bd,bc,early.onset,b.roughLag+delta,physicalMin,physicalMax,20,120,&quality);
                        out[delta<0?"rough_minus5":"rough_plus5"]=QJsonObject{{"lag",lag},{"corr",quality}};
                    }
                    frames.append(out);
                }
                files.append(QJsonObject{{"file",name},{"source",dir.filePath(name)},
                    {"sha256",QString::fromLatin1(QCryptographicHash::hash(bytes,QCryptographicHash::Sha256).toHex())},
                    {"config",config},{"invalid_B",invalid},{"frames",frames}});
                std::fprintf(stderr,"Verified %d B frames, %d invalid: %s\n",int(frames.size()),invalid,qPrintable(name));
            }
        }
        QFile output(QString::fromLocal8Bit(argv[1]));require(output.open(QIODevice::WriteOnly),"Cannot write output");
        const auto data=QJsonDocument(QJsonObject{{"files",files},{"scope","Research-only raw B reconstruction and fixed-window correlation-peak diagnostics. Source-profile accepted/kept frames are not candidate replay. No product change or paired accuracy claim."}}).toJson();
        require(output.write(data)==data.size(),"Short output write");
    }catch(const std::exception& e){std::fprintf(stderr,"Failed: %s\n",e.what());return 1;}
    return 0;
}
