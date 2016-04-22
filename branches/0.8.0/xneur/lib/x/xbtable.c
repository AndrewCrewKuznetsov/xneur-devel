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
#include <stdio.h>

#include "xnconfig.h"

#include "utils.h"
#include "log.h"
#include "types.h"

#include "xkeymap.h"

#include "xbtable.h"

static struct _xbtable btable[MAX_HOTKEYS] = { {XK_Pause, XK_Break, 0}, 
												{XK_Pause, XK_Break, 4}, 
												{XK_Print, XK_Sys_Req, 4}, 
												{XK_Pause, XK_Break, 1}, 
												{XK_Scroll_Lock, XK_Scroll_Lock, 8}, 
												{XK_Pause, XK_Break, 8},
												{XK_q, XK_Q, 4},
												{XK_w, XK_W, 4},
												{XK_e, XK_E, 4},
												{XK_r, XK_R, 4}};
static const char *normal_action_names[] = {"Change Last Word", "Change Last String", "Change Mode", 
									"Change Selected", "Translit Selected", "Changecase Selected",
									"Enable Layout 1", "Enable Layout 2", "Enable Layout 3", "Enable Layout 4"};
extern struct _xneur_config *xconfig;
	
static void bind_action(enum _hotkey_action action)
{
	int temp_mask = btable[action].modifier_mask;
	btable[action].modifier_mask = 0;
	
	if (xconfig->hotkeys[action].modifier1 != NULL)
		btable[action].modifier_mask = btable[action].modifier_mask + 1; // Shift
	if (xconfig->hotkeys[action].modifier2 != NULL)
		btable[action].modifier_mask = btable[action].modifier_mask + 4; // Control
	if (xconfig->hotkeys[action].modifier3 != NULL)
		btable[action].modifier_mask = btable[action].modifier_mask + 8; // Alt
	if (xconfig->hotkeys[action].modifier4 != NULL)
		btable[action].modifier_mask = btable[action].modifier_mask + 64; // Super
	
	if (!(btable[action].modifier_mask & ControlMask) && !(btable[action].modifier_mask & Mod1Mask) 
				&& !(btable[action].modifier_mask & ShiftMask) && !(btable[action].modifier_mask & Mod4Mask))
		btable[action].modifier_mask = temp_mask;
	
	if (xconfig->hotkeys[action].key != NULL) 
	{
		KeySym key_sym, key_sym_shift;
		get_keysyms_by_string(xconfig->hotkeys[action].key, &key_sym, &key_sym_shift);
		if (key_sym == NoSymbol) 
			key_sym = None;
		if (key_sym_shift == NoSymbol)
			key_sym_shift = None;

		btable[action].key_sym = key_sym;
		btable[action].key_sym_shift = key_sym_shift;
	}
	log_message(DEBUG, "    Action \"%s\" with mod_mask %d and key \"%s (%s)\"",
						normal_action_names[action],
						btable[action].modifier_mask,
						XKeysymToString(btable[action].key_sym),
						XKeysymToString(btable[action].key_sym_shift));
}

enum _hotkey_action get_manual_action(KeySym key_sym, int mask)
{
	enum _hotkey_action action;

	for (action = 0; action < MAX_HOTKEYS; action++)
	{
		if (btable[action].key_sym != key_sym && btable[action].key_sym_shift != key_sym)
			continue;

		if (btable[action].modifier_mask == mask)
			return action;
	}
	return ACTION_NONE;
}

void bind_manual_actions(void)
{
	enum _hotkey_action action;
	
	log_message(DEBUG, "Binded hotkeys actions (mod_mask = \"Shift (1)\"+\"Ctrl (4)\"+\"Alt (8)\"+\"Win(Super) (64)\"):");
	for (action = 0; action < MAX_HOTKEYS; action++)
		bind_action(action);
}
