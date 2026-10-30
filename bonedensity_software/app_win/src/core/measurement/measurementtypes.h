#pragma once

// ==================== Measurement ====================
struct RoundCandidate {
    double sos = 0.0;
    double a = 0.0;
    double b = 0.0;
    double corrA = 0.0;
    double corrB = 0.0;
};

// ==================== Measurement Configuration ====================
// Every tunable value of the patient measurement. The values that decide how
// SOS is produced are recorded with each result (measurementParameters()).
struct MeasureConfig {
    // 质量底线
    double frameCorrBMin = 0.55;
    double frameCorrAMin = 0.78;
    double roundCorrBMin = 0.55;
    double roundCorrAMin = 0.78;
    // 姿态门控（D = lagA - lagB，G = B/A 两组特征点中心之差）
    bool angleGateEnabled = true;
    double angleSignedDiffMin = 5.0;
    double angleSignedDiffMax = 15.0;
    double anglePairMidGapMin = -12.0;
    double anglePairMidGapMax = 0;
    double anglePairMidGapTarget = 0.0;
    double angleSignedDiffTarget = 9.0;
    // B 首波一致性：onset 相对首次越阈的最大前移（采样点）
    int bOnsetForwardLimit = 40;
    // 稳定性门控：先观察 warmup 个候选 lagB，锁定中位簇后只接受簇内的帧
    int stableLagWindowSize = 20;
    int stableLagWarmupCount = 14;
    int stableLagLockNeedCount = 10;
    int stableLagTolerance = 5;
    int boneLagUnlockCount = 10;
    // 失锁后重新锁定：新簇与已存 B 值的 lag 都在此容差内才保留半轮数据
    int partialRelockRetentionTolerance = 2;
    // 轮次与结果
    int framesPerRound = 30;               // 每轮需要的有效帧数
    int roundsPerMeasurement = 5;          // 一次检测取 5 轮
    double roundClusterTolerance = 180.0;  // 轮次之间 ±180 m/s 归为同一簇
    // 正式 SOS：A 通道只用于姿态与质量判断，结果只取 B 通道
    bool bOnlySos = true;
    double sosOffset = 0.0;                // 最终 SOS 校准偏移（m/s）
};

// ==================== Signal Processing ====================
struct GateConfig {
    int baselineStart;
    int baselineEnd;
    int eraseStart;
    int eraseEnd;
    int rampEnd;
};

struct ArrivalResult {
    bool valid = false;
    int firstHit = -1;
    int onset = -1;
    int peak = -1;
    double noiseMean = 0;
    double noiseStd = 0;
    double threshold = 0;
    double peakEnv = 0;

    bool onsetConsistent(int maximumForwardShift) const {
        return valid && firstHit >= 0 && onset >= 0
            && onset - firstHit <= maximumForwardShift;
    }
};

struct PairResult {
    bool valid = false;
    int earlyOnset = -1;
    int lateOnset = -1;
    int roughLag = 0;
    int refinedLag = 0;
    double corr = 0;
    double sos = 0;
};

struct ValleyResult {
    bool valid = false;
    int idx = -1;
    double value = 0.0;
    double depth = 0.0;
};
