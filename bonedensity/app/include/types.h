#pragma once
#include <QString>
#include <QVector>

// ==================== Patient Data ====================
struct PatientInfo {
    QString id;
    QString name;
    QString gender;
    QString birthDay;
    QString checkDate;;
    QString height;
    QString weight;
    QString diagprompt;
    QString speedOfSound;
};

struct MeasurementRecord {
    QString id;
    QString patientId;
    QString measuredAt;
    QString operatorName;
    QString part;
    QString sos;
    QString tScore;
    QString zScore;
    QString diagnosis;
};

// ==================== Serial / Waveform ====================
struct WaveGroup {
    bool has[4] = {false, false, false, false};
    QVector<quint16> ch[4];
};

// ==================== Measurement ====================
struct RoundCandidate {
    double sos = 0.0;
    double a = 0.0;
    double b = 0.0;
    double corrA = 0.0;
    double corrB = 0.0;
};

// ==================== Measurement Configuration ====================
struct MeasureConfig {
    // 质量底线
    double frameCorrBMin = 0.55;
    double frameCorrAMin = 0.78;
    double roundCorrBMin = 0.55;
    double roundCorrAMin = 0.80;
    // 姿态门控
    double angleSignedDiffMin = 5.0;
    double angleSignedDiffMax = 15.0;
    double anglePairMidGapMin = -6.0;
    double anglePairMidGapMax = 6.0;
    double anglePairMidGapTarget = 0.0;
    double angleSignedDiffTarget = 9.0;
    // 稳定性门控
    int stableLagWarmupCount = 14;
    int stableLagLockNeedCount = 10;
    int stableLagTolerance = 5;
    int boneLagUnlockCount = 10;
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
    int onset = -1;
    int peak = -1;
    double noiseMean = 0;
    double noiseStd = 0;
    double threshold = 0;
    double peakEnv = 0;
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

// ==================== Enums ====================
enum ArchiveMode {
    NormalMode,   // 普通档案管理
    ImportMode,   // 导入病人信息
    PrintMode     // 打印功能时选择病人
};

enum AcquireMode {
    DebugAcquireMode,       // trigger / 获取波形：只调试，不走病人测量流程
    PatientMeasureMode      // 开始检测：走病人测量流程
};
