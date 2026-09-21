/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/mainwindow.h"
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
    "on_btnBackToMain_clicked",
    "on_btnPatientNewSave_clicked",
    "on_btnImportFromDB_clicked",
    "updateCurrentPatientUI",
    "on_btnSaveResult_clicked",
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
    "on_btnFormSave_clicked",
    "on_btnFormBack_clicked",
    "on_btnDetailBack_clicked",
    "on_btnDetailSave_clicked",
    "on_btnDetailDelete_clicked",
    "on_btnShowResult_clicked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {
    uint offsetsAndSizes[70];
    char stringdata0[11];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[25];
    char stringdata5[25];
    char stringdata6[22];
    char stringdata7[18];
    char stringdata8[29];
    char stringdata9[6];
    char stringdata10[8];
    char stringdata11[22];
    char stringdata12[30];
    char stringdata13[26];
    char stringdata14[25];
    char stringdata15[29];
    char stringdata16[27];
    char stringdata17[23];
    char stringdata18[25];
    char stringdata19[22];
    char stringdata20[25];
    char stringdata21[23];
    char stringdata22[25];
    char stringdata23[18];
    char stringdata24[27];
    char stringdata25[4];
    char stringdata26[7];
    char stringdata27[29];
    char stringdata28[15];
    char stringdata29[23];
    char stringdata30[23];
    char stringdata31[25];
    char stringdata32[25];
    char stringdata33[27];
    char stringdata34[25];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMainWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMainWindowENDCLASS_t qt_meta_stringdata_CLASSMainWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 19),  // "on_btnLogin_clicked"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 9),  // "scanPorts"
        QT_MOC_LITERAL(42, 24),  // "on_connectButton_clicked"
        QT_MOC_LITERAL(67, 24),  // "on_triggerButton_clicked"
        QT_MOC_LITERAL(92, 21),  // "handleSerialReadyRead"
        QT_MOC_LITERAL(114, 17),  // "handleSerialError"
        QT_MOC_LITERAL(132, 28),  // "QSerialPort::SerialPortError"
        QT_MOC_LITERAL(161, 5),  // "error"
        QT_MOC_LITERAL(167, 7),  // "sendCmd"
        QT_MOC_LITERAL(175, 21),  // "on_btnArchive_clicked"
        QT_MOC_LITERAL(197, 29),  // "on_btnBackFromArchive_clicked"
        QT_MOC_LITERAL(227, 25),  // "on_btnPatientInfo_clicked"
        QT_MOC_LITERAL(253, 24),  // "on_btnBackToMain_clicked"
        QT_MOC_LITERAL(278, 28),  // "on_btnPatientNewSave_clicked"
        QT_MOC_LITERAL(307, 26),  // "on_btnImportFromDB_clicked"
        QT_MOC_LITERAL(334, 22),  // "updateCurrentPatientUI"
        QT_MOC_LITERAL(357, 24),  // "on_btnSaveResult_clicked"
        QT_MOC_LITERAL(382, 21),  // "on_btnShowAll_clicked"
        QT_MOC_LITERAL(404, 24),  // "on_btnSearchName_clicked"
        QT_MOC_LITERAL(429, 22),  // "on_btnSearchID_clicked"
        QT_MOC_LITERAL(452, 24),  // "on_btnSearchDate_clicked"
        QT_MOC_LITERAL(477, 17),  // "on_btnAdd_clicked"
        QT_MOC_LITERAL(495, 26),  // "on_table_cellDoubleClicked"
        QT_MOC_LITERAL(522, 3),  // "row"
        QT_MOC_LITERAL(526, 6),  // "column"
        QT_MOC_LITERAL(533, 28),  // "on_btnDeleteSelected_clicked"
        QT_MOC_LITERAL(562, 14),  // "updateDayCombo"
        QT_MOC_LITERAL(577, 22),  // "on_btnFormSave_clicked"
        QT_MOC_LITERAL(600, 22),  // "on_btnFormBack_clicked"
        QT_MOC_LITERAL(623, 24),  // "on_btnDetailBack_clicked"
        QT_MOC_LITERAL(648, 24),  // "on_btnDetailSave_clicked"
        QT_MOC_LITERAL(673, 26),  // "on_btnDetailDelete_clicked"
        QT_MOC_LITERAL(700, 24)   // "on_btnShowResult_clicked"
    },
    "MainWindow",
    "on_btnLogin_clicked",
    "",
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
    "on_btnBackToMain_clicked",
    "on_btnPatientNewSave_clicked",
    "on_btnImportFromDB_clicked",
    "updateCurrentPatientUI",
    "on_btnSaveResult_clicked",
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
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  188,    2, 0x08,    1 /* Private */,
       3,    0,  189,    2, 0x08,    2 /* Private */,
       4,    0,  190,    2, 0x08,    3 /* Private */,
       5,    0,  191,    2, 0x08,    4 /* Private */,
       6,    0,  192,    2, 0x08,    5 /* Private */,
       7,    1,  193,    2, 0x08,    6 /* Private */,
      10,    0,  196,    2, 0x08,    8 /* Private */,
      11,    0,  197,    2, 0x08,    9 /* Private */,
      12,    0,  198,    2, 0x08,   10 /* Private */,
      13,    0,  199,    2, 0x08,   11 /* Private */,
      14,    0,  200,    2, 0x08,   12 /* Private */,
      15,    0,  201,    2, 0x08,   13 /* Private */,
      16,    0,  202,    2, 0x08,   14 /* Private */,
      17,    0,  203,    2, 0x08,   15 /* Private */,
      18,    0,  204,    2, 0x08,   16 /* Private */,
      19,    0,  205,    2, 0x08,   17 /* Private */,
      20,    0,  206,    2, 0x08,   18 /* Private */,
      21,    0,  207,    2, 0x08,   19 /* Private */,
      22,    0,  208,    2, 0x08,   20 /* Private */,
      23,    0,  209,    2, 0x08,   21 /* Private */,
      24,    2,  210,    2, 0x08,   22 /* Private */,
      27,    0,  215,    2, 0x08,   25 /* Private */,
      28,    0,  216,    2, 0x08,   26 /* Private */,
      29,    0,  217,    2, 0x08,   27 /* Private */,
      30,    0,  218,    2, 0x08,   28 /* Private */,
      31,    0,  219,    2, 0x08,   29 /* Private */,
      32,    0,  220,    2, 0x08,   30 /* Private */,
      33,    0,  221,    2, 0x08,   31 /* Private */,
      34,    0,  222,    2, 0x08,   32 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
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
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   25,   26,
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
        case 1: _t->scanPorts(); break;
        case 2: _t->on_connectButton_clicked(); break;
        case 3: _t->on_triggerButton_clicked(); break;
        case 4: _t->handleSerialReadyRead(); break;
        case 5: _t->handleSerialError((*reinterpret_cast< std::add_pointer_t<QSerialPort::SerialPortError>>(_a[1]))); break;
        case 6: _t->sendCmd(); break;
        case 7: _t->on_btnArchive_clicked(); break;
        case 8: _t->on_btnBackFromArchive_clicked(); break;
        case 9: _t->on_btnPatientInfo_clicked(); break;
        case 10: _t->on_btnBackToMain_clicked(); break;
        case 11: _t->on_btnPatientNewSave_clicked(); break;
        case 12: _t->on_btnImportFromDB_clicked(); break;
        case 13: _t->updateCurrentPatientUI(); break;
        case 14: _t->on_btnSaveResult_clicked(); break;
        case 15: _t->on_btnShowAll_clicked(); break;
        case 16: _t->on_btnSearchName_clicked(); break;
        case 17: _t->on_btnSearchID_clicked(); break;
        case 18: _t->on_btnSearchDate_clicked(); break;
        case 19: _t->on_btnAdd_clicked(); break;
        case 20: _t->on_table_cellDoubleClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 21: _t->on_btnDeleteSelected_clicked(); break;
        case 22: _t->updateDayCombo(); break;
        case 23: _t->on_btnFormSave_clicked(); break;
        case 24: _t->on_btnFormBack_clicked(); break;
        case 25: _t->on_btnDetailBack_clicked(); break;
        case 26: _t->on_btnDetailSave_clicked(); break;
        case 27: _t->on_btnDetailDelete_clicked(); break;
        case 28: _t->on_btnShowResult_clicked(); break;
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
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 29;
    }
    return _id;
}
QT_WARNING_POP
