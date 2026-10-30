#include "measurement/measurementsession.h"

#include "measurement/signalprocessor.h"
#include "measurement/utils.h"

#include <QDebug>
#include <QJsonArray>
#include <algorithm>
#include <cmath>

void RoundValues::append(double sosValue, double aValue, double bValue, double corrAValue,
                         double corrBValue, double pairMidGapValue, double signedLagDiffValue)
{
    sos.append(sosValue);
    a.append(aValue);
    b.append(bValue);
    corrA.append(corrAValue);
    corrB.append(corrBValue);
    pairMidGap.append(pairMidGapValue);
    signedLagDiff.append(signedLagDiffValue);
}

void RoundValues::clear()
{
    sos.clear();
    a.clear();
    b.clear();
    corrA.clear();
    corrB.clear();
    pairMidGap.clear();
    signedLagDiff.clear();
}

void GateStatistics::print(double frameCorrAMin) const
{
    if (totalFrames <= 0) {
        qDebug() << "[闸门统计] 暂无数据";
        return;
    }

    const auto pct = [this](int fail) {
        const double r = double(fail) / double(totalFrames) * 100.0;
        return QString("%1%").arg(r, 0, 'f', 1);
    };

    // 分两行短输出，避免 Qt Creator 截断长行
    qDebug().noquote()
        << QString("[闸门统计 共%1帧] BJump=%2 Boundary=%3 Diff=%4 Dir=%5")
               .arg(totalFrames)
               .arg(pct(failBJump))
               .arg(pct(failBoundary))
               .arg(pct(failDiff))
               .arg(pct(failDirection));

    qDebug().noquote()
        << QString("[闸门统计] CorrA=%1 CorrB=%2 AngDiff=%3 AngGap=%4 StabWarm=%5 StabConc=%6 StabLock=%7")
               .arg(pct(failCorrA))
               .arg(pct(failCorrB))
               .arg(pct(failAngleSignedDiff))
               .arg(pct(failAnglePairMidGap))
               .arg(pct(failStableWarmup))
               .arg(pct(failStableNotConcentrated))
               .arg(pct(failStableOutOfLock));

    if (decoupledFrames > 0) {
        qDebug().noquote()
            << QString("  (另有 %1 帧因脱耦被忽略，corr<0.25)").arg(decoupledFrames);
    }

    qDebug().noquote()
        << QString("[CorrA分布] 最小=%1  平均=%2  最大=%3  门槛=%4")
               .arg(corrAMin, 0, 'f', 3)
               .arg(corrASum / totalFrames, 0, 'f', 3)
               .arg(corrAMax, 0, 'f', 3)
               .arg(frameCorrAMin, 0, 'f', 2);
}

bool MeasurementSession::hasIncompleteRounds() const
{
    return !currentRound.isEmpty()
        || (!roundSos.isEmpty() && roundSos.size() < config_.roundsPerMeasurement);
}

FrameVerdict MeasurementSession::evaluate(const FrameAnalysis& frame)
{
    const bool gateEnabled = config_.angleGateEnabled;
    const FrameGates& gates = frame.gates;

    // Only frames that pass the prechecks may enter the stability window;
    // others would pollute it. G may be observed later (observeStabilityBeforeG),
    // but a G-failing frame never counts as a value.
    FrameVerdict verdict;
    verdict.stabilityEvaluated = gates.prechecks(gateEnabled)
        && (profile_.observeStabilityBeforeG || gates.posture(gateEnabled));
    if (verdict.stabilityEvaluated) verdict.stable = checkLagStable(frame.b.refinedLag);
    else rejectLagCandidate();
    verdict.accepted = gates.all(gateEnabled) && verdict.stable;

    if (frame.b.corr < 0.25) {
        gateStats.decoupledFrames++;
    } else {
        GateStatistics& s = gateStats;
        s.totalFrames++;
        if (s.totalFrames == 1) {
            s.corrAMin = s.corrAMax = s.corrASum = frame.a.corr;
        } else {
            s.corrAMin = qMin(s.corrAMin, frame.a.corr);
            s.corrAMax = qMax(s.corrAMax, frame.a.corr);
            s.corrASum += frame.a.corr;
        }
        if (!gates.bJump) s.failBJump++;
        if (!gates.boundary) s.failBoundary++;
        if (!gates.abDiff) s.failDiff++;
        if (!gates.direction) s.failDirection++;
        if (frame.b.corr < config_.frameCorrBMin) s.failCorrB++;
        if (frame.a.corr < config_.frameCorrAMin) s.failCorrA++;
        if (!gates.d) s.failAngleSignedDiff++;
        if (!gates.g) s.failAnglePairMidGap++;
        if (!verdict.stable) {
            if (locked_) s.failStableOutOfLock++;
            else if (recentLags_.size() < config_.stableLagWarmupCount) s.failStableWarmup++;
            else s.failStableNotConcentrated++;
        }
    }
    // 每 50 帧输出一次当前统计（不需要等有效帧攒满）
    if (gateStats.totalFrames % 50 == 0) printGateStats();
    return verdict;
}

void MeasurementSession::addValue(double sos, double a, double b, double corrA, double corrB,
                                  double pairMidGap, int signedLagDiff)
{
    currentRound.append(sos, a, b, corrA, corrB, pairMidGap, signedLagDiff);
}

bool MeasurementSession::checkLagStable(int lagB)
{
    rejectedFrameCount_ = 0;
    recentLags_.append(lagB);
    while (recentLags_.size() > config_.stableLagWindowSize) recentLags_.removeFirst();

    if (!locked_) {
        // Warm-up: observe before locking.
        if (recentLags_.size() < config_.stableLagWarmupCount) return false;

        // Lock on the median of the recent candidates (not the highest-SOS cluster).
        QVector<int> sorted = recentLags_;
        std::sort(sorted.begin(), sorted.end());
        const int center = sorted[sorted.size() / 2];
        int countAroundCenter = 0;
        for (int v : recentLags_) {
            if (std::abs(v - center) <= config_.stableLagTolerance) countAroundCenter++;
        }
        if (countAroundCenter < config_.stableLagLockNeedCount) return false;

        locked_ = true;
        lockedCenter_ = center;
        outOfLockCount_ = 0;

        if (partialRelockPending_) {
            const int previousCenter = partialPreviousLagCenter_;
            bool sameCluster = std::abs(center - previousCenter) <= config_.partialRelockRetentionTolerance;
            for (double sosB : std::as_const(currentRound.b)) {
                if (!std::isfinite(sosB) || sosB <= 0.0) {
                    sameCluster = false;
                    break;
                }
                const int sampleLag = qRound(processor_.probeDistanceCD
                    / (sosB * processor_.samplePeriod));
                if (std::abs(sampleLag - center) > config_.partialRelockRetentionTolerance) {
                    sameCluster = false;
                    break;
                }
            }
            partialRelockPending_ = false;
            partialPreviousLagCenter_ = 0;
            write({{"event", sameCluster ? "partial_relock_retained" : "partial_relock_discarded"},
                   {"previous_lag", previousCenter}, {"new_lag", center},
                   {"preserved_values", validCount()}});
            if (!sameCluster) discardPartialRound();
        }
        // The locking frame counts when it is inside the new cluster.
        return std::abs(lagB - lockedCenter_) <= config_.stableLagTolerance;
    }

    if (std::abs(lagB - lockedCenter_) <= config_.stableLagTolerance) {
        outOfLockCount_ = 0;
        return true;
    }

    // Many consecutive frames outside the cluster: the probe has moved. Look
    // for a new cluster and never average values across two positions.
    if (++outOfLockCount_ >= config_.boneLagUnlockCount) {
        write({{"event", "cluster_lost"}, {"old_lag", lockedCenter_}});
        resetStability();
        discardPartialRound();
    }
    return false;
}

void MeasurementSession::rejectLagCandidate()
{
    // Reuses the sustained-loss count; no separate tuning constant.
    if (++rejectedFrameCount_ < config_.boneLagUnlockCount) return;

    write({{"event", "sustained_precheck_loss"}});
    const bool deferDiscard = profile_.deferPartialDiscardUntilRelock && validCount() > 0
        && (locked_ || partialRelockPending_);
    const int previousCenter = locked_ ? lockedCenter_ : partialPreviousLagCenter_;
    resetStability();
    if (deferDiscard) {
        partialRelockPending_ = true;
        partialPreviousLagCenter_ = previousCenter;
        write({{"event", "partial_discard_deferred"},
               {"previous_lag", previousCenter},
               {"preserved_values", validCount()}});
    } else {
        discardPartialRound();
    }
}

void MeasurementSession::resetStability()
{
    rejectedFrameCount_ = 0;
    recentLags_.clear();
    locked_ = false;
    lockedCenter_ = 0;
    outOfLockCount_ = 0;
    partialRelockPending_ = false;
    partialPreviousLagCenter_ = 0;
}

void MeasurementSession::discardPartialRound()
{
    write({{"event", "discard_partial"}, {"discarded_values", validCount()}});
    currentRound.clear();
}

RoundOutcome MeasurementSession::completeRound()
{
    RoundOutcome outcome;
    RoundSummary& s = outcome.summary;
    s.sos = Utils::trimmedMeanValue(currentRound.sos, 0.2);
    s.a = Utils::trimmedMeanValue(currentRound.a, 0.2);
    s.b = Utils::trimmedMeanValue(currentRound.b, 0.2);
    s.corrA = Utils::trimmedMeanValue(currentRound.corrA, 0.2);
    s.corrB = Utils::trimmedMeanValue(currentRound.corrB, 0.2);
    s.pairMidGap = Utils::trimmedMeanValue(currentRound.pairMidGap, 0.2);
    s.signedLagDiff = Utils::trimmedMeanValue(currentRound.signedLagDiff, 0.2);
    s.signedLagDiffOk = s.signedLagDiff >= config_.angleSignedDiffMin &&
                        s.signedLagDiff <= config_.angleSignedDiffMax;
    s.pairMidGapOk = s.pairMidGap >= config_.anglePairMidGapMin &&
                     s.pairMidGap <= config_.anglePairMidGapMax;
    s.postureOk = !config_.angleGateEnabled || (s.signedLagDiffOk && s.pairMidGapOk);
    // Frames can pass one by one while the whole round is still a stable but
    // misplaced measurement; its mean quality decides.
    s.qualityPass = s.corrB >= config_.roundCorrBMin && s.corrA >= config_.roundCorrAMin
                    && s.postureOk;
    write({{"event", "round_summary"}, {"sos", s.sos},
           {"corr_A", s.corrA}, {"corr_B", s.corrB},
           {"D", s.signedLagDiff}, {"G", s.pairMidGap},
           {"quality_pass", s.qualityPass}});

    qDebug() << "One patient round candidate:"
             << "sos =" << s.sos
             << "A =" << s.a
             << "B =" << s.b
             << "corrA =" << s.corrA
             << "corrB =" << s.corrB
             << "pairMidGap =" << s.pairMidGap
             << "signedLagDiff =" << s.signedLagDiff
             << "oneRoundAngleSignedDiffOk =" << s.signedLagDiffOk
             << "oneRoundPairMidGapOk =" << s.pairMidGapOk
             << "oneRoundAngleOk =" << s.postureOk
             << "mCfg.roundCorrAMin =" << config_.roundCorrAMin
             << "mCfg.roundCorrBMin =" << config_.roundCorrBMin;

    if (s.corrB < config_.roundCorrBMin || s.corrA < config_.roundCorrAMin || !s.postureOk) {
        qDebug() << "One patient round rejected by quality:"
                 << "sos =" << s.sos
                 << "corrA =" << s.corrA
                 << "corrB =" << s.corrB;
        printGateStats();
        currentRound.clear();
        outcome.acceptedRounds = roundSos.size();
        return outcome;
    }

    RoundCandidate candidate;
    candidate.sos = s.sos;
    candidate.a = s.a;
    candidate.b = s.b;
    candidate.corrA = s.corrA;
    candidate.corrB = s.corrB;
    candidates.append(candidate);

    // Outlying rounds stay candidates but never enter roundSos.
    Utils::rebuildAcceptedRoundsFromCandidates(candidates, roundSos, roundA, roundB,
                                               config_.roundClusterTolerance);
    const int finished = roundSos.size();
    QJsonArray acceptedSos;
    for (double sos : roundSos) acceptedSos.append(sos);
    write({{"event", "round_cluster"}, {"accepted_sos", acceptedSos},
           {"candidate_count", candidates.size()}});
    if (finished >= config_.roundsPerMeasurement) {
        double sos = 0, a = 0, b = 0;
        QVector<int> selected;
        if (finalMeans(sos, a, b, &selected)) {
            QJsonArray indices;
            for (int index : selected) indices.append(index);
            write({{"event", "final_round_selection"},
                   {"source_accepted_sos", acceptedSos}, {"selected_indices", indices},
                   {"final_sos", sos}, {"final_A", a}, {"final_B", b}});
        }
    }

    qDebug() << "One patient round accepted:"
             << "acceptedRoundCount =" << finished
             << "candidateCount =" << candidates.size()
             << "sos =" << s.sos
             << "A =" << s.a
             << "B =" << s.b
             << "corrA =" << s.corrA
             << "corrB =" << s.corrB;
    printGateStats();

    currentRound.clear();
    outcome.accepted = true;
    outcome.acceptedRounds = finished;
    return outcome;
}

QVector<int> MeasurementSession::selectFinalRoundIndices(const QVector<double>& values, int target)
{
    if (target <= 0 || values.size() < target) return {};
    QVector<int> order;
    for (int i=0; i<values.size(); ++i) {
        if (!std::isfinite(values[i])) return {};
        order.append(i);
    }
    if (values.size() == target) return order;
    std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
        return values[a] < values[b];
    });
    int bestStart=-1;
    double bestRange=0, bestDeviation=0;
    for (int start=0; start+target<=order.size(); ++start) {
        const double range=values[order[start+target-1]]-values[order[start]];
        const double median=target%2 ? values[order[start+target/2]]
            : .5*(values[order[start+target/2-1]]+values[order[start+target/2]]);
        double deviation=0;
        for (int k=0; k<target; ++k) deviation+=std::abs(values[order[start+k]]-median);
        if (bestStart<0 || range<bestRange || (range==bestRange && deviation<bestDeviation)) {
            bestStart=start; bestRange=range; bestDeviation=deviation;
        }
    }
    auto selected=order.mid(bestStart,target);
    // Preserve acquisition order and the same companion-value indices.
    std::sort(selected.begin(),selected.end());
    return selected;
}

bool MeasurementSession::finalMeans(double& sos, double& a, double& b,
                                    QVector<int>* selectedIndices) const
{
    const int target = config_.roundsPerMeasurement;
    if (roundA.size()!=roundSos.size() || roundB.size()!=roundSos.size()) return false;
    const auto selected=selectFinalRoundIndices(roundSos,target);
    if (selected.size()!=target || selected.isEmpty()) return false;
    double sumSos=0,sumA=0,sumB=0;
    for (int index : selected) {
        if (!std::isfinite(roundA[index]) || !std::isfinite(roundB[index])) return false;
        sumSos+=roundSos[index]; sumA+=roundA[index]; sumB+=roundB[index];
    }
    sos=sumSos/selected.size(); a=sumA/selected.size(); b=sumB/selected.size();
    if (selectedIndices) *selectedIndices=selected;
    return true;
}

void MeasurementSession::resetRound()
{
    currentRound.clear();
    resetStability();
    gateStats.reset();
}

void MeasurementSession::resetAll()
{
    resetRound();
    roundSos.clear();
    roundA.clear();
    roundB.clear();
    candidates.clear();
}
