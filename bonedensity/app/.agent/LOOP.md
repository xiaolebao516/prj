# Loop Contract

## Status

`active`

AGE-SOS-DEMO-001 is the active Standard Loop in this repository.

## Goal

`Prepare the exact Qt Creator runtime used by the user with clearly marked anonymous age-SOS demonstration archives, and verify the records and key scenarios in the actual application before requesting user acceptance.`

## Boundary / Scope

- Diagnose the runtime-directory mismatch shown by the user's screenshot.
- Preserve the existing `ID=1` archive and create recoverable backups before changing runtime XML.
- Add only the anonymous age-SOS demonstration patients and measurements to the exact runtime directory used by the shown application.
- Verify actual application behavior through the GUI; do not change algorithms, schemas, product code, dependencies, or protected engineering parameters.
- Correct the formal report if its reproduction path is inaccurate.
- Preserve unrelated user changes, `docs/param_tuning_analysis.md`, and `tmp/`; do not commit or push.

## Success Criteria

- `SC-1`: The runtime directory is identified from matching observable archive data, and its executable reads XML beside that executable.
- `SC-2`: The existing `ID=1` patient and any existing measurement data are backed up and preserved; seven anonymous patients and seven measurements are added with obvious test labels.
- `SC-3`: Launching the exact runtime executable shows the test patients in archive management, and GUI checks cover `G19/W20`, `M45`, and `EMPTY/OUTSIDE` without relying only on XML parsing.
- `SC-4`: Restoration is unambiguous and the weekly report points to the correct demonstration runtime and cleanup method.
- `SC-5`: XML parsing and focused integrity checks pass; unrelated files remain untouched and Git changes are reported accurately.

## SOP

1. Identify the exact runtime from observable data and code-path evidence.
2. Back up its XML, append marked anonymous fixtures, and preserve existing records.
3. Launch that executable and verify archive and age-SOS scenarios in the GUI.
4. Correct documentation, verify integrity, review learning, and deliver only after actual checks pass.

## Stop and Escalation Conditions

- Stop if the actual runtime cannot be identified unambiguously, if existing data cannot be backed up, or if GUI behavior contradicts the fixture expectations.
- Do not weaken verification or alter product behavior without user approval.

## Learning and Evolution

Route a repeated runtime-targeting failure to the cheapest reliable preflight or rule candidate, but obtain user approval before changing a Skill, `AGENTS.md`, or another strong constraint.
