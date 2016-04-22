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
#ifndef KXNKEYBOARD_H
#define KXNKEYBOARD_H

#include <qobject.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

/**
	@author Vadim Likhota <vadim-lvv{a}yandex.ru>
this file based on xkeyboard from kkbswith
*/

class QStringList;

class KXNKeyboard : public QObject
{
    Q_OBJECT
public:
    static KXNKeyboard *self();
    ~KXNKeyboard();

    // Set the current keyboard group to the given groupno
    void setGroupNo(int groupno);
    // Get the names of the currently configured keyboard groups
    // void getGroupNames(QStringList &list);
    // return the current keyboard group index
    int getGroupNo();
    // return if XKEYBOARD extension is available in the X server
    bool xkbAvailable() { return m_xkb_available; }
    // Examines an X Event passed to it and takes actions if the event is of interest to XKeyboard
    void processEvent(XEvent *ev);
private:
    Display *display;
    int m_event_code;
    int m_numgroups;
    bool m_xkb_available;
    static KXNKeyboard *mSelf;

    KXNKeyboard();
    void retrieveNumKbdGroups();

signals:
    /* Signals that new keyboard group is selected */
    void groupChanged(int groupno);
    /* Signals that keyboard layout has changed and thus we need to reconfigure */
    void layoutChanged();
};

#endif
