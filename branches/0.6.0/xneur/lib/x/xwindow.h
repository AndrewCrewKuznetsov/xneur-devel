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

#ifndef _XWINDOW_H_
#define _XWINDOW_H_

#include <X11/Xutil.h>

#include "xkeymap.h"

struct _xwindow
{
	Display *display;
	Window window;
	
	struct _xkeymap *xkeymap;
	
	int  (*create) (struct _xwindow *p);
	void (*destroy) (struct _xwindow *p);
	void (*move_window) (struct _xwindow *p, int x, int y);
};

struct _xwindow* xwindow_init(void);

#endif /* _XWINDOW_H_ */
