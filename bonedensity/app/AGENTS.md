# 项目实施约定

- Debug 构建统一在项目根目录运行 `powershell -ExecutionPolicy Bypass -File .\build-debug.ps1`。
- 不得直接裸跑 `qmake`、`mingw32-make` 或 `g++`。系统 `PATH` 中的 MSYS2 GCC 与 Qt 6.5.3 的 MinGW 运行库不兼容，会导致 `__glibcxx_assert_fail` 入口错误。
- 固定工具链：Qt 6.5.3 `D:\Qt\6.5.3\mingw_64`，MinGW 11.2 `D:\Qt\Tools\mingw1120_64`。
- 禁止修改 `src/signalprocessor.cpp`、`src/bonehealth.cpp`、`src/utils.cpp` 中的算法，以及串口协议、采集时序、检测门限和稳定性判断。
- 采用最小可靠改动；不要因 `mainwindow.cpp` 较大而进行无关重构。
- 每个实施阶段完成后必须构建、更新 `tasks.md` 和 `progress.md`，并单独提交 Git。
