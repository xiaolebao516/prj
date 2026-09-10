include(code_probe.pro)
CONFIG -= debug debug_and_release
CONFIG += release
DEFINES += BONE_COMPLETE_B_PEAK_EXPERIMENT
TARGET = b_peak_full_replay
SOURCES -= $$PWD/code_probe.cpp
SOURCES += $$PWD/b_peak_full_replay.cpp
