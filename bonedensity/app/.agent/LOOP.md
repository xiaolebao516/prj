# Loop Contract

## Status

`active`

MEASUREMENT-FEEDBACK-001 is the active Standard Loop in this repository.

## Goal

`Complete the approved operation tutorial and, following the 2026-09-05 algorithm audit, repair demonstrated stale/mixed patient-round state and record experiments automatically before evaluating threshold changes. Preserve next-round controls, valid data compatibility, and semantic patient/history validation.`

## Boundary / Scope

2026-09-06 approved dual-window trial exception: user confirmed “好的” after the exact proposal to use the minimum of A valley correlations over [-20,+30] and [0,+60] at the existing selected A lag, with frame A0.78 and round A0.78. Implement only a distinct isolated Debug build retaining observe-before-G. Fallback scoring, SOS/lag/D/G, B checks, stability/expiry, serial timing, and default/previous builds remain unchanged. This supersedes preservation clauses only for this approved experimental quality definition and round threshold. Recalled3900 is not a fitting target.

2026-09-06 approved isolated trial exception: after the precise observe-before-G proposal and its replay limitations were presented, the user answered “只要能够帮助实现我的目标就行了”. Implement only an explicitly marked independent experimental build: original basic quality/D checks may feed stability observation before G, while actual accepted frames still require every original precheck including G. Preserve all numerical thresholds/formulas, default build behavior, bars and serial timing. G-only failures may maintain observation; sustained basic-signal failure and cluster loss must still expire/discard partial state. Keep the original deployed executable/data untouched. Build and deploy this experimental variant in an isolated directory using the fixed Qt/MinGW toolchain rather than overwriting the canonical original directory. Software equivalence/invariants are acceptance for the artifact, not evidence of improved hardware accuracy or completion time.

2026-09-05 approved extension: the user approved checking principles/code first, fixing demonstrated problems before tuning, and testing on himself before other subjects. The following narrow exceptions supersede earlier tutorial-only preservation clauses: expire patient stability/partial-round state after the existing unlock-count number of consecutive rejected frames; discard partial-round values when the existing stability lock is lost; indicate unavailable bar readings without moving/remapping the controls; add bounded, non-identifying Debug experiment recording of complete input frames, decisions and round events. Preserve transient tolerance. Numerical thresholds, SOS/lag/D/G formulas, calibration, serial framing/timing and clinical persistence remain unchanged. Easier positioning and acceptable error are external self-trial criteria, not claims established by software tests.

- Keep the original progress bars, center lines, positions, value bindings, numerical values, and measurement behavior unchanged.
- Remove the per-frame approaching/moving-away/invalid-signal instruction text and all other operator sentences that change on every incoming frame.
- Keep a fixed reminder on the process page: adjust the right `D` bar for probe position, then the left `G` bar for probe tilt, and ultimately stabilize both at their center lines.
- Keep the current overall round and current-round valid-frame progress visible. Round completion and rejection remain clear, but status text changes only at round-level events.
- Add an idle-only `操作教学` entry and a modal, responsive three-page tutorial: preparation/overview, right-D position adjustment, and left-G tilt adjustment plus round completion.
- Use the approved first-version abstract diagrams as replaceable temporary assets. Keep text outside the images so later GPT-generated or photographed replacements do not require interaction changes.
- Before the first eligible measurement in a copied application folder, show the tutorial. `跳过教程` or the final acknowledgement marks the current tutorial version as seen and continues measurement; closing the dialog cancels without marking it seen or starting acquisition.
- A manually opened tutorial never starts acquisition when it closes.
- Store only a versioned tutorial-seen marker beside the executable, separate from patient, measurement, account, and calibration XML so copying the application folder preserves the setting.
- Preserve the proven next-round behavior: after a completed round, Space activates the focused `开始检测` button, and clicking that same button also continues. Do not treat arbitrary page clicks as confirmation.
- A rejected round must clearly say it was not counted; an accepted round must clearly say which of five rounds completed.
- Do not add timers, move progress controls, change bar mapping, create new measurement states, or alter measurement acceptance behavior.
- Preserve SOS calculation, lag calculation and types, D/G formulas, measurement gates and thresholds, acquisition timing, persistence, schemas, dependencies, and final-result behavior.
- Connect the existing patient/history document validators to the normal load path so unsupported versions, empty or duplicate IDs, and orphan measurements are backed up and rejected before any output list is replaced.
- Preserve the current XML schemas, valid version-2 load/save behavior, versionless legacy migration, transaction recovery, atomic writes, backup naming, and caller-owned output lists on failure.
- Preserve unrelated worktree changes and the protected `docs/param_tuning_analysis.md` file; do not commit or push without explicit authorization.

## Success Criteria

- `SC-1`: The original D/G progress bars, center lines, geometry, value bindings, measurement gates, accepted-frame count, five-round behavior, acquisition timing, and final results remain unchanged.
- `SC-2`: No instruction sentence changes on each incoming frame; the fixed right-D/left-G reminder, current round, valid-frame progress, accepted round, and rejected round remain readable.
- `SC-3`: The responsive tutorial contains exactly the three approved pages and can be opened manually while idle without starting acquisition.
- `SC-4`: The first eligible measurement shows the current tutorial version once; skip, finish, close, reopen, and version behavior follow the approved contract without modifying clinical XML.
- `SC-5`: The temporary abstract diagrams are clear, text-free, and replaceable later without changing tutorial control flow.
- `SC-6`: After a completed round, Space or clicking `开始检测` continues; closing or clicking elsewhere does not start acquisition.
- `SC-7`: Focused tests, all existing regressions, canonical Debug build, screenshot inspection, and final diff checks pass with no protected algorithm or unrelated product change.
- `SC-8`: The user visually confirms the tutorial wording, layout, temporary diagrams, and next-round behavior in the normal application window; physical probe usability remains real-device acceptance.
- `SC-9`: All seven existing semantic-corruption cases fail closed, preserve the source bytes, create the expected `.bad.bak`, report its path, and leave caller-owned patient/measurement lists unchanged.
- `SC-10`: Valid version-2 data, versionless legacy migration, paired save/delete, and pending-transaction recovery continue to pass without an XML schema change.
- `SC-11`: Patient-store, account, calibration, main-window, portable-handoff, canonical Debug build, and final diff checks pass after the tutorial change, except for explicitly external hardware acceptance.
- `SC-12`: A sustained rejected-frame gap cannot reuse a stale patient stability lock/window; fewer consecutive rejections than the existing unlock count preserve partial progress. Lock loss clears all seven partial-round lists and progress without stopping acquisition or mixing clusters. Uninterrupted valid input retains its acceptance sequence.
- `SC-13`: Debug measurements automatically record complete-frame raw channels, monotonic timing, actual feature branch/values, rejection/acceptance decisions and round boundaries in unique bounded files, without patient identifiers or clinical XML changes. Recording failure is visible and cannot change measurement acceptance; Release does not enable this automatically.
- `SC-14`: Focused regression and canonical build pass. Hardware self-trial must assess positioning effort, stalls and paired/repeated SOS before threshold changes or claims of accuracy/usability improvement; subsequent other-subject verification remains pending.
- `SC-15`: Default flow reproduces the original raw baseline. Experimental flow observes G-only failures without accepting them, preserves loss/cluster clearing, and matches the approved research candidate on all eight raw recordings. All numerical parameters, formulas and bar mappings remain unchanged.
- `SC-16`: An independently built/deployed trial artifact is visibly marked and logs its distinct flow version. Original executable, input logs and real clinical XML remain untouched; automated checks run only in isolated directories. Actual self-trial benefit remains external and unproven.

- `SC-17`: Actual dual-window raw pipeline matches the independent cached candidate on all nine recordings, including unchanged SOS/lag/D/G features. Both windows must pass at the same existing lag; invalid windows fail closed. Default raw baseline and regression remain unchanged.
- `SC-18`: Independent marked dual-window0.78 build and recorded profile are verified; original binaries and input hashes are preserved. Hardware positioning time, repeatability and paired accuracy remain external, not established by replay.

## SOP

Approved extension sequence: add regression reproductions; repair stale/mixed state locally; add automatic Debug-only full-path experiment recording with bounded storage and no patient identifiers; verify steady-input behavior, transient tolerance, isolation and recording failures; build; then self-trial before selecting any tuning package. Do not deploy an unvalidated wider threshold package.

1. Remove per-frame instruction updates and replace them with the approved fixed reminder without changing any bar or measurement input.
2. Add the separate three-page tutorial, temporary abstract resources, first-use marker, manual entry, and approved skip/finish/close behavior.
3. Restore focus to `开始检测` after an accepted round and lock the existing Space/click continuation behavior with focused tests.
4. Add focused regression checks for tutorial content, first-use versioning, manual open, non-starting close, stable text, unchanged bars, and layout.
5. Run the existing safety and persistence suites, inspect the rendered pages, build the canonical Debug executable, and confirm the final diff scope.

## Stop and Escalation Conditions

- Stop before changing SOS, lag estimation, D/G formulas, thresholds, acquisition timing, or accepted-frame behavior.
- Stop if any existing progress-bar geometry, mapping, gate, accepted-frame count, five-round behavior, acquisition timing, or algorithm differs from the pre-task implementation.
- Use only the user-approved physical teaching mapping `right D = position; left G = tilt`; do not infer absolute movement directions.
- Stop before embedding generated or photographed final assets without a separate user review of those images.
- Stop before changing an XML schema, valid-record interpretation, legacy migration semantics, transaction protocol, or backup/atomic-write behavior beyond rejecting the already enumerated invalid documents.

## Learning and Evolution

The rejected approach changed text at frame rate even though a human operator could not read it. The durable prevention is to verify instructional usability at the real update rate and to keep per-frame feedback visual while text changes only at operator-readable state boundaries. This remains a focused product/test rule; no broader Skill or system rule is needed.
