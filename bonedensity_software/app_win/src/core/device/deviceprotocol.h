#pragma once
#include <QByteArray>
#include <QMap>
#include <QQueue>
#include <QVector>

// Serial protocol of the Pico/RP2040 acquisition board (USB CDC, 115200 8N1).
//
//   command, host -> device:  A5 5A | gain u16 | frame index u16
//   channel, device -> host:  AA 55 | gain u16 | frame index u16 | channel u8 (1..4)
//                             | sample count u16 (1..5000) | samples u16 x count | EE EE
//
// All integers are little-endian; samples carry 12 bits. One acquisition is
// complete when all four channels of the same frame index have arrived.
namespace DeviceProtocol {

QByteArray acquireCommand(quint16 gain, quint16 frameIndex);

} // namespace DeviceProtocol

// One complete acquisition. Device channel order: CH1 B->C, CH2 B->D,
// CH3 A->C, CH4 A->D.
struct WaveFrame {
    QVector<quint16> bc;
    QVector<quint16> bd;
    QVector<quint16> ac;
    QVector<quint16> ad;

    void clear() { bc.clear(); bd.clear(); ac.clear(); ad.clear(); }
};

// Channels received so far for one frame index.
struct WaveGroup {
    bool has[4] = {false, false, false, false};
    QVector<quint16> ch[4];
};

// Reassembles channel frames from the byte stream: resynchronises after noise
// or a damaged frame, drops invalid channels and keeps at most
// maxPendingFrames incomplete frame indexes (oldest dropped first).
class FrameAssembler
{
public:
    static constexpr int maxPendingFrames = 16;

    void append(const QByteArray& bytes) { buffer_.append(bytes); }

    // Consumes buffered bytes up to and including the next completed
    // acquisition. Returns false once more bytes are needed.
    bool takeFrame(WaveFrame* frame);

    // Drops buffered bytes and incomplete frames.
    void clear();

    const QByteArray& buffer() const { return buffer_; }
    const QMap<quint16, WaveGroup>& pendingFrames() const { return groups_; }
    const QQueue<quint16>& pendingOrder() const { return order_; }

private:
    QByteArray buffer_;
    QMap<quint16, WaveGroup> groups_;
    QQueue<quint16> order_;
};
