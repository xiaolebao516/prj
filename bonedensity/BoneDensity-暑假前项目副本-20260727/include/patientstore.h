#pragma once

#include <QList>
#include <QString>
#include "types.h"

class PatientStore {
public:
    bool load(const QString& patientsPath,
              const QString& measurementsPath,
              QList<PatientInfo>* patients,
              QList<MeasurementRecord>* measurements,
              QString* errorMessage = nullptr) const;

    bool savePatients(const QString& patientsPath,
                      const QList<PatientInfo>& patients,
                      QString* errorMessage = nullptr) const;
    bool saveMeasurements(const QString& measurementsPath,
                          const QList<MeasurementRecord>& measurements,
                          QString* errorMessage = nullptr) const;
    bool savePatientData(const QString& patientsPath,
                         const QString& measurementsPath,
                         const QList<PatientInfo>& patients,
                         const QList<MeasurementRecord>& measurements,
                         QString* errorMessage = nullptr) const;
};
