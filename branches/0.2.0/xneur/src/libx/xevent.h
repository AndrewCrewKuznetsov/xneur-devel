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
 *  (c) Crew IT Research Labs 2004-2006
 *
 */

#ifndef _XEVENT_H_
#define _XEVENT_H_

#include <X11/Xutil.h>

#include "types.h"

typedef struct _xevent
{
	struct _xobj *communicator;	// xObject that recieves/sends event
	XEvent event;			// Event to process

	Window (*get_event_win) (struct _xevent *p);
	KeySym (*get_cur_keysym) (struct _xevent *p);
	KeySym (*get_cur_modifier) (struct _xevent *p);
	void (*set_communicator) (struct _xevent *p, struct _xobj *communicator);
	void (*send_xkey) (struct _xevent *p, KeyCode kc, int modifiers);
	void (*send_convert_string) (struct _xevent *p, struct _xstr *str, struct _xswitchlang *swlng, int lang);
	void (*send_string) (struct _xevent *p, struct _xstr *str);
	bool (*check_key_press) (struct _xevent *p);
	bool (*is_native) (struct _xevent *p);
	char (*get_cur_char) (struct _xevent *p);
	void (*del_nchrbck) (struct _xevent *p, int n);
	void (*uninit) (struct _xevent *p);
} xevent;

struct _xevent* xevent_init(void);

#endif /* _XEVENT_H_ */
