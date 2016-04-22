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

#ifndef _XOBJECT_H_
#define _XOBJECT_H_

#include <X11/Xutil.h>

#include "types.h"

typedef struct _xobj
{
	Display *xDisplay;
	Window xWindow;

	Window (*get_app_root_win) (struct _xobj *p);
	Window (*find_win_byatom) (struct _xobj *p,char *atom);
	Window (*find_child_byatom) (struct _xobj *p, Window xwin, Atom atom);
	XClassHint* (*get_wm_class) (struct _xobj *p);
	int   (*connect) (struct _xobj *p);
	bool  (*create_win) (struct _xobj *p);
	char* (*get_wmclass_name) (struct _xobj *p);
	char* (*get_wmclass_class) (struct _xobj *p);
	bool  (*set_event_mask) (struct _xobj *p, int event_mask);
	void  (*uninit) (struct _xobj *p);
} xobj;

struct _xobj* xobj_init(void);

#endif /* _XOBJECT_H_ */
