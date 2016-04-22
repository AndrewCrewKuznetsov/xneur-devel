/****************************************************************************
** Meta object code from reading C++ file 'getnameapp.h'
**
** Created: Thu Oct 17 12:29:39 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "getnameapp.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'getnameapp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_kXneurApp__getNameApp[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x08,
      49,   22,   22,   22, 0x08,
      58,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_kXneurApp__getNameApp[] = {
    "kXneurApp::getNameApp\0\0Clicked(QAbstractButton*)\0"
    "getApp()\0readOutput()\0"
};

void kXneurApp::getNameApp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        getNameApp *_t = static_cast<getNameApp *>(_o);
        switch (_id) {
        case 0: _t->Clicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 1: _t->getApp(); break;
        case 2: _t->readOutput(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData kXneurApp::getNameApp::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject kXneurApp::getNameApp::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_kXneurApp__getNameApp,
      qt_meta_data_kXneurApp__getNameApp, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &kXneurApp::getNameApp::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *kXneurApp::getNameApp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *kXneurApp::getNameApp::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_kXneurApp__getNameApp))
        return static_cast<void*>(const_cast< getNameApp*>(this));
    return QDialog::qt_metacast(_clname);
}

int kXneurApp::getNameApp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
