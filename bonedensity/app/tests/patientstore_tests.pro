QT += core testlib xml
CONFIG += c++17 testcase console
CONFIG -= app_bundle

TARGET = patientstore_tests
INCLUDEPATH += ../include

SOURCES += \
    patientstore_tests.cpp \
    ../src/patientstore.cpp

HEADERS += \
    ../include/patientstore.h \
    ../include/types.h
