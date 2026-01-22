/****************************************************************************
** Meta object code from reading C++ file 'circle_fitter_view.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../circle_fitter_view.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'circle_fitter_view.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSCircleFitterViewENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSCircleFitterViewENDCLASS = QtMocHelpers::stringData(
    "CircleFitterView",
    "onPropBtnClickedSlots",
    "",
    "onPageChangedSlots",
    "onPropValueChanged",
    "key",
    "value"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSCircleFitterViewENDCLASS_t {
    uint offsetsAndSizes[14];
    char stringdata0[17];
    char stringdata1[22];
    char stringdata2[1];
    char stringdata3[19];
    char stringdata4[19];
    char stringdata5[4];
    char stringdata6[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSCircleFitterViewENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSCircleFitterViewENDCLASS_t qt_meta_stringdata_CLASSCircleFitterViewENDCLASS = {
    {
        QT_MOC_LITERAL(0, 16),  // "CircleFitterView"
        QT_MOC_LITERAL(17, 21),  // "onPropBtnClickedSlots"
        QT_MOC_LITERAL(39, 0),  // ""
        QT_MOC_LITERAL(40, 18),  // "onPageChangedSlots"
        QT_MOC_LITERAL(59, 18),  // "onPropValueChanged"
        QT_MOC_LITERAL(78, 3),  // "key"
        QT_MOC_LITERAL(82, 5)   // "value"
    },
    "CircleFitterView",
    "onPropBtnClickedSlots",
    "",
    "onPageChangedSlots",
    "onPropValueChanged",
    "key",
    "value"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCircleFitterViewENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   32,    2, 0x08,    1 /* Private */,
       3,    1,   35,    2, 0x08,    3 /* Private */,
       4,    2,   38,    2, 0x08,    5 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,    5,    6,

       0        // eod
};

Q_CONSTINIT const QMetaObject CircleFitterView::staticMetaObject = { {
    QMetaObject::SuperData::link<PageBase::staticMetaObject>(),
    qt_meta_stringdata_CLASSCircleFitterViewENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCircleFitterViewENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCircleFitterViewENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CircleFitterView, std::true_type>,
        // method 'onPropBtnClickedSlots'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'onPageChangedSlots'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'onPropValueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVariant, std::false_type>
    >,
    nullptr
} };

void CircleFitterView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CircleFitterView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onPropBtnClickedSlots((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->onPageChangedSlots((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->onPropValueChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *CircleFitterView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CircleFitterView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCircleFitterViewENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return PageBase::qt_metacast(_clname);
}

int CircleFitterView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PageBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
