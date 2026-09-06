#include "calibration.h"

#include <QDateTime>
#include <QUuid>
#include <cmath>

namespace {

bool finitePositive(double value)
{
    return std::isfinite(value) && value > 0.0;
}

void setError(QString* error, const QString& value)
{
    if (error) *error = value;
}

} // namespace

bool CalibrationSession::configure(const CalibrationSetup& setup,
                                   double activeD,
                                   QString* error)
{
    if (!validateSetup(setup, activeD, error)) return false;

    record_ = CalibrationRecord{};
    record_.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    record_.startedAt = QDateTime::currentDateTime().toString(Qt::ISODate);
    record_.setup = setup;
    record_.setup.operatorName = setup.operatorName.trimmed();
    record_.setup.probeId = setup.probeId.trimmed();
    record_.setup.phantomModel = setup.phantomModel.trimmed();
    record_.setup.phantomSerial = setup.phantomSerial.trimmed();
    record_.setup.certificateId = setup.certificateId.trimmed();
    record_.oldD = activeD;
    record_.finalActiveD = activeD;
    record_.status = QStringLiteral("CONFIGURED");
    pending_ = PendingMeasurement{};
    configured_ = true;
    return true;
}

bool CalibrationSession::beginMeasurement(CalibrationPhase phase,
                                           double currentTemperature,
                                           QString* error)
{
    if (!configured_) {
        setError(error, QStringLiteral("请先填写并确认校准资料。"));
        return false;
    }
    if (pending_.active) {
        setError(error, QStringLiteral("当前测量尚未结束。"));
        return false;
    }
    if (!std::isfinite(currentTemperature)
        || std::abs(currentTemperature - record_.setup.referenceTemperature)
            > TemperatureToleranceC) {
        setError(error, QStringLiteral("当前试块温度与参考温度相差超过 ±1 C。"));
        return false;
    }

    if (phase == CalibrationPhase::Calibration) {
        if (record_.calibrationMeasurements.size() >= RequiredMeasurementsPerPhase) {
            setError(error, QStringLiteral("校准阶段已经完成6次独立测量。"));
            return false;
        }
        if (!record_.validationMeasurements.isEmpty()) {
            setError(error, QStringLiteral("进入验证阶段后不能再追加校准数据。"));
            return false;
        }
    } else {
        if (!calibrationComplete()) {
            setError(error, QStringLiteral("必须先完成6次校准测量。"));
            return false;
        }
        if (record_.validationMeasurements.size() >= RequiredMeasurementsPerPhase) {
            setError(error, QStringLiteral("独立验证阶段已经完成6次测量。"));
            return false;
        }
    }

    pending_ = PendingMeasurement{};
    pending_.active = true;
    pending_.phase = phase;
    pending_.temperature = currentTemperature;
    return true;
}

CalibrationFrameResult CalibrationSession::addFrame(const CalibrationFrame& frame,
                                                     QString* rejectionReason)
{
    if (!pending_.active) {
        setError(rejectionReason, QStringLiteral("当前没有正在进行的独立测量。"));
        return CalibrationFrameResult::Rejected;
    }
    if (!frame.bValid || !finitePositive(frame.sosB) || !std::isfinite(frame.corrB)) {
        pending_.rejectedFrames++;
        setError(rejectionReason, QStringLiteral("B通道信号无效。"));
        return CalibrationFrameResult::Rejected;
    }
    if (frame.boundaryPeak) {
        pending_.rejectedFrames++;
        setError(rejectionReason, QStringLiteral("相关峰位于搜索边界。"));
        return CalibrationFrameResult::Rejected;
    }
    if (pending_.phase == CalibrationPhase::Validation && !frame.peakConsistent) {
        pending_.rejectedFrames++;
        setError(rejectionReason, QStringLiteral("候选D与原D选择的相关峰不一致。"));
        return CalibrationFrameResult::Rejected;
    }

    pending_.sosB.append(frame.sosB);
    pending_.corrB.append(frame.corrB);
    pending_.lagB.append(frame.lagB);

    if (frame.aValid && finitePositive(frame.sosA) && std::isfinite(frame.corrA)) {
        pending_.sosA.append(frame.sosA);
        pending_.corrA.append(frame.corrA);
        pending_.lagA.append(frame.lagA);
    }

    if (pending_.sosB.size() >= RequiredFramesPerMeasurement) {
        finishCurrentMeasurement();
        return CalibrationFrameResult::MeasurementCompleted;
    }
    return CalibrationFrameResult::Accepted;
}

void CalibrationSession::cancelCurrentMeasurement()
{
    pending_ = PendingMeasurement{};
}

bool CalibrationSession::isConfigured() const
{
    return configured_;
}

bool CalibrationSession::isCollecting() const
{
    return pending_.active;
}

CalibrationPhase CalibrationSession::currentPhase() const
{
    return pending_.phase;
}

int CalibrationSession::currentValidFrameCount() const
{
    return pending_.sosB.size();
}

int CalibrationSession::currentRejectedFrameCount() const
{
    return pending_.rejectedFrames;
}

int CalibrationSession::completedMeasurementCount(CalibrationPhase phase) const
{
    return phase == CalibrationPhase::Calibration
        ? record_.calibrationMeasurements.size()
        : record_.validationMeasurements.size();
}

bool CalibrationSession::calibrationComplete() const
{
    return record_.calibrationMeasurements.size() >= RequiredMeasurementsPerPhase
        && finitePositive(record_.candidateD);
}

bool CalibrationSession::validationComplete() const
{
    return record_.validationMeasurements.size() >= RequiredMeasurementsPerPhase
        && record_.evaluation.complete;
}

bool CalibrationSession::canActivate() const
{
    return record_.setup.mode == CalibrationMode::Formal
        && validationComplete()
        && record_.evaluation.passed;
}

double CalibrationSession::processingD() const
{
    if (pending_.phase == CalibrationPhase::Validation && finitePositive(record_.candidateD)) {
        return record_.candidateD;
    }
    return record_.oldD;
}

double CalibrationSession::candidateD() const
{
    return record_.candidateD;
}

const CalibrationRecord& CalibrationSession::record() const
{
    return record_;
}

double CalibrationSession::mean(const QVector<double>& values)
{
    if (values.isEmpty()) return 0.0;
    double total = 0.0;
    for (double value : values) total += value;
    return total / values.size();
}

double CalibrationSession::coefficientOfVariationPercent(const QVector<double>& values)
{
    if (values.size() < 2) return 0.0;
    const double average = mean(values);
    if (!finitePositive(average)) return 0.0;

    double squared = 0.0;
    for (double value : values) {
        const double delta = value - average;
        squared += delta * delta;
    }
    const double sampleDeviation = std::sqrt(squared / (values.size() - 1));
    return sampleDeviation / average * 100.0;
}

double CalibrationSession::candidateD(double activeD,
                                      double referenceSos,
                                      double measuredSos)
{
    if (!finitePositive(activeD)
        || !finitePositive(referenceSos)
        || !finitePositive(measuredSos)) {
        return 0.0;
    }
    return activeD * referenceSos / measuredSos;
}

bool CalibrationSession::validateSetup(const CalibrationSetup& setup,
                                       double activeD,
                                       QString* error) const
{
    if (setup.operatorName.trimmed().isEmpty()) {
        setError(error, QStringLiteral("操作者不能为空。"));
        return false;
    }
    if (setup.probeId.trimmed().isEmpty()) {
        setError(error, QStringLiteral("探头编号不能为空。"));
        return false;
    }
    if (!finitePositive(activeD)) {
        setError(error, QStringLiteral("当前D不是有效正数。"));
        return false;
    }
    if (!std::isfinite(setup.referenceTemperature)) {
        setError(error, QStringLiteral("参考温度无效。"));
        return false;
    }
    if (!finitePositive(setup.referenceSos)) {
        setError(error, QStringLiteral("参考SOS必须为正数。"));
        return false;
    }
    if (setup.mode == CalibrationMode::Formal
        && (setup.phantomModel.trimmed().isEmpty()
            || setup.phantomSerial.trimmed().isEmpty()
            || setup.certificateId.trimmed().isEmpty())) {
        setError(error, QStringLiteral("正式校准必须填写试块型号、序列号和证书编号。"));
        return false;
    }
    return true;
}

void CalibrationSession::finishCurrentMeasurement()
{
    CalibrationMeasurement measurement;
    measurement.phase = pending_.phase;
    measurement.index = completedMeasurementCount(pending_.phase) + 1;
    measurement.measuredAt = QDateTime::currentDateTime().toString(Qt::ISODate);
    measurement.temperature = pending_.temperature;
    measurement.validFrameCount = pending_.sosB.size();
    measurement.rejectedFrameCount = pending_.rejectedFrames;
    measurement.meanSosB = mean(pending_.sosB);
    measurement.meanCorrB = mean(pending_.corrB);
    measurement.meanLagB = mean(pending_.lagB);
    measurement.aDiagnosticAvailable = !pending_.sosA.isEmpty();
    measurement.meanSosA = mean(pending_.sosA);
    measurement.meanCorrA = mean(pending_.corrA);
    measurement.meanLagA = mean(pending_.lagA);
    measurement.peakConsistent = true;

    if (pending_.phase == CalibrationPhase::Calibration) {
        record_.calibrationMeasurements.append(measurement);
    } else {
        record_.validationMeasurements.append(measurement);
    }
    pending_ = PendingMeasurement{};

    if (record_.calibrationMeasurements.size() == RequiredMeasurementsPerPhase) {
        updateCandidate();
    }
    if (record_.validationMeasurements.size() == RequiredMeasurementsPerPhase) {
        updateEvaluation();
    }
}

void CalibrationSession::updateCandidate()
{
    QVector<double> values;
    for (const CalibrationMeasurement& measurement : record_.calibrationMeasurements) {
        values.append(measurement.meanSosB);
    }
    record_.candidateD = candidateD(record_.oldD,
                                    record_.setup.referenceSos,
                                    mean(values));
    record_.status = finitePositive(record_.candidateD)
        ? QStringLiteral("CANDIDATE_READY")
        : QStringLiteral("FAILED");
}

void CalibrationSession::updateEvaluation()
{
    QVector<double> values;
    for (const CalibrationMeasurement& measurement : record_.validationMeasurements) {
        values.append(measurement.meanSosB);
    }

    record_.evaluation.complete = values.size() == RequiredMeasurementsPerPhase;
    record_.evaluation.meanSosB = mean(values);
    record_.evaluation.relativeErrorPercent = finitePositive(record_.setup.referenceSos)
        ? (record_.evaluation.meanSosB - record_.setup.referenceSos)
            / record_.setup.referenceSos * 100.0
        : 0.0;
    record_.evaluation.repeatabilityCvPercent = coefficientOfVariationPercent(values);

    const bool errorOk = std::abs(record_.evaluation.relativeErrorPercent)
        <= MaxErrorPercent;
    const bool repeatabilityOk = record_.evaluation.repeatabilityCvPercent
        <= MaxRepeatabilityCvPercent;
    record_.evaluation.passed = record_.evaluation.complete && errorOk && repeatabilityOk;

    if (!errorOk) {
        record_.evaluation.failureReason = QStringLiteral("B通道平均SOS相对误差超过 ±2%。");
    } else if (!repeatabilityOk) {
        record_.evaluation.failureReason = QStringLiteral("6次验证结果的变异系数超过1%。");
    }

    record_.finishedAt = QDateTime::currentDateTime().toString(Qt::ISODate);
    record_.status = record_.evaluation.passed
        ? QStringLiteral("PASSED")
        : QStringLiteral("FAILED");
}

QString calibrationModeToString(CalibrationMode mode)
{
    return mode == CalibrationMode::Formal
        ? QStringLiteral("formal")
        : QStringLiteral("development");
}

CalibrationMode calibrationModeFromString(const QString& value)
{
    return value == QStringLiteral("development")
        ? CalibrationMode::Development
        : CalibrationMode::Formal;
}

QString calibrationPhaseToString(CalibrationPhase phase)
{
    return phase == CalibrationPhase::Calibration
        ? QStringLiteral("calibration")
        : QStringLiteral("validation");
}

CalibrationPhase calibrationPhaseFromString(const QString& value)
{
    return value == QStringLiteral("validation")
        ? CalibrationPhase::Validation
        : CalibrationPhase::Calibration;
}
