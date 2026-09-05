// Diagnostic reproducer compiled against unchanged production sources.
// Synthetic features/waveforms demonstrate code behavior, not patient accuracy.
#include "mainwindow.h"
#include "signalprocessor.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <cmath>

class MainWindowSafetyTests {
public:
    static QJsonObject run() {
        QJsonObject result;
        {
            MainWindow w;
            w.hide();
            for (int i=0; i<13; ++i) w.checkBoneLagStable(128);
            w.updateProcessPanel(3500, 3828, 137, 128, 9, 0, false);
            int oldD=w.ui->barPairA->value(), oldG=w.ui->barPairB->value();
            for (int i=0; i<100; ++i) w.updateProcessInvalid("synthetic invalid frame");
            result["candidate_window_after_100_invalid_frames"]=w.recentBoneLagBList.size();
            result["next_candidate_immediately_locks"]=w.checkBoneLagStable(128);
            result["invalid_frame_leaves_bar_values_unchanged"]=
                oldD==w.ui->barPairA->value() && oldG==w.ui->barPairB->value();
        }
        {
            MainWindow w;
            w.hide();
            w.patientMeasureRunning=true;
            w.acquireMode=PatientMeasureMode;
            auto feed=[&](int lag) {
                bool accepted=w.checkBoneLagStable(lag);
                w.handlePatientMeasureValue(490000.0/(lag+9), 490000.0/lag,
                    490000.0/lag, lag+9, lag, 9, 0, .95, .95, accepted);
            };
            for (int i=0;i<28;++i) feed(120);
            result["accepted_before_cluster_change"]=w.currentRoundSosList.size();
            for (int i=0;i<10;++i) feed(140);
            result["accepted_after_unlock"]=w.currentRoundSosList.size();
            for (int i=0;i<28;++i) feed(140);
            result["mixed_round_candidate_count"]=w.candidateRoundList.size();
            if (!w.candidateRoundList.isEmpty())
                result["mixed_round_sos"]=w.candidateRoundList.first().sos;
            result["input_cluster_sos_1"]=490000.0/120;
            result["input_cluster_sos_2"]=490000.0/140;
            w.closeRoundFinishedTip();
        }
        QJsonArray delays;
        for (int trueLag: {106, 128, 150, 180, 205}) {
            QVector<double> early(1800), late(1800);
            for (int i=0;i<1800;++i) {
                double t=i-800;
                early[i]=std::exp(-t*t/(2*35.0*35.0))*std::cos(2*3.141592653589793*t/50);
            }
            for (int i=trueLag;i<1800;++i) late[i]=early[i-trueLag];
            double corr=0;
            int selected=SignalProcessor::refineLagByPositiveCrossCorrelation(
                early,late,800,trueLag,93,278,20,120,&corr);
            delays.append(QJsonObject{{"true_lag",trueLag},{"selected_lag",selected},
                {"selected_corr",corr},{"true_sos",490000.0/trueLag},
                {"selected_sos",490000.0/selected}});
        }
        result["identical_delayed_waveforms"]=delays;
        return result;
    }
};

int main(int argc,char** argv) {
    QApplication app(argc,argv);
    if (argc < 2) return 2;
    const QJsonObject result=MainWindowSafetyTests::run();
    QFile output(QString::fromLocal8Bit(argv[1]));
    if (!output.open(QIODevice::WriteOnly)) return 3;
    output.write(QJsonDocument(result).toJson());
    return 0;
}
