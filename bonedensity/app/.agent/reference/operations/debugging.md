# Gate Diagnostics Reference

> **Authority:** Agent-only, default-unread engineering reference for measurement-gate diagnostics. It is subordinate to `AGENTS.md`, [product-spec.md](../product-spec.md), the active Loop contract, and current code. The [human guide](../../../docs/guides/gate-diagnostics.md) is an operating aid, not engineering authority.

## When to Load

Load only for patient-measurement gate diagnostics, accepted-frame failures, stability investigation, or related evidence review.

## Pipeline and Code Anchors

A patient frame passes pre-checks, correlation/posture gates, then lag-B stability before entering accepted-frame accumulation. Inspect current behavior at:

- `include/types.h`: `MeasureConfig` correlation, posture, and stability settings.
- `include/mainwindow.h`: acquisition cardinality and clustering declarations.
- `src/mainwindow.cpp`: gate evaluation, counter recording, diagnostic output, operator cues, round handling, and serial timing.
- `src/signalprocessor.cpp`: arrival and refined correlation-lag calculations.

Numeric values in code are executable authority. This reference intentionally does not duplicate them.

## Counter Semantics

- A rejection summary is a triage signal, not a root-cause verdict.
- Counters are not mutually exclusive.
- A stability category may be recorded after a preceding pre-check prevented a full stability evaluation.
- A high percentage supports investigation; it never authorizes relaxing a gate.
- D/G cues describe software-derived positioning signals only. They are not validated anatomical, clinical, hardware-fault, or calibration conclusions.

## Evidence Collection

1. Record the commit/worktree state, build identity, device/probe identifier, test medium, temperature source, coupling method, and operator action.
2. Capture the smallest reproducible output excerpt, including raw frame count and all emitted diagnostic lines.
3. Separate code-path inspection, actual software run, and supervised hardware/laboratory evidence.
4. Remove patient identifiers, credentials, and device-private data from logs or screenshots.
5. Compare current source values before interpreting any older report or guide.

## Protected Changes and Verification

Changes to gates, correlations, stability, SOS, timing, clustering, channel selection, or empirical parameters require explicit user approval and Standard workflow handling. Verification must map to the approved Loop SCs and product requirements, preserve accepted-frame meaning, and use real laboratory evidence for hardware claims. Faster progress or a single block/subject comparison is not correctness evidence.

Historical proposals and stale values are preserved only in [the archive](../../../docs/archive/gate-diagnostics-and-tuning-legacy.md); they are never approval evidence.
