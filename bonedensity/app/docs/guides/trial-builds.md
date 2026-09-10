# 试测版本与源码说明

`build/.../debug/` 是程序运行目录，只保存 EXE、Qt 运行库和运行数据，不保存一份重复的 C++ 源码。所有版本共同使用项目根目录下的 `src/`、`include/`、`ui/` 和 `resources/`。

## 当前版本区别

| 程序 | 相对普通版启用的试测逻辑 | 用途 |
| --- | --- | --- |
| `BoneDensity.exe` | 正式启用：G 门限前观察稳定性、A 双段评分（整轮下限 0.78）、同一稳定簇的部分进度续接 | 正式版；窗口标题保持“骨密度仪APP”，不会显示试测版字样 |
| `BoneDensity_SelfTrial.exe` | G 不合格时仍观察位置稳定性，但该帧不能计入有效值 | 早期姿态流程试测 |
| `BoneDensity_DualWindowTrial.exe` | 上一项，加上 A 通道双段质量评分，整轮 A 下限为 0.78 | 双段评分试测 |
| `BoneDensity_RelockTrial.exe` | 上两项，再加上同一稳定簇的部分进度续接 | 已并入正式版；保留此带试测标题的程序用于旧数据和版本对照 |
| `BoneDensity_BPeakTrial.exe` | B 峰边界补全研究 | 全量回放结果变差，没有作为可用试测版交付 |

从 2026-09-08 开始重新编译的普通版和试测版还共享一项交互改进：第 1～4 个有效轮次完成后，停止采集 1 秒再自动开始下一轮；按空格或点击按钮仍可立即继续。未通过的轮次和第 5 轮不会自动重启。这项改动不调整 SOS、D/G、相关系数或稳定性阈值。

正式版没有加入 B 峰补全，也没有放宽 D/G 范围；这些方案在已有数据回放中表现更差或风险更大，因此仍只保留为研究记录。

## 主要源码位置

- 轮次、有效值、稳定簇、自动续测和界面状态：`src/mainwindow.cpp`、`include/mainwindow.h`
- 波形特征与延迟计算：`src/signalprocessor.cpp`、`include/signalprocessor.h`
- 各试测程序的编译开关和 EXE 名称：`BoneDensity.pro`
- 自动化验证：`tests/mainwindow_safety_tests.cpp`

如果要比较某个 EXE 与普通版，先看窗口标题和本文件中的程序名，不要只看它是否位于名为 `debug` 的文件夹。
