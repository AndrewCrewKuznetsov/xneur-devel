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
 *  Copyright (C) 2006-2012 XNeur Team
 *
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>

void   set_event_mask(Window window, int event_mask);
void   set_mask_to_window(Window window, int event_mask);
char*  get_wm_class_name(Window window);
void   grab_button(Window window, int is_grab);
void   grab_keyboard(Window window, int is_grab);
void   grab_key(Window window, KeyCode kc, int is_grab);
void   grab_spec_keys(Window window, int is_grab);
unsigned char *get_win_prop(Window window, Atom atom, long *nitems, Atom *type, int *size); 

#endif /* _UTILS_H_ */
