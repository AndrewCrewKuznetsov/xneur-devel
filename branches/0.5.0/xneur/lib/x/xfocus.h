/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  (c) XNeur Team 2006
 *
 */

#ifndef _XFOCUS_H_
#define _XFOCUS_H_

#include <X11/Xutil.h>

#define FOCUS_NONE		0
#define FOCUS_CHANGED		1
#define FOCUS_UNCHANGED		2
#define FOCUS_EXCLUDED		3
#define FOCUS_DUMMY		4

#define LISTEN_FLUSH		0
#define LISTEN_GRAB_INPUT	1
#define LISTEN_DONTGRAB_INPUT	2
#define LISTEN_DUMMY		3

#define FORCE_MODE_NORMAL	0
#define FORCE_MODE_MANUAL	1
#define FORCE_MODE_AUTO		2

struct _xfocus
{
	Window owner_window;		// Input focus window
	Window last_parent_window;	// Last grab parent window
	int last_focus;			// Last focus status

	int  (*get_focus_status) (struct _xfocus *p, int *forced_mode, int *focus_status);
	void (*update_events) (struct _xfocus *p, int mode);
	void (*send_button_event) (struct _xfocus *p, XEvent *event);
	void (*uninit) (struct _xfocus *p);
};

struct _xfocus* xfocus_init(void);

#endif /* _XFOCUS_H_ */
