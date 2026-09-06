include(code_probe.pro)
TARGET = measurement_offline_replay
SOURCES -= $$PWD/code_probe.cpp
SOURCES += $$PWD/offline_replay.cpp
