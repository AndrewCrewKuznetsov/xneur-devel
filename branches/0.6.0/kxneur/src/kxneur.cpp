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

#include <signal.h>
#include <kaction.h>
#include <kpopupmenu.h>
#include <qstringlist.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <qpainter.h>
#include <kaction.h>
#include <klocale.h>
#include "kxneur.h"
#include "kxneurtray.h"
#include "kxneursettings.h"
#include "kxneurconf.h"
#include "kxnkeyboard.h"


KXNeurApp::KXNeurApp()
 : KUniqueApplication()
{
    xnkb = KXNKeyboard::self();
    trayicon = new KXNeurTray();
    setMainWidget( trayicon );
    trayicon->show();

    xneur_pid = 0;
    xnconf = NULL;
    cur_lang = -1;
    prev_lang = -1;
    if ( KXNeurSettings::self()->RunXNeur() )
	xneur_start(); // TODO: проверить старт
    else {
	trayicon->run->setText(i18n("Start xneur daemon"));
	trayicon->run->setIcon("fork");
	trayicon->mode->setEnabled(false);
	xnconf_reload();
    }
    cur_lang = xnkb->getGroupNo();
    refreshLang();
    trayicon->setPixmap(langs[cur_lang]->pic);

    QObject::connect(xnkb, SIGNAL(groupChanged(int)), this, SLOT(groupChange(int)));
    QObject::connect(xnkb, SIGNAL(layoutChanged()), this, SLOT(refreshLang()));
    QObject::connect(trayicon, SIGNAL(clicked()), this, SLOT(setNextLang()));
    QObject::connect(trayicon, SIGNAL(quitSelected()), this, SLOT(slotExit()));
}


KXNeurApp::~KXNeurApp()
{
}


bool KXNeurApp::xneur_start()
{
    xnconf_reload();
    xneur_pid = xnconf->get_pid(xnconf);
    if ( xneur_pid > 0 )
	xneur_stop();
    kdeinitExec("xneur", 0, 0, &xneur_pid, 0);
    if ( xneur_pid > 0 ) {
	xnconf_reload();
	xnconf_gets();
	trayicon->run->setText(i18n("Stop xneur daemon"));
	trayicon->run->setIcon("cancel"); // stop, no, remove
	trayicon->mode->setEnabled(true);
	return true;
    }
    xnconf = NULL;
    return false;
}


bool KXNeurApp::xneur_stop()
{
    if ( xneur_pid > 0 ) {
	if (kill(xneur_pid, SIGTERM) == -1)
            return false;
	xneur_pid = 0;
	xnconf = NULL;
	trayicon->run->setText(i18n("Start xneur daemon"));
	trayicon->run->setIcon("fork"); // launch
	trayicon->mode->setEnabled(false);
	return true;
    }
    return false;
}


void KXNeurApp::slotExit()
{
    xneur_stop();
}


void KXNeurApp::xnconf_reload()
{
    if (xnconf != NULL)
	xnconf->uninit(xnconf);
    xnconf = xneur_config_init();
    enum _error error_code = xnconf->load(xnconf);
    if (error_code != ERROR_SUCCESS) {
	xnconf->uninit(xnconf);
	printf("XNeur config broken!\nPlease, remove ~/.xneur/xneurrc and reinstall package!\n");
	// exit(EXIT_FAILURE);
	quit();
    }
}


void KXNeurApp::slotUpdateMode()
{
    // xnconf_reload();
    if ( xnconf->get_current_mode(xnconf) == AUTO_MODE )
	xnconf->set_current_mode(xnconf, MANUAL_MODE);
    else
	xnconf->set_current_mode(xnconf, AUTO_MODE);

    xnconf_gets();
}


void KXNeurApp::xnconf_gets()
{
    cnt_langs = xnconf->total_languages;
    

    if ( xnconf->get_current_mode(xnconf) == AUTO_MODE ) {
	trayicon->mode->setText(i18n("Set manual mode"));
	trayicon->mode->setIcon("embedjs");
	// printf("manual\n");
    }
    else {
	trayicon->mode->setText(i18n("Set auto mode"));
	trayicon->mode->setIcon("exec"); // mics, gear
	// printf("auto\n");
    }
}


void KXNeurApp::slotUpdateRun()
{
    if ( xneur_pid > 0 )
	xneur_stop();
    else
	xneur_start();
}


void KXNeurApp::slotPref()
{
    KXNeurConf dlg(this);
    dlg.exec();
    refreshLang();
    trayicon->setPixmap(langs[cur_lang]->pic);
}


bool KXNeurApp::x11EventFilter(XEvent *e)
{
    // let m_xkb process the event and emit signals if necessary
    xnkb->processEvent(e);
    return KApplication::x11EventFilter(e);
}

void KXNeurApp::groupChange(int groupno)
{
    if ( cur_lang != groupno ) {
	prev_lang = cur_lang;
	cur_lang = groupno;
	trayicon->setPixmap(langs[cur_lang]->pic);
    }
}


void KXNeurApp::refreshLang()
{
    for ( uint i = 0 ; i < langs.count() ; i++ )
	trayicon->menu->removeItem(langs[i]->menuid);
    langs.clear();

    langs.resize(xnconf->total_languages);
    for ( int i = 0 ; i < xnconf->total_languages ; i++ ) {
	// TODO: move into external file and use KConfig for read this file
	QString path;
	QPixmap pic;
	if ( strcmp(xnconf->get_lang_name(xnconf, i), "English") == 0 ) {
	    langs.insert(i, new XNLang("English"));
	    path = locate("pixmap", "English.png");
	    langs[i]->lg = "EN";
	}
	else if ( strcmp(xnconf->get_lang_name(xnconf, i), "Russian") == 0 ) {
	    langs.insert(i, new XNLang("Russian"));
	    path = locate("pixmap", "Russian.png");
	    langs[i]->lg = "RU";
	}
	else if ( strcmp(xnconf->get_lang_name(xnconf, i), "Ukrainian") == 0 ) {
	    langs.insert(i, new XNLang("Ukrainian"));
	    path = locate("pixmap", "Ukrainian.png");
	    langs[i]->lg = "UK";
	}
	else if ( strcmp(xnconf->get_lang_name(xnconf, i), "Belarussian") == 0 ) {
	    langs.insert(i, new XNLang("Belarussian"));
	    path = locate("pixmap", "Belarussian.png");
	    langs[i]->lg = "BE";
	}
	else if ( strcmp(xnconf->get_lang_name(xnconf, i), "French") == 0 ) {
	    langs.insert(i, new XNLang("French"));
	    path = locate("pixmap", "French.png");
	    langs[i]->lg = "FR";
	}
	else if ( strcmp(xnconf->get_lang_name(xnconf, i), "Romanian") == 0 ) {
	    langs.insert(i, new XNLang("Romanian"));
	    path = locate("pixmap", "Romanian.png");
	    langs[i]->lg = "RO";
	}
	if ( KXNeurSettings::ShowInTray() == SHOW_ICON && !path.isEmpty() ) {
	    // printf("show flag\n");
	    pic.load(path);
	    langs[i]->pic = pic;
	}
	else {
	    // printf("show lang\n");
	    QPixmap pix(19, 16);
	    QPainter painter(&pix);
	    QFont font("helvetica", 9, QFont::Bold);
	    font.setPixelSize(10);
	    painter.setFont(font);
            painter.setPen(KGlobalSettings::highlightedTextColor());
            pix.fill(KGlobalSettings::highlightColor());
	    painter.drawText(1, 0, pix.width(), pix.height(), Qt::AlignHCenter | Qt::AlignVCenter, langs[i]->lg);
	    langs[i]->pic = pix;
	}
	langs[i]->menuid = trayicon->menu->insertItem(langs[i]->pic, langs[i]->name, i, i+1);
    }
}


void KXNeurApp::setNextLang()
{
    int groupno;
    if ( KXNeurSettings::SwitcherMode() && prev_lang >= 0 ) {
	groupno = prev_lang;
	// printf("switch mode\n");
    }
    else {
        groupno = cur_lang + 1;
	if ( groupno >= (int)langs.count() )
	    groupno = 0;
	// printf("ring mode\n");
    }
    xnkb->setGroupNo(groupno);
}


void KXNeurApp::setMenuLang(int nn)
{
    // printf("click lang %d\n", nn);
    for ( uint i = 0 ; i < langs.count() ; i++ ) {
	if ( langs[i]->menuid == nn ) {
	    xnkb->setGroupNo(i);
	    break;
	}
    }
}




XNLang::XNLang(const QString& n)
{
    name = n;
}

XNLang::~XNLang()
{
}


#include "kxneur.moc"

