#pragma once
#include <QString>
#include <QDate>

namespace BoneHealth {

int calcPatientAge(const QDate& birthDay);

double calcAgeReferenceMean(int age);

QString classifyBoneStrength(double tScore);

double calcRelativeFractureRisk(double tScore);

int estimateBoneAgeFromSos(double sos, const QString& gender);

} // namespace BoneHealth
