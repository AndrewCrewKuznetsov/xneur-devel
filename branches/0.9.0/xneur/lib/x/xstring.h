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

#ifndef _XSTRING_H_
#define _XSTRING_H_

#include <X11/Xutil.h>

struct _xstring
{
	char *content;		// String itself
	KeyCode *keycode;	// Array of string chars keycodes
	int *keycode_modifiers;	// Array of string chars keycodes modifiers

	int cur_size;		// Current size of content, keycode, keycodeModifiers fields
	int cur_pos;		// Current filled size

	void (*clear) (struct _xstring *p);
	void (*savelog) (struct _xstring *p, char *path, Window window);
	int  (*is_space_last) (struct _xstring *p);
	void (*set_key_code) (struct _xstring *p, int group);
	void (*set_content) (struct _xstring *p, const char *new_content);
	void (*changecase_content) (struct _xstring *p);
	void (*add_symbol) (struct _xstring *p, char sym, KeyCode keycode, int modifier);
	void (*del_symbol) (struct _xstring *p);
	char*(*get_utf_string) (struct _xstring *p);
	void (*uninit) (struct _xstring *p);
};

struct _xstring* xstring_init(void);

#endif /* _XSTRING_H_ */
