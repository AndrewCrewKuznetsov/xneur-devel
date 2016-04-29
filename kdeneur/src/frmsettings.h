#ifndef FRMSETTINGS_H
#define FRMSETTINGS_H
//app headre files
#include "frmaddabbreviature.h"
#include "xneurconfig.h"
#include "getnameapp.h"
#include "ruleschange.h"
#include "adduseraction.h"

//Qt header files
#include <QDialog>
#include <QAbstractButton>
#include <QListWidget>
#include <QTableWidget>


//Kde header files
#include <kconfig.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kconfiggroup.h>

namespace Ui
{
  class frmSettings;
}

namespace kXneurApp
{
  class frmSettings : public QDialog
  {
    Q_OBJECT

  public:
    explicit frmSettings(QWidget *parent = 0,kXneurApp::xNeurConfig *cfg=0);
    ~frmSettings();

  private slots:
    void Clicked(QAbstractButton *);


    //tab layout
    void addApp_OneLayout();
    void removeApp_OneLayout();
    void rulesChange();

    //tab hotkeys
    void editHotkey();
    void removeUserAction();
    void addUserAction();
    void editUserAction();
    void clearHotKey();

    //tab autocompletion
    void auto_add_app_list_disable_autocompletion();
    void auto_del_app_list_disable_autocompletion();

    //tab applications
    void app_add_app_ignore_list();
    void app_del_app_ignore_list();
    void app_add_app_auto_mode_list();
    void app_del_app_auto_mode_list();
    void app_add_app_manual_mode_list();
    void app_del_app_manual_mode_list();

    //tab notifications
    void notif_dublClicked(int,int);

    //tab abbreviations
    void addAbbreviation();
    void delAbbreviation();

    //tab log
    void openLoFileIn(int);
    void openLogFile();

    //tab advanced
    void addDelay_tabAdvanced();
    void removeDelay_tabAdvanced();
    void addApp_tabAdvanced();
    void removeApp_tabAdvanced();

    //tab properties
    void RecoverKeyboardCommand();
    void EditKeyboardCommand();
    void TypeIconTray(int);
    //void TypeEngine(int);
    void BrowseIconTray();
    void chekAutostart(bool);
    void delayStartApp(int);

  private:
    Ui::frmSettings *ui;
    void saveSettingsNeur();
    QStringList getListFromWidget(QListWidget *);
    void add_Application_to_Widget(QListWidget *);
    void del_Application_to_Widget(QListWidget *);

    xNeurConfig *cfgNeur;
    KConfig *config;
    KConfigGroup general;
    KConfigGroup layouts;
    KConfigGroup hotkeys;
    KConfigGroup autocompletion;
    KConfigGroup applications;
    KConfigGroup notifications;
    KConfigGroup Abbreviations;
    KConfigGroup log;
    KConfigGroup troubleshooting;
    KConfigGroup advanced;
    KConfigGroup plugins;
    KConfigGroup properties;
    void settintgGrid();
    void createConnect();
    void readSettingsKdeNeur();
    void readSettingsNeur();

    void tab_lay_get_list_lang(QStringList);
    void tab_lay_get_list_app(QStringList);
    QHash<QString, bool> tab_lay_save_list_lang();

    void hot_get_list_hotkeys(QMap<QString, QString>);
    QMap<QString, QString> hot_save_list_hotkeys();
    void hot_get_list_user_actions(QMap<QString, QMap<QString, QString> >);
    QMap<QString, QMap<QString, QString> > hot_save_list_user_actions();


    void notif_get_list_action_sound(QMap<QString, QMultiMap<bool, QString> >);
    void notif_get_list_action_osd(QMap<QString, QMultiMap<bool, QString> >);
    void notif_get_list_action_popup(QMap<QString, QMultiMap<bool, QString> >);

    QMap<QString, QMultiMap<bool, QString> > get_lget_from_notif_widget(QTableWidget *);

    void abbr_get_list_abbreviations(QMap <QString, QString>);
    QMap <QString, QString> abbr_save_list_apprevaitions();

    void plug_get_list_plugins(QMap<QString, QMultiMap<bool, QString> >);
    QMap<QString, QMultiMap<bool, QString> > plug_save_list_plugins();

    void auto_get_list_app_autocomp(QStringList);

    void adv_get_list_key_release_app(QStringList);
    void adv_get_list_delay_app(QStringList);
  };
}
#endif // FRMSETTINGS_H
