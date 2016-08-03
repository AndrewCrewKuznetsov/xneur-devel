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

static struct _bind_table btable[MAX_HOTKEYS] =	{
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
							{0, 0, 0, 0},
	                        {0, 0, 0, 0},
		                    {0, 0, 0, 0},
							{0, 0, 0, 0}
						};

static const char *normal_action_names[] =	{
										"Correct/Undo correction", "Transliterate", "Change case", "Preview correction", 
										"Correct last line", 
										"Correct selected text", "Transliterate selected text", "Change case of selected text", "Preview correction of selected text",
	                                    "Correct clipboard text", "Transliterate clipboard text", "Change case of clipboard text", "Preview correction of clipboard text",
										"Switch to layout 1", "Switch to layout 2", "Switch to layout 3", "Switch to layout 4",
		                                "Rotate layouts", "Rotate layouts back", "Expand abbreviations", "Autocompletion confirmation", 
										"Rotate Autocompletion", "Block/Unblock keyboard and mouse events", "Insert date"
						};

static const char *modifier_names[] =	{"Shift", "Control", "Alt", "Super"};

extern struct _xneur_config *xconfig;
extern struct _window *main_window;

static char* hotkeys_concat_bind(int action)
{
	char *text = (char *) malloc((24 + 1 + strlen(xconfig->hotkeys[action].key)) * sizeof(char));
	text[0] = '\0';

	int total_modifiers	= sizeof(modifier_names) / sizeof(modifier_names[0]);
	for (int i = 0; i < total_modifiers; i++)
	{
		if ((xconfig->hotkeys[action].modifiers & (0x1 << i)) == 0)
			continue;

		strcat(text, modifier_names[i]);
		strcat(text, "+");
	}

	strcat(text, xconfig->hotkeys[action].key);
	
	return text;
}

static char* user_hotkeys_concat_bind(int action)
{
	char *text = (char *) malloc((24 + 1 + strlen(xconfig->actions[action].hotkey.key)) * sizeof(char));
	text[0] = '\0';

	int total_modifiers	= sizeof(modifier_names) / sizeof(modifier_names[0]);
	for (int i = 0; i < total_modifiers; i++)
	{
		if ((xconfig->actions[action].hotkey.modifiers & (0x1 << i)) == 0)
			continue;

		strcat(text, modifier_names[i]);
		strcat(text, "+");
	}

	strcat(text, xconfig->actions[action].hotkey.key);
	
	return text;
}

static void bind_action(enum _hotkey_action action)
{
	btable[action].modifier_mask	= 0;
	btable[action].key_sym		= 0;
	btable[action].key_sym_shift	= 0;
	btable[action].key_code =0;

	if (xconfig->hotkeys[action].key == NULL)
	{
		log_message(DEBUG, _("   No key set for action \"%s\""), _(normal_action_names[action]));
		return;
	}

	int modifiers = xconfig->hotkeys[action].modifiers;
	if (modifiers & 0x01)
		btable[action].modifier_mask = btable[action].modifier_mask + 1;	// Shift
	if (modifiers & 0x02)
		btable[action].modifier_mask = btable[action].modifier_mask + 4;	// Control
	if (modifiers & 0x04)
		btable[action].modifier_mask = btable[action].modifier_mask + 8;	// Alt
	if (modifiers & 0x08)
		btable[action].modifier_mask = btable[action].modifier_mask + 64;	// Win

	
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
}

static void bind_user_action(int action)
{
	ubtable[action].modifier_mask	= 0;
	ubtable[action].key_sym		= 0;
	ubtable[action].key_sym_shift	= 0;
	ubtable[action].key_code		= 0;
	
	if (xconfig->actions[action].hotkey.key == NULL)
	{
		log_message(DEBUG, _("   No key set for action \"%s\""), xconfig->actions[action].name);
		return;
	}

	int action_modifiers = xconfig->actions[action].hotkey.modifiers;

	if (action_modifiers & 0x01)
		ubtable[action].modifier_mask = ubtable[action].modifier_mask + 1;	// Shift
	if (action_modifiers & 0x02)
		ubtable[action].modifier_mask = ubtable[action].modifier_mask + 4;	// Control
	if (action_modifiers & 0x04)
		ubtable[action].modifier_mask = ubtable[action].modifier_mask + 8;	// Alt
	if (action_modifiers & 0x08)
		ubtable[action].modifier_mask = ubtable[action].modifier_mask + 64;	// Win

	KeySym key_sym, key_sym_shift;
	key_sym = NoSymbol;
	key_sym_shift = NoSymbol;
	main_window->keymap->get_keysyms_by_string(main_window->keymap, xconfig->actions[action].hotkey.key, &key_sym, &key_sym_shift);
	if (key_sym == NoSymbol)
		key_sym = None;
	if (key_sym_shift == NoSymbol)
		key_sym_shift = key_sym;

	ubtable[action].key_sym = key_sym;
	ubtable[action].key_sym_shift = key_sym_shift;
	ubtable[action].key_code = XKeysymToKeycode(main_window->display, key_sym);
	
	char *key = user_hotkeys_concat_bind (action);
	log_message(DEBUG, _("   Action \"%s\" with key \"%s\""), xconfig->actions[action].name, key);
	//log_message(ERROR, _("      KeySym %d (%d) keycode %d"), key_sym, key_sym_shift, ubtable[action].key_code);
	if ((key_sym == None) || (key_sym_shift == None))
	{
		log_message(ERROR, _("      KeySym (or with Shift modifier) is undefined!"), _(normal_action_names[action]), key);
	
	}
	if (key != NULL)
		free(key);
}

enum _hotkey_action get_manual_action(KeySym key_sym, int mask)
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
	
	for (enum _hotkey_action action = 0; action < MAX_HOTKEYS; action++)
	{
	//log_message (ERROR, "%d---%d %d", action, btable[action].key_code, kc);
		//if (btable[action].key_sym != key_sym && btable[action].key_sym_shift != key_sym)
		//	continue;
		if (btable[action].key_code != kc)
			continue;
		if (btable[action].modifier_mask == mask)
		{
			return action;
		}
	}
	return ACTION_NONE;
}

void bind_manual_actions(void)
{
	log_message(DEBUG, _("Binded hotkeys actions:"));
	for (enum _hotkey_action action = 0; action < MAX_HOTKEYS; action++)
		bind_action(action);
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
	
	for (int action = 0; action < xconfig->actions_count; action++)
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

	ubtable = (struct _bind_table *) malloc(xconfig->actions_count * sizeof(struct _bind_table));
	for (int action = 0; action < xconfig->actions_count; action++)
		bind_user_action(action);
}

void unbind_user_actions(void)
{
	free(ubtable);
}

void grab_manual_action(Window window)
{
	for (enum _hotkey_action action = 0; action < MAX_HOTKEYS; action++)
	{
		if (btable[action].key_sym == 0)
			continue;
		
		if (IsModifierKey(btable[action].key_sym))
		{
		    continue;
		}
		
		XGrabKey(main_window->display, 
					XKeysymToKeycode(main_window->display, btable[action].key_sym), 
					btable[action].modifier_mask, 
						window, 
		         	FALSE, GrabModeAsync, GrabModeAsync);
	
		if (main_window->keymap->numlock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btable[action].key_sym), 
						btable[action].modifier_mask | main_window->keymap->numlock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);

		if (main_window->keymap->capslock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btable[action].key_sym), 
						btable[action].modifier_mask | main_window->keymap->capslock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);

		if (main_window->keymap->scrolllock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btable[action].key_sym), 
						btable[action].modifier_mask | main_window->keymap->scrolllock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);

		if (main_window->keymap->numlock_mask && main_window->keymap->capslock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btable[action].key_sym), 
						btable[action].modifier_mask | main_window->keymap->numlock_mask | main_window->keymap->capslock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);

		if (main_window->keymap->numlock_mask && main_window->keymap->scrolllock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btable[action].key_sym), 
						btable[action].modifier_mask | main_window->keymap->numlock_mask | main_window->keymap->scrolllock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);

		if (main_window->keymap->capslock_mask && main_window->keymap->scrolllock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btable[action].key_sym), 
						btable[action].modifier_mask | main_window->keymap->capslock_mask | main_window->keymap->scrolllock_mask,
						DefaultRootWindow (main_window->display), 
						FALSE, GrabModeAsync, GrabModeAsync);

		if (main_window->keymap->numlock_mask && main_window->keymap->capslock_mask && main_window->keymap->scrolllock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, btable[action].key_sym), 
						btable[action].modifier_mask | main_window->keymap->numlock_mask | main_window->keymap->capslock_mask | main_window->keymap->scrolllock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);
	}
}

void grab_user_action(Window window)
{
	for (int action = 0; action < xconfig->actions_count; action++)
	{
		if (ubtable[action].key_sym == 0)
			continue;

		if (IsModifierKey(ubtable[action].key_sym))
		    continue;
		    
		XGrabKey(main_window->display, 
					XKeysymToKeycode(main_window->display, ubtable[action].key_sym), 
					ubtable[action].modifier_mask, 
					window, 
		         	FALSE, GrabModeAsync, GrabModeAsync);
		
		if (main_window->keymap->numlock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, ubtable[action].key_sym), 
						ubtable[action].modifier_mask | main_window->keymap->numlock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);

		if (main_window->keymap->capslock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, ubtable[action].key_sym), 
						ubtable[action].modifier_mask | main_window->keymap->capslock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);

		if (main_window->keymap->scrolllock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, ubtable[action].key_sym), 
						ubtable[action].modifier_mask | main_window->keymap->scrolllock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);

		if (main_window->keymap->numlock_mask && main_window->keymap->capslock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, ubtable[action].key_sym), 
						ubtable[action].modifier_mask | main_window->keymap->numlock_mask | main_window->keymap->capslock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);

		if (main_window->keymap->numlock_mask && main_window->keymap->scrolllock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, ubtable[action].key_sym), 
						ubtable[action].modifier_mask | main_window->keymap->numlock_mask | main_window->keymap->scrolllock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);

		if (main_window->keymap->capslock_mask && main_window->keymap->scrolllock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, ubtable[action].key_sym), 
						ubtable[action].modifier_mask | main_window->keymap->capslock_mask | main_window->keymap->scrolllock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);

		if (main_window->keymap->numlock_mask && main_window->keymap->capslock_mask && main_window->keymap->scrolllock_mask)
				XGrabKey (main_window->display, XKeysymToKeycode(main_window->display, ubtable[action].key_sym), 
						ubtable[action].modifier_mask | main_window->keymap->numlock_mask | main_window->keymap->capslock_mask | main_window->keymap->scrolllock_mask,
						window, 
						FALSE, GrabModeAsync, GrabModeAsync);
	}		
}
