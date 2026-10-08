#include "databackup.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>

#include <algorithm>

namespace DataBackup {
namespace {

const QString kPartialSuffix = QStringLiteral(".partial");
const QString kBeforeDeleteSuffix = QStringLiteral("_before-delete");

const QRegularExpression& dailyPattern()
{
    static const QRegularExpression pattern(QStringLiteral("^\\d{4}-\\d{2}-\\d{2}$"));
    return pattern;
}

const QRegularExpression& beforeDeletePattern()
{
    static const QRegularExpression pattern(
        QStringLiteral("^\\d{4}-\\d{2}-\\d{2}_\\d{6}_before-delete$"));
    return pattern;
}

void keepNewest(QDir& root, QStringList names, int keep)
{
    names.sort();   // labels start with the date/time, so name order is age order
    for (int i = 0; i < names.size() - keep; ++i) {
        QDir(root.filePath(names.at(i))).removeRecursively();
    }
}

} // namespace

QString dailyLabel(const QDate& date)
{
    return date.toString(QStringLiteral("yyyy-MM-dd"));
}

QString beforeDeleteLabel(const QDateTime& time)
{
    return time.toString(QStringLiteral("yyyy-MM-dd_HHmmss")) + kBeforeDeleteSuffix;
}

Result snapshot(const QString& root, const QStringList& filePaths, const QString& label)
{
    Result result;
    QDir rootDir(root);
    result.folder = rootDir.filePath(label);
    if (QFileInfo::exists(result.folder)) return result;
    const bool anySource = std::any_of(filePaths.cbegin(), filePaths.cend(),
                                       [](const QString& path) { return QFileInfo(path).isFile(); });
    if (!anySource) {
        result.folder.clear();
        return result;   // nothing to back up yet (fresh installation)
    }

    if (!rootDir.mkpath(QStringLiteral("."))) {
        result.ok = false;
        result.error = QStringLiteral("无法创建备份文件夹：%1").arg(QDir::toNativeSeparators(root));
        return result;
    }
    const QString partial = result.folder + kPartialSuffix;
    QDir(partial).removeRecursively();
    if (!rootDir.mkpath(QFileInfo(partial).fileName())) {
        result.ok = false;
        result.error = QStringLiteral("无法创建备份文件夹：%1").arg(QDir::toNativeSeparators(partial));
        return result;
    }

    for (const QString& path : filePaths) {
        const QFileInfo source(path);
        if (!source.isFile()) continue;
        const QString target = QDir(partial).filePath(source.fileName());
        if (!QFile::copy(source.absoluteFilePath(), target)) {
            QDir(partial).removeRecursively();
            result.ok = false;
            result.error = QStringLiteral("无法复制 %1").arg(source.fileName());
            return result;
        }
    }

    if (!rootDir.rename(QFileInfo(partial).fileName(), label)) {
        QDir(partial).removeRecursively();
        result.ok = false;
        result.error = QStringLiteral("无法完成备份文件夹：%1").arg(QDir::toNativeSeparators(result.folder));
        return result;
    }
    result.created = true;
    return result;
}

void prune(const QString& root, int keepDaily, int keepBeforeDelete)
{
    QDir rootDir(root);
    if (!rootDir.exists()) return;

    QStringList daily;
    QStringList beforeDelete;
    const QStringList folders = rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& name : folders) {
        if (name.endsWith(kPartialSuffix)) {
            QDir(rootDir.filePath(name)).removeRecursively();
        } else if (dailyPattern().match(name).hasMatch()) {
            daily << name;
        } else if (beforeDeletePattern().match(name).hasMatch()) {
            beforeDelete << name;
        }
    }
    keepNewest(rootDir, daily, keepDaily);
    keepNewest(rootDir, beforeDelete, keepBeforeDelete);
}

} // namespace DataBackup
