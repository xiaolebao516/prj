// Research only: reconstruct logged features with unchanged product code.
// Window probes do not change lag, thresholds, gating, or patient results.
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

static void require(bool ok, const QString& why) {
    if (!ok) throw std::runtime_error(why.toStdString());
}
static QVector<quint16> decode(const QJsonObject& row, const char* key) {
    const auto b = QByteArray::fromBase64Encoding(row[key].toString().toLatin1(), QByteArray::AbortOnBase64DecodingErrors);
    require(bool(b) && !b.decoded.isEmpty() && b.decoded.size()%2==0, "Bad raw encoding");
    QVector<quint16> out(b.decoded.size()/2);
    for (qsizetype i=0;i<out.size();++i) out[i]=qFromLittleEndian<quint16>(b.decoded.constData()+2*i);
    return out;
}
static double corr(const QVector<double>& a, const QVector<double>& b, int onset, int lag, int lo, int hi) {
    double ma=0,mb=0,aa=0,bb=0,ab=0; int n=0;
    for (int i=onset+lo;i<=onset+hi;++i) if (i>=0 && i<a.size() && i+lag>=0 && i+lag<b.size()) {
        ma+=a[i]; mb+=b[i+lag]; ++n;
    }
    if (n<10) return -2;
    ma/=n; mb/=n;
    for (int i=onset+lo;i<=onset+hi;++i) if (i>=0 && i<a.size() && i+lag>=0 && i+lag<b.size()) {
        const double x=a[i]-ma,y=b[i+lag]-mb; aa+=x*x; bb+=y*y; ab+=x*y;
    }
    return aa<1e-12 || bb<1e-12 ? -2 : ab/std::sqrt(aa*bb);
}
static double rms(const QVector<double>& x, int start, int end) {
    double mean=0,sum=0; start=qMax(0,start); end=qMin(int(x.size())-1,end);
    require(end>=start,"Bad RMS bounds");
    for (int i=start;i<=end;++i) mean+=x[i];
    mean/=(end-start+1);
    for (int i=start;i<=end;++i) sum+=(x[i]-mean)*(x[i]-mean);
    return std::sqrt(sum/(end-start+1));
}
static QJsonObject window(const QVector<double>& ad, const QVector<double>& ac, const PairResult& a, int lo, int hi) {
    double peak=-2; int lag=0;
    for (int k=qMax(1,a.roughLag-15);k<=a.roughLag+15;++k) {
        double c=corr(ad,ac,a.earlyOnset,k,lo,hi);
        if (c>peak) {peak=c;lag=k;}
    }
    return {{"fixed_lag_corr",corr(ad,ac,a.earlyOnset,a.refinedLag,lo,hi)},
        {"peak_corr",peak},{"peak_lag",lag},
        {"AD_rms",rms(ad,a.earlyOnset+lo,a.earlyOnset+hi)},
        {"AC_rms",rms(ac,a.earlyOnset+a.refinedLag+lo,a.earlyOnset+a.refinedLag+hi)}};
}
int main(int argc,char** argv) {
    QCoreApplication app(argc,argv);
    qInstallMessageHandler([](QtMsgType,const QMessageLogContext&,const QString&){});
    try {
        require(argc==4,"Usage: a_wave_audit output.json original-log-dir failed-log.jsonl");
        const QDir old(QString::fromLocal8Bit(argv[2]));
        QStringList paths;
        for (const auto& name:old.entryList({"round-*.jsonl"},QDir::Files,QDir::Name)) paths.append(old.filePath(name));
        require(paths.size()==8,"Expected the original eight recordings");
        paths.append(QString::fromLocal8Bit(argv[3]));
        SignalProcessor p; p.designFIR(1250000,600000,62500000);
        const GateConfig aGate{0,15,18,480,550};
        QJsonArray results;
        for (const auto& path:paths) {
            QFile file(path); require(file.open(QIODevice::ReadOnly),"Cannot read input");
            const auto data=file.readAll(); QJsonArray frames; int total=0,verified=0,fallback=0,seq=0;
            QJsonObject config; QString lastEvent;
            for (const auto& line:data.split('\n')) {
                if (line.trimmed().isEmpty()) continue;
                QJsonParseError error; const auto doc=QJsonDocument::fromJson(line,&error);
                require(error.error==QJsonParseError::NoError && doc.isObject(),"Bad JSON");
                const auto r=doc.object(); require(r["sequence"].toInt()==++seq,"Sequence gap");
                lastEvent=r["event"].toString();
                if (seq==1) {require(lastEvent=="start","Missing start"); config=r["config"].toObject();}
                if (lastEvent!="frame") continue;
                ++total; if (!r.contains("gates")) continue;
                if (r["A_feature_branch"]!="valley") {++fallback;continue;}
                auto rawAD=decode(r,"raw_AD"),rawAC=decode(r,"raw_AC");
                auto ad=p.applyFIRDouble(SignalProcessor::preprocessRawForFIR(rawAD,aGate,"AD"));
                auto ac=p.applyFIRDouble(SignalProcessor::preprocessRawForFIR(rawAC,aGate,"AC"));
                const auto pick=[](const QVector<double>& x) {
                    return SignalProcessor::detectFirstArrivalSmart(x,560,615,620,qMin(int(x.size())-1,1600),4,8,20,.2,140);
                };
                auto pad=pick(ad),pac=pick(ac); const auto logged=r["A"].toObject(),b=r["B"].toObject();
                p.probeDistanceCD=config["probe_distance_m"].toDouble(); p.samplePeriod=config["sample_period_s"].toDouble();
                auto a=p.estimatePairSpeedByValley(ad,ac,pad,b["lag"].toInt(),20,"audit");
                require(a.valid && a.earlyOnset==logged["early_feature"].toInt() && a.lateOnset==logged["late_feature"].toInt() &&
                    a.roughLag==logged["rough_lag"].toInt() && a.refinedLag==logged["lag"].toInt() &&
                    std::abs(a.corr-logged["corr"].toDouble())<1e-8 && std::abs(a.sos-logged["sos"].toDouble())<1e-8,
                    "Product A reconstruction mismatch at "+QString::number(seq));
                const auto original=window(ad,ac,a,-20,90);
                // Product stores zero if no positive correlation exists.
                const double c=original["fixed_lag_corr"].toDouble();
                require(std::abs(c-a.corr)<1e-8 || (a.corr==0 && original["peak_corr"].toDouble()<=0),"Independent correlation mismatch");
                ++verified;
                QJsonObject out{{"sequence",seq},{"elapsed_ms",r["elapsed_ms"]},{"gates",r["gates"]},
                    {"decision",r["decision"]},{"D",r["D"]},{"G",r["G"]},{"B",b},{"A",logged},
                    {"AD_arrival",pad.onset},{"AC_arrival",pac.onset},{"AD_peak",pad.peak},{"AC_peak",pac.peak},
                    {"AD_noise_rms",rms(ad,560,615)},{"AC_noise_rms",rms(ac,560,615)},
                    {"original",original},{"front",window(ad,ac,a,-20,30)},
                    {"middle",window(ad,ac,a,0,60)},{"tail",window(ad,ac,a,40,90)},
                    {"long",window(ad,ac,a,-20,140)}};
                int clipAD=0,clipAC=0;
                for (int k=-20;k<=140;++k) {
                    int i=a.earlyOnset+k,j=i+a.refinedLag;
                    if (i>=0 && i<rawAD.size() && (rawAD[i]==0 || rawAD[i]>=4095)) ++clipAD;
                    if (j>=0 && j<rawAC.size() && (rawAC[j]==0 || rawAC[j]>=4095)) ++clipAC;
                }
                out["AD_rail_samples"]=clipAD;out["AC_rail_samples"]=clipAC;
                frames.append(out);
            }
            require(lastEvent=="stop","Incomplete input");
            results.append(QJsonObject{{"file",QFileInfo(path).fileName()},
                {"sha256",QString::fromLatin1(QCryptographicHash::hash(data,QCryptographicHash::Sha256).toHex())},
                {"profile",config["implementation"]},{"total_frames",total},{"verified_valley_frames",verified},
                {"gate_fallback_frames_not_analyzed",fallback},{"frames",frames}});
            std::fprintf(stderr,"Verified %d valley frames: %s\n",verified,qPrintable(QFileInfo(path).fileName()));
        }
        QFile out(QString::fromLocal8Bit(argv[1]));
        for (const auto& path:paths) require(QFileInfo(out).absoluteFilePath()!=QFileInfo(path).absoluteFilePath(),"Output must not overwrite input");
        require(out.open(QIODevice::WriteOnly),"Cannot write output");
        QJsonObject report{{"scope","A-valley raw reconstruction plus fixed diagnostic windows; not whole-flow replay, acceptance or accuracy proof"},{"files",results}};
        const auto bytes=QJsonDocument(report).toJson(); require(out.write(bytes)==bytes.size(),"Incomplete output write");
    } catch (const std::exception& e) {std::fprintf(stderr,"Audit failed: %s\n",e.what());return 1;}
    return 0;
}
