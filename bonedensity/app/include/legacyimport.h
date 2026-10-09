#pragma once

#include <QString>
#include <QStringList>

// Brings data written by older versions next to the program (patients,
// measurements, accounts, calibration, settings, experiment logs) into the
// unified data folder. The old folder is only read; a marker file there
// records that it was imported, so it is not imported twice.
//
// Merge rules:
//  - patients: added when the archive number is new; same number with the same
//    name, sex and birth date is the same person (kept as is); same number with
//    a different person is a conflict: that person and their records are left
//    out and listed in the report;
//  - measurements: added unless a record with the same id (or patient, time and
//    SOS) already exists;
//  - accounts, calibration, settings: copied only when the data folder has none;
//  - experiment logs: copied into the folder of the parameter group written at
//    the top of each log; other files go to experiments/未分组.
namespace LegacyImport {

struct Result {
    bool performed = false;   // false: nothing to import or already imported
    bool ok = true;
    QString error;
    int patientsAdded = 0;
    int patientsAlreadyPresent = 0;
    int patientConflicts = 0;
    int measurementsAdded = 0;
    int measurementsAlreadyPresent = 0;
    int measurementsLeftOut = 0;
    int logsCopied = 0;
    int otherFilesCopied = 0;
    QStringList notes;        // conflicts and files that were not taken over
    QString reportPath;
};

inline constexpr const char* kMarkerName = "BoneDensity.data-moved.txt";

bool hasLegacyData(const QString& legacyDir);
Result importFrom(const QString& legacyDir, const QString& dataRoot);
QString summary(const Result& result);

} // namespace LegacyImport
