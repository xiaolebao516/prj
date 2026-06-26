# 重构日志：从 mainwindow.cpp 抽离纯算法函数

**日期**: 2026-06-27

## 背景

原 `mainwindow.cpp` 约 138,000 字符，混杂了 UI 逻辑、串口通信、信号处理和统计工具函数，严重违反单一职责原则，难以维护和测试。

## 改动内容

### 1. 删除 mainwindow.cpp 中的旧成员函数体（-28,686 字符）

以下 19 个 MainWindow 成员函数已从 `mainwindow.cpp` 中安全删除（使用精确字符串匹配，无硬编码行号）：

| 模块 | 删除的函数 | 新位置 |
|------|-----------|--------|
| **统计工具** | `meanValue`, `medianValue`, `trimmedMeanValue`, `safeRatio`, `findBestRoundCluster`, `rebuildAcceptedRoundsFromCandidates` | `utils.h` / `utils.cpp` (命名空间 `Utils`) |
| **骨健康评分** | `calcPatientAge`, `calcAgeReferenceMean`, `classifyBoneStrength`, `calcRelativeFractureRisk`, `estimateBoneAgeFromSos` | `bonehealth.h` / `bonehealth.cpp` (命名空间 `BoneHealth`) |
| **FIR 滤波器** | `preprocessRawForFIR`, `applyFIRDouble`, `buildEnvelopeFromFiltered` | `signalprocessor.h` / `signalprocessor.cpp` (类 `SignalProcessor`) |
| **首波检测** | `detectFirstArrivalFromFiltered`, `detectFirstArrivalSmart` | `signalprocessor.h` / `signalprocessor.cpp` |
| **互相关精修** | `refineLagByCrossCorrelation`, `refineLagByPositiveCrossCorrelation` | `signalprocessor.h` / `signalprocessor.cpp` |
| **声速估计** | `estimatePairSpeed`, `estimatePairSpeedByValley` | `signalprocessor.h` / `signalprocessor.cpp` |
| **波谷检测** | `findFirstProminentValley` | `signalprocessor.h` / `signalprocessor.cpp` |

### 2. 修复调用点引用

- `printAngleFeatureDebug()` 中 4 处 `findFirstProminentValley(...)` 改为 `SignalProcessor::findFirstProminentValley(...)`

### 3. 修复 signalprocessor.cpp 类型限定符

- `SignalProcessor::ArrivalResult` → `ArrivalResult`（4 处，类型定义在 `types.h` 全局作用域）

### 4. 清理

- 删除残留备份文件 `mainwindow.cpp.bak`

## 好处

1. **关注点分离** — UI 代码与纯算法解耦，`mainwindow.cpp` 缩减约 21%
2. **可复用性** — `SignalProcessor` 类、`Utils` / `BoneHealth` 命名空间可独立引用
3. **可测试性** — 纯函数可脱离 Qt GUI 环境单独单元测试
4. **可维护性** — 算法修改只需定位到对应小文件，不再在海量代码中搜索
5. **编译安全** — 所有删除使用文本精确匹配，非硬编码行号，已验证 `qmake + make` 编译通过（零错误）
