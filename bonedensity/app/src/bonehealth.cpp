#include "bonehealth.h"
#include "sosreference.h"
#include <QtGlobal>
#include <cmath>
#include <limits>

namespace BoneHealth {

int calcPatientAge(const QDate& birthDay)
{
    QDate birth = birthDay;

    if (!birth.isValid()) {
        return 0;
    }

    QDate today = QDate::currentDate();

    int age = today.year() - birth.year();

    if (today.month() < birth.month() ||
        (today.month() == birth.month() && today.day() < birth.day())) {
        age--;
    }

    return qMax(0, age);
}

QString classifyBoneStrength(double tScore)
{
    if (tScore <= -2.5) {
        return "严重不足";
    } else if (tScore < -1.0) {
        return "不足";
    } else {
        return "正常";
    }
}

double calcRelativeFractureRisk(double tScore)
{
    if (tScore >= 0.0) {
        return 1.0;
    }

    // 临时估计：T值每降低1个标准差，风险按约1.5倍增加
    return std::pow(1.5, -tScore);
}

int estimateBoneAgeFromSos(double sos, const QString& gender)
{
    // Same rule as before: the age whose reference mean SOS is closest to the
    // measured SOS. It now reads the age-SOS chart's own mean curve
    // (SosReference at age + 0.5, where the chart plots that age), so the bone
    // age is a point on the curve shown to the user. Only the part from the
    // peak onward is searched: there the curve falls with age, so every SOS has
    // one bone age and a lower SOS never gives a younger one. (The male table
    // rises by under 2 m/s at 50~ and 75~; those bumps are read as flat.)
    const SosReference::Sex sex = SosReference::sexFromGender(gender);
    if (sex == SosReference::Sex::Unknown || !std::isfinite(sos)) return -1;

    const int firstAge = qMax(SosReference::kAdultMinAge,
                              int(std::ceil(SosReference::peakAge(sex) - 0.5)));
    int bestAge = firstAge;
    double bestDiff = std::numeric_limits<double>::infinity();
    double falling = std::numeric_limits<double>::infinity();
    for (int age = firstAge; age <= 100; ++age) {
        falling = qMin(falling, SosReference::atAge(sex, age + 0.5).mean);
        const double diff = std::abs(sos - falling);
        if (diff < bestDiff) {
            bestDiff = diff;
            bestAge = age;
        }
    }
    return bestAge;
}

DerivedResult deriveResult(double sos, const QString& gender, int ageInYears)
{
    DerivedResult result;
    result.strength = QStringLiteral("不评定");
    const SosReference::Sex sex = SosReference::sexFromGender(gender);
    if (sex == SosReference::Sex::Unknown) {
        result.diagnosis = QStringLiteral("性别未填写：无法匹配参考数据，仅记录SOS");
        return result;
    }
    if (ageInYears < 0) {
        result.diagnosis = QStringLiteral("出生日期无效：无法匹配参考数据，仅记录SOS");
        return result;
    }
    if (!SosReference::coversAge(ageInYears)) {
        result.diagnosis = QStringLiteral("未满20岁：T值不适用，暂无儿童参考数据，仅记录SOS");
        return result;
    }

    const double tScore = SosReference::tScore(sos, sex);
    const double zScore = SosReference::zScore(sos, sex, ageInYears);
    result.tScore = QString::number(tScore, 'f', 2);
    result.zScore = QString::number(zScore, 'f', 2);
    result.strength = classifyBoneStrength(tScore);
    result.diagnosis = result.strength;
    result.fractureRisk = QString::number(calcRelativeFractureRisk(tScore), 'f', 1);
    const int boneAge = estimateBoneAgeFromSos(sos, gender);
    if (boneAge >= 0) result.boneAge = QString::number(boneAge);
    return result;
}

} // namespace BoneHealth
