#include "storage/legacyimport.h"

#include "measurement/parametergroup.h"
#include "storage/patientstore.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QSet>

namespace LegacyImport {
namespace {

const QStringList& dataFiles()
{
    static const QStringList files = {QStringLiteral("patients.xml"), QStringLiteral("measurements.xml"),
                                      QStringLiteral("accounts.xml"), QStringLiteral("calibration.xml")};
    return files;
}

bool samePerson(const PatientInfo& a, const PatientInfo& b)
{
    return a.name.trimmed() == b.name.trimmed() && a.gender.trimmed() == b.gender.trimmed()
        && a.birthDay.trimmed() == b.birthDay.trimmed();
}

QString recordKey(const MeasurementRecord& record)
{
    if (!record.id.trimmed().isEmpty()) return QStringLiteral("id:") + record.id;
    return QStringLiteral("k:%1|%2|%3").arg(record.patientId, record.measuredAt, record.sos);
}

bool sameBytes(const QString& a, const QString& b)
{
    QFile fa(a), fb(b);
    if (!fa.open(QIODevice::ReadOnly) || !fb.open(QIODevice::ReadOnly)) return false;
    return fa.readAll() == fb.readAll();
}

// Copies a file unless the target exists; a differing existing target is noted.
void copyIfAbsent(const QString& source, const QString& target, const QString& label,
                  Result* result)
{
    if (!QFileInfo(source).isFile()) return;
    if (QFileInfo::exists(target)) {
        if (!sameBytes(source, target)) {
            result->notes << QStringLiteral("%1：数据文件夹里已有不同的版本，保留数据文件夹中的，旧文件未导入（%2）")
                                 .arg(label, QDir::toNativeSeparators(source));
        }
        return;
    }
    QDir().mkpath(QFileInfo(target).absolutePath());
    if (QFile::copy(source, target)) {
        ++result->otherFilesCopied;
    } else {
        result->ok = false;
        result->notes << QStringLiteral("%1：复制失败（%2）").arg(label, QDir::toNativeSeparators(source));
    }
}

QJsonObject logConfig(const QString& path, QDateTime* startedUtc)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return {};
    const QJsonObject first = QJsonDocument::fromJson(file.readLine()).object();
    if (first.value(QStringLiteral("event")).toString() != QStringLiteral("start")) return {};
    if (startedUtc) {
        *startedUtc = QDateTime::fromString(first.value(QStringLiteral("utc")).toString(), Qt::ISODateWithMs);
        if (!startedUtc->isValid()) *startedUtc = QFileInfo(path).lastModified();
    }
    return first.value(QStringLiteral("config")).toObject();
}

void importExperiments(const QString& legacyDir, const QString& dataRoot, Result* result)
{
    const QDir source(QDir(legacyDir).filePath(QStringLiteral("measurement-experiments")));
    if (!source.exists()) return;
    const QString experiments = QDir(dataRoot).filePath(QStringLiteral("experiments"));
    const QString unsorted = QDir(experiments).filePath(QStringLiteral("未分组"));
    const QFileInfoList files = source.entryInfoList(QDir::Files, QDir::Name);
    for (const QFileInfo& info : files) {
        QString folder;
        QDateTime started;
        const QJsonObject config = info.suffix() == QStringLiteral("jsonl")
            ? logConfig(info.absoluteFilePath(), &started) : QJsonObject();
        if (!config.isEmpty() && config.contains(QStringLiteral("implementation"))) {
            QString error;
            folder = ParameterGroup::prepareFolder(experiments, config, started, true, &error);
            if (folder.isEmpty()) {
                result->ok = false;
                result->notes << error;
                continue;
            }
        } else {
            folder = unsorted;
            QDir().mkpath(folder);
        }
        const QString target = QDir(folder).filePath(info.fileName());
        if (QFileInfo::exists(target)) continue;
        if (QFile::copy(info.absoluteFilePath(), target)) {
            if (folder == unsorted) ++result->otherFilesCopied;
            else ++result->logsCopied;
        } else {
            result->ok = false;
            result->notes << QStringLiteral("实验日志复制失败：%1").arg(info.fileName());
        }
    }
}

} // namespace

bool hasLegacyData(const QString& legacyDir)
{
    const QDir dir(legacyDir);
    if (QFileInfo::exists(dir.filePath(QString::fromLatin1(kMarkerName)))) return false;
    for (const QString& name : dataFiles()) {
        if (QFileInfo(dir.filePath(name)).isFile()) return true;
    }
    return QFileInfo(dir.filePath(QStringLiteral("measurement-experiments"))).isDir();
}

Result importFrom(const QString& legacyDir, const QString& dataRoot)
{
    Result result;
    if (QDir(legacyDir).absolutePath() == QDir(dataRoot).absolutePath() || !hasLegacyData(legacyDir))
        return result;
    result.performed = true;
    const QDir legacy(legacyDir);
    const QDir root(dataRoot);
    root.mkpath(QStringLiteral("."));

    // Archive: merge with whatever the data folder already holds.
    PatientStore store;
    QList<PatientInfo> oldPatients, patients;
    QList<MeasurementRecord> oldRecords, records;
    QString error;
    if (!store.load(legacy.filePath(QStringLiteral("patients.xml")),
                    legacy.filePath(QStringLiteral("measurements.xml")), &oldPatients, &oldRecords, &error)) {
        result.ok = false;
        result.error = QStringLiteral("旧档案无法读取：%1").arg(error);
        return result;
    }
    const QString patientsPath = root.filePath(QStringLiteral("patients.xml"));
    const QString recordsPath = root.filePath(QStringLiteral("measurements.xml"));
    if (!store.load(patientsPath, recordsPath, &patients, &records, &error)) {
        result.ok = false;
        result.error = QStringLiteral("数据文件夹中的档案无法读取：%1").arg(error);
        return result;
    }

    QHash<QString, int> byId;
    for (int i = 0; i < patients.size(); ++i) byId.insert(patients.at(i).id, i);
    QSet<QString> conflicting;
    for (const PatientInfo& patient : oldPatients) {
        const auto found = byId.constFind(patient.id);
        if (found == byId.cend()) {
            byId.insert(patient.id, patients.size());
            patients.append(patient);
            ++result.patientsAdded;
        } else if (samePerson(patients.at(*found), patient)) {
            ++result.patientsAlreadyPresent;
        } else {
            conflicting.insert(patient.id);
            ++result.patientConflicts;
            const PatientInfo& kept = patients.at(*found);
            result.notes << QStringLiteral("档案编号 %1 冲突：数据文件夹中是 %2（%3，%4），旧数据中是 %5（%6，%7）。"
                                           "旧数据中的这个人及其检测记录未导入。")
                                .arg(patient.id, kept.name, kept.gender, kept.birthDay,
                                     patient.name, patient.gender, patient.birthDay);
        }
    }
    QSet<QString> recordKeys;
    for (const MeasurementRecord& record : records) recordKeys.insert(recordKey(record));
    for (const MeasurementRecord& record : oldRecords) {
        if (conflicting.contains(record.patientId)) {
            ++result.measurementsLeftOut;
            continue;
        }
        const QString key = recordKey(record);
        if (recordKeys.contains(key)) {
            ++result.measurementsAlreadyPresent;
            continue;
        }
        recordKeys.insert(key);
        records.append(record);
        ++result.measurementsAdded;
    }
    if (result.patientsAdded > 0 || result.measurementsAdded > 0) {
        if (!store.savePatientData(patientsPath, recordsPath, patients, records, &error)) {
            result.ok = false;
            result.error = QStringLiteral("合并后的档案保存失败：%1").arg(error);
            return result;
        }
    }

    copyIfAbsent(legacy.filePath(QStringLiteral("accounts.xml")), root.filePath(QStringLiteral("accounts.xml")),
                 QStringLiteral("账号"), &result);
    copyIfAbsent(legacy.filePath(QStringLiteral("calibration.xml")), root.filePath(QStringLiteral("calibration.xml")),
                 QStringLiteral("校准参数"), &result);
    for (const QString& ini : {QStringLiteral("device.ini"), QStringLiteral("measurement-guide.ini")}) {
        copyIfAbsent(legacy.filePath(ini), root.filePath(QStringLiteral("settings/") + ini),
                     QStringLiteral("设置 ") + ini, &result);
    }
    copyIfAbsent(legacy.filePath(QStringLiteral("angle_features.csv")),
                 root.filePath(QStringLiteral("angle_features.csv")), QStringLiteral("角度特征表"), &result);
    importExperiments(legacyDir, dataRoot, &result);

    // Report in the data folder; marker in the old folder.
    const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss"));
    const QString reports = root.filePath(QStringLiteral("import-reports"));
    root.mkpath(QStringLiteral("import-reports"));
    result.reportPath = QDir(reports).filePath(QStringLiteral("import-%1.txt").arg(stamp));
    for (int n = 2; QFileInfo::exists(result.reportPath); ++n)
        result.reportPath = QDir(reports).filePath(QStringLiteral("import-%1-%2.txt").arg(stamp).arg(n));
    const QString text = QStringLiteral("从旧位置导入数据\n来源：%1\n目标：%2\n时间：%3\n\n%4\n")
                             .arg(QDir::toNativeSeparators(legacyDir), QDir::toNativeSeparators(dataRoot),
                                  QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")),
                                  summary(result));
    QSaveFile report(result.reportPath);
    if (report.open(QIODevice::WriteOnly)) {
        report.write(QByteArray("\xEF\xBB\xBF") + text.toUtf8());
        report.commit();
    }
    if (result.ok) {
        QSaveFile marker(legacy.filePath(QString::fromLatin1(kMarkerName)));
        if (marker.open(QIODevice::WriteOnly)) {
            marker.write(QByteArray("\xEF\xBB\xBF") +
                         (QStringLiteral("本文件夹的数据已导入统一数据文件夹，以后不再从这里读取。\n") + text).toUtf8());
            marker.commit();
        }
    }
    return result;
}

QString summary(const Result& result)
{
    if (!result.performed) return QStringLiteral("没有需要导入的旧数据。");
    QStringList lines;
    if (!result.error.isEmpty()) lines << QStringLiteral("导入失败：") + result.error;
    lines << QStringLiteral("档案：新增 %1 人，已存在 %2 人，编号冲突 %3 人")
                 .arg(result.patientsAdded).arg(result.patientsAlreadyPresent).arg(result.patientConflicts)
          << QStringLiteral("检测记录：新增 %1 条，已存在 %2 条，因冲突未导入 %3 条")
                 .arg(result.measurementsAdded).arg(result.measurementsAlreadyPresent).arg(result.measurementsLeftOut)
          << QStringLiteral("实验日志：按参数组导入 %1 份；其他文件 %2 个")
                 .arg(result.logsCopied).arg(result.otherFilesCopied);
    if (!result.notes.isEmpty()) lines << QString() << QStringLiteral("需要注意：") << result.notes;
    lines << QString() << QStringLiteral("旧文件夹中的数据文件仍然保留，没有被删除。");
    return lines.join(QLatin1Char('\n'));
}

} // namespace LegacyImport
