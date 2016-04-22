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
#include <kconfig.h>
#include <klocale.h>
#include "kxneur.h"
#include "kxneurtray.h"
#include "kxneursettings.h"
#include "kxneurconf.h"
#include "kxnkeyboard.h"

#define XNEUR_MAJOR_VER      2
// #define XNEUR_MINOR_VER       0


KXNeurApp::KXNeurApp()
 : KUniqueApplication()
// : KApplication()
{
    KGlobal::dirs()->addResourceDir("appdata", ".");
    all_langs = new KConfig("langs", true, true, "appdata");
    all_langs->setGroup("Languages");

    xnkb = KXNKeyboard::self();
    trayicon = new KXNeurTray();
    setMainWidget( trayicon );
    trayicon->show();

    xneur_pid = 0;
    xnconf = NULL;
    cur_lang = -1;
    prev_lang = -1;
    if ( !(KXNeurSettings::self()->RunXNeur() && xneur_start()) ) {
//	printf("start -- ok\n");
//    else {
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
    xneur_stop();
}


bool KXNeurApp::xneur_start()
{
    if ( !xnconf_reload() ) {
#ifndef XN_END
	printf("start -- fail 0\n");
	fflush(stdout);
#endif
	return false;
    }
    xneur_pid = xnconf->get_pid(xnconf);
    if ( xneur_pid > 0 )
	if (!xneur_stop()) {
#ifndef XN_END
	    printf("start -- fail 1\n");
	    fflush(stdout);
#endif
	    return false;
	}
    QStringList args;
    if ( KXNeurSettings::self()->ForceRun() )
	args << "-f";
    kdeinitExec("xneur", args, 0, &xneur_pid, 0);
    if ( xneur_pid > 0 ) {
	xnconf_reload();
	xnconf_gets();
	trayicon->run->setText(i18n("Stop xneur daemon"));
	trayicon->run->setIcon("cancel"); // stop, no, remove
	trayicon->mode->setEnabled(true);
#ifndef XN_END
	printf("start -- ok\n");
	fflush(stdout);
#endif
	return true;
    }
    xneur_pid = 0;
    // xnconf = NULL;
#ifndef XN_END
    printf("start -- fail 2\n");
    fflush(stdout);
#endif
    return false;
}


bool KXNeurApp::xneur_stop()
{
    if ( xneur_pid > 0 ) {
	if ( xnconf->kill(xnconf) )
	    xnconf->set_pid(xnconf, 0);
	else {
#ifndef XN_END
	    printf("stop -- fail 1\n");
	    fflush(stdout);
#endif
            return false;
	}
	xneur_pid = 0;
	// xnconf = NULL;
	trayicon->run->setText(i18n("Start xneur daemon"));
	trayicon->run->setIcon("fork"); // launch
	trayicon->mode->setEnabled(false);
#ifndef XN_END
	printf("stop -- ok\n");
	fflush(stdout);
#endif
	return true;
    }
    // printf("stop -- fail 3\n");
    return false;
}


void KXNeurApp::slotExit()
{
    xneur_stop();
}


bool KXNeurApp::xnconf_reload()
{
    if (xnconf != NULL)
	xnconf->uninit(xnconf);
    if ( ( xnconf = xneur_config_init() ) == NULL ) {
#ifndef XN_END
	printf("reload -- fail\n");
	fflush(stdout);
#endif
	return false;
    }
    int major_v, minor_v;
    xnconf->get_library_api_version(&major_v, &minor_v);
    if ( major_v != XNEUR_MAJOR_VER ) {
                printf("Wrong XNeur configuration library api version.\nPlease install libxnconfig version %d.x\n", XNEUR_MAJOR_VER);
                xnconf->uninit(xnconf);
		quit();
    }
    if ( !xnconf->load(xnconf) ) {
	xnconf->uninit(xnconf);
	printf("XNeur config broken!\nPlease, remove ~/.xneur/xneurrc and reinstall package!\n");
	quit();
    }
    return true;
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

    // bool st = xneur_stop();
    dlg.exec();
    /*if ( st )
	xneur_start();
    else
	printf("without restart\n");*/

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
    QStringList lngs;
    xnkb->getGroupNames(lngs);
    QString lng;
    uint i;


    for ( i = 0 ; i < langs.count() ; i++ )
	trayicon->menu->removeItem(langs[i]->menuid);
    langs.clear();

    i = 0;
    langs.resize(lngs.count());

    for ( QStringList::Iterator iter = lngs.begin(); iter != lngs.end(); iter++, i++ ) {
	lng = *iter;
	QString code = all_langs->readEntry(lng);
	// printf("%s - %s\n", lng.latin1(), code.latin1());
	if ( code == QString::null ) {
	    langs.insert(i, new XNLang(i18n("* Unknown lang *")));
	    langs[i]->lg = QString::number(i);
	    langs[i]->supp_lg = -1;
	}
	else {
	    langs.insert(i, new XNLang(lng));
	    langs[i]->lg = code;
	    langs[i]->supp_lg = 0;
	}

	QPixmap pix(19, 16);
	QPainter painter(&pix);
	QFont font("helvetica", 9, QFont::Bold);
	font.setPixelSize(10);
	painter.setFont(font);
	if ( langs[i]->supp_lg >= 0 ) {
            painter.setPen(KGlobalSettings::highlightedTextColor());
            pix.fill(KGlobalSettings::highlightColor());
	    painter.drawText(1, 0, pix.width(), pix.height(), Qt::AlignHCenter | Qt::AlignVCenter, langs[i]->lg);
	}
	else {
	    pix.fill();
	    painter.drawText(0, 0, pix.width(), pix.height(), Qt::AlignHCenter | Qt::AlignVCenter, langs[i]->lg);
	}
	langs[i]->pic = pix;

    };

    for ( int j = 0 ; j < xnconf->total_languages ; j++ ) {
	QString code = all_langs->readEntry(xnconf->get_lang_name(xnconf, j));
	if ( code != QString::null ) {
	    QString path;
	    QPixmap pic;
	    for ( i = 0 ; i < langs.count() ; i++ ) {
		if ( code == langs[i]->lg ) {
		    langs[i]->name = xnconf->get_lang_name(xnconf, j);
		    langs[i]->supp_lg = 1;
		    break;
		}
	    }
	    path = locate("pixmap", langs[i]->name+".png");
	    if ( KXNeurSettings::ShowInTray() == SHOW_ICON && !path.isEmpty() ) {
	    // printf("show flag\n");
		pic.load(path);
		langs[i]->pic = pic;
	    }
	}
    }

    for ( i = 0 ; i < langs.count() ; i++ )
	langs[i]->menuid = trayicon->menu->insertItem(langs[i]->pic, langs[i]->name, i, i+1);
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
    for ( uint i = 0 ; i < langs.count() ; i++ )
	if ( langs[i]->menuid == nn ) {
	    xnkb->setGroupNo(i);
	    break;
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

