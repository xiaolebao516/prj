include(code_probe.pro)
TARGET = warmup_policy_replay
SOURCES -= $$PWD/code_probe.cpp
SOURCES += $$PWD/warmup_policy_replay.cpp
