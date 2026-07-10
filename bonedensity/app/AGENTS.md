# 项目实施约定

- 构建：在 `build/` 运行 Qt 6.5.3 qmake，再运行 `mingw32-make`。
- 禁止修改 `src/signalprocessor.cpp`、`src/bonehealth.cpp`、`src/utils.cpp` 中的算法，以及串口协议、采集时序、检测门限和稳定性判断。
- 采用最小可靠改动；不要因 `mainwindow.cpp` 较大而进行无关重构。
- 每个实施阶段完成后必须构建、更新 `tasks.md` 和 `progress.md`，并单独提交 Git。
