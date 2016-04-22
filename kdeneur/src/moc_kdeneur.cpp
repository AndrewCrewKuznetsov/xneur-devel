/****************************************************************************
** Meta object code from reading C++ file 'kdeneur.h'
**
** Created: Thu Oct 17 12:29:37 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "kdeneur.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kdeneur.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_kXneurApp__kXneur[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      43,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      56,   18,   18,   18, 0x08,
      79,   18,   18,   18, 0x08,
      95,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_kXneurApp__kXneur[] = {
    "kXneurApp::kXneur\0\0changeIconTray(QString)\0"
    "reLoadNeur()\0layoutChanged(QString)\0"
    "startStopNeur()\0OpenSettings()\0"
};

void kXneurApp::kXneur::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        kXneur *_t = static_cast<kXneur *>(_o);
        switch (_id) {
        case 0: _t->changeIconTray((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->reLoadNeur(); break;
        case 2: _t->layoutChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->startStopNeur(); break;
        case 4: _t->OpenSettings(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData kXneurApp::kXneur::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject kXneurApp::kXneur::staticMetaObject = {
    { &QApplication::staticMetaObject, qt_meta_stringdata_kXneurApp__kXneur,
      qt_meta_data_kXneurApp__kXneur, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &kXneurApp::kXneur::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *kXneurApp::kXneur::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *kXneurApp::kXneur::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_kXneurApp__kXneur))
        return static_cast<void*>(const_cast< kXneur*>(this));
    return QApplication::qt_metacast(_clname);
}

int kXneurApp::kXneur::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void kXneurApp::kXneur::changeIconTray(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void kXneurApp::kXneur::reLoadNeur()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
