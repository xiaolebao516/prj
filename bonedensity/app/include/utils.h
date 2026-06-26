#pragma once
#include "types.h"
#include <QVector>
#include <QDebug>

namespace Utils {

double meanValue(const QVector<double>& values);
double medianValue(QVector<double> values);
double trimmedMeanValue(const QVector<double>& values, double trimRatio = 0.2);
double safeRatio(double numerator, double denominator);

QVector<int> findBestRoundCluster(const QVector<RoundCandidate>& candidates,
                                   double tolerance,
                                   const QVector<double>& acceptedSosList);

void rebuildAcceptedRoundsFromCandidates(
    const QVector<RoundCandidate>& candidateRoundList,
    QVector<double>& roundSosList,
    QVector<double>& roundAList,
    QVector<double>& roundBList,
    double roundClusterTolerance);

} // namespace Utils
