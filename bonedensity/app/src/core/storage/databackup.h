#pragma once

#include <QDateTime>
#include <QString>
#include <QStringList>

// Rolling copies of the runtime data files (patients, measurements, accounts,
// calibration) under <data folder>/backups/. A daily copy is taken at the first
// login of each day and an extra one right before any deletion; only the newest
// copies of each kind are kept. Restoring is manual: quit the program and copy
// the files from a backup folder back next to BoneDensity.exe.
namespace DataBackup {

struct Result {
    bool ok = true;        // false only when a copy was attempted and failed
    bool created = false;  // a new backup folder was written by this call
    QString folder;        // the backup folder (new or already present)
    QString error;
};

constexpr int kKeepDaily = 30;
constexpr int kKeepBeforeDelete = 30;

QString dailyLabel(const QDate& date);
QString beforeDeleteLabel(const QDateTime& time);

// Copies the existing files among filePaths into <root>/<label>/. Does nothing
// if that folder already exists. The copy is written to "<label>.partial" and
// renamed when complete, so an interrupted copy never looks like a backup.
Result snapshot(const QString& root, const QStringList& filePaths, const QString& label);

// Keeps the newest kKeepDaily daily and kKeepBeforeDelete pre-deletion folders
// and removes leftover ".partial" folders. Other folders under root are ignored.
void prune(const QString& root, int keepDaily = kKeepDaily,
           int keepBeforeDelete = kKeepBeforeDelete);

} // namespace DataBackup
