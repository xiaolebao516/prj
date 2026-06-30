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

#### 3. UI 操作指引（`updateProcessPanel` 增强）

帧被拒时，`lblProcessStatus`（有效值进度条下方）显示**简洁的操作建议**，引导用户调整探头：

```
探头偏左，请向右(桡骨远端)轻移
【闸门统计请看 Qt Creator 控制台输出】
```

字体放大为 14px 橙色加粗，一眼可见。

#### 4. 操作指引文案（简短版）

| 失败场景 | 提示 |
|---------|------|
| G 偏大 | 探头偏左，请向右(桡骨远端)轻移 |
| G 偏小 | 探头偏右，请向左(桡骨近端)轻移 |
| D 偏小 | 倾角偏高，请放平探头 |
| D 偏大 | 倾角偏低，请立起探头 |
| 预热中 | 预热中，请保持不动 X 秒 |
| 波动大 | 波形波动大，请减小手部晃动 |
| 偏离锁定 | 探头偏离锁定位置，请回到之前的触压角度 |
| 信号异常 | 请检查耦合剂是否充足、探头是否贴紧 |

#### 5. 控制台闸门统计（qDebug 输出）

每轮结束时自动输出到 Qt Creator "应用程序输出"面板：

```
闸门统计(87帧): BJump=0.0% Boundary=0.0% Diff=12.6% Dir=2.3%
CorrA=26.4% CorrB=8.0% AngDiff=35.6% AngGap=28.7%
StableWarmup=18.4% StableConc=11.5% StableLock=5.7%
```

各参数含义见 `docs/debug_guide.md`。

### 设计原则

- **不改任何检验逻辑** — 所有闸门的阈值和判定条件完全保持不变
- 新增代码仅在闸门判定结果产生后**读取并统计**，不参与控制流
- 方便在分支间对比：同一组数据在 `main` 和 `feature/gate-diagnostics` 分支上 SOS 结果完全一致

### 相关文档

- `docs/debug_guide.md` — 实验室调试指南（步骤、判据、调参决策表）
- `docs/architecture_review.md` — 检验管线架构评审
- `docs/param_tuning_analysis.md` — 参数调优分析

---

## 界面与交互优化（2026-06-27 round 2）

### 背景

第一版诊断界面存在三个问题：
1. 闸门统计塞在 `lblProcessStatus`（81×16 像素）中，文字截断看不清
2. 检测中无法暂停/退出，操作者被迫等完整轮
3. 换到低分辨率笔记本后界面底部被裁切

### 改动内容

#### 1. UI 文字简化

- `lblProcessStatus` 标签扩大为 481×81，启用自动换行
- 有效值下方**只显示操作建议**（如"探头偏左，请向右轻移"），14px 橙色大字
- 闸门统计**不显示在界面上**，改为输出到 Qt Creator 控制台（qDebug）
- 新增 `docs/debug_guide.md` 详细说明每个参数含义和诊断方法

#### 2. 检测中可停止

- "开始检测"按钮改为**开始/停止切换**：检测中变为"停止检测"，点击即停止
- trigger 按钮和"获取波形"按钮在检测中也能触发停止
- 停止后：清空当前轮数据，保留已完成的历史轮次，界面恢复初始状态

#### 3. 窗口自适应屏幕

- 构造函数末尾添加屏幕检测：窗口按比例缩放到当前屏幕（设计稿 1920×1080）
- 小屏幕自动缩小，大屏幕居中显示

#### 4. 声速调试面板可读性

- 字体大小：11px→13px（标题），10-11px→12px（数值）
- 面板高度：48→60px
- 行间距适当加大

---

## 调试信息清理 + 参数调优（2026-06-27 round 3）

### 背景

实测发现三个问题：
1. Qt Creator 控制台每分钟打印数百行逐帧调试数据，完全无法阅读
2. 探头在空气中或刚贴上时的"垃圾帧"污染了闸门统计百分比
3. 单人测试中有效帧推进速度偏慢

### 改动内容

#### 1. 逐帧 qDebug 开关

- 在 `mainwindow.cpp` 和 `signalprocessor.cpp` 顶部新增 `static constexpr bool kDebugPerFrame = false`
- 所有逐帧输出（波形数据 dump、首波定位、角度特征、稳定性状态等 15+ 处）包在 `if (kDebugPerFrame)` 内
- 默认关闭，改为 `true` 重新编译即可恢复逐帧调试
- 保留输出：每 50 帧打印闸门统计，每轮结束时打印汇总 + CorrA 分布

#### 2. 闸门统计输出优化

- `gateStatsSummary()` 从单行长字符串拆为两行短输出，避免 Qt Creator 字符上限截断
- 新增 `printGateStats()` 直接 qDebug 输出（替代旧接口）
- 新增 `[CorrA分布]` 行：显示当前轮 CorrA 最小值/平均值/最大值/门槛值

#### 3. 脱耦过滤

- 新增 `gateDecoupledFrames` 计数器
- `bRes.corr < 0.25` 判定为探头悬空/无耦合，**不计入** `gateTotalFrames`，也不累加任何闸门 fail 计数
- 每轮统计输出附加 `(另有 XX 帧因脱耦被忽略，corr<0.25)`

#### 4. 参数调优

基于真人测试数据的 CorrA 分布分析（平均 ~0.78，最小 0.058，最大 0.95）：

| 参数 | 改前 | 改后 | 原因 |
|------|------|------|------|
| `stableLagWarmupCount` | 22 | **14** | 热身阶段过长，缩短后进度条更快启动 |
| `frameCorrAMin` | 0.78 | **0.65** | 桡骨 A 通道相关度天生偏低，旧门槛刚好卡在平均值上导致纯随机筛选 |
| `anglePairMidGapMin/Max` | [-4, 4] | **[-10, 10]** | 真人左右手感波动大，±4 导致 ~50% 帧被卡 |

> **结论**：从日志分析来看，提升最大的是热身帧数（22→14），CorrA 和 AngGap 的放宽对速度改善有限——瓶颈主要在探头摆放手法，不是参数。

---

## 参数回退（2026-06-30）

### 背景

0.65 CorrA + AngGap ±10 的配置虽然有效帧推进快，但测量结果不准：测完一轮经常没算进轮次（round-level 质量检查失败），最终 SOS 偏离参考值。

### 回退内容

| 参数 | 回退前 | 回退后 | 原因 |
|------|--------|--------|------|
| `frameCorrAMin` | 0.65 | **0.78** | 放宽后低相关帧混入，导致轮级 CorrA 均值 (≥0.80) 难以达标 |
| `anglePairMidGapMin/Max` | [-10, 10] | **[-4, 4]** | 放宽后姿态错误的帧混入，轮级一致性变差 |

### 保留

| 参数 | 值 | 原因 |
|------|-----|------|
| `stableLagWarmupCount` | **14**（不改回22） | 唯一明确有效的改动，显著加快进度条启动 |

---

## 参数集中管理 + EMA 尝试与回退（2026-06-30 round 2）

### 1. 提取 MeasureConfig 结构体

**目的**：14 个散落在 `mainwindow.h` 中的可调参数（质量底线 ×4、姿态门控 ×6、稳定性 ×4）集中到 `types.h` 的 `struct MeasureConfig`，便于批量调整和版本对比。

**改动**：
- `include/types.h` — 新增 `struct MeasureConfig { ... }`，含全部默认值
- `include/mainwindow.h` — 删除所有零散成员变量，替换为 `MeasureConfig mCfg;`
- `src/mainwindow.cpp` — 50+ 处引用全部加 `mCfg.` 前缀（`frameCorrAMin` → `mCfg.frameCorrAMin` 等）

> **注意**：`stableLagWindowSize`、`boneLagLocked` 等运行时状态变量**未**移入 MeasureConfig，它们不属于可调参数。

### 2. EMA 平滑滤波（已撤销）

尝试在 `detectAndPlotSpeed()` 中对 `pairMidGap` 应用 EMA（α=0.20）抑制 UI 平衡条跳变。实测发现平滑引入的相位延迟导致探头定位滞后，用户体验反而下降，随后完全撤销。

**撤销范围**：`smoothedPairMidGap` 成员变量、EMA 计算逻辑、重置代码——全局搜索 **0 残留**。

---

## 试块校准 + 参数精调 + B通道优先（2026-06-30 round 3）

### 关键发现

在试块（phantom）上测试发现：

- **B 通道**：误差在 1% 以内，稳定可靠
- **A 通道**：表现不稳定，偏差较大，疑似 A 通道探头硬件问题

### 改动

#### 1. SOS 输出策略：纯 B 通道

`useBOnlyForPatientSos`：`false` → **`true`**

最终 SOS 不再使用 A/B 加权平均，完全依赖 B 通道。A 通道仅保留姿态门控功能（`signedLagDiff`、`angleSignedDiffOk`）。

#### 2. MeasureConfig 参数精调

基于试块测试结果进一步放宽：

| 参数 | 改前 | 改后 | 原因 |
|------|------|------|------|
| `angleSignedDiffMin` | 7.0 | **5.0** | 试块上 D 值分布更宽 |
| `angleSignedDiffMax` | 13.0 | **15.0** | 同上 |
| `anglePairMidGapMin` | -4.0 | **-6.0** | 左右手感容差 |
| `anglePairMidGapMax` | 4.0 | **6.0** | 同上 |
| `stableLagLockNeedCount` | 15 | **10** | 降低锁定门槛，配合窗口缩小 |
| `stableLagTolerance` | 4 | **5** | 放宽锁定簇范围 |
| `boneLagUnlockCount` | 8 | **10** | 减少误脱锁 |
| `stableLagWindowSize` | 30 | **20** | 更快响应位置变化 |

#### 3. 下一步

- 加入**试块校准机制**：用已知 SOS 的试块测量各通道偏移量，存入校准参数自动补偿
- A 通道修复后可通过 `useBOnlyForPatientSos` 一键切回双通道模式
