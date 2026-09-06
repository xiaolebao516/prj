/****************************************************************************
** Meta object code from reading C++ file 'calibrationdialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../include/calibrationdialog.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'calibrationdialog.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSCalibrationDialogENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSCalibrationDialogENDCLASS = QtMocHelpers::stringData(
    "CalibrationDialog",
    "acquisitionStartRequested",
    "",
    "processingD",
    "acquisitionStopRequested",
    "activeDChanged",
    "activeD",
    "goBack",
    "goNext",
    "startCalibrationMeasurement",
    "startValidationMeasurement",
    "activateCandidate",
    "restoreDefault",
    "restorePrevious",
    "showHistory",
    "updateModeFields"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSCalibrationDialogENDCLASS_t {
    uint offsetsAndSizes[32];
    char stringdata0[18];
    char stringdata1[26];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[25];
    char stringdata5[15];
    char stringdata6[8];
    char stringdata7[7];
    char stringdata8[7];
    char stringdata9[28];
    char stringdata10[27];
    char stringdata11[18];
    char stringdata12[15];
    char stringdata13[16];
    char stringdata14[12];
    char stringdata15[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSCalibrationDialogENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSCalibrationDialogENDCLASS_t qt_meta_stringdata_CLASSCalibrationDialogENDCLASS = {
    {
        QT_MOC_LITERAL(0, 17),  // "CalibrationDialog"
        QT_MOC_LITERAL(18, 25),  // "acquisitionStartRequested"
        QT_MOC_LITERAL(44, 0),  // ""
        QT_MOC_LITERAL(45, 11),  // "processingD"
        QT_MOC_LITERAL(57, 24),  // "acquisitionStopRequested"
        QT_MOC_LITERAL(82, 14),  // "activeDChanged"
        QT_MOC_LITERAL(97, 7),  // "activeD"
        QT_MOC_LITERAL(105, 6),  // "goBack"
        QT_MOC_LITERAL(112, 6),  // "goNext"
        QT_MOC_LITERAL(119, 27),  // "startCalibrationMeasurement"
        QT_MOC_LITERAL(147, 26),  // "startValidationMeasurement"
        QT_MOC_LITERAL(174, 17),  // "activateCandidate"
        QT_MOC_LITERAL(192, 14),  // "restoreDefault"
        QT_MOC_LITERAL(207, 15),  // "restorePrevious"
        QT_MOC_LITERAL(223, 11),  // "showHistory"
        QT_MOC_LITERAL(235, 16)   // "updateModeFields"
    },
    "CalibrationDialog",
    "acquisitionStartRequested",
    "",
    "processingD",
    "acquisitionStopRequested",
    "activeDChanged",
    "activeD",
    "goBack",
    "goNext",
    "startCalibrationMeasurement",
    "startValidationMeasurement",
    "activateCandidate",
    "restoreDefault",
    "restorePrevious",
    "showHistory",
    "updateModeFields"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCalibrationDialogENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   86,    2, 0x06,    1 /* Public */,
       4,    0,   89,    2, 0x06,    3 /* Public */,
       5,    1,   90,    2, 0x06,    4 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    0,   93,    2, 0x08,    6 /* Private */,
       8,    0,   94,    2, 0x08,    7 /* Private */,
       9,    0,   95,    2, 0x08,    8 /* Private */,
      10,    0,   96,    2, 0x08,    9 /* Private */,
      11,    0,   97,    2, 0x08,   10 /* Private */,
      12,    0,   98,    2, 0x08,   11 /* Private */,
      13,    0,   99,    2, 0x08,   12 /* Private */,
      14,    0,  100,    2, 0x08,   13 /* Private */,
      15,    0,  101,    2, 0x08,   14 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    6,

 // slots: parameters
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

Q_CONSTINIT const QMetaObject CalibrationDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSCalibrationDialogENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCalibrationDialogENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCalibrationDialogENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CalibrationDialog, std::true_type>,
        // method 'acquisitionStartRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'acquisitionStopRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'activeDChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'goBack'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'goNext'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startCalibrationMeasurement'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startValidationMeasurement'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'activateCandidate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'restoreDefault'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'restorePrevious'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showHistory'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateModeFields'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void CalibrationDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CalibrationDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->acquisitionStartRequested((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->acquisitionStopRequested(); break;
        case 2: _t->activeDChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 3: _t->goBack(); break;
        case 4: _t->goNext(); break;
        case 5: _t->startCalibrationMeasurement(); break;
        case 6: _t->startValidationMeasurement(); break;
        case 7: _t->activateCandidate(); break;
        case 8: _t->restoreDefault(); break;
        case 9: _t->restorePrevious(); break;
        case 10: _t->showHistory(); break;
        case 11: _t->updateModeFields(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CalibrationDialog::*)(double );
            if (_t _q_method = &CalibrationDialog::acquisitionStartRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CalibrationDialog::*)();
            if (_t _q_method = &CalibrationDialog::acquisitionStopRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CalibrationDialog::*)(double );
            if (_t _q_method = &CalibrationDialog::activeDChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *CalibrationDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalibrationDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCalibrationDialogENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int CalibrationDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void CalibrationDialog::acquisitionStartRequested(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CalibrationDialog::acquisitionStopRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void CalibrationDialog::activeDChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
