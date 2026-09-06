#pragma once
#include "types.h"
#include <QVector>
#include <QString>
#include <vector>

// Free function: Kaiser window Bessel I0 for FIR design
double besselI0(double x);

class SignalProcessor {
public:
    // ---- Configuration (public for easy setup) ----
    double probeDistanceCD = 7.84e-3;
    double samplePeriod = 16e-9;

    // ---- FIR Design (writes firCoeffs / firOrder internally) ----
    void designFIR(double centerFreq, double bandwidth, double sampleRate);

    // ---- FIR Filtering (reads firCoeffs) ----
    QVector<double> applyFIR(const QVector<quint16>& input) const;
    QVector<double> applyFIRDouble(const QVector<double>& input) const;

    // ---- Static: Pure signal processing (no member state) ----

    static QVector<double> preprocessRawForFIR(const QVector<quint16>& input,
                                                const GateConfig& cfg,
                                                const QString& name);

    static QVector<double> buildEnvelopeFromFiltered(const QVector<double>& x,
                                                      int winLen);

    static ArrivalResult detectFirstArrivalSmart(const QVector<double>& fil,
                                                  int noiseStart,
                                                  int noiseEnd,
                                                  int searchStart,
                                                  int searchEnd,
                                                  double kSigma,
                                                  int runLen,
                                                  int envWin,
                                                  double onsetRatio,
                                                  int peakLookAhead);

    static int refineLagByPositiveCrossCorrelation(
        const QVector<double>& early,
        const QVector<double>& late,
        int earlyOnset,
        int roughLag,
        int lagMin,
        int lagMax,
        int prePts,
        int postPts,
        double* bestCorrOut);

    static ValleyResult findFirstProminentValley(const QVector<double>& x,
                                                  int start,
                                                  int end,
                                                  double minDepth,
                                                  double ratioToMax,
                                                  int minSep);

    // ---- Instance: Speed estimation (reads probeDistanceCD / samplePeriod) ----

    PairResult estimatePairSpeed(const QVector<double>& early,
                                  const QVector<double>& late,
                                  const ArrivalResult& earlyPick,
                                  const ArrivalResult& latePick,
                                  double vMin,
                                  double vMax,
                                  const QString& pairName,
                                  int forcedLagMin = -1,
                                  int forcedLagMax = -1) const;

    PairResult estimatePairSpeedByValley(const QVector<double>& early,
                                          const QVector<double>& late,
                                          const ArrivalResult& earlyPick,
                                          int priorLag,
                                          int priorTolerance,
                                          const QString& pairName) const;

private:
    std::vector<double> firCoeffs;
    int firOrder = 80;
};
