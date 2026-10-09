QT += testlib
include(tests.pri)
include(../src/core/core.pri)

QT += widgets
INCLUDEPATH += ../src/ui
HEADERS += ../src/ui/dialogs/calibrationdialog.h
SOURCES += ../src/ui/dialogs/calibrationdialog.cpp

TARGET = calibration_tests
SOURCES += calibration_tests.cpp
