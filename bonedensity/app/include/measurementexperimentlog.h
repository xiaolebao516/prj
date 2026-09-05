#pragma once

#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>
#include <QVector>
#include <QtEndian>

// Development evidence only. No patient/account identifiers are accepted here.
// Buffered QFile writes, no per-frame flush, no automatic deletion/overwrite.
class MeasurementExperimentLog
{
public:
    bool start(const QString& directory, const QJsonObject& config,
               qint64 fileLimit = 256LL * 1024 * 1024,
               qint64 directoryLimit = 2LL * 1024 * 1024 * 1024)
    {
        close();
        error_.clear();
        bytes_ = 0;
        sequence_ = 0;
        limit_ = fileLimit;
        QDir dir(directory);
        if (!dir.mkpath(".")) return fail("cannot create experiment directory");
        qint64 used = 0;
        for (const auto& info : dir.entryInfoList({"round-*.jsonl"}, QDir::Files))
            used += info.size();
        limit_ = qMin(limit_, directoryLimit - used);
        if (limit_ <= 0) return fail("experiment storage limit reached");
        file_.setFileName(dir.filePath("round-" +
            QDateTime::currentDateTimeUtc().toString("yyyyMMdd-HHmmss-zzz") + "-" +
            QUuid::createUuid().toString(QUuid::WithoutBraces) + ".jsonl"));
        if (!file_.open(QIODevice::WriteOnly | QIODevice::NewOnly))
            return fail(file_.errorString());
        timer_.start();
        return write({{"event", "start"}, {"schema", 1}, {"config", config},
            {"utc", QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)},
            {"raw_encoding", "base64 uint16 little-endian; BC, BD, AC, AD"}});
    }

    bool write(QJsonObject record)
    {
        if (!file_.isOpen()) return false;
        record["sequence"] = ++sequence_;
        record["elapsed_ms"] = timer_.elapsed();
        const QByteArray bytes = QJsonDocument(record).toJson(QJsonDocument::Compact) + '\n';
        if (bytes_ + bytes.size() > limit_) return fail("experiment storage limit reached");
        if (file_.write(bytes) != bytes.size()) return fail(file_.errorString());
        bytes_ += bytes.size();
        return true;
    }

    bool close()
    {
        if (!file_.isOpen()) return error_.isEmpty();
        const bool ok = file_.flush();
        if (!ok) error_ = file_.errorString();
        file_.close();
        return ok;
    }
    ~MeasurementExperimentLog() { close(); }
    bool active() const { return file_.isOpen(); }
    QString path() const { return file_.fileName(); }
    QString error() const { return error_; }

    static QString encodeRaw(const QVector<quint16>& samples)
    {
        QByteArray bytes(samples.size() * 2, Qt::Uninitialized);
        for (qsizetype i = 0; i < samples.size(); ++i)
            qToLittleEndian<quint16>(samples[i], bytes.data() + i * 2);
        return QString::fromLatin1(bytes.toBase64());
    }

private:
    bool fail(const QString& error)
    {
        error_ = error;
        file_.close();
        return false;
    }
    QFile file_;
    QElapsedTimer timer_;
    QString error_;
    qint64 bytes_ = 0;
    qint64 limit_ = 0;
    qint64 sequence_ = 0;
};
