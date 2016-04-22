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
#ifndef KXNEURTRAY_H
#define KXNEURTRAY_H

#include <ksystemtray.h>

/**
	@author Vadim Likhota <vadim-lvv{a}yandex.ru>
*/

class KAction;
class KPopupMenu;

class KXNeurTray : public KSystemTray
{
    Q_OBJECT
public:
    KXNeurTray();
    ~KXNeurTray();

    KAction *mode;
    KAction *run;
    KPopupMenu *menu;
    KActionCollection *actions;
public slots:
    void slotHelp();
    void slotAbout();

private:
    void mouseReleaseEvent (QMouseEvent *event);

signals:
    void clicked();
};

#endif
