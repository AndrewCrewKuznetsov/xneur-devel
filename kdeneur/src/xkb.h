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
 *  Copyright (C) 2006-2010 XNeur Team
 *
 * file from gxneur author Andrew Crew Kuznetsov
 */


#ifndef _XKBGXNEUR_H_
#define _XKBGGXNEUR_H_

#include <X11/XKBlib.h>

int get_active_kbd_group(Display *dpy);
int get_kbd_group_count(Display *dpy);
int set_next_kbd_group(Display *dpy);

#endif /* _XKB_H_ */
