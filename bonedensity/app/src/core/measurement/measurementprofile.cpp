#include "measurement/measurementprofile.h"

#include "measurement/signalprocessor.h"

MeasurementProfile MeasurementProfile::current()
{
    MeasurementProfile profile;
#if defined(BONE_COMPLETE_B_PEAK_EXPERIMENT)
    profile.implementation = QStringLiteral("b-peak-completion-20260908-v1");
    profile.title = QStringLiteral("骨密度仪 · B峰补全试测版（仅研发验证）");
    profile.enforceBOnsetConsistency = false;
    profile.completeTruncatedBPeak = true;
    profile.deferPartialDiscardUntilRelock = false;
#elif defined(BONE_RELOCK_PRESERVATION_EXPERIMENT)
    profile.implementation = QStringLiteral("relock-preservation-20260908-v1");
    profile.title = QStringLiteral("骨密度仪 · 稳定簇续接试测版（仅研发验证）");
    profile.enforceBOnsetConsistency = false;
#elif defined(BONE_DUAL_WINDOW_A_EXPERIMENT)
    profile.implementation = QStringLiteral("dual-window-a078-20260906-v1");
    profile.title = QStringLiteral("骨密度仪 · 双段评分试测版（仅研发验证）");
    profile.enforceBOnsetConsistency = false;
    profile.deferPartialDiscardUntilRelock = false;
#elif defined(BONE_OBSERVE_BEFORE_G_EXPERIMENT)
    profile.implementation = QStringLiteral("observe-before-g-20260906-v1");
    profile.title = QStringLiteral("骨密度仪 · 姿态流程试测版（仅研发验证）");
    profile.enforceBOnsetConsistency = false;
    profile.useDualWindowAQuality = false;
    profile.deferPartialDiscardUntilRelock = false;
#else
    profile.implementation = QStringLiteral("onset-consistency-20260915-v1");
    profile.title = QStringLiteral("超声骨密度仪");
#endif
    return profile;
}

QJsonObject measurementParameters(const MeasureConfig& config, const MeasurementProfile& profile,
                                  const SignalProcessor& processor)
{
    return QJsonObject{
        {"implementation", profile.implementation},
        {"B_onset_forward_limit", profile.enforceBOnsetConsistency ? config.bOnsetForwardLimit : 0},
        {"B_clipped_peak_extension", profile.clippedBPeakExtension()},
        {"partial_relock_retention_lag", profile.deferPartialDiscardUntilRelock
            ? config.partialRelockRetentionTolerance : 0},
        {"round_target", config.roundsPerMeasurement},
        {"frame_target", config.framesPerRound},
        {"round_cluster_tolerance", config.roundClusterTolerance},
        {"probe_distance_m", processor.probeDistanceCD},
        {"sample_period_s", processor.samplePeriod},
        {"B_only", config.bOnlySos}, {"SOS_offset", config.sosOffset},
        {"angle_gate_enabled", config.angleGateEnabled},
        {"frame_corr_A", config.frameCorrAMin}, {"frame_corr_B", config.frameCorrBMin},
        {"round_corr_A", config.roundCorrAMin}, {"round_corr_B", config.roundCorrBMin},
        {"D_min", config.angleSignedDiffMin}, {"D_max", config.angleSignedDiffMax},
        {"G_min", config.anglePairMidGapMin}, {"G_max", config.anglePairMidGapMax},
        {"warmup", config.stableLagWarmupCount}, {"lock_need", config.stableLagLockNeedCount},
        {"lag_tolerance", config.stableLagTolerance}, {"unlock_count", config.boneLagUnlockCount},
        {"window_size", config.stableLagWindowSize}};
}
