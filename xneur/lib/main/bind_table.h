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
 */

#ifndef _BIND_TABLE_H_
#define _BIND_TABLE_H_

#include <X11/XKBlib.h>

struct _bind_table
{
	KeySym key_sym;
	KeySym key_sym_shift;
	KeyCode key_code;
	int modifier_mask;
};

void bind_actions(void);
void unbind_actions(void);

void bind_user_actions(void);
void unbind_user_actions(void);

void grab_action(Window window);
void grab_user_action(Window window);

void grab_action_common(struct _bind_table btaction, Window window, Window strange_window);

//void ungrab_modifiers(Window window);

enum _hotkey_action get_action(KeySym key_sym, int mask);
int get_user_action(KeySym key_sym, int mask);


#endif /* _BIND_TABLE_H_ */
