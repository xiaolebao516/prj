# Live-chart crash stabilization evidence

## Confirmed failure path

- Windows recorded `BoneDensity.exe` application crashes on 2026-09-20 and 2026-09-21 with exception `0xc0000005` in Qt 6.5.3 `Qt6Gui.dll`, at offsets `0x237773` and `0x237a06`.
- The retained 2026-09-21 dump (`C:/Users/Administrator/AppData/Local/CrashDumps/BoneDensity.exe.28416.dmp`) places the GUI thread in `Qt6Charts::LineChartItem -> QPaintEngineEx::drawLines -> QRasterPaintEngine::stroke`.
- The corresponding experiment logs stop abruptly without a normal stop/close event. Serial and worker threads were waiting normally in the dump.

This establishes the live Qt Charts software paint path as the confirmed crash class. Stripped Qt symbols do not expose the exact internal Qt source statement.

## Repair boundary

- All acquisition frames still run signal processing, measurement decisions, and experiment recording at the existing cadence.
- The four 2,000-point waveform displays refresh at most once per 250 ms.
- Antialiasing is disabled on the four waveform charts and the speed chart.
- Fixed axes are no longer invalidated when their ranges have not changed.
- Non-finite values are rejected before they enter a live chart series.
- No SOS calculation, gate, stability rule, serial command/timing, persistence schema, report, or stored result is changed.

## Verification

- Baseline focused test: failed at the first waveform chart because antialiasing was enabled.
- Focused repair tests: 4 passed, 0 failed. This includes 2,400 updates of four 2,000-point series and 600 forced offscreen paints, equivalent to ten minutes at the new 4 Hz display rate.
- Full `MainWindowSafetyTests`: 59 passed, 0 failed, 2 optional skips.
- `git diff --check`: passed.
- Clean Debug build and `windeployqt`: passed with Qt 6.5.3 / MinGW 11.2 from the approved local installation.

Supervised device endurance is still required because the original access violation was intermittent and depended on the native Windows paint path.
