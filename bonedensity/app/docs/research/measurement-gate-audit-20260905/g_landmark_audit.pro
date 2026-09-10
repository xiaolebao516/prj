QT = core
CONFIG += console c++17
CONFIG -= app_bundle debug_and_release
TEMPLATE = app
TARGET = g_landmark_audit
INCLUDEPATH += ../../../include
SOURCES += g_landmark_audit.cpp ../../../src/signalprocessor.cpp
