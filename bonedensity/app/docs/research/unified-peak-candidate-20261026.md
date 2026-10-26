# 2026-10-26 统一人体/试块 B 通道选峰：工作分支候选

**状态：candidate / pending tests and physical verification**。此文不宣称三体模达到校准验收，也不授权部署临床仪器。

## 修复前：已复现的规格不一致

- `FrameAnalyzer::analyze(filtered, false)`（试块自动采集）和 `FrameAnalyzer::calibrationFrame`：调用 `estimatePairSpeed(..., robustLocalPeaks=true)`。
- `FrameAnalyzer::analyze(filtered, true)`（正式人体测量）：原来调用 `estimatePairSpeed(..., robustLocalPeaks=false)`。
- 已有原始波形离线审计显示，KJMK-02 同一批 328 帧在两分支的 B lag 全部不同，约 68 帧差 72–74 采样点（跨相关旁瓣）。这不是两个设备可以共用的一套底层校准算法。

## 本候选最小变更

只在 `src/core/measurement/frameanalyzer.cpp` 的 B 通道估计处，统一调用原有的 `robustLocalPeaks=true`，使同一过滤波形进入同一选峰实现。A 通道搜索、正式人体测量的独立首波一致性前置门禁、姿态/相关性/稳定性门禁、最终五轮汇总、声程 D、采样 Ts、80 ms 采集和原始日志均保持代码原样。

`tests/core_tests.cpp` 补充同一合成反相 KJ 波形和紫铜波形的 `analyze(frame,true)`、`analyze(frame,false)` 与 `calibrationFrame` B lag/corr/SOS 一致性断言。为单独检查选峰，单测中显式关闭人体前置 onset consistency gate；真实产品默认 gate **不变**。

## 这项改动对三体模数值的影响

三份 2026-10-26 试块采集原先**已经在 robust 分支上**，因此单纯把人体也统一到此分支，并不会改变试块的原始三个中位数：

| 23°C 材料 | 新/旧试块选峰不变：B SOS 中位数 | 23°C 参考值 | 原始偏差 |
|---|---:|---:|---:|
| KSGR-3 | 2450 m/s（lag=200） | 2310 | +6.06% |
| KJMK-02 | 2800 m/s（lag=175） | 2711 | +3.28% |
| KSGR-2 | 4711.54 m/s（lag=104） | 4729 | -0.37% |

本轮仅**消除不一致**，不能伪称“已把三个试块修准”。聚苯乙烯稳定耦合的后 326 帧 B 仍以 lag=200 为中位数，lag=212 的相关性中位约 0.2875，而当前选中峰约 0.9204。因此不能通过强选 lag=212 或随意调平台门限解决；须优先确认物理路径、通道时序、材料频散和波形相位的依据。

## 独立补充：已有无目标 SOS 的选峰方法对照

对三体模部分 B 有效帧，使用相同 81-tap Kaiser FIR + 原始窗，独立尝试在既定物理 lag 范围内选「单纯最大正相关峰」、`abs(signal)` 归一化相关峰、Hilbert 包络归一化相关峰。聚苯乙烯的最大正相关峰约 lag=197–199、绝对波形/包络相关还可能落到更早的周期；**均未自然落在参考 lag≈212**。这只是一小批离线探索，不能独立证明哪一种才是正确的传播时间算法，也不能据此部署。

## 未完成的验证与阻断

- 运行项目的 `test.ps1 -Suite core,calibration,mainwindow` 与 `build-debug.ps1`，确保新增对照断言与原有测试通过（**未运行**）。
- 用真实人体原始 JSONL（不是试块）同时回放原旧路径与统一候选，观察 B 选峰、A/B、姿态/稳定性以及五轮最终 SOS；患者入选率/输出不得仅靠理论推断。**目前缺少本轮可独立核验的人体原始日志。**
- 用独立采集确认真实 ADC 时基、探头有效声程、发射接收通道/相位；在此之前不得通过校准参考值强行调整 `D`、`samplePeriod`、`sosOffset` 或为不同材料写特例。
- 若要让三个参考点在同一最终校准模型下满足明确误差门槛，必须用**新一批独立轮次**验证；三个训练点拟合为零误差不是计量验收。

**当前结论**：这项候选修复使人体与试块共享底层 B 选峰定义，但试块 SOS 的剩余系统偏差还未闭合。工作分支可用于代码审查和测试，**不得直接视为已经可用的校准设备更新**。
