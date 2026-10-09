# UI: the main window, its dialogs and widgets. Pulls in the core; include
# this file or core.pri, never both.
include(../core/core.pri)

QT += gui widgets serialport charts printsupport
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/mainwindow/mainwindow.h \
    $$PWD/mainwindow/mainwindow_internal.h \
    $$PWD/theme/theme.h \
    $$PWD/widgets/uikit.h \
    $$PWD/widgets/agesoschartwidget.h \
    $$PWD/widgets/reportwidget.h \
    $$PWD/dialogs/calibrationdialog.h \
    $$PWD/dialogs/measurementguidedialog.h \
    $$PWD/dialogs/patientformdialog.h

SOURCES += \
    $$PWD/mainwindow/mainwindow.cpp \
    $$PWD/mainwindow/mainwindow_device.cpp \
    $$PWD/mainwindow/mainwindow_measurement.cpp \
    $$PWD/mainwindow/mainwindow_display.cpp \
    $$PWD/mainwindow/mainwindow_patients.cpp \
    $$PWD/mainwindow/mainwindow_report.cpp \
    $$PWD/mainwindow/mainwindow_layout.cpp \
    $$PWD/theme/theme.cpp \
    $$PWD/widgets/uikit.cpp \
    $$PWD/widgets/agesoschartwidget.cpp \
    $$PWD/widgets/reportwidget.cpp \
    $$PWD/dialogs/calibrationdialog.cpp \
    $$PWD/dialogs/measurementguidedialog.cpp \
    $$PWD/dialogs/patientformdialog.cpp

FORMS += $$PWD/mainwindow/mainwindow.ui

RESOURCES += $$PWD/../../resources/resources.qrc
