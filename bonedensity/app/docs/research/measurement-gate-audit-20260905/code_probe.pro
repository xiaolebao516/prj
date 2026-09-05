QT += core gui widgets serialport charts printsupport xml
CONFIG += c++17 console debug
CONFIG -= app_bundle
TARGET = measurement_gate_code_probe
ROOT = $$clean_path($$PWD/../../..)
INCLUDEPATH += $$ROOT/include
SOURCES += $$PWD/code_probe.cpp
SOURCES += $$ROOT/src/mainwindow.cpp $$ROOT/src/accountstore.cpp \
    $$ROOT/src/calibration.cpp $$ROOT/src/calibrationdialog.cpp \
    $$ROOT/src/measurementguidedialog.cpp $$ROOT/src/calibrationstore.cpp \
    $$ROOT/src/patientstore.cpp $$ROOT/src/agesoschartwidget.cpp \
    $$ROOT/src/reportwidget.cpp $$ROOT/src/signalprocessor.cpp \
    $$ROOT/src/bonehealth.cpp $$ROOT/src/utils.cpp
HEADERS += $$ROOT/include/mainwindow.h $$ROOT/include/calibrationdialog.h \
    $$ROOT/include/measurementguidedialog.h $$ROOT/include/reportwidget.h
FORMS += $$ROOT/ui/mainwindow.ui
RESOURCES += $$ROOT/resources/resources.qrc
