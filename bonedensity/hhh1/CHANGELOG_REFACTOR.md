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

---

## 门控诊断功能（feature/gate-diagnostics 分支）

**日期**: 2026-06-27

### 背景

人体测量时有效帧持续不推进，需定位 7 道闸门中哪一个（或哪几个）是真正瓶颈。

### 新增内容

#### 1. 门控拒绝率统计（12 个计数器）

每帧在 `detectAndPlotSpeed()` 中自动累加各闸门的失败次数：

| 计数器 | 对应闸门 |
|--------|---------|
| `gateFailBJump` | B 通道精修跳变检查 |
| `gateFailBoundary` | 边界反射排除 |
| `gateFailDiff` | A/B lag 差异上限 |
| `gateFailDirection` | A lag 方向检查 |
| `gateFailCorrA` / `gateFailCorrB` | 相关质量底线（分通道） |
| `gateFailAngleSignedDiff` | 角度 signedLagDiff 门控 |
| `gateFailAnglePairMidGap` | 角度 pairMidGap 门控 |
| `gateFailStableWarmup` | 稳定簇预热中 |
| `gateFailStableNotConcentrated` | 稳定簇不够集中 |
| `gateFailStableOutOfLock` | 偏离锁定簇 |

每轮开始时自动清零（`resetOneRoundMeasurementState()` → `resetGateStats()`）。

#### 2. 当前帧闸门状态快照

`detectAndPlotSpeed()` 将最近一帧的各闸门通过状态写入成员变量（`lastFrameBJumpOk`, `lastFrameCorrOk`, `lastFrameStableState` 等），供 `updateProcessPanel()` 读取。

#### 3. UI 实时诊断（`updateProcessPanel` 增强）

帧被拒时，状态标签显示三行信息：

```
未累计：D=6.5∉[7.0,13.0], 预热中(12/22)
→ D偏小：探头倾角偏高，请稍微放平探头
统计(30帧): BJump0% Diff13% Corr27% Ang53% Stab40%
```

| 行 | 内容 |
|----|------|
| 第一行 | 具体失败的闸门和当前值 vs 允许范围 |
| 第二行 | 操作指引（7 种场景，各有定制文案） |
| 第三行 | 10 帧后出现，显示各大类拒绝率百分比 |

#### 4. 操作指引文案

| 失败场景 | 提示 |
|---------|------|
| G 偏大 | 探头偏左侧，请向右（桡骨远端方向）轻移 |
| G 偏小 | 探头偏右侧，请向左（桡骨近端方向）轻移 |
| D 偏小 | 探头倾角偏高，请稍微放平探头 |
| D 偏大 | 探头倾角偏低，请稍微立起探头 |
| 预热中 | 保持当前角度不动 X 秒后锁定 |
| 波动大 | 减小手部晃动或检查耦合剂 |
| 偏离锁定 | 回到之前的触压角度 |
| 相关性低 | 增加耦合剂并确保探头贴合皮肤 |
| 信号异常 | 检查耦合剂是否充足、探头是否贴紧 |

#### 5. 轮结束时控制台输出

```
闸门统计(87帧): BJump=0.0% Boundary=0.0% Diff=12.6% Dir=2.3%
CorrA=26.4% CorrB=8.0% AngDiff=35.6% AngGap=28.7%
StableWarmup=18.4% StableConc=11.5% StableLock=5.7%
```

### 设计原则

- **不改任何检验逻辑** — 所有闸门的阈值和判定条件完全保持不变
- 新增代码仅在闸门判定结果产生后**读取并统计**，不参与控制流
- 方便在分支间对比：同一组数据在 `main` 和 `feature/gate-diagnostics` 分支上 SOS 结果完全一致

### 相关文档

- `docs/debug_guide.md` — 实验室调试指南（步骤、判据、调参决策表）
- `docs/architecture_review.md` — 检验管线架构评审
- `docs/param_tuning_analysis.md` — 参数调优分析
