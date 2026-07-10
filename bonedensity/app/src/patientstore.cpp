#include "patientstore.h"

#include <QDateTime>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QSaveFile>
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

} // namespace

bool PatientStore::load(const QString& patientsPath,
                        const QString& measurementsPath,
                        QList<PatientInfo>* patients,
                        QList<MeasurementRecord>* measurements,
                        QString* errorMessage) const
{
    patients->clear();
    measurements->clear();

    QFile patientFile(patientsPath);
    if (patientFile.exists()) {
        if (!patientFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            if (errorMessage) *errorMessage = patientFile.errorString();
            return false;
        }
        QDomDocument patientDocument;
        if (!patientDocument.setContent(&patientFile)) {
            if (errorMessage) *errorMessage = QString::fromUtf8("无法读取 patients.xml");
            return false;
        }
        const QDomElement root = patientDocument.documentElement();
        const bool legacy = root.attribute("version") != "2";
        QHash<QString, int> patientIndexes;
        const QDomNodeList nodes = root.elementsByTagName("patient");
        for (int i = 0; i < nodes.count(); ++i) {
            const QDomElement element = nodes.at(i).toElement();
            const PatientInfo patient = patientFromElement(element);
            if (patient.id.isEmpty()) continue;
            if (!patientIndexes.contains(patient.id)) {
                patientIndexes.insert(patient.id, patients->size());
                patients->append(patient);
            }
            if (legacy && (!element.attribute("sos").isEmpty() ||
                           !element.attribute("diag").isEmpty() ||
                           !element.attribute("checkDate").isEmpty())) {
                measurements->append(legacyRecordFromElement(element));
            }
        }

        if (legacy && !QFileInfo::exists(measurementsPath)) {
            const QString backupPath = patientsPath + ".bak";
            if (!QFileInfo::exists(backupPath) && !QFile::copy(patientsPath, backupPath)) {
                if (errorMessage) *errorMessage = QString::fromUtf8("旧 patients.xml 备份失败，已取消迁移");
                patients->clear();
                measurements->clear();
                return false;
            }
            QString saveError;
            if (!saveMeasurements(measurementsPath, *measurements, &saveError) ||
                !savePatients(patientsPath, *patients, &saveError)) {
                if (errorMessage) *errorMessage = QString::fromUtf8("旧数据迁移失败：") + saveError;
                patients->clear();
                measurements->clear();
                return false;
            }
        }
    }

    QFile measurementFile(measurementsPath);
    if (measurementFile.exists()) {
        if (!measurementFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            if (errorMessage) *errorMessage = measurementFile.errorString();
            return false;
        }
        QDomDocument measurementDocument;
        if (!measurementDocument.setContent(&measurementFile)) {
            if (errorMessage) *errorMessage = QString::fromUtf8("无法读取 measurements.xml");
            return false;
        }
        measurements->clear();
        const QDomNodeList nodes = measurementDocument.documentElement().elementsByTagName("measurement");
        for (int i = 0; i < nodes.count(); ++i) {
            const QDomElement element = nodes.at(i).toElement();
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
            if (!record.id.isEmpty() && !record.patientId.isEmpty()) measurements->append(record);
        }
    }
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
