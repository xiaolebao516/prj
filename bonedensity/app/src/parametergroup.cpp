#include "parametergroup.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QSaveFile>

namespace ParameterGroup {
namespace {

struct Label {
    const char* key;
    const char* text;
};

// Order and wording of 参数说明.txt; also the list of group-defining keys.
const Label kLabels[] = {
    {"implementation", "算法版本"},
    {"probe_distance_m", "探头距离 D (m)"},
    {"sample_period_s", "采样周期 (s)"},
    {"SOS_offset", "SOS 偏移 (m/s)"},
    {"B_only", "只用 B 通道出结果"},
    {"frame_corr_A", "单帧 corrA 下限"},
    {"frame_corr_B", "单帧 corrB 下限"},
    {"round_corr_A", "整轮 corrA 下限"},
    {"round_corr_B", "整轮 corrB 下限"},
    {"angle_gate_enabled", "姿态门控"},
    {"D_min", "姿态 D 下限"},
    {"D_max", "姿态 D 上限"},
    {"G_min", "姿态 G 下限"},
    {"G_max", "姿态 G 上限"},
    {"warmup", "稳定簇预热帧数"},
    {"lock_need", "稳定簇锁定所需帧数"},
    {"lag_tolerance", "稳定簇容差 (采样点)"},
    {"unlock_count", "解锁所需帧数"},
    {"window_size", "稳定窗口帧数"},
    {"frame_target", "每轮有效帧数"},
    {"round_target", "测量轮数"},
    {"round_cluster_tolerance", "轮间聚类容差"},
    {"B_onset_forward_limit", "B 起始点前移上限"},
    {"B_clipped_peak_extension", "B 截顶峰延伸"},
    {"partial_relock_retention_lag", "重锁保留容差"},
};

QString valueText(const QJsonValue& value)
{
    if (value.isBool()) return value.toBool() ? QStringLiteral("是") : QStringLiteral("否");
    if (value.isDouble()) return QString::number(value.toDouble(), 'g', 10);
    if (value.isString()) return value.toString();
    return QString::fromUtf8(QJsonDocument(QJsonObject{{"v", value}}).toJson(QJsonDocument::Compact));
}

bool writeText(const QString& path, const QByteArray& bytes, QString* error)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) {
        if (error) *error = QStringLiteral("无法写入 %1").arg(QDir::toNativeSeparators(path));
        return false;
    }
    return true;
}

} // namespace

const QStringList& keys()
{
    static const QStringList list = [] {
        QStringList result;
        for (const Label& label : kLabels) result << QString::fromLatin1(label.key);
        return result;
    }();
    return list;
}

QJsonObject parameters(const QJsonObject& config)
{
    QJsonObject subset;
    for (const QString& key : keys()) {
        if (config.contains(key)) subset.insert(key, config.value(key));
    }
    return subset;
}

QString id(const QJsonObject& config)
{
    const QJsonObject subset = parameters(config);
    // QJsonObject keeps keys sorted, so the compact form is canonical.
    const QByteArray canonical = QJsonDocument(subset).toJson(QJsonDocument::Compact);
    const QString hash = QString::fromLatin1(
        QCryptographicHash::hash(canonical, QCryptographicHash::Sha1).toHex().left(8));
    QString name = subset.value(QStringLiteral("implementation")).toString();
    name.replace(QRegularExpression(QStringLiteral("[^A-Za-z0-9._-]")), QStringLiteral("-"));
    if (name.isEmpty()) name = QStringLiteral("unknown");
    return name + QLatin1Char('_') + hash;
}

QString describe(const QJsonObject& parameters)
{
    QStringList lines;
    for (const Label& label : kLabels) {
        const QString key = QString::fromLatin1(label.key);
        if (!parameters.contains(key)) continue;
        lines << QStringLiteral("%1：%2").arg(QString::fromUtf8(label.text), valueText(parameters.value(key)));
    }
    return lines.join(QLatin1Char('\n'));
}

QString prepareFolder(const QString& experimentsRoot, const QJsonObject& config,
                      const QDateTime& when, bool newLog, QString* error)
{
    const QString groupId = id(config);
    const QString folder = QDir(experimentsRoot).filePath(groupId);
    if (!QDir().mkpath(folder)) {
        if (error) *error = QStringLiteral("无法创建参数组文件夹：%1").arg(QDir::toNativeSeparators(folder));
        return QString();
    }

    const QString jsonPath = QDir(folder).filePath(QStringLiteral("parameters.json"));
    QJsonObject info;
    QFile existing(jsonPath);
    if (existing.open(QIODevice::ReadOnly)) info = QJsonDocument::fromJson(existing.readAll()).object();
    existing.close();

    const QString stamp = when.toUTC().toString(Qt::ISODate);
    const QString first = info.value(QStringLiteral("first_used_utc")).toString();
    const QString last = info.value(QStringLiteral("last_used_utc")).toString();
    const int logs = info.value(QStringLiteral("logs")).toInt() + (newLog ? 1 : 0);
    QJsonArray sessionIds = info.value(QStringLiteral("session_ids")).toArray();
    int anonymous = info.value(QStringLiteral("logs_without_session_id")).toInt();
    if (newLog) {
        const QString session = config.value(QStringLiteral("measurement_session_id")).toString();
        if (session.isEmpty()) ++anonymous;
        else if (!sessionIds.contains(session)) sessionIds.append(session);
    }
    const int sessions = sessionIds.size() + anonymous;
    const QJsonObject subset = parameters(config);
    const QJsonObject updated{
        {"group_id", groupId},
        {"parameters", subset},
        {"first_used_utc", first.isEmpty() || stamp < first ? stamp : first},
        {"last_used_utc", last.isEmpty() || stamp > last ? stamp : last},
        {"sessions", sessions},
        {"logs", logs},
        {"logs_without_session_id", anonymous},
        {"session_ids", sessionIds}};
    if (updated == info) return folder;

    const QString localFirst = QDateTime::fromString(updated.value("first_used_utc").toString(), Qt::ISODate)
                                   .toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm"));
    const QString localLast = QDateTime::fromString(updated.value("last_used_utc").toString(), Qt::ISODate)
                                  .toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm"));
    const QString text = QStringLiteral("参数组：%1\n首次使用：%2\n最近使用：%3\n检测次数：%4（实验日志 %5 份，每轮一份）\n\n"
                                        "本文件夹中的数据都在以下参数下测得（增益每次检测单独记录在日志里）：\n%6\n")
                             .arg(groupId, localFirst, localLast)
                             .arg(sessions)
                             .arg(logs)
                             .arg(describe(subset));

    if (!writeText(jsonPath, QJsonDocument(updated).toJson(QJsonDocument::Indented), error)) return QString();
    if (!writeText(QDir(folder).filePath(QStringLiteral("参数说明.txt")),
                   QByteArray("\xEF\xBB\xBF") + text.toUtf8(), error)) {
        return QString();
    }
    return folder;
}

} // namespace ParameterGroup
