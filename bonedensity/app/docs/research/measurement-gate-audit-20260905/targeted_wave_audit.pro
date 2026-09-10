QT = core
CONFIG += console c++17
CONFIG -= app_bundle debug_and_release
TEMPLATE = app
TARGET = targeted_wave_audit
INCLUDEPATH += ../../../include
SOURCES += targeted_wave_audit.cpp ../../../src/signalprocessor.cpp
