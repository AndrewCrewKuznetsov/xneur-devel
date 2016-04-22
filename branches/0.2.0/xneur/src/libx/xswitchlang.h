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

#ifndef _XSWITCHLANG_H_
#define _XSWITCHLANG_H_

#include <X11/Xutil.h>

#include "types.h"

// Working with language switching
typedef struct _xswitchlang
{
	Display *xDisplay;
	int cur_lang;

	void (*switch_lang) (struct _xswitchlang *p, int new_lang);
	void (*print_keyboard_list) (const struct _xswitchlang *p);
	void (*set_active_keyboard) (struct _xswitchlang *p, int group);
	int  (*get_active_keyboard) (struct _xswitchlang *p);
	int  (*get_cur_lang) (struct _xswitchlang *p);
	void (*uninit) (struct _xswitchlang *p);
} xswitchlang;

struct _xswitchlang* xswitchlang_init(Display *xDisplay);

#endif /* _XSWITCHLANG_H_ */
