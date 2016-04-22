#ifndef KXNEURTRAY_H
#define KXNEURTRAY_H
//Qt header files
#include <QSystemTrayIcon>
#include <QObject>
#include <QAction>
#include <QMenu>
#include <QProcess>


#include <kconfig.h>

namespace kXneurApp
{
  class kXneurTray : public QObject
  {
    Q_OBJECT
  public:
    explicit kXneurTray(QMap<QString, QMap<QString, QString> > listAction, QObject *parent=0);
    void  setTrayToolTips(QString);
  public slots:
    void setTrayIconFlags(QString);
    void kXneurAbout();
    void keyboardProperties();
    void showJournal();
    void settingsApp();
    void startStopNeur();
    void trayClicked(QSystemTrayIcon::ActivationReason);
    void setStatusXneur(bool);
  private:
    enum trayStat {FLAG=0,FLAG_USER, TEXT,ICON}; //what is to be displayed in the tray- flag, user's flag, text layout, application icon
    //enum logViewer {BROWSER=0,PROGRAM};
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QMenu *user_action_menu;
    QAction *start_stop_neur;
    QAction *user_action;
    QAction *show_journal;
    QAction *settings_app;
    QAction *settings_keyboard;
    QAction *about_app;
    QAction *exit_app;
    void createActions(QMap<QString, QMap<QString, QString> >);
    bool add_user_action_menu_from_file(QMap<QString, QMap<QString, QString> >);
  private slots:
    void runUserActions();
signals:
    void statusDaemon(bool);
    void statusDaemon();
    void restartNeur();
    void nextLang();
    void exitApp();
    void openSettings();
  };
}
#endif // KXNEURTRAY_H
