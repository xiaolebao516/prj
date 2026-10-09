# Project Map Reference

> **Authority:** Agent-only, task-triggered map of durable code, data, and component ownership. Product behavior belongs to [product-spec.md](product-spec.md); current implementation details belong to source code.

## Product and Toolchain

- Root: `D:/Repository/prj/bonedensity/app`
- Application: Qt 6.5.3 / C++17 / qmake / MinGW 11.2 Windows desktop application.
- Hardware connection: Pico/RP2040 over USB serial at the protected 115200-baud protocol.
- Canonical commands and toolchain paths are defined once in `AGENTS.md`.

## Components

Sources live in two layers, each listed once in a qmake include file used by the application and the tests:

- `src/core/` (`core.pri`): Qt Core/Xml only. It must never include a Gui/Widgets header; the core-only test builds (`tests/core_tests.pro`, store tests) fail if it does.
- `src/ui/` (`ui.pri`, pulls in the core): widgets, dialogs, the main window and `resources/`.
- `src/app/main.cpp`: single-instance lock, `--import-legacy`, legacy take-over, main window.

Core:

- `src/core/records.h`: `PatientInfo` and `MeasurementRecord` (archive records).
- `src/core/device/deviceprotocol.cpp`: serial command encoding and `FrameAssembler` (byte stream -> complete four-channel `WaveFrame`; resynchronisation, bounded partial frames).
- `src/core/measurement/`:
  - `measurementtypes.h`: `MeasureConfig` (all measurement parameters: gates, stability, rounds, SOS policy) and signal result types.
  - `measurementprofile.cpp`: `MeasurementProfile` (formal build vs isolated `*_trial` builds; the only reader of the `BONE_*_EXPERIMENT` defines) and `measurementParameters()`, the recorded parameter set.
  - `signalprocessor.cpp`: filtering, arrival time, refined correlation peaks, and SOS calculation.
  - `frameanalyzer.cpp`: stateless per-frame analysis (false-wave erase + FIR, arrivals, B/A pairs, D/G, frame gates), calibration frames, and the experiment-log frame record.
  - `measurementsession.cpp`: stateful patient measurement (B-lag stability lock, relock deferral, round values, round quality gate, candidate clustering, exactly-five final selection, gate statistics); experiment events go to a callback.
  - `utils.cpp`: final-round clustering and trimmed means.
  - `parametergroup.cpp`: parameter groups (algorithm version, gates, probe D, SOS offset, channel choice); experiment logs go to `BoneDensityData/experiments/<group>/` with `parameters.json` and `参数说明.txt`; results store `parameterGroup`.
  - `measurementexperimentlog.h`: bounded JSONL experiment log (Debug builds).
- `src/core/calibration/`: `calibration.cpp` calculation, `calibrationstore.cpp` history, activation, and recovery.
- `src/core/storage/`:
  - `accountstore.cpp`: local accounts, authentication, atomic save, and damaged-admin recovery.
  - `patientstore.cpp`: patients, measurement history, legacy migration, and backups.
  - `databackup.cpp`: rolling copies of the data XML files under `backups/`.
  - `datalocation.cpp`: the single data folder `BoneDensityData` (env `BONE_DATA_DIR`; beside the program in a package; `app/BoneDensityData` for every build in the development tree; git-ignored).
  - `legacyimport.cpp`: one-time import of data kept next to older programs (automatic at start-up, or `BoneDensity.exe --import-legacy <folder>...`); conflicting archive numbers are never merged and are listed in `BoneDensityData/import-reports/`.
- `src/core/health/`: `sosreference.cpp` adult radius SOS reference table (中国公共卫生 2015), T/Z, the single source for scores and the adult age-SOS chart; `bonehealth.cpp` patient age, bone-strength, fracture-risk and bone-age calculations, and `deriveResult` used for new and saved records.

UI:

- `src/ui/mainwindow/mainwindow*.cpp`: one `MainWindow` class split by area (`mainwindow.cpp` construction/login/close guard/account menu, `_device` serial link, `_measurement` patient measurement flow and calibration acquisition, `_display` live charts and process panel, `_patients` archive, history, export and backups, `_report` report, `_layout` theme and page layouts; shared helpers in `mainwindow_internal.h`; pages and controls in `mainwindow.ui`). It orchestrates; the measurement algorithm stays in the core.
- `src/ui/widgets/`: `reportwidget.cpp` shared screen, PDF, and print rendering; `agesoschartwidget.cpp` age/sex reference chart and latest measurement point.
- `src/ui/dialogs/`: calibration wizard, measurement guide, patient form.
- `src/ui/theme/theme.cpp`: the "Quiet Instrument" token palette shared by `resources/theme.qss` (a template with `@tokens`) and C++ painting; `src/ui/widgets/uikit.cpp`: paint-only widgets (icons, avatars, T-score band, round progress). Design notes: `docs/design/2026-10-09-quiet-instrument-ui.md`.
- `resources/`: theme, measurement, report, and age-SOS images.

Tests and data:

- `test.ps1`: builds and runs every suite (`-Suite` selects): `tests/core_tests.cpp` (device protocol and measurement core, no GUI), `accountstore_tests`, `patientstore_tests`, `calibration_tests`, `mainwindow_safety_tests` (main-window integration and UI regressions, offscreen) and `portable_handoff_tests.ps1`.
- `testdata/age-sos-reference/`: anonymous age-SOS demonstration XML; `testdata/onset-consistency/`: recorded frame for the onset guard.

For measurement-pipeline invariants and change impact, load [architecture.md](architecture.md). For gate diagnosis, load [operations/debugging.md](operations/debugging.md). For calibration conclusions, load [domains/calibration.md](domains/calibration.md).

## Runtime Data

The application reads and writes in the data folder resolved by `DataLocation` (`BoneDensityData`, see above):

- `accounts.xml`: account, role, enabled state, random salt, and SHA-256 hash.
- `patients.xml`: one record per patient.
- `measurements.xml`: multiple records per patient with patient/result snapshots.
- `calibration.xml`: current, previous, default D, and calibration history.
- `settings/`, `backups/`, `experiments/`, `import-reports/`: device/guide settings, rolling backups, Debug experiment logs, legacy-import reports.

Patient ID is the history key; measurement records use independent UUIDs. Candidate data is saved before memory replacement. Migration and damage recovery preserve backups. Runtime XML may contain unencrypted operational or patient data, so tests use isolated directories or anonymous fixtures only.
