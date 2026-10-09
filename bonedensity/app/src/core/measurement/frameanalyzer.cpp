#include "measurement/frameanalyzer.h"

#include <QDebug>
#include <QtGlobal>
#include <algorithm>
#include <cmath>

namespace {

// First-arrival search (samples): noise reference window and search window.
constexpr int kNoiseStart = 560;
constexpr int kNoiseEnd = 615;
constexpr int kSearchStart = 620;
constexpr int kSearchEnd = 1600;
constexpr double kArrivalSigma = 4.0;
constexpr int kArrivalRunLength = 8;
constexpr int kEnvelopeWindow = 20;
constexpr double kOnsetRatio = 0.20;
constexpr int kPeakLookAhead = 140;

// Physically plausible SOS range of a pair (m/s).
constexpr double kMinSos = 1800.0;
constexpr double kMaxSos = 5000.0;

// On the radius the correct cluster has a B jump of about 10..34 samples; a
// wrong 2400 m/s cluster jumps 109..115, so 70 separates them safely.
constexpr int kMaxBLagJump = 70;
// The A lag is searched within this distance of the B lag.
constexpr int kABLagTolerance = 20;

// Patient gates (samples).
constexpr int kBoundaryLag = 260;
constexpr int kMaxAbsLagDiff = 18;
constexpr int kMinSignedLagDiff = -2;

QString decisionName(FrameDecision decision)
{
    switch (decision) {
    case FrameDecision::EmptyInput: return QStringLiteral("empty_filtered_input");
    case FrameDecision::BOnsetInconsistent: return QStringLiteral("B_onset_inconsistent");
    case FrameDecision::BPairInvalid: return QStringLiteral("B_pair_invalid");
    case FrameDecision::BLagJump: return QStringLiteral("B_lag_jump");
    case FrameDecision::APairInvalid: return QStringLiteral("A_pair_invalid");
    case FrameDecision::ABDifference: return QStringLiteral("AB_difference");
    case FrameDecision::Measured: break;
    }
    return QString();
}

QJsonObject arrivalEvidence(const ArrivalResult& pick)
{
    return QJsonObject{{"valid", pick.valid}, {"first_hit", pick.firstHit},
        {"onset", pick.onset}, {"peak", pick.peak}, {"threshold", pick.threshold},
        {"forward_shift", pick.valid ? QJsonValue(pick.onset - pick.firstHit) : QJsonValue()}};
}

QJsonObject pairEvidence(const PairResult& pair)
{
    return QJsonObject{{"valid", pair.valid}, {"rough_lag", pair.roughLag},
        {"lag", pair.refinedLag}, {"sos", pair.sos}, {"corr", pair.corr},
        {"early_feature", pair.earlyOnset}, {"late_feature", pair.lateOnset}};
}

} // namespace

int FilteredFrame::length() const
{
    return std::min(std::min(bc.size(), bd.size()), std::min(ac.size(), ad.size()));
}

FilteredFrame FrameAnalyzer::filter(const SignalProcessor& processor, const WaveFrame& raw)
{
    // False-wave erase windows, set from unfiltered copper-block recordings.
    GateConfig bGate;
    bGate.baselineStart = 20;
    bGate.baselineEnd   = 105;
    bGate.eraseStart    = 115;
    bGate.eraseEnd      = 500;
    bGate.rampEnd       = 560;

    GateConfig aGate;
    aGate.baselineStart = 0;
    aGate.baselineEnd   = 15;
    aGate.eraseStart    = 18;
    aGate.eraseEnd      = 480;
    aGate.rampEnd       = 550;

    FilteredFrame frame;
    frame.bc = processor.applyFIRDouble(SignalProcessor::preprocessRawForFIR(raw.bc, bGate, "B->C"));
    frame.bd = processor.applyFIRDouble(SignalProcessor::preprocessRawForFIR(raw.bd, bGate, "B->D"));
    frame.ac = processor.applyFIRDouble(SignalProcessor::preprocessRawForFIR(raw.ac, aGate, "A->C"));
    frame.ad = processor.applyFIRDouble(SignalProcessor::preprocessRawForFIR(raw.ad, aGate, "A->D"));
    return frame;
}

FrameArrivals FrameAnalyzer::detectArrivals(const FilteredFrame& frame)
{
    const int searchEnd = qMin(frame.length() - 1, kSearchEnd);
    const auto detect = [searchEnd](const QVector<double>& channel) {
        return SignalProcessor::detectFirstArrivalSmart(
            channel, kNoiseStart, kNoiseEnd, kSearchStart, searchEnd,
            kArrivalSigma, kArrivalRunLength, kEnvelopeWindow, kOnsetRatio, kPeakLookAhead);
    };
    FrameArrivals arrivals;
    arrivals.bc = detect(frame.bc);
    arrivals.bd = detect(frame.bd);
    arrivals.ac = detect(frame.ac);
    arrivals.ad = detect(frame.ad);
    return arrivals;
}

FrameAnalysis FrameAnalyzer::analyze(const FilteredFrame& frame, bool patientMeasurement) const
{
    FrameAnalysis result;
    if (frame.length() <= 0) return result;
    result.arrivals = detectArrivals(frame);
    const FrameArrivals& pick = result.arrivals;

    const bool inconsistentBOnset =
        (pick.bd.valid && !pick.bd.onsetConsistent(config_.bOnsetForwardLimit)) ||
        (pick.bc.valid && !pick.bc.onsetConsistent(config_.bOnsetForwardLimit));
    if (patientMeasurement && profile_.enforceBOnsetConsistency && inconsistentBOnset) {
        result.decision = FrameDecision::BOnsetInconsistent;
        return result;
    }

    // B pair BD -> BC is the reference pair.
    PairResult& b = result.b;
    b = processor_.estimatePairSpeed(frame.bd, frame.bc, pick.bd, pick.bc, kMinSos, kMaxSos,
                                     "B_pair / BD->BC", -1, -1,
                                     profile_.clippedBPeakExtension());
    if (!b.valid) {
        qDebug() << "Skip: B_pair invalid, because B_pair is the reference pair";
        result.decision = FrameDecision::BPairInvalid;
        return result;
    }
    // Guard against jumping from the first-wave rough lag to a later packet.
    const int bLagJump = std::abs(b.refinedLag - b.roughLag);
    if (bLagJump > kMaxBLagJump) {
        qDebug() << "Skip: B_pair jump too large"
                 << "roughLag =" << b.roughLag
                 << "refinedLag =" << b.refinedLag
                 << "jump =" << bLagJump
                 << "limit =" << kMaxBLagJump;
        result.decision = FrameDecision::BLagJump;
        return result;
    }

    // A pair AD -> AC, near the B lag: valley branch first, then the
    // constrained envelope/correlation search.
    PairResult& a = result.a;
    a = processor_.estimatePairSpeedByValley(frame.ad, frame.ac, pick.ad, b.refinedLag,
                                             kABLagTolerance, "A_pair / AD->AC / valley");
    result.aFromValley = a.valid;
    if (profile_.useDualWindowAQuality && a.valid) {
        result.aDualWindowScored = true;
        result.aOriginalCorr = a.corr;
        a.corr = dualWindowAQuality(frame.ad, frame.ac, a.earlyOnset, a.refinedLag,
                                    &result.aFrontCorr, &result.aMiddleCorr);
    }
    if (!a.valid) {
        const int forcedMin = qMax(1, b.refinedLag - kABLagTolerance);
        const int forcedMax = b.refinedLag + kABLagTolerance;
        qDebug() << "A_pair valley failed, fallback to constrained corr"
                 << "forced range = [" << forcedMin << "," << forcedMax << "]";
        a = processor_.estimatePairSpeed(frame.ad, frame.ac, pick.ad, pick.ac, kMinSos, kMaxSos,
                                         "A_pair / AD->AC / constrained", forcedMin, forcedMax);
    }
    if (!a.valid) {
        qDebug() << "Skip: A_pair invalid";
        result.decision = FrameDecision::APairInvalid;
        return result;
    }

    // Posture features; computed before the A/B check so the balance bars can
    // still show the tilt of a rejected frame.
    result.signedLagDiff = a.refinedLag - b.refinedLag;
    const double pairMidB = 0.5 * (b.earlyOnset + b.lateOnset);
    const double pairMidA = 0.5 * (a.earlyOnset + a.lateOnset);
    result.pairMidGap = pairMidB - pairMidA;
    if (result.diffLag() > kABLagTolerance) {
        qDebug() << "Skip: A_pair and B_pair inconsistent"
                 << "lagA =" << a.refinedLag
                 << "lagB =" << b.refinedLag
                 << "diff =" << result.diffLag()
                 << "tolerance =" << kABLagTolerance
                 << "pairMidGap =" << result.pairMidGap;
        result.decision = FrameDecision::ABDifference;
        return result;
    }

    // The A/B weighted SOS is kept for observation; by default the reported
    // SOS is B only (A reads systematically low on the radius).
    double weightB = 0.75;
    if (a.refinedLag > b.refinedLag + 3) weightB = 0.8;
    if (std::abs(a.refinedLag - b.refinedLag) <= 2) weightB = 0.7;
    result.weightB = weightB;
    const double sosWeighted = weightB * b.sos + (1.0 - weightB) * a.sos;
    result.sos = (config_.bOnlySos ? b.sos : sosWeighted) + config_.sosOffset;

    FrameGates& gates = result.gates;
    gates.bJump = bLagJump <= kMaxBLagJump;
    gates.boundary = b.refinedLag < kBoundaryLag;
    gates.abDiff = result.diffLag() <= kMaxAbsLagDiff;
    gates.direction = result.signedLagDiff >= kMinSignedLagDiff;
    // B correlation is only a floor: at the correct radius angle it is not the highest.
    gates.corrB = b.corr >= config_.frameCorrBMin;
    gates.corrA = a.corr >= config_.frameCorrAMin;
    gates.d = result.signedLagDiff >= config_.angleSignedDiffMin &&
              result.signedLagDiff <= config_.angleSignedDiffMax;
    gates.g = result.pairMidGap >= config_.anglePairMidGapMin &&
              result.pairMidGap <= config_.anglePairMidGapMax;
    result.decision = FrameDecision::Measured;
    return result;
}

double FrameAnalyzer::dualWindowAQuality(const QVector<double>& early, const QVector<double>& late,
                                         int onset, int lag, double* front, double* middle)
{
    const auto score = [&](int lo, int hi) {
        const qint64 begin=qint64(onset)+lo, end=qint64(onset)+hi;
        if (lag<=0 || begin<0 || end>=early.size() || end+lag>=late.size()) return 0.0;
        double ma=0,mb=0,aa=0,bb=0,ab=0;
        for (qint64 i=begin;i<=end;++i) {
            if (!std::isfinite(early[i]) || !std::isfinite(late[i+lag])) return 0.0;
            ma+=early[i]; mb+=late[i+lag];
        }
        ma/=end-begin+1; mb/=end-begin+1;
        for (qint64 i=begin;i<=end;++i) {
            const double a=early[i]-ma,b=late[i+lag]-mb;
            aa+=a*a; bb+=b*b; ab+=a*b;
        }
        if (aa<1e-12 || bb<1e-12) return 0.0;
        const double value=ab/std::sqrt(aa*bb);
        return std::isfinite(value) ? qBound(-1.0,value,1.0) : 0.0;
    };
    const double a=score(-20,30),b=score(0,60);
    if (front) *front=a;
    if (middle) *middle=b;
    return qMin(a,b);
}

CalibrationFrame FrameAnalyzer::calibrationFrame(const SignalProcessor& candidate,
                                                 const SignalProcessor& active,
                                                 double frameCorrBMin, const FilteredFrame& frame,
                                                 const FrameArrivals& pick,
                                                 PairResult* aOut, PairResult* bOut)
{
    CalibrationFrame result;
    const PairResult b = candidate.estimatePairSpeed(
        frame.bd, frame.bc, pick.bd, pick.bc, kMinSos, kMaxSos,
        QStringLiteral("Calibration B_pair / BD->BC"));
    if (bOut) *bOut = b;
    if (!b.valid) return result;

    const int bLagJump = std::abs(b.refinedLag - b.roughLag);
    const int lagMin = qMax(1, static_cast<int>(std::floor(
        candidate.probeDistanceCD / (kMaxSos * candidate.samplePeriod))) - 5);
    const int lagMax = qMax(lagMin + 1, static_cast<int>(std::ceil(
        candidate.probeDistanceCD / (kMinSos * candidate.samplePeriod))) + 5);

    result.bValid = bLagJump <= kMaxBLagJump && b.corr >= frameCorrBMin;
    result.boundaryPeak = std::abs(b.refinedLag) <= lagMin + 2
        || std::abs(b.refinedLag) >= lagMax - 2;
    result.sosB = b.sos;
    result.corrB = b.corr;
    result.lagB = b.refinedLag;

    PairResult a = candidate.estimatePairSpeedByValley(
        frame.ad, frame.ac, pick.ad, b.refinedLag, kABLagTolerance,
        QStringLiteral("Calibration A_pair / AD->AC / valley"));
    if (!a.valid) {
        a = candidate.estimatePairSpeed(
            frame.ad, frame.ac, pick.ad, pick.ac, kMinSos, kMaxSos,
            QStringLiteral("Calibration A_pair / AD->AC / constrained"),
            qMax(1, b.refinedLag - kABLagTolerance), b.refinedLag + kABLagTolerance);
    }
    if (aOut) *aOut = a;
    result.aValid = a.valid && a.corr >= 0.25;
    if (a.valid) {
        result.sosA = a.sos;
        result.corrA = a.corr;
        result.lagA = a.refinedLag;
    }

    const PairResult audit = active.estimatePairSpeed(
        frame.bd, frame.bc, pick.bd, pick.bc, kMinSos, kMaxSos,
        QStringLiteral("Calibration active-D peak audit"));
    result.peakConsistent = audit.valid && std::abs(audit.refinedLag - b.refinedLag) <= 3;
    return result;
}

QJsonObject frameEvidence(const FrameAnalysis& frame, const MeasureConfig& config,
                          const MeasurementProfile& profile, const FrameVerdict* verdict)
{
    QJsonObject evidence{{"event", "frame"}, {"decision", decisionName(frame.decision)}};
    if (frame.decision == FrameDecision::EmptyInput) return evidence;

    evidence["B_arrivals"] = QJsonObject{{"BD", arrivalEvidence(frame.arrivals.bd)},
                                         {"BC", arrivalEvidence(frame.arrivals.bc)}};
    if (frame.decision == FrameDecision::BOnsetInconsistent) {
        evidence["B_onset_forward_limit"] = config.bOnsetForwardLimit;
        return evidence;
    }

    evidence["B"] = pairEvidence(frame.b);
    if (frame.decision == FrameDecision::BPairInvalid || frame.decision == FrameDecision::BLagJump)
        return evidence;

    evidence["A_feature_branch"] = frame.aFromValley ? "valley" : "envelope_fallback";
    if (frame.aDualWindowScored) {
        evidence["A_quality_trial"] = QJsonObject{{"original_corr", frame.aOriginalCorr},
            {"front_corr", frame.aFrontCorr}, {"middle_corr", frame.aMiddleCorr},
            {"common_lag", frame.a.refinedLag}};
    }
    evidence["A"] = pairEvidence(frame.a);
    if (frame.decision == FrameDecision::APairInvalid) return evidence;

    evidence["D"] = frame.signedLagDiff;
    evidence["G"] = frame.pairMidGap;
    if (frame.decision != FrameDecision::Measured || !verdict) return evidence;

    const FrameGates& gates = frame.gates;
    evidence["decision"] = verdict->accepted ? "accepted" : "rejected";
    evidence["sos_patient"] = frame.sos;
    QJsonObject gateEvidence{{"B_jump", gates.bJump}, {"boundary", gates.boundary},
        {"AB_diff", gates.abDiff}, {"direction", gates.direction},
        {"corr_A", gates.corrA}, {"corr_B", gates.corrB},
        {"D", gates.d}, {"G", gates.g},
        {"stability_evaluated", verdict->stabilityEvaluated},
        {"stable", verdict->stable}};
    if (profile.observeStabilityBeforeG)
        gateEvidence["all_prechecks_passed"] = gates.all(config.angleGateEnabled);
    evidence["gates"] = gateEvidence;
    return evidence;
}
