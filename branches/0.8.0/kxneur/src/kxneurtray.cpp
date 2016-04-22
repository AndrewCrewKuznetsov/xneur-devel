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

#include <kuniqueapplication.h>
#include <kstandarddirs.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kaboutapplication.h>
#include <kiconloader.h>
#include <klocale.h>
#include "kxneurtray.h"

KXNeurTray::KXNeurTray()
 : KSystemTray()
{
    QPixmap pic;
    actions = new KActionCollection(this);
    menu = contextMenu();
    KApplication *app = KApplication::kApplication();

    // languages set in KXNeurApp::refresh_lang()
    menu->insertSeparator();

    mode = new KAction(i18n("Set manual mode"), "embedjs", 0, app, SLOT(slotUpdateMode()), actions);
    mode->plug(menu);
    run = new KAction(i18n("Stop xneur daemon"), "stop", 0, app, SLOT(slotUpdateRun()), actions);
    run->plug(menu);

    menu->insertSeparator();
    KAction *pref = KStdAction::preferences(app, SLOT(slotPref()), actions);
    pref->plug(menu);
    KAction *help = KStdAction::help(this, SLOT(slotHelp()), actions);
    help->plug(menu);
    KAction *about = KStdAction::aboutApp(this, SLOT(slotAbout()), actions);
    about->plug(menu);

    QObject::connect(menu, SIGNAL(activated(int)), app, SLOT(setMenuLang(int)));

    pic = kapp->iconLoader()->loadIcon("keyboard_layout", KIcon::Small);
    menu->changeTitle(menu->idAt(0), pic, i18n("KXNeur"));
    setPixmap(pic);
}


KXNeurTray::~KXNeurTray()
{
}


void KXNeurTray::slotHelp()
{
    kapp->invokeHelp();
}


void KXNeurTray::slotAbout()
{
    KAboutApplication about;
    about.exec();
}


void KXNeurTray::mouseReleaseEvent (QMouseEvent *event)
{
    if (event->button() == LeftButton) {
	emit clicked();
    }
}

#include "kxneurtray.moc"
