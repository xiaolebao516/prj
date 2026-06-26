# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Bone densitometry (骨密度仪) desktop application — a Qt 6.5.3 C++17 app that communicates with a custom ultrasound hardware device over serial (115200 baud) to measure bone density via speed-of-sound (SOS) through the radius (桡骨).

## Build & Run

- **Qt version**: 6.5.3 (MinGW 64-bit), installed at `D:\Qt\6.5.3\mingw_64\`
- **Build system**: qmake (`.pro` file)
- **IDE**: Qt Creator (`.qtcreator/hhh1.pro.user`)

```bash
# Configure (from build directory)
cd build/Desktop_Qt_6_5_3_MinGW_64_bit_Debug
D:\Qt\6.5.3\mingw_64\bin\qmake.exe -o Makefile ..\..\hhh1.pro -spec win32-g++ "CONFIG+=debug"

# Build
make

# Or build directly with Qt Creator by opening hhh1.pro
```

The `.claudignore` file is empty — add build artifact patterns there if needed.

## Architecture

### Page Navigation (QStackedWidget)

The app uses a single `QStackedWidget` in `MainWindow` with these pages:
- **pageLogin** — simple hardcoded login (user/pass = "1"/"1")
- **pageMain** — 4-channel waveform display + speed trend chart + measurement controls
- **pagePatientInfo** — patient data entry/selection form
- **pageArchive** — patient archive table with search/filter/CRUD

### Data Flow

```
Hardware (Pico/RP2040) → Serial (115200, binary frames) → handleSerialReadyRead()
  → parseIncomingData() → frameGroups (4 channels: A/B/C/D, per-frame-index)
  → FIR filter (bandpass 600kHz–1.25MHz @ 62.5MHz sample rate)
  → detectFirstArrivalSmart() + estimatePairSpeed() → SOS values
  → Patient measure workflow OR debug display → Charts update
```

### Key Subsystems

**Serial Protocol** (`mainwindow.cpp`, ~line 280–450):
- Binary framed protocol, 4 ultrasound channels (A/B/C/D)
- Frame format parsed in `parseIncomingData()`: gain, frame index, channel, payload length, raw samples
- Each channel's data is assembled into `frameGroups[frameIdx].ch[0..3]`
- Auto-trigger mode sends acquisition commands every 80ms via `autoTimer`

**Signal Processing** (`mainwindow.cpp`, `designFIR()` + `applyFIR()`):
- FIR bandpass filter: low cutoff 1.25 MHz, high cutoff 600 kHz, sample rate 62.5 MHz
- Filter order: 100 taps (N=101)
- Processing chain: raw samples → baseline subtraction + gate erase → FIR → envelope detection → first-arrival picking

**Speed of Sound Calculation** (`detectAndPlotSpeed()` + `estimatePairSpeed()`):
- Two probe pairs: B_pair (BD→BC) and A_pair (AD→AC), each measuring time-of-flight difference
- Probe C-D physical spacing: 7.84 mm, sample period: 16 ns (62.5 MHz)
- First arrival detected via noise-floor thresholding (`detectFirstArrivalSmart()`)
- Lag refined by cross-correlation (`refineLagByPositiveCrossCorrelation()`)
- Weighted average: `sosAvg = wB * sosB + (1-wB) * sosA` where wB typically ~0.8
- Optional: valley-based pair speed estimation (`estimatePairSpeedByValley()`) as alternative

**Patient Measurement Workflow**:
- `DebugAcquireMode` — free-running acquisition for debugging
- `PatientMeasureMode` — formal measurement with quality gates and progress tracking
- Per-round: collect valid frames until `processValidTarget` (30) is reached, then average
- Multi-round: typically 5 rounds, final SOS is the mean of round averages
- Angle/pose gating: `signedLagDiff` (7–13 range) and `pairMidGap` (-4 to 4 range) filter out incorrect probe angles
- Stability tracking: locks onto a stable `lagB` cluster before accepting frames
- Round clustering: groups round candidates within ±180 m/s, requires ≥3 in main cluster

**Patient Database** (`patientmanager.h/cpp`, XML-based):
- Stored as `patients.xml` in the application directory
- Fields: id, name, gender, birthDay, height, weight, diagprompt, speedOfSound
- CRUD via QDomDocument; also inline methods in `mainwindow.cpp` (`loadPatients()`, `savePatients()`)

**Bone Health Scoring** (`mainwindow.h` ~line 442–465):
- T-score: `(SOS - age_reference_mean) / population_SD`
- Z-score: age-matched comparison
- Outputs: bone strength classification, relative fracture risk, estimated bone age

### UI Notes

- Forms designed in Qt Designer: `mainwindow.ui`, `addpatientdialog.ui`, `PatientArchiveWindow.ui`, `PatientDetailDialog.ui`
- Custom QSS stylesheet applied programmatically in `MainWindow` constructor (Element Plus-inspired blue theme)
- Four QChartView widgets for real-time waveform display (channels A/B/C/D)
- One QChartView for speed-of-sound trend line (`chartSpeed`/`seriesSpeed`)
- Balance indicators show probe angle quality via horizontal progress bars

### Code Organization Warning

Most application logic lives in `mainwindow.cpp` (~157k, single massive file). The `PatientManager` and `PatientArchiveWindow` classes exist but are underused — the MainWindow duplicates much patient CRUD logic inline. When adding features, consider extracting logic into separate classes rather than expanding `mainwindow.cpp` further.

### Important Constants & Parameters

| Parameter | Value | Location |
|-----------|-------|----------|
| Sample rate | 62.5 MHz | `samplePeriod = 16e-9` |
| Probe C-D spacing | 7.84 mm | `probeDistanceCD` |
| FIR low cutoff | 1.25 MHz | `designFIR()` call |
| FIR high cutoff | 600 kHz | `designFIR()` call |
| Serial baud | 115200 | `on_connectButton_clicked()` |
| Min lag threshold | 100 points | `minLagThreshold` |
| Frame corrA min | 0.78 | `frameCorrAMin` |
| Frame corrB min | 0.55 | `frameCorrBMin` |
| Angle signed diff range | 7.0–13.0 | `angleSignedDiffMin/Max` |
| Pair mid gap range | -4.0–4.0 | `anglePairMidGapMin/Max` |
