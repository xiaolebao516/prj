#include "device/deviceprotocol.h"

QByteArray DeviceProtocol::acquireCommand(quint16 gain, quint16 frameIndex)
{
    QByteArray cmd;
    cmd.append(char(0xA5));
    cmd.append(char(0x5A));
    cmd.append(char(gain & 0xFF));
    cmd.append(char((gain >> 8) & 0xFF));
    cmd.append(char(frameIndex & 0xFF));
    cmd.append(char((frameIndex >> 8) & 0xFF));
    return cmd;
}

void FrameAssembler::clear()
{
    buffer_.clear();
    groups_.clear();
    order_.clear();
}

bool FrameAssembler::takeFrame(WaveFrame* frame)
{
    const QByteArray magic = QByteArray::fromHex("AA55");
    const int headerSize = 9;

    while (true) {
        const int start = buffer_.indexOf(magic);
        if (start < 0) {
            if (buffer_.size() > 4096)
                buffer_.remove(0, buffer_.size() - 2);
            return false;
        }
        if (start > 0) buffer_.remove(0, start);
        if (buffer_.size() < headerSize) return false;

        const unsigned char* p = reinterpret_cast<const unsigned char*>(buffer_.constData());
        // p[2..3] carries the frame's gain; it is not used on this side.
        const quint16 index = p[4] | (p[5] << 8);
        const quint8 channel = p[6];
        const quint16 length = p[7] | (p[8] << 8);

        if (length == 0 || length > 5000) {
            buffer_.remove(0, 1);
            continue;
        }
        const int frameBytes = headerSize + length * 2 + 2;
        if (buffer_.size() < frameBytes) return false;

        if (p[headerSize + length * 2] != 0xEE || p[headerSize + length * 2 + 1] != 0xEE) {
            buffer_.remove(0, 1);
            continue;
        }
        if (channel < 1 || channel > 4) {
            buffer_.remove(0, frameBytes);
            continue;
        }

        const unsigned char* payload = p + headerSize;
        QVector<quint16> samples;
        samples.reserve(length);
        for (int i = 0; i < length; ++i)
            samples.append((payload[2 * i] | (payload[2 * i + 1] << 8)) & 0x0FFF);

        if (!groups_.contains(index)) {
            while (order_.size() >= maxPendingFrames) groups_.remove(order_.dequeue());
            order_.enqueue(index);
        }
        WaveGroup& group = groups_[index];
        group.ch[channel - 1] = samples;
        group.has[channel - 1] = true;
        buffer_.remove(0, frameBytes);

        if (group.has[0] && group.has[1] && group.has[2] && group.has[3]) {
            frame->bc = group.ch[0];
            frame->bd = group.ch[1];
            frame->ac = group.ch[2];
            frame->ad = group.ch[3];
            groups_.remove(index);
            order_.removeAll(index);
            return true;
        }
    }
}
