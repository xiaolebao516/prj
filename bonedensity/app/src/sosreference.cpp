#include "sosreference.h"

#include <limits>

namespace SosReference {
namespace {

struct Row {
    double midAge;
    Stat male;
    Stat female;
};

// 中国公共卫生 2015;31(4) 表1，单位 m/s。
const Row kRows[] = {
    {22.5, {3995.3, 118.6}, {4079.9, 111.1}},
    {27.5, {4047.1, 116.8}, {4126.6, 124.5}},
    {32.5, {4081.2, 117.3}, {4163.9, 122.4}},
    {37.5, {4089.6, 125.0}, {4169.6, 124.0}},
    {42.5, {4112.8, 131.3}, {4175.4, 132.4}},
    {47.5, {4105.9, 136.8}, {4176.9, 144.1}},
    {52.5, {4107.8, 150.6}, {4096.0, 166.6}},
    {57.5, {4097.5, 151.6}, {4020.4, 166.0}},
    {62.5, {4076.7, 153.6}, {3958.9, 165.3}},
    {67.5, {4049.0, 155.4}, {3932.2, 156.6}},
    {72.5, {4029.5, 150.1}, {3904.8, 152.0}},
    {77.5, {4030.2, 163.0}, {3884.7, 161.3}},
};
constexpr int kRowCount = int(sizeof(kRows) / sizeof(kRows[0]));

Stat statOf(const Row& row, Sex sex)
{
    return sex == Sex::Male ? row.male : row.female;
}

constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();

const Row& peakRow(Sex sex)
{
    const Row* peak = &kRows[0];
    for (const Row& row : kRows) {
        if (statOf(row, sex).mean > statOf(*peak, sex).mean) peak = &row;
    }
    return *peak;
}

} // namespace

Sex sexFromGender(const QString& gender)
{
    if (gender.contains(QStringLiteral("女"))) return Sex::Female;
    if (gender.contains(QStringLiteral("男"))) return Sex::Male;
    return Sex::Unknown;
}

Stat youngAdult(Sex sex)
{
    if (sex == Sex::Unknown) return {};
    return statOf(peakRow(sex), sex);
}

double peakAge(Sex sex)
{
    return sex == Sex::Unknown ? kNaN : peakRow(sex).midAge;
}

Stat atAge(Sex sex, double age)
{
    if (sex == Sex::Unknown) return {};
    if (age <= kRows[0].midAge) return statOf(kRows[0], sex);
    if (age >= kRows[kRowCount - 1].midAge) return statOf(kRows[kRowCount - 1], sex);
    for (int i = 0; i + 1 < kRowCount; ++i) {
        const Row& lo = kRows[i];
        const Row& hi = kRows[i + 1];
        if (age > hi.midAge) continue;
        const double k = (age - lo.midAge) / (hi.midAge - lo.midAge);
        const Stat a = statOf(lo, sex);
        const Stat b = statOf(hi, sex);
        return {a.mean + k * (b.mean - a.mean), a.sd + k * (b.sd - a.sd)};
    }
    return statOf(kRows[kRowCount - 1], sex);
}

bool coversAge(int age)
{
    return age >= kAdultMinAge;
}

double tScore(double sos, Sex sex)
{
    const Stat ref = youngAdult(sex);
    return ref.isValid() ? (sos - ref.mean) / ref.sd : kNaN;
}

double zScore(double sos, Sex sex, int ageInYears)
{
    if (!coversAge(ageInYears)) return kNaN;
    // Someone aged N is on average N + 0.5 years old, which lines up with the
    // 5-year group midpoints (a 22-year-old sits at the 20~24 midpoint 22.5).
    const Stat ref = atAge(sex, ageInYears + 0.5);
    return ref.isValid() ? (sos - ref.mean) / ref.sd : kNaN;
}

double lastGroupMidAge()
{
    return kRows[kRowCount - 1].midAge;
}

QString sourceLabel()
{
    return QStringLiteral("参考：中国成人桡骨 SOS（中国公共卫生 2015）");
}

} // namespace SosReference
