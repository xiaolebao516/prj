#pragma once
#include <QElapsedTimer>
#include <QJsonObject>
#include <QList>
#include <QMainWindow>
#include <QPair>
#include <QSerialPort>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVector>

#include "records.h"
#include "calibration/calibrationstore.h"
#include "device/deviceprotocol.h"
#include "measurement/frameanalyzer.h"
#include "measurement/measurementexperimentlog.h"
#include "measurement/measurementprofile.h"
#include "measurement/measurementsession.h"
#include "measurement/measurementtypes.h"
#include "measurement/signalprocessor.h"
#include "storage/accountstore.h"
#include "storage/patientstore.h"
#include "widgets/reportwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QAction;
class QChart;
class QChartView;
class QCheckBox;
class QCloseEvent;
class QDateEdit;
class QLabel;
class QLineSeries;
class QMessageBox;
class QPrinter;
class QProgressBar;
class QPushButton;
class QSlider;
class QToolButton;
class CalibrationDialog;
class MainWindowSafetyTests;
class AvatarBadge;
class TScoreGauge;
class RoundProgress;

enum AcquireMode {
    DebugAcquireMode,       // trigger / 获取波形：只调试，不走病人测量流程
    PatientMeasureMode,     // 开始检测：走病人测量流程
    CalibrationAcquireMode  // 标准试块校准：使用独立校准处理器
};

// The application window. It wires the device link, the measurement core
// (FrameAnalyzer, MeasurementSession) and the stores to the UI; the
// measurement algorithm itself lives in src/core/measurement.
//
// Split by area: mainwindow.cpp (construction, login, close guard, accounts),
// _device (serial link), _measurement (patient measurement flow, calibration
// acquisition, guide), _display (live charts, process panel), _patients
// (archive, history, export, backups), _report, _layout (theme, page layouts).
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnLogin_clicked();
    void manageAccounts();

    // 串口
    void scanPorts();
    void on_connectButton_clicked();
    void on_triggerButton_clicked();
    void handleSerialReadyRead();
    void handleSerialError(QSerialPort::SerialPortError error);
    void sendCmd();

    // 页面切换
    void on_btnArchive_clicked();
    void on_btnBackFromArchive_clicked();
    void on_btnPatientInfo_clicked();
    void on_btnStartMeasurement_clicked();
    void on_btnMeasurementGuide_clicked();

    void updateCurrentPatientUI();
    void on_btnSaveResult_clicked();
    void on_btnSelectPatient_clicked();
    void on_btnViewHistory_clicked();

    // 档案管理
    void on_btnShowAll_clicked();
    void on_btnSearchName_clicked();
    void on_btnAdd_clicked();
    void on_table_cellDoubleClicked(int row, int column);
    void on_btnDeleteSelected_clicked();
    void scheduleResponsiveLayout();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    friend class MainWindowSafetyTests;
    Ui::MainWindow *ui;

    // ================== 设备连接（mainwindow_device.cpp）==================
    QSerialPort *serial;
    bool serialErrorHandled = false;
    QTimer scanTimer;
    QTimer *autoTimer;                 // 每 80 ms 发送一条采集命令
    bool autoRunning = false;          // 调试用连续采集
    quint16 globalGain = 1024;         // 统一的增益值
    quint16 nextFrameIdx = 0;          // 每条采集命令 +1（循环）
    FrameAssembler frameAssembler;     // 串口字节流 -> 四通道完整帧
    WaveFrame currentFrame;            // 正在处理的一帧原始数据

    void on_btnAcquireWaveform_clicked();   // “获取波形”按钮
    void processReceivedFrames();           // 处理已拼好的完整帧
    void resetFrameAssembly();              // 丢弃半帧和当前帧
    void restartFrameStream();              // 同上，并丢弃串口里尚未读取的数据
    void resetDisconnectedAcquisitionState();

    // ports: (name, description). preferred is selected when the previous
    // selection is gone (e.g. at start-up); otherwise the selection is kept.
    void applyPortList(const QList<QPair<QString, QString>>& ports,
                       const QString& preferred = QString());
    QString rememberedPort() const;
    void rememberPort(const QString& portName);
    QString deviceSettingsPath;

    // Device link feedback (display only; command timing is unchanged).
    void noteCommandSent();
    void noteDeviceFrameReceived();
    void checkDeviceResponse();
    QTimer deviceWatchdog;
    QElapsedTimer awaitingFrameSince;
    bool awaitingDeviceFrame = false;
    bool deviceUnresponsive = false;
    int deviceResponseTimeoutMs = 2500;

    // ================== 测量核心（src/core/measurement）==================
    MeasureConfig mCfg;
    MeasurementProfile profile = MeasurementProfile::current();
    SignalProcessor signalProcessor;
    SignalProcessor calibrationSignalProcessor;
    FrameAnalyzer frameAnalyzer{signalProcessor, mCfg, profile};
    MeasurementSession session{signalProcessor, mCfg, profile};

    // ================== 被测者检测流程（mainwindow_measurement.cpp）==================
    AcquireMode acquireMode = DebugAcquireMode;
    bool patientMeasureRunning = false;
    bool patientMeasurementActive() const
    {
        return patientMeasureRunning && acquireMode == PatientMeasureMode;
    }

    QTimer nextRoundTimer;
    int nextRoundDelayMs = 1000;
    QString pendingNextRoundPatientId;
    int pendingNextRoundFinishedRounds = 0;
    QMessageBox *measureTipBox = nullptr;   // 每轮完成后的非阻塞提示框

    void processFilteredFrame(const FilteredFrame& frame);
    void processCalibrationFrame(const FilteredFrame& frame);
    // A patient frame that failed before the gates: expires stability after a
    // sustained loss and clears the live readings.
    void rejectPatientFrame();
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

    bool hasCurrentPatient() const;
    void startPatientMeasurement(bool offerFirstUseGuide = false);
    void stopPatientMeasurement();
    void stopPatientMeasurementManually();
    void scheduleNextPatientRound(int finishedRounds);
    void cancelPendingNextPatientRound();
    void resetAllPatientMeasurementData();
    void resetOneRoundMeasurementState();
    bool hasIncompletePatientRounds() const;
    void showRoundFinishedTip(int finishedRounds, int totalRounds, bool accepted = true);
    void closeRoundFinishedTip();
    void showPatientMeasureFinishedDialog(const MeasurementRecord& completedMeasurement);

    bool runMeasurementGuide(bool automatic);
    bool shouldOfferMeasurementGuide() const;
    QString measurementGuideSettingsPath;
    bool measurementGuideSeenThisRun = false;

    // Development evidence (Debug builds): one log per round.
    MeasurementExperimentLog experimentLog;
    QString experimentSessionId;
    QJsonObject experimentSubjectSnapshot;
    bool experimentLogWarningShown = false;
    void startExperimentLog();
    void checkExperimentLogError();

    // ================== 探头校准 ==================
    CalibrationStore calibrationStore;
    CalibrationDialog* calibrationDialog = nullptr;
    void openCalibrationDialog();
    void startCalibrationAcquisition(double processingD);
    void stopCalibrationAcquisition();

    // ================== 波形与检测过程显示（mainwindow_display.cpp）==================
    QSlider *gainSliderA = nullptr;
    QSlider *gainSliderB = nullptr;
    QSlider *gainSliderC = nullptr;
    QSlider *gainSliderD = nullptr;

    QChart *chartA, *chartB, *chartC, *chartD;
    QLineSeries *seriesA, *seriesB, *seriesC, *seriesD;
    QChartView *viewA, *viewB, *viewC, *viewD;
    static constexpr qint64 liveWaveformRefreshIntervalMs = 250;
    QElapsedTimer liveWaveformRenderTimer;
    bool shouldRefreshLiveWaveforms();
    void setupChart();
    void onGainSliderChanged(int value);    // 任意一条增益滑条被拉
    void plotSamples();

    QChart *chartSpeed;
    QLineSeries *seriesSpeed;
    int speedPointIndex = 0;          // 声速曲线的横坐标计数
    void setupSpeedChart();
    void appendSpeedPoint(double speedAvg);

    // 声速调试显示：A/B 两组声速、正式采用值、lag/corr 信息
    QLabel *lblSosA = nullptr;
    QLabel *lblSosB = nullptr;
    QLabel *lblSosAvg = nullptr;
    QLabel *lblSosInfo = nullptr;
    void setupSpeedDebugPanel();
    void updateSpeedDebugPanel(double sosA, double sosB, double sosAvg,
                               int lagA, int lagB, int diffLag,
                               double corrA, double corrB);
    void setSpeedDebugInvalid(const QString& reason);

    void initProcessPanel();
    void addMiddleLineToProgressBar(QProgressBar *bar);
    bool eventFilter(QObject* watched, QEvent* event) override;
    void updateCorrAFeedback(double corrA);
    void clearFeedbackReadings();
    // Posture balance bars and D/G read-outs of the latest measured frame.
    void showPostureFeedback(int lagA, int lagB, double pairMidGap);
    // Valid-value progress of the round in progress.
    void updateMeasureProgress();
    QProgressBar* barCorrA = nullptr;
    QLabel* lblCorrAValue = nullptr;
    QLabel* lblCorrAStatus = nullptr;
    QLabel* lblCorrAThreshold = nullptr;
    QLabel* lblGStatus = nullptr;
    QLabel* lblDStatus = nullptr;
    QList<QLabel*> gainValueLabels;

    // ================== 档案与结果（mainwindow_patients.cpp）==================
    QString xmlFilePath;
    QString measurementsFilePath;
    QString accountsFilePath;
    QString calibrationFilePath;
    QList<PatientInfo> patientList;
    QList<MeasurementRecord> measurementList;
    PatientStore patientStore;
    AccountStore accountStore;
    AccountInfo currentAccount;
    PatientInfo currentPatient;            // 当前正在测量的被测者
    MeasurementRecord pendingMeasurement;  // 自动保存失败、等待重试的结果
    bool hasPendingMeasurement = false;
    bool patientDataWritable = false;
    QString patientDataLoadError;

    void loadPatients();
    bool savePatients(const QList<PatientInfo>& patients);
    bool saveMeasurements(const QList<MeasurementRecord>& measurements);
    bool savePatientData(const QList<PatientInfo>& patients,
                         const QList<MeasurementRecord>& measurements);
    bool ensurePatientDataWritable();
    bool trySavePendingMeasurement();
    QList<MeasurementRecord> measurementsForPatient(const QString& patientId) const;
    const MeasurementRecord* latestMeasurementForPatient(const QString& patientId) const;

    bool confirmPatientChange(const QString& targetPatientId);
    void applyCurrentPatient(const PatientInfo& patient);
    bool selectCurrentPatient(const PatientInfo& patient);
    void clearCurrentPatient();
    void updatePatientSelectionUi();
    void updateAgeSosReference();
    void showPatientHistory(const QString& patientId);
    void refreshTable(const QList<PatientInfo> &list);
    void updateArchiveSelectionBar();
    QString selectedArchivePatientId() const;
    void openNewPatientDialog(bool makeCurrent);
    void openEditPatientDialog(const QString& patientId);
    bool createPatient(const PatientInfo& patient, bool makeCurrent);
    bool updatePatient(const PatientInfo& patient);
    bool deleteMeasurementRecord(const QString& recordId);

    // CSV export of measurement records (archive page).
    QPushButton* btnExport = nullptr;
    QStringList checkedArchivePatientIds() const;
    void exportMeasurements();
    // Rows as shown on screen (current reference), sorted by patient and time.
    static QString measurementsCsv(const QList<PatientInfo>& patients,
                                   const QList<MeasurementRecord>& measurements);

    // Data backups (DataBackup): daily at first login, and before deletions.
    QString backupRoot() const;
    QStringList dataFilePaths() const;
    void backupDataDaily();
    bool backupBeforeDelete();

    // ================== 报表（mainwindow_report.cpp）==================
    ReportWidget* reportWidget = nullptr;
    QWidget* reportReturnPage = nullptr;
    void setupReportPage();
    void showReport(const PatientInfo& patient, const MeasurementRecord& measurement);
    void showReportFrom(QWidget* returnPage, const PatientInfo& patient,
                        const MeasurementRecord& measurement);
    ReportData buildReportData(const PatientInfo& patient,
                               const MeasurementRecord& measurement) const;
    AgeSosChartData buildAgeSosChartData(const PatientInfo& patient,
                                         const MeasurementRecord& focalMeasurement,
                                         bool cutoffAtFocal) const;
    bool renderReportToPrinter(QPrinter* printer);

    // ================== 账号（mainwindow.cpp）==================
    void updateAccountUi();
    void openDataFolder();
    void switchAccount();

    // ================== 界面布局（mainwindow_layout.cpp）==================
    static constexpr int ArchiveIdColumn = 0;
    static constexpr int ArchiveNameColumn = 1;
    static constexpr int ArchiveGenderColumn = 2;
    static constexpr int ArchiveBirthColumn = 3;
    static constexpr int ArchiveLatestColumn = 4;
    static constexpr int ArchiveSosColumn = 5;
    static constexpr int ArchiveCountColumn = 6;
    static constexpr int ArchiveColumnCount = 7;

    void applyTheme();
    void setupLoginPage();
    void setupMainLayout();
    void setupToolbar();
    void setupRightColumn();
    void setupArchivePage();
    void fitReferenceChartHeight();
    void updatePartImage();
    void initLatestResultPanel();
    void updateResultPanel();
    void refreshPatientDerivedViews();

    QWidget* mainBlock = nullptr;            // 前两列（波形/参考图/趋势/检测过程）
    QLabel* lblDeviceStatus = nullptr;
    QToolButton* btnAccount = nullptr;      // "账号 xxx" menu: 账号管理 / 打开数据文件夹 / 切换账号
    QAction* actManageAccounts = nullptr;
    QPushButton* btnNewPatient = nullptr;
    QLabel* lblPatientMeta = nullptr;
    QLabel* lblStartHint = nullptr;
    QLabel* lblResultNote = nullptr;
    QLabel* lblRecentHistory = nullptr;
    QLabel* lblArchiveSelection = nullptr;
    QLabel* lblCheckedCount = nullptr;
    QPushButton* btnEditPatient = nullptr;
    QCheckBox* chkDateFilter = nullptr;
    QDateEdit* dateFilter = nullptr;
    QLabel* lblRunState = nullptr;          // toolbar pill: 正在检测 · 第 n / 5 轮
    AvatarBadge* patientAvatar = nullptr;
    TScoreGauge* tScoreGauge = nullptr;
    RoundProgress* roundProgress = nullptr;
    void updateRunStateUi();
};
