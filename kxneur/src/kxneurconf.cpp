/**********************************************************************************
 *  KXNeur (KDE X Neural Switcher) is XNeur front-end for KDE ( http://xneur.ru ).*
 *  Copyright (C) 2007-2008  Vadim Likhota <vadim-lvv@yandex.ru>                  *
 *                                                                                *
 *  This program is free software; you can redistribute it and/or modify          *
 *  it under the terms of the GNU General Public License as published by          *
 *  the Free Software Foundation; either version 2 of the License, or             *
 *  (at your option) any later version.                                           *
 *                                                                                *
 *  This program is distributed in the hope that it will be useful,               *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
 *  GNU General Public License for more details.                                  *
 *                                                                                *
 *  You should have received a copy of the GNU General Public License             *
 *  along with this program; if not, write to the Free Software                   *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA    *
 **********************************************************************************/

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
#include <qspinbox.h>
#include <qfile.h>
#include <qwhatsthis.h>
#include <klocale.h>
extern "C" {
#include <xneur/list_char.h>
}
#include "kxnlistbox.h"
#include "kxncombobox.h"
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
	// emit set_file(knapp->xnconf->get_dict_path(lang, file));
	emit set_file(knapp->xnconf->get_global_dict_path(lang, file));
    }
}


KXNeurPage::KXNeurPage( QWidget* parent, const char* name, WFlags fl )
 : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "KXNeurPage" );

    vlayout = new QVBoxLayout(this, 1, 12);
    vlayout->setAlignment( Qt::AlignTop );

    group1 = new QGroupBox(i18n("KXNeur (front-end) options "), this);
    vlayout1 = new QVBoxLayout(group1, 16, 3);
    vlayout1->setAlignment( Qt::AlignTop );

    run_xneur = new QCheckBox(i18n("Run NXeur with start KXNeur"), group1);
    vlayout1->addWidget(run_xneur);
    // QWhatsThis::add(run_xneur, i18n(""));

    force_run = new QCheckBox(i18n("Start XNeur with '--force' option"), group1);
    vlayout1->addWidget(force_run);
    QWhatsThis::add(force_run, i18n("With this option xneur don't check other xneur copies in ram"));

    autostart = new QCheckBox(i18n("Autostart"), group1);
    vlayout1->addWidget(autostart);
    QWhatsThis::add(autostart, i18n("Autostart of the program when entering the session. If recovery mode was enabled in KDE and KXNeur is running when exiting from this session, then KXneur will run automatically regardless of  autostart setting."));

    sw_mode = new QCheckBox(i18n("Switcher mode"), group1);
    vlayout1->addWidget(sw_mode);
    QWhatsThis::add(sw_mode, i18n("This mode  switches between two languages used the last. For select anothey language need pointed it. For selecting the third language one needs to click it in the program menu. The switcher mode makes sense if one uses three or more languages."));

    in_tray = new QComboBox(group1);
    in_tray->setEditable(false);
    in_tray->insertItem(i18n("show flag in tray"));
    in_tray->insertItem(i18n("show lang in tray"));
    vlayout1->addWidget(in_tray);
    QWhatsThis::add(in_tray, i18n("The style of icon reflected in the tray"));

    vlayout->addWidget(group1);

    group2 = new QGroupBox(i18n("XNeur (daemon) options "), this);
    vlayout2 = new QVBoxLayout(group2, 16, 3);
    vlayout2->setAlignment( Qt::AlignTop );

    fix_caps_lock = new QCheckBox(i18n("Change iNCIDENTAL CapsLock"), group2);
    vlayout2->addWidget(fix_caps_lock);

    fix_two_big = new QCheckBox(i18n("Change two CApital letter"), group2);
    vlayout2->addWidget(fix_two_big);

    flush_buf_on_enter = new QCheckBox(i18n("Flush internal buffer when pressed Enter"), group2);
    vlayout2->addWidget(flush_buf_on_enter);

    not_work_on_enter = new QCheckBox(i18n("Don't correct word when pressed Enter"), group2);
    vlayout2->addWidget(not_work_on_enter);

    edu_mode = new QCheckBox(i18n("Enable Education Mode"), group2);
    vlayout2->addWidget(edu_mode);
    QWhatsThis::add(edu_mode, i18n("All the returned (incorrect) switchings will be automatically added to the dictionary."));

    mouse_mode = new QCheckBox(i18n("Processing Mouse Events"), group2);
    vlayout2->addWidget(mouse_mode);
    QWhatsThis::add(mouse_mode, i18n("If activated this mode considers not only Space, Tab, Enter etc. to be the end of a word, but also mouse clicking to another field."));

    save_sel_text = new QCheckBox(i18n("Save Selection After Change Selected Text"), group2);
    vlayout2->addWidget(save_sel_text);
    // QWhatsThis::add(save_sel_text, i18n(""));

    save_kbd_log = new QCheckBox(i18n("Save Keyboard Log"), group2);
    vlayout2->addWidget(save_kbd_log);

    box2 = new QHBox(group2);
    vlayout2->addWidget(box2);
    new QLabel(i18n("Set Mode on Start"), box2);
    xneur_mode = new QComboBox(box2);
    xneur_mode->setEditable(false);
    xneur_mode->insertItem(i18n("Automatic"));
    xneur_mode->insertItem(i18n("Manual"));
    QWhatsThis::add(box2, i18n("The mode of program working set with its start"));

    vlayout->addWidget(group2);
}

KXNeurPage::~KXNeurPage()
{
}

XNeurPage::XNeurPage( QWidget* parent, const char* name, WFlags fl )
 : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "XNeurPage" );

    vlayout = new QVBoxLayout(this, 2, 8);
    vlayout->setSpacing(5);

    grid = new QGrid(2, this);

    new QLabel(i18n("Set Default Language for programs"), grid);
    default_group = new QComboBox(grid);
    default_group->setEditable(false);
    QWhatsThis::add(default_group, i18n("Define initial keyboard layout for all new applications"));

    new QLabel(i18n("Delay before send event (in milliseconds)                "), grid);
    send_delay = new QSpinBox(0, 9999, 1, grid);
    vlayout->addWidget(grid);

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
	connect(dict[i], SIGNAL(set_file(char *)), this, SLOT(open_file(char *)));
	dict[i]->setEnabled(false);
	glayout->addWidget(dict[i], i+1, 2);
	QWhatsThis::add(dict[i], i18n("The dictionaries for identification of certain language words. The words must be typed in Latine layout. For example, Russian 'tree' must be typed as 'lthtdj'."));
	regexp[i] = new KXNPushButton(i18n("Regular Expressions..."), group1);
	regexp[i]->lang = lang_code_list[i];
	regexp[i]->file = (char *)"regexp";
	connect(regexp[i], SIGNAL(clicked()), regexp[i], SLOT(click1()));
	connect(regexp[i], SIGNAL(set_file(char *)), this, SLOT(open_file(char *)));
	regexp[i]->setEnabled(false);
	glayout->addWidget(regexp[i], i+1, 3);
	QWhatsThis::add(regexp[i], i18n("To set a word in the form of regular expression"));
	connect(lang[i], SIGNAL(activated(int)), num[i], SLOT(enable(int)));
	connect(lang[i], SIGNAL(activated(int)), regexp[i], SLOT(update(int)));
	connect(lang[i], SIGNAL(activated(int)), dict[i], SLOT(update(int)));
    }

    vlayout->addWidget(group1);
    QWhatsThis::add(group1, i18n("The block of layouts allow to set  languages and words for their processing by the program."));


    remem_win = new QCheckBox(i18n("Remember Layout For Each Window"), this);
    vlayout->addWidget(remem_win);
    QWhatsThis::add(remem_win, i18n("Program will save a language for each window."));

    group2 =  new QGroupBox(i18n("Excluded List (one layout for program) "), this);
    glayout2 = new QGridLayout(group2, 5, 3, 15, 5);
    list = new KXNListBox(group2);
    glayout2->addMultiCellWidget(list, 0, 4, 0, 1);
    QWhatsThis::add(list, i18n("List of the programs or names of MainWindow of these programs that coincide in most cases."));
    addprg = new QPushButton(i18n("Add program from list..."), group2);
    QObject::connect(addprg, SIGNAL(clicked()), list, SLOT( getProgList() ));
    glayout2->addWidget(addprg, 0, 2);
    QWhatsThis::add(addprg, i18n("List of the running graphical programs or the names of their  MainWindow (for several programs, for example sim, they do not coincide). If You do not see in the list  your program use the button 'Point the window' below. The list of the programs was received  by a query of child windows of the root-window by means of xwininfo from x11-tools."));
    add2prg = new QPushButton(i18n("Point window..."), group2);
    QObject::connect(add2prg, SIGNAL(clicked()), list, SLOT( getPointedProg() ));
    glayout2->addWidget(add2prg, 1, 2);
    QWhatsThis::add(add2prg, i18n("After pressing the button 'Point the button' one should make mouse click on a program window that must be added to the list. It is possible one will have to point out several windows for some sdi programs that have independant windows. Xprop from X11 is used for receiving the information of pointed window."));
    delprg = new QPushButton(i18n("Remove"), group2);
    QObject::connect(delprg, SIGNAL(clicked()), list, SLOT( deleteSelected() ));
    glayout2->addWidget(delprg, 2, 2);
    glayout2->addWidget(new QLabel(" ", group2), 3, 2);

    QObject::connect(remem_win, SIGNAL(toggled(bool)), group2, SLOT( setEnabled(bool) ));

    vlayout->addWidget(group2);
    vlayout->setAlignment( Qt::AlignTop );
}

XNeurPage::~XNeurPage()
{
}


void XNeurPage::open_file(char * path)
{
    bool ok;
    QString p(path), txt;
    QFile fl(p);
    if ( fl.open(IO_ReadOnly) ) {
	QTextStream ts( &fl );
	txt = ts.read();
        fl.close();
	QString text = KInputDialog::getMultiLineText( p, NULL, txt, &ok, this );
	if ( ok ) {
	    // printf("press ok\n");
	    if ( fl.open(IO_WriteOnly) ) {
		QTextStream ts2( &fl );
		ts2 << text;
		fl.close();
	    }
	    else
		qDebug("can't open for write %s\n", path);
	}
	/* else
	    printf("calcel\n");*/
    }
    else
	qDebug("can't open for read %s\n", path);
}


KeysPage::KeysPage( QWidget* parent, const char* name, WFlags fl )
 : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "KeysPage" );

    vlayout = new QVBoxLayout(this);
    vlayout->setSpacing(5);
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

    for ( int i = 0 ; i < MAX_SOUNDS; i++ ) {
        switch ( i ) {
	    case  0 : new QLabel(i18n("Xneur started "), grid); break;
	    case  1 : new QLabel(i18n("Xneur reloaded "), grid); break;
	    case  2 : new QLabel(i18n("Xneur stopped "), grid); break;
	    case  3 : new QLabel(i18n("Press Key On Layout 1 "), grid); break;
	    case  4 : new QLabel(i18n("Press Key On Layout 2 "), grid); break;
	    case  5 : new QLabel(i18n("Press Key On Layout 3 "), grid); break;
	    case  6 : new QLabel(i18n("Press Key On Layout 4 "), grid); break;
	    case  7 : new QLabel(i18n("Enable Layout 1 "), grid); break;
	    case  8 : new QLabel(i18n("Enable Layout 2 "), grid); break;
	    case  9 : new QLabel(i18n("Enable Layout 3 "), grid); break;
	    case 10 : new QLabel(i18n("Enable Layout 4 "), grid); break;
	    case 11 : new QLabel(i18n("Automatic Change Word "), grid); break;
	    case 12 : new QLabel(i18n("Manual Change Word "), grid); break;
	    case 13 : new QLabel(i18n("Change String "), grid); break;
	    case 14 : new QLabel(i18n("Change Selected Text "), grid); break;
	    case 15 : new QLabel(i18n("Translit Selected Text "), grid); break;
	    case 16 : new QLabel(i18n("Changecase Selected Text "), grid); break;
	    case 17 : new QLabel(i18n("Expand abbreviations "), grid); break;
	    case 18 : new QLabel(i18n("Correct aCCIDENTAL caps "), grid); break;
	    case 19 : new QLabel(i18n("Correct TWo INitial caps "), grid); break;
	    case 20 : new QLabel(i18n("Execute user action "), grid); break;
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

OsdPage::OsdPage( QWidget* parent, const char* name, WFlags fl )
 : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "OsdPage" );

    vlayout = new QVBoxLayout(this);
    vlayout->setSpacing(5);

    enable_osd = new QCheckBox(i18n("Enable OSD on XNeur actions"), this);
    vlayout->addWidget(enable_osd);

    hlayout = new QHBoxLayout(vlayout);
    QLabel *label = new QLabel(i18n("OSD Font "), this);
    hlayout->addWidget(label);
    osd_font = new QLineEdit(i18n("Use this font for OSD"), this);
    hlayout->addWidget(osd_font);

    label = new QLabel(i18n("Use command \"xfontsel -print\" to get new font."), this);
    vlayout->addWidget(label);

    grid = new QGrid(2, this);
    grid->setSpacing(2);
    grid->setMargin(5);
    grid->setFrameShape(QFrame::GroupBoxPanel);

    for ( int i = 0 ; i < MAX_OSDS; i++ ) {
        switch ( i ) {
	    case  0 : new QLabel(i18n("Xneur started "), grid); break;
	    case  1 : new QLabel(i18n("Xneur reloaded "), grid); break;
	    case  2 : new QLabel(i18n("Xneur stopped "), grid); break;
	    case  3 : new QLabel(i18n("Press Key On Layout 1 "), grid); break;
	    case  4 : new QLabel(i18n("Press Key On Layout 2 "), grid); break;
	    case  5 : new QLabel(i18n("Press Key On Layout 3 "), grid); break;
	    case  6 : new QLabel(i18n("Press Key On Layout 4 "), grid); break;
	    case  7 : new QLabel(i18n("Enable Layout 1 "), grid); break;
	    case  8 : new QLabel(i18n("Enable Layout 2 "), grid); break;
	    case  9 : new QLabel(i18n("Enable Layout 3 "), grid); break;
	    case 10 : new QLabel(i18n("Enable Layout 4 "), grid); break;
	    case 11 : new QLabel(i18n("Automatic Change Word "), grid); break;
	    case 12 : new QLabel(i18n("Manual Change Word "), grid); break;
	    case 13 : new QLabel(i18n("Change String "), grid); break;
	    case 14 : new QLabel(i18n("Change Selected Text "), grid); break;
	    case 15 : new QLabel(i18n("Translit Selected Text "), grid); break;
	    case 16 : new QLabel(i18n("Changecase Selected Text "), grid); break;
	    case 17 : new QLabel(i18n("Expand abbreviations "), grid); break;
	    case 18 : new QLabel(i18n("Correct aCCIDENTAL caps "), grid); break;
	    case 19 : new QLabel(i18n("Correct TWo INitial caps "), grid); break;
	    case 20 : new QLabel(i18n("Execute user action "), grid); break;
	}
	edit[i] = new KXNLineEdit(grid);
    }

    vlayout->addWidget(grid);
    QWhatsThis::add(grid, i18n("List of OSDs"));

    vlayout->setAlignment( Qt::AlignTop );
}

OsdPage::~OsdPage()
{
}

AbbrPage::AbbrPage( QWidget* parent, const char* name, WFlags fl )
 : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "AbbrPage" );

    vlayout = new QVBoxLayout(this);
    vlayout->setSpacing(5);

    ignore_layout = new QCheckBox(i18n("Ignore layout for abbreviations"), this);
    vlayout->addWidget( ignore_layout );

    /*grid = new QGrid(2, this);
    grid->setSpacing(2);
    grid->setMargin(5);
    grid->setFrameShape(QFrame::GroupBoxPanel);
    vlayout->addWidget( grid );*/
    vlayout->setAlignment( Qt::AlignTop );
}

AbbrPage::~AbbrPage()
{
}

KXNeurConf::KXNeurConf(KXNeurApp *app, QWidget *parent)
 : KConfigDialog(parent, i18n("Settings"), KXNeurSettings::self(), KDialogBase::IconList, Ok|Cancel|Help, Ok, true)
{
    knapp = app;

    kxneur_page = new KXNeurPage(0, "KXNeurPage");
    xneur_page = new XNeurPage(0, "XNeurPage");
    keys_page = new KeysPage(0, "KeysPage");
    prog_page = new ProgPage(0, "ProgPage");
    snd_page = new SndPage(0, "SndPage");
    osd_page = new OsdPage(0, "OsdPage");
    abbr_page = new AbbrPage(0, "AbbrPage");

    addPage( kxneur_page, i18n("Common"), "keyboard_layout", i18n("Commons Options") ); // or pic = embedjs, exec
    addPage( xneur_page, i18n("Languages"), "locale", i18n("Languages Options") ); // or pic = 
    addPage( keys_page, i18n("Keys"), "key_bindings", i18n("Key Combinations") );
    addPage( prog_page, i18n("Programs"), "kwin", i18n("Exclusions for Program") );
    addPage( snd_page, i18n("Sounds"), "kmix", i18n("Sounds for Events") );
    addPage( osd_page, i18n("OSD"), "kmix", i18n("OSD for Events") );
    addPage( abbr_page, i18n("Abbreviations"), "kmix", i18n("Replace Abbreviations Options") );
    LoadSettings();
}

KXNeurConf::~KXNeurConf()
{
    delete kxneur_page;
    delete xneur_page;
    delete keys_page;
    delete snd_page;
    delete prog_page;
    delete osd_page;
    delete abbr_page;
}

void KXNeurConf::LoadSettings()
{

    kxneur_page->run_xneur->setChecked( KXNeurSettings::RunXNeur() );
    kxneur_page->force_run->setChecked( KXNeurSettings::ForceRun() );
    kxneur_page->autostart->setChecked( KXNeurSettings::Autostart() );
    kxneur_page->in_tray->setCurrentItem( KXNeurSettings::ShowInTray() );
    kxneur_page->sw_mode->setChecked( KXNeurSettings::SwitcherMode() );

    if ( knapp->xnconf->correct_incidental_caps )
	kxneur_page->fix_caps_lock->setChecked(true);
    else
	kxneur_page->fix_caps_lock->setChecked(false);

    if ( knapp->xnconf->correct_two_capital_letter )
	kxneur_page->fix_two_big->setChecked(true);
    else
	kxneur_page->fix_two_big->setChecked(false);

    if ( knapp->xnconf->flush_buffer_when_press_enter )
	kxneur_page->flush_buf_on_enter->setChecked(true);
    else
	kxneur_page->flush_buf_on_enter->setChecked(false);

    if ( knapp->xnconf->dont_process_when_press_enter )
	kxneur_page->not_work_on_enter->setChecked(true);
    else
	kxneur_page->not_work_on_enter->setChecked(false);

    if ( knapp->xnconf->grab_mouse )
	kxneur_page->mouse_mode->setChecked(true);
    else
	kxneur_page->mouse_mode->setChecked(false);

    if ( knapp->xnconf->educate )
	kxneur_page->edu_mode->setChecked(true);
    else
	kxneur_page->edu_mode->setChecked(false);

    if ( knapp->xnconf->save_selection )
	kxneur_page->save_sel_text->setChecked(true);
    else
	kxneur_page->save_sel_text->setChecked(false);

    if (knapp->xnconf->save_keyboard_log )
	kxneur_page->save_kbd_log->setChecked(true);
    else
	kxneur_page->save_kbd_log->setChecked(false);

    // if ( knapp->xnconf->xneur_data->manual_mode )
    if ( knapp->xnconf->manual_mode )
	kxneur_page->xneur_mode->setCurrentItem(1);
    else
	kxneur_page->xneur_mode->setCurrentItem(0);

    xneur_page->send_delay->setValue(knapp->xnconf->send_delay);

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
	    xneur_page->lang[i]->setCurrentItem(8);
	    xneur_page->regexp[i]->lang = lang_code_list[8];
	    xneur_page->dict[i]->lang = lang_code_list[8];
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

    if ( knapp->xnconf->remember_layout )
	xneur_page->remem_win->setChecked(true);
    else
	xneur_page->remem_win->setChecked(false);

    for (int i = 0 ; i < knapp->xnconf->layout_remember_apps->data_count ; i++ )
	xneur_page->list->insertItem(knapp->xnconf->layout_remember_apps->data[i].string);


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
    keys->insert("Replace Abbreviation ",        i18n("Replace Abbreviation "),        QString::null, KeyFromXNConf(10), 0, this, NULL);

    keys_page->keyChooser = new KKeyChooser(keys, keys_page);
    keys_page->vlayout->addWidget(keys_page->keyChooser);

    // keys_page->vlayout->addWidget(new QLabel(i18n("kdelibs have bug with Ctrl+Break"), keys_page));
    QWhatsThis::add(keys_page->keyChooser, i18n("warning! kdelibs have bug with Ctrl+Break"));

    for (int i = 0 ; i < knapp->xnconf->excluded_apps->data_count ; i++ )
	prog_page->list[0]->insertItem(knapp->xnconf->excluded_apps->data[i].string);
    for (int i = 0 ; i < knapp->xnconf->auto_apps->data_count ; i++ )
	prog_page->list[1]->insertItem(knapp->xnconf->auto_apps->data[i].string);
    for (int i = 0 ; i < knapp->xnconf->manual_apps->data_count ; i++ )
	prog_page->list[2]->insertItem(knapp->xnconf->manual_apps->data[i].string);

    snd_page->enable_snd->setChecked( knapp->xnconf->play_sounds );

    for (int i = 0 ; i < MAX_SOUNDS ; i++)
	snd_page->edit[i]->setText(knapp->xnconf->sounds[i].file);

    osd_page->enable_osd->setChecked( knapp->xnconf->show_osd );
    osd_page->osd_font->setText(i18n(knapp->xnconf->osd_font));
    for (int i = 0 ; i < MAX_OSDS ; i++)
	osd_page->edit[i]->setText(i18n(knapp->xnconf->osds[i].file));

    abbr_page->ignore_layout->setChecked( knapp->xnconf->abbr_ignore_layout );
}

void KXNeurConf::SaveSettings()
{
    KXNeurSettings::setRunXNeur( 
	kxneur_page->run_xneur->isChecked() );
    KXNeurSettings::setForceRun( 
	kxneur_page->force_run->isChecked() );
    KXNeurSettings::setAutostart( 
	kxneur_page->autostart->isChecked() );
    KXNeurSettings::setShowInTray( 
	kxneur_page->in_tray->currentItem() );
    KXNeurSettings::setSwitcherMode( 
	kxneur_page->sw_mode->isChecked() );

    knapp->xnconf->clear(knapp->xnconf);

    if ( kxneur_page->fix_caps_lock->isChecked() )
	knapp->xnconf->correct_incidental_caps = 1;
    else
	knapp->xnconf->correct_incidental_caps = 0;

    if ( kxneur_page->fix_two_big->isChecked() )
	knapp->xnconf->correct_two_capital_letter = 1;
    else
	knapp->xnconf->correct_two_capital_letter = 0;

    if ( kxneur_page->flush_buf_on_enter->isChecked() )
	knapp->xnconf->flush_buffer_when_press_enter = 1;
    else
	knapp->xnconf->dont_process_when_press_enter = 0;

    if ( kxneur_page->mouse_mode->isChecked() )
	knapp->xnconf->grab_mouse = 1;
    else
	knapp->xnconf->grab_mouse = 0;

    if ( kxneur_page->edu_mode->isChecked() )
	knapp->xnconf->educate = 1;
    else
	knapp->xnconf->educate = 0;

    if ( kxneur_page->save_sel_text->isChecked() )
	knapp->xnconf->save_selection = 1;
    else
	knapp->xnconf->save_selection = 0;

    if ( kxneur_page->save_kbd_log->isChecked() )
	knapp->xnconf->save_keyboard_log = 1;
    else
	knapp->xnconf->save_keyboard_log = 0;

    if ( kxneur_page->xneur_mode->currentItem() )
	knapp->xnconf->manual_mode = 1;
    else
	knapp->xnconf->manual_mode = 0;

    knapp->xnconf->send_delay = xneur_page->send_delay->value();

    knapp->xnconf->default_group = xneur_page->default_group->currentItem();

    for ( int i = 0 ; i < MAX_LANGUAGES ; i++ )
	if ( xneur_page->lang[i]->currentItem() ) {
	    const char *lang_name = lang_name_list[ xneur_page->lang[i]->currentItem() ];
	    const char *lang_dir = lang_code_list[ xneur_page->lang[i]->currentItem() ];
	    int lang_group = xneur_page->num[i]->currentItem();
	    knapp->xnconf->add_language(knapp->xnconf, lang_name, lang_dir, lang_group);
	}

    if ( xneur_page->remem_win->isChecked() )
	knapp->xnconf->remember_layout = 1;
    else
	knapp->xnconf->remember_layout = 0;

    for ( int i = 0 ; i < xneur_page->list->numRows() ; i++ )
	knapp->xnconf->layout_remember_apps->add(knapp->xnconf->layout_remember_apps, xneur_page->list->text(i).latin1());


    keys_page->keyChooser->commitChanges();
    // printf("%s - %x\n", keys->shortcut("Replace / Cancel Replace ").toString().latin1(), keys->shortcut("Replace / Cancel Replace ").keyCodeQt() );
    KeyToXNConf(0, keys->shortcut("Replace / Cancel Replace "   ).keyCodeQt());
    KeyToXNConf(1, keys->shortcut("Replace Last String "        ).keyCodeQt());
    KeyToXNConf(2, keys->shortcut("Change Processing Mode "     ).keyCodeQt());
    KeyToXNConf(3, keys->shortcut("Replace Selected Text "      ).keyCodeQt());
    KeyToXNConf(4, keys->shortcut("Transliterate Selected Text ").keyCodeQt());
    KeyToXNConf(5, keys->shortcut("Change Case Selected Text "  ).keyCodeQt());
    KeyToXNConf(6, keys->shortcut("Enable First Layout "        ).keyCodeQt());
    KeyToXNConf(7, keys->shortcut("Enable Secondary Layout "    ).keyCodeQt());
    KeyToXNConf(8, keys->shortcut("Enable Thrid Layout "        ).keyCodeQt());
    KeyToXNConf(9, keys->shortcut("Enable Fourth Layout "       ).keyCodeQt());
    KeyToXNConf(10, keys->shortcut("Replace Abbreviation "       ).keyCodeQt());


    for ( int i = 0 ; i < prog_page->list[0]->numRows() ; i++ )
	knapp->xnconf->excluded_apps->add(knapp->xnconf->excluded_apps, prog_page->list[0]->text(i).latin1());

    for ( int i = 0 ; i < prog_page->list[1]->numRows() ; i++ )
	knapp->xnconf->auto_apps->add(knapp->xnconf->auto_apps, prog_page->list[1]->text(i).latin1());

    for ( int i = 0 ; i < prog_page->list[2]->numRows() ; i++ )
	knapp->xnconf->manual_apps->add(knapp->xnconf->manual_apps, prog_page->list[2]->text(i).latin1());

    if ( snd_page->enable_snd->isChecked() )
	knapp->xnconf->play_sounds = 1;
    else
	knapp->xnconf->play_sounds = 0;

    for ( int i = 0 ; i < MAX_SOUNDS ; i++ ) {
	if ( knapp->xnconf->sounds[i].file )
	    free(knapp->xnconf->sounds[i].file);
	knapp->xnconf->sounds[i].file = qstrdup(snd_page->edit[i]->text().utf8());
    }

    if ( osd_page->enable_osd->isChecked() )
	knapp->xnconf->show_osd = 1;
    else
	knapp->xnconf->show_osd = 0;

    if ( knapp->xnconf->osd_font )
	free(knapp->xnconf->osd_font);
    knapp->xnconf->osd_font = qstrdup(osd_page->osd_font->text().utf8());

    for ( int i = 0 ; i < MAX_OSDS ; i++ ) {
	if ( knapp->xnconf->osds[i].file )
	    free(knapp->xnconf->osds[i].file);
	knapp->xnconf->osds[i].file = qstrdup(osd_page->edit[i]->text().utf8());
    }

    if ( abbr_page->ignore_layout->isChecked() )
	knapp->xnconf->abbr_ignore_layout = 1;
    else
	knapp->xnconf->abbr_ignore_layout = 0; 

    knapp->xnconf->save(knapp->xnconf);
    knapp->xnconf->reload(knapp->xnconf);

/*    if ( knapp->xneur_stop() )
	knapp->xneur_start();
    else
	qDebug("without restart\n");*/

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

    if ( knapp->xnconf->hotkeys[action].modifiers & 0x1 )
	shc += Qt::SHIFT;
    if ( knapp->xnconf->hotkeys[action].modifiers & 0x2  )
	shc += Qt::CTRL;
    if ( knapp->xnconf->hotkeys[action].modifiers & 0x4 )
	shc += Qt::ALT;
    if ( knapp->xnconf->hotkeys[action].modifiers & 0x8 )
	shc += KKey::QtWIN;
    int ccc = kc.key2code(knapp->xnconf->hotkeys[action].key);
    //qDebug("%d : %s == %x -> ", action, knapp->xnconf->hotkeys[action].key, ccc);
    shc += ccc;
    // qDebug("%x\n", shc);
    orig_keys[action] = shc;
    return shc;
}

void KXNeurConf::KeyToXNConf(int a, int c)
{
    int action = a, cod = c;

    if ( (cod & 0xffff) == 0 ) {
	qDebug("not found key for action %d: key code = %x; restore old key\n", action, cod); // kdelibs have bug with Ctrl+Break
	// fflush(stdout);
        cod = orig_keys[action];
	// return;
    }

    knapp->xnconf->hotkeys[action].modifiers = 0;
    if ( knapp->xnconf->hotkeys[action].key ) {
    	free(knapp->xnconf->hotkeys[action].key);
	knapp->xnconf->hotkeys[action].key = NULL;
    }

    if ( cod & Qt::SHIFT )
        knapp->xnconf->hotkeys[action].modifiers |= 0x1;
    if ( cod & Qt::CTRL )
	knapp->xnconf->hotkeys[action].modifiers |= 0x2;
    if ( cod & Qt::ALT )
	knapp->xnconf->hotkeys[action].modifiers |= 0x4;
    if ( cod & KKey::QtWIN )
	knapp->xnconf->hotkeys[action].modifiers |= 0x8;

    cod &= 0x0000ffff;
    QString cc = kc.code2key(cod);
	// printf("%x -> ", cod );
	// printf("%s\n", cc.latin1());
	// fflush(stdout);
    knapp->xnconf->hotkeys[action].key = qstrdup( cc.latin1() );
}
