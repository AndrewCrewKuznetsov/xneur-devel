/****************************************************************************
** Meta object code from reading C++ file 'edithotkey.h'
**
** Created: Thu Oct 17 12:29:41 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "edithotkey.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'edithotkey.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_kXneurApp__EditHotKey[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x08,
      35,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_kXneurApp__EditHotKey[] = {
    "kXneurApp::EditHotKey\0\0closeForm()\0"
    "save()\0"
};

void kXneurApp::EditHotKey::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditHotKey *_t = static_cast<EditHotKey *>(_o);
        switch (_id) {
        case 0: _t->closeForm(); break;
        case 1: _t->save(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData kXneurApp::EditHotKey::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject kXneurApp::EditHotKey::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_kXneurApp__EditHotKey,
      qt_meta_data_kXneurApp__EditHotKey, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &kXneurApp::EditHotKey::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *kXneurApp::EditHotKey::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *kXneurApp::EditHotKey::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_kXneurApp__EditHotKey))
        return static_cast<void*>(const_cast< EditHotKey*>(this));
    return QDialog::qt_metacast(_clname);
}

int kXneurApp::EditHotKey::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
