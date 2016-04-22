/****************************************************************************
** Meta object code from reading C++ file 'xneurconfig.h'
**
** Created: Thu Oct 17 12:29:38 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "xneurconfig.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'xneurconfig.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_kXneurApp__xNeurConfig[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x05,

 // slots: signature, parameters, type, tag, flags
      45,   23,   23,   23, 0x08,
      64,   62,   23,   23, 0x08,
     104,   23,   23,   23, 0x08,
     122,   23,   23,   23, 0x0a,
     136,   23,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_kXneurApp__xNeurConfig[] = {
    "kXneurApp::xNeurConfig\0\0setStatusXneur(bool)\0"
    "procxNeurStart()\0,\0"
    "procxNeurStop(int,QProcess::ExitStatus)\0"
    "procxNeurOutput()\0setNextLang()\0"
    "restartNeur()\0"
};

void kXneurApp::xNeurConfig::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        xNeurConfig *_t = static_cast<xNeurConfig *>(_o);
        switch (_id) {
        case 0: _t->setStatusXneur((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->procxNeurStart(); break;
        case 2: _t->procxNeurStop((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 3: _t->procxNeurOutput(); break;
        case 4: _t->setNextLang(); break;
        case 5: _t->restartNeur(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData kXneurApp::xNeurConfig::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject kXneurApp::xNeurConfig::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_kXneurApp__xNeurConfig,
      qt_meta_data_kXneurApp__xNeurConfig, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &kXneurApp::xNeurConfig::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *kXneurApp::xNeurConfig::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *kXneurApp::xNeurConfig::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_kXneurApp__xNeurConfig))
        return static_cast<void*>(const_cast< xNeurConfig*>(this));
    return QObject::qt_metacast(_clname);
}

int kXneurApp::xNeurConfig::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void kXneurApp::xNeurConfig::setStatusXneur(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
