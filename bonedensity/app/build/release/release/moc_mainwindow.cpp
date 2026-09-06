/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCharts/qlineseries.h>
#include <QtCharts/qabstractbarseries.h>
#include <QtCharts/qvbarmodelmapper.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCharts/qcandlestickseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qxyseries.h>
#include <QtCharts/qxyseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qxyseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "on_btnLogin_clicked",
    "",
    "manageAccounts",
    "scanPorts",
    "on_connectButton_clicked",
    "on_triggerButton_clicked",
    "handleSerialReadyRead",
    "handleSerialError",
    "QSerialPort::SerialPortError",
    "error",
    "sendCmd",
    "on_btnArchive_clicked",
    "on_btnBackFromArchive_clicked",
    "on_btnPatientInfo_clicked",
    "on_btnStartMeasurement_clicked",
    "on_btnBackToMain_clicked",
    "on_btnPatientNewSave_clicked",
    "on_btnImportFromDB_clicked",
    "updateCurrentPatientUI",
    "on_btnSaveResult_clicked",
    "on_btnSelectPatient_clicked",
    "on_btnViewHistory_clicked",
    "on_btnShowAll_clicked",
    "on_btnSearchName_clicked",
    "on_btnSearchID_clicked",
    "on_btnSearchDate_clicked",
    "on_btnAdd_clicked",
    "on_table_cellDoubleClicked",
    "row",
    "column",
    "on_btnDeleteSelected_clicked",
    "updateDayCombo",
    "scheduleResponsiveLayout",
    "on_btnFormSave_clicked",
    "on_btnFormBack_clicked",
    "on_btnDetailBack_clicked",
    "on_btnDetailSave_clicked",
    "on_btnDetailDelete_clicked",
    "on_btnShowResult_clicked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {
    uint offsetsAndSizes[80];
    char stringdata0[11];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[10];
    char stringdata5[25];
    char stringdata6[25];
    char stringdata7[22];
    char stringdata8[18];
    char stringdata9[29];
    char stringdata10[6];
    char stringdata11[8];
    char stringdata12[22];
    char stringdata13[30];
    char stringdata14[26];
    char stringdata15[31];
    char stringdata16[25];
    char stringdata17[29];
    char stringdata18[27];
    char stringdata19[23];
    char stringdata20[25];
    char stringdata21[28];
    char stringdata22[26];
    char stringdata23[22];
    char stringdata24[25];
    char stringdata25[23];
    char stringdata26[25];
    char stringdata27[18];
    char stringdata28[27];
    char stringdata29[4];
    char stringdata30[7];
    char stringdata31[29];
    char stringdata32[15];
    char stringdata33[25];
    char stringdata34[23];
    char stringdata35[23];
    char stringdata36[25];
    char stringdata37[25];
    char stringdata38[27];
    char stringdata39[25];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMainWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMainWindowENDCLASS_t qt_meta_stringdata_CLASSMainWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 19),  // "on_btnLogin_clicked"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 14),  // "manageAccounts"
        QT_MOC_LITERAL(47, 9),  // "scanPorts"
        QT_MOC_LITERAL(57, 24),  // "on_connectButton_clicked"
        QT_MOC_LITERAL(82, 24),  // "on_triggerButton_clicked"
        QT_MOC_LITERAL(107, 21),  // "handleSerialReadyRead"
        QT_MOC_LITERAL(129, 17),  // "handleSerialError"
        QT_MOC_LITERAL(147, 28),  // "QSerialPort::SerialPortError"
        QT_MOC_LITERAL(176, 5),  // "error"
        QT_MOC_LITERAL(182, 7),  // "sendCmd"
        QT_MOC_LITERAL(190, 21),  // "on_btnArchive_clicked"
        QT_MOC_LITERAL(212, 29),  // "on_btnBackFromArchive_clicked"
        QT_MOC_LITERAL(242, 25),  // "on_btnPatientInfo_clicked"
        QT_MOC_LITERAL(268, 30),  // "on_btnStartMeasurement_clicked"
        QT_MOC_LITERAL(299, 24),  // "on_btnBackToMain_clicked"
        QT_MOC_LITERAL(324, 28),  // "on_btnPatientNewSave_clicked"
        QT_MOC_LITERAL(353, 26),  // "on_btnImportFromDB_clicked"
        QT_MOC_LITERAL(380, 22),  // "updateCurrentPatientUI"
        QT_MOC_LITERAL(403, 24),  // "on_btnSaveResult_clicked"
        QT_MOC_LITERAL(428, 27),  // "on_btnSelectPatient_clicked"
        QT_MOC_LITERAL(456, 25),  // "on_btnViewHistory_clicked"
        QT_MOC_LITERAL(482, 21),  // "on_btnShowAll_clicked"
        QT_MOC_LITERAL(504, 24),  // "on_btnSearchName_clicked"
        QT_MOC_LITERAL(529, 22),  // "on_btnSearchID_clicked"
        QT_MOC_LITERAL(552, 24),  // "on_btnSearchDate_clicked"
        QT_MOC_LITERAL(577, 17),  // "on_btnAdd_clicked"
        QT_MOC_LITERAL(595, 26),  // "on_table_cellDoubleClicked"
        QT_MOC_LITERAL(622, 3),  // "row"
        QT_MOC_LITERAL(626, 6),  // "column"
        QT_MOC_LITERAL(633, 28),  // "on_btnDeleteSelected_clicked"
        QT_MOC_LITERAL(662, 14),  // "updateDayCombo"
        QT_MOC_LITERAL(677, 24),  // "scheduleResponsiveLayout"
        QT_MOC_LITERAL(702, 22),  // "on_btnFormSave_clicked"
        QT_MOC_LITERAL(725, 22),  // "on_btnFormBack_clicked"
        QT_MOC_LITERAL(748, 24),  // "on_btnDetailBack_clicked"
        QT_MOC_LITERAL(773, 24),  // "on_btnDetailSave_clicked"
        QT_MOC_LITERAL(798, 26),  // "on_btnDetailDelete_clicked"
        QT_MOC_LITERAL(825, 24)   // "on_btnShowResult_clicked"
    },
    "MainWindow",
    "on_btnLogin_clicked",
    "",
    "manageAccounts",
    "scanPorts",
    "on_connectButton_clicked",
    "on_triggerButton_clicked",
    "handleSerialReadyRead",
    "handleSerialError",
    "QSerialPort::SerialPortError",
    "error",
    "sendCmd",
    "on_btnArchive_clicked",
    "on_btnBackFromArchive_clicked",
    "on_btnPatientInfo_clicked",
    "on_btnStartMeasurement_clicked",
    "on_btnBackToMain_clicked",
    "on_btnPatientNewSave_clicked",
    "on_btnImportFromDB_clicked",
    "updateCurrentPatientUI",
    "on_btnSaveResult_clicked",
    "on_btnSelectPatient_clicked",
    "on_btnViewHistory_clicked",
    "on_btnShowAll_clicked",
    "on_btnSearchName_clicked",
    "on_btnSearchID_clicked",
    "on_btnSearchDate_clicked",
    "on_btnAdd_clicked",
    "on_table_cellDoubleClicked",
    "row",
    "column",
    "on_btnDeleteSelected_clicked",
    "updateDayCombo",
    "scheduleResponsiveLayout",
    "on_btnFormSave_clicked",
    "on_btnFormBack_clicked",
    "on_btnDetailBack_clicked",
    "on_btnDetailSave_clicked",
    "on_btnDetailDelete_clicked",
    "on_btnShowResult_clicked"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      34,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  218,    2, 0x08,    1 /* Private */,
       3,    0,  219,    2, 0x08,    2 /* Private */,
       4,    0,  220,    2, 0x08,    3 /* Private */,
       5,    0,  221,    2, 0x08,    4 /* Private */,
       6,    0,  222,    2, 0x08,    5 /* Private */,
       7,    0,  223,    2, 0x08,    6 /* Private */,
       8,    1,  224,    2, 0x08,    7 /* Private */,
      11,    0,  227,    2, 0x08,    9 /* Private */,
      12,    0,  228,    2, 0x08,   10 /* Private */,
      13,    0,  229,    2, 0x08,   11 /* Private */,
      14,    0,  230,    2, 0x08,   12 /* Private */,
      15,    0,  231,    2, 0x08,   13 /* Private */,
      16,    0,  232,    2, 0x08,   14 /* Private */,
      17,    0,  233,    2, 0x08,   15 /* Private */,
      18,    0,  234,    2, 0x08,   16 /* Private */,
      19,    0,  235,    2, 0x08,   17 /* Private */,
      20,    0,  236,    2, 0x08,   18 /* Private */,
      21,    0,  237,    2, 0x08,   19 /* Private */,
      22,    0,  238,    2, 0x08,   20 /* Private */,
      23,    0,  239,    2, 0x08,   21 /* Private */,
      24,    0,  240,    2, 0x08,   22 /* Private */,
      25,    0,  241,    2, 0x08,   23 /* Private */,
      26,    0,  242,    2, 0x08,   24 /* Private */,
      27,    0,  243,    2, 0x08,   25 /* Private */,
      28,    2,  244,    2, 0x08,   26 /* Private */,
      31,    0,  249,    2, 0x08,   29 /* Private */,
      32,    0,  250,    2, 0x08,   30 /* Private */,
      33,    0,  251,    2, 0x08,   31 /* Private */,
      34,    0,  252,    2, 0x08,   32 /* Private */,
      35,    0,  253,    2, 0x08,   33 /* Private */,
      36,    0,  254,    2, 0x08,   34 /* Private */,
      37,    0,  255,    2, 0x08,   35 /* Private */,
      38,    0,  256,    2, 0x08,   36 /* Private */,
      39,    0,  257,    2, 0x08,   37 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   29,   30,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'on_btnLogin_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'manageAccounts'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'scanPorts'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_connectButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_triggerButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleSerialReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleSerialError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QSerialPort::SerialPortError, std::false_type>,
        // method 'sendCmd'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnArchive_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnBackFromArchive_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnPatientInfo_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnStartMeasurement_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnBackToMain_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnPatientNewSave_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnImportFromDB_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateCurrentPatientUI'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSaveResult_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSelectPatient_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnViewHistory_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnShowAll_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSearchName_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSearchID_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSearchDate_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnAdd_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_table_cellDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_btnDeleteSelected_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateDayCombo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'scheduleResponsiveLayout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnFormSave_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnFormBack_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnDetailBack_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnDetailSave_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnDetailDelete_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnShowResult_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_btnLogin_clicked(); break;
        case 1: _t->manageAccounts(); break;
        case 2: _t->scanPorts(); break;
        case 3: _t->on_connectButton_clicked(); break;
        case 4: _t->on_triggerButton_clicked(); break;
        case 5: _t->handleSerialReadyRead(); break;
        case 6: _t->handleSerialError((*reinterpret_cast< std::add_pointer_t<QSerialPort::SerialPortError>>(_a[1]))); break;
        case 7: _t->sendCmd(); break;
        case 8: _t->on_btnArchive_clicked(); break;
        case 9: _t->on_btnBackFromArchive_clicked(); break;
        case 10: _t->on_btnPatientInfo_clicked(); break;
        case 11: _t->on_btnStartMeasurement_clicked(); break;
        case 12: _t->on_btnBackToMain_clicked(); break;
        case 13: _t->on_btnPatientNewSave_clicked(); break;
        case 14: _t->on_btnImportFromDB_clicked(); break;
        case 15: _t->updateCurrentPatientUI(); break;
        case 16: _t->on_btnSaveResult_clicked(); break;
        case 17: _t->on_btnSelectPatient_clicked(); break;
        case 18: _t->on_btnViewHistory_clicked(); break;
        case 19: _t->on_btnShowAll_clicked(); break;
        case 20: _t->on_btnSearchName_clicked(); break;
        case 21: _t->on_btnSearchID_clicked(); break;
        case 22: _t->on_btnSearchDate_clicked(); break;
        case 23: _t->on_btnAdd_clicked(); break;
        case 24: _t->on_table_cellDoubleClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 25: _t->on_btnDeleteSelected_clicked(); break;
        case 26: _t->updateDayCombo(); break;
        case 27: _t->scheduleResponsiveLayout(); break;
        case 28: _t->on_btnFormSave_clicked(); break;
        case 29: _t->on_btnFormBack_clicked(); break;
        case 30: _t->on_btnDetailBack_clicked(); break;
        case 31: _t->on_btnDetailSave_clicked(); break;
        case 32: _t->on_btnDetailDelete_clicked(); break;
        case 33: _t->on_btnShowResult_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 34)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 34;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 34)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 34;
    }
    return _id;
}
QT_WARNING_POP
