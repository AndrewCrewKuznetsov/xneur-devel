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
 *  (c) XNeur Team 2007
 *
 */
 
#ifndef _XKEYMAP_H_
#define _XKEYMAP_H_

#include <X11/Xutil.h>
 
struct _xkeymap_alpha
{
	char *lower_sym;
	KeySym *lower_keysym;
	
	char *upper_sym;
	KeySym *upper_keysym;
};

struct _xkeymap
{
	int   latin_group;
	int   latin_group_mask;				// Define latin group mask
	KeySym *keymap;
	
	struct _xkeymap_alpha **alphabet;
	int min_keycode;
	int max_keycode;
	int total_key_arrays;
	
	char* (*keycode_to_symbol)(struct _xkeymap *p, KeyCode kc, int group, int state);
	char (*get_ascii)(struct _xkeymap *p, const char *sym);
	char (*get_cur_ascii_char) (struct _xkeymap *p, XEvent e);
	void (*convert_text_to_ascii)(struct _xkeymap *p, char *text);
	void (*char_to_keycode)(struct _xkeymap *p, char ch, KeyCode *kc, int *modifier);
	void (*store_keymaps)(struct _xkeymap *p, int group);
	void (*uninit) (struct _xkeymap *p);
};

struct _xkeymap *xkeymap_init(void);

#endif /* _XKEYMAP_H_ */
