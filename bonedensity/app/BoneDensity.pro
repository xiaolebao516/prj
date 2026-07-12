    QT       += core gui
QT += core gui widgets serialport charts
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = BoneDensity

INCLUDEPATH += include

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/accountstore.cpp \
    src/patientstore.cpp \
    src/signalprocessor.cpp \
    src/bonehealth.cpp \
    src/utils.cpp

HEADERS += \
    include/mainwindow.h \
    include/accountstore.h \
    include/patientstore.h \
    include/types.h \
    include/signalprocessor.h \
    include/bonehealth.h \
    include/utils.h

FORMS += \
    ui/mainwindow.ui

RESOURCES += \
    resources/resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
