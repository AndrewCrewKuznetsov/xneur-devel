//app header files
#include "kdeneur.h"
#include "frmsettings.h"

//Kde header files
#include <ktoolinvocation.h>
#include <kstandarddirs.h>

//Qt header files
#include <QDBusConnection>
#include <QDebug>
#include <QTranslator>

kXneurApp::kXneur::kXneur(int& argc, char **argv): QApplication (argc, argv)
{
    QTranslator *transApp = new QTranslator(this);
    transApp->load(QString("%1/kdeneur_%2.qm").arg(LOCALEDIR).arg(QLocale::system().name()));
    qDebug() << "FILE TRANSLATE " << QString("%1/kdeneur_%2.qm").arg(LOCALEDIR).arg(QLocale::system().name());
    installTranslator(transApp);
    cfgXneur = new kXneurApp::xNeurConfig();
    settignsTray();
    int xneur_pid=cfgXneur->getNeur_pid();
    if(xneur_pid < 0)
    {
        cfgXneur->xneurStart();
    }
    else
    {
        cfgXneur->xneurStop();
        cfgXneur->xneurStart();
        qDebug()<<"xneur is running";
    }
    emit changeIconTray(QString("%1").arg(cfgXneur->getCurrentLang()));
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.connect("org.kde.keyboard", "/Layouts", "org.kde.KeyboardLayouts","currentLayoutChanged", this, SLOT(layoutChanged(QString)));
 }

kXneurApp::kXneur::~kXneur()
{
    if( cfgXneur->xneurStop())
    {
        qDebug()<<"Stop xneur";
    }
    delete cfgXneur;
}

void kXneurApp::kXneur::settignsTray()
{
    trayApp = new kXneurApp::kXneurTray(cfgXneur->hot_get_list_user_actions(),this);
    connect(this, SIGNAL(changeIconTray(QString)), trayApp, SLOT(setTrayIconFlags(QString)));
    connect(cfgXneur, SIGNAL(setStatusXneur(bool)), trayApp, SLOT(setStatusXneur(bool)));
    connect(cfgXneur, SIGNAL(setStatusXneur(bool)), trayApp, SLOT(setStatusXneur(bool)));
    connect(trayApp, SIGNAL(exitApp()), SLOT(quit()));
    connect(trayApp, SIGNAL(openSettings()), SLOT(OpenSettings()));
    connect(trayApp, SIGNAL(nextLang()), cfgXneur, SLOT(setNextLang()));
    connect(trayApp, SIGNAL(statusDaemon()), SLOT(startStopNeur()));
    connect(this,SIGNAL(reLoadNeur()), cfgXneur, SLOT(restartNeur()));
}

void kXneurApp::kXneur::layoutChanged(QString lang)
{
    emit changeIconTray(lang.left(2));
}

void kXneurApp::kXneur::startStopNeur()
{
    if(cfgXneur->getNeur_pid()>0)
    {
        cfgXneur->xneurStop();
    }
    else
    {
        cfgXneur->xneurStart();
    }
}

void kXneurApp::kXneur::OpenSettings()
{
    kXneurApp::frmSettings *formSettings = new kXneurApp::frmSettings(0,cfgXneur);
    if(formSettings->exec() == QDialog::Accepted)
    {
      emit reLoadNeur();
    }
}
