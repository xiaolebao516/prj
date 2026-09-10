// Offline only: unchanged product sources, recorded features and actual round flow.
#define main included_warmup_main
#include "warmup_policy_replay.cpp"
#undef main

static void testAnchor() {
    RelativeGAnchor a;
    for(int i=0;i<13;++i)a.feed(true,-40,128,i*80,12);
    require(!a.ready,"Anchor learned before14");a.feed(true,-40,128,1040,12);
    require(a.ready && a.center==-40,"Nonzero centre not learned");
    a.feed(true,20,150,1120,12);require(a.center==-40,"Anchor chased movement");
    RelativeGAnchor b;
    for(int i=0;i<14;++i)b.feed(true,i%2?-34:-46,128,i*80,12);
    require(b.ready && b.center==-40,"Inclusive span boundary failed");
    RelativeGAnchor c;
    for(int i=0;i<50;++i)c.feed(true,i%2?-33:-46,128,i*80,12);
    require(!c.ready,"Excessive G span accepted");
    RelativeGAnchor d;
    for(int i=0;i<13;++i)d.feed(true,-40,128,i*80,12);
    d.feed(false,-40,128,1040,12);d.feed(true,-40,128,1120,12);
    require(!d.ready && d.values.size()==1,"Bad-signal learning reset failed");
    RelativeGAnchor e;
    for(int i=0;i<14;++i)e.feed(true,-40,i%2?128:134,i*80,12);
    require(!e.ready,"Moving B cluster learned");
    RelativeGAnchor f;
    for(int i=0;i<13;++i)f.feed(true,-40,128,i*80,12);
    f.feed(true,-40,128,2000,12);require(!f.ready && f.values.size()==1,"Gap reset failed");
}

int main(int argc,char** argv) {
    QApplication app(argc,argv);
    qInstallMessageHandler([](QtMsgType,const QMessageLogContext&,const QString&){});
    try {
        require(argc==5,"Usage: runner manifest input prior-production-results output");
        testAnchor();
        QFile manifest(QString::fromLocal8Bit(argv[1]));require(manifest.open(QIODevice::ReadOnly),"Manifest unavailable");
        const auto files=QJsonDocument::fromJson(manifest.readAll()).object()["files"].toArray();
        require(files.size()==73,"Manifest count mismatch");
        QFile prior(QString::fromLocal8Bit(argv[3]));require(prior.open(QIODevice::ReadOnly),"Prior evidence unavailable");
        const auto oldFiles=QJsonDocument::fromJson(prior.readAll()).object()["files"].toArray();
        require(oldFiles.size()==63,"Prior production evidence count mismatch");
        QJsonArray output;
        const Case original{"baseline",.78,.78,-6,6,14,10,true,true,false};
        const Case production{"production",.78,.78,-6,6,14,10,true,true,true};
        Case candidate=production;candidate.name="relative14-span12";candidate.relativeGSpan=12;
        for(const auto& value:files) {
            const auto entry=value.toObject();
            if(entry["file"].toString().mid(15,2).toInt()<8)continue;
            bool damaged=false;
            const auto rows=validatedPrefix(QDir(QString::fromLocal8Bit(argv[2])).filePath(entry["file"].toString()),entry["sha256"].toString(),damaged);
            MainWindowSafetyTests::runFile(rows,original,false,true); // all historical decisions/summaries
            const auto base=MainWindowSafetyTests::runFile(rows,production,false,true);
            bool matched=false;
            for(const auto& old:oldFiles)if(old.toObject()["file"]==entry["file"]) {
                require(equivalent(base,old.toObject()["candidate"]),"Production replay differs from preserved result");matched=true;break;
            }
            require(matched,"Missing prior file");
            output.append(QJsonObject{{"file",entry["file"]},{"sha256",entry["sha256"]},
                {"damaged_prefix_only",damaged},{"production",base},
                {"relative",MainWindowSafetyTests::runFile(rows,candidate,false,true)}});
            std::fprintf(stderr,"relative G %d/63\n",int(output.size()));
        }
        require(output.size()==63,"Missing attempts");
        QFile out(QString::fromLocal8Bit(argv[4]));require(out.open(QIODevice::WriteOnly|QIODevice::NewOnly),"Use fresh output");
        const auto bytes=QJsonDocument(QJsonObject{{"files",output},{"anchor_unit_checks_passed",true},
            {"historical_baseline_and_production_verified",true},
            {"scope","Fixed per-attempt median G centre from14 consecutive other-gate-qualified frames, G span<=12/B-lag span<=5; max500ms between learning frames; frame and round G range centre+-6. No SOS fitting. Product lag stability/aggregation unchanged. Fixed recorded trajectories, not prospective or cross-session final accuracy."}}).toJson();
        require(out.write(bytes)==bytes.size()&&out.flush(),"Output write failed");
    } catch(const std::exception& e){std::fprintf(stderr,"Failed: %s\n",e.what());return 1;}
    return 0;
}
