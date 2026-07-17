# BoneDensity Ultrasonic Bone Densitometer Agent Guide

## Project Map

- Project root: `D:/Repository/prj/bonedensity/app`
- Key implementation directories: `src/`, `include/`, `ui/`, `resources/`, `tests/`, and `testdata/`
- Product specification: `.agent/reference/product-spec.md`
- Code/data map: `.agent/reference/project-map.md`
- Architecture reference: `.agent/reference/architecture.md`
- Verification reference: `.agent/reference/validation.md`
- Domain and operation references: `.agent/reference/domains/` and `.agent/reference/operations/`
- Human documents: `docs/reports/`, `docs/research/`, `docs/guides/`, `docs/evidence/`, and `docs/archive/`
- Loop contract: `.agent/LOOP.md`
- Current execution state: `.agent/STATE.md`
- Completed task log: `.agent/LOG.md`
- Model recommendation policy: `.agent/MODEL_POLICY.md`
- Standard state-machine protocol: `.agent/STATE_MACHINE.md`
- Workflow skills: `.agents/skills/`

## Command Entry Points

| Action | Command |
| --- | --- |
| Build | `powershell -ExecutionPolicy Bypass -File ./build-debug.ps1` |
| Run | No unattended run command; after Build, manually launch `build/debug/debug/BoneDensity.exe` |
| Test | `powershell -ExecutionPolicy Bypass -File ./test-calibration.ps1` (calibration scope only) |
| Verify | No single command; use task-specific tests plus required human or laboratory acceptance |

## Golden Rules

1. Preserve user changes and avoid unrelated modifications.
2. Ask the user when a critical requirement has multiple reasonable interpretations.
3. Stay within the approved scope; do not change core requirements or weaken success criteria.
4. Treat compilation and test results as evidence, not as completion. Complete work against the Loop success criteria and observable behavior.
5. Diagnose ordinary implementation and execution problems autonomously.
6. Obtain user confirmation before high-impact decisions, public interface changes, irreversible operations, or changes to core algorithms, architecture, or dependencies.

## Workflow Selection

Before execution, recommend one workflow and load only its Skill. State the Recommended Workflow, Confidence, Triggered Conditions, Why not the other Workflow, and Whether user confirmation is required.

### Lite Basis Conditions

Recommend Lite only when all of the following hold: the requirement is clear; the work is local, low-risk, and easily reversible; it does not need formal plan approval, sustained state, external research, or complex real-operation verification; and it does not affect core algorithms, architecture, public interfaces, important technical parameters, or a formal delivery.

### Strong Triggers

Recommend Standard when any one of these conditions applies:

- formal plan approval is required;
- sustained state or cross-context recovery is required;
- external research or a repository-constraint audit will affect the approach;
- the work affects a core algorithm, architecture, public interface, or important technical parameter;
- rollback is difficult or the error cost is high;
- complex real-operation verification is required;
- the result affects an important experiment, report, or formal delivery;
- the root cause is unknown; or
- failures have repeated without a new diagnosis.

### Weak Signals

Do not let any one of these signals force Standard by itself:

- multiple files change, but the change is clear and low-coupling;
- minor ambiguity does not affect the core approach;
- some codebase understanding is needed; or
- the work has several clear, low-risk, reversible steps.

### Selection and Override

1. Any Strong Trigger recommends Standard.
2. With no Strong Trigger, recommend Standard only when multiple Weak Signals together materially increase planning, risk, or recovery cost.
3. Otherwise recommend Lite only if every Lite Basis Condition holds. Multiple files alone are not sufficient to require Standard.
4. When evidence is insufficient, make a low-confidence recommendation and ask the user to confirm the workflow before execution.
5. The user may select Lite or Standard explicitly. A Lite task that later needs Standard pauses for an approved upgrade; an established Standard task never automatically downgrades to Lite.

A workflow override never waives confirmation for high-impact or irreversible work, a material Goal / Scope / Success Criteria change, or a core algorithm, architecture, public interface, or important parameter change. It also never waives necessary real-operation verification or any other Golden Rule in this file or `.agent/LOOP.md`. When the user selects Lite while such a gate applies, explain the gate that remains in force.

Load `.agents/skills/workflow-lite/SKILL.md` only for Lite and `.agents/skills/workflow-standard/SKILL.md` only for Standard.

## Operating Context

After selecting a workflow, load only the files required by that workflow.

- **Inactive bootstrap:** `inactive` means no Standard Loop or recoverable Standard draft is active. Lite does not activate it; Standard initializes a fresh instance under `.agent/STATE_MACHINE.md`.
- **Lite:** Read `.agent/LOOP.md` only when the task belongs to that Loop, then load the Lite Skill. Lite does not normally read `.agent/STATE_MACHINE.md` or maintain `.agent/STATE.md`; an independent Lite task has no state-machine context cost.
- **Standard:** Read in this order: `.agent/LOOP.md` → `.agent/STATE_MACHINE.md` → `.agent/STATE.md` → Standard Skill. The state-machine protocol defines Standard stages, legal transitions, recovery, and State update requirements; the Skill executes the current stage.
- **Lite upgrade:** Load the state-machine protocol and initialize the relevant Standard State only after the user approves the upgrade.

Read `.agent/MODEL_POLICY.md` at its defined recommendation checkpoints. It recommends capability and reasoning effort only; it never switches a model automatically.

Use `.agent/LOG.md` only for completed-task records as defined there.

After the workflow control files are loaded, read Agent references only by the precise task route below; never recursively load `.agent/reference/` or `docs/`:

- Product behavior, requirement status, backlog, or acceptance boundary → `.agent/reference/product-spec.md`
- Code/data ownership or repository navigation → `.agent/reference/project-map.md`
- Measurement architecture, persistence invariants, or cross-component impact → `.agent/reference/architecture.md`
- Durable verification evidence or reproduction entry points → `.agent/reference/validation.md`
- Calibration implementation or claims → `.agent/reference/domains/calibration.md`
- Gate-diagnostic investigation → `.agent/reference/operations/debugging.md`

Human documents under `docs/` are not default Agent context. Read an exact human file only when the task needs its research rationale, report, operating guide, evidence, or history. Do not store current task progress or extensive project knowledge in this file.

## Project Documentation Ownership

- `.agent/reference/product-spec.md`: passive source for long-term product requirements, durable product facts, backlog, evidence entry points, and external-acceptance boundaries. It does not define workflow or current task state.
- `.agent/reference/`: concise, durable Agent facts and task-triggered technical references. Files must declare authority and must not duplicate long-form human rationale.
- `.agent/LOOP.md`: active Standard Loop contract. Reference applicable `REQ-*` entries without copying or weakening them.
- `.agent/STATE.md`: current Standard plan, progress, judgment, and verification state as required by `.agent/STATE_MACHINE.md`; inactive state contains no fabricated task.
- `.agent/LOG.md`: minimal completed-task records according to its logging policy.
- `.agent/work/<loop-id>/`: optional temporary working material for one Loop. Route durable results and remove the Loop directory before delivery.
- `docs/reports/project-progress.md`: formal concise weekly report for mentors and project members. Record only verified results; label unaccepted work explicitly.
- `docs/research/`: human rationale, source analysis, and exploratory review; non-normative unless a canonical Agent reference explicitly adopts a settled conclusion.
- `docs/guides/`: human operating aids; they do not authorize algorithm, parameter, product, or acceptance changes.
- `docs/evidence/`: minimal reproducible human-facing evidence when a task actually needs it.
- `docs/archive/`: historical, superseded, or unsafe material retained for traceability and explicitly non-authoritative.
- `docs/param_tuning_analysis.md`: protected pre-existing human research exception. It remains in place, is default-unread, and must not be modified, moved, deleted, formatted, staged, or treated as parameter authority without new explicit user approval.
- Framework installation or migration never becomes a Loop instance and is not recorded in `.agent/LOOP.md`, `.agent/STATE.md`, or `.agent/LOG.md`.

For durable research: define the decision question, prefer primary/official sources, keep source links and reasoning in one focused `docs/research/` file, and distill only settled operational conclusions into the exact Agent reference. Label assumptions, conflicting evidence, and external-verification limits; do not copy the same long-form analysis into both layers.

## Fixed Toolchain and Commands

- Use only Qt 6.5.3 at `D:\\Qt\\6.5.3\\mingw_64` with MinGW 11.2 at `D:\\Qt\\Tools\\mingw1120_64` for command-line builds.
- Do not invoke an unqualified system `qmake`, `mingw32-make`, or `g++`. Mixing the system MSYS2 toolchain with the Qt runtime can produce incompatible binaries.
- `build-debug.ps1` is the canonical clean Debug build and deployment entry point.
- `test-calibration.ps1` is a focused calibration suite, not a whole-project test or final acceptance command.
- Qt Creator may open `BoneDensity.pro` for interactive runs. Hardware acquisition, physical printing, and qualified-phantom calibration require supervised real-operation checks.

## Protected Engineering Boundaries

Obtain explicit user approval before changing any of the following:

- measurement algorithms or constants in `src/signalprocessor.cpp`, `src/bonehealth.cpp`, or `src/utils.cpp`;
- SOS calculation, gating thresholds, empirical parameters, correlation or stability rules, the active probe baseline D, or channel-selection policy;
- serial framing, 115200-baud configuration, acquisition commands, or the 80 ms acquisition timing;
- persistent data schemas, migration semantics, authentication behavior, architecture, public interfaces, or dependencies.

Treat `src/mainwindow.cpp` as a high-coupling orchestration hotspot. Make localized changes and do not refactor it merely for tidiness.

## Data and Safety Rules

- Runtime `accounts.xml`, `patients.xml`, `measurements.xml`, and `calibration.xml` live beside the executable and may contain real operational or patient data.
- Never use a real executable directory for automated tests. Use an isolated temporary directory or committed anonymous fixtures under `testdata/`.
- Preserve atomic-write, backup, corruption-recovery, and historical-patient-snapshot behavior when touching storage.
- Do not claim medical, metrological, DICOM, hospital-system, printer, USB, or hardware acceptance beyond the evidence actually obtained.

## Verification Expectations

- Map verification to the applicable `REQ-*` or Loop `SC-*`; a successful build proves only compilation and deployment.
- Run `test-calibration.ps1` for calibration changes, plus the canonical Debug build when the application is affected.
- Use anonymous fixtures for age-SOS and persistence demonstrations. Keep visual evidence minimal and reproducible.
- Store reusable boundary fixtures under `testdata/<feature>/`; keep only a few decisive visual artifacts under `docs/evidence/<date>-<feature>/`. Record case-to-operation mappings in the active verification plan or delivery evidence, not in parallel progress files.
- Mark device acquisition, five-round hardware flow, USB hot-unplug/reconnection, physical print output, coupling repeatability, temperature truth, and qualified-phantom accuracy as external acceptance unless they were actually exercised.
- For the same stable failure, allow at most five meaningful repair iterations as defined by the state-machine protocol.

## Git and Scope Discipline

- Start by inspecting `git status --short --branch`; preserve pre-existing modified and untracked files.
- Do not discard, overwrite, stage, or include unrelated user work.
- Do not commit, push, merge, rebase, or modify remote state unless the user explicitly authorizes it.
- Before delivery, run `git diff --check`, inspect the path-level diff, and report which checks were actually run and which acceptance remains external.
