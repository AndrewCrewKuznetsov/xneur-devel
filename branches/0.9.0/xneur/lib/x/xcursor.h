/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  Copyright (C) 2006-2008 XNeur Team
 *
 */

#ifndef _XCURSOR_H_
#define _XCURSOR_H_
 
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef WITH_XPM

#include <X11/Xutil.h>
#include <X11/xpm.h>

#include "xnconfig.h"

struct _xcursor
{

	Pixmap bitmap;
	Pixmap bitmap_mask;
	XpmAttributes Attrs;

	GC gc;
	
	int last_layuot;

	void (*show_flag) (struct _xcursor *p, int x, int y);
	void (*hide_flag) (struct _xcursor *p);
	void (*uninit) (struct _xcursor *p);
};

#else

struct _xcursor
{
	void (*show_flag) (struct _xcursor *p, int x, int y);
	void (*hide_flag) (struct _xcursor *p);
	void (*uninit) (struct _xcursor *p);
};

#endif /* WITH_XPM */

struct _xcursor* xcursor_init(void);

#endif /* _XCURSOR_H_ */
