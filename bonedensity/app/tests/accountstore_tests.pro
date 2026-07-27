QT += core testlib xml
CONFIG += c++17 testcase console
CONFIG -= app_bundle

TARGET = accountstore_tests
INCLUDEPATH += ../include

SOURCES += \
    accountstore_tests.cpp \
    ../src/accountstore.cpp

HEADERS += \
    ../include/accountstore.h
