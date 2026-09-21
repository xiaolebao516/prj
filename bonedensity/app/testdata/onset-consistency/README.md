# 首波一致性回归夹具

`low-1984.jsonl`包含一条配置记录和一帧匿名四通道原始波形，不含患者标识或临床XML。

来源：`round-20260915-073724-414-98ceb483-2a58-4cce-b3b6-b2e2030750c2.jsonl`，sequence333；原文件SHA-256为 `4acf92cef51c27cdec4d227568f47eaac084c373eeee0e4708dda64e380c4e6d`。

旧结果SOS1983.805668、A相关性0.957704729；BD firstHit1114/onset1113，BC firstHit1227/onset1333。反复输入同帧在原流程可计数，新患者检查应以`B_onset_inconsistent`拒绝。该夹具用于复现软件门控，不证明物理声速或新门限跨人有效性。

检查入口：`MainWindowSafetyTests::onsetGuardRejectsRecordedLowFrame`。40/41点边界另由`onsetConsistencyBoundaries`覆盖。
