# UI-REFRESH-001 verification (2026-10-08)

Scope: re-layout and restyle of the main window, archive page and patient form; interaction fixes. Measurement algorithms, gates, serial protocol/timing, experiment logs, XML schemas, stored data and the report renderer are unchanged.

## Source baseline

- Managed worktree `ui-refresh` at `8e27c64`, with the uncommitted and previously software-verified `age-sos-report-preview` (SC-46..48) and `crash-render-fix` (SC-43..45) diffs applied (both applied cleanly, no conflicts).
- Integrated baseline before UI work: main-window suite 61 passed / 0 failed / 2 skipped.

## Results

| Check | Result |
| --- | --- |
| Main-window safety suite (offscreen, Qt 6.5.3 / MinGW 11.2 installed paths) | 75 passed, 0 failed, 1 skipped (`onsetGuardMatchesRecordedBatch` needs external replay paths) |
| New focused cases | patient dialog defaults / edit read-only ID / unique suggested ID, latest-result display, retry-save refresh, record-deletion refresh, current-patient edit refresh, Enter-to-login, close-cancel keeps 1 s next round, batch-delete confirmation names + counts, archive double-click, toolbar/result-card structure, layout at 1366x768 / 1600x900 / 1920x1080 |
| Calibration suite | passed |
| Canonical Debug build (`build-debug.ps1`, installed Qt paths) | passed; `build/debug/debug/BoneDensity.exe` SHA-256 `22014b6fbf56cf446bf9a52e0f0b78c4d048923e0d02a034e6286b8f638e7c51` |
| `git diff --check` | clean |
| Protected sources (`signalprocessor`, `bonehealth`, `utils`, stores, calibration, `types.h`) | no diff |
| Runtime XML | none created or modified; build directory of the worktree contains no clinical data |

Structural assertions that changed with the UI (not behavioural weakening): the WIFI button and the three legacy patient-form pages no longer exist (checked as absent / replaced by `PatientFormDialog`), and the default build title is `超声骨密度仪`.

## Screenshots (offscreen Qt rendering, anonymous in-memory sample data)

- `before-main.png` – original main window
- `main-1366.png`, `main-1920.png` – refreshed main window
- `archive.png` – archive page with batch-delete checkbox and action bar
- `patient-form.png` – new/edit patient dialog without gender or birth-date defaults
- `login.png` – rebuilt login page with version footer

## Review follow-up: P0 fixes (2026-10-09)

| Item | Change | Check |
| --- | --- | --- |
| Login page | Rebuilt as a centred card (title, account/password, full-width primary button, error line, version footer); fields were previously squeezed to a few characters wide | `loginPageIsUsableAndShowsVersion`: fields and button >= 260 px at 1366x768, version text present; `login.png` |
| Serial port list | Refresh no longer clears and refills the combo every second; it rebuilds only when the port set changes and never while the drop-down is open, keeping the selection by port name | `portListRefreshKeepsSelectionWithoutRebuilding` |
| Device response | Display-only watchdog: if a command was sent and no complete frame arrived within 2.5 s, the status shows `设备无响应` with a hint; it clears on the next frame or on disconnect. Command timing and acquisition logic are unchanged | `deviceWatchdogReportsMissingFrames` |
| Version | `version.pri` (`2.0.0`) feeds `APP_VERSION` to the app and the tests | login footer |
| Resources | Unused `images/report.bmp` removed from the qrc and `ReportWidget` (it was loaded but never drawn); report drawing code untouched | report tests in the suite pass |

Main-window safety suite after these fixes: 79 passed, 0 failed, 2 skipped (`onsetGuardMatchesRecordedBatch` needs replay paths; `capturePagesWhenRequested` ran separately with `BONE_UI_CAPTURE_DIR` and passed). Debug build passed; `BoneDensity.exe` SHA-256 `19b282c6745e09ce65834d2b72f62431b11c3b9f99e67b46203d908d9daa109d`. `git diff --check` clean.

T/Z reference values, fracture risk and bone-age formulas are **not** changed in this round; they wait for an explicit reference decision.

## External acceptance still required

Real-device five-round flow, USB connect/disconnect, appearance on the actual monitor and DPI, and physical printing.
