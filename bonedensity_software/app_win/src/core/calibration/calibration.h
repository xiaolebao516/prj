#pragma once

#include <QList>
#include <QString>
#include <QVector>

enum class CalibrationMode {
    Formal,
    Development
};

enum class CalibrationPhase {
    Calibration,
    Validation
};

enum class CalibrationFrameResult {
    Rejected,
    Accepted,
    MeasurementCompleted
};

struct CalibrationSetup {
    CalibrationMode mode = CalibrationMode::Formal;
    QString operatorName;
    QString probeId;
    QString phantomModel;
    QString phantomSerial;
    QString certificateId;
    double referenceTemperature = 0.0;
    double referenceSos = 0.0;
};

struct CalibrationFrame {
    bool bValid = false;
    bool aValid = false;
    bool peakConsistent = true;
    bool boundaryPeak = false;
    double sosA = 0.0;
    double sosB = 0.0;
    double corrA = 0.0;
    double corrB = 0.0;
    int lagA = 0;
    int lagB = 0;
};

struct CalibrationMeasurement {
    CalibrationPhase phase = CalibrationPhase::Calibration;
    int index = 0;
    QString measuredAt;
    double temperature = 0.0;
    int validFrameCount = 0;
    int rejectedFrameCount = 0;
    double meanSosA = 0.0;
    double meanSosB = 0.0;
    double meanCorrA = 0.0;
    double meanCorrB = 0.0;
    double meanLagA = 0.0;
    double meanLagB = 0.0;
    bool aDiagnosticAvailable = false;
    bool peakConsistent = true;
};

struct CalibrationEvaluation {
    bool complete = false;
    bool passed = false;
    double meanSosB = 0.0;
    double relativeErrorPercent = 0.0;
    double repeatabilityCvPercent = 0.0;
    QString failureReason;
};

struct CalibrationRecord {
    QString id;
    QString startedAt;
    QString finishedAt;
    CalibrationSetup setup;
    double oldD = 0.0;
    double candidateD = 0.0;
    double finalActiveD = 0.0;
    QList<CalibrationMeasurement> calibrationMeasurements;
    QList<CalibrationMeasurement> validationMeasurements;
    CalibrationEvaluation evaluation;
    QString status;
};

class CalibrationSession
{
public:
    static constexpr int RequiredFramesPerMeasurement = 30;
    static constexpr int RequiredMeasurementsPerPhase = 6;
    static constexpr double TemperatureToleranceC = 1.0;
    static constexpr double MaxErrorPercent = 2.0;
    static constexpr double MaxRepeatabilityCvPercent = 1.0;

    bool configure(const CalibrationSetup& setup, double activeD, QString* error = nullptr);
    bool beginMeasurement(CalibrationPhase phase,
                          double currentTemperature,
                          QString* error = nullptr);
    CalibrationFrameResult addFrame(const CalibrationFrame& frame,
                                    QString* rejectionReason = nullptr);
    void cancelCurrentMeasurement();

    bool isConfigured() const;
    bool isCollecting() const;
    CalibrationPhase currentPhase() const;
    int currentValidFrameCount() const;
    int currentRejectedFrameCount() const;
    int completedMeasurementCount(CalibrationPhase phase) const;
    bool calibrationComplete() const;
    bool validationComplete() const;
    bool canActivate() const;
    double processingD() const;
    double candidateD() const;
    const CalibrationRecord& record() const;

    static double mean(const QVector<double>& values);
    static double coefficientOfVariationPercent(const QVector<double>& values);
    static double candidateD(double activeD, double referenceSos, double measuredSos);

private:
    struct PendingMeasurement {
        bool active = false;
        CalibrationPhase phase = CalibrationPhase::Calibration;
        double temperature = 0.0;
        int rejectedFrames = 0;
        QVector<double> sosA;
        QVector<double> sosB;
        QVector<double> corrA;
        QVector<double> corrB;
        QVector<double> lagA;
        QVector<double> lagB;
    };

    bool validateSetup(const CalibrationSetup& setup, double activeD, QString* error) const;
    void finishCurrentMeasurement();
    void updateCandidate();
    void updateEvaluation();

    bool configured_ = false;
    CalibrationRecord record_;
    PendingMeasurement pending_;
};

QString calibrationModeToString(CalibrationMode mode);
CalibrationMode calibrationModeFromString(const QString& value);
QString calibrationPhaseToString(CalibrationPhase phase);
CalibrationPhase calibrationPhaseFromString(const QString& value);
