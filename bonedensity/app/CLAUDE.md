# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Bone densitometry (骨密度仪) desktop application — a Qt 6.5.3 C++17 app that communicates with a custom ultrasound hardware device over serial (115200 baud) to measure bone density via speed-of-sound (SOS) through the radius (桡骨).

> **Note**: The project directory is still named `hhh1` (legacy). The `.pro` file and build target have been renamed to `BoneDensity`.

### Project History

This project was originally developed by the user's senior (学长) through iterative ChatGPT-driven development. Key characteristics inherited from that origin:

- **All code was in a single file**: `mainwindow.cpp` reached ~157k chars with UI, serial, DSP, statistics, and bone health scoring all interleaved. This was produced by repeatedly appending features via chat conversation, not by architectural design.
- **Validation parameters were empirically tuned**: The 7-gate AND chain thresholds (angle ranges, correlation minima, stability tolerances) were derived from repeated experiments comparing against a reference device (样机). The senior confirmed that loosening any gate degraded agreement with the reference.
- **Code was flat in one directory**: No subdirectory structure, all `.cpp`/`.h`/`.ui` files in root.
- **Project was named `hhh1`**: A placeholder name with no meaning.

**2026-06-27 refactoring (current session)** addressed these issues without changing any validation logic:

| Step | What | Why |
|------|------|-----|
| 1. Extract 19 pure functions | Moved from `mainwindow.cpp` → `signalprocessor.cpp`, `utils.cpp`, `bonehealth.cpp` | Separate UI from algorithms; enable independent testing |
| 2. Add gate diagnostics | 12 rejection counters + real-time UI feedback on `feature/gate-diagnostics` branch | Human subjects couldn't accumulate valid frames; needed to identify which gate was the bottleneck |
| 3. Restructure directories | Created `src/` `include/` `ui/` `resources/` subdirectories | Match normal C++ project convention |
| 4. Rename project | `hhh1.pro` → `BoneDensity.pro`, target = `BoneDensity.exe` | Meaningful name |

**Known remaining issues** (documented but not yet addressed):
- `mainwindow.cpp` is still ~130k — the 19 extracted functions were pure algorithms; UI, serial, and measurement workflow code remain inline
- The 7-gate chain may have redundant gates: `diffOk` is a strict subset of `angleSignedDiffOk` when angle gating is enabled
- `stableOk` locks onto the median angle the operator settles at, not necessarily the physiologically correct angle (see `docs/architecture_review.md`)
- Parameter tuning was done on a small sample set; angles may not generalize to all bone types (see `docs/param_tuning_analysis.md`)

## Build & Run

- **Qt version**: 6.5.3 (MinGW 64-bit), installed at `D:\Qt\6.5.3\mingw_64\`
- **Build system**: qmake (`.pro` file)
- **IDE**: Qt Creator (`.qtcreator/BoneDensity.pro.user`)

```bash
# Configure (from build directory)
cd build/Desktop_Qt_6_5_3_MinGW_64_bit_Debug
D:\Qt\6.5.3\mingw_64\bin\qmake.exe -o Makefile ..\..\BoneDensity.pro -spec win32-g++ "CONFIG+=debug"

# Build
make

# Or build directly with Qt Creator by opening BoneDensity.pro
```

The `.claudignore` file is empty — add build artifact patterns there if needed.

### After Clone — Quick Start

1. **Prerequisites**: Qt 6.5.3 (MinGW 64-bit) + Qt Charts. On a new machine, update the qmake path below.
2. **Open** `BoneDensity.pro` in Qt Creator, or build from CLI as above.
3. **Login**: user `1`, password `1`.
4. **Hardware**: Connect Pico/RP2040 via USB, select COM port in app (115200 baud).

### Branches

| Branch | Purpose |
|--------|---------|
| `main` | Production / stable — lean, no debug noise |
| `feature/gate-diagnostics` | Lab debugging — gate rejection counters + UI operator guidance. Same validation logic as main, just with diagnostics added. |

Use `feature/gate-diagnostics` in the lab to identify which validation gate is blocking frame acceptance.

### Documentation Index

| File | Content |
|------|---------|
| `CHANGELOG_REFACTOR.md` | Refactoring changes and diagnostic feature log |
| `docs/debug_guide.md` | Step-by-step lab debugging procedure + parameter tuning decision table |
| `docs/architecture_review.md` | Validation pipeline structural review (7-gate chain analysis) |
| `docs/param_tuning_analysis.md` | Per-gate parameter analysis with modification risk assessment |

## Directory Structure

```
hhh1/                           # 旧目录名未改，.pro 和 target 已更名为 BoneDensity
├── src/                        # .cpp 源文件
│   ├── main.cpp                # 入口
│   ├── mainwindow.cpp          # 主窗口 (~130k, UI + 串口 + 测量流程)
│   ├── signalprocessor.cpp     # FIR滤波 / 首波检测 / 互相关 / 声速估计
│   ├── bonehealth.cpp          # T-score / Z-score / 骨强度分级
│   └── utils.cpp               # 统计工具 / 聚类 / 截尾均值
├── include/                    # .h 头文件
│   ├── mainwindow.h
│   ├── types.h                 # 全局结构体 (WaveGroup, ArrivalResult, PairResult 等)
│   ├── signalprocessor.h
│   ├── bonehealth.h
│   └── utils.h
├── ui/                         # Qt Designer 表单
│   └── mainwindow.ui
├── resources/                  # Qt 资源
│   ├── resources.qrc
│   └── images/Radius.bmp
├── docs/                       # 文档
├── BoneDensity.pro             # qmake 项目文件
├── CLAUDE.md
└── CHANGELOG_REFACTOR.md
```

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

**Serial Protocol** (`src/mainwindow.cpp`):
- Binary framed protocol, 4 ultrasound channels (A/B/C/D)
- Frame format parsed in `parseIncomingData()`: gain, frame index, channel, payload length, raw samples
- Each channel's data is assembled into `frameGroups[frameIdx].ch[0..3]`
- Auto-trigger mode sends acquisition commands every 80ms via `autoTimer`

**Signal Processing** (`src/signalprocessor.cpp`, class `SignalProcessor`):
- FIR bandpass filter: low cutoff 1.25 MHz, high cutoff 600 kHz, sample rate 62.5 MHz
- Filter order: 100 taps (N=101)
- Processing chain: raw samples → baseline subtraction + gate erase → FIR → envelope detection
- First arrival picking: `detectFirstArrivalSmart()` (noise-floor thresholding)
- Lag refinement: `refineLagByPositiveCrossCorrelation()`
- Pair speed estimation: `estimatePairSpeed()`, `estimatePairSpeedByValley()`
- Valley detection: `findFirstProminentValley()`

**Statistics Utilities** (`src/utils.cpp`, namespace `Utils`):
- `meanValue`, `medianValue`, `trimmedMeanValue` (20% trim), `safeRatio`
- `findBestRoundCluster` — ±180 m/s tolerance, ≥3 rounds for main cluster
- `rebuildAcceptedRoundsFromCandidates`

**Bone Health Scoring** (`src/bonehealth.cpp`, namespace `BoneHealth`):
- T-score: `(SOS - age_reference_mean) / population_SD`
- Z-score: age-matched comparison
- Outputs: bone strength classification, relative fracture risk, estimated bone age

**Patient Measurement Workflow** (`src/mainwindow.cpp`):
- `DebugAcquireMode` — free-running acquisition for debugging
- `PatientMeasureMode` — formal measurement with quality gates and progress tracking
- Per-round: collect valid frames until `processValidTarget` (30) is reached, then trimmed mean
- Multi-round: typically 5 rounds, final SOS = mean of round averages (after clustering)
- 7-gate AND chain filters every frame (see below)
- Round clustering → trimmed mean → final SOS

**Patient Database** (inline in `src/mainwindow.cpp`, XML-based):
- Stored as `patients.xml` in the application directory
- Fields: id, name, gender, birthDay, height, weight, diagprompt, speedOfSound
- CRUD via QDomDocument (`loadPatients()`, `savePatients()`)

### Validation Pipeline (7-Gate AND Chain)

Each frame must pass ALL gates to contribute to measurement:

| # | Gate | Condition | Diagnostic Counter |
|---|------|-----------|-------------------|
| 1 | `bJumpOk` | B-channel onset jump ≤ 70 points | `gateFailBJump` |
| 2 | `notBoundary` | Onset position < 260 | `gateFailBoundary` |
| 3 | `diffOk` | abs(lagA - lagB) ≤ 18 | `gateFailDiff` |
| 4 | `directionOk` | lagA sign check | `gateFailDirection` |
| 5 | `corrOk` | corrA ≥ 0.78, corrB ≥ 0.55 | `gateFailCorrA/B` |
| 6 | `angleOk` | signedLagDiff ∈ [7,13], pairMidGap ∈ [-4,4] | `gateFailAngleSignedDiff`, `gateFailAnglePairMidGap` |
| 7 | `stableOk` | lagB stability (warmup 22 → lock ±4 pts → out-of-lock 8) | `gateFailStable*` (3 sub-states) |

On `feature/gate-diagnostics` branch, each rejection is counted and displayed in real-time on the UI with operator guidance text. See `docs/debug_guide.md` for lab testing procedures.

**Round-level quality check**: after 30 valid frames accumulated, `corrA ≥ 0.80`, `corrB ≥ 0.55`, `angleOk` — if failed, entire round is discarded.

**Final clustering**: `findBestRoundCluster` with ±180 m/s tolerance, requires ≥3 rounds in main cluster, then 20% trimmed mean.

### UI Notes

- Main form in Qt Designer: `ui/mainwindow.ui`
- Two additional dialog forms: `addpatientdialog.ui`, `PatientDetailDialog.ui`
- Custom QSS stylesheet (Element Plus-inspired blue theme)
- Four QChartView widgets for real-time waveform display (channels A/B/C/D)
- One QChartView for speed-of-sound trend line (`chartSpeed`/`seriesSpeed`)
- Balance indicators show probe angle quality via horizontal progress bars

### Code Organization

`mainwindow.cpp` (~130k) still holds most UI/serial/measurement logic. After the 2026-06-27 refactoring, pure algorithm functions were extracted:

| Module | Location | Functions |
|--------|----------|-----------|
| Signal DSP | `src/signalprocessor.cpp` | FIR, onset detection, cross-correlation, SOS estimation |
| Statistics | `src/utils.cpp` | mean/median/trimmed mean, clustering |
| Bone health | `src/bonehealth.cpp` | T-score, Z-score, fracture risk |

When adding features, continue extracting logic into separate classes rather than expanding `mainwindow.cpp`.

### Important Constants & Parameters

#### Core Physics

| Parameter | Value | Location |
|-----------|-------|----------|
| Sample rate | 62.5 MHz | `samplePeriod = 16e-9` |
| Probe C-D spacing | 7.84 mm | `probeDistanceCD` |
| Serial baud | 115200 | `on_connectButton_clicked()` |
| FIR low cutoff | 1.25 MHz | `designFIR()` call |
| FIR high cutoff | 600 kHz | `designFIR()` call |
| Min lag threshold | 100 points | `minLagThreshold` |

#### Frame-Level Validation Gates

| Parameter | Value | Member |
|-----------|-------|--------|
| B-jump threshold | 70 points | (inline in `detectAndPlotSpeed()`) |
| Boundary onset max | 260 | (inline) |
| A/B lag diff max | 18 | `processStrictLagTolerance` |
| corrA min (frame) | 0.78 | `frameCorrAMin` |
| corrB min (frame) | 0.55 | `frameCorrBMin` |
| Angle signedLagDiff | 7.0 – 13.0 | `angleSignedDiffMin / Max` |
| Angle pairMidGap | -4.0 – 4.0 | `anglePairMidGapMin / Max` |

#### Stability Locking (stableOk)

| Parameter | Value | Member |
|-----------|-------|--------|
| Warmup window | 22 frames | `stableLagWarmupCount` |
| Window size | 30 frames | `stableLagWindowSize` |
| Lock tolerance | ±4 points | `stableLagTolerance` |
| Lock need count | 15/22 within tolerance | `stableLagLockNeedCount` |
| Unlock threshold | 8 consecutive out-of-lock frames | `boneLagUnlockCount` |

#### Round & Final Clustering

| Parameter | Value | Member |
|-----------|-------|--------|
| Frames per round | 30 | `processValidTarget` |
| Rounds per patient | 5 | `normalMeasureRounds` |
| corrA min (round) | 0.80 | `roundCorrAMin` |
| corrB min (round) | 0.55 | `roundCorrBMin` |
| Round cluster tolerance | ±180 m/s | `roundClusterTolerance` |
| Min rounds in cluster | 3 | `roundClusterMinCount` |
| Trim percentage | 20% | (in `Utils::trimmedMeanValue`)
