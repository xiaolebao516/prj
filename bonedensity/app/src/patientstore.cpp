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

bool loadMeasurementsFile(const QString& path, QList<MeasurementRecord>* records,
                          QString* errorMessage)
{
    if (!QFileInfo::exists(path)) return true;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    QDomDocument document;
    const bool parsed = static_cast<bool>(document.setContent(&file));
    file.close();
    if (!parsed || document.documentElement().tagName() != "measurements") {
        if (errorMessage) *errorMessage = QString::fromUtf8("无法读取 measurements.xml");
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
    QList<PatientInfo> loadedPatients;
    QList<MeasurementRecord> legacyRecords;
    bool legacy = false;

    QFile patientFile(patientsPath);
    if (patientFile.exists()) {
        if (!patientFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            if (errorMessage) *errorMessage = patientFile.errorString();
            return false;
        }
        QDomDocument patientDocument;
        const bool parsed = static_cast<bool>(patientDocument.setContent(&patientFile));
        patientFile.close();
        if (!parsed || patientDocument.documentElement().tagName() != "patients") {
            if (errorMessage) *errorMessage = QString::fromUtf8("无法读取 patients.xml");
            return false;
        }

        const QDomElement root = patientDocument.documentElement();
        legacy = root.attribute("version") != "2";
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
    if (!loadMeasurementsFile(measurementsPath, &existingMeasurements, errorMessage)) return false;

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
        root.appendChild(element);
    }
    return writeDocument(measurementsPath, document, errorMessage);
}
