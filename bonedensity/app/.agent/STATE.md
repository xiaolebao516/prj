# Current Loop State

## Workflow

`standard`

## Stage

`verification`

## Status

`Approved state repairs and automatic Debug experiment capture are implemented and software-verified. Canonical Debug executable is built. External self-trial and tutorial visual acceptance remain pending; no wider numerical thresholds or core signal-formula changes were deployed.`

## Active Milestone

`MEASUREMENT-FEEDBACK-001 - Complete operator-facing measurement feedback`

## Current Task

`Await the user's initial self-trial and paired reference readings for SC-14. Analyze automatic recordings before selecting tuning candidates; preserve separately pending tutorial acceptance.`

## Model Recommendation

- Model Capability: `DEEP`
- Reasoning Effort: `high`
- Switch Benefit: `unknown`
- Reason: the new task concerns uncertain measurement gates, inconsistent feature definitions, and scientific validity of single-subject data.

## Plan Status

`approved`

Approval evidence: after the repository trace showed that the existing bottom status was clipped and rapidly overwritten, the user replied “好的” to the proposed four-part implementation: repair visibility, keep round/frame progress visible, replace developer wording, and preserve the D/G bars and measurement logic.

Persistence approval evidence: after the seven failing semantic-corruption cases and their effect were explained, the user explicitly requested “那请你修复”.

Revision evidence: the user reported that per-frame text changes too quickly to read and requested a tutorial replacement. The user has requested discussion before design or implementation, so the revised contract and plan are not yet approved.

Replacement approval evidence: after reviewing the interactive UI and abstract diagram storyboards, the user said “可以先拿这个替代……可以先按这个做一版”. This approves the reviewed UI and temporary abstract assets for a first implementation, while reserving final generated or photographed image replacement for later review.

## Plan and Steps

Approved extension (2026-09-05): user said “根据原理，看代码有没有明显问题……通过调参尝试改善，首先是我自己测试”. This continues the presented code-repair/automatic-recording-first proposal; numerical tuning and cross-subject improvement are not yet validated.

A. Reproduce stale-state, mixed-cluster and transient/steady controls with focused tests. `three tests failed before repair; passed after repair`
B. Clear all partial-round state on existing lock loss; expire after the existing unlock count of consecutive precheck failures, and mark unavailable readings without changing bar geometry/mapping. `implemented; regression passed`
C. Record bounded Debug-only raw complete frames and actual decisions/events automatically; exclude identity and keep failures outside the measurement decision path. `implemented; early-return, raw encoding, uniqueness, cap and failure-isolation checks passed`
D. Run focused regressions, canonical build and scope review, then hand off self-trial without claiming accuracy or easier positioning. `software checks completed; external self-trial pending. Synthetic fixture was corrected to finite packet support; no production parameter changed for that fixture issue`

1. Align exactly three tutorial pages and their wording. `completed and approved`
2. Confirm the physical teaching sequence `right D = position; left G = tilt` before finalizing diagrams. `completed`
3. Remove per-frame trend/suggestion text while preserving bars, numeric values, effective-value progress, round status, gates, and results. `completed`
4. Add an idle-only `操作教学` entry and a tutorial shown before the first eligible measurement. `completed`
5. Store only a versioned tutorial-seen marker beside the executable; do not change clinical XML. `completed`
6. Preserve the existing next-round behavior: Space activates the focused `开始检测` button, and the same button can be clicked with the mouse. Explicitly restore/verify focus rather than adding a global shortcut or arbitrary-click behavior. `completed`
7. Implement the approved temporary abstract diagrams as replaceable, text-free assets; do not generate or embed final images yet. `completed`
8. Add focused first-use, reopen, skip, cancel, focus/Space, non-flickering text, unchanged-bar, and layout tests. `completed`
9. Run existing regressions, the canonical Debug build, screenshot inspection, and real-device usability acceptance. `software verification completed; user visual and real-device acceptance pending`

## Progress

- 2026-09-05 extension: three new state regression cases failed on the pre-repair code, then passed after localized repair. Brief rejected-frame sequences preserve partial progress; sustained rejection at the existing unlock count clears stale state. Existing cluster loss clears all seven partial-round lists without stopping acquisition or deleting completed rounds.
- Automatic Debug round logs now capture raw four-channel complete frames, actual feature branch/points, per-gate decisions (including unevaluated stability), partial-discard events, round summaries, and stop/disconnection boundaries. Release does not enable logging automatically. Files are unique, locally bounded, excluded from Git, and contain no patient identifiers. Details and limits: `docs/research/measurement-gate-audit-20260905/repair-validation.md`.
- New verification: complete Debug main-window suite `43 passed / 0 failed / 1 screenshot test skipped`; Release state/default-recording checks `6 passed / 0 failed`. Actual synthetic processing produces the expected 43-input / 30-valid round with the final frame recorded before the stop event. A 500-frame four-channel recording microbenchmark took 69 ms on this local disk (not a hardware performance claim).
- Canonical `build-debug.ps1` succeeded, producing `build/debug/debug/BoneDensity.exe`. `git diff --check` passed (line-ending notices only). `src/signalprocessor.cpp`, `src/utils.cpp`, `src/bonehealth.cpp`, `include/types.h`, and `include/signalprocessor.h` still have no diff. Existing tutorial changes and unrelated parent backup/zip remain intact; no commit/push authorization for this increment.
- The existing isolated calibration executable was rerun against unchanged calibration/signal-processing sources: `16 passed / 0 failed`. This is software regression evidence, not phantom/hardware calibration acceptance.

- The user clarified that the original bars must remain untouched while the separate explanation area provides both static instructions and live trend feedback.
- The emphasized real-time label states left-side tilt and right-side position and updates after each existing process-panel frame; a smaller gray line below states that the final goal is for both bars to reach their center lines.
- The live label uses orange for waiting or small changes, green for approaching/centered, and red for moving away/reverse so the actionable prompt remains visually prominent.
- Trend text compares only each bar's current pixel distance from its existing center with the previous frame: approaching, moving away/reverse, centered, or small change.
- No timer, bar mapping, title/value binding, measurement gate, accepted-frame rule, or algorithm was changed; the approved work changes only guidance/status presentation.
- The focused test passes and locks both original bar geometries while covering approaching, moving away/reverse, centered, invalid signal, prompt emphasis, status colors, the final note, and non-overlap with both bars.
- The full safety suite passes with `33 passed, 0 failed, 0 skipped`; rendered-page inspection shows the upper age-SOS area restored and both text areas confined to the blank space between bars.
- The canonical Debug build passed and deployed `build/debug/debug/BoneDensity.exe`; `git diff --check` passed.
- Final scope review confirms no diff in protected algorithm/type files and no diff to the existing progress-bar definitions or `setValue` bindings; the only added bar-adjacent action reads `gBar`/`dBar` for guidance text.
- The clipped bottom status was reduced to one visible line inside `widgetBalanceArea`; it now keeps `第 n/5 轮` and `本轮有效值 n/30` visible and uses only operator-facing valid, pose, stability, contact, or signal guidance.
- A rejected round now says it was not counted in both the bottom status and the non-modal tip; accepted-round logic is unchanged.
- Measurement-feedback focused tests passed (`4 passed`), the complete main-window suite passed (`34 passed` when screenshot capture is enabled), and the canonical Debug build completed.
- Additional isolated regression initially exposed seven accepted semantic-corruption documents because the existing patient/history validators were not called by the normal load path.
- The approved persistence repair now routes parsed patient and measurement documents through those existing validators before replacing caller-owned data. Unsupported versions, empty or duplicate IDs, and orphan measurements therefore use the existing bad-file error and `.bad.bak` recovery path.
- The persistence repair changes no XML schema, migration rule, or valid-data representation. The complete patient-store suite now passes (`23 passed, 0 failed, 0 skipped`), including valid data, versionless legacy migration, paired save/delete, and transaction recovery.
- Final automated regression passed: main-window safety (`34 passed`), account-store, calibration (`16 passed`), portable handoff, the canonical Debug build, `git diff --check`, and protected algorithm/bar-binding scope checks.
- New repository audit confirms that the visible and bound bars are left `G` and right `D`.
- There is no explicit Space-key handler. The user's real-operation evidence is consistent with Qt's focused-button behavior: Space activates the still-focused `开始检测` button after a round. The revision should preserve and test that behavior rather than replace it.
- The repository contains a distal-radius anatomy image but no probe-operation illustration, so new text-free instructional assets are required after the physical mapping is accepted.
- The user explicitly confirmed the teaching sequence `right D = position; left G = tilt`.
- A review-only interactive prototype now exists at `.agent/work/measurement-guide-002/measurement-guide-ui-preview.html`; it contains the three tutorial pages, first-use/manual button states, and text-free geometric storyboards without changing product code.
- The prototype was wrapped successfully for preview and its JavaScript syntax check passed.
- The approved first version is implemented as a separate three-page Qt dialog. It uses code-drawn, text-free abstract illustrations and checks optional fixed image resource paths first, so approved generated images or photographs can later replace the drawings without changing the tutorial interaction.
- The measurement page now contains only the fixed `right D = position; left G = tilt` instruction, a small final-center-line reminder, and an idle-only `操作教学` button. The original bar geometries and `barPairA`/`barPairB` value assignments are unchanged.
- The first eligible measurement opens the tutorial once per tutorial version. Automatic skip/final acknowledgement writes `measurement-guide.ini` beside the executable with `QSaveFile`; closing cancels without writing or starting. A manually opened tutorial never starts acquisition.
- Focus is restored to the existing `开始检测` button after tutorial and round-tip transitions. The existing focused-button Space behavior continues round 2, while no global shortcut or arbitrary-click continuation was introduced.
- Qt-rendered screenshots of all three tutorial pages were inspected. Controls, page indicators, copy, and illustration areas do not overlap; the isolated offscreen environment lacks a Chinese font, so wording still requires confirmation in the normal deployed application.
- Final automated verification passed: main-window safety `35 passed, 0 failed, 1 environment-gated screenshot test skipped`; account store `6 passed`; patient store `23 passed`; calibration `16 passed`; portable handoff passed; and the canonical Debug build produced `build/debug/debug/BoneDensity.exe`.
- `git diff --check` passed apart from line-ending notices. Protected algorithm/type files have no diff, and no persistent clinical schema, serial timing, threshold, dependency, or measurement formula was changed.

## Current Judgment

2026-09-05 continuation approval authorizes the narrow state repairs and experiment capture described above. Earlier statements below that no production repair is approved are historical audit context and are superseded only for this extension. No wider D/G/correlation threshold, feature redefinition, or SOS formula change is selected. First self-trial is the user's chosen acceptance order; the user need not diagnose rejection causes or manually time acquisition.

On 2026-09-05 the user requested evidence-backed algorithm improvements and provided an algorithm DOCX plus six CSV attachments (five unique hashes). They explicitly confirmed these are only one subject at different angles, with no paired reference readings or hard-subject logs. The existing tutorial contract and approved plan remain applicable only to that earlier work; the new request authorizes inspection, offline analysis, and diagnostic reproduction, not an invented final threshold package.

The audit found: stale candidate/bar state across invalid input; retained accepted values across cluster changes, demonstrated by existing C++ accepting a 3791.67 round formed from 4083.33 and 3500 clusters; asymmetric envelope/valley G definitions; and speed-dependent correlation-pick bias on exactly delayed identical synthetic waveforms. Historical CSV sensitivity shows that increasing G tolerance increases both reference-region and high-SOS rows. The 3500-named file contains 6058 mixed-mode/mixed-output rows and cannot serve as a homogeneous label or continuous replay. These are reproducible code/data findings, not evidence of improved clinical accuracy or the specific cause on an unrecorded subject.

Evidence and concrete next-change recommendations: `docs/research/measurement-gate-audit-20260905/README.md`, `results.json`, and `code-results.json`. Product source files and the deployed executable were not modified by this audit. Continue by specifying full-path experiment recording and stale/mixed-state repair, then comparing consistent-feature and local-correlation candidates against raw recordings. Exact threshold selection and hard-subject acceptance need new paired recordings. Do not overwrite the existing tutorial milestone or mark it accepted on the basis of this audit.

## Verification Status

| Criterion | Status | Result / evidence |
| --- | --- | --- |
| `SC-1` | passed | Original bar geometry/mapping and numerical formulas/thresholds/timing remain unchanged. Approved exception: patient stale/mixed state and unavailable-reading indication are verified under SC-12, not claimed behavior-identical. |
| `SC-2` | passed | Stable fixed instruction and round-level status remain unchanged across valid and invalid incoming frames; effective-frame progress still advances. |
| `SC-3` | passed | Tests verify exactly three pages and a separate manual dialog; the entry is disabled during acquisition/debug and manual close cannot start acquisition. |
| `SC-4` | passed | Isolated tests cover current-version marker write/read, automatic skip, cancel-without-marker, and start/no-start behavior without clinical XML changes. |
| `SC-5` | passed | Qt screenshots verify text-free temporary drawings; optional page-specific resource paths allow later image replacement without flow changes. |
| `SC-6` | passed | A focused test starts round 2 with Space on the existing button; no global shortcut or page-click handler was added. |
| `SC-7` | passed | All focused/existing suites, canonical Debug build, Qt screenshot inspection, `git diff --check`, and protected-file scope checks passed. |
| `SC-8` | pending | Requires user visual acceptance and later real-device usability confirmation. |
| `SC-9` | passed | Existing patient-store evidence covers all seven semantic-corruption cases and source/list preservation. |
| `SC-10` | passed | Existing patient-store evidence covers valid version 2, legacy migration, paired operations, and transaction recovery. |
| `SC-11` | passed | Main-window 35, patient 23, account 6, calibration 16, portable-handoff, and canonical Debug build all passed. |
| `SC-12` | passed | Three before-failing/after-passing cases plus actual invalid-wave pipeline; all seven lists clear on loss, brief dropout survives, uninterrupted 43-input/30-valid sequence preserved. |
| `SC-13` | passed | Early failures, actual features, full round order, raw encoding, unique paths, cap/failure isolation and automatic Debug/disabled Release verified. Incomplete serial packets and hardware/storage truth remain documented limits. |
| `SC-14` | pending | Debug 43 passed, Release 6 passed and canonical build passed; external self-trial with paired/repeated SOS remains required before improvement claims or threshold choice. |

## Next Actions

`User opens build/debug/debug/BoneDensity.exe, measures himself normally, then supplies measurement-experiments plus contemporary sample-machine SOS readings. No manual timing or rejection diagnosis is required. Compare new evidence before selecting tuning; do not mark hardware accuracy, positioning tolerance, or tutorial visual acceptance complete.`

## Blockers

`none`
