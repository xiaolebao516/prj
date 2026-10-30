# 2026-09-19 档案姓名与实验日志关联验证

## 批准与范围

用户明确要求把所选档案姓名映射到生成的实验记录，准备提供多人测量和各自参考值，并要求完成后提供测量说明。此前非识别实验记录限制仅在档案编号/姓名两字段上由本次授权替代。用户已正常关闭运行程序以便更新；同版本本机Qt/MinGW路径例外已获批准。

修改仅为Debug日志添加配置元数据及会话生命周期；不改变算法、A0.78/G[-12,0]、首波40点、增益、串口、临床XML或校准。

## 实现及验收

| SC | 软件证据 | 结果 |
|---|---|---|
| SC-37 | start config含`subject.archive_id`/`subject.name`、`measurement_session_id`和`recording_profile=subject-linked-20260918-v1`；仅两项身份信息，Unicode/引号/换行通过JSON往返，名字不进入文件路径 | 通过 |
| SC-38 | 真实启动路径、失败重试、自动续轮计时器共享UUID/冻结姓名；完成5轮后新测、显式重置、换同名不同档案各生成新UUID；改名后旧文件不变 | 通过 |
| SC-39 | RED→GREEN、完整主窗口套件、75旧日志真实Qt回放、canonical Debug构建、运行数据保存核对、操作说明 | 软件通过；实际多人采集待人工执行 |

局部改动：`include/mainwindow.h`增加会话状态；`src/mainwindow.cpp`在实验开始冻结ID/姓名并写入config，在全部重置时清空；`include/measurementexperimentlog.h`更新已授权记录范围注释。记录文件仍使用时间+UUID命名，旧schema1可选config字段兼容旧读取方式，无旧文件回填。新测试在`tests/subject_recording_cases.inc`，入口注册于`tests/mainwindow_safety_tests.cpp`。

本功能开始于9月18日，因此记录标识/测试目录沿用20260918；最终构建验证日期为9月19日。算法标识仍为`onset-consistency-20260915-v1`，未冒充新算法版本。

## 实际运行

- RED：原程序缺少`recording_profile`，新测试按预期失败；2通过、1失败。
- GREEN：身份/会话测试3通过、0失败（含初始化/清理）。
- 完整主窗口回归：**58通过、0失败、1跳过**，约155.9秒。跳过项仅为可选截图；75文件/18362帧原始及首波候选C++回放实际执行通过。
- canonical Debug构建及依赖部署：成功。Qt6.5.3，MinGW11.2，使用已批准的`D:/QT6.5.3/6.5.3/mingw_64`与`D:/QT6.5.3/Tools/mingw1120_64`。
- 110个原有文件的SHA-256保持：两处实际运行目录的现有临床XML、101份实验日志和旧QtCreator EXE。自动测试只在`build/tests/subject-20260918`运行，未用真实运行目录作为测试目录。
- `git diff --check`及新增文件空白检查通过；分支与远端0前/0后；未提交、推送或操作真实设备。

## 产物

- EXE：`build/debug/debug/BoneDensity.exe`
- SHA-256：`4cb4316b0af7ee98c9eff662b096933425483bfd775293e3dcd2660c319058e8`
- 使用说明：[subject-linked-measurements.md](../../guides/subject-linked-measurements.md)
- 本机日志：`build/tests/subject-20260918/red.txt`、`green.txt`、`full.txt`、`app-build.log`、`preserved-before.json`。

## 复现命令

在独立构建目录用上述绝对路径qmake构建`tests/mainwindow_safety_tests.pro`，再用指定MinGW的mingw32-make。测试设置`QT_QPA_PLATFORM=offscreen`、`QT_PLUGIN_PATH=D:/QT6.5.3/6.5.3/mingw_64/plugins`，PATH包含该Qt及MinGW的bin。完整回放还需：

```powershell
$env:BONE_ONSET_REPLAY_INPUT='D:/wyl/prj/bonedensity/app/build/Desktop_Qt_6_5_3_MinGW_64_bit_Debug/debug/measurement-experiments'
$env:BONE_ONSET_REPLAY_EXPECTED='D:/wyl/prj/bonedensity/app/build'
# 在独立测试目录执行
./debug/mainwindow_safety_tests.exe -o full.txt,txt
# 从项目根构建产品
powershell -ExecutionPolicy Bypass -File ./build-debug.ps1 -qtRoot 'D:/QT6.5.3/6.5.3/mingw_64' -mingwRoot 'D:/QT6.5.3/Tools/mingw1120_64'
```

名字与完整测量分组由软件记录；探头是否实际移动、参考仪器准确度、同部位对照及个人重复性均未由自动测试证明。不同人的结果必须分别匹配参考信息；没有对旧匿名记录推断身份。
