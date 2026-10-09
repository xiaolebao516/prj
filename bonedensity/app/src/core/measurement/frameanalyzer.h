#pragma once
#include "calibration/calibration.h"
#include "device/deviceprotocol.h"
#include "measurement/measurementprofile.h"
#include "measurement/measurementtypes.h"
#include "measurement/signalprocessor.h"

#include <QJsonObject>
#include <QVector>
#include <cstdlib>

// The four channels of one acquisition after the false-wave erase, baseline
// removal and FIR filter.
struct FilteredFrame {
    QVector<double> bc;
    QVector<double> bd;
    QVector<double> ac;
    QVector<double> ad;

    int length() const;   // shortest channel
};

struct FrameArrivals {
    ArrivalResult bc;
    ArrivalResult bd;
    ArrivalResult ac;
    ArrivalResult ad;
};

// Where per-frame processing stopped (experiment-log "decision").
enum class FrameDecision {
    EmptyInput,          // empty_filtered_input
    BOnsetInconsistent,  // B_onset_inconsistent (patient measurement only)
    BPairInvalid,        // B_pair_invalid
    BLagJump,            // B_lag_jump: the refined B lag left the first wave packet
    APairInvalid,        // A_pair_invalid
    ABDifference,        // AB_difference: A and B lags disagree
    Measured             // SOS available; the patient gates decide acceptance
};

// Patient gates of one measured frame, computed from the frame alone.
struct FrameGates {
    bool bJump = false;      // refined B lag stays near the rough lag
    bool boundary = false;   // B lag is not at the search boundary
    bool abDiff = false;     // |D| small enough
    bool direction = false;  // D is not clearly negative
    bool corrA = false;
    bool corrB = false;
    bool d = false;          // posture D within range
    bool g = false;          // posture G within range

    bool corr() const { return corrA && corrB; }
    bool posture(bool gateEnabled) const { return !gateEnabled || (d && g); }
    // Checks a frame must pass before it may enter the lag-stability window.
    bool prechecks(bool gateEnabled) const
    {
        return bJump && boundary && abDiff && direction && corr() && (!gateEnabled || d);
    }
    bool all(bool gateEnabled) const { return prechecks(gateEnabled) && posture(gateEnabled); }
};

struct FrameAnalysis {
    FrameDecision decision = FrameDecision::EmptyInput;
    FrameArrivals arrivals;
    PairResult b;                      // reference pair BD -> BC
    PairResult a;                      // pair AD -> AC, searched near the B lag
    bool aFromValley = false;          // valley branch (otherwise envelope fallback)
    bool aDualWindowScored = false;    // A quality replaced by the dual-window score
    double aOriginalCorr = 0.0;
    double aFrontCorr = 0.0;
    double aMiddleCorr = 0.0;
    int signedLagDiff = 0;             // D = lagA - lagB
    double pairMidGap = 0.0;           // G = B pair centre - A pair centre
    double weightB = 0.0;              // weight of B in the A/B weighted SOS
    double sos = 0.0;                  // SOS reported for this frame
    FrameGates gates;                  // valid when decision == Measured

    int diffLag() const { return std::abs(signedLagDiff); }
};

// Outcome of the patient gates plus lag stability for a measured frame.
struct FrameVerdict {
    bool stabilityEvaluated = false;
    bool stable = false;
    bool accepted = false;
};

// Stateless per-frame analysis: first-arrival picks, B and A pair lags/SOS
// and the patient gates. Lag stability and round aggregation are stateful and
// belong to MeasurementSession.
class FrameAnalyzer
{
public:
    FrameAnalyzer(const SignalProcessor& processor, const MeasureConfig& config,
                  const MeasurementProfile& profile)
        : processor_(processor), config_(config), profile_(profile) {}

    // Raw ADC frame -> filtered channels (uses the processor's FIR design).
    static FilteredFrame filter(const SignalProcessor& processor, const WaveFrame& raw);
    // Requires frame.length() > 0.
    static FrameArrivals detectArrivals(const FilteredFrame& frame);

    // The B onset guard only applies to a patient measurement.
    FrameAnalysis analyze(const FilteredFrame& frame, bool patientMeasurement) const;

    // Quality only: both windows use the already selected A lag, never a new peak.
    static double dualWindowAQuality(const QVector<double>& early, const QVector<double>& late,
                                     int onset, int lag, double* front = nullptr,
                                     double* middle = nullptr);

    // Calibration frame from the candidate-D processor, audited against the
    // active-D processor. *a / *b receive the pairs for display.
    static CalibrationFrame calibrationFrame(const SignalProcessor& candidate,
                                             const SignalProcessor& active,
                                             double frameCorrBMin, const FilteredFrame& frame,
                                             const FrameArrivals& arrivals,
                                             PairResult* a, PairResult* b);

private:
    const SignalProcessor& processor_;
    const MeasureConfig& config_;
    const MeasurementProfile& profile_;
};

// The experiment-log "frame" record of an analysis. A verdict adds the
// patient gates of a measured frame.
QJsonObject frameEvidence(const FrameAnalysis& frame, const MeasureConfig& config,
                          const MeasurementProfile& profile, const FrameVerdict* verdict = nullptr);
