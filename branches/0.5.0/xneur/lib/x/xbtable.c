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

#include <X11/keysym.h>

#include <stdlib.h>
#include <string.h>

#include "xnconfig.h"

#include "utils.h"

#include "xbtable.h"

static struct _xbtable btable[MAX_HOTKEYS] = { {XK_Pause, XK_Break, None}, {XK_Pause, XK_Break, XK_Control_R}, {XK_Print, XK_Sys_Req, XK_Control_R}, {XK_Pause, XK_Break, XK_Shift_R}, {XK_Scroll_Lock, XK_Scroll_Lock, XK_Alt_R}, {XK_Pause, XK_Break, XK_Alt_R} };

extern struct _xneur_config *xconfig;

static void bind_action(enum _hotkey_action action)
{
	enum _hotkey_modifier modifier	= xconfig->hotkeys[action].modifier;
	enum _hotkey_key key		= xconfig->hotkeys[action].key;

	KeySym key_modifier;
	switch (modifier)
	{
		case MODIFIER_NONE:
		{
			key_modifier = None;
			break;
		}
		case MODIFIER_SHIFT:
		{
			key_modifier = XK_Shift_R;
			break;
		}
		case MODIFIER_CONTROL:
		{
			key_modifier = XK_Control_R;
			break;
		}
		case MODIFIER_ALT:
		{
			key_modifier = XK_Alt_R;
			break;
		}
		default:
			return;
	}

	KeySym key_sym, key_sym_alt;
	switch (key)
	{
		case KEY_BREAK:
		{
			key_sym = XK_Break;
			key_sym_alt = XK_Pause;
			break;
		}
		case KEY_SCROLL_LOCK:
		{
			key_sym = XK_Scroll_Lock;
			key_sym_alt = None;
			break;
		}
		case KEY_PRINT_SCREEN:
		{
			key_sym = XK_Print;
			key_sym_alt = XK_Sys_Req;
			break;
		}
		default:
			return;
	}

	btable[action].key_sym = key_sym;
	btable[action].key_sym_alt = key_sym_alt;
	btable[action].key_modifier = key_modifier;
}

enum _hotkey_action get_manual_action(KeySym key_sym, KeySym key_modifier)
{
	enum _hotkey_action action;
	for (action = 0; action < MAX_HOTKEYS; action++)
	{
		if (btable[action].key_sym != key_sym && btable[action].key_sym_alt != key_sym)
			continue;

		if (btable[action].key_modifier == key_modifier)
			return action;
	}
	return ACTION_NONE;
}

void bind_manual_actions(void)
{
	enum _hotkey_action action;
	for (action = 0; action < MAX_HOTKEYS; action++)
		bind_action(action);
}
