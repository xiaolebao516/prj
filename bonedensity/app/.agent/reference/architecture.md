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

Serial waveform → signal processing → frame acceptance → lag-B stability lock → accepted-frame accumulation → per-round trimmed summary and quality gate → candidate-round clustering → exact-five final mean → bone-health derivation → measurement snapshot persistence/report.

- Current patient SOS output is B-channel based; A participates in posture and quality diagnostics.
- Rejected frames and rounds do not enter the next aggregation level.
- Starting a new round resets round-local stability and diagnostic state before acquisition.
- The formal/default profile uses the replay-validated relock stack: A quality is the lower of its front and middle correlation windows at the same selected lag, with the evaluated round A limit; frames failing G may update position-stability observation but can never count as valid values; after a partial round loses lock, saved values are retained only if a new strict-D-qualified cluster and every saved B-derived lag are all within two samples. A different cluster discards the old partial values before its current frame can count. B-peak boundary completion remains disabled.
- Accepted rounds 1-4 stop acquisition and schedule the existing guarded start path after one second; Space/button can start immediately and cancels the pending timer. Rejected rounds and round 5 do not auto-restart.
- `Utils::trimmedMeanValue` uses an arithmetic mean when fewer than 10 values are supplied. Thus a 30-value round uses its requested trimming, but the normal five-round final aggregation does not trim extremes; do not infer behavior solely from the helper name.
- Current source finalization selects exactly five indices from the accepted pool: smallest SOS range, then smallest absolute deviation from its median, then deterministic stable order. SOS/A/B share the indices; exactly-five input retains acquisition-order means. All candidates/pools remain intact and Debug `final_round_selection` records the source pool and selected indices. This is a final cardinality correction, not narrower frame gates or an accuracy claim. Verification: `tests/mainwindow_safety_tests.cpp` exact-five cases; existing deployed binaries may predate this source change.

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
