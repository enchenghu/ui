/****************************************************************************
** Meta object code from reading C++ file 'osc_chart.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/arbe_gui/src/arbe_gui_main/osc_chart.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'osc_chart.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_OSC_chart_t {
    QByteArrayData data[12];
    char stringdata0[123];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OSC_chart_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OSC_chart_t qt_meta_stringdata_OSC_chart = {
    {
QT_MOC_LITERAL(0, 0, 9), // "OSC_chart"
QT_MOC_LITERAL(1, 10, 7), // "clicked"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 12), // "QMouseEvent*"
QT_MOC_LITERAL(4, 32, 5), // "event"
QT_MOC_LITERAL(5, 38, 7), // "moveing"
QT_MOC_LITERAL(6, 46, 13), // "doubleClicked"
QT_MOC_LITERAL(7, 60, 11), // "slotClicked"
QT_MOC_LITERAL(8, 72, 11), // "slotmoveing"
QT_MOC_LITERAL(9, 84, 17), // "slotdoubleClicked"
QT_MOC_LITERAL(10, 102, 11), // "updateViews"
QT_MOC_LITERAL(11, 114, 8) // "slotshow"

    },
    "OSC_chart\0clicked\0\0QMouseEvent*\0event\0"
    "moveing\0doubleClicked\0slotClicked\0"
    "slotmoveing\0slotdoubleClicked\0updateViews\0"
    "slotshow"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OSC_chart[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       5,    1,   57,    2, 0x06 /* Public */,
       6,    1,   60,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   63,    2, 0x0a /* Public */,
       8,    1,   66,    2, 0x0a /* Public */,
       9,    1,   69,    2, 0x0a /* Public */,
      10,    0,   72,    2, 0x0a /* Public */,
      11,    0,   73,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void OSC_chart::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OSC_chart *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->clicked((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 1: _t->moveing((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 2: _t->doubleClicked((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 3: _t->slotClicked((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 4: _t->slotmoveing((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 5: _t->slotdoubleClicked((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 6: _t->updateViews(); break;
        case 7: _t->slotshow(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OSC_chart::*)(QMouseEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OSC_chart::clicked)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (OSC_chart::*)(QMouseEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OSC_chart::moveing)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (OSC_chart::*)(QMouseEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OSC_chart::doubleClicked)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject OSC_chart::staticMetaObject = { {
    &QLabel::staticMetaObject,
    qt_meta_stringdata_OSC_chart.data,
    qt_meta_data_OSC_chart,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *OSC_chart::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OSC_chart::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OSC_chart.stringdata0))
        return static_cast<void*>(this);
    return QLabel::qt_metacast(_clname);
}

int OSC_chart::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void OSC_chart::clicked(QMouseEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void OSC_chart::moveing(QMouseEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void OSC_chart::doubleClicked(QMouseEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
