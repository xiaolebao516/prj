# Android 开发工具

## replay_check：在电脑上核对 core 结果

用 CMake 编译 `app_win/src/core`（文件列表从 `core.pri` 读取），把一段实验日志中的帧完整走一遍：协议字节 → `FrameAssembler` → 滤波 → `FrameAnalyzer`，然后输出结果。

```bash
cmake -S tools/replay_check -B build/replay_check   # 需要 Qt6 Core/Xml
cmake --build build/replay_check
build/replay_check/replay_check ../app_win/testdata/onset-consistency/low-1984.jsonl --expect-low-1984
```

`--expect-low-1984` 会检查以下几项与桌面端测试 `onsetGuardRejectsRecordedLowFrame` 一致：
- B_onset_inconsistent；
- BC 的 first_hit=1227、onset=1333；
- 去掉首波守卫后，SOS = 1983.805668。

`--stream captured.bin` 用抓下来的设备字节代替程序自己合成的字节流。

## fake_device.py：模拟下位机

协议和 Pico 2 固件一致。每收到一条 `A5 5A` 命令，就用日志里的下一帧回复 4 个通道（回显命令中的 gain 和 frameIndex），日志播完后从头循环。只依赖 Python 标准库。

```bash
python3 tools/fake_device.py ../app_win/testdata/onset-consistency/low-1984.jsonl --port 5555
# 在电脑上自测
python3 tools/fake_device_client.py --frames 13 --out captured.bin
build/replay_check/replay_check ../app_win/testdata/onset-consistency/low-1984.jsonl --expect-low-1984 --stream captured.bin
```

安卓模拟器通过 `10.0.2.2:5555` 访问电脑上的这个服务。
