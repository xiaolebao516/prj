#pragma once
#include <QString>

// Archive records as stored in patients.xml / measurements.xml (PatientStore).

struct PatientInfo {
    QString id;
    QString name;
    QString gender;
    QString birthDay;
    QString checkDate;
    QString height;
    QString weight;
    QString diagprompt;
    QString speedOfSound;
};

struct MeasurementRecord {
    QString id;
    QString patientId;
    QString measuredAt;
    QString operatorName;
    QString part;
    QString sos;
    QString tScore;
    QString zScore;
    QString diagnosis;
    QString patientName;
    QString patientGender;
    QString patientBirthDay;
    QString patientHeight;
    QString patientWeight;
    QString patientAge;
    QString boneStrength;
    QString fractureRisk;
    QString boneAge;
    // ParameterGroup id the result was measured with; empty for older records.
    QString parameterGroup;
};
