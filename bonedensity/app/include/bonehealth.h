#pragma once
#include <QString>
#include <QDate>

namespace BoneHealth {

int calcPatientAge(const QDate& birthDay);

QString classifyBoneStrength(double tScore);

double calcRelativeFractureRisk(double tScore);

int estimateBoneAgeFromSos(double sos, const QString& gender);

// Everything shown next to a measured SOS, as stored/displayed text. Empty
// fields mean "not applicable" (shown as --).
struct DerivedResult {
    QString tScore;
    QString zScore;
    QString strength;      // 正常 / 不足 / 严重不足 / 不评定
    QString diagnosis;
    QString fractureRisk;
    QString boneAge;
};

// T/Z need an adult (SosReference::coversAge) with a known sex; ageInYears < 0
// means the birth date is unknown. Used for new results and to show saved
// records with the current reference.
DerivedResult deriveResult(double sos, const QString& gender, int ageInYears);

} // namespace BoneHealth
