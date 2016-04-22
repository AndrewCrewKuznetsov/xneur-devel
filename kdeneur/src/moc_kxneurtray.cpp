/****************************************************************************
** Meta object code from reading C++ file 'kxneurtray.h'
**
** Created: Thu Oct 17 12:29:34 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "kxneurtray.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kxneurtray.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_kXneurApp__kXneurTray[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,
      42,   22,   22,   22, 0x05,
      57,   22,   22,   22, 0x05,
      71,   22,   22,   22, 0x05,
      82,   22,   22,   22, 0x05,
      92,   22,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
     107,   22,   22,   22, 0x0a,
     133,   22,   22,   22, 0x0a,
     147,   22,   22,   22, 0x0a,
     168,   22,   22,   22, 0x0a,
     182,   22,   22,   22, 0x0a,
     196,   22,   22,   22, 0x0a,
     212,   22,   22,   22, 0x0a,
     259,   22,   22,   22, 0x0a,
     280,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_kXneurApp__kXneurTray[] = {
    "kXneurApp::kXneurTray\0\0statusDaemon(bool)\0"
    "statusDaemon()\0restartNeur()\0nextLang()\0"
    "exitApp()\0openSettings()\0"
    "setTrayIconFlags(QString)\0kXneurAbout()\0"
    "keyboardProperties()\0showJournal()\0"
    "settingsApp()\0startStopNeur()\0"
    "trayClicked(QSystemTrayIcon::ActivationReason)\0"
    "setStatusXneur(bool)\0runUserActions()\0"
};

void kXneurApp::kXneurTray::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        kXneurTray *_t = static_cast<kXneurTray *>(_o);
        switch (_id) {
        case 0: _t->statusDaemon((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->statusDaemon(); break;
        case 2: _t->restartNeur(); break;
        case 3: _t->nextLang(); break;
        case 4: _t->exitApp(); break;
        case 5: _t->openSettings(); break;
        case 6: _t->setTrayIconFlags((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->kXneurAbout(); break;
        case 8: _t->keyboardProperties(); break;
        case 9: _t->showJournal(); break;
        case 10: _t->settingsApp(); break;
        case 11: _t->startStopNeur(); break;
        case 12: _t->trayClicked((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 13: _t->setStatusXneur((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->runUserActions(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData kXneurApp::kXneurTray::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject kXneurApp::kXneurTray::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_kXneurApp__kXneurTray,
      qt_meta_data_kXneurApp__kXneurTray, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &kXneurApp::kXneurTray::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *kXneurApp::kXneurTray::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *kXneurApp::kXneurTray::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_kXneurApp__kXneurTray))
        return static_cast<void*>(const_cast< kXneurTray*>(this));
    return QObject::qt_metacast(_clname);
}

int kXneurApp::kXneurTray::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void kXneurApp::kXneurTray::statusDaemon(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void kXneurApp::kXneurTray::statusDaemon()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void kXneurApp::kXneurTray::restartNeur()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void kXneurApp::kXneurTray::nextLang()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void kXneurApp::kXneurTray::exitApp()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void kXneurApp::kXneurTray::openSettings()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}
QT_END_MOC_NAMESPACE
