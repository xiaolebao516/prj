# Project Map Reference

> **Authority:** Agent-only, task-triggered map of durable code, data, and component ownership. Product behavior belongs to [product-spec.md](product-spec.md); current implementation details belong to source code.

## Product and Toolchain

- Root: `D:/Repository/prj/bonedensity/app`
- Application: Qt 6.5.3 / C++17 / qmake / MinGW 11.2 Windows desktop application.
- Hardware connection: Pico/RP2040 over USB serial at the protected 115200-baud protocol.
- Canonical commands and toolchain paths are defined once in `AGENTS.md`.

## Components

- `src/mainwindow.cpp`: login, UI, serial acquisition, patient measurement, archive/chart/report orchestration; high-coupling and changed locally only.
- `src/signalprocessor.cpp`: filtering, arrival time, refined correlation peaks, and SOS calculation.
- `src/bonehealth.cpp`: age/reference mean, bone-strength, fracture-risk, and bone-age calculations.
- `src/utils.cpp`: final-round clustering and trimmed means.
- `src/accountstore.cpp`: local accounts, authentication, atomic save, and damaged-admin recovery.
- `src/patientstore.cpp`: patients, measurement history, legacy migration, and backups.
- `src/calibration.cpp`, `src/calibrationdialog.cpp`, `src/calibrationstore.cpp`: calibration calculation, wizard, history, activation, and recovery.
- `src/reportwidget.cpp`: shared screen, PDF, and print rendering.
- `src/agesoschartwidget.cpp`: age/sex reference chart and latest measurement point.
- `ui/mainwindow.ui`: primary pages and controls.
- `resources/`: measurement, report, and age-SOS images.
- `tests/`: calibration-focused automated suite.
- `testdata/age-sos-reference/`: anonymous age-SOS demonstration XML.

For measurement-pipeline invariants and change impact, load [architecture.md](architecture.md). For gate diagnosis, load [operations/debugging.md](operations/debugging.md). For calibration conclusions, load [domains/calibration.md](domains/calibration.md).

## Runtime Data

The application reads and writes beside `QCoreApplication::applicationDirPath()`:

- `accounts.xml`: account, role, enabled state, random salt, and SHA-256 hash.
- `patients.xml`: one record per patient.
- `measurements.xml`: multiple records per patient with patient/result snapshots.
- `calibration.xml`: current, previous, default D, and calibration history.
- optional `angle_features.csv`: diagnostic output.

Patient ID is the history key; measurement records use independent UUIDs. Candidate data is saved before memory replacement. Migration and damage recovery preserve backups. Runtime XML may contain unencrypted operational or patient data, so tests use isolated directories or anonymous fixtures only.
