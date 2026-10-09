#include "datalocation.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

namespace DataLocation {

QString resolve(const QString& programDir, const QString& overrideDir)
{
    if (!overrideDir.trimmed().isEmpty()) return QDir::cleanPath(overrideDir);

    const QDir program(programDir);
    const QString beside = program.filePath(QString::fromLatin1(kFolderName));
    if (QFileInfo(beside).isDir()) return QDir::cleanPath(beside);

    QDir dir(programDir);
    for (int level = 0; level < 8; ++level) {
        if (QFileInfo::exists(dir.filePath(QStringLiteral("BoneDensity.pro"))))
            return QDir::cleanPath(dir.filePath(QString::fromLatin1(kFolderName)));
        if (!dir.cdUp()) break;
    }
    return QDir::cleanPath(beside);
}

QString root()
{
    static const QString path = [] {
        const QString resolved = resolve(QCoreApplication::applicationDirPath(),
                                         qEnvironmentVariable("BONE_DATA_DIR"));
        QDir().mkpath(resolved);
        return resolved;
    }();
    return path;
}

QString filePath(const QString& name)
{
    return QDir(root()).filePath(name);
}

QString settingsFile(const QString& name)
{
    const QString folder = QDir(root()).filePath(QStringLiteral("settings"));
    QDir().mkpath(folder);
    return QDir(folder).filePath(name);
}

QString experimentsRoot()
{
    return QDir(root()).filePath(QStringLiteral("experiments"));
}

} // namespace DataLocation
