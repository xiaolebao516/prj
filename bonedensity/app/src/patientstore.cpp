#include "patientstore.h"

#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QSaveFile>
#include <QSet>
#include <QTextStream>
#include <QUuid>

namespace {

bool writeDocument(const QString& path, const QDomDocument& document, QString* errorMessage)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    QTextStream out(&file);
    document.save(out, 4);
    if (!file.commit()) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    return true;
}

QString transactionMarkerPath(const QString& patientsPath)
{
    return patientsPath + ".txn";
}

QString transactionBackupPath(const QString& path)
{
    return path + ".txn.bak";
}

QString nextBadBackupPath(const QString& path)
{
    QString backupPath = path + ".bad.bak";
    int suffix = 1;
    while (QFileInfo::exists(backupPath)) {
        backupPath = path + QString(".bad.%1.bak").arg(suffix++);
    }
    return backupPath;
}

void setBadFileError(const QString& path, const QString& reason, QString* errorMessage)
{
    const QString backupPath = nextBadBackupPath(path);
    QFile source(path);
    if (!source.copy(backupPath)) {
        if (errorMessage) {
            *errorMessage = reason + QString::fromUtf8("；异常文件备份失败：") + source.errorString();
        }
        return;
    }
    if (errorMessage) {
        *errorMessage = reason + QString::fromUtf8("；异常文件已备份到：") + backupPath;
    }
}

bool validatePatientsDocument(const QDomDocument& document,
                              QSet<QString>* patientIds,
                              bool* legacy,
                              QString* errorMessage)
{
    const QDomElement root = document.documentElement();
    if (root.tagName() != "patients") {
        if (errorMessage) *errorMessage = QString::fromUtf8("根节点不是 patients");
        return false;
    }

    const QString version = root.attribute("version").trimmed();
    if (!version.isEmpty() && version != "2") {
        if (errorMessage) {
            *errorMessage = QString::fromUtf8("不支持的 patients.xml 版本：") + version;
        }
        return false;
    }
    const bool isLegacy = version.isEmpty();
    QSet<QString> ids;
    const QDomNodeList nodes = root.elementsByTagName("patient");
    for (int i = 0; i < nodes.count(); ++i) {
        const QString id = nodes.at(i).toElement().attribute("id");
        if (id.trimmed().isEmpty()) {
            if (errorMessage) *errorMessage = QString::fromUtf8("患者编号为空");
            return false;
        }
        if (!isLegacy && ids.contains(id)) {
            if (errorMessage) *errorMessage = QString::fromUtf8("患者编号重复：") + id;
            return false;
        }
        ids.insert(id);
    }
    if (patientIds) *patientIds = ids;
    if (legacy) *legacy = isLegacy;
    return true;
}

bool validateMeasurementsDocument(const QDomDocument& document,
                                  const QSet<QString>& patientIds,
                                  QString* errorMessage)
{
    const QDomElement root = document.documentElement();
    if (root.tagName() != "measurements") {
        if (errorMessage) *errorMessage = QString::fromUtf8("根节点不是 measurements");
        return false;
    }
    const QString version = root.attribute("version").trimmed();
    if (version != "1") {
        if (errorMessage) {
            *errorMessage = QString::fromUtf8("不支持的 measurements.xml 版本：")
                + (version.isEmpty() ? QString::fromUtf8("空") : version);
        }
        return false;
    }

    QSet<QString> measurementIds;
    const QDomNodeList nodes = root.elementsByTagName("measurement");
    for (int i = 0; i < nodes.count(); ++i) {
        const QDomElement element = nodes.at(i).toElement();
        const QString id = element.attribute("id");
        const QString patientId = element.attribute("patientId");
        if (id.trimmed().isEmpty()) {
            if (errorMessage) *errorMessage = QString::fromUtf8("检测记录编号为空");
            return false;
        }
        if (patientId.trimmed().isEmpty()) {
            if (errorMessage) *errorMessage = QString::fromUtf8("检测记录患者编号为空");
            return false;
        }
        if (measurementIds.contains(id)) {
            if (errorMessage) *errorMessage = QString::fromUtf8("检测记录编号重复：") + id;
            return false;
        }
        if (!patientIds.contains(patientId)) {
            if (errorMessage) {
                *errorMessage = QString::fromUtf8("检测记录找不到对应患者：") + patientId;
            }
            return false;
        }
        measurementIds.insert(id);
    }
    return true;
}

bool readFileBytes(const QString& path, QByteArray* data, QString* errorMessage)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    *data = file.readAll();
    if (file.error() != QFileDevice::NoError) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    return true;
}

bool writeFileBytes(const QString& path, const QByteArray& data, QString* errorMessage)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    if (file.write(data) != data.size()) {
        if (errorMessage) *errorMessage = file.errorString();
        file.cancelWriting();
        return false;
    }
    if (!file.commit()) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    return true;
}

bool removeIfPresent(const QString& path, QString* errorMessage)
{
    if (!QFileInfo::exists(path)) return true;
    QFile file(path);
    if (file.remove()) return true;
    if (errorMessage) *errorMessage = file.errorString();
    return false;
}

bool copyTransactionBackup(const QString& sourcePath,
                           const QString& backupPath,
                           QString* errorMessage)
{
    QString removeError;
    if (!removeIfPresent(backupPath, &removeError)) {
        if (errorMessage) *errorMessage = QString::fromUtf8("无法清理旧事务备份：") + removeError;
        return false;
    }
    if (!QFileInfo::exists(sourcePath)) return true;

    QFile source(sourcePath);
    if (!source.copy(backupPath)) {
        if (errorMessage) *errorMessage = source.errorString();
        return false;
    }
    return true;
}

bool markerFlag(const QDomElement& root, const QString& name, bool* value)
{
    const QString text = root.attribute(name);
    if (text == "1") {
        *value = true;
        return true;
    }
    if (text == "0") {
        *value = false;
        return true;
    }
    return false;
}

bool recoverPendingTransaction(const QString& patientsPath,
                               const QString& measurementsPath,
                               QString* errorMessage)
{
    const QString markerPath = transactionMarkerPath(patientsPath);
    if (!QFileInfo::exists(markerPath)) return true;

    QFile markerFile(markerPath);
    if (!markerFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage) {
            *errorMessage = QString::fromUtf8("无法读取未完成的数据事务标记：") + markerFile.errorString();
        }
        return false;
    }
    QDomDocument markerDocument;
    const bool parsed = static_cast<bool>(markerDocument.setContent(&markerFile));
    markerFile.close();
    const QDomElement root = markerDocument.documentElement();
    bool patientsExisted = false;
    bool measurementsExisted = false;
    if (!parsed || root.tagName() != "patientDataTransaction" ||
        root.attribute("version") != "1" ||
        !markerFlag(root, "patientsExisted", &patientsExisted) ||
        !markerFlag(root, "measurementsExisted", &measurementsExisted)) {
        if (errorMessage) *errorMessage = QString::fromUtf8("未完成的数据事务标记无效，已拒绝加载档案");
        return false;
    }

    const QString patientsBackup = transactionBackupPath(patientsPath);
    const QString measurementsBackup = transactionBackupPath(measurementsPath);
    QByteArray patientBytes;
    QByteArray measurementBytes;
    QString ioError;
    if (patientsExisted && !readFileBytes(patientsBackup, &patientBytes, &ioError)) {
        if (errorMessage) {
            *errorMessage = QString::fromUtf8("患者事务备份缺失或不可读，已拒绝加载档案：") + ioError;
        }
        return false;
    }
    if (measurementsExisted && !readFileBytes(measurementsBackup, &measurementBytes, &ioError)) {
        if (errorMessage) {
            *errorMessage = QString::fromUtf8("检测记录事务备份缺失或不可读，已拒绝加载档案：") + ioError;
        }
        return false;
    }

    QSet<QString> backupPatientIds;
    if (patientsExisted) {
        QDomDocument patientDocument;
        QString validationError;
        const bool parsed = static_cast<bool>(patientDocument.setContent(patientBytes));
        if (!parsed) validationError = QString::fromUtf8("XML格式损坏");
        if (!parsed || !validatePatientsDocument(patientDocument, &backupPatientIds, nullptr,
                                                 &validationError)) {
            if (errorMessage) {
                *errorMessage = QString::fromUtf8(
                    "患者事务备份内容无效，已保留当前文件和事务现场：") + validationError;
            }
            return false;
        }
    }
    if (measurementsExisted) {
        QDomDocument measurementDocument;
        QString validationError;
        const bool parsed = static_cast<bool>(measurementDocument.setContent(measurementBytes));
        if (!parsed) validationError = QString::fromUtf8("XML格式损坏");
        if (!parsed || !validateMeasurementsDocument(measurementDocument, backupPatientIds,
                                                     &validationError)) {
            if (errorMessage) {
                *errorMessage = QString::fromUtf8(
                    "检测记录事务备份内容无效，已保留当前文件和事务现场：") + validationError;
            }
            return false;
        }
    }

    if (patientsExisted) {
        if (!writeFileBytes(patientsPath, patientBytes, &ioError)) {
            if (errorMessage) *errorMessage = QString::fromUtf8("患者事务恢复失败：") + ioError;
            return false;
        }
    } else if (!removeIfPresent(patientsPath, &ioError)) {
        if (errorMessage) *errorMessage = QString::fromUtf8("患者事务恢复失败：") + ioError;
        return false;
    }

    if (measurementsExisted) {
        if (!writeFileBytes(measurementsPath, measurementBytes, &ioError)) {
            if (errorMessage) *errorMessage = QString::fromUtf8("检测记录事务恢复失败：") + ioError;
            return false;
        }
    } else if (!removeIfPresent(measurementsPath, &ioError)) {
        if (errorMessage) *errorMessage = QString::fromUtf8("检测记录事务恢复失败：") + ioError;
        return false;
    }

    if (!removeIfPresent(markerPath, &ioError)) {
        if (errorMessage) *errorMessage = QString::fromUtf8("数据已恢复，但无法清理事务标记：") + ioError;
        return false;
    }
    removeIfPresent(patientsBackup, nullptr);
    removeIfPresent(measurementsBackup, nullptr);
    return true;
}

bool writeTransactionMarker(const QString& markerPath,
                            bool patientsExisted,
                            bool measurementsExisted,
                            QString* errorMessage)
{
    QDomDocument document;
    QDomElement root = document.createElement("patientDataTransaction");
    root.setAttribute("version", "1");
    root.setAttribute("patientsExisted", patientsExisted ? "1" : "0");
    root.setAttribute("measurementsExisted", measurementsExisted ? "1" : "0");
    document.appendChild(root);
    return writeDocument(markerPath, document, errorMessage);
}

PatientInfo patientFromElement(const QDomElement& element)
{
    PatientInfo patient;
    patient.id = element.attribute("id");
    patient.name = element.attribute("name");
    patient.gender = element.attribute("gender");
    patient.birthDay = element.attribute("birth");
    patient.height = element.attribute("height");
    patient.weight = element.attribute("weight");
    return patient;
}

MeasurementRecord recordFromElement(const QDomElement& element)
{
    MeasurementRecord record;
    record.id = element.attribute("id");
    record.patientId = element.attribute("patientId");
    record.measuredAt = element.attribute("measuredAt");
    record.operatorName = element.attribute("operator");
    record.part = element.attribute("part");
    record.sos = element.attribute("sos");
    record.tScore = element.attribute("tScore");
    record.zScore = element.attribute("zScore");
    record.diagnosis = element.attribute("diagnosis");
    record.patientName = element.attribute("patientName");
    record.patientGender = element.attribute("patientGender");
    record.patientBirthDay = element.attribute("patientBirthDay");
    record.patientHeight = element.attribute("patientHeight");
    record.patientWeight = element.attribute("patientWeight");
    record.patientAge = element.attribute("patientAge");
    record.boneStrength = element.attribute("boneStrength");
    record.fractureRisk = element.attribute("fractureRisk");
    record.boneAge = element.attribute("boneAge");
    return record;
}

MeasurementRecord legacyRecordFromElement(const QDomElement& element)
{
    MeasurementRecord record;
    record.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    record.patientId = element.attribute("id");
    const QString date = element.attribute("checkDate");
    record.measuredAt = date.isEmpty() ? QString() : date + "T00:00:00";
    record.part = element.attribute("part", QString::fromUtf8("桡骨"));
    record.sos = element.attribute("sos");
    record.diagnosis = element.attribute("diag");
    return record;
}

QString recordKey(const MeasurementRecord& record)
{
    return QStringList{record.patientId, record.measuredAt, record.part,
                       record.sos, record.diagnosis}.join(QChar(0x1f));
}

void appendUniqueRecord(QList<MeasurementRecord>* records, QSet<QString>* keys,
                        const MeasurementRecord& record)
{
    if (record.patientId.isEmpty()) return;
    const QString key = recordKey(record);
    if (keys->contains(key)) return;
    keys->insert(key);
    records->append(record);
}

bool loadMeasurementsFile(const QString& path,
                          const QSet<QString>& patientIds,
                          QList<MeasurementRecord>* records,
                          QString* errorMessage)
{
    if (!QFileInfo::exists(path)) return true;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setBadFileError(path,
                        QString::fromUtf8("无法读取 measurements.xml：") + file.errorString(),
                        errorMessage);
        return false;
    }
    QDomDocument document;
    const bool parsed = static_cast<bool>(document.setContent(&file));
    file.close();
    QString validationError;
    if (!parsed ||
        !validateMeasurementsDocument(document, patientIds, &validationError)) {
        const QString reason = parsed
            ? QString::fromUtf8("检测记录内容无效：") + validationError
            : QString::fromUtf8("无法读取 measurements.xml");
        setBadFileError(path, reason, errorMessage);
        return false;
    }

    const QDomNodeList nodes = document.documentElement().elementsByTagName("measurement");
    for (int i = 0; i < nodes.count(); ++i) {
        const MeasurementRecord record = recordFromElement(nodes.at(i).toElement());
        if (!record.id.isEmpty() && !record.patientId.isEmpty()) records->append(record);
    }
    return true;
}

} // namespace

bool PatientStore::load(const QString& patientsPath,
                        const QString& measurementsPath,
                        QList<PatientInfo>* patients,
                        QList<MeasurementRecord>* measurements,
                        QString* errorMessage) const
{
    if (!recoverPendingTransaction(patientsPath, measurementsPath, errorMessage)) return false;

    QList<PatientInfo> loadedPatients;
    QList<MeasurementRecord> legacyRecords;
    bool legacy = false;
    QSet<QString> patientIds;

    QFile patientFile(patientsPath);
    if (patientFile.exists()) {
        if (!patientFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            setBadFileError(patientsPath,
                            QString::fromUtf8("无法读取 patients.xml：") + patientFile.errorString(),
                            errorMessage);
            return false;
        }
        QDomDocument patientDocument;
        const bool parsed = static_cast<bool>(patientDocument.setContent(&patientFile));
        patientFile.close();
        QString validationError;
        if (!parsed ||
            !validatePatientsDocument(patientDocument,
                                      &patientIds,
                                      &legacy,
                                      &validationError)) {
            const QString reason = parsed
                ? QString::fromUtf8("患者档案内容无效：") + validationError
                : QString::fromUtf8("无法读取 patients.xml");
            setBadFileError(patientsPath, reason, errorMessage);
            return false;
        }

        const QDomElement root = patientDocument.documentElement();
        QHash<QString, int> patientIndexes;
        QSet<QString> legacyKeys;
        const QDomNodeList nodes = root.elementsByTagName("patient");
        for (int i = 0; i < nodes.count(); ++i) {
            const QDomElement element = nodes.at(i).toElement();
            const PatientInfo patient = patientFromElement(element);
            if (patient.id.isEmpty()) continue;
            if (!patientIndexes.contains(patient.id)) {
                patientIndexes.insert(patient.id, loadedPatients.size());
                loadedPatients.append(patient);
            }
            if (legacy && (!element.attribute("sos").isEmpty() ||
                           !element.attribute("diag").isEmpty() ||
                           !element.attribute("checkDate").isEmpty())) {
                appendUniqueRecord(&legacyRecords, &legacyKeys, legacyRecordFromElement(element));
            }
        }
    }

    QList<MeasurementRecord> existingMeasurements;
    if (!loadMeasurementsFile(measurementsPath,
                              patientIds,
                              &existingMeasurements,
                              errorMessage)) {
        return false;
    }

    QList<MeasurementRecord> loadedMeasurements = existingMeasurements;
    if (legacy) {
        const QString backupPath = patientsPath + ".bak";
        if (!QFileInfo::exists(backupPath) && !QFile::copy(patientsPath, backupPath)) {
            if (errorMessage) *errorMessage = QString::fromUtf8("旧 patients.xml 备份失败，已取消迁移");
            return false;
        }

        QSet<QString> keys;
        for (const MeasurementRecord& record : loadedMeasurements) keys.insert(recordKey(record));
        for (const MeasurementRecord& record : legacyRecords) {
            appendUniqueRecord(&loadedMeasurements, &keys, record);
        }

        QString saveError;
        if (!saveMeasurements(measurementsPath, loadedMeasurements, &saveError)) {
            if (errorMessage) *errorMessage = QString::fromUtf8("旧数据迁移失败：") + saveError;
            return false;
        }
        if (!savePatients(patientsPath, loadedPatients, &saveError)) {
            QString rollbackError;
            if (!saveMeasurements(measurementsPath, existingMeasurements, &rollbackError)) {
                saveError += QString::fromUtf8("；检测记录恢复失败：") + rollbackError;
            }
            if (errorMessage) *errorMessage = QString::fromUtf8("旧数据迁移失败：") + saveError;
            return false;
        }
    }

    *patients = loadedPatients;
    *measurements = loadedMeasurements;
    return true;
}

bool PatientStore::savePatients(const QString& patientsPath,
                                const QList<PatientInfo>& patients,
                                QString* errorMessage) const
{
    QDomDocument document;
    QDomElement root = document.createElement("patients");
    root.setAttribute("version", "2");
    document.appendChild(root);
    for (const PatientInfo& patient : patients) {
        QDomElement element = document.createElement("patient");
        element.setAttribute("id", patient.id);
        element.setAttribute("name", patient.name);
        element.setAttribute("gender", patient.gender);
        element.setAttribute("birth", patient.birthDay);
        element.setAttribute("height", patient.height);
        element.setAttribute("weight", patient.weight);
        root.appendChild(element);
    }
    return writeDocument(patientsPath, document, errorMessage);
}

bool PatientStore::saveMeasurements(const QString& measurementsPath,
                                    const QList<MeasurementRecord>& measurements,
                                    QString* errorMessage) const
{
    QDomDocument document;
    QDomElement root = document.createElement("measurements");
    root.setAttribute("version", "1");
    document.appendChild(root);
    for (const MeasurementRecord& record : measurements) {
        QDomElement element = document.createElement("measurement");
        element.setAttribute("id", record.id);
        element.setAttribute("patientId", record.patientId);
        element.setAttribute("measuredAt", record.measuredAt);
        element.setAttribute("operator", record.operatorName);
        element.setAttribute("part", record.part);
        element.setAttribute("sos", record.sos);
        element.setAttribute("tScore", record.tScore);
        element.setAttribute("zScore", record.zScore);
        element.setAttribute("diagnosis", record.diagnosis);
        element.setAttribute("patientName", record.patientName);
        element.setAttribute("patientGender", record.patientGender);
        element.setAttribute("patientBirthDay", record.patientBirthDay);
        element.setAttribute("patientHeight", record.patientHeight);
        element.setAttribute("patientWeight", record.patientWeight);
        element.setAttribute("patientAge", record.patientAge);
        element.setAttribute("boneStrength", record.boneStrength);
        element.setAttribute("fractureRisk", record.fractureRisk);
        element.setAttribute("boneAge", record.boneAge);
        root.appendChild(element);
    }
    return writeDocument(measurementsPath, document, errorMessage);
}

bool PatientStore::savePatientData(const QString& patientsPath,
                                   const QString& measurementsPath,
                                   const QList<PatientInfo>& patients,
                                   const QList<MeasurementRecord>& measurements,
                                   QString* errorMessage) const
{
    QString transactionError;
    if (!recoverPendingTransaction(patientsPath, measurementsPath, &transactionError)) {
        if (errorMessage) *errorMessage = transactionError;
        return false;
    }

    const QString markerPath = transactionMarkerPath(patientsPath);
    const QString patientsBackup = transactionBackupPath(patientsPath);
    const QString measurementsBackup = transactionBackupPath(measurementsPath);
    const bool patientsExisted = QFileInfo::exists(patientsPath);
    const bool measurementsExisted = QFileInfo::exists(measurementsPath);

    if (!copyTransactionBackup(patientsPath, patientsBackup, &transactionError)) {
        if (errorMessage) *errorMessage = QString::fromUtf8("患者事务备份失败：") + transactionError;
        return false;
    }
    if (!copyTransactionBackup(measurementsPath, measurementsBackup, &transactionError)) {
        removeIfPresent(patientsBackup, nullptr);
        if (errorMessage) *errorMessage = QString::fromUtf8("检测记录事务备份失败：") + transactionError;
        return false;
    }
    if (!writeTransactionMarker(markerPath, patientsExisted, measurementsExisted, &transactionError)) {
        removeIfPresent(patientsBackup, nullptr);
        removeIfPresent(measurementsBackup, nullptr);
        if (errorMessage) *errorMessage = QString::fromUtf8("数据事务标记创建失败：") + transactionError;
        return false;
    }

    QString saveError;
    if (!savePatients(patientsPath, patients, &saveError) ||
        !saveMeasurements(measurementsPath, measurements, &saveError)) {
        QString recoveryError;
        if (!recoverPendingTransaction(patientsPath, measurementsPath, &recoveryError)) {
            saveError += QString::fromUtf8("；事务恢复失败：") + recoveryError;
        }
        if (errorMessage) *errorMessage = saveError;
        return false;
    }

    if (!removeIfPresent(markerPath, &transactionError)) {
        QString recoveryError;
        if (!recoverPendingTransaction(patientsPath, measurementsPath, &recoveryError)) {
            transactionError += QString::fromUtf8("；事务恢复失败：") + recoveryError;
        }
        if (errorMessage) *errorMessage = QString::fromUtf8("数据已写入但事务无法完成：") + transactionError;
        return false;
    }
    removeIfPresent(patientsBackup, nullptr);
    removeIfPresent(measurementsBackup, nullptr);
    return true;
}
