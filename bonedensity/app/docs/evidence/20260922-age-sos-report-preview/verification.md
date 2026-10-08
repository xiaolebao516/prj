# Age-SOS History and Report Verification

Date: 2026-09-23

## Scope

- Main age-SOS chart plots the selected patient's valid same-profile measurement history using each record's stored age.
- The latest valid measurement is red and earlier measurements are gray.
- A historical report highlights its selected measurement, excludes later records, and uses the approved one-page chart-and-history layout.
- Adult and child profiles remain separated at the project-defined age-20 boundary; excluded opposite-stage records remain stored and are summarized by count.

## Automated verification

- Main-window Qt suite: 59 passed, 0 failed, 2 skipped, 0 blacklisted (4060 ms).
- The two skips are existing environment-dependent cases: absent optional 75-log replay inputs and the page-capture case when its capture environment variable is unset.
- Added focused coverage for stored measurement age, same-profile filtering, age-stage omission count, report cutoff, latest-valid selection, and report chart colors.
- Canonical Debug build passed with Qt 6.5.3 and MinGW 11.2 through `build-debug.ps1`.
- `git diff --check` passed; only Git's informational LF-to-CRLF notices were emitted.

## Render verification

- Inspected the Qt screen rendering: `qt-report-render.png`.
- Exported and inspected the populated one-page A4 PDF: `qt-report-render.pdf`.
- Rendered the PDF through Poppler and inspected the resulting page: `qt-report-pdf-render.png`.
- The chart, Chinese text, result block, historical list, diagnosis area, and footer are visible without clipping or overlap.

## Build and preservation evidence

- Executable: `build/debug/debug/BoneDensity.exe`
- Executable SHA-256: `F363F90137D9D018DDB3E49DDAD759B371C65FE89149C1FFDD84305A1E882E77`
- PDF SHA-256: `CA8151D818AF1508986E5465750434E4AA81BBFB1C9BF2C7D1E57DD52C767C67`
- The build ran in an isolated worktree. Runtime XML preservation check reported `PRESERVED_XML=True`; the isolated executable directory contained zero clinical XML files before the build.
- No persistence schema, measurement algorithm, SOS gate, correlation rule, or calibration code was changed.

## External acceptance

- User visual acceptance of the main chart and report layout remains pending.
- Physical printer output remains external and was not exercised.
