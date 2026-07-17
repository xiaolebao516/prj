# Measurement Architecture Reference

> **Authority:** Agent-only, task-triggered architecture map. Product meaning comes from [product-spec.md](product-spec.md); executable behavior comes from current source code. Human rationale in [measurement-pipeline-architecture-review.md](../../docs/research/measurement-pipeline-architecture-review.md) is non-normative.

Read this file only for architecture, measurement-pipeline, persistence, calibration, or cross-component work.

## Component Ownership

- `src/mainwindow.cpp`: serial intake, patient workflow, gate coordination, aggregation, persistence handoff, and UI/report transitions.
- `src/signalprocessor.cpp`: filtering, arrival detection, correlation-lag refinement, and per-channel SOS.
- `src/utils.cpp`: trimmed aggregation and candidate-round clustering.
- `src/bonehealth.cpp`: T/Z and derived bone-health results.
- Account, patient, measurement, and calibration stores own their XML persistence; `src/reportwidget.cpp` owns shared report rendering.

## Patient Measurement Data Flow

Serial waveform → signal processing → frame acceptance → lag-B stability lock → accepted-frame accumulation → per-round trimmed summary and quality gate → candidate-round clustering → final trimmed result → bone-health derivation → measurement snapshot persistence/report.

- Current patient SOS output is B-channel based; A participates in posture and quality diagnostics.
- Rejected frames and rounds do not enter the next aggregation level.
- Starting a new round resets round-local stability and diagnostic state before acquisition.

## Configuration and Change Impact

- Correlation, posture, and stability thresholds are centralized in `MeasureConfig` in `include/types.h`.
- Other acquisition cardinality and clustering controls are in `include/mainwindow.h` and `src/mainwindow.cpp`.
- Do not copy mutable numeric values into architecture decisions. Read current values from code.
- SOS, thresholds, stability, clustering, channel selection, acquisition timing, and probe-baseline changes cross the protected boundary in `AGENTS.md` and require explicit approval.

## Calibration Separation

Calibration reuses signal processing but has distinct acquisition, workflow, and persistence paths. Candidate values remain isolated from patient measurement until independent verification and operator confirmation. Phantom qualification, temperature, coupling, hardware accuracy, and patient impact remain laboratory acceptance.

## Persistence and Report Invariants

- Runtime XML resides beside the executable.
- Candidate-state writes must succeed before in-memory replacement.
- Backup/recovery behavior and measurement snapshots must be preserved.
- Automated tests must use isolated directories, never real executable data.
