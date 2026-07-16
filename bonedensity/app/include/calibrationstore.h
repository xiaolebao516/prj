#pragma once

#include "calibration.h"

#include <QList>
#include <QString>

struct CalibrationParameterState {
    double factoryDefaultD = 7.84e-3;
    double activeD = 7.84e-3;
    double previousD = 7.84e-3;
    QString activeProbeId;
    QString updatedAt;
};

class CalibrationStore
{
public:
    static constexpr double FactoryDefaultD = 7.84e-3;

    bool loadOrInitialize(const QString& filePath, QString* error = nullptr);
    bool saveCompletedSession(const CalibrationRecord& record,
                              bool activateCandidate,
                              QString* error = nullptr);
    bool restoreDefault(const QString& operatorName, QString* error = nullptr);
    bool restorePrevious(const QString& operatorName, QString* error = nullptr);

    const CalibrationParameterState& parameters() const;
    const QList<CalibrationRecord>& history() const;
    QString filePath() const;
    QString lastBackupPath() const;

private:
    bool load(QString* error);
    bool writeState(const CalibrationParameterState& parameters,
                    const QList<CalibrationRecord>& history,
                    QString* error) const;
    bool recoverCorruptFile(const QString& reason, QString* error);
    bool restoreValue(double value,
                      const QString& status,
                      const QString& operatorName,
                      QString* error);

    QString filePath_;
    QString lastBackupPath_;
    CalibrationParameterState parameters_;
    QList<CalibrationRecord> history_;
};
