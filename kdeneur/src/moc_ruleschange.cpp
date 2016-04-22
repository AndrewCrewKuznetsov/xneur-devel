/****************************************************************************
** Meta object code from reading C++ file 'ruleschange.h'
**
** Created: Thu Oct 17 12:29:39 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ruleschange.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ruleschange.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_kXneurApp__RulesChange[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x08,
      36,   23,   23,   23, 0x08,
      47,   23,   23,   23, 0x08,
      58,   23,   23,   23, 0x08,
      69,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_kXneurApp__RulesChange[] = {
    "kXneurApp::RulesChange\0\0closeForm()\0"
    "addWords()\0editWors()\0delWords()\0"
    "saveDict()\0"
};

void kXneurApp::RulesChange::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RulesChange *_t = static_cast<RulesChange *>(_o);
        switch (_id) {
        case 0: _t->closeForm(); break;
        case 1: _t->addWords(); break;
        case 2: _t->editWors(); break;
        case 3: _t->delWords(); break;
        case 4: _t->saveDict(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData kXneurApp::RulesChange::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject kXneurApp::RulesChange::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_kXneurApp__RulesChange,
      qt_meta_data_kXneurApp__RulesChange, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &kXneurApp::RulesChange::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *kXneurApp::RulesChange::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *kXneurApp::RulesChange::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_kXneurApp__RulesChange))
        return static_cast<void*>(const_cast< RulesChange*>(this));
    return QDialog::qt_metacast(_clname);
}

int kXneurApp::RulesChange::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
