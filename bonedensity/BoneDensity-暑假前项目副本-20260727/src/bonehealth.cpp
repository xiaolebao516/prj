#include "bonehealth.h"
#include <QVector>
#include <QPair>
#include <cmath>

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

double calcAgeReferenceMean(int age)
{
    // ======================================================
    // 临时参考曲线，只用于把流程跑通。
    // 正式版本必须换成你们设备、桡骨部位、目标人群的参考数据库。
    // ======================================================

    QVector<QPair<int, double>> table;
    table << qMakePair(20, 4010.0)
          << qMakePair(30, 3985.0)
          << qMakePair(40, 3950.0)
          << qMakePair(50, 3905.0)
          << qMakePair(60, 3845.0)
          << qMakePair(70, 3785.0)
          << qMakePair(80, 3725.0)
          << qMakePair(90, 3670.0)
          << qMakePair(100, 3620.0);

    if (age <= table.first().first) {
        return table.first().second;
    }

    if (age >= table.last().first) {
        return table.last().second;
    }

    for (int i = 0; i < table.size() - 1; ++i) {
        int a0 = table[i].first;
        int a1 = table[i + 1].first;

        double v0 = table[i].second;
        double v1 = table[i + 1].second;

        if (age >= a0 && age <= a1) {
            double k = double(age - a0) / double(a1 - a0);
            return v0 + k * (v1 - v0);
        }
    }

    return table.last().second;
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
    int bestAge = 20;
    double bestDiff = 1e9;

    for (int age = 20; age <= 100; ++age) {
        double ref = calcAgeReferenceMean(age);

        if (gender.contains("男")) {
            ref += 20.0;
        }

        double diff = std::abs(sos - ref);

        if (diff < bestDiff) {
            bestDiff = diff;
            bestAge = age;
        }
    }

    return bestAge;
}

} // namespace BoneHealth
