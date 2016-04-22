/****************************************************************************
** Meta object code from reading C++ file 'addrules.h'
**
** Created: Thu Oct 17 12:29:40 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "addrules.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'addrules.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_kXneurApp__AddRules[] = {

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
      21,   20,   20,   20, 0x08,
      32,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_kXneurApp__AddRules[] = {
    "kXneurApp::AddRules\0\0saveData()\0"
    "closeForm()\0"
};

void kXneurApp::AddRules::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AddRules *_t = static_cast<AddRules *>(_o);
        switch (_id) {
        case 0: _t->saveData(); break;
        case 1: _t->closeForm(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData kXneurApp::AddRules::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject kXneurApp::AddRules::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_kXneurApp__AddRules,
      qt_meta_data_kXneurApp__AddRules, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &kXneurApp::AddRules::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *kXneurApp::AddRules::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *kXneurApp::AddRules::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_kXneurApp__AddRules))
        return static_cast<void*>(const_cast< AddRules*>(this));
    return QDialog::qt_metacast(_clname);
}

int kXneurApp::AddRules::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
