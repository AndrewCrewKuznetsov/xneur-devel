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

#ifndef _XEVENT_H_
#define _XEVENT_H_

#include <X11/Xutil.h>

int get_key_state(int key);

struct _xevent
{
	Window owner_window;		// Window that recieves/sends event
	XEvent event;			// Event to process
	KeyCode backspace;		// Backspace key code
	KeyCode left;			// Left Arrow key code
	KeyCode right;			// Right Arrow key code

	KeySym (*get_cur_keysym) (struct _xevent *p);
	KeySym (*get_cur_modifier) (struct _xevent *p);

	int  (*get_next_event) (struct _xevent *p);
	void (*set_owner_window) (struct _xevent *p, Window window);
	void (*send_xkey) (struct _xevent *p, KeyCode kc, int modifiers);
	void (*send_string) (struct _xevent *p, struct _xstring *str);
	char (*get_cur_char) (struct _xevent *p);
	void (*send_backspaces) (struct _xevent *p, int n);
	void (*send_selection) (struct _xevent *p, int n);
	void (*uninit) (struct _xevent *p);
};

struct _xevent* xevent_init(void);

#endif /* _XEVENT_H_ */
