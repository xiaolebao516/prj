#include "calibrationstore.h"

#include <QDateTime>
#include <QDomDocument>
#include <QFile>
#include <QSaveFile>
#include <QTextStream>
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

double doubleAttribute(const QDomElement& element,
                       const QString& name,
                       bool* ok = nullptr)
{
    bool converted = false;
    const double value = element.attribute(name).toDouble(&converted);
    if (ok) *ok = converted && std::isfinite(value);
    return value;
}

QDomElement measurementElement(QDomDocument& document,
                               const CalibrationMeasurement& measurement)
{
    QDomElement element = document.createElement(QStringLiteral("Measurement"));
    element.setAttribute(QStringLiteral("phase"), calibrationPhaseToString(measurement.phase));
    element.setAttribute(QStringLiteral("index"), measurement.index);
    element.setAttribute(QStringLiteral("measuredAt"), measurement.measuredAt);
    element.setAttribute(QStringLiteral("temperature"), QString::number(measurement.temperature, 'g', 16));
    element.setAttribute(QStringLiteral("validFrames"), measurement.validFrameCount);
    element.setAttribute(QStringLiteral("rejectedFrames"), measurement.rejectedFrameCount);
    element.setAttribute(QStringLiteral("meanSosA"), QString::number(measurement.meanSosA, 'g', 16));
    element.setAttribute(QStringLiteral("meanSosB"), QString::number(measurement.meanSosB, 'g', 16));
    element.setAttribute(QStringLiteral("meanCorrA"), QString::number(measurement.meanCorrA, 'g', 16));
    element.setAttribute(QStringLiteral("meanCorrB"), QString::number(measurement.meanCorrB, 'g', 16));
    element.setAttribute(QStringLiteral("meanLagA"), QString::number(measurement.meanLagA, 'g', 16));
    element.setAttribute(QStringLiteral("meanLagB"), QString::number(measurement.meanLagB, 'g', 16));
    element.setAttribute(QStringLiteral("aDiagnostic"), measurement.aDiagnosticAvailable ? 1 : 0);
    element.setAttribute(QStringLiteral("peakConsistent"), measurement.peakConsistent ? 1 : 0);
    return element;
}

CalibrationMeasurement parseMeasurement(const QDomElement& element)
{
    CalibrationMeasurement measurement;
    measurement.phase = calibrationPhaseFromString(element.attribute(QStringLiteral("phase")));
    measurement.index = element.attribute(QStringLiteral("index")).toInt();
    measurement.measuredAt = element.attribute(QStringLiteral("measuredAt"));
    measurement.temperature = element.attribute(QStringLiteral("temperature")).toDouble();
    measurement.validFrameCount = element.attribute(QStringLiteral("validFrames")).toInt();
    measurement.rejectedFrameCount = element.attribute(QStringLiteral("rejectedFrames")).toInt();
    measurement.meanSosA = element.attribute(QStringLiteral("meanSosA")).toDouble();
    measurement.meanSosB = element.attribute(QStringLiteral("meanSosB")).toDouble();
    measurement.meanCorrA = element.attribute(QStringLiteral("meanCorrA")).toDouble();
    measurement.meanCorrB = element.attribute(QStringLiteral("meanCorrB")).toDouble();
    measurement.meanLagA = element.attribute(QStringLiteral("meanLagA")).toDouble();
    measurement.meanLagB = element.attribute(QStringLiteral("meanLagB")).toDouble();
    measurement.aDiagnosticAvailable = element.attribute(QStringLiteral("aDiagnostic")) == QStringLiteral("1");
    measurement.peakConsistent = element.attribute(QStringLiteral("peakConsistent"), QStringLiteral("1")) == QStringLiteral("1");
    return measurement;
}

QDomElement recordElement(QDomDocument& document, const CalibrationRecord& record)
{
    QDomElement element = document.createElement(QStringLiteral("Session"));
    element.setAttribute(QStringLiteral("id"), record.id);
    element.setAttribute(QStringLiteral("startedAt"), record.startedAt);
    element.setAttribute(QStringLiteral("finishedAt"), record.finishedAt);
    element.setAttribute(QStringLiteral("mode"), calibrationModeToString(record.setup.mode));
    element.setAttribute(QStringLiteral("operator"), record.setup.operatorName);
    element.setAttribute(QStringLiteral("probeId"), record.setup.probeId);
    element.setAttribute(QStringLiteral("phantomModel"), record.setup.phantomModel);
    element.setAttribute(QStringLiteral("phantomSerial"), record.setup.phantomSerial);
    element.setAttribute(QStringLiteral("certificateId"), record.setup.certificateId);
    element.setAttribute(QStringLiteral("referenceTemperature"), QString::number(record.setup.referenceTemperature, 'g', 16));
    element.setAttribute(QStringLiteral("referenceSos"), QString::number(record.setup.referenceSos, 'g', 16));
    element.setAttribute(QStringLiteral("oldD"), QString::number(record.oldD, 'g', 16));
    element.setAttribute(QStringLiteral("candidateD"), QString::number(record.candidateD, 'g', 16));
    element.setAttribute(QStringLiteral("finalActiveD"), QString::number(record.finalActiveD, 'g', 16));
    element.setAttribute(QStringLiteral("status"), record.status);
    element.setAttribute(QStringLiteral("meanSosB"), QString::number(record.evaluation.meanSosB, 'g', 16));
    element.setAttribute(QStringLiteral("relativeErrorPercent"), QString::number(record.evaluation.relativeErrorPercent, 'g', 16));
    element.setAttribute(QStringLiteral("repeatabilityCvPercent"), QString::number(record.evaluation.repeatabilityCvPercent, 'g', 16));
    element.setAttribute(QStringLiteral("passed"), record.evaluation.passed ? 1 : 0);
    element.setAttribute(QStringLiteral("failureReason"), record.evaluation.failureReason);

    QDomElement measurements = document.createElement(QStringLiteral("Measurements"));
    for (const CalibrationMeasurement& measurement : record.calibrationMeasurements) {
        measurements.appendChild(measurementElement(document, measurement));
    }
    for (const CalibrationMeasurement& measurement : record.validationMeasurements) {
        measurements.appendChild(measurementElement(document, measurement));
    }
    element.appendChild(measurements);
    return element;
}

CalibrationRecord parseRecord(const QDomElement& element)
{
    CalibrationRecord record;
    record.id = element.attribute(QStringLiteral("id"));
    record.startedAt = element.attribute(QStringLiteral("startedAt"));
    record.finishedAt = element.attribute(QStringLiteral("finishedAt"));
    record.setup.mode = calibrationModeFromString(element.attribute(QStringLiteral("mode")));
    record.setup.operatorName = element.attribute(QStringLiteral("operator"));
    record.setup.probeId = element.attribute(QStringLiteral("probeId"));
    record.setup.phantomModel = element.attribute(QStringLiteral("phantomModel"));
    record.setup.phantomSerial = element.attribute(QStringLiteral("phantomSerial"));
    record.setup.certificateId = element.attribute(QStringLiteral("certificateId"));
    record.setup.referenceTemperature = element.attribute(QStringLiteral("referenceTemperature")).toDouble();
    record.setup.referenceSos = element.attribute(QStringLiteral("referenceSos")).toDouble();
    record.oldD = element.attribute(QStringLiteral("oldD")).toDouble();
    record.candidateD = element.attribute(QStringLiteral("candidateD")).toDouble();
    record.finalActiveD = element.attribute(QStringLiteral("finalActiveD")).toDouble();
    record.status = element.attribute(QStringLiteral("status"));
    record.evaluation.complete = true;
    record.evaluation.meanSosB = element.attribute(QStringLiteral("meanSosB")).toDouble();
    record.evaluation.relativeErrorPercent = element.attribute(QStringLiteral("relativeErrorPercent")).toDouble();
    record.evaluation.repeatabilityCvPercent = element.attribute(QStringLiteral("repeatabilityCvPercent")).toDouble();
    record.evaluation.passed = element.attribute(QStringLiteral("passed")) == QStringLiteral("1");
    record.evaluation.failureReason = element.attribute(QStringLiteral("failureReason"));

    const QDomElement measurements = element.firstChildElement(QStringLiteral("Measurements"));
    for (QDomElement child = measurements.firstChildElement(QStringLiteral("Measurement"));
         !child.isNull();
         child = child.nextSiblingElement(QStringLiteral("Measurement"))) {
        const CalibrationMeasurement measurement = parseMeasurement(child);
        if (measurement.phase == CalibrationPhase::Calibration) {
            record.calibrationMeasurements.append(measurement);
        } else {
            record.validationMeasurements.append(measurement);
        }
    }
    return record;
}

} // namespace

bool CalibrationStore::loadOrInitialize(const QString& filePath, QString* error)
{
    filePath_ = filePath;
    lastBackupPath_.clear();
    parameters_ = CalibrationParameterState{};
    history_.clear();

    if (!QFile::exists(filePath_)) {
        return writeState(parameters_, history_, error);
    }
    return load(error);
}

bool CalibrationStore::saveCompletedSession(const CalibrationRecord& source,
                                            bool activateCandidate,
                                            QString* error)
{
    CalibrationRecord record = source;
    if (record.id.trimmed().isEmpty()) {
        setError(error, QStringLiteral("校准记录缺少唯一编号。"));
        return false;
    }
    if (!record.evaluation.complete) {
        setError(error, QStringLiteral("校准记录尚未完成独立验证。"));
        return false;
    }
    if (activateCandidate
        && (record.setup.mode != CalibrationMode::Formal
            || !record.evaluation.passed
            || !finitePositive(record.candidateD))) {
        setError(error, QStringLiteral("当前校准结果不允许激活。"));
        return false;
    }

    CalibrationParameterState nextParameters = parameters_;
    QList<CalibrationRecord> nextHistory = history_;
    for (qsizetype i = nextHistory.size() - 1; i >= 0; --i) {
        if (nextHistory.at(i).id == record.id) nextHistory.removeAt(i);
    }

    if (activateCandidate) {
        nextParameters.previousD = parameters_.activeD;
        nextParameters.activeD = record.candidateD;
        nextParameters.activeProbeId = record.setup.probeId;
        nextParameters.updatedAt = QDateTime::currentDateTime().toString(Qt::ISODate);
        record.finalActiveD = record.candidateD;
        record.status = QStringLiteral("ACTIVATED");
    } else {
        record.finalActiveD = parameters_.activeD;
        if (!record.evaluation.passed) {
            record.status = QStringLiteral("FAILED");
        } else if (record.setup.mode == CalibrationMode::Development) {
            record.status = QStringLiteral("DEVELOPMENT_COMPLETE");
        } else {
            record.status = QStringLiteral("PASSED_NOT_ACTIVATED");
        }
    }
    nextHistory.append(record);

    if (!writeState(nextParameters, nextHistory, error)) return false;
    parameters_ = nextParameters;
    history_ = nextHistory;
    return true;
}

bool CalibrationStore::restoreDefault(const QString& operatorName, QString* error)
{
    return restoreValue(FactoryDefaultD,
                        QStringLiteral("RESTORED_DEFAULT"),
                        operatorName,
                        error);
}

bool CalibrationStore::restorePrevious(const QString& operatorName, QString* error)
{
    if (!finitePositive(parameters_.previousD)) {
        setError(error, QStringLiteral("没有可恢复的上一参数。"));
        return false;
    }
    return restoreValue(parameters_.previousD,
                        QStringLiteral("RESTORED_PREVIOUS"),
                        operatorName,
                        error);
}

const CalibrationParameterState& CalibrationStore::parameters() const
{
    return parameters_;
}

const QList<CalibrationRecord>& CalibrationStore::history() const
{
    return history_;
}

QString CalibrationStore::filePath() const
{
    return filePath_;
}

QString CalibrationStore::lastBackupPath() const
{
    return lastBackupPath_;
}

bool CalibrationStore::load(QString* error)
{
    QFile file(filePath_);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(error, QStringLiteral("无法读取校准参数文件：%1").arg(file.errorString()));
        return false;
    }

    QDomDocument document;
    QString parseError;
    int parseLine = 0;
    int parseColumn = 0;
    if (!document.setContent(&file, &parseError, &parseLine, &parseColumn)) {
        file.close();
        return recoverCorruptFile(
            QStringLiteral("XML解析失败（%1:%2）：%3")
                .arg(parseLine).arg(parseColumn).arg(parseError),
            error);
    }
    file.close();

    const QDomElement root = document.documentElement();
    const QDomElement parameterElement = root.firstChildElement(QStringLiteral("Parameters"));
    bool activeOk = false;
    bool previousOk = false;
    const double activeD = doubleAttribute(parameterElement, QStringLiteral("activeD"), &activeOk);
    const double previousD = doubleAttribute(parameterElement, QStringLiteral("previousD"), &previousOk);
    if (root.tagName() != QStringLiteral("CalibrationData")
        || parameterElement.isNull()
        || !activeOk || !previousOk
        || !finitePositive(activeD) || !finitePositive(previousD)) {
        return recoverCorruptFile(QStringLiteral("校准参数字段缺失或数值无效。"), error);
    }

    CalibrationParameterState loadedParameters;
    loadedParameters.factoryDefaultD = FactoryDefaultD;
    loadedParameters.activeD = activeD;
    loadedParameters.previousD = previousD;
    loadedParameters.activeProbeId = parameterElement.attribute(QStringLiteral("activeProbeId"));
    loadedParameters.updatedAt = parameterElement.attribute(QStringLiteral("updatedAt"));

    QList<CalibrationRecord> loadedHistory;
    const QDomElement historyElement = root.firstChildElement(QStringLiteral("History"));
    for (QDomElement session = historyElement.firstChildElement(QStringLiteral("Session"));
         !session.isNull();
         session = session.nextSiblingElement(QStringLiteral("Session"))) {
        loadedHistory.append(parseRecord(session));
    }

    parameters_ = loadedParameters;
    history_ = loadedHistory;
    return true;
}

bool CalibrationStore::writeState(const CalibrationParameterState& parameters,
                                  const QList<CalibrationRecord>& history,
                                  QString* error) const
{
    if (filePath_.trimmed().isEmpty()) {
        setError(error, QStringLiteral("校准参数文件路径为空。"));
        return false;
    }

    QDomDocument document(QStringLiteral("CalibrationData"));
    QDomElement root = document.createElement(QStringLiteral("CalibrationData"));
    root.setAttribute(QStringLiteral("version"), QStringLiteral("1"));
    document.appendChild(root);

    QDomElement parameterElement = document.createElement(QStringLiteral("Parameters"));
    parameterElement.setAttribute(QStringLiteral("factoryDefaultD"), QString::number(FactoryDefaultD, 'g', 16));
    parameterElement.setAttribute(QStringLiteral("activeD"), QString::number(parameters.activeD, 'g', 16));
    parameterElement.setAttribute(QStringLiteral("previousD"), QString::number(parameters.previousD, 'g', 16));
    parameterElement.setAttribute(QStringLiteral("activeProbeId"), parameters.activeProbeId);
    parameterElement.setAttribute(QStringLiteral("updatedAt"), parameters.updatedAt);
    root.appendChild(parameterElement);

    QDomElement historyElement = document.createElement(QStringLiteral("History"));
    for (const CalibrationRecord& record : history) {
        historyElement.appendChild(recordElement(document, record));
    }
    root.appendChild(historyElement);

    QSaveFile file(filePath_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(error, QStringLiteral("无法写入校准参数文件：%1").arg(file.errorString()));
        return false;
    }
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    document.save(stream, 2);
    if (!file.commit()) {
        setError(error, QStringLiteral("校准参数文件提交失败：%1").arg(file.errorString()));
        return false;
    }
    return true;
}

bool CalibrationStore::recoverCorruptFile(const QString& reason, QString* error)
{
    const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmsszzz"));
    lastBackupPath_ = filePath_ + QStringLiteral(".corrupt-") + stamp + QStringLiteral(".bak");
    if (!QFile::copy(filePath_, lastBackupPath_)) {
        setError(error,
                 QStringLiteral("校准参数文件损坏，且无法创建备份。原因：%1").arg(reason));
        return false;
    }

    CalibrationParameterState recovered;
    QList<CalibrationRecord> emptyHistory;
    QString writeError;
    if (!writeState(recovered, emptyHistory, &writeError)) {
        setError(error,
                 QStringLiteral("校准参数文件损坏，已备份到 %1，但恢复默认参数失败：%2")
                     .arg(lastBackupPath_, writeError));
        return false;
    }

    parameters_ = recovered;
    history_.clear();
    setError(error,
             QStringLiteral("校准参数文件异常，已备份到 %1，并恢复默认D。原因：%2")
                 .arg(lastBackupPath_, reason));
    return true;
}

bool CalibrationStore::restoreValue(double value,
                                    const QString& status,
                                    const QString& operatorName,
                                    QString* error)
{
    if (!finitePositive(value)) {
        setError(error, QStringLiteral("目标D不是有效正数。"));
        return false;
    }
    if (operatorName.trimmed().isEmpty()) {
        setError(error, QStringLiteral("操作者不能为空。"));
        return false;
    }

    CalibrationParameterState nextParameters = parameters_;
    nextParameters.previousD = parameters_.activeD;
    nextParameters.activeD = value;
    nextParameters.updatedAt = QDateTime::currentDateTime().toString(Qt::ISODate);

    CalibrationRecord record;
    record.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    record.startedAt = nextParameters.updatedAt;
    record.finishedAt = nextParameters.updatedAt;
    record.setup.operatorName = operatorName.trimmed();
    record.setup.probeId = parameters_.activeProbeId;
    record.oldD = parameters_.activeD;
    record.candidateD = value;
    record.finalActiveD = value;
    record.status = status;

    QList<CalibrationRecord> nextHistory = history_;
    nextHistory.append(record);
    if (!writeState(nextParameters, nextHistory, error)) return false;
    parameters_ = nextParameters;
    history_ = nextHistory;
    return true;
}
