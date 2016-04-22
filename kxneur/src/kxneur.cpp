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

#include <signal.h>
#include <kaction.h>
#include <kpopupmenu.h>
#include <qstringlist.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <qpainter.h>
#include <kconfig.h>
#include <klocale.h>
#include <qmessagebox.h>
#include "kxneur.h"
#include "kxneurtray.h"
#include "kxneursettings.h"
#include "kxneurconf.h"
#include "kxnkeyboard.h"


KXNeurApp::KXNeurApp()
#ifdef XN_END
 : KUniqueApplication()
#else
 : KApplication()
#endif
{
    KGlobal::dirs()->addResourceDir("appdata", ".");
    all_langs = new KConfig("langs", true, true, "appdata");
    all_langs->setGroup("Languages");

    xnkb = KXNKeyboard::self();
    trayicon = new KXNeurTray();
    setMainWidget( trayicon );
    trayicon->show();

    // xneur_pid = 0;
    xnconf = NULL;
    cur_lang = -1;
    prev_lang = -1;
    for ( int i = 0 ; i < MAX_LANGUAGES ; i++ )
	langs[i] = NULL;

    if ( !(KXNeurSettings::self()->RunXNeur() && xneur_start()) ) {
#ifndef XN_END
	qDebug("start -- ok\n");
#endif
	trayicon->run->setText(i18n("Start xneur daemon"));
	trayicon->run->setIcon("fork");
	trayicon->mode->setEnabled(false);
	// xnconf_reload();
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
    int major_v, minor_v;

    if ( xnconf )
	if (!xneur_stop()) {
#ifndef XN_END
	    qDebug("start -- fail 1\n");
#endif
	    return false;
	}
	else
	    xnconf = NULL;
    xnconf = xneur_config_init();
    if ( xnconf > 0 ) {
	xnconf->get_library_version(&major_v, &minor_v);
	if ( major_v != XNEUR_MAJOR_VERSION && minor_v != XNEUR_MINOR_VERSION ) {
		qDebug("Wrong XNeur configuration library api version.\nPlease install libxnconfig version %d.x\n", XNEUR_MAJOR_VERSION);
		QMessageBox::critical(0, "XNeur", i18n("Wrong XNeur configuration library api version.\nPlease install libxnconfig version ")+
			QString::number(XNEUR_MAJOR_VERSION)+".x");
                xnconf->uninit(xnconf);
		quit();
	}
	// xnconf_reload();
	if ( !xnconf->load(xnconf) ) {
		qDebug("XNeur config broken!\nPlease, remove ~/.xneur/xneurrc and reinstall XNeur package!\n");
		QMessageBox::critical(0, "XNeur", i18n("XNeur config broken!\nPlease, remove ~/.xneur/xneurrc and reinstall XNeur package!\n"));
                xnconf->uninit(xnconf);
		quit();
	}
        QStringList args;
        if ( KXNeurSettings::self()->ForceRun() )
	    args << "-f";
	if ( kdeinitExec("xneur", args, 0, NULL, 0) == 0 ) {
	    xnconf_gets();
	    trayicon->run->setText(i18n("Stop xneur daemon"));
	    trayicon->run->setIcon("cancel"); // stop, no, remove
	    trayicon->mode->setEnabled(true);
#ifndef XN_END
	    qDebug("start -- ok\n");
#endif
	    return true;
	}
	qDebug("Couldn't start xneur\nVerify that it installed\n");
	QMessageBox::critical(0, "XNeur", i18n("Couldn't start xneur\nVerify that it installed\n"));
    }
    xnconf = NULL;
#ifndef XN_END
    qDebug("start -- fail 2\n");
#endif
    return false;
}


bool KXNeurApp::xneur_stop()
{
    if ( xnconf && xnconf->get_pid(xnconf) ) {
	if ( !xnconf->kill(xnconf) ) {
#ifndef XN_END
	    qDebug("stop -- fail 1\n");
#endif
            return false;
	}
	xnconf->uninit(xnconf);
	xnconf = NULL;
	trayicon->run->setText(i18n("Start xneur daemon"));
	trayicon->run->setIcon("fork"); // launch
	trayicon->mode->setEnabled(false);
#ifndef XN_END
	qDebug("stop -- ok\n");
#endif
	return true;
    }
#ifndef XN_END
    qDebug("stop -- fail 3\n");
#endif
    return false;
}


void KXNeurApp::slotExit()
{
    xneur_stop();
}

void KXNeurApp::slotUpdateMode()
{
    if ( xnconf->is_manual_mode(xnconf) )
	xnconf->set_manual_mode(xnconf, 0);
    else
	xnconf->set_manual_mode(xnconf, 1);

    xnconf_gets();
}


void KXNeurApp::xnconf_gets()
{
    cnt_langs = xnconf->total_languages;


    if ( xnconf->manual_mode ) {
	trayicon->mode->setText(i18n("Set auto mode"));
	trayicon->mode->setIcon("exec"); // mics, gear
	// printf("auto\n");
    }
    else {
	trayicon->mode->setText(i18n("Set manual mode"));
	trayicon->mode->setIcon("embedjs");
	// printf("manual\n");
    }
}


void KXNeurApp::slotUpdateRun()
{
    if ( xnconf )
	xneur_stop();
    else
	xneur_start();
}


void KXNeurApp::slotPref()
{
    KXNeurConf *dlg = new KXNeurConf(this);

    dlg->exec();

    delete dlg;

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
    int i = 0;


    for ( ; i < MAX_LANGUAGES ; i++ ) {
	if ( langs[i] != NULL ) {
	    trayicon->menu->removeItem(langs[i]->menuid);
	    delete langs[i];
	}
	langs[i] = NULL;
    }

    i = 0;
    for ( QStringList::Iterator iter = lngs.begin(); iter != lngs.end() ; iter++, i++ ) {
	if ( i >= MAX_LANGUAGES ) {
	    qDebug("Error: found more then 4 langs!!!");
	    xneur_stop();
	    exit(1);
	}
	lng = *iter;
	QString code = all_langs->readEntry(lng);
	if ( code == QString::null ) {
	    langs[i] = new XNLang(i18n("* Unknown lang *"));
	    langs[i]->lg = QString::number(i);
	    langs[i]->supp_lg = -1;
	}
	else {
	    langs[i] = new XNLang(lng);
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
	int l = -1;
	if ( code != QString::null ) {
	    for ( i = 0 ; i < MAX_LANGUAGES && langs[i] != NULL ; i++ ) {
		if ( code == langs[i]->lg ) {
		    langs[i]->name = xnconf->get_lang_name(xnconf, j);
		    langs[i]->supp_lg = 1;
		    l = i;
		    break;
		}
	    }
	    if ( l >= 0 ) {
		QString path = locate("appdata", langs[l]->name+".png");
		QPixmap pic;
		if ( KXNeurSettings::ShowInTray() == SHOW_ICON && !path.isEmpty() ) {
			// printf("show flag\n");
		    pic.load(path);
		    langs[l]->pic = pic;
		}
	    }
	}
    }

    for ( i = 0 ; i < MAX_LANGUAGES && langs[i] != NULL ; i++ )
	langs[i]->menuid = trayicon->menu->insertItem(langs[i]->pic, langs[i]->name, i, i+1);
}


void KXNeurApp::setNextLang()
{
    int groupno;
    if ( KXNeurSettings::SwitcherMode() && prev_lang >= 0 ) {
	groupno = prev_lang;
	// qDebug("switch mode");
    }
    else {
        groupno = cur_lang + 1;
	if ( groupno >= MAX_LANGUAGES || langs[groupno] == NULL )
	    groupno = 0;
	// qDebug("ring mode");
    }
    xnkb->setGroupNo(groupno);
}


void KXNeurApp::setMenuLang(int nn)
{
    // qDebug("click lang %d\n", nn);
    for ( uint i = 0 ; i < MAX_LANGUAGES && langs[i] != NULL ; i++ )
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
