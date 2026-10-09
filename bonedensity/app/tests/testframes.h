#pragma once
#include <QByteArray>

// One device channel frame (see DeviceProtocol) carrying a single sample.
inline QByteArray channelFrame(quint16 index, quint8 channel, quint16 sample = 2048)
{
    const quint16 gain = 1024;
    QByteArray bytes;
    bytes.append(char(0xAA));
    bytes.append(char(0x55));
    bytes.append(char(gain & 0xFF));
    bytes.append(char((gain >> 8) & 0xFF));
    bytes.append(char(index & 0xFF));
    bytes.append(char((index >> 8) & 0xFF));
    bytes.append(char(channel));
    bytes.append(char(1));
    bytes.append(char(0));
    bytes.append(char(sample & 0xFF));
    bytes.append(char((sample >> 8) & 0xFF));
    bytes.append(char(0xEE));
    bytes.append(char(0xEE));
    return bytes;
}
