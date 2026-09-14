# corrA 优先引导：软件验证（2026-09-12）

已将用户认可的界面接入正常 Qt 程序：corrA 主提示、G 辅助、D 小型辅助条；三页教学依次说明贴合准备、长轴方向调整、倾角微调与计数。D/G 映射、测量判定及一秒自动续轮不变。教程版本更新为 2，旧用户会看到新版教学。

## 验证结果

- 旧程序缺少 corrA 的检查先失败，实现后通过。
- 实际波形处理记录中的 corrA 与界面值一致；显示读取原判定值和配置阈值，无平滑、不修改有效值判定。边界、负值、达标但不计数、无效帧、停止和轮次重置有检查。
- 三页导航、旧版标记升级、跳过/关闭、手动打开、空格及自动续轮回归通过。
- 完整主窗口测试：54 通过、0 失败、1 个可选全页面截图测试跳过。当前功能截图另外运行并通过。
- 实际 Qt 截图检查：主窗口1920×1080、1366×768；教程900×620、760×560。新进度区未遮挡参考图。截图数值是测试输入，不是患者结果。
- 正常 Debug 构建部署通过。算法文件、阈值、采集时序和临床 XML 未修改。未操作桌面或真实仪器。

实际界面：[进度区](feedback-panel.png)、[长轴方向教学](measurement-guide-2.png)。

## 复现和产物

测试项目 `tests/mainwindow_safety_tests.pro`，必须在独立目录运行。固定 Qt6.5.3/MinGW11.2；无桌面截图设置 `QT_QPA_PLATFORM=offscreen`、`QT_QPA_FONTDIR=C:/Windows/Fonts` 和 `BONE_UI_CAPTURE_DIR`。

重点测试：`corrAFeedbackBindingAndResponsiveLayout`、`positionGuideTracksExistingBarsWithoutChangingThem`、`experimentRecordingCoversFeatureDecisions`、`measurementGuideHasThreeApprovedPagesAndPortableMarker`、`measurementGuideFirstUseAndSpaceContinue`、`automaticNextRoundIsGuardedAndCancelable`。

本机证据目录 `build/tests/corra-ui-20260911/` 包含 `red.txt`、`full-final.txt`、`captures-final.txt`、`canonical-build.txt` 和 `captures/`。

正常程序 `build/debug/debug/BoneDensity.exe` 由 `build-debug.ps1` 构建。旧程序在上述证据目录的 `rollback/BoneDensity-before-ui.exe`；SHA-256 与更新前一致：`9B65749728B396B9D59AEBA85A399E4D735F99FE9B672DF7B2E853970085E7F5`。过期的自动生成 qmake 缓存也保留在 rollback，新缓存已确认使用 GCC11.2。

## 更新后 SHA-256

| 文件 | SHA-256 | 与更新前比较 |
| --- | --- | --- |
| accounts.xml | 03568B320B88AEDC4B1CFD9F2B2317DF2A4EF8BD507A48C2526AD4742134C7F6 | 未改变 |
| BoneDensity.exe | 274192AE60833997729E621D67699AD8073DD185D4A8FAD21622C56BC97E389B | 已更新 |
| calibration.xml | 44E105CE61D8B45C632BE110400A0487F5DA51358F37FBCAC362DC6C950C0B88 | 未改变 |
| measurements.xml | 37BFCA7DAD4FD59057534569C305F9C8E22E4534AEE0D93E963C98E5CEA36687 | 未改变 |
| patients.xml | C50BD268C24DE309C4C35C81BBC67970F339390E491F193DC79543020AD51EDF | 未改变 |

构建前后均未生成或改写实际目录的教学已读标记。

## 外部验收

打开正常程序，点击检测过程区“操作教学”，再用设备评估 corrA 优先引导是否方便找姿势。软件验证不代表定位耗时、重复性或准确性已通过硬件验收。未提交或推送 Git。
