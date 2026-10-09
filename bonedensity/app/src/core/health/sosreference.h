#pragma once

#include <QString>

// Adult distal-radius SOS reference used for T/Z scores and the age-SOS chart.
// One table feeds both, so the chart's T axis and the reported scores agree.
//
// Source: 中国成年居民超声骨速及骨质疏松症分析. 中国公共卫生, 2015, 31(4).
// Table 1: 14 229 urban adults, distal 1/3 radius, Sunlight ultrasound system,
// mean ± SD by sex and 5-year age group (20~ … 75~).
namespace SosReference {

enum class Sex { Unknown, Female, Male };

struct Stat {
    double mean = 0.0;
    double sd = 0.0;
    bool isValid() const { return sd > 0.0; }
};

constexpr int kAdultMinAge = 20;

Sex sexFromGender(const QString& gender);

// T-score reference: the peak age group of that sex (female 45~, male 40~).
Stat youngAdult(Sex sex);
// Midpoint age of that peak group (female 47.5, male 42.5).
double peakAge(Sex sex);

// Z-score reference at an exact age: linear between 5-year group midpoints,
// held flat below the first and above the last group (75~ covers 75 and older).
Stat atAge(Sex sex, double age);

// The table covers adults only: under 20 there is no T-score (peak bone mass
// not yet reached) and no Chinese pediatric radius reference to give a Z-score.
bool coversAge(int age);

// NaN when the sex is unknown or the age is outside the table.
double tScore(double sos, Sex sex);
double zScore(double sos, Sex sex, int ageInYears);

// Midpoint of the oldest group; beyond it the reference is held flat.
double lastGroupMidAge();

QString sourceLabel();

} // namespace SosReference
