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
#include <qhbox.h>
#include <qgrid.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qfile.h>
#include <qwhatsthis.h>
#include <klocale.h>
#include "kxnlistbox.h"
#include "kxncombobox.h"
#include "kxneur.h"
#include "kxneursettings.h"
extern "C" {
#include "list.h"
}
#include "kxneurconf.h"


char * ret_lang_on_num (int l) {
    switch ( l ) {
	case 1 : return (char *)"en";
	case 2 : return (char *)"ru";
	case 3 : return (char *)"uk";
	case 4 : return (char *)"be";
	case 5 : return (char *)"fr";
	case 6 : return (char *)"ro";
    }
    return NULL;
}



KXNPushButton::KXNPushButton(const QString & text, QWidget * parent, const char * name)
 : QPushButton(text, parent, name)
{
}


KXNPushButton::~KXNPushButton()
{
}


void KXNPushButton::update(int nn) {
    lang = ret_lang_on_num(nn);
    if ( nn )
	setEnabled(true);
    else
	setEnabled(false);
}


void KXNPushButton::click1() {
    if ( lang ) {
	KXNeurApp *knapp = (KXNeurApp *)KApplication::kApplication();
	emit set_file(knapp->xnconf->get_file_path_name(lang, file));
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

    autostart = new QCheckBox(i18n("Autostart"), this);
    vlayout->addWidget(autostart);
    QWhatsThis::add(autostart, i18n("auto_start"));

    in_tray = new QComboBox(this);
    in_tray->setEditable(false);
    in_tray->insertItem(i18n("show flag in tray"));
    in_tray->insertItem(i18n("show lang in tray"));
    vlayout->addWidget(in_tray);
    QWhatsThis::add(in_tray, i18n("in_tray"));

    sw_mode = new QCheckBox(i18n("Switcher mode"), this);
    vlayout->addWidget(sw_mode);
    QWhatsThis::add(sw_mode, i18n("sw_mode"));

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
    QWhatsThis::add(hbox, i18n("hbox"));


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
	lang[i]->insertItem(i18n("Belarussian"));
	lang[i]->insertItem(i18n("French"));
	lang[i]->insertItem(i18n("Romanian"));
	glayout->addWidget(lang[i], i+1, 0);
	num[i] = new KXNComboBox(group1);
	num[i]->setEditable(false);
	num[i]->insertItem("0");
	num[i]->insertItem("1");
	num[i]->insertItem("2");
	num[i]->insertItem("3");
	num[i]->setEnabled(false);
	glayout->addWidget(num[i], i+1, 1);
	QWhatsThis::add(num[i], i18n("num_i"));
	dict[i] = new KXNPushButton(i18n("Dictionary..."), group1);
	dict[i]->lang = ret_lang_on_num(i);
	dict[i]->file = (char *)"dict";
	connect(dict[i], SIGNAL(clicked()), dict[i], SLOT(click1()));
	connect(dict[i], SIGNAL(set_file(char *)), this, SLOT(open_file(char *)));
	dict[i]->setEnabled(false);
	glayout->addWidget(dict[i], i+1, 2);
	QWhatsThis::add(dict[i], i18n("dict_i"));
	regexp[i] = new KXNPushButton(i18n("Regular Expressions..."), group1);
	regexp[i]->lang = ret_lang_on_num(i);
	regexp[i]->file = (char *)"regexp";
	connect(regexp[i], SIGNAL(clicked()), regexp[i], SLOT(click1()));
	connect(regexp[i], SIGNAL(set_file(char *)), this, SLOT(open_file(char *)));
	regexp[i]->setEnabled(false);
	glayout->addWidget(regexp[i], i+1, 3);
	QWhatsThis::add(regexp[i], i18n("regexp_i"));
	connect(lang[i], SIGNAL(activated(int)), num[i], SLOT(enable(int)));
	connect(lang[i], SIGNAL(activated(int)), regexp[i], SLOT(update(int)));
	connect(lang[i], SIGNAL(activated(int)), dict[i], SLOT(update(int)));
    }

    vlayout->addWidget(group1);
    QWhatsThis::add(group1, i18n("group1"));

    group2 =  new QGroupBox(i18n("Additional"), this);
    vlayout2 = new QVBoxLayout(group2, 15, 5);

    mouse_mode = new QCheckBox(i18n("Processing Mouse Events"), group2);
    vlayout2->addWidget(mouse_mode);
    QWhatsThis::add(mouse_mode, i18n("mouse_mode"));
    edu_mode = new QCheckBox(i18n("Enable Education Mode"), group2);
    vlayout2->addWidget(edu_mode);
    QWhatsThis::add(edu_mode, i18n("edu_mode"));
    remem_win = new QCheckBox(i18n("Remember Layoute For Each Window"), group2);
    vlayout2->addWidget(remem_win);
    QWhatsThis::add(remem_win, i18n("remem_win"));
    save_sel_text = new QCheckBox(i18n("Save Selection After Change Selected Text"), group2);
    vlayout2->addWidget(save_sel_text);
    // QWhatsThis::add(save_sel_text, i18n(""));

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
		printf("can't open for write %s\n", path);
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
	    case 3 : new QLabel(i18n("Replace Last String "), grid); break;
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

    vlayout->addWidget(grid);
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
	addprg[i] = new QPushButton(i18n("Add program from list..."), group[i]);
	QObject::connect(addprg[i], SIGNAL(clicked()), list[i], SLOT( getProgList() ));
	glayout[i]->addWidget(addprg[i], 0, 2);
	add2prg[i] = new QPushButton(i18n("Point window..."), group[i]);
	QObject::connect(add2prg[i], SIGNAL(clicked()), list[i], SLOT( getPointedProg() ));
	glayout[i]->addWidget(add2prg[i], 1, 2);
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


KXNeurConf::KXNeurConf(KXNeurApp *app, QWidget *parent)
 : KConfigDialog(parent, i18n("Settings"), KXNeurSettings::self(), KDialogBase::IconList, Ok|/*Apply|*/Cancel|Help, Ok, true), knapp(app)
{
    kxneur_page = new KXNeurPage(0, "KXNeurPage");
    xneur_page = new XNeurPage(0, "XNeurPage");
    keys_page = new KeysPage(0, "KeysPage");
    prog_page = new ProgPage(0, "ProgPage");

    addPage( kxneur_page, i18n("KXNeur"), "keyboard_layout", i18n("KXNeur Options") ); // or pic = locale
    addPage( xneur_page, i18n("XNeur"), "embedjs", i18n("XNeur Daemon Options") );     // exec
    addPage( keys_page, i18n("Keys"), "key_bindings", i18n("Key Combinations") );
    addPage( prog_page, i18n("Programs"), "kwin", i18n("Exclusions for Program") );

    LoadSettings();
}


KXNeurConf::~KXNeurConf()
{
}

void KXNeurConf::LoadSettings()
{
    knapp->xnconf_reload();


    kxneur_page->run_xneur->setChecked( KXNeurSettings::RunXNeur() );
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

    if ( knapp->xnconf->save_selection_mode == SAVESELECTION_DISABLE )
	xneur_page->save_sel_text->setChecked(false);
    else
	xneur_page->save_sel_text->setChecked(true);


    for ( int i = 0 ; i < knapp->xnconf->total_languages && i < MAX_LANGUAGES ; i++ ) {
	if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "English") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(1);
	    xneur_page->regexp[i]->lang = ret_lang_on_num(1);
	    xneur_page->dict[i]->lang = ret_lang_on_num(1);
	}
	else if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "Russian") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(2);
	    xneur_page->regexp[i]->lang = ret_lang_on_num(2);
	    xneur_page->dict[i]->lang = ret_lang_on_num(2);
	}
	else if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "Ukrainian") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(3);
	    xneur_page->regexp[i]->lang = ret_lang_on_num(3);
	    xneur_page->dict[i]->lang = ret_lang_on_num(3);
	}
	else if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "Belarussian") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(4);
	    xneur_page->regexp[i]->lang = ret_lang_on_num(4);
	    xneur_page->dict[i]->lang = ret_lang_on_num(4);
	}
	else if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "French") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(5);
	    xneur_page->regexp[i]->lang = ret_lang_on_num(5);
	    xneur_page->dict[i]->lang = ret_lang_on_num(5);
	}
	else if ( strcmp(knapp->xnconf->get_lang_name(knapp->xnconf, i), "Romanian") == 0 ) {
	    xneur_page->lang[i]->setCurrentItem(6);
	    xneur_page->regexp[i]->lang = ret_lang_on_num(6);
	    xneur_page->dict[i]->lang = ret_lang_on_num(6);
	}
	xneur_page->num[i]->setCurrentItem(knapp->xnconf->get_lang_group(knapp->xnconf, i));
	xneur_page->num[i]->setEnabled(true);
	xneur_page->regexp[i]->setEnabled(true);
	xneur_page->dict[i]->setEnabled(true);
    }


    for ( int i = 0 ; i < MAX_HOTKEYS ; i++ ) {
	switch ( knapp->xnconf->hotkeys[i].modifier ) {
	    case MODIFIER_NONE : keys_page->mod[i]->setCurrentItem(0); break;
	    case MODIFIER_SHIFT : keys_page->mod[i]->setCurrentItem(1); break;
	    case MODIFIER_CONTROL : keys_page->mod[i]->setCurrentItem(2); break;
	    // case MODIFIER_ALT : keys_page->mod[i]->setCurrentItem(3); break;
	    default : keys_page->mod[i]->setCurrentItem(3); break;
	}
	switch ( knapp->xnconf->hotkeys[i].key ) {
	    case KEY_NONE : keys_page->key[i]->setCurrentItem(0); break;
	    case KEY_BREAK : keys_page->key[i]->setCurrentItem(1); break;
	    case KEY_SCROLL_LOCK : keys_page->key[i]->setCurrentItem(2); break;
	    // case KEY_PRINT_SCREEN : keys_page->key[i]->setCurrentItem(3); break;
	    default : keys_page->key[i]->setCurrentItem(3); break;
	}
    }


    for (int i = 0 ; i < knapp->xnconf->excluded_apps->data_count ; i++ )
	prog_page->list[0]->insertItem(knapp->xnconf->excluded_apps->data[i].string);
    for (int i = 0 ; i < knapp->xnconf->auto_apps->data_count ; i++ )
	prog_page->list[1]->insertItem(knapp->xnconf->auto_apps->data[i].string);
    for (int i = 0 ; i < knapp->xnconf->manual_apps->data_count ; i++ )
	prog_page->list[2]->insertItem(knapp->xnconf->manual_apps->data[i].string);

}

void KXNeurConf::SaveSettings()
{
    // knapp->xnconf_reload();

    KXNeurSettings::setRunXNeur( kxneur_page->run_xneur->isChecked() );
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
	knapp->xnconf->save_selection_mode = SAVESELECTION_ENABLE;
    else
	knapp->xnconf->save_selection_mode = SAVESELECTION_DISABLE;

    int total_lang = 0;
    char **lang_name = (char **)malloc(4 * sizeof(char*));
    char **lang_dir = (char **)malloc(4 * sizeof(char*));
    int *lang_group = (int *)malloc(4 * sizeof(int));
    for ( int i = 0 ; i < MAX_LANGUAGES ; i++ )
	if ( xneur_page->lang[i]->currentItem() ) {
	    lang_name[total_lang] = (char *)(xneur_page->lang[i]->currentText().latin1());
	    lang_dir[total_lang] = ret_lang_on_num( xneur_page->lang[i]->currentItem() );
	    lang_group[total_lang] = xneur_page->num[i]->currentItem();
	    total_lang++;
	}
    knapp->xnconf->set_languages(knapp->xnconf, lang_name, lang_dir, lang_group, total_lang);


    for ( int i = 0 ; i < MAX_HOTKEYS ; i++ ) {
	switch ( keys_page->mod[i]->currentItem() ) {
	    case 0 : knapp->xnconf->hotkeys[i].modifier = MODIFIER_NONE ; break;
	    case 1 : knapp->xnconf->hotkeys[i].modifier = MODIFIER_SHIFT ; break;
	    case 2 : knapp->xnconf->hotkeys[i].modifier = MODIFIER_CONTROL ; break;
	    case 3 : knapp->xnconf->hotkeys[i].modifier = MODIFIER_ALT ; break;
	}
	switch ( keys_page->key[i]->currentItem() ) {
	    case 0 : knapp->xnconf->hotkeys[i].key = KEY_NONE ; break;
	    case 1 : knapp->xnconf->hotkeys[i].key = KEY_BREAK ; break;
	    case 2 : knapp->xnconf->hotkeys[i].key = KEY_SCROLL_LOCK ; break;
	    case 3 : knapp->xnconf->hotkeys[i].key = KEY_PRINT_SCREEN ; break;
	}
    }


    knapp->xnconf->excluded_apps->uninit(knapp->xnconf->excluded_apps);
    knapp->xnconf->excluded_apps = list_init();
    for ( int i = 0 ; i < prog_page->list[0]->numRows() ; i++ )
	knapp->xnconf->excluded_apps->add(knapp->xnconf->excluded_apps, prog_page->list[0]->text(i).latin1());

    knapp->xnconf->auto_apps->uninit(knapp->xnconf->auto_apps);
    knapp->xnconf->auto_apps = list_init();
    for ( int i = 0 ; i < prog_page->list[1]->numRows() ; i++ )
	knapp->xnconf->auto_apps->add(knapp->xnconf->auto_apps, prog_page->list[1]->text(i).latin1());

    knapp->xnconf->manual_apps->uninit(knapp->xnconf->manual_apps);
    knapp->xnconf->manual_apps = list_init();
    for ( int i = 0 ; i < prog_page->list[2]->numRows() ; i++ )
	knapp->xnconf->manual_apps->add(knapp->xnconf->manual_apps, prog_page->list[2]->text(i).latin1());


    knapp->xnconf->save(knapp->xnconf);
    knapp->xneur_stop();
    knapp->xneur_start();
    KXNeurSettings::writeConfig();
}

void KXNeurConf::slotApply(){
    SaveSettings();
    KConfigDialog::slotApply();
}

void KXNeurConf::slotOk(){
    SaveSettings();
    KConfigDialog::slotOk();
}


#include "kxneurconf.moc"

