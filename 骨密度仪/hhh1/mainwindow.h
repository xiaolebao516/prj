#pragma once
#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>
#include <QByteArray>
#include <QString>
#include <QVector>
#include <QPointF>
#include <QFile>
#include <QTextStream>
#include <QtCharts/QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QList>
#include <QLabel>
#include <QDomDocument>     // ✅ 新增，用于XML管理
#include <QProgressBar>
#include <QtCharts/QScatterSeries> // 如果想画散点图，或者用 QLineSeries
QT_USE_NAMESPACE

    QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// ✅ 定义患者结构体
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

class QMessageBox;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnLogin_clicked();

    // 串口与绘图相关
    void scanPorts();
    void on_connectButton_clicked();
    void on_triggerButton_clicked();
    void handleSerialReadyRead();
    void handleSerialError(QSerialPort::SerialPortError error);
    //void onRxTimeout();
    void sendCmd();
    // 设计滤波器系数 (初始化时调用一次)
    void designFIR(double lowCutoff, double highCutoff, double sampleRate);

    // 执行滤波 (绘图前调用)
    QVector<double> applyFIR(const QVector<quint16>& input);

    // ✅ 页面切换
    void on_btnArchive_clicked();   // 切换到档案管理
    void on_btnBackFromArchive_clicked();      // 从档案返回主页面
    void on_btnPatientInfo_clicked(); //主界面进入选择患者信息
    void on_btnBackToMain_clicked(); //选择患者信息进入主界面

    // 患者信息页面
    void on_btnPatientNewSave_clicked();
    void on_btnImportFromDB_clicked();
    void updateCurrentPatientUI();
    void on_btnSaveResult_clicked();

    // ✅ 档案管理逻辑
    void on_btnShowAll_clicked();
    void on_btnSearchName_clicked();
    void on_btnSearchID_clicked();
    void on_btnSearchDate_clicked();
    void on_btnAdd_clicked();
    void on_table_cellDoubleClicked(int row, int column);
    void on_btnDeleteSelected_clicked();
    // ✅ 新增：日期级联更新槽函数
    void updateDayCombo();


    // ✅ 新增页和详情页的交互
    void on_btnFormSave_clicked();
    void on_btnFormBack_clicked();
    void on_btnDetailBack_clicked();
    void on_btnDetailSave_clicked();
    void on_btnDetailDelete_clicked();

    // ✅ 新增：处理“显示结果”按钮点击
    void on_btnShowResult_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QTimer scanTimer;



    quint16  nextFrameIdx = 0;      // 每次点击获取波形就+1循环

    // 临时接收缓存，用于拼接一整帧
    QByteArray rxBuffer;

    QVector<quint16> samplesA;
    QVector<quint16> samplesB;
    QVector<quint16> samplesC;
    QVector<quint16> samplesD;
    // --- FIR 滤波器相关 ---
    std::vector<double> firCoeffs; // 存储滤波器系数
    int firOrder = 100;            // 滤波器阶数 (点数 N=101)

    QTimer *rxTimer;

    quint16 gain_tmp = 0;
    quint16 idx_tmp = 0;
    quint8  ch_tmp = 0;
    quint16 len_tmp = 0;
    QByteArray payload_tmp;
    int payloadNeeded = 0;


    bool chReceived[4] = {false,false,false,false};

    struct WaveGroup {
        bool has[4] = {false, false, false, false};
        QVector<quint16> ch[4];
    };

    QMap<quint16, WaveGroup> frameGroups;

    QSlider *gainSliderA;
    QSlider *gainSliderB;
    QSlider *gainSliderC;
    QSlider *gainSliderD;

    QChart *chartA, *chartB, *chartC, *chartD;
    QLineSeries *seriesA, *seriesB, *seriesC, *seriesD;
    QChartView *viewA, *viewB, *viewC, *viewD;

    QTimer *autoTimer;
    bool autoRunning = false;
    quint16 globalGain = 1024;  //统一的增益值
    uint8_t frameIdx = 0;      // 帧计数，用于命令区分

    QString xmlFilePath;
    QList<PatientInfo> patientList;   // ✅ 用于管理XML数据

    PatientInfo currentPatient;   // 当前正在测量的患者

    enum ArchiveMode {
        NormalMode,   // 普通档案管理
        ImportMode,   // 导入病人信息
        PrintMode     // 打印功能时选择病人
    };
    ArchiveMode archiveMode = NormalMode;  // 默认普通模式

    void setupChart();
    void clearSamples();
    void plotSamples();



    // ✅ XML相关
    void loadPatients();
    void savePatients();
    void refreshTable(const QList<PatientInfo> &list);
    int editingIndex = -1;
    // ✅ 新增：表单/详情页填充 & 查找
    void clearNewForm();
    //int  findPatientIndexById(const QString& id) const;
    //void fillDetailPage(const PatientInfo& p, int index);
    void fillDetailPage(const PatientInfo& p, int index);

    void on_btnAcquireWaveform_clicked();   // “获取波形”按钮
    void parseIncomingData();               // 解析rxBuffer里的下位机帧
    void onGainSliderChanged(int value);    // 任意一条增益滑条被拉

    // ✅ 新增：初始化搜索界面的辅助函数
    void initSearchControls();

    // ✅ 新增：声速图表相关成员变量
    QChart *chartSpeed;
    QLineSeries *seriesSpeed; // 声速曲线

    // ✅ 声速调试显示标签：显示两个单独声速值和加权平均值
    QLabel *lblSosA = nullptr;       // A_pair / AD->AC
    QLabel *lblSosB = nullptr;       // B_pair / BD->BC
    QLabel *lblSosAvg = nullptr;     // 加权平均
    QLabel *lblSosInfo = nullptr;    // lag、corr、状态信息

    // ✅ 初始化声速图表的函数
    void setupSpeedChart();

    // ✅ 初始化声速调试显示区域
    void setupSpeedDebugPanel();


    // ✅ 刷新声速调试显示
    void updateSpeedDebugPanel(double sosA,
                               double sosB,
                               double sosAvg,
                               int lagA,
                               int lagB,
                               int diffLag,
                               double corrA,
                               double corrB);

    // ✅ 本次检测无效时显示原因
    void setSpeedDebugInvalid(const QString& reason);

    // 声速曲线的横坐标计数
    int speedPointIndex = 0;

    // 探头 C-D 间距（米）
    double probeDistanceCD = 7.84e-3;

    // 采样周期（秒），62.5MHz -> 16ns
    double samplePeriod = 16e-9;

    // 过小 lag 判定阈值（点数），小于它就认为本次不可信
    int minLagThreshold = 100;

    void printRangeFormatted(const QString& name,
                             const QVector<double>& data,
                             int start, int end) const;

    QVector<double> buildEnvelopeFromFiltered(const QVector<double>& x, int winLen) const;

    int detectFirstArrivalFromFiltered(const QVector<double>& fil,
                                       int noiseStart,
                                       int noiseEnd,
                                       int searchStart,
                                       int searchEnd,
                                       double kSigma,
                                       int runLen,
                                       int envWin) const;

    void detectAllFirstArrivalsFromFiltered(const QVector<double>& filA,
                                            const QVector<double>& filB,
                                            const QVector<double>& filC,
                                            const QVector<double>& filD) const;

    int refineLagByCrossCorrelation(const QVector<double>& x,
                                    const QVector<double>& y,
                                    int centerX,
                                    int centerY,
                                    int prePts,
                                    int postPts,
                                    int refineRange) const;

    void detectAndPlotSpeed(const QVector<double>& filA,
                            const QVector<double>& filB,
                            const QVector<double>& filC,
                            const QVector<double>& filD);

    void appendSpeedPoint(double speedAvg);

    // ================== 检查过程显示区 ==================
    int processValidCount = 0;              // 当前横向进度条已经累计的有效次数
    int processValidTarget = 30;            // 满 50 次认为本轮测量完成

    // 原来是 6，但人体桡骨这类复杂波形中，正确结果的 A/B lag 可能差到 7~14。
    // 所以这里放宽一点，真正有效性不只靠 diffLag，而是靠下面的稳定簇判断。
    int processStrictLagTolerance = 18;


    // ================== 正式测量稳定性判定 ==================
    // 新思路：正式测量不再追逐“最高声速簇”，而是先观察一段时间，
    // 锁定一个稳定的 lagB 中位簇。之后只有当前帧仍属于这个稳定簇，进度条才动。
    QVector<int> recentBoneLagBList;     // 最近若干个通过初筛的 B_pair lag

    // 观察窗口。前 stableLagWarmupCount 个候选只用于找稳定簇，不推动进度条。
    int stableLagWindowSize = 30;        // 最近最多保留 40 个候选 lag
    int stableLagWarmupCount = 22;       // 先观察 40 个候选，再锁定簇
    int stableLagTolerance = 4;          // 锁定簇允许 ±4 点

    // 锁定簇需要足够集中。
    // 例如 40 个候选里至少 24 个落在中心 ±4 点内，才认为位置稳定。
    int stableLagLockNeedCount = 15;

    // 当前本轮正式测量锁定的 lagB 中心
    bool boneLagLocked = false;
    int lockedBoneLagCenter = 0;

    // 如果已经锁定，但连续很多帧偏离锁定簇，说明探头位置变了，重新寻找稳定簇。
    int boneLagOutOfLockCount = 0;
    int boneLagUnlockCount = 8;

    bool checkBoneLagStable(int lagB, int* centerOut = nullptr, int* countOut = nullptr);
    void resetBoneLagStability();

    // ================== 病人检测 / 调试模式控制 ==================
    enum AcquireMode {
        DebugAcquireMode,       // trigger / 获取波形：只调试，不走病人测量流程
        PatientMeasureMode      // 开始检测：走病人测量流程
    };

    AcquireMode acquireMode = DebugAcquireMode;

    bool patientMeasureRunning = false;

    // 这个变量现在不再用于“填完病人信息后自动开始检测”，保留也可以，但固定为 false
    bool pendingStartAfterPatientInfo = false;

    int normalMeasureRounds = 5;                 // 一个病人总共需要测 5 次
    int currentMeasureTargetRounds = 5;          // 当前病人总目标次数，默认 5

    QMessageBox *measureTipBox = nullptr;        // 每次测量完成后的非阻塞提示框

    QVector<double> currentRoundSosList;         // 当前这一轮 20 个有效值
    QVector<double> currentRoundAList;
    QVector<double> currentRoundBList;

    QVector<double> currentRoundCorrAList;
    QVector<double> currentRoundCorrBList;

    // ✅ 新增：当前这一轮每个有效帧的姿态特征
    // 用于一轮结束后再判断这一整轮是不是姿态正确。
    QVector<double> currentRoundPairMidGapList;
    QVector<double> currentRoundSignedLagDiffList;

    QVector<double> roundSosList;                // 每一轮的平均值，最终 5 个
    QVector<double> roundAList;
    QVector<double> roundBList;

    // ================== 小测量轮次候选池 ==================
    // roundSosList 只保存“当前主簇”的有效小测量。
    // candidateRoundList 保存所有完成过的小测量候选。
    // 这样 3624 这种离群轮次不会直接进入最终平均。
    struct RoundCandidate {
        double sos = 0.0;
        double a = 0.0;
        double b = 0.0;
        double corrA = 0.0;
        double corrB = 0.0;
    };

    QVector<RoundCandidate> candidateRoundList;

    double roundClusterTolerance = 180.0;  // 小测量之间允许 ±180 m/s 归为同一簇
    int roundClusterMinCount = 3;          // 至少3次形成主簇才比较可信

    // ================== 正式测量质量门槛 ==================
    //
    // 重要修正：
    // 这次 CSV 说明，桡骨 3800 正确角度下 B_corr 反而不高，
    // 而 3600 / 4000 错误角度下 B_corr 可能更高。
    // 所以 B_corr 不能再作为主要姿态判据，只作为“别太差”的底线。
    double frameCorrBMin = 0.55;
    double frameCorrAMin = 0.78;

    double roundCorrBMin = 0.55;
    double roundCorrAMin = 0.80;


    // ================== 桡骨角度 / 姿态判定门槛 ==================
    //
    // 这组参数来自这次五组 CSV 的统计：
    // 桡骨3600：signedLagDiff≈5,  pairMidGap≈27
    // 桡骨3800：signedLagDiff≈10, pairMidGap≈6.5
    // 桡骨4000：signedLagDiff≈1,  pairMidGap≈-4.5
    //
    // 因此正式测量时，只有 A/B 几何关系落在下面范围，
    // 才允许进度条前进。
    bool enablePatientAngleGate = true;

    // lagA - lagB 的合理范围
    //
    // 新一轮 CSV 里，3800 左右的正确姿态大多在 8~12 左右。
    // 3500 多的低值姿态里，signedLagDiff 有不少落在 4~7。
    // 所以下限从 7.0 略微提高到 8.0，减少低速姿态混入。
    double angleSignedDiffMin = 7.0;
    double angleSignedDiffMax = 13.0;


    // pairMidGap = B组中心位置 - A组中心位置
    //
    // 这里采用较早一版正式测量门槛：
    // signedLagDiff 用于约束 A/B 延迟一致性；
    // pairMidGap 用于约束 A/B 回波中心位置关系。
    // 注意：pairMidGap 的目标值会随实验批次和识别策略变化，
    // 当前先恢复为 [-4, 4] 的旧标准。
    double anglePairMidGapMin = -4.0;
    double anglePairMidGapMax = 4.0;

    // 竖向平衡条的目标中心。
    // 注意：不要再用 (min+max)/2 自动算。
    // 现在明确让 pairMidGap=0 时两个竖条在 50%左右。
    double anglePairMidGapTarget = 0.0;
    double angleSignedDiffTarget = 9.0;

    double balanceFullScaleGap = 35.0;   // pairMidGap 偏离多少点时接近打满
    double balanceMaxOffset = 320.0;     // 最大偏离 500 的距离
    double balanceCurvePower = 1.7;      // >1：中线附近刻度更细

    // ================== 正式 SOS 输出策略 ==================
    //
    // 现在 A 通道主要用于姿态门控，不建议继续直接参与最终 SOS 平均。
    // 因为桡骨上 A 通道系统性低于 B 通道，继续 0.8B+0.2A 会把最终值拉低。
    bool useBOnlyForPatientSos = false;

    // 最终 SOS 校准偏移，先设为 0。
    // 如果后续大量对比参考仪器后发现整体偏低/偏高，再用它微调。
    // 例如整体偏低 30 m/s，可设为 +30.0。
    double patientSosOffset = 0.0;

    QVector<int> findBestRoundCluster(double tolerance) const;
    void rebuildAcceptedRoundsFromCandidates();
    double medianValue(QVector<double> values) const;

    bool hasCurrentPatient() const;

    void startPatientMeasurement(int targetRounds);
    void stopPatientMeasurement();
    void resetPatientMeasurementState(int targetRounds);

    void resetAllPatientMeasurementData();
    void resetOneRoundMeasurementState();

    void showRoundFinishedTip(int finishedRounds, int totalRounds);
    void closeRoundFinishedTip();

    void handlePatientMeasureValue(double sosA,
                                   double sosB,
                                   double sosAvg,
                                   int lagA,
                                   int lagB,
                                   int diffLag,
                                   double pairMidGap,
                                   double corrA,
                                   double corrB,
                                   bool strictValid);

    void finishOnePatientRound();
    void finishAllPatientRounds();

    double meanValue(const QVector<double>& values) const;

    double trimmedMeanValue(const QVector<double>& values, double trimRatio = 0.2) const;

    int calcPatientAge() const;
    double calcAgeReferenceMean(int age) const;
    QString classifyBoneStrength(double tScore) const;
    double calcRelativeFractureRisk(double tScore) const;
    int estimateBoneAgeFromSos(double sos) const;

    void initLatestResultPanel();
    void updateLatestResultPanel(double sos,
                                 double tScore,
                                 double zScore,
                                 const QString& strength,
                                 double risk,
                                 int boneAge);

    void showPatientMeasureFinishedDialog(double sos,
                                          double tScore,
                                          double zScore,
                                          const QString& strength,
                                          double risk,
                                          int boneAge);

    void initProcessPanel();
    void addMiddleLineToProgressBar(QProgressBar *bar);

    void updateProcessPanel(double sosA,
                            double sosB,
                            int lagA,
                            int lagB,
                            int diffLag,
                            double pairMidGap,
                            bool countThisFrame);

    void updateProcessInvalid(const QString& reason);

    // ================== 假波抹除 + 首波检测新增结构体 ==================
    struct GateConfig {
        int baselineStart;
        int baselineEnd;

        int eraseStart;   // 从这里开始抹掉假波
        int eraseEnd;     // 抹到这里为止，期间置为基线
        int rampEnd;      // eraseEnd~rampEnd 平滑恢复到原始波形
    };

    struct ArrivalResult {
        bool valid = false;
        int onset = -1;       // 第一波前沿点
        int peak = -1;        // 第一波包络峰
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
        int idx = -1;          // 波谷位置
        double value = 0.0;    // 波谷值，注意是滤波后未加2048的值
        double depth = 0.0;    // 波谷深度 = -value
    };

    QVector<double> preprocessRawForFIR(const QVector<quint16>& input,
                                        const GateConfig& cfg,
                                        const QString& name) const;

    QVector<double> applyFIRDouble(const QVector<double>& input) const;

    ArrivalResult detectFirstArrivalSmart(const QVector<double>& fil,
                                          int noiseStart,
                                          int noiseEnd,
                                          int searchStart,
                                          int searchEnd,
                                          double kSigma,
                                          int runLen,
                                          int envWin,
                                          double onsetRatio,
                                          int peakLookAhead) const;

    PairResult estimatePairSpeed(const QVector<double>& early,
                                 const QVector<double>& late,
                                 const ArrivalResult& earlyPick,
                                 const ArrivalResult& latePick,
                                 double vMin,
                                 double vMax,
                                 const QString& pairName,
                                 int forcedLagMin = -1,
                                 int forcedLagMax = -1) const;

    int refineLagByPositiveCrossCorrelation(const QVector<double>& early,
                                            const QVector<double>& late,
                                            int earlyOnset,
                                            int roughLag,
                                            int lagMin,
                                            int lagMax,
                                            int prePts,
                                            int postPts,
                                            double* bestCorrOut) const;

    ValleyResult findFirstProminentValley(const QVector<double>& x,
                                          int start,
                                          int end,
                                          double minDepth,
                                          double ratioToMax,
                                          int minSep) const;

    PairResult estimatePairSpeedByValley(const QVector<double>& early,
                                         const QVector<double>& late,
                                         const ArrivalResult& earlyPick,
                                         int priorLag,
                                         int priorTolerance,
                                         const QString& pairName) const;

    // ================== 角度/姿态特征调试 ==================
    // 只用于调试输出，不参与正式测量判定。
    // 目的是研究：3600、3800、4000 三种桡骨状态下，
    // 四路波形的绝对位置关系到底有什么区别。
    double safeRatio(double numerator, double denominator) const;

    void printAngleFeatureDebug(const QVector<double>& filBC,
                                const QVector<double>& filBD,
                                const QVector<double>& filAC,
                                const QVector<double>& filAD,
                                const ArrivalResult& pickBC,
                                const ArrivalResult& pickBD,
                                const ArrivalResult& pickAC,
                                const ArrivalResult& pickAD,
                                const PairResult& bRes,
                                const PairResult& aRes,
                                double sosAvg,
                                double wB) const;

    void appendAngleFeatureCsv(const QString& mode,
                               double sosAvg,
                               double sosA,
                               double sosB,
                               int lagA,
                               int lagB,
                               int diffLag,
                               int signedLagDiff,
                               double wB,
                               double corrA,
                               double corrB,
                               int bcOnset,
                               int bdOnset,
                               int acOnset,
                               int adOnset,
                               int bcPeak,
                               int bdPeak,
                               int acPeak,
                               int adPeak,
                               int bcValley,
                               int bdValley,
                               int acValley,
                               int adValley,
                               int valleyLagB,
                               int valleyLagA,
                               double pairMidB,
                               double pairMidA,
                               double pairMidGap,
                               double onsetMidB,
                               double onsetMidA,
                               double onsetMidGap,
                               double peakMidB,
                               double peakMidA,
                               double peakMidGap,
                               double valleyMidB,
                               double valleyMidA,
                               double valleyMidGap,
                               double depthBC,
                               double depthBD,
                               double depthAC,
                               double depthAD,
                               double depthRatioBCBD,
                               double depthRatioACAD,
                               double depthRatioAB,
                               double expectedOffset,
                               double offsetResidual) const;

};
