#pragma once
#include "measurement/frameanalyzer.h"
#include "measurement/measurementprofile.h"
#include "measurement/measurementtypes.h"

#include <QJsonObject>
#include <QVector>
#include <functional>

class SignalProcessor;

// Values of the accepted frames of the round in progress.
struct RoundValues {
    QVector<double> sos;
    QVector<double> a;
    QVector<double> b;
    QVector<double> corrA;
    QVector<double> corrB;
    QVector<double> pairMidGap;
    QVector<double> signedLagDiff;

    int size() const { return sos.size(); }
    bool isEmpty() const { return sos.isEmpty(); }
    void append(double sosValue, double aValue, double bValue, double corrAValue,
                double corrBValue, double pairMidGapValue, double signedLagDiffValue);
    void clear();
};

// Trimmed means of a completed round and the round quality gate.
struct RoundSummary {
    double sos = 0.0;
    double a = 0.0;
    double b = 0.0;
    double corrA = 0.0;
    double corrB = 0.0;
    double pairMidGap = 0.0;
    double signedLagDiff = 0.0;
    bool signedLagDiffOk = false;
    bool pairMidGapOk = false;
    bool postureOk = false;
    bool qualityPass = false;
};

struct RoundOutcome {
    RoundSummary summary;
    bool accepted = false;     // passed the round quality gate
    int acceptedRounds = 0;    // rounds in the main cluster afterwards
};

// Per-round gate rejection counts; diagnostics only, they never decide.
struct GateStatistics {
    int totalFrames = 0;
    int decoupledFrames = 0;   // corrB < 0.25: probe in the air, not counted
    int failBJump = 0;
    int failBoundary = 0;
    int failDiff = 0;
    int failDirection = 0;
    int failCorrA = 0;
    int failCorrB = 0;
    int failAngleSignedDiff = 0;
    int failAnglePairMidGap = 0;
    int failStableWarmup = 0;
    int failStableNotConcentrated = 0;
    int failStableOutOfLock = 0;
    double corrAMin = 0.0;
    double corrAMax = 0.0;
    double corrASum = 0.0;

    void reset() { *this = GateStatistics(); }
    void print(double frameCorrAMin) const;
};

// Stateful part of a patient measurement: B-lag stability lock, the round in
// progress, completed round candidates and the final exactly-N selection.
// Events go to the experiment log through `log`.
class MeasurementSession
{
public:
    using EventLog = std::function<void(const QJsonObject&)>;

    // processor: its probe geometry converts saved B SOS values back to lags.
    MeasurementSession(const SignalProcessor& processor, const MeasureConfig& config,
                       const MeasurementProfile& profile)
        : processor_(processor), config_(config), profile_(profile) {}

    EventLog log;

    RoundValues currentRound;              // accepted frames of the round in progress
    QVector<RoundCandidate> candidates;    // completed rounds that passed the quality gate
    QVector<double> roundSos;              // main cluster of the candidates, acquisition order
    QVector<double> roundA;
    QVector<double> roundB;
    GateStatistics gateStats;

    int validCount() const { return currentRound.size(); }
    bool roundFull() const { return currentRound.size() >= config_.framesPerRound; }
    bool measurementComplete() const { return roundSos.size() >= config_.roundsPerMeasurement; }
    // A round has values, or fewer than the required rounds were accepted.
    bool hasIncompleteRounds() const;

    // ---- per frame ----
    // Patient gates plus lag stability for a measured frame; updates the
    // stability window and the gate statistics.
    FrameVerdict evaluate(const FrameAnalysis& frame);
    void addValue(double sos, double a, double b, double corrA, double corrB,
                  double pairMidGap, int signedLagDiff);

    // ---- B-lag stability ----
    bool isLocked() const { return locked_; }
    int lockedLag() const { return lockedCenter_; }
    int recentLagCount() const { return recentLags_.size(); }
    // Adds a candidate lag; true when it belongs to the locked cluster.
    bool checkLagStable(int lagB);
    // A frame that could not enter the window. Brief dropouts are tolerated;
    // a sustained loss resets the lock and discards (or defers) the partial round.
    void rejectLagCandidate();
    void resetStability();
    // A new lock must never inherit values measured at the previous position.
    void discardPartialRound();

    // ---- rounds ----
    // Summarises the full round in progress, applies the round quality gate,
    // rebuilds the main cluster and clears the round in progress.
    RoundOutcome completeRound();
    // Means of exactly roundsPerMeasurement rounds chosen by selectFinalRoundIndices().
    bool finalMeans(double& sos, double& a, double& b, QVector<int>* selectedIndices = nullptr) const;
    // Smallest SOS range, then smallest deviation from its median, then the
    // earliest; returned in acquisition order.
    static QVector<int> selectFinalRoundIndices(const QVector<double>& values, int target);

    void resetRound();   // round in progress, stability and statistics
    void resetAll();     // and all completed rounds
    void printGateStats() const { gateStats.print(config_.frameCorrAMin); }

private:
    void write(const QJsonObject& event) const { if (log) log(event); }

    const SignalProcessor& processor_;
    const MeasureConfig& config_;
    const MeasurementProfile& profile_;

    QVector<int> recentLags_;      // recent B lags that passed the prechecks
    bool locked_ = false;
    int lockedCenter_ = 0;
    int outOfLockCount_ = 0;       // consecutive frames outside the locked cluster
    int rejectedFrameCount_ = 0;   // consecutive frames that failed before the window
    bool partialRelockPending_ = false;
    int partialPreviousLagCenter_ = 0;
};
