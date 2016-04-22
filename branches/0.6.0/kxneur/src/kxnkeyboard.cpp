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
#include "kxnkeyboard.h"

#include <qwindowdefs.h>
#include <qstringlist.h>
#include <klocale.h>


KXNKeyboard *KXNKeyboard::mSelf = 0;

KXNKeyboard::KXNKeyboard()
{
    display = qt_xdisplay();
    int opcode, errorBase, major = XkbMajorVersion, minor = XkbMinorVersion;

    // check the library version
    if (!XkbLibraryVersion(&major, &minor))
	printf("warning: major and  minor version is not match\n");

    // initialize the extension
    m_xkb_available = XkbQueryExtension(display, &opcode, &m_event_code, &errorBase, &major, &minor);
    if (!m_xkb_available) {
	printf("The X Server does not support a compatible XKB extension.\nexit");
	// exit(1);
    }
    else {
	// register for XKB events
	// group state change, i.e. the current group changed:
	XkbSelectEventDetails(display, XkbUseCoreKbd, XkbStateNotify, XkbAllStateComponentsMask, XkbGroupStateMask);
 	// keyboard mapping change:
	XkbSelectEventDetails(display, XkbUseCoreKbd, XkbMapNotify, XkbAllMapComponentsMask, XkbKeySymsMask);
	// group names change:
	XkbSelectEventDetails(display, XkbUseCoreKbd, XkbNamesNotify, XkbAllNamesMask, XkbGroupNamesMask);
	// new keyboard:
	XkbSelectEventDetails(display, XkbUseCoreKbd, XkbNewKeyboardNotify, XkbAllNewKeyboardEventsMask, XkbAllNewKeyboardEventsMask);
    }
    mSelf = this;
}


KXNKeyboard::~KXNKeyboard()
{
}


/* Set the current keyboard group to the given groupno */
void KXNKeyboard::setGroupNo(int groupno){
    XkbLockGroup(display, XkbUseCoreKbd, groupno);
}


KXNKeyboard *KXNKeyboard::self()
{
  if ( !mSelf ) {
    new KXNKeyboard();
  }

  return mSelf;
}
/*
extern "C" {
static int IgnoreXError(Display *, XErrorEvent *) {
    return 0;
}
}


// Get the names of the currently configured keyboard groups
void KXNKeyboard::getGroupNames(QStringList &list){
    XkbDescRec xkb;
    // Display *display = qt_xdisplay();
    char *names[XkbNumKbdGroups];

    memset(&xkb, 0, sizeof(xkb));
    xkb.device_spec = XkbUseCoreKbd;
    XkbGetNames(display, XkbGroupNamesMask, &xkb);
    memset(names, 0, sizeof(char *) * XkbNumKbdGroups);
    // XGetAtomNames below may generate BadAtom error, which is not a problem.
    // (it may happen if the name for a group was not defined)
    // Thus we temporarily ignore X errors
    XErrorHandler old_handler = XSetErrorHandler(IgnoreXError);
    XGetAtomNames(display, xkb.names->groups, m_numgroups, names);
    // resume normal X error processing
    XSetErrorHandler(old_handler);
    for (int i = 0; i < m_numgroups; i++) {
	if (names[i]) {
	    list.append(names[i]);
	    XFree(names[i]);
	}
	else 
	    list.append(QString::null);
    }
    XkbFreeNames(&xkb, XkbGroupNamesMask, 1);
}
*/

// return the current keyboard group index
int KXNKeyboard::getGroupNo(){
    XkbStateRec rec;
    XkbGetState(display, XkbUseCoreKbd, &rec);
    return (int) rec.group;
}


void KXNKeyboard::retrieveNumKbdGroups(){
    XkbDescRec xkb;

    memset(&xkb, 0, sizeof(xkb));
    // Interestingly, in RedHat 6.0 (XFree86 3.3.3.1) the XkbGetControls call
    // below works even if xkb.device_spec is not set. But in RedHat 7.1 (XFree86 4.0.3)
    // it returns BadImplementation status code, and you have to specify
    // xkb.device_spec = XkbUseCoreKbd.
    xkb.device_spec = XkbUseCoreKbd;
    XkbGetControls(qt_xdisplay(), XkbGroupsWrapMask, &xkb);
    m_numgroups = xkb.ctrls->num_groups;
    XkbFreeControls(&xkb, XkbGroupsWrapMask, 1);
}


// Examines an X Event passed to it and takes actions if the event is of interest to XKeyboard
void KXNKeyboard::processEvent(XEvent *ev) {
    if (ev->type == m_event_code) {
	// This an XKB event
	XkbEvent *xkb_ev = (XkbEvent *) ev;
	if (xkb_ev->any.xkb_type == XkbStateNotify)
	    emit groupChanged(xkb_ev->state.group); // state notify event, the current group has changed
	else if ((xkb_ev->any.xkb_type == XkbMapNotify) && (xkb_ev->map.changed & XkbKeySymsMask)
	|| (xkb_ev->any.xkb_type == XkbNamesNotify) && (xkb_ev->names.changed & XkbGroupNamesMask)
	|| (xkb_ev->any.xkb_type == XkbNewKeyboardNotify)) {
	    // keyboard layout has changed
	    retrieveNumKbdGroups();
	    emit layoutChanged();
	}
    }
}


#include "kxnkeyboard.moc"
