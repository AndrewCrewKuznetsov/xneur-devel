#ifndef XNEURCONFIG_H
#define XNEURCONFIG_H

#include <QString>
#include <QProcess>
#include <QObject>
#include <QDebug>
#include <QHash>
#include <QDir>
#include <QLibrary>
#include <qwindowdefs.h>

namespace kXneurApp
{
    class xNeurConfig: public QObject
    {
        Q_OBJECT
    private:
        int xneur_pid;
        QString get_bind(int);
        QStringList notifyNames;
        QStringList lstCommand_hotKey;
        QStringList lstModifer;

    private slots:
        void procxNeurStart();
        void procxNeurStop(int,QProcess::ExitStatus);
        void procxNeurOutput();

    public:
       explicit xNeurConfig(QObject *parent = 0);
        ~xNeurConfig();
        static Display *dpy;
        QProcess *procxNeur;
        struct _xneur_config *xconfig;
        bool xneurStop();
        bool xneurStart();
        bool init_libxnconfig();
        QString getCurrentLang();
        int getNeur_pid();
        void clearNeurConfig();
        void saveNeurConfig();
        void test(QString);


        //tab General
        void gen_main_save_manual_switch(bool);
        bool gen_main_get_manual_switch();
        void gen_main_save_auto_learning(bool);
        bool gen_main_get_auto_learning();
        void gen_main_save_keep_select(bool);
        bool gen_main_get_keep_select();
        void gen_main_save_rotate_layout(bool);
        bool gen_main_get_rotate_layout();
        void gen_main_save_check_lang(bool);
        void gen_main_save_check_similar(bool);
        bool gen_main_get_check_lang();
        bool gen_main_get_check_similar();
        void gen_tipo_save_correct_caps(bool);
        bool gen_tipo_get_correct_caps();
        void gen_tipo_save_disable_caps(bool);
        bool gen_tipo_get_disable_caps();
        void gen_tipo_save_correct_two_caps(bool);
        bool gen_tipo_get_correct_two_caps();
        void gen_tipo_save_correct_space(bool);
        bool gen_tipo_get_correct_space();
        void gen_tipo_save_correct_small_letter(bool);
        bool gen_tipo_get_correct_small_letter();
        void gen_tipo_save_correct_two_space(bool);
        bool gen_tipo_get_correct_two_space();
        void gen_tipo_save_correct_two_minus(bool);
        bool gen_tipo_get_correct_two_minus();
        void gen_tipo_save_correct_c(bool);
        bool gen_tipo_get_correct_c();
        void gen_tipo_save_correct_tm(bool);
        bool gen_tipo_get_correct_tm();
        void gen_tipo_save_correct_r(bool);
        bool gen_tipo_get_correct_r();
        void gen_tipo_save_correct_three_point(bool);
        bool gen_tipo_get_correct_three_point();
        void gen_tipo_save_correct_dash(bool);
        bool gen_tipo_get_correct_dash();
        void gen_tipo_save_correct_misprint(bool);
        bool gen_tipo_get_correct_misprint();

        //tab Layout
        void lay_save_number_layout(int);
        int lay_get_number_layout();
        void lay_save_remember_layout_for_app(bool);
        bool lay_get_remember_layout_for_app();
        QStringList lay_get_list_language();
        void lay_save_list_language(QHash<QString, bool>);
        QStringList lay_get_list_app_one_layout();
        void lay_save_list_app_one_layout(QStringList);
        QStringList lay_get_text_dictionary(QString);

        //tab hotkeys
        QMap <QString, QString> hot_get_list_command_hotkeys();
        QMap<QString, QMap<QString, QString> >  hot_get_list_user_actions();
        void hot_save_list_command_hotkeys(QMap <QString, QString>);
        void hot_save_list_user_actions(QMap<QString, QMap<QString, QString> > );

        //tab autocompletion
        void auto_save_enable_pattern(bool);
        bool auto_get_enable_pattern();

        void auto_save_add_space(bool);
        bool auto_get_add_space();

        QStringList auto_get_list_app_disable_autocomplite();
        void auto_save_list_app_disable_autocomplite(QStringList);

        //tab applications
        QStringList app_get_list_ignore_app();
        QStringList app_get_list_auto_mode_app();
        QStringList app_get_list_manual_mode_app();
        void app_save_list_ignore_app(QStringList);
        void app_save_list_auto_mode_app(QStringList);
        void app_save_list_manual_mode_app(QStringList);

        //tab Notifications
            //tab SOUND
        void notif_save_enable_sound(bool);
        bool notif_get_enable_sound();
        void notif_save_volume_sound(int);
        int notif_get_volume_sound();
        QMap<QString, QMultiMap<bool, QString> > notif_get_list_action_sound();
        void notif_save_list_action_sound(QMap<QString, QMultiMap<bool, QString> > );
            //tab OSD
        void notif_save_enable_show_osd(bool);
        bool notif_get_enable_show_osd();
        void notif_save_set_font_osd(QString);
        QString notif_get_font_osd();
        QMap<QString, QMultiMap<bool, QString> >  notif_get_list_action_osd();
        void notif_save_list_action_osd( QMap<QString, QMultiMap<bool, QString> >);
            //tab POPUP MSG
        void notif_save_enable_show_popup_msg(bool);
        bool notif_get_enable_show_popup_msg();
        void notif_save_interval_popup_msg(int);
        int notif_get_interval_popup_msg();
        QMap<QString, QMultiMap<bool, QString> >  notif_get_list_action_popup_msg();
        void notif_save_list_action_popup_msg(QMap<QString, QMultiMap<bool, QString> >);

        //tab Abbreviations
        void abbr_save_ignore_keyboarf_layout(bool);
        bool abbr_get_ignore_keyboard_layout();
        QMap <QString, QString> abbr_get_list_abbreviations();
        void abbr_save_list_abbreviations(QMap <QString, QString>);

        //tab Log
        void log_save_enable_keylog(bool);
        bool log_get_enable_keylog();
        void log_save_size_log_file(int);
        int log_get_size_log_file();
        void log_save_email(QString);
        QString log_get_email();
        void log_save_host(QString);
        QString log_get_host();
        void log_save_port(int);
        int log_get_port();

        //tab Trobleshooting
        bool trabl_get_backspace();
        void trabl_save_backspace(bool);
        bool trabl_get_left_arrow();
        void trabl_save_left_arrow(bool);
        bool trabl_get_right_arrow ();
        void trabl_save_right_arrow (bool);
        bool trabl_get_up_arrow ();
        void trabl_save_up_arrow (bool);
        bool trabl_get_down_arrow ();
        void trabl_save_down_arrow (bool);
        bool trabl_get_delete ();
        void trabl_save_delete (bool);
        bool trabl_get_user_change_layout ();
        void trabl_save_user_change_layout (bool);
        bool trabl_get_full_screen ();
        void trabl_save_full_screen (bool);
        bool trabl_get_flush_buffer_esc ();
        void trabl_save_flush_buffer_esc (bool);
        bool trabl_get_flush_buffer_tab_enter ();
        void trabl_save_flush_buffer_tab_enter (bool);

        bool trabl_get_dont_correct_press_enter();
        bool trabl_get_dont_correct_press_tab ();
        void trabl_save_dont_correct_press_enter(bool);
        void trabl_save_dont_correct_press_tab (bool);

        bool trabl_get_compat_with_completion ();
        void trabl_save_compat_with_completion (bool);
        bool trabl_get_monitor_input ();
        void trabl_save_monitor_input (bool);
        bool trabl_get_monitor_mouse ();
        void trabl_save_monitor_mouse (bool);

        //tab Advanced
        void adv_save_delay_sending_events(int);
        int adv_get_delay_sending_events();

        void adv_save_key_release_app(QStringList);
        QStringList adv_get_key_release_app();

        void adv_save_log_level(int);
        int adv_get_log_level();



        //tab Plugins
        QMap<QString, QMultiMap<bool, QString> >  plug_get_list_plugins();
        void plug_save_list_plugins(QMap<QString, QMultiMap<bool, QString> > );

        //tab Properties

        //xconfig->play_sounds
        //void delayStartApp(int);

    public slots:
        void setNextLang();
        void restartNeur();

    signals:
        void setStatusXneur(bool);
    };
}
#endif // XNEURCONFIG_H
