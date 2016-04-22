/***************************************************************************
 *   Copyright (C) 2007 by Vadim Likhota   *
 *   vadim-lvv{a}yandex.ru   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdlib.h>
#include <qwidget.h>
#include <kuniqueapplication.h>
#include <kinputdialog.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <qhbox.h>
#include <qgrid.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <kkeydialog.h>
#include <kglobalaccel.h>
#include <kshortcut.h>
#include <qlabel.h>
#include <qfile.h>
#include <qwhatsthis.h>
#include <klocale.h>
extern "C" {
#include <xneur/list_char.h>
}
#include "kxnlistbox.h"
#include "kxncombobox.h"
#include "kxneur.h"
#include "kxneursettings.h"
#include "kxneurconf.h"


extern "C" {
static char *lang_name_list[9] = {NULL, "English", "Russian", "Ukrainian", "Belarusian", "French", "Romanian", "Kazakh", "German"};
static char *lang_code_list[9] = {NULL, "en",      "ru",      "uk",        "be",         "fr",     "ro"      , "kz",     "de"};
}



KXNPushButton::KXNPushButton(const QString & text, QWidget * parent, const char * name)
 : QPushButton(text, parent, name)
{
}


KXNPushButton::~KXNPushButton()
{
}


void KXNPushButton::update(int nn) {
    lang = lang_code_list[nn];
    if ( nn )
	setEnabled(true);
    else
	setEnabled(false);
}


void KXNPushButton::click1() {
    if ( lang ) {
	KXNeurApp *knapp = (KXNeurApp *)KApplication::kApplication();
	// emit set_file(knapp->xnconf->get_file_path_name(lang, file)); get_dict_path
	emit set_file(knapp->xnconf->get_dict_path(lang, file), knapp->xnconf->get_home_dict_path(lang, file));
    }
}


KXNeurPage::KXNeurPage( QWidget* parent, const char* name, WFlags fl )
 : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "KXNeurPage" );

    vlayout = new QVBoxLayout(this, 10);
    run_xneur = new QCheckBox(i18n("Run NXeur with start KXNeur"), this);
    vlayout->addWidget(run_xneur);
    // QWhatsThis::add(run_xneur, i18n(""));

    force_run = new QCheckBox(i18n("Start XNeur with '--force' option"), this);
    vlayout->addWidget(force_run);
    QWhatsThis::add(force_run, i18n("With this option xneur don't check other xneur copies in ram"));

    autostart = new QCheckBox(i18n("Autostart"), this);
    vlayout->addWidget(autostart);
    QWhatsThis::add(autostart, i18n("Autostart of the program when entering the session. If recovery mode was enabled in KDE and KXNeur is running when exiting from this session, then KXneur will run automatically regardless of  autostart setting."));

    in_tray = new QComboBox(this);
    in_tray->setEditable(false);
    in_tray->insertItem(i18n("show flag in tray"));
    in_tray->insertItem(i18n("show lang in tray"));
    vlayout->addWidget(in_tray);
    QWhatsThis::add(in_tray, i18n("The style of icon reflected in the tray"));

    sw_mode = new QCheckBox(i18n("Switcher mode"), this);
    vlayout->addWidget(sw_mode);
    QWhatsThis::add(sw_mode, i18n("This mode  switches between two languages used the last. For select anothey language need pointed it. For selecting the third language one needs to click it in the program menu. The switcher mode makes sense if one uses three or more languages."));

    vlayout->setAlignment( Qt::AlignTop );
}

KXNeurPage::~KXNeurPage()
{
}

XNeurPage::XNeurPage( QWidget* parent, const char* name, WFlags fl )
 : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "XNeurPage" );

    vlayout = new QVBoxLayout(this);
    vlayout->setSpacing(5);

    hbox = new QHBox(this);
    new QLabel(i18n("Set Mode on Start"), hbox);
    xneur_mode = new QComboBox(hbox);
    xneur_mode->setEditable(false);
    xneur_mode->insertItem(i18n("Automatic"));
    xneur_mode->insertItem(i18n("Manual"));
    vlayout->addWidget(hbox);
    QWhatsThis::add(hbox, i18n("The mode of program working set with its start"));

    hbox2 = new QHBox(this);
    new QLabel(i18n("Set Default Language for programs"), hbox2);
    default_group = new QComboBox(hbox2);
    default_group->setEditable(false);
    vlayout->addWidget(hbox2);
    QWhatsThis::add(hbox, i18n("Define initial keyboard layout for all new applications"));

    group1 = new QGroupBox(i18n("Layouts"), this);
    glayout = new QGridLayout(group1, MAX_LANGUAGES+1, 4, 15, 5);
    glayout->addWidget(new QLabel(i18n(" Language Name "), group1), 0, 0);
    glayout->addWidget(new QLabel(i18n(" Number "), group1), 0, 1);

    for ( int i = 0; i < MAX_LANGUAGES ; i++ ) {
	lang[i] = new QComboBox(group1);
	lang[i]->setEditable(false);
	lang[i]->insertItem(i18n(" "));
	lang[i]->insertItem(i18n("English"));
	lang[i]->insertItem(i18n("Russian"));
	lang[i]->insertItem(i18n("Ukrainian"));
	lang[i]->insertItem(i18n("Belarusian"));
	lang[i]->insertItem(i18n("French"));
	lang[i]->insertItem(i18n("Romanian"));
	lang[i]->insertItem(i18n("Kazakh"));
	lang[i]->insertItem(i18n("German"));
	glayout->addWidget(lang[i], i+1, 0);
	num[i] = new KXNComboBox(group1);
	num[i]->setEditable(false);
	num[i]->insertItem("0");
	num[i]->insertItem("1");
	num[i]->insertItem("2");
	num[i]->insertItem("3");
	num[i]->setEnabled(false);
	glayout->addWidget(num[i], i+1, 1);
	QWhatsThis::add(num[i], i18n("A number of the language group. NB! The numbers must be unique."));
	dict[i] = new KXNPushButton(i18n("Dictionary..."), group1);
	dict[i]->lang = lang_code_list[i];
	dict[i]->file = (char *)"dict";
	connect(dict[i], SIGNAL(clicked()), dict[i], SLOT(click1()));
	connect(dict[i], SIGNAL(set_file(char *, char *)), this, SLOT(open_file(char *, char *)));
	dict[i]->setEnabled(false);
	glayout->addWidget(dict[i], i+1, 2);
	QWhatsThis::add(dict[i], i18n("The dictionaries for identification of certain language words. The words must be typed in Latine layout. For example, Russian 'tree' must be typed as 'lthtdj'."));
	regexp[i] = new KXNPushButton(i18n("Regular Expressions..."), group1);
	regexp[i]->lang = lang_code_list[i];
	regexp[i]->file = (char *)"regexp";
	connect(regexp[i], SIGNAL(clicked()), regexp[i], SLOT(click1()));
	connect(regexp[i], SIGNAL(set_file(char *, char *)), this, SLOT(open_file(char *, char *)));
	regexp[i]->setEnabled(false);
	glayout->addWidget(regexp[i], i+1, 3);
	QWhatsThis::add(regexp[i], i18n("To set a word in the form of regular expression"));
	connect(lang[i], SIGNAL(activated(int)), num[i], SLOT(enable(int)));
	connect(lang[i], SIGNAL(activated(int)), regexp[i], SLOT(update(int)));
	connect(lang[i], SIGNAL(activated(int)), dict[i], SLOT(update(int)));
    }

    vlayout->addWidget(group1);
    QWhatsThis::add(group1, i18n("The block of layouts allow to set  languages and words for their processing by the program."));

    group2 =  new QGroupBox(i18n("Additional"), this);
    vlayout2 = new QVBoxLayout(group2, 15, 5);

    mouse_mode = new QCheckBox(i18n("Processing Mouse Events"), group2);
    vlayout2->addWidget(mouse_mode);
    QWhatsThis::add(mouse_mode, i18n("If activated this mode considers not only Space, Tab, Enter etc. to be the end of a word, but also mouse clicking to another field."));
    edu_mode = new QCheckBox(i18n("Enable Education Mode"), group2);
    vlayout2->addWidget(edu_mode);
    QWhatsThis::add(edu_mode, i18n("All the returned (incorrect) switchings will be automatically added to the dictionary."));
    remem_win = new QCheckBox(i18n("Remember Layoute For Each Window"), group2);
    vlayout2->addWidget(remem_win);
    QWhatsThis::add(remem_win, i18n("Program will save a language for each window."));
    save_sel_text = new QCheckBox(i18n("Save Selection After Change Selected Text"), group2);
    vlayout2->addWidget(save_sel_text);
    // QWhatsThis::add(save_sel_text, i18n(""));

    vlayout->addWidget(group2);
    vlayout->setAlignment( Qt::AlignTop );
}

XNeurPage::~XNeurPage()
{
}


void XNeurPage::open_file(char * path, char * home_path)
{
    bool ok;
    QString p(path), home_p(home_path), txt;
    QFile fl(p), home_fl(home_p);
    if ( fl.open(IO_ReadOnly) ) {
	QTextStream ts( &fl );
	txt = ts.read();
        fl.close();
	QString text = KInputDialog::getMultiLineText( p, NULL, txt, &ok, this );
	if ( ok ) {
	    // printf("press ok\n");
	    if ( home_fl.open(IO_WriteOnly) ) {
		QTextStream ts2( &home_fl );
		ts2 << text;
		home_fl.close();
	    }
	    else
		printf("can't open for write %s\n", home_path);
	}
	/* else
	    printf("calcel\n");*/
    }
    else
	printf("can't open for read %s\n", path);
}


KeysPage::KeysPage( QWidget* parent, const char* name, WFlags fl )
 : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "KeysPage" );

    vlayout = new QVBoxLayout(this);
    vlayout->setSpacing(5);
/*
    grid = new QGrid(3, this);
    grid->setSpacing(5);
    grid->setMargin(15);
    grid->setFrameShape(QFrame::GroupBoxPanel);

    new QLabel(i18n("Key Bindings "), grid);
    new QLabel(i18n("Modifier "), grid);
    new QLabel(i18n("Keys "), grid);

    for ( int i = 0 ; i < MAX_HOTKEYS ; i++ ) {
	switch ( i ) {
	    case 0 : new QLabel(i18n("Replace / Cancel Replace "), grid); break;
	    case 1 : new QLabel(i18n("Replace Last String "), grid); break;
	    case 2 : new QLabel(i18n("Change Processing Mode "), grid); break;
	    case 3 : new QLabel(i18n("Replace Selected Text "), grid); break;
	    case 4 : new QLabel(i18n("Transliterate Selected Text "), grid); break;
	    case 5 : new QLabel(i18n("Change Case Selected Text "), grid); break;
	}

	mod[i] = new QComboBox(grid);
	mod[i]->setEditable(false);
	mod[i]->insertItem(i18n("None"));
	mod[i]->insertItem(i18n("Shift"));
	mod[i]->insertItem(i18n("Control"));
	mod[i]->insertItem(i18n("Alt"));

	key[i] = new QComboBox(grid);
	key[i]->setEditable(false);
	key[i]->insertItem(i18n("None"));
	key[i]->insertItem(i18n("Break"));
	key[i]->insertItem(i18n("Scroll Lock"));
	key[i]->insertItem(i18n("Prt Scr"));
    }

    vlayout->addWidget(grid);*/
    vlayout->setAlignment( Qt::AlignTop );
}

KeysPage::~KeysPage()
{
}


ProgPage::ProgPage( QWidget* parent, const char* name, WFlags fl )
 : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "ProgPage" );

    vlayout = new QVBoxLayout(this);
    vlayout->setSpacing(5);
    for ( int i = 0 ; i < 3 ; i++ ) {
	switch ( i ) {
	    case 0 : group[0] = new QGroupBox(i18n("Excluded List"), this); break;
	    case 1 : group[1] = new QGroupBox(i18n("Force Automatic Processing List"), this); break;
	    case 2 : group[2] = new QGroupBox(i18n("Force Manual Processing List"), this); break;
	}
	glayout[i] = new QGridLayout(group[i], 5, 3, 15, 5);
	list[i] = new KXNListBox(group[i]);
	glayout[i]->addMultiCellWidget(list[i], 0, 4, 0, 1);
	QWhatsThis::add(list[i], i18n("List of the programs or names of MainWindow of these programs that coincide in most cases."));
	addprg[i] = new QPushButton(i18n("Add program from list..."), group[i]);
	QObject::connect(addprg[i], SIGNAL(clicked()), list[i], SLOT( getProgList() ));
	glayout[i]->addWidget(addprg[i], 0, 2);
	QWhatsThis::add(addprg[i], i18n("List of the running graphical programs or the names of their  MainWindow (for several programs, for example sim, they do not coincide). If You do not see in the list  your program use the button 'Point the window' below. The list of the programs was received  by a query of child windows of the root-window by means of xwininfo from x11-tools."));
	add2prg[i] = new QPushButton(i18n("Point window..."), group[i]);
	QObject::connect(add2prg[i], SIGNAL(clicked()), list[i], SLOT( getPointedProg() ));
	glayout[i]->addWidget(add2prg[i], 1, 2);
	QWhatsThis::add(add2prg[i], i18n("After pressing the button 'Point the button' one should make mouse click on a program window that must be added to the list. It is possible one will have to point out several windows for some sdi programs that have independant windows. Xprop from X11 is used for receiving the information of pointed window."));
	delprg[i] = new QPushButton(i18n("Remove"), group[i]);
	QObject::connect(delprg[i], SIGNAL(clicked()), list[i], SLOT(deleteSelected()));
	glayout[i]->addWidget(delprg[i], 2, 2);
	glayout[i]->addWidget(new QLabel(" ", group[i]), 3, 2);

	vlayout->addWidget(group[i]);
    }
}

ProgPage::~ProgPage()
{
}



KXNLineEdit::KXNLineEdit(QWidget * parent, const char * name)
 : QLineEdit(parent, name)
{
    // par = parent;
}

KXNLineEdit::~KXNLineEdit()
{
}


void KXNLineEdit::openDlg()
{
    QString str = KFileDialog::getOpenFileName();
    if ( str.length() > 0 )
        setText(str);
}




SndPage::SndPage( QWidget* parent, const char* name, WFlags fl )
 : QWidget( parent, name, fl )
{
    QPixmap pic( kapp->iconLoader()->loadIcon("fileopen", KIcon::Small)); // ??? may be "folder_sound_3"

    if ( !name )
	setName( "SndPage" );

    vlayout = new QVBoxLayout(this);
    vlayout->setSpacing(5);

    enable_snd = new QCheckBox(i18n("Enable sounds on XNeur actions"), this);
    vlayout->addWidget(enable_snd);

    grid = new QGrid(3, this);
    grid->setSpacing(3);
    grid->setMargin(5);
    grid->setFrameShape(QFrame::GroupBoxPanel);



    for ( int i = 0 ; i < 14 ; i++ ) {
        switch ( i ) {
	    case  0 : new QLabel(i18n("Press Key On Layout 1 "), grid); break;
	    case  1 : new QLabel(i18n("Press Key On Layout 2 "), grid); break;
	    case  2 : new QLabel(i18n("Press Key On Layout 3 "), grid); break;
	    case  3 : new QLabel(i18n("Press Key On Layout 4 "), grid); break;
	    case  4 : new QLabel(i18n("Enable Layout 1 "), grid); break;
	    case  5 : new QLabel(i18n("Enable Layout 2 "), grid); break;
	    case  6 : new QLabel(i18n("Enable Layout 3 "), grid); break;
	    case  7 : new QLabel(i18n("Enable Layout 4 "), grid); break;
	    case  8 : new QLabel(i18n("Automatic Change Word "), grid); break;
	    case  9 : new QLabel(i18n("Manual Change Word "), grid); break;
	    case 10 : new QLabel(i18n("Change String "), grid); break;
	    case 11 : new QLabel(i18n("Change Selected Text "), grid); break;
	    case 12 : new QLabel(i18n("Translit Selected Text "), grid); break;
	    case 13 : new QLabel(i18n("Changecase Selected Text "), grid); break;
	}
	edit[i] = new KXNLineEdit(grid);
	btn[i] = new QPushButton(grid);
	btn[i]->setPixmap(pic);
	connect(btn[i], SIGNAL(clicked()), edit[i], SLOT(openDlg()));
    }

    vlayout->addWidget(grid);
    QWhatsThis::add(grid, i18n("List of sounds"));

    vlayout->setAlignment( Qt::AlignTop );
}

SndPage::~SndPage()
{
}


KXNeurConf::KXNeurConf(KXNeurApp *app, QWidget *parent)
 : KConfigDialog(parent, i18n("Settings"), KXNeurSettings::self(), KDialogBase::IconList, Ok|/*Apply|*/Cancel|Help, Ok, true), knapp(app)
{
    kxneur_page = new KXNeurPage(0, "KXNeurPage");
    xneur_page = new XNeurPage(0, "XNeurPage");
    keys_page = new KeysPage(0, "KeysPage");
    prog_page = new ProgPage(0, "ProgPage");
    snd_page = new SndPage(0, "SndPage");

    addPage( kxneur_page, i18n("KXNeur"), "keyboard_layout", i18n("KXNeur Options") ); // or pic = locale
    addPage( xneur_page, i18n("XNeur"), "embedjs", i18n("XNeur Daemon Options") );     // or pic = exec
    addPage( keys_page, i18n("Keys"), "key_bindings", i18n("Key Combinations") );
    addPage( prog_page, i18n("Programs"), "kwin", i18n("Exclusions for Program") );
    addPage( snd_page, i18n("Sounds"), "kmix", i18n("Sounds for Events") );

    LoadSettings();
}


KXNeurConf::~KXNeurConf()
{
}

void KXNeurConf::LoadSettings()
{
    knapp->xnconf_reload();


    kxneur_page->run_xneur->setChecked( KXNeurSettings::RunXNeur() );
    kxneur_page->force_run->setChecked( KXNeurSettings::ForceRun() );
    kxneur_page->autostart->setChecked( KXNeurSettings::Autostart() );
    kxneur_page->in_tray->setCurrentItem( KXNeurSettings::ShowInTray() );
    kxneur_page->sw_mode->setChecked( KXNeurSettings::SwitcherMode() );


    if ( knapp->xnconf->get_current_mode(knapp->xnconf) == AUTO_MODE )
	xneur_page->xneur_mode->setCurrentItem(0);
    else
	xneur_page->xneur_mode->setCurrentItem(1);

    if ( knapp->xnconf->mouse_processing_mode == MOUSE_GRAB_DISABLE )
	xneur_page->mouse_mode->setChecked(false);
    else
	xneur_page->mouse_mode->setChecked(true);

    if ( knapp->xnconf->education_mode == EDUCATION_MODE_DISABLE )
	xneur_page->edu_mode->setChecked(false);
    else
	xneur_page->edu_mode->setChecked(true);

    if ( knapp->xnconf->layout_remember_mode == LAYOUTE_REMEMBER_DISABLE )
	xneur_page->remem_win->setChecked(false);
    else
	xneur_page->remem_win->setChecked(true);

    if ( knapp->xnconf->save_selection_mode == SELECTION_SAVE_DISABLED )
	xneur_page->save_sel_text->setChecked(false);
    else
	xneur_page->save_sel_text->setChecked(true);


    for ( int i = 0 ; i < knapp->xnconf->total_languages && i < MAX_LANGUAGES ; i++ ) {
	if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "English") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(1);
	    xneur_page->regexp[i]->lang = lang_code_list[1];
	    xneur_page->dict[i]->lang = lang_code_list[1];
	    xneur_page->default_group->insertItem(i18n("English"));
	}
	else if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "Russian") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(2);
	    xneur_page->regexp[i]->lang = lang_code_list[2];
	    xneur_page->dict[i]->lang = lang_code_list[2];
	    xneur_page->default_group->insertItem(i18n("Russian"));
	}
	else if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "Ukrainian") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(3);
	    xneur_page->regexp[i]->lang = lang_code_list[3];
	    xneur_page->dict[i]->lang = lang_code_list[3];
	    xneur_page->default_group->insertItem(i18n("Ukrainian"));
	}
	else if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "Belarusian") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(4);
	    xneur_page->regexp[i]->lang = lang_code_list[4];
	    xneur_page->dict[i]->lang = lang_code_list[4];
	    xneur_page->default_group->insertItem(i18n("Belarusian"));
	}
	else if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "French") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(5);
	    xneur_page->regexp[i]->lang = lang_code_list[5];
	    xneur_page->dict[i]->lang = lang_code_list[5];
	    xneur_page->default_group->insertItem(i18n("French"));
	}
	else if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "Romanian") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(6);
	    xneur_page->regexp[i]->lang = lang_code_list[6];
	    xneur_page->dict[i]->lang = lang_code_list[6];
	    xneur_page->default_group->insertItem(i18n("Romanian"));
	}
	else if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "Kazakh") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(7);
	    xneur_page->regexp[i]->lang = lang_code_list[7];
	    xneur_page->dict[i]->lang = lang_code_list[7];
	    xneur_page->default_group->insertItem(i18n("Kazakh"));
	}
	else if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "German") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(7);
	    xneur_page->regexp[i]->lang = lang_code_list[7];
	    xneur_page->dict[i]->lang = lang_code_list[7];
	    xneur_page->default_group->insertItem(i18n("German"));
	}
	else
	     xneur_page->default_group->insertItem("group "+QString::number(i));
	xneur_page->num[i]->setCurrentItem(knapp->xnconf->get_lang_group(knapp->xnconf, i));
	xneur_page->num[i]->setEnabled(true);
	xneur_page->regexp[i]->setEnabled(true);
	xneur_page->dict[i]->setEnabled(true);
    }

    xneur_page->default_group->setCurrentItem(knapp->xnconf->default_group);

    keys = new KGlobalAccel(NULL);
    keys->insert("Replace / Cancel Replace ",    i18n("Replace / Cancel Replace "),    QString::null, KeyFromXNConf(0), 0, this, NULL);
    keys->insert("Replace Last String ",         i18n("Replace Last String "),         QString::null, KeyFromXNConf(1), 0, this, NULL);
    keys->insert("Change Processing Mode ",      i18n("Change Processing Mode "),      QString::null, KeyFromXNConf(2), 0, this, NULL);
    keys->insert("Replace Selected Text ",       i18n("Replace Selected Text "),       QString::null, KeyFromXNConf(3), 0, this, NULL);
    keys->insert("Transliterate Selected Text ", i18n("Transliterate Selected Text "), QString::null, KeyFromXNConf(4), 0, this, NULL);
    keys->insert("Change Case Selected Text ",   i18n("Change Case Selected Text "),   QString::null, KeyFromXNConf(5), 0, this, NULL);
    keys->insert("Enable First Layout ",         i18n("Enable First Layout "),         QString::null, KeyFromXNConf(6), 0, this, NULL);
    keys->insert("Enable Secondary Layout ",     i18n("Enable Secondary Layout "),     QString::null, KeyFromXNConf(7), 0, this, NULL);
    keys->insert("Enable Thrid Layout ",         i18n("Enable Thrid Layout "),         QString::null, KeyFromXNConf(8), 0, this, NULL);
    keys->insert("Enable Fourth Layout ",        i18n("Enable Fourth Layout "),        QString::null, KeyFromXNConf(9), 0, this, NULL);

    keys_page->keyChooser = new KKeyChooser(keys, keys_page);
    keys_page->vlayout->addWidget(keys_page->keyChooser);


    for (int i = 0 ; i < knapp->xnconf->excluded_apps->data_count ; i++ )
	prog_page->list[0]->insertItem(knapp->xnconf->excluded_apps->data[i].string);
    for (int i = 0 ; i < knapp->xnconf->auto_apps->data_count ; i++ )
	prog_page->list[1]->insertItem(knapp->xnconf->auto_apps->data[i].string);
    for (int i = 0 ; i < knapp->xnconf->manual_apps->data_count ; i++ )
	prog_page->list[2]->insertItem(knapp->xnconf->manual_apps->data[i].string);

    snd_page->enable_snd->setChecked( knapp->xnconf->sound_mode );

    for (int i = 0 ; i < 14 ; i++)
	snd_page->edit[i]->setText(knapp->xnconf->sounds[i]);

}

void KXNeurConf::SaveSettings()
{

    KXNeurSettings::setRunXNeur( kxneur_page->run_xneur->isChecked() );
    KXNeurSettings::setForceRun( kxneur_page->force_run->isChecked() );
    KXNeurSettings::setAutostart( kxneur_page->autostart->isChecked() );
    KXNeurSettings::setShowInTray( kxneur_page->in_tray->currentItem() );
    KXNeurSettings::setSwitcherMode( kxneur_page->sw_mode->isChecked() );


    if ( xneur_page->xneur_mode->currentItem() )
	knapp->xnconf->set_current_mode(knapp->xnconf, MANUAL_MODE);
    else
	knapp->xnconf->set_current_mode(knapp->xnconf, AUTO_MODE);

    if ( xneur_page->mouse_mode->isChecked() )
	knapp->xnconf->mouse_processing_mode = MOUSE_GRAB_ENABLE;
    else
	knapp->xnconf->mouse_processing_mode = MOUSE_GRAB_DISABLE;

    if ( xneur_page->edu_mode->isChecked() )
	knapp->xnconf->education_mode = EDUCATION_MODE_ENABLE;
    else
	knapp->xnconf->education_mode = EDUCATION_MODE_DISABLE;

    if ( xneur_page->remem_win->isChecked() )
	knapp->xnconf->layout_remember_mode = LAYOUTE_REMEMBER_ENABLE;
    else
	knapp->xnconf->layout_remember_mode = LAYOUTE_REMEMBER_DISABLE;

    if ( xneur_page->save_sel_text->isChecked() )
	knapp->xnconf->save_selection_mode = SELECTION_SAVE_ENABLED;
    else
	knapp->xnconf->save_selection_mode = SELECTION_SAVE_DISABLED;

    knapp->xnconf->default_group = xneur_page->default_group->currentItem();

    int total_lang = 0;
    char **lang_name = (char **)malloc(4 * sizeof(char*));
    char **lang_dir = (char **)malloc(4 * sizeof(char*));
    int *lang_group = (int *)malloc(4 * sizeof(int));
    for ( int i = 0 ; i < MAX_LANGUAGES ; i++ )
	if ( xneur_page->lang[i]->currentItem() ) {
	    lang_name[total_lang] = lang_name_list[ xneur_page->lang[i]->currentItem() ];
	    lang_dir[total_lang] = lang_code_list[ xneur_page->lang[i]->currentItem() ];
	    lang_group[total_lang] = xneur_page->num[i]->currentItem();
	    total_lang++;
	}
    knapp->xnconf->set_languages(knapp->xnconf, lang_name, lang_dir, lang_group, total_lang);

    keys_page->keyChooser->commitChanges();

    // printf("%s - %x\n", keys->shortcut("Replace / Cancel Replace ").toString().latin1(), keys->shortcut("Replace / Cancel Replace ").keyCodeQt() );
    // ToXNConf(0, keys->shortcut("Replace / Cancel Replace ").toString());
    KeyToXNConf(0, keys->shortcut("Replace / Cancel Replace "   ).keyCodeQt());
    // printf("%s - %x\n", keys->shortcut("Replace Last String ").toString().latin1(), keys->shortcut("Replace Last String ").keyCodeQt() );
    KeyToXNConf(1, keys->shortcut("Replace Last String "        ).keyCodeQt());
    // printf("%s - %x\n", keys->shortcut("Change Processing Mode ").toString().latin1(), keys->shortcut("Change Processing Mode ").keyCodeQt() );
    KeyToXNConf(2, keys->shortcut("Change Processing Mode "     ).keyCodeQt());
    KeyToXNConf(3, keys->shortcut("Replace Selected Text "      ).keyCodeQt());
    KeyToXNConf(4, keys->shortcut("Transliterate Selected Text ").keyCodeQt());
    KeyToXNConf(5, keys->shortcut("Change Case Selected Text "  ).keyCodeQt());
    KeyToXNConf(6, keys->shortcut("Enable First Layout "        ).keyCodeQt());
    KeyToXNConf(7, keys->shortcut("Enable Secondary Layout "    ).keyCodeQt());
    KeyToXNConf(8, keys->shortcut("Enable Thrid Layout "        ).keyCodeQt());
    KeyToXNConf(9, keys->shortcut("Enable Fourth Layout "       ).keyCodeQt());

    knapp->xnconf->excluded_apps->uninit(knapp->xnconf->excluded_apps);
    knapp->xnconf->excluded_apps = list_char_init();
    for ( int i = 0 ; i < prog_page->list[0]->numRows() ; i++ )
	knapp->xnconf->excluded_apps->add(knapp->xnconf->excluded_apps, prog_page->list[0]->text(i).latin1());

    knapp->xnconf->auto_apps->uninit(knapp->xnconf->auto_apps);
    knapp->xnconf->auto_apps = list_char_init();
    for ( int i = 0 ; i < prog_page->list[1]->numRows() ; i++ )
	knapp->xnconf->auto_apps->add(knapp->xnconf->auto_apps, prog_page->list[1]->text(i).latin1());

    knapp->xnconf->manual_apps->uninit(knapp->xnconf->manual_apps);
    knapp->xnconf->manual_apps = list_char_init();
    for ( int i = 0 ; i < prog_page->list[2]->numRows() ; i++ )
	knapp->xnconf->manual_apps->add(knapp->xnconf->manual_apps, prog_page->list[2]->text(i).latin1());

    if ( snd_page->enable_snd->isChecked() )
	knapp->xnconf->sound_mode = SOUND_ENABLED;
    else
	knapp->xnconf->sound_mode = SOUND_DISABLED;

    for ( int i = 0 ; i < 14 ; i++ ) {
	free(knapp->xnconf->sounds[i]);
	knapp->xnconf->sounds[i] = (char *)malloc( snd_page->edit[i]->maxLength() * sizeof(char) );
	strcpy(knapp->xnconf->sounds[i], snd_page->edit[i]->text().utf8() );
    }

    knapp->xnconf->save(knapp->xnconf);

    if ( knapp->xneur_stop() )
	knapp->xneur_start();
    else
	printf("without restart\n");

    KXNeurSettings::writeConfig();
}

void KXNeurConf::slotApply()
{
    SaveSettings();
    KConfigDialog::slotApply();
}

void KXNeurConf::slotOk()
{
    SaveSettings();
    delete keys;
    KConfigDialog::slotOk();
}

int KXNeurConf::KeyFromXNConf(int action)
{
    int shc = 0;

    if ( knapp->xnconf->hotkeys[action].modifier1 )
	shc += Qt::SHIFT;
    if ( knapp->xnconf->hotkeys[action].modifier2 )
	shc += Qt::CTRL;
    if ( knapp->xnconf->hotkeys[action].modifier3 )
	shc += Qt::ALT;
    if ( knapp->xnconf->hotkeys[action].modifier4 )
	shc += KKey::QtWIN;
    int ccc = kc.key2code(knapp->xnconf->hotkeys[action].key);
    // printf("%s == %x\n", knapp->xnconf->hotkeys[action].key, ccc);
    shc += ccc;
    // printf("%x\n", shc);
    return shc;
}

// void KXNeurConf::ToXNConf(int action, QString s)
void KXNeurConf::KeyToXNConf(int a, int c)
{
    int action = a, cod = c;

    if ( (cod & 0xffff) == 0 ) {
	printf("not found key for action %d: key code = %x;  key will not change\n", action, cod);
	// fflush(stdout);
	return;
    }

    if ( knapp->xnconf->hotkeys[action].modifier1 ) {
	free(knapp->xnconf->hotkeys[action].modifier1);
	knapp->xnconf->hotkeys[action].modifier1 = NULL;
    }
    if ( knapp->xnconf->hotkeys[action].modifier2 ) {
	free(knapp->xnconf->hotkeys[action].modifier2);
    	knapp->xnconf->hotkeys[action].modifier2 = NULL;
    }
    if ( knapp->xnconf->hotkeys[action].modifier3 ) {
	free(knapp->xnconf->hotkeys[action].modifier3);
	knapp->xnconf->hotkeys[action].modifier3 = NULL;
    }
    if ( knapp->xnconf->hotkeys[action].modifier4 ) {
	free(knapp->xnconf->hotkeys[action].modifier4);
        knapp->xnconf->hotkeys[action].modifier4 = NULL;
    }
    if ( knapp->xnconf->hotkeys[action].key ) {
    	free(knapp->xnconf->hotkeys[action].key);
	knapp->xnconf->hotkeys[action].key = NULL;
    }
/*
    QStringList sl = QStringList::split( '+', s );

    QStringList::Iterator it = sl.begin();

    for ( ; it != sl.end(); ++it ) {
	// printf("%s\n", (*it).latin1());
	if ( *it == "Shift" )
	    knapp->xnconf->hotkeys[action].modifier1 = (char *)"Shift";
	else if ( *it == "Control" )
	    knapp->xnconf->hotkeys[action].modifier2 = (char *)"Control";
	else if ( *it == "Alt" )
	    knapp->xnconf->hotkeys[action].modifier3 = (char *)"Alt";
	else if ( *it == "Win" )
	    knapp->xnconf->hotkeys[action].modifier4 = (char *)"Super";
	else {
	    knapp->xnconf->hotkeys[action].key = (char *)malloc( sizeof( (*it).latin1() ) );
	    strcpy(knapp->xnconf->hotkeys[action].key, (*it).latin1());
	}
    }*/

    if ( cod & Qt::SHIFT ) {
	knapp->xnconf->hotkeys[action].modifier1 = (char *)malloc( 8 * sizeof(char) );
	strcpy(knapp->xnconf->hotkeys[action].modifier1, (char *)"Shift");
    }
    if ( cod & Qt::CTRL ) {
	knapp->xnconf->hotkeys[action].modifier2 = (char *)malloc( 8 * sizeof(char) );
	strcpy(knapp->xnconf->hotkeys[action].modifier2, (char *)"Control");
    }
    if ( cod & Qt::ALT ) {
	knapp->xnconf->hotkeys[action].modifier3 = (char *)malloc( 8 * sizeof(char) );
	strcpy(knapp->xnconf->hotkeys[action].modifier3, (char *)"Alt");
    }
    if ( cod & KKey::QtWIN ) {
	knapp->xnconf->hotkeys[action].modifier4 = (char *)malloc( 8 * sizeof(char) );
	strcpy(knapp->xnconf->hotkeys[action].modifier4, (char *)"Super");
    }
    cod &= 0x0000ffff;
    // if ( cod ) {
	QString cc = kc.code2key(cod);
	// printf("%x\n", cod );
	// printf("%s\n", cc.latin1());
	// fflush(stdout);
	knapp->xnconf->hotkeys[action].key = (char *)malloc( sizeof( cc.latin1() ) );
	strcpy(knapp->xnconf->hotkeys[action].key, cc.latin1());
    // }
}

/*
void KXNeurConf::SndFromXNConf(int sound)
{

}


void KXNeurConf::SndToXNConf(int sound)
{

}
*/






#include "kxneurconf.moc"

