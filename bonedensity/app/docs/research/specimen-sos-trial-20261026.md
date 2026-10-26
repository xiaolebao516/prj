# 三试块统一 SOS 校准试验版（2026-10-26）

**实验候选；不是正式发布或临床仪器校准证书。**

- 仅在 **Debug qmake `CONFIG+=specimen_sos_trial`** 的独立构建 `BoneDensity_SpecimenSosTrial` 中生效；普通 `BoneDensity` 正式构建仍保持原来的 SOS。
- **Qt Creator 编译说明（2026-10-26 修复）**：选择 Debug 构建，在 qmake 的“额外的参数”中写 `CONFIG+=specimen_sos_trial` 即可。Windows Qt 可能默认启用 `debug_and_release`，导致生成 Release 子 Makefile 并误触试验版的 Debug-only 保护；本仓库已在 `core.pri` 的试验分支自动关闭多配置生成，保留对真正 Release 试验构建的禁止。**更新代码后需重新运行 qmake**。旧版本可临时将 qmake 参数改为 `CONFIG+=specimen_sos_trial CONFIG-=debug_and_release`；更新到修复提交后可移除后半段。请勿在正式 Release 构建中启用试验配置。
- 底层 B 通道选峰继续使用人体/试块共用的 `robustLocalPeaks=true`；不按材料名称指定峰、不更改 lag/D/Ts、不放宽人体门禁。
- 对 **A、B 通道有效 SOS** 使用同一材料无关的数学映射：`SOS_trial = 1.07490541 * SOS_raw - 312.11634` (m/s)。最后的 `sosOffset` 参数继续在正式 SOS 汇总后按现有代码使用。B 通道有效而 A 无效的自动采集帧，也会在 B 证据字段中显示修正后的 SOS。
- 所有试块/人体使用相同的校正映射；不存在 `specimen_id` 运行时分支。保留原始四通道波形和 B lag，可通过每帧 `SOS_raw = D/(lag*Ts)` 离线逆算原值；记录的 `measurementParameters` 增加试验标识、gain、intercept 以避免将试验读数误当正式数据。
- 23°C 训练中位数：KSGR-3 从 2450→2321.40（证书 2310、误差 +0.494%）；KJMK-02 从 2800→2697.62（标签 2711、误差 -0.494%）；KSGR-2 从 4711.54→4752.34（证书 4729、误差 +0.494%）。
- 这是**旧三轮数据内拟合**，尚未在新采集、重新放置、重新耦合条件下验证，绝不保证下一次三块都落入 1%。KJMK-02 同轮 174–178 lag 变化及 KSGR-3 212 参考 lag 的低相关问题仍未解释；本校正不是选峰物理修复。
- 目标下轮试验：三个试块均在实际测温、重新涂抹耦合剂后各做独立重复采集，报告每轮 B 中位数、lag 分布、有效区间、参考误差；先暂定观察 ±1% 作为工程测试门槛，不擅自声称计量验收。注意两份体模证书 SOS 扩展不确定度 2%（k=2）。
- **验证状况**：新增源码与 C++ 单测候选，GitHub 静态回读待完成；Qt/MinGW 编译、单测、实机尚未执行。不要直接替换实验室正式测量 EXE。

见 [23°C 三试块拟合报告](specimen-fit-20261026.md)、[选峰波形审计](peak-selection-audit-20261026.md) 和 [完整温度表](specimen-reference-values.md)。
