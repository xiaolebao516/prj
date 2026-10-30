#pragma once

#include <QString>

// One place for every runtime file: archive XML, accounts, calibration,
// settings, backups and experiment logs, so the script build, the Qt Creator
// build and an installed package never keep separate copies.
//
// The folder is found in this order:
//   1. the BONE_DATA_DIR environment variable (tests, special setups);
//   2. BoneDensityData next to the program, if it exists (installed package);
//   3. BoneDensityData in the nearest parent folder holding BoneDensity.pro
//      (development tree: every build under app/build uses app/BoneDensityData);
//   4. BoneDensityData next to the program (created).
namespace DataLocation {

inline constexpr const char* kFolderName = "BoneDensityData";

QString resolve(const QString& programDir, const QString& overrideDir);

// Resolved once per run and created if missing.
QString root();
QString filePath(const QString& name);
QString settingsFile(const QString& name);    // <root>/settings/<name>
QString experimentsRoot();                    // <root>/experiments

} // namespace DataLocation
