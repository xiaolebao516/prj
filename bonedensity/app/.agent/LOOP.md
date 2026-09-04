# Loop Contract

## Status

`active`

MEASUREMENT-FEEDBACK-001 is the active Standard Loop in this repository.

## Goal

`Make measurement-process feedback consistently understandable and ensure semantically invalid patient/history XML cannot be loaded and later overwrite recoverable data, while preserving measurement behavior and valid data compatibility.`

## Boundary / Scope

- Keep the original progress bars, center lines, positions, value bindings, titles, and measurement behavior unchanged.
- Use the existing blank space for guidance: the emphasized real-time label states that the left bar is used while adjusting probe tilt and the right bar while adjusting probe position; a smaller but readable one-line note below states that both should ultimately be brought to their center lines.
- Update only the emphasized label from consecutive existing bar positions to show waiting, approaching center, moving away and reverse, holding at center, or invalid signal.
- Repair the clipped bottom status label without moving or resizing the three existing progress bars.
- Keep the current overall round and current-round valid-frame progress visible during formal patient measurement.
- Replace user-facing `Gap`, `lag`, `Corr`, pair/debug, and Qt Creator wording with concise operator-facing valid, adjust-pose, stabilize-probe, or improve-contact guidance.
- A rejected round must clearly say it was not counted; an accepted round must clearly say which of five rounds completed.
- Do not add timers, move controls, change bar mapping, create new measurement states, or alter measurement acceptance behavior.
- Preserve SOS calculation, lag calculation and types, D/G formulas, measurement gates and thresholds, acquisition timing, persistence, schemas, dependencies, and final-result behavior.
- Connect the existing patient/history document validators to the normal load path so unsupported versions, empty or duplicate IDs, and orphan measurements are backed up and rejected before any output list is replaced.
- Preserve the current XML schemas, valid version-2 load/save behavior, versionless legacy migration, transaction recovery, atomic writes, backup naming, and caller-owned output lists on failure.
- Preserve unrelated worktree changes and the protected `docs/param_tuning_analysis.md` file; do not commit or push without explicit authorization.

## Success Criteria

- `SC-1`: The original progress bars, center lines, geometry, value bindings, titles, measurement gates, accepted-frame count, five-round behavior, and final results remain unchanged.
- `SC-2`: The emphasized real-time explanation states left-bar tilt adjustment and right-bar position adjustment; a smaller readable one-line note below states the final goal of bringing both bars to their center lines.
- `SC-3`: Consecutive existing bar positions produce immediate approaching, moving-away/reverse, centered, small-change, waiting, and invalid-signal text without changing bar values.
- `SC-4`: The bottom measurement status is fully inside its parent, keeps round/frame progress visible, contains no developer-only terminology, and distinguishes valid, rejected, invalid-signal, accepted-round, and rejected-round states.
- `SC-5`: The canonical Debug build, focused and existing safety tests, screenshot inspection, and final diff checks pass with no protected or unrelated product change.
- `SC-6`: The user visually confirms the wording, placement, and real-device response in the normal application window.
- `SC-7`: All seven existing semantic-corruption cases fail closed, preserve the source bytes, create the expected `.bad.bak`, report its path, and leave caller-owned patient/measurement lists unchanged.
- `SC-8`: Valid version-2 data, versionless legacy migration, paired save/delete, and pending-transaction recovery continue to pass without an XML schema change.
- `SC-9`: Patient-store, account, calibration, main-window, portable-handoff, canonical Debug build, and final diff checks pass after the persistence repair, except for explicitly external hardware acceptance.

## SOP

1. Preserve the original progress bars and D/G guidance while fitting the bottom status into its existing parent.
2. Format the existing measurement states as one concise line containing overall round, current valid-frame progress, and an operator-facing status.
3. Replace misleading rejected-round completion wording and developer-only runtime text without changing any decision condition.
4. Add focused regression checks for visibility, wording, progress, rejected-round behavior, trend wording, and unchanged bar geometry.
5. Run the existing safety suite, inspect the rendered page, build the canonical Debug executable, and confirm the final diff scope.
6. Route normal patient/history loading through the existing semantic validators before converting documents into records.
7. Run focused patient-store tests first, then the cross-feature regression and canonical build.

## Stop and Escalation Conditions

- Stop before changing SOS, lag estimation, D/G formulas, thresholds, acquisition timing, or accepted-frame behavior.
- Stop if any existing progress-bar geometry, mapping, title, gate, accepted-frame count, five-round behavior, or algorithm differs from the pre-task implementation.
- Do not infer new D/G semantics or alter the existing left/right bindings in this task.
- Stop before changing an XML schema, valid-record interpretation, legacy migration semantics, transaction protocol, or backup/atomic-write behavior beyond rejecting the already enumerated invalid documents.

## Learning and Evolution

The regression was caused by coupling guidance work to progress-bar layout and behavior. The durable prevention is a focused test that locks the original bar geometry while exercising only the separate guidance label; no broader Skill or system rule is needed.
