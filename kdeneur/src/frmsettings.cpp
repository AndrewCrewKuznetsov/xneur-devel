#include "frmsettings.h"
#include "ui_frmsettings.h"

//app header
#include "edithotkey.h"

//Qt header files
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDesktopServices>
#include <QMap>

kXneurApp::frmSettings::frmSettings(QWidget *parent, kXneurApp::xNeurConfig *cfg) :  QDialog(parent),  ui(new Ui::frmSettings)
{
  ui->setupUi(this);
  setAttribute( Qt::WA_DeleteOnClose, true);
  ui->tabWidget->setCurrentIndex(0);
  cfgNeur = cfg;
  config = new KConfig("kdeneurrc");
  properties = config->group("Properties");
  settintgGrid();
  createConnect();
  readSettingsKdeNeur();
}

kXneurApp::frmSettings::~frmSettings()
{
  delete ui;
}

void kXneurApp::frmSettings::saveSettingsNeur()
{
    cfgNeur->clearNeurConfig();

    //tab General
    cfgNeur->gen_main_save_manual_switch(ui->chkGenMain_ManualSwitch->isChecked());
    cfgNeur->gen_main_save_auto_learning(ui->chkGenMain_AutoLearning->isChecked());
    cfgNeur->gen_main_save_keep_select(ui->chkGenMain_KeepSelect->isChecked());
    cfgNeur->gen_main_save_rotate_layout(ui->chkGenMain_RotateLayout->isChecked());
    cfgNeur->gen_main_save_check_lang(ui->chkGenMain_CheckLang->isChecked());
    cfgNeur->gen_main_save_check_similar(ui->chkGenMain_CheckSimilar->isChecked());
    cfgNeur->gen_tipo_save_correct_caps(ui->chkGenTipograph_CeorrectCAPS->isChecked());
    cfgNeur->gen_tipo_save_disable_caps(ui->chkGenTipograph_DisableCaps->isChecked());
    cfgNeur->gen_tipo_save_correct_two_caps(ui->chkGenTipograph_CorrectTwoCaps->isChecked());
    cfgNeur->gen_tipo_save_correct_space(ui->chkGenTipograph_CorrectSpace->isChecked());
    cfgNeur->gen_tipo_save_correct_small_letter(ui->chkGenTipograph_CorrectSmallLitter->isChecked());
    cfgNeur->gen_tipo_save_correct_two_space(ui->chkGenTipograph_CorrectTwoSpase->isChecked());
    cfgNeur->gen_tipo_save_correct_two_minus(ui->chkGenTipograph_CorrectTwoMinus->isChecked());
    cfgNeur->gen_tipo_save_correct_c(ui->chkGenTipograph_Correct_c_->isChecked());
    cfgNeur->gen_tipo_save_correct_tm(ui->chkGenTipograph_Correct_tm_->isChecked());
    cfgNeur->gen_tipo_save_correct_r(ui->chkGenTipograph_Correct_r_->isChecked());
    cfgNeur->gen_tipo_save_correct_three_point(ui->chkGenTipograph_Correct_Three_Point->isChecked());
    cfgNeur->gen_tipo_save_correct_dash(ui->chkGenTipograph_CorrectDash->isChecked());
    cfgNeur->gen_tipo_save_correct_misprint(ui->chkGenTipograph_Correct_Misprint->isChecked());

    //tab Layout
    cfgNeur->lay_save_number_layout(ui->Layout_spbLayoutNumber->value()-1);
    cfgNeur->lay_save_remember_layout_for_app(ui->Layout_chkRememberKbLayout->isChecked());
    cfgNeur->lay_save_list_app_one_layout(getListFromWidget(ui->Layout_lstListApplicationOneKbLayout));
    cfgNeur->lay_save_list_language(tab_lay_save_list_lang());

    //tab HotKeys
    cfgNeur->hot_save_list_command_hotkeys(hot_save_list_hotkeys());
    cfgNeur->hot_save_list_user_actions(hot_save_list_user_actions());

    //tab Autocompletion
    cfgNeur->auto_save_enable_pattern(ui->tabAutocompletion_chkTrueAutocomplit->isChecked());
    cfgNeur->auto_save_add_space(ui->tabAutocompletion_chkAddSpace->isChecked());
    cfgNeur->auto_save_list_app_disable_autocomplite(getListFromWidget(ui->tabAutocompletion_lstApp));

    //tab Applications
    cfgNeur->app_save_list_auto_mode_app(getListFromWidget(ui->taApplication_lstAppAutoMode));
    cfgNeur->app_save_list_ignore_app(getListFromWidget(ui->taApplication_lstAppNotUsed));
    cfgNeur->app_save_list_manual_mode_app(getListFromWidget(ui->taApplication_lstAppManualMode));

    //tab Notifications
    cfgNeur->notif_save_enable_sound(ui->tabSound_chkEnableSound->isChecked());
    cfgNeur->notif_save_volume_sound(ui->tabSound_spbSoundVolume->value());
    cfgNeur->notif_save_enable_show_osd(ui->tabOSD_chkEnableOSD->isChecked());
    cfgNeur->notif_save_set_font_osd(ui->tabOSD_txtFontOSD->text());
    cfgNeur->notif_save_enable_show_popup_msg(ui->tabPopupMessage_chkShowPopupMessage->isChecked());
    cfgNeur->notif_save_interval_popup_msg(ui->tabPopupMessage_spbIntervalPopup->value());
    //save list actions (sounf osd popup msg)
    cfgNeur->notif_save_list_action_sound(get_lget_from_notif_widget(ui->tabSound_lstListSound));
    cfgNeur->notif_save_list_action_osd(get_lget_from_notif_widget(ui->tabOSD_lstListOSD));
    cfgNeur->notif_save_list_action_popup_msg(get_lget_from_notif_widget(ui->tabPopupMessage_lstListPopupMessage));

    //tab Abbreviations
    cfgNeur->abbr_save_ignore_keyboarf_layout(ui->tabAbbreviations_chkIgnoreKeyLayout->isChecked());
    cfgNeur->abbr_save_list_abbreviations(abbr_save_list_apprevaitions());

    //tab Log
    cfgNeur->log_save_enable_keylog(ui->tabLog_chkEnableLog->isChecked());
    cfgNeur->log_save_size_log_file(ui->tabLog_spbSizeLog->value());
    cfgNeur->log_save_email(ui->tabLog_txtSendLogEmail->text());
    cfgNeur->log_save_host(ui->tabLog_txtSendLogHost->text());
    cfgNeur->log_save_port(ui->tabLog_spbSendLogPort->value());

    //tab Troubleshooting
    cfgNeur->trabl_save_backspace(ui->tabTroubleshooting_chkBackspace->isChecked());
    cfgNeur->trabl_save_left_arrow(ui->tabTroubleshooting_chkLeftArrow->isChecked());
    cfgNeur->trabl_save_right_arrow(ui->tabTroubleshooting_chkRightArrow->isChecked());
    cfgNeur->trabl_save_up_arrow(ui->tabTroubleshooting_chkUpArrow->isChecked());
    cfgNeur->trabl_save_down_arrow(ui->tabTroubleshooting_chkDownArrow->isChecked());
    cfgNeur->trabl_save_delete(ui->tabTroubleshooting_chkDelete->isChecked());
    cfgNeur->trabl_save_user_change_layout(ui->tabTroubleshooting_chkChangeLayout->isChecked());
    cfgNeur->trabl_save_full_screen(ui->tabTroubleshooting_chkFullScreen->isChecked());
    cfgNeur->trabl_save_flush_buffer_esc(ui->tabTroubleshooting_chkFlushInterBuffEscape->isChecked());
    cfgNeur->trabl_save_flush_buffer_tab_enter(ui->tabTroubleshooting_chkFlushInterBuffEnterTab->isChecked());
    cfgNeur->trabl_save_dont_correct_press_enter(ui->tabTroubleshooting_chkEnter->isChecked());
    cfgNeur->trabl_save_dont_correct_press_tab(ui->tabTroubleshooting_chkTab->isChecked());
    cfgNeur->trabl_save_compat_with_completion(ui->tabTroubleshooting_chkCompatCompletion->isChecked());
    cfgNeur->trabl_save_monitor_input(ui->tabTroubleshooting_chkMonitorInput->isChecked());
    cfgNeur->trabl_save_monitor_mouse(ui->tabTroubleshooting_chkMonitorMouse->isChecked());

    //tab Advanced
    cfgNeur->adv_save_log_level(ui->tabAdvanced_cmbLogLevel->currentIndex());
    cfgNeur->adv_save_delay_sending_events(ui->tabAdvanced_spbDelay->value());
    cfgNeur->adv_save_key_release_app(getListFromWidget(ui->tabAdvanced_tabAppList));

    //tab Plugins
    cfgNeur->plug_save_list_plugins(plug_save_list_plugins());

    cfgNeur->saveNeurConfig();

}

void kXneurApp::frmSettings::settintgGrid()
{

    //general settings all tabwidgen on form
    QList<QTableWidget *> allTable = ui->tabWidget->findChildren<QTableWidget *>();
    for (int i=0; i< allTable.size();++i)
    {
        allTable.at(i)->verticalHeader()->setDefaultSectionSize(22);
        allTable.at(i)->verticalHeader()->hide();
        allTable.at(i)->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
        allTable.at(i)->setSelectionMode(QAbstractItemView::SingleSelection);
        allTable.at(i)->setSelectionBehavior(QAbstractItemView::SelectRows);
        allTable.at(i)->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    //tab layout
    tab_lay_get_list_lang(cfgNeur->lay_get_list_language());
    tab_lay_get_list_app(cfgNeur->lay_get_list_app_one_layout());


    //tab HotKeys
    hot_get_list_hotkeys(cfgNeur->hot_get_list_command_hotkeys());
    hot_get_list_user_actions(cfgNeur->hot_get_list_user_actions());


    //tab autocomplection
    auto_get_list_app_autocomp(cfgNeur->auto_get_list_app_disable_autocomplite());

    //tab application
    ui->taApplication_lstAppNotUsed->addItems(cfgNeur->app_get_list_ignore_app());
    ui->taApplication_lstAppAutoMode->addItems(cfgNeur->app_get_list_auto_mode_app());
    ui->taApplication_lstAppManualMode->addItems(cfgNeur->app_get_list_manual_mode_app());

    //tab Notifications
    notif_get_list_action_sound(cfgNeur->notif_get_list_action_sound());
    notif_get_list_action_osd(cfgNeur->notif_get_list_action_osd());
    notif_get_list_action_popup(cfgNeur->notif_get_list_action_popup_msg());

    //tab Advanced
    adv_get_list_key_release_app(cfgNeur->adv_get_key_release_app());

    //tab Abbreviations
    abbr_get_list_abbreviations(cfgNeur->abbr_get_list_abbreviations());

    //tab Plugins
    plug_get_list_plugins(cfgNeur->plug_get_list_plugins());
}

void kXneurApp::frmSettings::readSettingsKdeNeur()
{
    readSettingsNeur();

    //tab log
    QString view=properties.readEntry("Viewer","browser");
    (view=="browser")?ui->tabLog_cmbOpenLongIn->setCurrentIndex(0): ui->tabLog_cmbOpenLongIn->setCurrentIndex(1);
    //tab Properties
    ui->tabProperties_chkEnableAutostart->setChecked(properties.readEntry("Autostart",false));
    if(ui->tabProperties_chkEnableAutostart->isChecked())
    {
        ui->tabProperties_spbDelayStartApp->setValue(properties.readEntry("WaiTime",15));
        ui->tabProperties_spbDelayStartApp->setEnabled(true);

    }
    ui->tabProperties_cmbTypeIconTray->setCurrentIndex(properties.readEntry("Typeicontray",0));
    if(ui->tabProperties_cmbTypeIconTray->currentIndex()==1)
    {
        ui->tabProperties_grpFolderIcon->setEnabled(true);
        ui->tabProperties_txtPathIconTray->setText(properties.readEntry("Iconpath",""));
    }
}

void kXneurApp::frmSettings::readSettingsNeur()
{

    //tab General
    ui->chkGenMain_ManualSwitch->setChecked(cfgNeur->gen_main_get_manual_switch());
    ui->chkGenMain_AutoLearning->setChecked(cfgNeur->gen_main_get_auto_learning());
    ui->chkGenMain_KeepSelect->setChecked(cfgNeur->gen_main_get_keep_select());
    ui->chkGenMain_RotateLayout->setChecked(cfgNeur->gen_main_get_rotate_layout());
    ui->chkGenMain_CheckLang->setChecked(cfgNeur->gen_main_get_check_lang());
    ui->chkGenMain_CheckSimilar->setChecked(cfgNeur->gen_main_get_check_similar());
    ui->chkGenTipograph_CeorrectCAPS->setChecked(cfgNeur->gen_tipo_get_correct_caps());
    ui->chkGenTipograph_DisableCaps->setChecked(cfgNeur->gen_tipo_get_disable_caps());
    ui->chkGenTipograph_CorrectTwoCaps->setChecked(cfgNeur->gen_tipo_get_correct_two_caps());
    ui->chkGenTipograph_CorrectSpace->setChecked(cfgNeur->gen_tipo_get_correct_space());
    ui->chkGenTipograph_CorrectSmallLitter->setChecked(cfgNeur->gen_tipo_get_correct_small_letter());
    ui->chkGenTipograph_CorrectTwoSpase->setChecked(cfgNeur->gen_tipo_get_correct_two_space());
    ui->chkGenTipograph_CorrectTwoMinus->setChecked(cfgNeur->gen_tipo_get_correct_two_minus());
    ui->chkGenTipograph_Correct_c_->setChecked(cfgNeur->gen_tipo_get_correct_c());
    ui->chkGenTipograph_Correct_tm_->setChecked(cfgNeur->gen_tipo_get_correct_tm());
    ui->chkGenTipograph_Correct_r_->setChecked(cfgNeur->gen_tipo_get_correct_r());
    ui->chkGenTipograph_CorrectDash->setChecked(cfgNeur->gen_tipo_get_correct_dash());
    ui->chkGenTipograph_Correct_Three_Point->setChecked(cfgNeur->gen_tipo_get_correct_three_point());
    ui->chkGenTipograph_Correct_Misprint->setChecked(cfgNeur->gen_tipo_get_correct_misprint());

    //tab Layout
    ui->Layout_spbLayoutNumber->setValue(cfgNeur->lay_get_number_layout());
    ui->Layout_chkRememberKbLayout->setChecked(cfgNeur->lay_get_remember_layout_for_app());

    //tab Autocompletion
    ui->tabAutocompletion_chkTrueAutocomplit->setChecked(cfgNeur->auto_get_enable_pattern());
    ui->tabAutocompletion_chkAddSpace->setChecked(cfgNeur->auto_get_add_space());

    //tab Notifications
        //sound
        ui->tabSound_chkEnableSound->setChecked(cfgNeur->notif_get_enable_sound());
        ui->tabSound_spbSoundVolume->setValue(cfgNeur->notif_get_volume_sound());
        //osd
        ui->tabOSD_chkEnableOSD->setChecked(cfgNeur->notif_get_enable_show_osd());
        ui->tabOSD_txtFontOSD->setText(cfgNeur->notif_get_font_osd());
        //Popup msg
        ui->tabPopupMessage_chkShowPopupMessage->setChecked(cfgNeur->notif_get_enable_show_popup_msg());
        ui->tabPopupMessage_spbIntervalPopup->setValue(cfgNeur->notif_get_interval_popup_msg());
    //tab Abbreviations
    ui->tabAbbreviations_chkIgnoreKeyLayout->setChecked(cfgNeur->abbr_get_ignore_keyboard_layout());

    //tab Log
    ui->tabLog_chkEnableLog->setChecked(cfgNeur->log_get_enable_keylog());
    ui->tabLog_spbSizeLog->setValue(cfgNeur->log_get_size_log_file());
    ui->tabLog_txtSendLogEmail->setText(cfgNeur->log_get_email());
    ui->tabLog_txtSendLogHost->setText(cfgNeur->log_get_host());
    ui->tabLog_spbSendLogPort->setValue(cfgNeur->log_get_port());

    //tab Troubleshooting

    ui->tabTroubleshooting_chkBackspace->setChecked(cfgNeur->trabl_get_backspace());
    ui->tabTroubleshooting_chkLeftArrow->setChecked(cfgNeur->trabl_get_left_arrow());
    ui->tabTroubleshooting_chkRightArrow->setChecked(cfgNeur->trabl_get_right_arrow());
    ui->tabTroubleshooting_chkUpArrow->setChecked(cfgNeur->trabl_get_up_arrow());
    ui->tabTroubleshooting_chkDownArrow->setChecked(cfgNeur->trabl_get_down_arrow());
    ui->tabTroubleshooting_chkDelete->setChecked(cfgNeur->trabl_get_delete());
    ui->tabTroubleshooting_chkChangeLayout->setChecked(cfgNeur->trabl_get_user_change_layout());
    ui->tabTroubleshooting_chkFullScreen->setChecked(cfgNeur->trabl_get_full_screen());

    ui->tabTroubleshooting_chkFlushInterBuffEscape->setChecked(cfgNeur->trabl_get_flush_buffer_esc());
    ui->tabTroubleshooting_chkFlushInterBuffEnterTab->setChecked(cfgNeur->trabl_get_flush_buffer_tab_enter());
    ui->tabTroubleshooting_chkCompatCompletion->setChecked(cfgNeur->trabl_get_compat_with_completion());
    ui->tabTroubleshooting_chkEnter->setChecked(cfgNeur->trabl_get_dont_correct_press_enter());
    ui->tabTroubleshooting_chkTab->setChecked(cfgNeur->trabl_get_dont_correct_press_tab());
    ui->tabTroubleshooting_chkMonitorInput->setChecked(cfgNeur->trabl_get_monitor_input());
    ui->tabTroubleshooting_chkMonitorMouse->setChecked(cfgNeur->trabl_get_monitor_mouse());

    //tab Advanced
   //ui->tabAdvanced_chkKeyRelease->setChecked(cfgNeur->adv_get_key_release_event());
   ui->tabAdvanced_spbDelay->setValue(cfgNeur->adv_get_delay_sending_events());
   ui->tabAdvanced_cmbLogLevel->setCurrentIndex(cfgNeur->adv_get_log_level());
}

void kXneurApp::frmSettings::Clicked(QAbstractButton *button)
{
  if(ui->cmdBox->standardButton(button) == QDialogButtonBox::Ok)
  {
      done(QDialog::Accepted);
      saveSettingsNeur();
      config->sync();
      this->close();
  }
  else
  {
      done(QDialog::Rejected);
      this->close();
  }
}

void kXneurApp::frmSettings::createConnect()
{
  connect(ui->cmdBox, SIGNAL(clicked(QAbstractButton*)), SLOT(Clicked(QAbstractButton*)));

  //tab layout
  connect(ui->Layout_AddApp, SIGNAL(clicked()), SLOT(addApp_OneLayout()));
  connect(ui->Layout_DelApp,SIGNAL(clicked()),SLOT(removeApp_OneLayout()));
  connect(ui->Layout_cmdRulesChange, SIGNAL(clicked()),SLOT(rulesChange()));

  //tab hotkeys
  connect(ui->tabHotKeys_EditHotKey, SIGNAL(clicked()), SLOT(editHotkey()));
  connect(ui->tabHotKeys_ClearHotKey, SIGNAL(clicked()),SLOT(clearHotKey()));
  connect(ui->tabHotKeys_UserActionsDel, SIGNAL(clicked()), SLOT(removeUserAction()));
  connect(ui->tabHotKeys_UserActionsAdd, SIGNAL(clicked()), SLOT(addUserAction()));
  connect(ui->tabHotKeys_UserActionsEdit, SIGNAL(clicked()), SLOT(editUserAction()));

  //tab autocompletion
  connect(ui->tabAutocompletion_cmdAddApp, SIGNAL(clicked()),SLOT(auto_add_app_list_disable_autocompletion()));
  connect(ui->tabAutocompletion_cmdDelApp, SIGNAL(clicked()),SLOT(auto_del_app_list_disable_autocompletion()));

  //tab applications
  connect(ui->taApplication_cmdAdd_NotUsed,SIGNAL(clicked()),SLOT(app_add_app_ignore_list()));
  connect(ui->taApplication_cmdDel_NotUsed,SIGNAL(clicked()),SLOT(app_del_app_ignore_list()));
  connect(ui->taApplication_cmdAdd_AutoMode,SIGNAL(clicked()),SLOT(app_add_app_auto_mode_list()));
  connect(ui->taApplication_cmdDel_AutoMode, SIGNAL(clicked()),SLOT(app_del_app_auto_mode_list()));
  connect(ui->taApplication_cmdAdd_ManualMode, SIGNAL(clicked()),SLOT(app_add_app_manual_mode_list()));
  connect(ui->taApplication_cmdDel_ManualMode, SIGNAL(clicked()),SLOT(app_del_app_manual_mode_list()));

  //tab notifications
  connect(ui->tabSound_lstListSound, SIGNAL(cellDoubleClicked(int,int)),SLOT(notif_dublClicked(int,int)));
  connect(ui->tabOSD_lstListOSD, SIGNAL(cellDoubleClicked(int,int)),SLOT(notif_dublClicked(int,int)));
  connect(ui->tabPopupMessage_lstListPopupMessage, SIGNAL(cellDoubleClicked(int,int)),SLOT(notif_dublClicked(int,int)));

  //tab abbreviations
  connect(ui->tabAbbreviations_cmdAdd, SIGNAL(clicked()), SLOT(addAbbreviation()));
  connect(ui->tabAbbreviations_cmdDel, SIGNAL(clicked()), SLOT(delAbbreviation()));


  //tab log
  connect(ui->tabLog_cmbOpenLongIn, SIGNAL(currentIndexChanged(int)), SLOT(openLoFileIn(int)));
  connect(ui->tabLog_cmdOpenLog, SIGNAL(clicked()),SLOT(openLogFile()));

  //tab advanced
  connect(ui->tabAdvanced_cmdAddApp, SIGNAL(clicked()), SLOT(addApp_tabAdvanced()));
  connect(ui->tabAdvanced_cmdRemoveApp, SIGNAL(clicked()), SLOT(removeApp_tabAdvanced()));

  //tab properties
  connect(ui->tabProperties_cmdRecoverKeyCommand, SIGNAL(clicked()), SLOT(RecoverKeyboardCommand()));
  connect(ui->tabProperties_cmdEditKeyCommand, SIGNAL(clicked()),SLOT(EditKeyboardCommand()));
  connect(ui->tabProperties_cmbTypeIconTray, SIGNAL(activated(int)),SLOT(TypeIconTray(int)));
  connect(ui->tabProperties_cmdBrowseIconTray, SIGNAL(clicked()),SLOT(BrowseIconTray()));
  connect(ui->tabProperties_chkEnableAutostart, SIGNAL(clicked(bool)), SLOT(chekAutostart(bool)));
  connect(ui->tabProperties_spbDelayStartApp, SIGNAL(valueChanged(int)), SLOT(delayStartApp(int)));
}

void kXneurApp::frmSettings::RecoverKeyboardCommand()
{
    ui->tabProperties_txtKeyboardCommad->clear();
    ui->tabProperties_txtKeyboardCommad->setText("/usr/bin/kcmshell4 --args=--tab=layouts kcm_keyboard");
    properties.writeEntry("KeyboardCommand", "/usr/bin/kcmshell4 --args=--tab=layouts kcm_keyboard");
}

void kXneurApp::frmSettings::openLoFileIn(int index)
{
    QString view;
    (index==0)?view="browser":view="viewer";
    properties.writeEntry("Viewer", view);
}

void kXneurApp::frmSettings::openLogFile()
{
    QString logFile = QString("%1/%2").arg(QDir::homePath()).arg(".xneur/xneurlog.html");
    if (QFile::exists(logFile))
    {
        int viewer = ui->tabLog_cmbOpenLongIn->currentIndex();
        switch(viewer)
        {
        case 0:
            QDesktopServices::openUrl(QUrl(logFile));
            break;
        case 1:
            break;
        }
    }
    else
    {
        QMessageBox::warning(0,tr("Log file not found..."),tr("Log file xneurlog.html not found. Maybe you do not have the option of logging!"));
    }
}

void kXneurApp::frmSettings::EditKeyboardCommand()
{
    ui->tabProperties_txtKeyboardCommad->setText(QFileDialog::getOpenFileName(0,tr("Select execute file"), "/usr/bin"));
    if(!ui->tabProperties_txtKeyboardCommad->text().isEmpty())
    {
      properties.writeEntry("KeyboardCommand", ui->tabProperties_txtKeyboardCommad->text());
    }
}


void kXneurApp::frmSettings::TypeIconTray(int selectIndex)
{
    switch(selectIndex)
    {
    case 1:
        ui->tabProperties_grpFolderIcon->setEnabled(true);
        break;
    default:
        ui->tabProperties_grpFolderIcon->setEnabled(false);
        break;
    }
    properties.writeEntry("Typeicontray", selectIndex);
}

void kXneurApp::frmSettings::BrowseIconTray()
{
    ui->tabProperties_txtPathIconTray->setText(QFileDialog::getExistingDirectory(this, tr("Select Directory with icon tray"),"",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks));
    if(! ui->tabProperties_txtPathIconTray->text().isEmpty())
    {
      properties.writeEntry("Iconpath",  ui->tabProperties_txtPathIconTray->text());
    }
}


void kXneurApp::frmSettings::chekAutostart(bool cheked)
{
    KStandardDirs dir;
    QString kdeneur = "kdeneur.desktop";
    QString str="[Desktop Entry]\nCategories=Qt;KDE;Utility;\nComment[ru]=Автоматический переключатель раскладки клавиатуры\nComment=Автоматический переключатель раскладки клавиатуры\nExec=kdeneur\nIcon=kdexneur\nName[ru]=kdeNeur\nName=kdeNeur\nStartupNotify=true\nTerminal=false\nType=Application\nX-KDE-SubstituteUID=false";
    QString pathAutostart = dir.findResourceDir("xdgconf-autostart", "");
    QString fileDesktop = QString("%1%2").arg(pathAutostart).arg(kdeneur);

    if(cheked && KStandardDirs::exists(pathAutostart))
    {
        QFile file(fileDesktop);
        QFileInfo tmp_file(file);
        if(file.open(QIODevice::WriteOnly))
        {
           QTextStream out(&file);
           out.setCodec("UTF-8");
           out <<str;
           qDebug()<< "FILE AUTOSTART IS fileDesktop " <<fileDesktop;
           qDebug()<< "FILE AUTOSTART IS FILE " <<tmp_file.absoluteFilePath();
           file.close();
        }
       properties.writeEntry("WaiTime",  ui->tabProperties_spbDelayStartApp->value());

    }
    else if(QFile::exists(fileDesktop))
    {
        if(!QFile::remove(fileDesktop))
        {
            qDebug()<<"ERROR: Don't del file";
        }
        properties.deleteEntry("WaiTime");
    }
    properties.writeEntry("Autostart",  cheked);
    ui->tabProperties_chkEnableAutostart->setEnabled(cheked);
    ui->tabProperties_spbDelayStartApp->setEnabled(cheked);
}

void kXneurApp::frmSettings::delayStartApp(int val)
{
    properties.writeEntry("WaiTime",  val);
}

void kXneurApp::frmSettings::addAbbreviation()
{
    kXneurApp::frmAddAbbreviature *frmAbb = new kXneurApp::frmAddAbbreviature();

    if(frmAbb->exec() == QDialog::Accepted)
    {
        ui->tabAbbreviations_lstListAbbreviations->setRowCount(ui->tabAbbreviations_lstListAbbreviations->rowCount()+1);
        ui->tabAbbreviations_lstListAbbreviations->setItem(ui->tabAbbreviations_lstListAbbreviations->rowCount()-1, 0, new QTableWidgetItem(frmAbb->abb));
        ui->tabAbbreviations_lstListAbbreviations->setItem(ui->tabAbbreviations_lstListAbbreviations->rowCount()-1, 1, new QTableWidgetItem(frmAbb->text));
    }
    delete frmAbb;
}

void kXneurApp::frmSettings::delAbbreviation()
{
    int row = ui->tabAbbreviations_lstListAbbreviations->currentRow();
    if(row <0)
    {
        QMessageBox::information(0, tr("Warning...."), tr("You don't select abbreviation for remove."), QMessageBox::Ok);
    }
    else
    {
        ui->tabAbbreviations_lstListAbbreviations->removeRow(row);
    }

}

void kXneurApp::frmSettings::tab_lay_get_list_lang(QStringList lstLng)
{
    ui->Layout_lstLayout->setRowCount(lstLng.size()/3);
    ui->Layout_lstLayout->setColumnCount(3);
    //create headers table
    ui->Layout_lstLayout->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Description")));
    ui->Layout_lstLayout->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Layout")));
    ui->Layout_lstLayout->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Excluded")));

    int p=0;
    bool ok;
    for(int j=0;j<lstLng.size()/3;j++)
    {
        for (int i=0;i<3;i++)
        {
            lstLng.at(p).toInt(&ok);
            if(!ok)
            {
                ui->Layout_lstLayout->setItem(j,i, new QTableWidgetItem(lstLng.at(p)));
            }
            else
            {
                QTableWidgetItem *itm = new QTableWidgetItem();
                (lstLng.at(p).toInt()==0) ? itm->setCheckState(Qt::Unchecked) :itm->setCheckState(Qt::Checked) ;
                ui->Layout_lstLayout->setItem(j,i,itm);
            }
            p++;
        }
    }
}

void kXneurApp::frmSettings::adv_get_list_key_release_app(QStringList lstApp)
{
    ui->tabAdvanced_tabAppList->addItems(lstApp);
}


void kXneurApp::frmSettings::tab_lay_get_list_app(QStringList lstApp)
{
    ui->Layout_lstListApplicationOneKbLayout->addItems(lstApp);
}

QStringList kXneurApp::frmSettings::getListFromWidget(QListWidget *wid)
{
    QStringList lstApp;
    QList<QListWidgetItem *> items =wid->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    foreach(QListWidgetItem *item, items)
    {
      lstApp<<item->text();
    }
    return lstApp;
}

void kXneurApp::frmSettings::add_Application_to_Widget(QListWidget *wid)
{
    kXneurApp::getNameApp *frm = new kXneurApp::getNameApp();
    if(frm->exec() == QDialog::Accepted)
    {
        QString str = frm->appName;
        wid->addItem(new QListWidgetItem(str));
    }
    delete frm;
}

void kXneurApp::frmSettings::del_Application_to_Widget(QListWidget *wid)
{
    if (wid->currentRow()<0)
    {
        QMessageBox::information(0,tr("Nothing deleted"), tr("You don't select an application that must be removed"), QMessageBox::Ok);
    }
    else
    {
        delete wid->takeItem(wid->currentRow());
    }
}

void kXneurApp::frmSettings::addApp_OneLayout()
{
    add_Application_to_Widget(ui->Layout_lstListApplicationOneKbLayout);
}

void kXneurApp::frmSettings::removeApp_OneLayout()
{
    del_Application_to_Widget( ui->Layout_lstListApplicationOneKbLayout);
}

//show  rule change layout
void kXneurApp::frmSettings::rulesChange()
{
    int row = ui->Layout_lstLayout->currentRow();
    if(row < 0)
    {
        qDebug()<< tr("WAR: Don't select language!");
        QMessageBox::information(0, tr("Warning...."), tr("You don't select lang"), QMessageBox::Ok);
    }
    else
    {
        QStringList text = cfgNeur->lay_get_text_dictionary(ui->Layout_lstLayout->item(row, 1)->text());
        kXneurApp::RulesChange *frm = new kXneurApp::RulesChange(text);
        frm->exec();
        delete frm;
    }
}

void kXneurApp::frmSettings::auto_get_list_app_autocomp(QStringList lstApp)
{
    ui->tabAutocompletion_lstApp->addItems(lstApp);
}

void kXneurApp::frmSettings::hot_get_list_hotkeys(QMap<QString, QString> lstCommand)
{
    ui->tabHotKey_lstHotKey->setRowCount(lstCommand.size());
    ui->tabHotKey_lstHotKey->setColumnCount(2);
    QString s1,s2;

    QMap<QString, QString>::const_iterator i = lstCommand.constBegin();
    int p=0;
    while (i != lstCommand.constEnd())
    {
       s1 =i.key();
       s2 = i.value();
        ui->tabHotKey_lstHotKey->setItem(p,0, new QTableWidgetItem(s1));
        ui->tabHotKey_lstHotKey->setItem(p,1, new QTableWidgetItem(s2));
     //   qDebug() << s1 << "    " << s2;
        ++p;++i;
    }
}

QMap<QString, QString> kXneurApp::frmSettings::hot_save_list_hotkeys()
{
    QMap<QString, QString> lstHotKey;
    for (int i=0; i< ui->tabHotKey_lstHotKey->rowCount();++i)
    {
        lstHotKey.insert(ui->tabHotKey_lstHotKey->item(i,0)->text(),ui->tabHotKey_lstHotKey->item(i,1)->text());
    }
    return lstHotKey;
}


void kXneurApp::frmSettings::hot_get_list_user_actions(QMap<QString, QMap<QString, QString> > lstUserActions)
{
    ui->tabHotKey_lstUserActions->setRowCount(lstUserActions.size());
    ui->tabHotKey_lstUserActions->setColumnCount(3);
    QMap<QString, QString> tmpMap;
    int p=0;

    QMap<QString, QMap<QString, QString> >::const_iterator i = lstUserActions.constBegin();
    while (i != lstUserActions.constEnd())
    {
        ui->tabHotKey_lstUserActions->setItem(p,1, new QTableWidgetItem(i.key()));
        tmpMap = i.value();
        QMap<QString, QString>::const_iterator j = tmpMap.constBegin();
        while (j != tmpMap.constEnd())
        {
            ui->tabHotKey_lstUserActions->setItem(p,0, new QTableWidgetItem(j.key()));
            ui->tabHotKey_lstUserActions->setItem(p,2, new QTableWidgetItem(j.value()));
            ++j;
        }
        ++p;++i;
    }
}

QMap<QString, QMap<QString, QString> > kXneurApp::frmSettings::hot_save_list_user_actions()
{
    QMap<QString, QMap<QString, QString> >  lstActions;
    QMap<QString, QString> lstcommand;

    for(int i=0; i< ui->tabHotKey_lstUserActions->rowCount();++i)
    {
        lstcommand.insert(ui->tabHotKey_lstUserActions->item(i,0)->text(),ui->tabHotKey_lstUserActions->item(i,2)->text());
        lstActions.insert(ui->tabHotKey_lstUserActions->item(i,1)->text(), lstcommand);
        lstcommand.clear();
    }
    return lstActions;
}

void kXneurApp::frmSettings::notif_get_list_action_sound(QMap<QString, QMultiMap<bool, QString> > lstActions)
{
    ui->tabSound_lstListSound->setRowCount(lstActions.size());
    ui->tabSound_lstListSound->setColumnCount(3);
    ui->tabSound_lstListSound->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    ui->tabSound_lstListSound->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    ui->tabSound_lstListSound->horizontalHeader()->setResizeMode(2, QHeaderView::Fixed);
    ui->tabSound_lstListSound->setObjectName("sound");

    QMultiMap<bool, QString> tmpMap;
    int p=0;

    QMap<QString, QMultiMap<bool, QString> >::const_iterator i = lstActions.constBegin();
    while (i != lstActions.constEnd())
    {
        QTableWidgetItem *itmNoEdit = new QTableWidgetItem();
        itmNoEdit->setText(i.key());
        itmNoEdit->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->tabSound_lstListSound->setItem(p,0,itmNoEdit);
        //ui->tabSound_lstListSound->setItem(p,0, new QTableWidgetItem(i.key()));
        tmpMap = i.value();
        QMultiMap<bool, QString>::const_iterator j = tmpMap.constBegin();
        while( j!= tmpMap.constEnd())
        {
            QTableWidgetItem *itm = new QTableWidgetItem();
            itm->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
            (j.key()) ? itm->setCheckState(Qt::Checked):itm->setCheckState(Qt::Unchecked);
            ui->tabSound_lstListSound->setItem(p,2, itm);
            ui->tabSound_lstListSound->setItem(p,1, new QTableWidgetItem(j.value()));
            ++j;
        }
        ++p;++i;
    }
}

void kXneurApp::frmSettings::notif_get_list_action_osd(QMap<QString, QMultiMap<bool, QString> > lstActions)
{
    ui->tabOSD_lstListOSD->setRowCount(lstActions.size());
    ui->tabOSD_lstListOSD->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    ui->tabOSD_lstListOSD->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    ui->tabOSD_lstListOSD->horizontalHeader()->setResizeMode(2, QHeaderView::Fixed);
    ui->tabOSD_lstListOSD->setColumnCount(3);
    ui->tabOSD_lstListOSD->setObjectName("osd");
    QMultiMap<bool, QString> tmpMap;
    int p=0;

    QMap<QString, QMultiMap<bool, QString> >::const_iterator i = lstActions.constBegin();
    while (i != lstActions.constEnd())
    {
        QTableWidgetItem *itmNoEdit = new QTableWidgetItem();
        itmNoEdit->setText(i.key());
        itmNoEdit->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->tabOSD_lstListOSD->setItem(p,0,itmNoEdit);
       // ui->tabOSD_lstListOSD->setItem(p,0, new QTableWidgetItem(i.key()));
        tmpMap = i.value();
        QMultiMap<bool, QString>::const_iterator j = tmpMap.constBegin();
        while( j!= tmpMap.constEnd())
        {
            QTableWidgetItem *itm = new QTableWidgetItem();
             itm->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
            (j.key()) ? itm->setCheckState(Qt::Checked):itm->setCheckState(Qt::Unchecked);
            ui->tabOSD_lstListOSD->setItem(p,2, itm);
            ui->tabOSD_lstListOSD->setItem(p,1, new QTableWidgetItem(j.value()));
            ++j;
        }
        ++p;++i;
    }
}

void kXneurApp::frmSettings::notif_get_list_action_popup(QMap<QString, QMultiMap<bool, QString> > lstActions)
{
    ui->tabPopupMessage_lstListPopupMessage->setRowCount(lstActions.size());
    ui->tabPopupMessage_lstListPopupMessage->setColumnCount(3);
    ui->tabPopupMessage_lstListPopupMessage->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    ui->tabPopupMessage_lstListPopupMessage->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    ui->tabPopupMessage_lstListPopupMessage->horizontalHeader()->setResizeMode(2, QHeaderView::Fixed);
    ui->tabPopupMessage_lstListPopupMessage->setObjectName("popup");
    QMultiMap<bool, QString> tmpMap;
    int p=0;

    QMap<QString, QMultiMap<bool, QString> >::const_iterator i = lstActions.constBegin();
    while (i != lstActions.constEnd())
    {
        QTableWidgetItem *itmNoEdit = new QTableWidgetItem();
        itmNoEdit->setText(i.key());
        itmNoEdit->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->tabPopupMessage_lstListPopupMessage->setItem(p,0,itmNoEdit);
      //  ui->tabPopupMessage_lstListPopupMessage->setItem(p,0, new QTableWidgetItem(i.key()));
        tmpMap = i.value();
        QMultiMap<bool, QString>::const_iterator j = tmpMap.constBegin();
        while( j!= tmpMap.constEnd())
        {
            QTableWidgetItem *itm = new QTableWidgetItem();
            itm->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
            (j.key()) ? itm->setCheckState(Qt::Checked):itm->setCheckState(Qt::Unchecked);
            ui->tabPopupMessage_lstListPopupMessage->setItem(p,2, itm);
            ui->tabPopupMessage_lstListPopupMessage->setItem(p,1, new QTableWidgetItem(j.value()));
            ++j;
        }
        ++p;++i;
    }
}

void kXneurApp::frmSettings::abbr_get_list_abbreviations(QMap<QString, QString> lstAbb)
{
    ui->tabAbbreviations_lstListAbbreviations->setRowCount(lstAbb.size());
    ui->tabAbbreviations_lstListAbbreviations->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    ui->tabAbbreviations_lstListAbbreviations->horizontalHeader()->stretchLastSection();
    int p=0;
    QMap <QString, QString>::const_iterator i = lstAbb.constBegin();
    while (i != lstAbb.constEnd())
    {
        ui->tabAbbreviations_lstListAbbreviations->setItem(p, 0, new QTableWidgetItem(QString("%1").arg(i.key()).trimmed()));
        ui->tabAbbreviations_lstListAbbreviations->setItem(p, 1, new QTableWidgetItem(QString("%1").arg(i.value()).trimmed()));
        ++p;++i;
    }
}
QMap <QString, QString> kXneurApp::frmSettings::abbr_save_list_apprevaitions()
{
    QMap <QString, QString> lstAbbr;
    QString key, value;
    for(int i=0; i < ui->tabAbbreviations_lstListAbbreviations->rowCount();++i)
    {
        key=ui->tabAbbreviations_lstListAbbreviations->item(i, 0)->text();
        value = ui->tabAbbreviations_lstListAbbreviations->item(i,1)->text();
        lstAbbr.insert(key,value);
        key=value="";
    }
    return lstAbbr;
}

void kXneurApp::frmSettings::plug_get_list_plugins(QMap<QString, QMultiMap<bool, QString> > lstPlg)
{
    ui->tabPlugins_lstListPlugins->setRowCount(lstPlg.size());
    ui->tabPlugins_lstListPlugins->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    ui->tabPlugins_lstListPlugins->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
    ui->tabPlugins_lstListPlugins->horizontalHeader()->setResizeMode(2,QHeaderView::ResizeToContents);
    //ui->tabPlugins_lstListPlugins->horizontalHeader()->stretchLastSection();

    QMultiMap<bool, QString> tmpMap;
    int p=0;

    QMap<QString, QMultiMap<bool, QString> >::const_iterator i = lstPlg.constBegin();
    while (i != lstPlg.constEnd())
    {
        ui->tabPlugins_lstListPlugins->setItem(p,2, new QTableWidgetItem(i.key()));
        tmpMap = i.value();
        QMultiMap<bool, QString>::const_iterator j = tmpMap.constBegin();
        while( j!= tmpMap.constEnd())
        {
            QTableWidgetItem *itm = new QTableWidgetItem();
            (j.key()) ? itm->setCheckState(Qt::Checked):itm->setCheckState(Qt::Unchecked);
            ui->tabPlugins_lstListPlugins->setItem(p,0, itm);
            ui->tabPlugins_lstListPlugins->setItem(p,1, new QTableWidgetItem(j.value()));
            ++j;
        }
        ++p;++i;
    }
}

QMap<QString, QMultiMap<bool, QString> > kXneurApp::frmSettings::plug_save_list_plugins()
{
    QMultiMap<bool, QString> mapTmp;
    bool chk =false;
    QMap<QString, QMultiMap<bool, QString> > lstPlugin;
    for (int i=0; i< ui->tabPlugins_lstListPlugins->rowCount();++i)
    {
        chk = false;
        if(ui->tabPlugins_lstListPlugins->item(i, 0)->checkState()==Qt::Checked)
            chk =true;
        mapTmp.insert(chk, ui->tabPlugins_lstListPlugins->item(i,1)->text());
        lstPlugin.insert(ui->tabPlugins_lstListPlugins->item(i,2)->text(), mapTmp);
        mapTmp.clear();
    }
    return lstPlugin;
}

QHash <QString, bool > kXneurApp::frmSettings::tab_lay_save_list_lang()
{
    QHash <QString, bool > lstLang;
    for (int i=0; i< ui->Layout_lstLayout->rowCount(); ++i)
    {
        (ui->Layout_lstLayout->item(i,2)->checkState()==Qt::Unchecked)?
                    lstLang.insert(ui->Layout_lstLayout->item(i,1)->text(),false):
                    lstLang.insert(ui->Layout_lstLayout->item(i,1)->text(),true);
    }
    return  lstLang;
}

void kXneurApp::frmSettings::editHotkey()
{
    int row=ui->tabHotKey_lstHotKey->currentRow();
    if(row<0)
    {
        //TODO
    }
    else
    {
        QString action=ui->tabHotKey_lstHotKey->item(row, 0)->text();
        QString key=ui->tabHotKey_lstHotKey->item(row, 1)->text();
        kXneurApp::EditHotKey *frm = new kXneurApp::EditHotKey(0,action, key);
        if(frm->exec()==QDialog::Accepted)
        {
            ui->tabHotKey_lstHotKey->item(row, 1)->setText(frm->hot_keys);
        }
        delete frm;
    }
}

void kXneurApp::frmSettings::removeUserAction()
{
    int row = ui->tabHotKey_lstUserActions->currentRow();

    if(row< 0)
    {
        //TODO
    }
    else
    {
        ui->tabHotKey_lstUserActions->removeRow(row);
    }
}

void kXneurApp::frmSettings::addUserAction()
{
    kXneurApp::addUserAction *frmUsrAction = new kXneurApp::addUserAction();
    if(frmUsrAction->exec() ==QDialog::Accepted)
    {
        int row = ui->tabHotKey_lstUserActions->rowCount();
        ui->tabHotKey_lstUserActions->insertRow(row);
        ui->tabHotKey_lstUserActions->setItem(row,0, new QTableWidgetItem(frmUsrAction->name));
        ui->tabHotKey_lstUserActions->setItem(row,1, new QTableWidgetItem(frmUsrAction->hot_key));
        ui->tabHotKey_lstUserActions->setItem(row,2, new QTableWidgetItem(frmUsrAction->command));
    }
    delete frmUsrAction;
}

void kXneurApp::frmSettings::editUserAction()
{
    int row = ui->tabHotKey_lstUserActions->currentRow();
    if (row < 0)
    {
        //TODO
    }
    else
    {
        QString name, key, cmd;
        name = ui->tabHotKey_lstUserActions->item(row,0)->text();
        key = ui->tabHotKey_lstUserActions->item(row,1)->text();
        cmd = ui->tabHotKey_lstUserActions->item(row,2)->text();
        kXneurApp::addUserAction *frmUsrAction = new kXneurApp::addUserAction(name, key, cmd);
        if(frmUsrAction->exec() ==QDialog::Accepted)
        {
            ui->tabHotKey_lstUserActions->item(row,0)->setText(frmUsrAction->name);
            ui->tabHotKey_lstUserActions->item(row,1)->setText(frmUsrAction->hot_key);
            ui->tabHotKey_lstUserActions->item(row,2)->setText(frmUsrAction->command);
        }
        delete frmUsrAction;
    }
}

void kXneurApp::frmSettings::clearHotKey()
{
    int row = ui->tabHotKey_lstHotKey->currentRow();
    ui->tabHotKey_lstHotKey->item(row, 1)->setText("");
}

void kXneurApp::frmSettings::auto_add_app_list_disable_autocompletion()
{
    add_Application_to_Widget(ui->tabAutocompletion_lstApp);
}

void kXneurApp::frmSettings::auto_del_app_list_disable_autocompletion()
{
    del_Application_to_Widget(ui->tabAutocompletion_lstApp);
}

void kXneurApp::frmSettings::app_add_app_ignore_list()
{
    add_Application_to_Widget(ui->taApplication_lstAppNotUsed);
}

void kXneurApp::frmSettings::app_del_app_ignore_list()
{
    del_Application_to_Widget(ui->taApplication_lstAppNotUsed);
}

void kXneurApp::frmSettings::app_add_app_auto_mode_list()
{
    add_Application_to_Widget(ui->taApplication_lstAppAutoMode);
}

void kXneurApp::frmSettings::app_del_app_auto_mode_list()
{
    del_Application_to_Widget(ui->taApplication_lstAppAutoMode);
}

void kXneurApp::frmSettings::app_add_app_manual_mode_list()
{
    add_Application_to_Widget(ui->taApplication_lstAppManualMode);
}

void kXneurApp::frmSettings::app_del_app_manual_mode_list()
{
    del_Application_to_Widget(ui->taApplication_lstAppManualMode);
}

void kXneurApp::frmSettings::notif_dublClicked(int row, int colum)
{
    QTableWidget *tmpWid = (QTableWidget *)sender();
    if(tmpWid->objectName()=="sound")
    {
        if(colum==1)
        {
            QString str = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Audio ( *.wav)"));
            if (!str.isEmpty())
            {
                tmpWid->item(row,colum)->setText(str);
                tmpWid->item(row,colum+1)->setCheckState(Qt::Checked);
            }
        }
    }
    else
    {
        if( colum==1)
        {
        tmpWid->setEditTriggers(QAbstractItemView::AllEditTriggers);
        }
    }
}

QMap<QString, QMultiMap<bool, QString> > kXneurApp::frmSettings::get_lget_from_notif_widget(QTableWidget *wid)
{
    QMap<QString, QMultiMap<bool, QString> > mapList;
    QMultiMap<bool, QString> mapTmp;
    for (int i=0; i< wid->rowCount();++i)
    {
        if (wid->item(i,2)->checkState()==Qt::Unchecked)
            {mapTmp.insert(false, wid->item(i,1)->text());}
        else
            {mapTmp.insert(true, wid->item(i,1)->text());}
        mapList.insert(wid->item(i,0)->text(),mapTmp);
        mapTmp.clear();
    }
    return mapList;
}

void kXneurApp::frmSettings::addApp_tabAdvanced()
{
    add_Application_to_Widget(ui->tabAdvanced_tabAppList);
}

void kXneurApp::frmSettings::removeApp_tabAdvanced()
{
    del_Application_to_Widget(ui->tabAdvanced_tabAppList);
}
