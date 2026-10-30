// Replays a recorded experiment-log frame through the desktop core:
//   recorded samples -> protocol bytes (with noise, random chunking)
//   -> FrameAssembler -> FrameAnalyzer::filter -> FrameAnalyzer::analyze
// and prints the experiment-log evidence. Exit code 1 when a check fails.
//
// Usage: replay_check <round.jsonl> [--expect-low-1984] [--stream captured.bin]
//   --stream  feed bytes captured from fake_device.py (or a real device)
//             instead of the synthesized noisy stream

#include "device/deviceprotocol.h"
#include "measurement/frameanalyzer.h"
#include "measurement/measurementprofile.h"
#include "measurement/measurementtypes.h"
#include "measurement/signalprocessor.h"

#include <QByteArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QtEndian>

#include <cmath>
#include <cstdio>

namespace {

int failures = 0;

void check(bool ok, const char* what)
{
    std::printf("[%s] %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) ++failures;
}

QVector<quint16> decodeRaw(const QJsonObject& row, const char* key)
{
    const QByteArray data = QByteArray::fromBase64(row[key].toString().toLatin1());
    QVector<quint16> samples(data.size() / 2);
    for (int i = 0; i < samples.size(); ++i)
        samples[i] = qFromLittleEndian<quint16>(data.constData() + 2 * i);
    return samples;
}

// One device -> host channel frame, as the firmware sends it.
QByteArray channelFrame(quint16 gain, quint16 index, quint8 channel, const QVector<quint16>& s)
{
    QByteArray b;
    auto u16 = [&b](quint16 v) { b.append(char(v & 0xFF)); b.append(char(v >> 8)); };
    b.append(char(0xAA)); b.append(char(0x55));
    u16(gain); u16(index);
    b.append(char(channel));
    u16(quint16(s.size()));
    for (quint16 v : s) u16(v);
    b.append(char(0xEE)); b.append(char(0xEE));
    return b;
}

const char* decisionName(FrameDecision d)
{
    switch (d) {
    case FrameDecision::EmptyInput: return "empty_filtered_input";
    case FrameDecision::BOnsetInconsistent: return "B_onset_inconsistent";
    case FrameDecision::BPairInvalid: return "B_pair_invalid";
    case FrameDecision::BLagJump: return "B_lag_jump";
    case FrameDecision::APairInvalid: return "A_pair_invalid";
    case FrameDecision::ABDifference: return "AB_difference";
    case FrameDecision::Measured: return "measured";
    }
    return "?";
}

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::fprintf(stderr, "usage: %s <round.jsonl> [--expect-low-1984]\n", argv[0]);
        return 2;
    }
    bool expectLow1984 = false;
    QString streamPath;  // bytes captured from a (fake) device instead of a synthesized stream
    for (int i = 2; i < argc; ++i) {
        const QByteArray arg(argv[i]);
        if (arg == "--expect-low-1984") expectLow1984 = true;
        else if (arg == "--stream" && i + 1 < argc) streamPath = QString::fromLocal8Bit(argv[++i]);
    }

    QFile file(QString::fromLocal8Bit(argv[1]));
    if (!file.open(QIODevice::ReadOnly)) {
        std::fprintf(stderr, "cannot open %s\n", argv[1]);
        return 2;
    }
    QJsonObject config;
    QJsonObject row;
    for (const QByteArray& line : file.readAll().split('\n')) {
        if (line.trimmed().isEmpty()) continue;
        const QJsonObject obj = QJsonDocument::fromJson(line).object();
        if (obj.contains("config")) config = obj["config"].toObject();
        if (obj["event"].toString() == "frame" && obj.contains("raw_BC")) row = obj;
    }
    if (row.isEmpty()) {
        std::fprintf(stderr, "no frame with raw_* channels\n");
        return 2;
    }

    // ---- protocol round trip ------------------------------------------------
    const QVector<quint16> bc = decodeRaw(row, "raw_BC"), bd = decodeRaw(row, "raw_BD"),
                           ac = decodeRaw(row, "raw_AC"), ad = decodeRaw(row, "raw_AD");
    const quint16 gain = quint16(config["gain_BC"].toInt(1241));
    const quint16 index = quint16(row["sequence"].toInt());

    QByteArray stream = QByteArray::fromHex("00AA13EEEE5A");  // leading noise
    stream += channelFrame(gain, index, 3, ac);              // channels out of order
    stream += channelFrame(gain, index, 1, bc);
    stream += QByteArray::fromHex("AA55FFFF");                // truncated junk header
    stream += channelFrame(gain, index, 4, ad);
    stream += channelFrame(gain, index, 2, bd);
    if (!streamPath.isEmpty()) {
        QFile captured(streamPath);
        if (!captured.open(QIODevice::ReadOnly)) {
            std::fprintf(stderr, "cannot open %s\n", qPrintable(streamPath));
            return 2;
        }
        stream = captured.readAll();
        std::printf("using captured stream %s (%lld bytes)\n", qPrintable(streamPath),
                    static_cast<long long>(stream.size()));
    }

    FrameAssembler assembler;
    WaveFrame frame;
    bool got = false;
    QRandomGenerator rng(1984);
    for (int pos = 0; pos < stream.size();) {
        const int chunk = 1 + int(rng.bounded(700));
        assembler.append(stream.mid(pos, chunk));
        pos += chunk;
        while (assembler.takeFrame(&frame)) got = true;
    }
    check(got, "FrameAssembler completes the frame from noisy, chunked bytes");
    check(frame.bc == bc && frame.bd == bd && frame.ac == ac && frame.ad == ad,
          "assembled samples equal the recorded samples");

    // ---- analysis, set up as the desktop MainWindow does ---------------------
    SignalProcessor processor;
    processor.probeDistanceCD = config["probe_distance_m"].toDouble(7.84e-3);
    processor.designFIR(1250000.0, 600000.0, 62500000.0);
    MeasureConfig measureConfig;
    const MeasurementProfile profile = MeasurementProfile::current();
    FrameAnalyzer analyzer(processor, measureConfig, profile);

    const FilteredFrame filtered = FrameAnalyzer::filter(processor, frame);
    const FrameAnalysis patient = analyzer.analyze(filtered, true);
    const FrameAnalysis debug = analyzer.analyze(filtered, false);

    std::printf("\nprofile.implementation = %s\n", qPrintable(profile.implementation));
    std::printf("patient decision = %s\n", decisionName(patient.decision));
    std::printf("debug   decision = %s, sos = %.6f (A %.6f, B %.6f), corrA %.6f, corrB %.6f, "
                "D %d, G %.3f\n",
                decisionName(debug.decision), debug.sos, debug.a.sos, debug.b.sos,
                debug.a.corr, debug.b.corr, debug.signedLagDiff, debug.pairMidGap);
    std::printf("recorded (build %s): sos_patient = %.6f, decision = %s\n",
                qPrintable(config["build"].toString()), row["sos_patient"].toDouble(),
                qPrintable(row["decision"].toString()));
    std::printf("\npatient evidence:\n%s\n",
                QJsonDocument(frameEvidence(patient, measureConfig, profile)).toJson().constData());

    if (expectLow1984) {
        // Same expectations as MainWindowSafetyTests::onsetGuardRejectsRecordedLowFrame.
        check(patient.decision == FrameDecision::BOnsetInconsistent,
              "patient measurement rejects the frame as B_onset_inconsistent");
        const QJsonObject bcEvidence = frameEvidence(patient, measureConfig, profile)
                                           ["B_arrivals"].toObject()["BC"].toObject();
        check(bcEvidence["first_hit"].toInt() == 1227, "BC first_hit == 1227");
        check(bcEvidence["onset"].toInt() == 1333, "BC onset == 1333");
        check(bcEvidence["forward_shift"].toInt() == 106, "BC forward_shift == 106");
        // The README records the old build's result for the same frame.
        check(debug.decision == FrameDecision::Measured
                  && std::abs(debug.b.sos - 1983.805668) < 0.01,
              "without the onset guard, B SOS matches the recorded 1983.805668 (+-0.01 m/s)");
        check(std::abs(debug.a.corr - 0.957704729) < 1e-6,
              "A correlation matches the recorded 0.957704729");
    }

    std::printf("\n%s: %d failure(s)\n", failures ? "FAILED" : "OK", failures);
    return failures ? 1 : 0;
}
