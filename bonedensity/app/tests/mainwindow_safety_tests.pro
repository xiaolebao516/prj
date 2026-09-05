QT += core gui widgets serialport charts printsupport testlib xml
CONFIG += c++17 testcase console
CONFIG -= app_bundle

TARGET = mainwindow_safety_tests
INCLUDEPATH += ../include

SOURCES += \
    mainwindow_safety_tests.cpp \
    ../src/mainwindow.cpp \
    ../src/accountstore.cpp \
    ../src/calibration.cpp \
    ../src/calibrationdialog.cpp \
    ../src/measurementguidedialog.cpp \
    ../src/calibrationstore.cpp \
    ../src/patientstore.cpp \
    ../src/agesoschartwidget.cpp \
    ../src/reportwidget.cpp \
    ../src/signalprocessor.cpp \
    ../src/bonehealth.cpp \
    ../src/utils.cpp

HEADERS += \
    ../include/measurementexperimentlog.h \
    ../include/mainwindow.h \
    ../include/accountstore.h \
    ../include/calibration.h \
    ../include/calibrationdialog.h \
    ../include/measurementguidedialog.h \
    ../include/calibrationstore.h \
    ../include/patientstore.h \
    ../include/agesoschartwidget.h \
    ../include/reportwidget.h \
    ../include/types.h \
    ../include/signalprocessor.h \
    ../include/bonehealth.h \
    ../include/utils.h

FORMS += ../ui/mainwindow.ui
RESOURCES += ../resources/resources.qrc
