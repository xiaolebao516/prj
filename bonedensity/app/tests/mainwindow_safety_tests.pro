QT += core gui widgets serialport charts printsupport testlib xml
include(../version.pri)
CONFIG += c++17 testcase console
CONFIG -= app_bundle

TARGET = mainwindow_safety_tests
INCLUDEPATH += ../include

SOURCES += \
    mainwindow_safety_tests.cpp \
    ../src/mainwindow.cpp \
    ../src/mainwindow_device.cpp \
    ../src/mainwindow_measurement.cpp \
    ../src/mainwindow_display.cpp \
    ../src/mainwindow_patients.cpp \
    ../src/mainwindow_report.cpp \
    ../src/mainwindow_layout.cpp \
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
    ../src/sosreference.cpp \
    ../src/databackup.cpp \
    ../src/legacyimport.cpp \
    ../src/parametergroup.cpp \
    ../src/datalocation.cpp \
    ../src/utils.cpp \
    ../src/patientformdialog.cpp

HEADERS += \
    ../include/measurementexperimentlog.h \
    ../include/mainwindow.h \
    ../include/mainwindow_internal.h \
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
    ../include/sosreference.h \
    ../include/databackup.h \
    ../include/legacyimport.h \
    ../include/parametergroup.h \
    ../include/datalocation.h \
    ../include/utils.h \
    ../include/patientformdialog.h

FORMS += ../ui/mainwindow.ui
RESOURCES += ../resources/resources.qrc
