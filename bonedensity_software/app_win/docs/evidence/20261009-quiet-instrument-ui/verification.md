# Quiet Instrument UI · verification (2026-10-09)

Scope: visual system and page layouts only (see `docs/design/2026-10-09-quiet-instrument-ui.md`). Measurement algorithms, gates, serial protocol and timing, XML schemas, stored data, account logic and the printed / exported report are unchanged. Protected sources (`signalprocessor`, `bonehealth`, `utils`, stores, `types.h`) have no diff.

| Check | Result |
| --- | --- |
| Main-window safety suite, offscreen, `QT_QPA_FONTDIR=C:/Windows/Fonts` | 96 passed, 0 failed, 1 skipped (`onsetGuardMatchesRecordedBatch` needs replay paths) |
| Same suite at the starting commit `a0dd515` without a font directory | 94 passed, 1 failed (`reportRenderingIsReadableAndArtifactFree`), 2 skipped — the failure comes from the offscreen platform rendering no glyphs, not from code |
| Calibration, patient-store, account-store suites | passed |
| Portable handoff tests | passed |
| `build-debug.ps1` (Qt 6.5.3 / MinGW 11.2) | passed; `BoneDensity.exe` SHA-256 `00450a59a858b830a46cd39d12c4b1db52efae31f35eb2189e3037d02b7dec19` |
| `git diff --check` | clean |

Test changes: `workflow_cases.inc` now expects the account button text `admin` (the avatar replaces the `账号 ` prefix). Card group titles are kept as accessible names but are not painted, so `disconnectedControlsAndPlaceholdersAreSafe` keeps passing unchanged.

Screenshots (offscreen, anonymous in-memory sample data): `main-1920x1080.png`, `main-1366x768.png`, `login.png`, `archive.png`, `feedback-panel.png`, `patient-form.png`, `guide-themed.png`. For the previous look see `../20261009-sos-reference/main-1366.png`.

Capture command (from `build/tests/mainwindow-safety`):

```
QT_QPA_FONTDIR=C:/Windows/Fonts QT_QPA_PLATFORM=offscreen BONE_UI_CAPTURE_DIR=<dir> ./debug/mainwindow_safety_tests.exe
```

Still needs on-site confirmation: real-device five-round flow, appearance on the actual monitor and DPI, and physical printing.
