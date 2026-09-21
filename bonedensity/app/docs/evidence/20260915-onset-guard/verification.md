# 首波一致性试测：软件验证

2026-09-15，用户批准“先试试1”，并批准本机同版本Qt/MinGW路径例外。实现限于患者B首波后移超过40点的拒绝；保留单帧/整轮A0.78、G[-12,0]和现有其他门控。

## 实现

- `ArrivalResult.firstHit`仅暴露原计算已有的首次连续越阈值位置；不改变首波、滤波、选峰或SOS计算。
- 校准处理在新增检查前返回。患者流程在B估计和稳定观察前检查BD/BC的`onset-firstHit`，超过40点进入原有无效帧处理；不在3850附近另找一个峰。
- 新日志字段`B_arrivals`含first_hit/onset/peak/threshold/forward_shift；拒绝原因`B_onset_inconsistent`；配置记录`B_onset_forward_limit=40`。
- 默认构建标题为“骨密度仪APP · 首波一致性试测”，记录标识`onset-consistency-20260915-v1`。既有独立试验宏保留其旧流程。
- 构建/校准脚本增加可选路径参数，原默认路径不变。本任务使用经批准的`D:/QT6.5.3/6.5.3/mingw_64`及`D:/QT6.5.3/Tools/mingw1120_64`。

## 验证结果

| 项目 | 结果 |
| --- | --- |
| 实际低值夹具RED | 原逻辑反复输入SOS1983.81帧后计入2值，期望0，测试失败 |
| 边界及实际低值GREEN | 40点允许/41点拒绝/无效与缺失firstHit不通过；实际低值不计数，诊断1227→1333匹配 |
| 原流程C++/Qt回放 | 75份日志、18362帧，关闭新检查时逐帧A/B/D/G/接受/锁/计数状态、轮次及完成帧与原记录一致 |
| 新流程C++/Qt回放 | 开启新检查时逐个接受帧序号、SOS、轮次和完成帧与独立Python候选一致；结果见[cpp-replay.json](cpp-replay.json) |
| 主窗口完整回归 | **57通过，0失败，1跳过**；跳过项为可选全页面截图。75日志回放已实际执行，不是跳过 |
| 校准回归 | **16通过，0失败，0跳过** |
| canonical Debug构建/部署 | 通过，生成`build/debug/debug/BoneDensity.exe`及Qt/MinGW依赖 |
| 保存检查 | 原Qt Creator EXE和4个XML哈希不变；75份原始日志哈希不变；新目录未复制临床XML |
| 差异检查 | `git diff --check`通过；未提交或推送 |

9月15日57个低值计数消失，另2个原本后来会被换簇清除的正常范围帧不再计入；34个形成轮次的结果/通过状态/完成帧不变。9月14日105个低值计数消失，3个约2520的轮次在原记录长度内不再形成，其余35轮不变。不推断被删轮次后的跨尝试最终五轮池行为。

首轮完整测试发现回放器未载入某历史构建的单帧A0.80、两个旧流程专用零噪声合成夹具触发新检查、旧日志标识断言过期。按日志配置补齐回放；在旧流程单元测试中显式关闭新检查，以继续验证原稳定机制，新检查由真实波形及75日志覆盖；更新预期标识。没有放宽产品门控。修复后完整套件重新通过。

## 复现

主窗口测试项目为`tests/mainwindow_safety_tests.pro`，固定版本工具构建在独立目录`build/tests/onset-20260915/`。运行前设置：

```powershell
$env:QT_QPA_PLATFORM='offscreen'
$env:BONE_ONSET_REPLAY_INPUT='D:\wyl\prj\bonedensity\app\build\Desktop_Qt_6_5_3_MinGW_64_bit_Debug\debug\measurement-experiments'
$env:BONE_ONSET_REPLAY_EXPECTED='D:\wyl\prj\bonedensity\app\build'
```

`BONE_ONSET_REPLAY_EXPECTED`下需要上一轮分析产生的`wave-audit-20260914/state-replay.json`和`wave-audit-20260915/state-replay.json`。原始研究脚本仅支持其记录的旧版本源码；本次产品源码已增加新检查，不要把旧研究脚本的源码哈希保护误认为新版本产品测试失败。

日志：`build/tests/onset-20260915/red.txt`、`full-final.txt`、`calibration-results.txt`、`app-build.log`、`runtime-before.json`。新EXE SHA-256：`ca25f6453f30a73f41e159a79b6ffa75316369cb946740df473ca61aae9ce39a`。

用户说明：[onset-consistency-trial.md](../../guides/onset-consistency-trial.md)。本轮未启动真实设备或实际程序进行无人值守测量；软件验证不代表硬件效果或准确度验收。3600附近稳定结果、增益/削顶调查仍未处理。
