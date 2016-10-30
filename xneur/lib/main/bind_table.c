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

#include <X11/XKBlib.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "xnconfig.h"

#include "types.h"
#include "utils.h"
#include "log.h"
#include "list_char.h"

#include "window.h"
#include "keymap.h"

#include "bind_table.h"

static struct _bind_table *ubtable;

static struct _bind_table *btable;

static const char *normal_action_names[] =	{
										"Correct/Undo correction", "Transliterate", "Change case", "Preview correction",
										"Correct last line",
										"Correct selected text", "Transliterate selected text", "Change case of selected text", "Preview correction of selected text",
	                                    "Correct clipboard text", "Transliterate clipboard text", "Change case of clipboard text", "Preview correction of clipboard text",
										"Switch to layout 1", "Switch to layout 2", "Switch to layout 3", "Switch to layout 4",
		                                "Rotate layouts", "Rotate layouts back", "Expand abbreviations", "Autocompletion confirmation",
										"Rotate autocompletion", "Block/Unblock keyboard and mouse events", "Insert date"
						};

static const char *modifier_names[] =	{"Shift", "Control", "Alt", "Super"};

extern struct _xneur_config *xconfig;
extern struct _window *main_window;

static char* hotkey_concat_bind(struct _xneur_hotkey * hotkey)
{
	char *text = (char *) malloc((24 + 1 + strlen(hotkey->key)) * sizeof(char));
	text[0] = '\0';

	int total_modifiers	= sizeof(modifier_names) / sizeof(modifier_names[0]);
	for (int i = 0; i < total_modifiers; i++)
	{
		if ((hotkey->modifiers & (0x1 << i)) == 0)
			continue;

		strcat(text, modifier_names[i]);
		strcat(text, "+");
	}

	strcat(text, hotkey->key);

	return text;
}

static int create_modifier_mask(int modifiers){
	// This function should be single point to edit modifiers
	int modifier_mask = 0;
	if (modifiers & 0x01)
		modifier_mask += 1; // Shift
	if (modifiers & 0x02)
		modifier_mask += 4; // Control
	if (modifiers & 0x04)
		modifier_mask += 8; // Alt
	if (modifiers & 0x08)
		modifier_mask += 64;// Win
	return modifier_mask;
}

static void bind_action(enum _hotkey_action action)
{
	btable[action].key_sym       = 0;
	btable[action].key_sym_shift = 0;
	btable[action].key_code      = 0;

	if (xconfig->actions[action].hotkey.key == NULL)
	{
		log_message(DEBUG, _("   No key set for action \"%s\""), _(normal_action_names[xconfig->actions[action].action]));
		return;
	}

	btable[action].modifier_mask = create_modifier_mask(xconfig->actions[action].hotkey.modifiers);

	KeySym key_sym, key_sym_shift;
	key_sym = NoSymbol;
	key_sym_shift = NoSymbol;
	main_window->keymap->get_keysyms_by_string(main_window->keymap, xconfig->actions[action].hotkey.key, &key_sym, &key_sym_shift);
	if (key_sym == NoSymbol)
		key_sym = None;
	if (key_sym_shift == NoSymbol)
		key_sym_shift = key_sym;

	btable[action].key_sym = key_sym;
	btable[action].key_sym_shift = key_sym_shift;
	btable[action].key_code = XKeysymToKeycode(main_window->display, key_sym);

	char *key = hotkey_concat_bind (&(xconfig->actions[action].hotkey));
	log_message(DEBUG, _("   Action \"%s\" with key \"%s\""), _(normal_action_names[xconfig->actions[action].action]), key);
	//log_message(ERROR, _("      KeySym %d (%d) keycode %d"), key_sym, key_sym_shift, ubtable[action].key_code);
	if ((key_sym == None) || (key_sym_shift == None))
	{
		log_message(ERROR, _("      KeySym (or with Shift modifier) is undefined!"), _(normal_action_names[action]), key);

	}
	if (key != NULL)
		free(key);
}

/*static void bind_action(enum _hotkey_action action)
{
	// Bind standart action to global config
	btable[action].key_sym       = 0;
	btable[action].key_sym_shift = 0;
	btable[action].key_code      = 0;

	if (xconfig->hotkeys[action].key == NULL)
	{
		log_message(DEBUG, _("   No key set for action \"%s\""), _(normal_action_names[action]));
		return;
	}

	btable[action].modifier_mask = create_modifier_mask(xconfig->hotkeys[action].modifiers);

	KeySym key_sym, key_sym_shift;
	key_sym = NoSymbol;
	key_sym_shift = NoSymbol;
	main_window->keymap->get_keysyms_by_string(main_window->keymap, xconfig->hotkeys[action].key, &key_sym, &key_sym_shift);
	if (key_sym == NoSymbol)
		key_sym = None;
	if (key_sym_shift == NoSymbol)
		key_sym_shift = key_sym;

	btable[action].key_sym = key_sym;
	btable[action].key_sym_shift = key_sym_shift;
	btable[action].key_code = XKeysymToKeycode(main_window->display, key_sym);

	char *key = hotkeys_concat_bind (action);
	log_message(DEBUG, _("   Action \"%s\" with key \"%s\""), _(normal_action_names[action]), key);
	//log_message(ERROR, _("      KeySym %d (%d) keycode %d mask %d"), key_sym, key_sym_shift, btable[action].key_code, btable[action].modifier_mask);
	if ((key_sym == None) || (key_sym_shift == None))
	{
		log_message(ERROR, _("      KeySym (or with Shift modifier) is undefined!"));

	}
	if (key != NULL)
		free(key);
}*/

static void bind_user_action(int action)
{
	ubtable[action].key_sym       = 0;
	ubtable[action].key_sym_shift = 0;
	ubtable[action].key_code      = 0;

	if (xconfig->user_actions[action].hotkey.key == NULL)
	{
		log_message(DEBUG, _("   No key set for action \"%s\""), xconfig->user_actions[action].name);
		return;
	}

	ubtable[action].modifier_mask = create_modifier_mask(xconfig->user_actions[action].hotkey.modifiers);

	KeySym key_sym, key_sym_shift;
	key_sym = NoSymbol;
	key_sym_shift = NoSymbol;
	main_window->keymap->get_keysyms_by_string(main_window->keymap, xconfig->user_actions[action].hotkey.key, &key_sym, &key_sym_shift);
	if (key_sym == NoSymbol)
		key_sym = None;
	if (key_sym_shift == NoSymbol)
		key_sym_shift = key_sym;

	ubtable[action].key_sym = key_sym;
	ubtable[action].key_sym_shift = key_sym_shift;
	ubtable[action].key_code = XKeysymToKeycode(main_window->display, key_sym);

	char *key = hotkey_concat_bind (&(xconfig->user_actions[action].hotkey));
	log_message(DEBUG, _("   Action \"%s\" with key \"%s\""), xconfig->user_actions[action].name, key);
	//log_message(ERROR, _("      KeySym %d (%d) keycode %d"), key_sym, key_sym_shift, ubtable[action].key_code);
	if ((key_sym == None) || (key_sym_shift == None))
	{
		log_message(ERROR, _("      KeySym (or with Shift modifier) is undefined!"));

	}
	if (key != NULL)
		free(key);
}

enum _hotkey_action get_action(KeySym key_sym, int mask)
{
	// Reset Caps and Num mask
	//mask &= ~LockMask;
	//mask &= ~Mod2Mask;
	//mask &= ~Mod3Mask;

	KeyCode kc = XKeysymToKeycode(main_window->display, key_sym);
	if (IsModifierKey(key_sym))
	{
		if (key_sym == XK_Shift_L || key_sym == XK_Shift_R)
			mask -= (1 << 0);
		if (key_sym == XK_Caps_Lock)
			mask -= (1 << 1);
		if (key_sym == XK_Control_L || key_sym == XK_Control_R)
			mask -= (1 << 2);
		if (key_sym == XK_Alt_L || key_sym == XK_Alt_R)
			mask -= (1 << 3);
		if (key_sym == XK_Meta_L || key_sym == XK_Meta_R)
			mask -= (1 << 4);
		if (key_sym == XK_Num_Lock)
			mask -= (1 << 5);
		if (key_sym == XK_Super_L || key_sym == XK_Super_R)
			mask -= (1 << 6);
		if (key_sym == XK_Hyper_L || key_sym == XK_Hyper_R || key_sym == XK_ISO_Level3_Shift)
			mask += (1 << 7);
	}

	for (int action = 0; action < xconfig->actions_count; action++)
	{
		//log_message (ERROR, "U%d---%d %d", action, ubtable[action].key_code, kc);
		//if (btable[action].key_sym != key_sym && ubtable[action].key_sym_shift != key_sym)
		//	continue;
		if (btable[action].key_code != kc)
			continue;
		if (btable[action].modifier_mask == mask)
		{
			return action;
		}
	}

	// New action storage - all together
	/*
	for (int action = 0; action < xconfig->actions_count; action++)
	{
		//log_message (ERROR, "U%d---%d %d", action, ubtable[action].key_code, kc);
		//if (ubtable[action].key_sym != key_sym && ubtable[action].key_sym_shift != key_sym)
		//      continue;
		enum _hotkey_action standard_action = xconfig->actions[action].standard_action;
		if (standard_action == ACTION_NONE)
			continue;
		if (ubtable[action].key_code != kc)
			continue;
		if (ubtable[action].modifier_mask == mask)
			return standard_action;
	}
	*/
	return ACTION_NONE;
}

void bind_actions(void)
{
	log_message(DEBUG, _("Binded hotkeys actions:"));
	btable = (struct _bind_table *) malloc(xconfig->actions_count * sizeof(struct _bind_table));
	for (int action = 0; action < xconfig->actions_count; action++)
		bind_action(action);
}

void unbind_actions(void)
{
	if (btable != NULL)
		free(btable);
	btable = NULL;
}

int get_user_action(KeySym key_sym, int mask)
{
	// Reset Caps and Num mask
	//mask &= ~LockMask;
	//mask &= ~Mod2Mask;
	//mask &= ~Mod3Mask;

	KeyCode kc = XKeysymToKeycode(main_window->display, key_sym);;	if (IsModifierKey(key_sym))
	{
		if (key_sym == XK_Shift_L || key_sym == XK_Shift_R)
			mask -= (1 << 0);
		if (key_sym == XK_Caps_Lock)
			mask -= (1 << 1);
		if (key_sym == XK_Control_L || key_sym == XK_Control_R)
			mask -= (1 << 2);
		if (key_sym == XK_Alt_L || key_sym == XK_Alt_R)
			mask -= (1 << 3);
		if (key_sym == XK_Meta_L || key_sym == XK_Meta_R)
			mask -= (1 << 4);
		if (key_sym == XK_Num_Lock)
			mask -= (1 << 5);
		if (key_sym == XK_Super_L || key_sym == XK_Super_R)
			mask -= (1 << 6);
		if (key_sym == XK_Hyper_L || key_sym == XK_Hyper_R || key_sym == XK_ISO_Level3_Shift)
			mask += (1 << 7);
	}

	for (int action = 0; action < xconfig->user_actions_count; action++)
	{
		//log_message (ERROR, "U%d---%d %d", action, ubtable[action].key_code, kc);
		//if (ubtable[action].key_sym != key_sym && ubtable[action].key_sym_shift != key_sym)
		//	continue;
		if (ubtable[action].key_code != kc)
			continue;
		if (ubtable[action].modifier_mask == mask)
		{
			return action;
		}
	}
	return -1;
}

void bind_user_actions(void)
{
	log_message(DEBUG, _("Binded hotkeys user actions:"));

	ubtable = (struct _bind_table *) malloc(xconfig->user_actions_count * sizeof(struct _bind_table));
	for (int action = 0; action < xconfig->user_actions_count; action++)
		bind_user_action(action);
}

void unbind_user_actions(void)
{
	if (ubtable != NULL)
		free(ubtable);
	ubtable = NULL;
}

void grab_action(Window window)
{
	for (enum _hotkey_action action = 0; action < MAX_HOTKEYS; action++)
	{
		grab_action_common(btable[action],window,DefaultRootWindow (main_window->display));
	}
}

void grab_user_action(Window window)
{
	for (int action = 0; action < xconfig->actions_count; action++)
	{
		grab_action_common(ubtable[action],window,window);
	}
}

void grab_action_common(struct _bind_table btaction, Window window, Window strange_window)
{
	if (btaction.key_sym == 0)
		return;

	if (IsModifierKey(btaction.key_sym))
	    return;

	XGrabKey(main_window->display,
				XKeysymToKeycode(main_window->display, btaction.key_sym),
				btaction.modifier_mask,
				window,
	         	FALSE, GrabModeAsync, GrabModeAsync);

	if (main_window->keymap->numlock_mask)
			XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btaction.key_sym),
					btaction.modifier_mask | main_window->keymap->numlock_mask,
					window,
					FALSE, GrabModeAsync, GrabModeAsync);

	if (main_window->keymap->capslock_mask)
			XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btaction.key_sym),
					btaction.modifier_mask | main_window->keymap->capslock_mask,
					window,
					FALSE, GrabModeAsync, GrabModeAsync);

	if (main_window->keymap->scrolllock_mask)
			XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btaction.key_sym),
					btaction.modifier_mask | main_window->keymap->scrolllock_mask,
					window,
					FALSE, GrabModeAsync, GrabModeAsync);

	if (main_window->keymap->numlock_mask && main_window->keymap->capslock_mask)
			XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btaction.key_sym),
					btaction.modifier_mask | main_window->keymap->numlock_mask | main_window->keymap->capslock_mask,
					window,
					FALSE, GrabModeAsync, GrabModeAsync);

	if (main_window->keymap->numlock_mask && main_window->keymap->scrolllock_mask)
			XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btaction.key_sym),
					btaction.modifier_mask | main_window->keymap->numlock_mask | main_window->keymap->scrolllock_mask,
					window,
					FALSE, GrabModeAsync, GrabModeAsync);

	if (main_window->keymap->capslock_mask && main_window->keymap->scrolllock_mask)
			XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btaction.key_sym),
					btaction.modifier_mask | main_window->keymap->capslock_mask | main_window->keymap->scrolllock_mask,
					strange_window, // Why only here?!
//					DefaultRootWindow (main_window->display),
					FALSE, GrabModeAsync, GrabModeAsync);

	if (main_window->keymap->numlock_mask && main_window->keymap->capslock_mask && main_window->keymap->scrolllock_mask)
			XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btaction.key_sym),
					btaction.modifier_mask | main_window->keymap->numlock_mask | main_window->keymap->capslock_mask | main_window->keymap->scrolllock_mask,
					window,
					FALSE, GrabModeAsync, GrabModeAsync);

}
