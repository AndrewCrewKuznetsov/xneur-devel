//app header files
#include "kxneurtray.h"
#include "frmabout.h"

//Qt header files
#include <QDebug>
#include <QStringList>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QUrl>
#include <QDesktopServices>
#include <QTranslator>
#include <QFont>
#include <QPoint>
#include <QColor>

//Kde header files
#include <ktoolinvocation.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <plasma/theme.h>
#include <plasma/paintutils.h>

kXneurApp::kXneurTray::kXneurTray( QMap<QString, QMap<QString, QString> > listActions,QObject *parent): QObject(parent)
{
  createActions(listActions);
}


void kXneurApp::kXneurTray::createActions(QMap<QString, QMap<QString, QString> > lstAct)
{
  trayIcon = new QSystemTrayIcon();
  trayIcon->setIcon(QIcon(":/by"));
  trayMenu = new QMenu();
  user_action_menu = new QMenu();

  qDebug()<< "COUNT USER ACTIONS: " << lstAct.size();

  exit_app = new QAction(tr("Exit"), this);
  connect(exit_app, SIGNAL(triggered()), SIGNAL(exitApp()));

  about_app = new QAction(tr("kdeNeur About..."), this);
  connect(about_app, SIGNAL(triggered()),SLOT(kXneurAbout()));

  settings_keyboard =  new QAction(tr("Keyboard Properties..."), this);
  connect(settings_keyboard,SIGNAL(triggered()), SLOT(keyboardProperties()));

  settings_app = new QAction(tr("Properties"), this);
  connect(settings_app,SIGNAL(triggered()), SLOT(settingsApp()));

  show_journal = new QAction(tr("View log..."), this);
  connect(show_journal,SIGNAL(triggered()), SLOT(showJournal()));

  if(add_user_action_menu_from_file(lstAct))
  {
      user_action = new QAction(tr("User Action"), this);
      user_action->setMenu(user_action_menu);
  }
  else
  {
      user_action=NULL;
  }

  //TODO:
  start_stop_neur = new QAction(tr("Start daemon"), this);
  start_stop_neur->setData(QVariant(false));
  connect(start_stop_neur, SIGNAL(triggered()), SLOT(startStopNeur()));
  trayMenu->addAction(start_stop_neur);
  trayMenu->addSeparator();
  if(user_action!=NULL)
    trayMenu->addAction(user_action);
  trayMenu->addAction(show_journal);
  trayMenu->addSeparator();
  trayMenu->addAction(settings_app);
  trayMenu->addAction(settings_keyboard);
  trayMenu->addAction(about_app);
  trayMenu->addAction(exit_app);

  trayIcon->setContextMenu(trayMenu);
  trayIcon->setToolTip(tr("X Neural Switcher running"));
  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
  trayIcon->show();
}

void kXneurApp::kXneurTray::setTrayIconFlags(QString lang)
{
    KConfig conf("kdeneurrc");
    KConfigGroup properties = conf.group("Properties");
    QString path, usrPath;
    int tray = properties.readEntry("Typeicontray", 0);
    usrPath = properties.readEntry("Iconpath", PACKAGE_PIXMAPS_DIR);
    switch(tray)
    {
    case FLAG:
        path=QString("%1/%2.png").arg(PACKAGE_PIXMAPS_DIR).arg(lang);
        qDebug()<< "TYPE ICON " << FLAG << path;
        if (QFile::exists(path))
        {
            trayIcon->setIcon(QIcon(path));
        }
        else
        {
            qDebug() << tr(QString("ERROR: Not Found %1").arg(path).toAscii());
            trayIcon->setIcon(QIcon(":/noLayout"));
        }
        break;
    case FLAG_USER:
        path=QString("%1/%2.png").arg(usrPath).arg(lang);
        qDebug()<< "TYPE ICON " << FLAG_USER << path;
        if (QFile::exists(path))
        {
            trayIcon->setIcon(QIcon(path));
        }
        else
        {
            qDebug() << tr(QString("ERROR: Not Found %1").arg(path).toAscii());
            trayIcon->setIcon(QIcon(":/noLayout"));
        }
          break;
    case TEXT:
    {
        //TODO: Add userSettings for text in tray
        QFont font;
        font.setFamily("Ubuntu");
        font.setBold(true);
        font.setPointSize(16);
        QColor textColor = Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor);
        QColor shadowColor = Plasma::Theme::defaultTheme()->color(Plasma::Theme::BackgroundColor);
        QPoint offset = QPoint(0, 0);
        trayIcon->setIcon(QIcon(Plasma::PaintUtils::shadowText(lang, font, textColor, shadowColor, offset, 0)));
    }break;
    case ICON:
        trayIcon->setIcon(QIcon(":/icons/kdeneur.png"));
        qDebug() << "Not found icon for " << lang << " language!";
        break;
    }
}

void kXneurApp::kXneurTray::kXneurAbout()
{
    kXneurApp::frmAbout *formAbout = new kXneurApp::frmAbout();
    formAbout->exec();
}

void kXneurApp::kXneurTray::keyboardProperties()
{
  QStringList arg;
  arg<< "--args=--tab=layouts";
  arg << "kcm_keyboard";
  KToolInvocation::kdeinitExec("/usr/bin/kcmshell4", arg);
}

void kXneurApp::kXneurTray::settingsApp()
{
    emit openSettings();
}

void kXneurApp::kXneurTray::showJournal()
{
    //TODO: лог файл может иметь расширение отличное от html
    QString logFile = QString("%1/%2").arg(QDir::homePath()).arg(".xneur/xneurlog.html");
    if (QFile::exists(logFile))
    {
        KConfig conf("kdeneurrc");
        KConfigGroup properties = conf.group("Properties");
        int viewer = properties.readEntry("Viewer", 0);
        switch(viewer)
        {
        case 0://browser
            QDesktopServices::openUrl(QUrl(logFile));
            break;
        case 1://logviewer
            break;
        }
    }
    else
    {
        QMessageBox::warning(0,tr("Log file not found..."),tr("Log file xneurlog.html not found. Maybe you do not have the option of logging!"));
    }
}

void kXneurApp::kXneurTray::trayClicked(QSystemTrayIcon::ActivationReason click)
{
  if(click ==QSystemTrayIcon::MiddleClick)
  {
      startStopNeur();
  }
  else if(click ==QSystemTrayIcon::Trigger)
  {
    emit nextLang();
  }
}

void kXneurApp::kXneurTray::startStopNeur()
{
    emit statusDaemon();

}

bool kXneurApp::kXneurTray::add_user_action_menu_from_file(QMap<QString, QMap<QString, QString> > lstAct)
{
    if (lstAct.size()>0)
    {
        QMap<QString, QString> tmpCmd;
        QMap<QString, QMap<QString, QString> >::const_iterator i = lstAct.constBegin();
        while (i!=lstAct.constEnd())
        {
            tmpCmd = i.value();
            QMap<QString, QString>::const_iterator p = tmpCmd.constBegin();
            while(p!=tmpCmd.constEnd())
            {
                QAction *usrAct = new QAction(p.key(), this);
                usrAct->setData(p.value());
                connect(usrAct, SIGNAL(triggered()), SLOT(runUserActions()));
                user_action_menu->addAction(usrAct);
                ++p;
            }
            ++i;
        }
        return true;
    }
    else
    {
        return false;
    }
}
//выполняем пользовательское действие
void kXneurApp::kXneurTray::runUserActions()
{
    QAction *usrAct = (QAction *)sender();
    QProcess prc;
    if(!prc.startDetached(usrAct->data().toString()))
    {
        QMessageBox::information(0, tr("Error: Execute Actions"), tr(prc.errorString().toUtf8().data()), QMessageBox::Ok);
        qDebug() << prc.errorString();
        qDebug() << usrAct->data().toString();
    }
}


void kXneurApp::kXneurTray::setTrayToolTips(QString tooltip)
{
    trayIcon->setToolTip(tooltip);
}

void kXneurApp::kXneurTray::setStatusXneur(bool status)
{
    if(status)
    {
        start_stop_neur->setText(tr("Stop daemon"));
    }
    else
    {
        start_stop_neur->setText(tr("Start daemon"));

    }
    //TODO:
    start_stop_neur->setData(QVariant(status));
}
