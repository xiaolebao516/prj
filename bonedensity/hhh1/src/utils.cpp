#include "utils.h"
#include <algorithm>
#include <cmath>

namespace Utils {

double meanValue(const QVector<double>& values)
{
    if (values.isEmpty()) {
        return 0.0;
    }

    double sum = 0.0;

    for (double v : values) {
        sum += v;
    }

    return sum / values.size();
}

double medianValue(QVector<double> values)
{
    if (values.isEmpty()) {
        return 0.0;
    }

    std::sort(values.begin(), values.end());

    int n = values.size();

    if (n % 2 == 1) {
        return values[n / 2];
    }

    return 0.5 * (values[n / 2 - 1] + values[n / 2]);
}

QVector<int> findBestRoundCluster(const QVector<RoundCandidate>& candidates,
                                   double tolerance,
                                   const QVector<double>& acceptedSosList)
{
    Q_UNUSED(acceptedSosList)
    QVector<int> bestIdx;

    if (candidates.isEmpty()) {
        return bestIdx;
    }

    for (int i = 0; i < candidates.size(); ++i) {
        QVector<int> idx;

        double center = candidates[i].sos;

        for (int j = 0; j < candidates.size(); ++j) {
            if (std::abs(candidates[j].sos - center) <= tolerance) {
                idx.append(j);
            }
        }

        if (idx.size() > bestIdx.size()) {
            bestIdx = idx;
        } else if (idx.size() == bestIdx.size() && !idx.isEmpty() && !bestIdx.isEmpty()) {
            // 如果数量一样，选内部离散程度更小的簇
            QVector<double> a, b;

            for (int id : idx) {
                a.append(candidates[id].sos);
            }

            for (int id : bestIdx) {
                b.append(candidates[id].sos);
            }

            double medA = medianValue(a);
            double medB = medianValue(b);

            double spreadA = 0.0;
            double spreadB = 0.0;

            for (double v : a) {
                spreadA += std::abs(v - medA);
            }

            for (double v : b) {
                spreadB += std::abs(v - medB);
            }

            if (spreadA < spreadB) {
                bestIdx = idx;
            }
        }
    }

    return bestIdx;
}

void rebuildAcceptedRoundsFromCandidates(
    const QVector<RoundCandidate>& candidateRoundList,
    QVector<double>& roundSosList,
    QVector<double>& roundAList,
    QVector<double>& roundBList,
    double roundClusterTolerance)
{
    QVector<int> best = findBestRoundCluster(candidateRoundList, roundClusterTolerance, roundSosList);

    roundSosList.clear();
    roundAList.clear();
    roundBList.clear();

    for (int idx : best) {
        roundSosList.append(candidateRoundList[idx].sos);
        roundAList.append(candidateRoundList[idx].a);
        roundBList.append(candidateRoundList[idx].b);
    }

    QVector<double> candSosList;
    QVector<double> candCorrAList;
    QVector<double> candCorrBList;

    for (const RoundCandidate& c : candidateRoundList) {
        candSosList.append(c.sos);
        candCorrAList.append(c.corrA);
        candCorrBList.append(c.corrB);
    }

    qDebug() << "Round cluster rebuilt:"
             << "candidateCount =" << candidateRoundList.size()
             << "candidateSos =" << candSosList
             << "candidateCorrA =" << candCorrAList
             << "candidateCorrB =" << candCorrBList
             << "acceptedClusterCount =" << roundSosList.size()
             << "roundSosList =" << roundSosList;
}

double trimmedMeanValue(const QVector<double>& values, double trimRatio)
{
    if (values.isEmpty()) {
        return 0.0;
    }

    QVector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());

    int n = sorted.size();

    int cut = int(n * trimRatio);

    // 数据太少时，不截尾，直接均值
    if (n < 10 || cut * 2 >= n) {
        return meanValue(values);
    }

    double sum = 0.0;
    int cnt = 0;

    for (int i = cut; i < n - cut; ++i) {
        sum += sorted[i];
        cnt++;
    }

    if (cnt <= 0) {
        return meanValue(values);
    }

    return sum / cnt;
}

double safeRatio(double numerator, double denominator)
{
    if (std::abs(denominator) < 1e-9) {
        return 0.0;
    }

    return numerator / denominator;
}

} // namespace Utils
