extern "C"
{
    #include <xneur/xneur.h>
    #include <xneur/xnconfig.h>
    #include <xneur/list_char.h>
}

test


#include "xneurconfig.h"
#define MAX_LANGUAGES 4
#define TOTAL_MODIFER 4
#define LANGUAGES_DIR "languages"
#define XNEUR_NEEDED_MAJOR_VERSION 15
#define XNEUR_BUILD_MINOR_VERSION 0
#define XNEUR_PLUGIN_DIR "/usr/lib/xneur"

extern "C"
{
    #include "xkb.h"
}

Display *kXneurApp::xNeurConfig::dpy=NULL;

kXneurApp::xNeurConfig::xNeurConfig(QObject *parent) :  QObject(parent)
{
    xconfig = NULL;
    init_libxnconfig();
    xneur_pid = xconfig->get_pid(xconfig);
    dpy=XOpenDisplay(NULL);
    procxNeur = new QProcess();
    connect(procxNeur, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(procxNeurStop(int,QProcess::ExitStatus)));
    connect(procxNeur, SIGNAL(started()), SLOT(procxNeurStart()));
   // plug_get_list_plugins();
   // qDebug() << XNEURPLUGINDIR;
    notifyNames << tr("Xneur started") << tr("Xneur reloaded") << tr("Xneur stopped") << tr("Keypress on layout 1")
                << tr( "Keypress on layout 2") << tr("Keypress on layout 3") << tr("Keypress on layout 4") << tr("Switch to layout 1")
                << tr( "Switch to layout 2") << tr("Switch to layout 3") << tr("Switch to layout 4") << tr("Correct word automatically")
                << tr( "Correct last word manually") << tr("Transliterate last word manually") << tr("Change case of last word manually")
                << tr( "Preview correction of last word manually") << tr("Correct last line") << tr("Correct selected text") << tr("Transliterate selected text")
                << tr( "Change case of selected text") << tr("Preview correction of selected text") << tr("Correct clipboard text") << tr("Transliterate clipboard text")
                << tr( "Change case of clipboard text") << tr("Preview correction of clipboard text") << tr("Expand abbreviations") << tr("Correct aCCIDENTAL caps")
                << tr( "Correct TWo INitial caps") << tr("Correct two space with a comma and a space") << tr("Correct two minus with a dash")
                << tr( "Correct (c) with a copyright sign") << tr("Correct (tm) with a trademark sign") << tr("Correct (r) with a registered sign")
                << tr( "Correct three points with a ellipsis sign") << tr("Execute user action") << tr("Block keyboard and mouse events")
                << tr( "Unblock keyboard and mouse events");

    lstCommand_hotKey << tr("Correct/Undo correction") << tr("Transliterate") << tr("Change case") << tr("Preview correction") << tr("Correct last line")
               << tr("Correct selected text") << tr("Transliterate selected text") << tr("Change case of selected text") << tr("Preview correction of selected text")
               << tr("Correct clipboard text") << tr("Transliterate clipboard text") << tr("Change case of clipboard text") << tr("Preview correction of clipboard text")
               << tr("Switch to layout 1") << tr("Switch to layout 2") << tr("Switch to layout 3") << tr("Switch to layout 4")
               << tr("Rotate layouts") << tr("Rotate layouts back") << tr("Expand abbreviations") << tr("Autocompletion confirmation")
               << tr("Block/Unblock keyboard and mouse events") << tr("Insert date");
     lstModifer << "Shift" << "Control" << "Alt" << "Super";

}

kXneurApp::xNeurConfig::~xNeurConfig()
{
    XCloseDisplay(dpy);
}
QString kXneurApp::xNeurConfig::get_bind(int ind)
{
    QString key;
    key.clear();
    for (int i = 0; i < TOTAL_MODIFER; ++i)
    {
        if ((xconfig->hotkeys[ind].modifiers & (0x1 << i)) == 0)
            continue;

        key += QString("%1+").arg(lstModifer.at(i));
    }
  key+=QString("%1").arg( xconfig->hotkeys[ind].key);
  return key;
}

bool kXneurApp::xNeurConfig::init_libxnconfig()
{
    if (xconfig != NULL)
    {
        return true;
    }

    if((xconfig = xneur_config_init())==NULL)
    {
        qDebug()<<"ERROR: lib init fail";
        return false;
    }
    int major_version, minor_version;
    xconfig->get_library_version(&major_version, &minor_version);
    if ((major_version != XNEUR_NEEDED_MAJOR_VERSION) || (minor_version != XNEUR_BUILD_MINOR_VERSION))
    {
        qDebug()<<QString(tr("Wrong XNeur configuration library api version.\nPlease, install libxnconfig version 0.%1.%2")).arg(XNEUR_NEEDED_MAJOR_VERSION).arg(XNEUR_BUILD_MINOR_VERSION);
        xconfig->uninit(xconfig);
        exit(1);
    }
    qDebug()<<QString(tr("Using libxnconfig API version 0.%1.%2 (build with 0.%3.%4)")).arg(major_version).arg(minor_version).arg(XNEUR_NEEDED_MAJOR_VERSION).arg(XNEUR_BUILD_MINOR_VERSION);
    if (!xconfig->load(xconfig))
    {
        qDebug()<< tr("XNeur's config broken or was created with old version!\nPlease, remove ~/.xneur/. It should solve the problem!\nIf you don't want to loose your configuration, back it up\nand manually patch new configuration which will be created after first run.");
        xconfig->uninit(xconfig);
        exit(1);
    }
  return true;
}

bool kXneurApp::xNeurConfig::xneurStop()
{

    if ( xneur_pid > 0 )
    {
        if ( xconfig->kill(xconfig) )
        {
            xconfig->set_pid(xconfig, 0);
        }
        else
        {
           qDebug()<< "ERROR: Fail xNeur stopped [don't kill xconfig']";
           return false;
        }
        xneur_pid = 0;
        //TODO
//        if(running)
//        {
//           procxNeurStop(0,QProcess::NormalExit);
//        }
        return true;
    }
    qDebug()<< "MSG: xNeur isn't running";
    return false;

    //

}

bool kXneurApp::xNeurConfig::xneurStart()
{
    if ( !init_libxnconfig())
    {
        qDebug("ERROR: start xNeur Fail [not init libxnconfig]");
        return false;
    }
    xneur_pid = xconfig->get_pid(xconfig);
    if ( xneur_pid > 0 )
    {
        if (!xneurStop())
        {
            return false;
        }
    }
    procxNeur->start("xneur",QIODevice::ReadWrite);
    xneur_pid = procxNeur->pid();
    if ( xneur_pid > 0 )
    {
        init_libxnconfig();
        return true;
    }
    qDebug() << tr("ERROR: start xNeur Fail") /*<< error*/;
    xneur_pid = 0;
    qDebug("start -- fail 3\n");
    return false;
}

int kXneurApp::xNeurConfig::getNeur_pid()
{
    return xconfig->get_pid(xconfig);
}

QString kXneurApp::xNeurConfig::getCurrentLang()
{
    return QString("%1").arg(xconfig->handle->languages[get_active_kbd_group(dpy)].dir);
}

void kXneurApp::xNeurConfig::setNextLang()
{
    set_next_kbd_group(dpy);
}

void kXneurApp::xNeurConfig::restartNeur()
{
   xconfig->reload(xconfig);
}

void kXneurApp::xNeurConfig::procxNeurStop(int exitcode, QProcess::ExitStatus exitstatus)
{
  emit setStatusXneur(false);
  qDebug()<<"MSG: xNeur stopped:" << " ExitCode " << exitcode << " ExitStatus " << exitstatus;
  if(exitstatus >0)
  {
      qDebug()<< tr("ERROR: Warning process xNeur crashed, please look log file and inform the author xNeur. Thank You!");
  }
}

void kXneurApp::xNeurConfig::procxNeurStart()
{
  qDebug()<<"MSG: xNeur started.";
  emit setStatusXneur(true);
}

void kXneurApp::xNeurConfig::clearNeurConfig()
{
  //  qDebug()<< "Clear config";
    xconfig->clear(xconfig);
}

void kXneurApp::xNeurConfig::saveNeurConfig()
{
      xconfig->save(xconfig);
}

void kXneurApp::xNeurConfig::test(QString str)
{
    qDebug()<<str;
}

//void kXneurApp::xNeurConfig::delayStartApp(int time)
//{
//    xconfig->
//}

/*================================= tab General =================================*/
void kXneurApp::xNeurConfig::gen_main_save_manual_switch(bool stat)
{
   // qDebug()<< "set manual mode " << stat;
  xconfig->manual_mode = stat;
}

bool kXneurApp::xNeurConfig::gen_main_get_manual_switch()
{
    return xconfig->manual_mode;
}

void kXneurApp::xNeurConfig::gen_main_save_auto_learning(bool stat)
{
  //  qDebug()<< "set educate "  << stat;
    xconfig->educate = stat;
}

bool kXneurApp::xNeurConfig::gen_main_get_auto_learning()
{
    return xconfig->educate;
}
void kXneurApp::xNeurConfig::gen_main_save_keep_select(bool stat)
{
   // qDebug()<< "set save selection text " << stat;
    xconfig->save_selection_after_convert=stat;
}

bool kXneurApp::xNeurConfig::gen_main_get_keep_select()
{
    return xconfig->save_selection_after_convert;
}

void kXneurApp::xNeurConfig::gen_main_save_rotate_layout(bool stat)
{
  //  qDebug()<< "set rotate layout convert "  << stat;
    xconfig->rotate_layout_after_convert=stat;
}

bool kXneurApp::xNeurConfig::gen_main_get_rotate_layout()
{
    return xconfig->rotate_layout_after_convert;
}

void kXneurApp::xNeurConfig::gen_main_save_check_lang(bool stat)
{
   // qDebug()<< "set check lang process "  << stat;
    xconfig->check_lang_on_process=stat;
}

bool kXneurApp::xNeurConfig::gen_main_get_check_lang()
{
    return xconfig->check_lang_on_process ;
}

void kXneurApp::xNeurConfig::gen_tipo_save_correct_caps(bool stat)
{
  //  qDebug()<< "set correct incidental caps "  << stat;
    xconfig->correct_incidental_caps = stat;
}

bool kXneurApp::xNeurConfig::gen_tipo_get_correct_caps()
{
    return xconfig->correct_incidental_caps;
}

void kXneurApp::xNeurConfig::gen_tipo_save_disable_caps(bool stat)
{
   // qDebug()<< "set disable caps lock "  << stat;
    xconfig->disable_capslock=stat;
}

bool kXneurApp::xNeurConfig::gen_tipo_get_disable_caps()
{
    return xconfig->disable_capslock;
}

void kXneurApp::xNeurConfig::gen_tipo_save_correct_two_caps(bool stat)
{
   // qDebug()<< "set correct two caps "  << stat;
    xconfig->correct_two_capital_letter =stat;
}

bool kXneurApp::xNeurConfig::gen_tipo_get_correct_two_caps()
{
    return xconfig->correct_two_capital_letter;
}

void kXneurApp::xNeurConfig::gen_tipo_save_correct_space(bool stat)
{
   // qDebug()<< "set correct two space with punct "  << stat;
    xconfig->correct_space_with_punctuation=stat;
}

bool kXneurApp::xNeurConfig::gen_tipo_get_correct_space()
{
    return xconfig->correct_space_with_punctuation;
}

void kXneurApp::xNeurConfig::gen_tipo_save_correct_small_letter(bool stat)
{
  //  qDebug()<< "set correct capital letter after dot "  << stat;
    xconfig->correct_capital_letter_after_dot = stat;
}

bool kXneurApp::xNeurConfig::gen_tipo_get_correct_small_letter()
{
    return xconfig->correct_capital_letter_after_dot;
}

void kXneurApp::xNeurConfig::gen_tipo_save_correct_two_space(bool stat)
{
  //  qDebug()<< "set correct two space with command and space "  << stat;
    xconfig->correct_two_space_with_comma_and_space =stat;
}

bool kXneurApp::xNeurConfig::gen_tipo_get_correct_two_space()
{
    return xconfig->correct_two_space_with_comma_and_space;
}

void kXneurApp::xNeurConfig::gen_tipo_save_correct_two_minus(bool stat)
{
   // qDebug()<< "set correct to minus "  << stat;
    xconfig->correct_two_minus_with_dash = stat;
}

bool kXneurApp::xNeurConfig::gen_tipo_get_correct_two_minus()
{
    return xconfig->correct_two_minus_with_dash;
}

void kXneurApp::xNeurConfig::gen_tipo_save_correct_c(bool stat)
{
   // qDebug()<< "set correct (c) "  << stat;
    xconfig->correct_c_with_copyright =stat;
}

bool kXneurApp::xNeurConfig::gen_tipo_get_correct_c()
{
    return xconfig->correct_c_with_copyright;
}

void kXneurApp::xNeurConfig::gen_tipo_save_correct_tm(bool stat)
{
  //  qDebug()<< "set correct (tm) "  << stat;
    xconfig->correct_tm_with_trademark = stat;
}

bool kXneurApp::xNeurConfig::gen_tipo_get_correct_tm()
{
    return xconfig->correct_tm_with_trademark;
}

void kXneurApp::xNeurConfig::gen_tipo_save_correct_r(bool stat)
{
  //  qDebug()<< "set correct (r) "  << stat;
    xconfig->correct_r_with_registered = stat;
}

bool kXneurApp::xNeurConfig::gen_tipo_get_correct_r()
{
    return xconfig->correct_r_with_registered;
}

void kXneurApp::xNeurConfig::gen_tipo_save_correct_three_point(bool stat)
{
    xconfig->correct_three_points_with_ellipsis = stat;
}

bool kXneurApp::xNeurConfig::gen_tipo_get_correct_three_point()
{
    return xconfig->correct_three_points_with_ellipsis;
}

void kXneurApp::xNeurConfig::gen_tipo_save_correct_dash(bool stat)
{
    xconfig->correct_dash_with_emdash = stat;
}

bool kXneurApp::xNeurConfig::gen_tipo_get_correct_dash()
{
    return xconfig->correct_dash_with_emdash;
}

/*================================= tab Layout =================================*/

void kXneurApp::xNeurConfig::lay_save_number_layout(int curIndex)
{
 //   qDebug()<< "set save layout";
    xconfig->default_group = curIndex;
}

int kXneurApp::xNeurConfig::lay_get_number_layout()
{
    return xconfig->default_group + 1;
}

void kXneurApp::xNeurConfig::lay_save_remember_layout_for_app(bool stat)
{
    //   qDebug()<< "set remember layout";
    xconfig->remember_layout = stat;
}

bool kXneurApp::xNeurConfig::lay_get_remember_layout_for_app()
{
    return xconfig->remember_layout;
}

void kXneurApp::xNeurConfig::lay_save_list_language(QHash<QString, bool> lstLang)
{
    for (int j=0;j< lstLang.size();++j)
    {
        QHash<QString, bool>::const_iterator i = lstLang.constBegin();
        while (i != lstLang.constEnd())
        {
            if (QString("%1").arg(xconfig->handle->languages[j].dir) == i.key())
            {
               // qDebug()<< "set save lang ignord";
                xconfig->handle->languages[j].excluded = i.value();
            }
           ++i;
        }
    }
}

QStringList kXneurApp::xNeurConfig::lay_get_list_language()
{
    QStringList lstLng;
    for(int lng=0; lng< xconfig->handle->total_languages && lng < MAX_LANGUAGES; lng++)
    {
        lstLng<< xconfig->handle->languages[lng].name;
        lstLng<<xconfig->handle->languages[lng].dir;
        lstLng<<QString("%1").arg(xconfig->handle->languages[lng].excluded);
    }
    return lstLng;
}

QStringList kXneurApp::xNeurConfig::lay_get_list_app_one_layout()
{
    QStringList lstApp;

    for(int i=0; i<xconfig->layout_remember_apps->data_count;i++)
    {
        lstApp << xconfig->layout_remember_apps->data[i].string;
    }
    return lstApp;
}
void kXneurApp::xNeurConfig::lay_save_list_app_one_layout(QStringList lstApp)
{
    for (int i=0; i<lstApp.size(); ++i)
    {
        xconfig->layout_remember_apps->add(xconfig->layout_remember_apps, lstApp.at(i).toAscii().data());
    }
 //   qDebug()<< "save list app one layout";
}

QStringList kXneurApp::xNeurConfig::lay_get_text_dictionary(QString lang)
{
    QStringList text;
    QString dir_name = QString("%1/%2").arg(LANGUAGES_DIR).arg(lang);
    QString path_dict = xconfig->get_global_dict_path(dir_name.toAscii().data(), "dictionary");
    text << xconfig->get_home_dict_path(dir_name.toAscii().data(), "dictionary");
    QFile fileDict(path_dict);
    if(fileDict.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream dict(&fileDict);
        while(!dict.atEnd())
        {
            text << dict.readLine();
        }
    }
    return text;
}

/*================================= tab HotKeys =================================*/
QMap <QString, QString> kXneurApp::xNeurConfig::hot_get_list_command_hotkeys()
{

    QString hot_key;
    QMap <QString, QString> tblHotKey;
    //TODO почему-то у меня в файле xnconfig.h переменная  MAX_HOTKEYS 24 а должна быть 23
    //for(int i=0;i<MAX_HOTKEYS; ++i)
    for(int i=0;i<lstCommand_hotKey.size(); ++i)
    {
        if(xconfig->hotkeys[i].key!=NULL)
        {
            hot_key = get_bind(i);
            tblHotKey.insert(lstCommand_hotKey.at(i), hot_key);
        }
        else
        {
            hot_key="";
            tblHotKey.insert(lstCommand_hotKey.at(i), hot_key);
        }
    }
return tblHotKey;
}

void kXneurApp::xNeurConfig::hot_save_list_command_hotkeys(QMap <QString, QString> listHotKey)
{

    bool key=false;
    for(int j=0; j< listHotKey.size();++j)
    {
        QMap <QString, QString>::const_iterator i = listHotKey.constBegin();
        while(i!=listHotKey.constEnd())
        {
            if (lstCommand_hotKey.at(j)==i.key() && !QString("%1").arg(i.value()).isEmpty())
            {
                QStringList lsh_k = QString("%1").arg(i.value()).replace(" ","").split("+");
                for(int k=0; k<lsh_k.size();++k)
                {
                    key=false;
                    for(int p=0; p<TOTAL_MODIFER;++p)
                    {
                        if(lsh_k.at(k)== lstModifer.at(p))
                        {
                            key = true;
                            xconfig->hotkeys[j].modifiers |= (0x1 << p);
                        }
                    }
                    if (key==false)
                    {
                        xconfig->hotkeys[j].key =strdup(lsh_k.at(k).toUtf8().data());
                    }
                }
             }
            ++i;
        }
    }
}

QMap<QString, QMap<QString, QString> >  kXneurApp::xNeurConfig::hot_get_list_user_actions()
{
    QString text;
    QStringList lstModifer;
    //   hot_key       name act  command act
    QMap<QString, QMap<QString, QString> > lstUserAction;
    QMap<QString, QString> lstNameCmd;
    lstModifer << "Shift" << "Control" << "Alt" << "Super";
    for (int action = 0; action < xconfig->actions_count; action++)
    {
        for (int i = 0; i < TOTAL_MODIFER; ++i)
        {
                if ((xconfig->actions[action].hotkey.modifiers & (0x1 << i)) == 0)
                {
                    continue;
                }

               text = QString("%1+").arg(lstModifer.at(i));
        }
        text += QString("%1").arg(xconfig->actions[action].hotkey.key);

        lstNameCmd.insert(xconfig->actions[action].name,xconfig->actions[action].command);
        lstUserAction.insert(text, lstNameCmd);
        lstNameCmd.clear();
        text="";
    }
    qDebug() << " COUNT USER ACTIONS " << lstUserAction.size();

    return lstUserAction;
}

void kXneurApp::xNeurConfig::hot_save_list_user_actions(QMap<QString, QMap<QString, QString> > lstActions)
{
     QMap<QString, QString> tmpCmd;
<<<<<<< local
     bool key=false;  /*int ppp=0; */int j=0;
//     for(int j=0; j< lstActions.size();++j)
//     {
         QMap<QString, QMap<QString, QString> >::const_iterator i = lstActions.constBegin();
         while(i!=lstActions.constEnd()) //<< Пропускает последний экшн. Надо <= !!!
         {
=======
    QMap<QString, QMap<QString, QString> >::const_iterator i = lstActions.constBegin();
     bool key=false;  /*int ppp=0; int j=0;*/
     for(int j=0; j< lstActions.size();++j)
     {
 qDebug () << "TOTAL.NUM---> " <<lstActions.size() ;
         //QMap<QString, QMap<QString, QString> >::const_iterator i = lstActions.constBegin();
        // while(i!=lstActions.constEnd()) // Пропускает последний экшн. Надо <= !!!
         //{
>>>>>>> other
qDebug () << "HOTKEY.NUM---> " << j ;
             xconfig->actions = (struct _xneur_action *) realloc(xconfig->actions, (j + 1) * sizeof(struct _xneur_action));
             bzero(&xconfig->actions[j], sizeof(struct _xneur_action));
            // memset(&xconfig->actions[j], 0,  sizeof(struct _xneur_action));
             xconfig->actions[j].hotkey.modifiers = 0;

             tmpCmd = i.value();
             QStringList lsh_k = QString("%1").arg(i.key()).replace(" ","").split("+");
             for(int k=0; k<lsh_k.size();++k)
             {

                 key=false;
                 for(int p=0; p<TOTAL_MODIFER;++p)
                 {
                     if(lsh_k.at(k)== lstModifer.at(p) || lsh_k.at(k).endsWith("_L") || lsh_k.at(k).endsWith("_R"))
                     {
                         key = true;
                         xconfig->actions[j].hotkey.modifiers |= (0x1 << p);
                     }
                 }
                 if (key==false)
                 {
                     QMap<QString, QString>::const_iterator l = tmpCmd.constBegin();
                     QMap<QString, QString>::const_iterator last = tmpCmd.constEnd();
                     while(!(l>last))
                   //for(int l=0;l<lsh_k.size();++l)
                     {
                         qDebug () << "HOTKEY.KEY ---> " << lsh_k.at(k).toAscii().data();

                         xconfig->actions[j].hotkey.key = strdup(lsh_k.at(k).toUtf8().data());
                         if (!QString("%1").arg(l.value()).isEmpty())
                         {
                             qDebug () << QString("command: %1").arg(l.value()).toAscii().data();
                             xconfig->actions[j].command = strdup(QString("%1").arg(l.value()).toAscii().data());
                         }
                         if (!QString("%1").arg(l.key()).isEmpty())
                         {
                             qDebug () <<  QString("name: %1").arg(l.key()).toAscii().data();
                             xconfig->actions[j].name = strdup(QString("%1").arg(l.key()).toAscii().data());
                         }
                        xconfig->actions_count = j + 1;
                         ++l;
                     }
                 }
             }
            ++i;//++j;
         }
        
   //  }
}


/*================================= tab Autocompletion =================================*/

void kXneurApp::xNeurConfig::auto_save_enable_pattern(bool stat)
{
    xconfig->autocompletion =stat;
}

bool kXneurApp::xNeurConfig::auto_get_enable_pattern()
{
    return xconfig->autocompletion;
}

void kXneurApp::xNeurConfig::auto_save_add_space(bool stat)
{
    xconfig->add_space_after_autocompletion =stat;
}

bool kXneurApp::xNeurConfig::auto_get_add_space()
{
    return xconfig->add_space_after_autocompletion;
}

QStringList kXneurApp::xNeurConfig::auto_get_list_app_disable_autocomplite()
{
    QStringList lstApp;
    for(int i=0; i<xconfig->autocompletion_excluded_apps->data_count; ++i)
    {
        lstApp<< xconfig->autocompletion_excluded_apps->data[i].string;
    }
    return lstApp;
}

void kXneurApp::xNeurConfig::auto_save_list_app_disable_autocomplite(QStringList lstApp)
{
    for (int i =0; i< lstApp.size(); ++i)
    {
      xconfig->autocompletion_excluded_apps->add(xconfig->autocompletion_excluded_apps, lstApp.at(i).toAscii().data());
    }
}


/*================================= tab Application =================================*/

QStringList kXneurApp::xNeurConfig::app_get_list_ignore_app()
{
    QStringList lstApp;

    for (int i = 0; i < xconfig->excluded_apps->data_count; i++)
    {
        lstApp<< xconfig->excluded_apps->data[i].string;
    }
    return lstApp;
}

QStringList kXneurApp::xNeurConfig::app_get_list_auto_mode_app()
{
    QStringList lstApp;

    for (int i = 0; i < xconfig->auto_apps->data_count; i++)
    {
        lstApp<< xconfig->auto_apps->data[i].string;
    }
    return lstApp;
}

QStringList kXneurApp::xNeurConfig::app_get_list_manual_mode_app()
{
    QStringList lstApp;
    for (int i = 0; i < xconfig->manual_apps->data_count; i++)
    {
        lstApp<<  xconfig->manual_apps->data[i].string;
    }
    return lstApp;
}

void kXneurApp::xNeurConfig::app_save_list_ignore_app(QStringList lstApp)
{
    for (int i =0; i< lstApp.size(); ++i)
    {
      xconfig->excluded_apps->add(xconfig->excluded_apps, lstApp.at(i).toAscii().data());
    }
}

void kXneurApp::xNeurConfig::app_save_list_auto_mode_app(QStringList lstApp)
{
    for (int i =0; i< lstApp.size(); ++i)
    {
      xconfig->auto_apps->add(xconfig->auto_apps, lstApp.at(i).toAscii().data());
    }
}

void kXneurApp::xNeurConfig::app_save_list_manual_mode_app(QStringList lstApp)
{
    for (int i =0; i< lstApp.size(); ++i)
    {
      xconfig->manual_apps->add(xconfig->manual_apps, lstApp.at(i).toAscii().data());
    }
}

/*================================= tab Notifications =================================*/
                            /*========== tab SOUND ==========*/
void kXneurApp::xNeurConfig::notif_save_enable_sound(bool stat)
{
    xconfig->play_sounds = stat;
}

bool kXneurApp::xNeurConfig::notif_get_enable_sound()
{
    return xconfig->play_sounds;
}

void kXneurApp::xNeurConfig::notif_save_volume_sound(int volume)
{
    xconfig->volume_percent = volume;
}

int kXneurApp::xNeurConfig::notif_get_volume_sound()
{
    return xconfig->volume_percent;
}

QMap<QString, QMultiMap<bool, QString> > kXneurApp::xNeurConfig::notif_get_list_action_sound()
{
    QMap<QString, QMultiMap<bool, QString> > lstSound;
    QMultiMap <bool, QString> lstFile;
    for (int i = 0; i <notifyNames.size(); ++i)
    {
      //  qDebug()<< "ENABLED " << xconfig->sounds[i].enabled << notifyNames.at(i);
        lstFile.insert(xconfig->sounds[i].enabled, QString("%1").arg(xconfig->sounds[i].file));
        lstSound.insert(notifyNames.at(i), lstFile);
        lstFile.clear();
    }
    return lstSound;
}

void kXneurApp::xNeurConfig::notif_save_list_action_sound(QMap<QString, QMultiMap<bool, QString> > lstSound)
{
    int i=0;
    QMultiMap<bool, QString> mapTmp;
    QMap<QString, QMultiMap<bool, QString> >::const_iterator p = lstSound.constBegin();
    while(p!=lstSound.constEnd())
    {
        mapTmp = p.value();
        QMultiMap<bool, QString>::const_iterator j = mapTmp.constBegin();
        while(j!=mapTmp.constEnd())
        {
             xconfig->sounds[i].enabled = j.key();
             if (xconfig->sounds[i].file != NULL)
             {
                 delete xconfig->sounds[i].file;
             }
             xconfig->sounds[i].file = QString("%1").arg(j.value()).toAscii().data();
            ++j;++i;
        }
        ++p;
    }
}
                            /*========== tab OSD ==========*/
void kXneurApp::xNeurConfig::notif_save_enable_show_osd(bool stat)
{
    xconfig->show_osd = stat;
}

bool kXneurApp::xNeurConfig::notif_get_enable_show_osd()
{
    return xconfig->show_osd;
}

void kXneurApp::xNeurConfig::notif_save_set_font_osd(QString osd_font)
{
    if (xconfig->osd_font != NULL)
    {
        qDebug()<< "delete font";
        delete xconfig->osd_font;
    }
    xconfig->osd_font = strdup(osd_font.toUtf8().data());
}

QString kXneurApp::xNeurConfig::notif_get_font_osd()
{
    return QString("%1").arg(xconfig->osd_font);
}

QMap<QString, QMultiMap<bool, QString> >  kXneurApp::xNeurConfig::notif_get_list_action_osd()
{
    QMap<QString, QMultiMap<bool, QString> > lstOSD;
    QMultiMap <bool, QString> lstFile;
    for (int i = 0; i <notifyNames.size(); ++i)
    {
        lstFile.insert(xconfig->osds[i].enabled, QString("%1").arg(xconfig->osds[i].file));
        lstOSD.insert(notifyNames.at(i), lstFile);
        lstFile.clear();
    }
    return lstOSD;
}

void kXneurApp::xNeurConfig::notif_save_list_action_osd(QMap<QString, QMultiMap<bool, QString> > lstOsd)
{
    int i=0;
    QMultiMap<bool, QString> mapTmp;
    QMap<QString, QMultiMap<bool, QString> >::const_iterator p = lstOsd.constBegin();
    while(p!=lstOsd.constEnd())
    {
        mapTmp = p.value();
        QMultiMap<bool, QString>::const_iterator j = mapTmp.constBegin();
        while(j!=mapTmp.constEnd())
        {
             xconfig->osds[i].enabled = j.key();
             if (xconfig->osds[i].file != NULL)
             {
                 delete xconfig->osds[i].file;
             }
             xconfig->osds[i].file = QString("%1").arg(j.value()).toAscii().data();
            ++j;++i;
        }
        ++p;
    }
}

                            /*========== tab POPUP MSG ==========*/
void kXneurApp::xNeurConfig::notif_save_enable_show_popup_msg(bool stat)
{
    xconfig->show_popup = stat;
}

bool kXneurApp::xNeurConfig::notif_get_enable_show_popup_msg()
{
    return xconfig->show_popup;
}

void kXneurApp::xNeurConfig::notif_save_interval_popup_msg(int interval)
{
    xconfig->popup_expire_timeout = interval;
}

int kXneurApp::xNeurConfig::notif_get_interval_popup_msg()
{
    return xconfig->popup_expire_timeout;
}

QMap<QString, QMultiMap<bool, QString> >  kXneurApp::xNeurConfig::notif_get_list_action_popup_msg()
{
    QMap<QString, QMultiMap<bool, QString> > lstPOPUP;
    QMultiMap <bool, QString> lstFile;
    for (int i = 0; i <notifyNames.size(); ++i)
    {
        lstFile.insert(xconfig->popups[i].enabled, QString("%1").arg(xconfig->popups[i].file));
        lstPOPUP.insert(notifyNames.at(i), lstFile);
        lstFile.clear();
    }
    return lstPOPUP;
}
void kXneurApp::xNeurConfig::notif_save_list_action_popup_msg(QMap<QString, QMultiMap<bool, QString> > lstPopMsg)
{
    int i=0;
    QMultiMap<bool, QString> mapTmp;
    QMap<QString, QMultiMap<bool, QString> >::const_iterator p = lstPopMsg.constBegin();
    while(p!=lstPopMsg.constEnd())
    {
        mapTmp = p.value();
        QMultiMap<bool, QString>::const_iterator j = mapTmp.constBegin();
        while(j!=mapTmp.constEnd())
        {
             xconfig->popups[i].enabled = j.key();
             if (xconfig->popups[i].file != NULL)
             {
                 delete xconfig->popups[i].file;
             }
             xconfig->popups[i].file = QString("%1").arg(j.value()).toAscii().data();
            ++j;++i;
        }
        ++p;
    }
}

/*================================= tab Abbreviations =================================*/
void kXneurApp::xNeurConfig::abbr_save_ignore_keyboarf_layout(bool stat)
{
    xconfig->abbr_ignore_layout = stat;
}

bool kXneurApp::xNeurConfig::abbr_get_ignore_keyboard_layout()
{
    return xconfig->abbr_ignore_layout;
}

QMap <QString, QString> kXneurApp::xNeurConfig::abbr_get_list_abbreviations()
{
    QString tmpStr;
    QMap <QString, QString> lstAbb;
    for (int i = 0; i < xconfig->abbreviations->data_count; ++i)
    {
        tmpStr = xconfig->abbreviations->data[i].string;
        lstAbb.insert(tmpStr.left(tmpStr.indexOf(" ")), tmpStr.right(tmpStr.length() -tmpStr.indexOf(" ")));
    }
return lstAbb;
}

void kXneurApp::xNeurConfig::abbr_save_list_abbreviations(QMap <QString, QString> lstAbbr)
{
    QMap <QString, QString>::const_iterator i = lstAbbr.constBegin();
    while (i!= lstAbbr.constEnd())
    {
        xconfig->abbreviations->add(xconfig->abbreviations, QString("%1 %2").arg(i.key()).arg(i.value()).toAscii().data());
        ++i;
    }
}

/*================================= tab Log =================================*/

void kXneurApp::xNeurConfig::log_save_enable_keylog(bool stat)
{
    xconfig->save_keyboard_log =stat;
}

bool kXneurApp::xNeurConfig::log_get_enable_keylog()
{
    return xconfig->save_keyboard_log;
}

void kXneurApp::xNeurConfig::log_save_size_log_file(int size)
{
    xconfig->size_keyboard_log = size;
}

int kXneurApp::xNeurConfig::log_get_size_log_file()
{
    return xconfig->size_keyboard_log;
}

void kXneurApp::xNeurConfig::log_save_email(QString email)
{
//    if (xconfig->mail_keyboard_log != NULL)
//    {
//        delete xconfig->mail_keyboard_log;
//    }
    xconfig->mail_keyboard_log = strdup(email.toAscii().data());
}
QString kXneurApp::xNeurConfig::log_get_email()
{
    return QString("%1").arg(xconfig->mail_keyboard_log);
}

void kXneurApp::xNeurConfig::log_save_host(QString host)
{
    if (xconfig->host_keyboard_log != NULL)
    {
        delete xconfig->host_keyboard_log;
    }
    xconfig->host_keyboard_log = strdup(host.toAscii().data());
}

QString kXneurApp::xNeurConfig::log_get_host()
{
    return QString("%1").arg(xconfig->host_keyboard_log);
}

void kXneurApp::xNeurConfig::log_save_port(int port)
{
    xconfig->port_keyboard_log = port;
}

int kXneurApp::xNeurConfig::log_get_port()
{
    return xconfig->port_keyboard_log;
}

/*============================== tab Troubleshooting ===============================*/

bool kXneurApp::xNeurConfig::trabl_get_backspace()
{
    return xconfig->troubleshoot_backspace;
}

void kXneurApp::xNeurConfig::trabl_save_backspace(bool stat)
{
    xconfig->troubleshoot_backspace = stat;
}

bool kXneurApp::xNeurConfig::trabl_get_left_arrow()
{
    return xconfig->troubleshoot_left_arrow;
}

void kXneurApp::xNeurConfig::trabl_save_left_arrow(bool stat)
{
    xconfig->troubleshoot_left_arrow = stat;
}

bool kXneurApp::xNeurConfig::trabl_get_right_arrow ()
{
    return xconfig->troubleshoot_right_arrow;
}

void kXneurApp::xNeurConfig::trabl_save_right_arrow (bool stat)
{
    xconfig->troubleshoot_right_arrow =stat;
}

bool kXneurApp::xNeurConfig::trabl_get_up_arrow ()
{
    return xconfig->troubleshoot_up_arrow;
}

void kXneurApp::xNeurConfig::trabl_save_up_arrow (bool stat)
{
    xconfig->troubleshoot_up_arrow = stat;
}

bool kXneurApp::xNeurConfig::trabl_get_down_arrow ()
{
    return xconfig->troubleshoot_down_arrow;
}

void kXneurApp::xNeurConfig::trabl_save_down_arrow (bool stat)
{
    xconfig->troubleshoot_down_arrow = stat;
}

bool kXneurApp::xNeurConfig::trabl_get_delete ()
{
    return xconfig->troubleshoot_delete;
}

void kXneurApp::xNeurConfig::trabl_save_delete (bool stat)
{
    xconfig->troubleshoot_delete = stat;
}

bool kXneurApp::xNeurConfig::trabl_get_user_change_layout ()
{
    return xconfig->troubleshoot_switch;
}

void kXneurApp::xNeurConfig::trabl_save_user_change_layout (bool stat)
{
    xconfig->troubleshoot_switch = stat;
}

bool kXneurApp::xNeurConfig::trabl_get_full_screen ()
{
    return xconfig->troubleshoot_full_screen;
}

void kXneurApp::xNeurConfig::trabl_save_full_screen (bool stat)
{
    xconfig->troubleshoot_full_screen = stat;
}

bool kXneurApp::xNeurConfig::trabl_get_flush_buffer_esc ()
{
    return xconfig->flush_buffer_when_press_escape;
}

void kXneurApp::xNeurConfig::trabl_save_flush_buffer_esc (bool stat)
{
    xconfig->flush_buffer_when_press_escape = stat;
}

bool kXneurApp::xNeurConfig::trabl_get_flush_buffer_tab_enter ()
{
    return xconfig->flush_buffer_when_press_enter;
}

void kXneurApp::xNeurConfig::trabl_save_flush_buffer_tab_enter (bool stat)
{
    xconfig->flush_buffer_when_press_enter = stat;
}

bool kXneurApp::xNeurConfig::trabl_get_words_enter_tab ()
{
    return xconfig->dont_process_when_press_enter;
}

void kXneurApp::xNeurConfig::trabl_save_words_enter_tab (bool stat)
{
    xconfig->dont_process_when_press_enter = stat;
}

bool kXneurApp::xNeurConfig::trabl_get_compat_with_completion ()
{
    return xconfig->compatibility_with_completion;
}

void kXneurApp::xNeurConfig::trabl_save_compat_with_completion (bool stat)
{
    xconfig->compatibility_with_completion = stat;
}

bool kXneurApp::xNeurConfig::trabl_get_monitor_input ()
{
    return xconfig->tracking_input;
}

void kXneurApp::xNeurConfig::trabl_save_monitor_input (bool stat)
{
    xconfig->tracking_input = stat;
}

bool kXneurApp::xNeurConfig::trabl_get_monitor_mouse ()
{
    return xconfig->tracking_mouse;
}

void kXneurApp::xNeurConfig::trabl_save_monitor_mouse (bool stat)
{
    xconfig->tracking_mouse = stat;
}

/*================================= tab Advanced =================================*/

void kXneurApp::xNeurConfig::adv_save_delay_sending_events(int time)
{
    xconfig->send_delay = time;
}
int kXneurApp::xNeurConfig::adv_get_delay_sending_events()
{
    return xconfig->send_delay;
}
void kXneurApp::xNeurConfig::adv_save_key_release_event(bool stat)
{
    xconfig->dont_send_key_release =stat;
}
bool kXneurApp::xNeurConfig::adv_get_key_release_event()
{
    return xconfig->dont_send_key_release;
}
void kXneurApp::xNeurConfig::adv_save_log_level(int index)
{
    xconfig->log_level = index;
}
int kXneurApp::xNeurConfig::adv_get_log_level()
{
    return xconfig->log_level;
}

/*================================= tab Plugins =================================*/
QMap<QString, QMultiMap<bool, QString> >  kXneurApp::xNeurConfig::plug_get_list_plugins()
{
    //   filename           bool     description
    QMap<QString, QMultiMap<bool, QString> > lstPlug;
    QMultiMap<bool, QString> plgDesript;
    QDir folder(XNEUR_PLUGIN_DIR);
    QLibrary xLib;
    bool statePlugin=false;
    typedef char *(*plgInfo)();
    QStringList filtr;
    filtr <<"*.so";
    QFileInfoList infoPlugins = folder.entryInfoList(filtr, QDir::Files | QDir::NoDotAndDotDot);

    for(int count=0; count<infoPlugins.size(); ++count)
    {
        xLib.setFileName(infoPlugins.at(count).absoluteFilePath());
        plgInfo libDescription = (plgInfo) xLib.resolve("on_plugin_info");
        if(!libDescription) continue;

        for(int i=0; i< xconfig->plugins->data_count;++i)
        {
            if (xconfig->plugins->data[i].string == infoPlugins.at(count).fileName())
                    statePlugin=true;
        }
        char *mod_info;
        mod_info =libDescription();
        plgDesript.insert(statePlugin,QString("%1").arg(mod_info));
        lstPlug.insert(infoPlugins.at(count).fileName(), plgDesript);
        plgDesript.clear();
        statePlugin =false;
        xLib.unload();
    }
    return lstPlug;
}

void  kXneurApp::xNeurConfig::plug_save_list_plugins(QMap<QString, QMultiMap<bool, QString> > lstPlugin)
{
    QMultiMap<bool, QString> mapTmp;
    QMap<QString, QMultiMap<bool, QString> >::const_iterator i = lstPlugin.constBegin();
    while(i!=lstPlugin.constEnd())
    {
        mapTmp = i.value();
        QMultiMap<bool, QString>::const_iterator j = mapTmp.constBegin();
        while(j!=mapTmp.constEnd())
        {
            if(j.key())
            {
               // qDebug()<< " chek plugin " << i.key();
               xconfig->plugins->add(xconfig->plugins,  QString("%1").arg(i.key()).toAscii().data());
            }
            ++j;
        }
        ++i;
    }

}
