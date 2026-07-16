QT += core widgets testlib xml
CONFIG += c++17 testcase console
CONFIG -= app_bundle

TARGET = calibration_tests
INCLUDEPATH += ../include

SOURCES += \
    calibration_tests.cpp \
    ../src/calibration.cpp \
    ../src/calibrationdialog.cpp \
    ../src/calibrationstore.cpp

HEADERS += \
    ../include/calibration.h \
    ../include/calibrationdialog.h \
    ../include/calibrationstore.h
