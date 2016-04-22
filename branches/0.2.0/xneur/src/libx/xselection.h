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

#ifndef _XSELECTION_H_
#define _XSELECTION_H_

#include "types.h"

#include <X11/Xutil.h>

typedef struct _xselection
{
	struct _xobj *owner;
	Atom selection;
	Atom target;
	
	XSelectionEvent* (*wait_for_notify) (struct _xselection *p);
	char* (*get_selected_text) (struct _xselection *p);
	char* (*get_to_string) (struct _xselection *p, XSelectionEvent *xselt);
	void  (*request_to_convert) (struct _xselection *p);
	void  (*uninit) (struct _xselection *p);
} xselection;

struct _xselection* xselection_init(Display *ManagedDisplay, Window ManagedWindow);

#endif /* _XSELECTION_H_ */
