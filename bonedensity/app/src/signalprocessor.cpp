#include "signalprocessor.h"
#include <cmath>
#include <algorithm>
#include <QDebug>

// 逐帧调试开关：与 mainwindow.cpp 中的 kDebugPerFrame 保持一致
static constexpr bool kDebugPerFrame = false;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ==================== Free function: besselI0 ====================

double besselI0(double x) {
    double sum = 1.0;
    double term = 1.0;
    double x_squared_over_4 = x * x / 4.0;

    // 级数展开，通常迭代20-25次精度就足够了
    for (int k = 1; k < 50; k++) {
        term *= x_squared_over_4 / (k * k);
        sum += term;
        if (term < 1e-9 * sum) break; // 精度足够则退出
    }
    return sum;
}

// ==================== FIR Design ====================

void SignalProcessor::designFIR(double centerFreq, double bandwidth, double sampleRate)
{
    firCoeffs.clear();

    firOrder = 80;

    int N = firOrder + 1;
    firCoeffs.resize(N);

    double centerIdx = firOrder / 2.0;
    double fl = (bandwidth / 2.0) / sampleRate;
    double w_c = 2.0 * M_PI * centerFreq / sampleRate;

    double beta = 5.0;
    double denom = besselI0(beta);

    for (int n = 0; n < N; ++n) {
        double x = n - centerIdx;

        double val;
        if (std::abs(x) < 1e-10) {
            val = 2.0 * fl;
        } else {
            val = sin(2.0 * M_PI * fl * x) / (M_PI * x);
        }

        double fraction = x / centerIdx;
        double inside = 1.0 - fraction * fraction;
        if (inside < 0.0) inside = 0.0;

        double window = besselI0(beta * sqrt(inside)) / denom;

        val *= window;
        val *= 2.0 * cos(w_c * x);

        firCoeffs[n] = val;
    }

    // 关键新增：强制 FIR 系数直流增益为 0
    double dc = 0.0;
    for (double c : firCoeffs) {
        dc += c;
    }
    dc /= firCoeffs.size();

    for (double &c : firCoeffs) {
        c -= dc;
    }

    // 重新计算 1.25 MHz 处的增益并归一化
    double gainReal = 0.0;
    double gainImag = 0.0;

    for (int n = 0; n < N; ++n) {
        gainReal += firCoeffs[n] * cos(-w_c * n);
        gainImag += firCoeffs[n] * sin(-w_c * n);
    }

    double magnitude = sqrt(gainReal * gainReal + gainImag * gainImag);

    if (magnitude > 1e-9) {
        double scale = 1.0 / magnitude;

        for (double &c : firCoeffs) {
            c *= scale;
        }
    }

    qDebug() << "FIR Designed:"
             << "centerFreq =" << centerFreq
             << "bandwidth =" << bandwidth
             << "Beta =" << beta
             << "Order =" << firOrder
             << "Taps =" << N;
}

// ==================== FIR Filtering ====================

QVector<double> SignalProcessor::applyFIR(const QVector<quint16>& input) const {
    if (input.isEmpty()) return QVector<double>();

    int nTaps = firCoeffs.size();
    int nSamples = input.size();
    QVector<double> output(nSamples);

    // 卷积
    for (int i = 0; i < nSamples; ++i) {
        double acc = 0.0;
        for (int j = 0; j < nTaps; ++j) {
            if (i - j >= 0) {
                // 将 quint16 转为 double 计算，减去直流分量(2048)效果更好，
                // 但带通滤波器本身就会阻隔直流，所以直接乘也可以。
                acc += (double)input[i - j] * firCoeffs[j];
            }
        }
        output[i] = acc;
    }

    // 注意：滤波后的波形会有 (nTaps-1)/2 个点的相位延迟 (群延时)
    // 也就是波形会整体向右平移。在计算 TOF 时需要减去这个固定值。
    return output;
}

QVector<double> SignalProcessor::applyFIRDouble(const QVector<double>& input) const
{
    if (input.isEmpty()) {
        return QVector<double>();
    }

    int nTaps = static_cast<int>(firCoeffs.size());
    int nSamples = input.size();

    QVector<double> output(nSamples, 0.0);

    for (int i = 0; i < nSamples; ++i) {
        double acc = 0.0;

        for (int j = 0; j < nTaps; ++j) {
            int k = i - j;

            if (k >= 0) {
                acc += input[k] * firCoeffs[j];
            }
        }

        output[i] = acc;
    }

    return output;
}

// ==================== Static: Pure Signal Processing ====================

QVector<double> SignalProcessor::preprocessRawForFIR(const QVector<quint16>& input,
                                                      const GateConfig& cfg,
                                                      const QString& name)
{
    QVector<double> out;

    if (input.isEmpty()) {
        return out;
    }

    int n = input.size();
    out.resize(n);

    int b0 = qMax(0, cfg.baselineStart);
    int b1 = qMin(n - 1, cfg.baselineEnd);

    if (b0 > b1) {
        b0 = 0;
        b1 = qMin(n - 1, 20);
    }

    // 用中位数估计基线，比均值更抗偶发尖峰
    QVector<double> baseVals;
    baseVals.reserve(b1 - b0 + 1);

    for (int i = b0; i <= b1; ++i) {
        baseVals.append(static_cast<double>(input[i]));
    }

    std::sort(baseVals.begin(), baseVals.end());
    double baseline = baseVals[baseVals.size() / 2];

    int eraseStart = qBound(0, cfg.eraseStart, n - 1);
    int eraseEnd   = qBound(0, cfg.eraseEnd,   n - 1);
    int rampEnd    = qBound(0, cfg.rampEnd,    n - 1);

    if (eraseEnd < eraseStart) {
        eraseEnd = eraseStart;
    }
    if (rampEnd < eraseEnd) {
        rampEnd = eraseEnd;
    }

    for (int i = 0; i < n; ++i) {
        double x = static_cast<double>(input[i]) - baseline;

        if (i < eraseStart) {
            // 假波之前，保留原始基线附近波动
            out[i] = x;
        } else if (i <= eraseEnd) {
            // 假波主区域：直接抹成 0，也就是抹成基线
            out[i] = 0.0;
        } else if (i <= rampEnd) {
            // 平滑恢复，避免硬切造成新的高频毛刺
            double t = double(i - eraseEnd) / double(qMax(1, rampEnd - eraseEnd));

            // smoothstep: 0 -> 1，且两端斜率为 0
            double w = t * t * (3.0 - 2.0 * t);

            out[i] = w * x;
        } else {
            // 后面全部保留，包括第一波前面的真实噪声参考段
            out[i] = x;
        }
    }

    if (kDebugPerFrame) {
        qDebug() << "Preprocess" << name
                 << "baseline =" << baseline
                 << "erase = [" << eraseStart << "," << eraseEnd << "]"
                 << "rampEnd =" << rampEnd;
    }

    return out;
}

QVector<double> SignalProcessor::buildEnvelopeFromFiltered(const QVector<double>& x, int winLen)
{
    if (x.isEmpty()) return {};

    int n = x.size();
    QVector<double> env(n, 0.0);
    QVector<double> absx(n, 0.0);

    for (int i = 0; i < n; ++i) {
        absx[i] = std::abs(x[i]);
    }

    double sum = 0.0;
    int w = qMax(1, winLen);

    for (int i = 0; i < n; ++i) {
        sum += absx[i];
        if (i >= w) sum -= absx[i - w];
        int denom = qMin(i + 1, w);
        env[i] = sum / denom;
    }

    return env;
}

ArrivalResult SignalProcessor::detectFirstArrivalSmart(
    const QVector<double>& fil,
    int noiseStart,
    int noiseEnd,
    int searchStart,
    int searchEnd,
    double kSigma,
    int runLen,
    int envWin,
    double onsetRatio,
    int peakLookAhead)
{
    ArrivalResult res;

    if (fil.isEmpty()) {
        return res;
    }

    int n = fil.size();

    noiseStart = qBound(0, noiseStart, n - 1);
    noiseEnd   = qBound(0, noiseEnd,   n - 1);
    searchStart = qBound(0, searchStart, n - 1);
    searchEnd   = qBound(0, searchEnd,   n - 1);

    if (noiseStart >= noiseEnd || searchStart >= searchEnd) {
        return res;
    }

    QVector<double> env = buildEnvelopeFromFiltered(fil, envWin);

    double mean = 0.0;
    int cnt = noiseEnd - noiseStart + 1;

    for (int i = noiseStart; i <= noiseEnd; ++i) {
        mean += env[i];
    }

    mean /= cnt;

    double var = 0.0;

    for (int i = noiseStart; i <= noiseEnd; ++i) {
        double d = env[i] - mean;
        var += d * d;
    }

    var /= cnt;

    double sigma = std::sqrt(var);
    double threshold = mean + kSigma * sigma;

    res.noiseMean = mean;
    res.noiseStd = sigma;
    res.threshold = threshold;

    // 第一步：找连续超阈值段
    int firstHit = -1;
    int overCount = 0;

    for (int i = searchStart; i <= searchEnd; ++i) {
        if (env[i] > threshold) {
            overCount++;

            if (overCount >= runLen) {
                firstHit = i - runLen + 1;
                break;
            }
        } else {
            overCount = 0;
        }
    }

    if (firstHit < 0) {
        return res;
    }

    // 第二步：在 firstHit 后面找第一个包络峰
    int peakSearchEnd = qMin(searchEnd, firstHit + peakLookAhead);

    int peakIdx = firstHit;
    double peakVal = env[firstHit];

    for (int i = firstHit; i <= peakSearchEnd; ++i) {
        if (env[i] > peakVal) {
            peakVal = env[i];
            peakIdx = i;
        }
    }

    // 第三步：从峰值往前回溯到 20%~25% 峰值位置
    double level = mean + onsetRatio * (peakVal - mean);

    int onset = firstHit;

    for (int i = peakIdx; i >= searchStart; --i) {
        if (env[i] <= level) {
            onset = i;
            break;
        }
    }

    // 防止回溯过头，至少不能早于 firstHit 前太多
    int maxBack = 40;
    if (onset < firstHit - maxBack) {
        onset = firstHit - maxBack;
    }

    res.valid = true;
    res.onset = onset;
    res.peak = peakIdx;
    res.peakEnv = peakVal;

    return res;
}

// ==================== Static: Cross-Correlation ====================

int SignalProcessor::refineLagByPositiveCrossCorrelation(
    const QVector<double>& early,
    const QVector<double>& late,
    int earlyOnset,
    int roughLag,
    int lagMin,
    int lagMax,
    int prePts,
    int postPts,
    double* bestCorrOut)
{
    int n = qMin(early.size(), late.size());

    if (n <= 0) {
        if (bestCorrOut) *bestCorrOut = 0.0;
        return roughLag;
    }

    int startEarly = earlyOnset - prePts;
    int endEarly   = earlyOnset + postPts;

    startEarly = qMax(0, startEarly);
    endEarly   = qMin(n - 1, endEarly);

    if (endEarly <= startEarly + 10) {
        if (bestCorrOut) *bestCorrOut = 0.0;
        return roughLag;
    }

    bool roughOk = (roughLag >= lagMin && roughLag <= lagMax);

    int searchLagMin;
    int searchLagMax;

    // ======================================================
    // 关键修正：
    // 如果 roughLag 只是比 lagMin 小一点，不要直接扫完整物理范围。
    // 这次桡骨数据里 roughLag 经常是 88~91，而 lagMin 是 92。
    // 它其实已经找到了正确第一波区域，只是略早。
    // ======================================================
    int roughBelowMargin = 35;   // 允许 roughLag 比 lagMin 小 35 点以内
    int roughMinTrust = 60;      // roughLag 太小才认为完全不可信

    if (roughOk) {
        int localRange = qBound(30, int(0.25 * std::abs(roughLag)), 55);

        searchLagMin = qMax(lagMin, roughLag - localRange);
        searchLagMax = qMin(lagMax, roughLag + localRange);

    } else if (roughLag < lagMin &&
               roughLag >= lagMin - roughBelowMargin &&
               roughLag >= roughMinTrust) {

        // 这种情况是：粗定位略早，但仍然可信。
        // 不扫完整范围，只允许它往后修正一段。
        //
        // 例如 roughLag=90，lagMin=92，
        // 正确桡骨 lag 可能在 120~130，
        // 但错误后波包在 200 左右。
        //
        // 所以这里限制到 roughLag + 55，避免跳到 200。
        searchLagMin = lagMin;
        searchLagMax = qMin(lagMax, roughLag + 55);

        qDebug() << "XCorr range adjusted: roughLag slightly below lagMin"
                 << "roughLag =" << roughLag
                 << "lagMin =" << lagMin
                 << "searchRange = [" << searchLagMin << "," << searchLagMax << "]";

    } else {
        // 真正完全不可信的粗定位，才扫完整物理范围。
        searchLagMin = lagMin;
        searchLagMax = lagMax;
    }

    auto calcCorr = [&](int lag) -> double {
        double meanX = 0.0;
        double meanY = 0.0;
        int cnt = 0;

        for (int i = startEarly; i <= endEarly; ++i) {
            int j = i + lag;
            if (j < 0 || j >= n) continue;

            meanX += early[i];
            meanY += late[j];
            cnt++;
        }

        if (cnt < 10) return -2.0;

        meanX /= cnt;
        meanY /= cnt;

        double num = 0.0;
        double denX = 0.0;
        double denY = 0.0;

        for (int i = startEarly; i <= endEarly; ++i) {
            int j = i + lag;
            if (j < 0 || j >= n) continue;

            double x = early[i] - meanX;
            double y = late[j] - meanY;

            num  += x * y;
            denX += x * x;
            denY += y * y;
        }

        if (denX < 1e-12 || denY < 1e-12) return -2.0;

        return num / std::sqrt(denX * denY);
    };

    // ======================================================
    // 第一遍：找最大正相关点 maxLag
    // ======================================================
    double maxCorr = -1.0;
    int maxLag = roughLag;

    for (int lag = searchLagMin; lag <= searchLagMax; ++lag) {
        double corr = calcCorr(lag);

        // 只允许正相关，不用 abs(corr)
        if (corr <= 0) continue;

        if (corr > maxCorr) {
            maxCorr = corr;
            maxLag = lag;
        }
    }

    if (maxCorr < 0) {
        if (bestCorrOut) *bestCorrOut = 0.0;
        return roughLag;
    }

    // ======================================================
    // 第二遍：在"高相关平台"里连续选点
    // ======================================================
    //
    // 以前是：
    // lag < 160 选平台最前面；
    // lag >= 160 选平台最后面。
    //
    // 现在改成连续过渡：
    // 铜块 lag≈105~110：选平台偏前
    // 中间速度 lag≈140~170：选平台中间
    // 塑料 lag≈200：选平台偏后
    //

    double fastLag = 106.0;   // 铜块附近，产品速度上限附近
    double slowLag = 205.0;   // 塑料附近，低速材料附近

    double alpha = (double(maxLag) - fastLag) / (slowLag - fastLag);
    alpha = qBound(0.0, alpha, 1.0);

    // alpha = 0：选平台最前面，偏向铜块/高声速
    // alpha = 1：选平台最后面，偏向塑料/低声速

    // 铜块这次极端偏低，说明 0.95 门槛偏严格，
    // 早一点的 lag 虽然相关略低，但更接近真实高声速。
    //
    // 所以快材料用 0.92，慢材料用 0.95。
    double fastRatio = 0.92;
    double slowRatio = 0.95;

    double ratio = fastRatio + alpha * (slowRatio - fastRatio);

    QVector<int> plateauLags;
    QVector<double> plateauCorrs;

    for (int lag = searchLagMin; lag <= searchLagMax; ++lag) {
        double corr = calcCorr(lag);
        if (corr <= 0) continue;

        if (corr >= ratio * maxCorr) {
            plateauLags.append(lag);
            plateauCorrs.append(corr);
        }
    }

    int bestLag = maxLag;
    double bestCorr = maxCorr;

    if (!plateauLags.isEmpty()) {
        int idx = int(std::round(alpha * (plateauLags.size() - 1)));
        idx = qBound(0, idx, plateauLags.size() - 1);

        bestLag = plateauLags[idx];
        bestCorr = plateauCorrs[idx];
    }

    if (kDebugPerFrame) {
        qDebug() << "XCorrPick:"
                 << "roughLag =" << roughLag
                 << "roughOk =" << roughOk
                 << "searchRange = [" << searchLagMin << "," << searchLagMax << "]"
                 << "maxLag =" << maxLag
                 << "maxCorr =" << maxCorr
                 << "alpha =" << alpha
                 << "ratio =" << ratio
                 << "plateauSize =" << plateauLags.size()
                 << "pickedLag =" << bestLag
                 << "pickedCorr =" << bestCorr;
    }

    if (bestCorrOut) {
        *bestCorrOut = bestCorr;
    }

    return bestLag;
}

// ==================== Static: Valley Detection ====================

ValleyResult SignalProcessor::findFirstProminentValley(
    const QVector<double>& x,
    int start,
    int end,
    double minDepth,
    double ratioToMax,
    int minSep)
{
    ValleyResult res;

    if (x.isEmpty()) return res;

    int n = x.size();

    start = qBound(1, start, n - 2);
    end   = qBound(1, end,   n - 2);

    if (start >= end) return res;

    struct Cand {
        int idx;
        double value;
        double depth;
    };

    QVector<Cand> raw;

    for (int i = start; i <= end; ++i) {
        // 局部极小值
        if (x[i] < x[i - 1] && x[i] <= x[i + 1]) {
            double depth = -x[i];   // 负波谷越深，depth 越大

            if (depth >= minDepth) {
                raw.push_back({i, x[i], depth});
            }
        }
    }

    if (raw.isEmpty()) return res;

    // 合并距离太近的波谷，只保留每一簇中最深的那个
    QVector<Cand> merged;

    int pos = 0;
    while (pos < raw.size()) {
        Cand best = raw[pos];
        int j = pos + 1;

        while (j < raw.size() && raw[j].idx - raw[j - 1].idx <= minSep) {
            if (raw[j].depth > best.depth) {
                best = raw[j];
            }
            j++;
        }

        merged.push_back(best);
        pos = j;
    }

    if (merged.isEmpty()) return res;

    double maxDepth = 0.0;
    for (const Cand& c : merged) {
        if (c.depth > maxDepth) {
            maxDepth = c.depth;
        }
    }

    double requiredDepth = qMax(minDepth, ratioToMax * maxDepth);

    // 选第一个足够明显的波谷，而不是最深波谷
    for (const Cand& c : merged) {
        if (c.depth >= requiredDepth) {
            res.valid = true;
            res.idx = c.idx;
            res.value = c.value;
            res.depth = c.depth;
            return res;
        }
    }

    return res;
}

// ==================== Instance: Speed Estimation ====================

PairResult SignalProcessor::estimatePairSpeed(
    const QVector<double>& early,
    const QVector<double>& late,
    const ArrivalResult& earlyPick,
    const ArrivalResult& latePick,
    double vMin,
    double vMax,
    const QString& pairName,
    int forcedLagMin,
    int forcedLagMax) const
{
    PairResult res;

    if (!earlyPick.valid || !latePick.valid) {
        qDebug() << pairName << "invalid: arrival pick failed";
        return res;
    }

    int lagMin = static_cast<int>(std::floor(probeDistanceCD / (vMax * samplePeriod))) - 5;
    int lagMax = static_cast<int>(std::ceil (probeDistanceCD / (vMin * samplePeriod))) + 5;

    lagMin = qMax(1, lagMin);
    lagMax = qMax(lagMin + 1, lagMax);

    if (forcedLagMin > 0 && forcedLagMax > forcedLagMin) {
        int oldMin = lagMin;
        int oldMax = lagMax;

        lagMin = qMax(lagMin, forcedLagMin);
        lagMax = qMin(lagMax, forcedLagMax);

        qDebug() << pairName
                 << "forced lag range:"
                 << "[" << forcedLagMin << "," << forcedLagMax << "]"
                 << "physical range:"
                 << "[" << oldMin << "," << oldMax << "]"
                 << "final range:"
                 << "[" << lagMin << "," << lagMax << "]";
    }

    if (lagMin >= lagMax) {
        qDebug() << pairName << "invalid: empty lag range";
        return res;
    }

    int roughLag = latePick.onset - earlyPick.onset;

    double bestCorr = 0.0;

    int refinedLag = refineLagByPositiveCrossCorrelation(
        early,
        late,
        earlyPick.onset,
        roughLag,
        lagMin,
        lagMax,
        20,
        120,
        &bestCorr
        );

    int absLag = std::abs(refinedLag);

    if (absLag < lagMin || absLag > lagMax) {
        qDebug() << pairName
                 << "invalid: lag out of range"
                 << "roughLag =" << roughLag
                 << "refinedLag =" << refinedLag
                 << "range = [" << lagMin << "," << lagMax << "]";
        return res;
    }

    if (bestCorr < 0.25) {
        qDebug() << pairName
                 << "invalid: corr too low"
                 << "corr =" << bestCorr;
        return res;
    }

    double sos = probeDistanceCD / (absLag * samplePeriod);

    res.valid = true;
    res.earlyOnset = earlyPick.onset;
    res.lateOnset = latePick.onset;
    res.roughLag = roughLag;
    res.refinedLag = refinedLag;
    res.corr = bestCorr;
    res.sos = sos;

    qDebug() << pairName
             << "earlyOnset =" << res.earlyOnset
             << "lateOnset =" << res.lateOnset
             << "roughLag =" << res.roughLag
             << "refinedLag =" << res.refinedLag
             << "corr =" << res.corr
             << "sos =" << res.sos
             << "lagRange = [" << lagMin << "," << lagMax << "]";

    return res;
}

PairResult SignalProcessor::estimatePairSpeedByValley(
    const QVector<double>& early,
    const QVector<double>& late,
    const ArrivalResult& earlyPick,
    int priorLag,
    int priorTolerance,
    const QString& pairName) const
{
    PairResult res;

    if (!earlyPick.valid) {
        qDebug() << pairName << "invalid: early pick failed";
        return res;
    }

    if (priorLag <= 0) {
        qDebug() << pairName << "invalid: priorLag invalid";
        return res;
    }

    // 1. 下面 A_pair 的早到通道 AD 不要完全相信阈值粗定位。
    // 铜块 AD 波谷大约在 635~700，塑料 AD 波谷大约在 1000~1070。
    // 所以先在 620~1300 的宽窗口里找第一个明显波谷。
    int earlySearchStart = 620;
    int earlySearchEnd   = qMin(1300, early.size() - 2);

    ValleyResult vEarly = findFirstProminentValley(
        early,
        earlySearchStart,
        earlySearchEnd,
        30.0,
        0.30,
        20
        );

    // 如果宽窗口找不到，再退回 earlyPick 附近
    if (!vEarly.valid && earlyPick.valid) {
        earlySearchStart = earlyPick.onset - 60;
        earlySearchEnd   = earlyPick.onset + 150;

        vEarly = findFirstProminentValley(
            early,
            earlySearchStart,
            earlySearchEnd,
            30.0,
            0.30,
            20
            );
    }

    if (!vEarly.valid) {
        qDebug() << pairName << "invalid: early valley not found"
                 << "earlySearch = [" << earlySearchStart << "," << earlySearchEnd << "]";
        return res;
    }

    // 2. 根据上面 B_pair 的 lag 预测晚到通道 AC 的波谷位置
    int predLate = vEarly.idx + priorLag;

    int lateSearchStart = predLate - priorTolerance;
    int lateSearchEnd   = predLate + priorTolerance;

    ValleyResult vLate = findFirstProminentValley(
        late,
        lateSearchStart,
        lateSearchEnd,
        25.0,    // AC 有时候弱一点，所以 minDepth 稍微低一点
        0.25,
        20
        );

    if (!vLate.valid) {
        qDebug() << pairName << "invalid: late valley not found"
                 << "predLate =" << predLate
                 << "lateSearch = [" << lateSearchStart << "," << lateSearchEnd << "]";
        return res;
    }

    int valleyLag = vLate.idx - vEarly.idx;

    // 3. 用波谷 lag 附近做小范围正相关精修
    int lagMin = qMax(1, valleyLag - 15);
    int lagMax = valleyLag + 15;

    double bestCorr = 0.0;

    int refinedLag = refineLagByPositiveCrossCorrelation(
        early,
        late,
        vEarly.idx,       // 用波谷位置作为窗口中心
        valleyLag,
        lagMin,
        lagMax,
        20,
        90,
        &bestCorr
        );

    int absLag = std::abs(refinedLag);

    double sos = probeDistanceCD / (absLag * samplePeriod);

    res.valid = true;
    res.earlyOnset = vEarly.idx;
    res.lateOnset = vLate.idx;
    res.roughLag = valleyLag;
    res.refinedLag = refinedLag;
    res.corr = bestCorr;
    res.sos = sos;

    qDebug() << pairName
             << "VALLEY earlyValley =" << vEarly.idx
             << "lateValley =" << vLate.idx
             << "valleyLag =" << valleyLag
             << "refinedLag =" << refinedLag
             << "corr =" << bestCorr
             << "sos =" << sos
             << "priorLag =" << priorLag
             << "priorTol =" << priorTolerance;

    return res;
}
