QT = core
CONFIG += console c++17
CONFIG -= app_bundle debug_and_release
TEMPLATE = app
TARGET = b_wave_audit
INCLUDEPATH += ../../../include
SOURCES += b_wave_audit.cpp ../../../src/signalprocessor.cpp
