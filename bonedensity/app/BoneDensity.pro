    QT       += core gui
QT += core gui widgets serialport charts printsupport
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = BoneDensity

INCLUDEPATH += include

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/accountstore.cpp \
    src/calibration.cpp \
    src/calibrationdialog.cpp \
    src/measurementguidedialog.cpp \
    src/calibrationstore.cpp \
    src/patientstore.cpp \
    src/agesoschartwidget.cpp \
    src/reportwidget.cpp \
    src/signalprocessor.cpp \
    src/bonehealth.cpp \
    src/utils.cpp

HEADERS += \
    include/measurementexperimentlog.h \
    include/mainwindow.h \
    include/accountstore.h \
    include/calibration.h \
    include/calibrationdialog.h \
    include/measurementguidedialog.h \
    include/calibrationstore.h \
    include/patientstore.h \
    include/agesoschartwidget.h \
    include/reportwidget.h \
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
