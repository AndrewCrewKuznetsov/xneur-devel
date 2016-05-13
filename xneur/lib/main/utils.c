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
 *  Copyright (C) 2006-2013 XNeur Team
 *
 */

#include <stdlib.h>
#include <string.h>

#include "window.h"
#include "defines.h"
#include "bind_table.h"
#include "xnconfig.h"

#include "types.h"
#include "utils.h"
#include "log.h"

#include "utils.h"

#define MAXSTR 10000

static const char *grab_ungrab[2] = {"ungrab", "grab"};

const KeySym mod_keys[] =	{
					XK_Shift_L, XK_Shift_R, XK_Caps_Lock,
					XK_Control_L, XK_Control_R,
					XK_Alt_L, XK_Alt_R, XK_Meta_L,
					XK_Num_Lock, XK_Super_L, XK_Super_R, XK_Hyper_L, XK_Hyper_R,
					XK_Mode_switch, XK_ISO_Level3_Shift, XK_Menu, XK_ISO_Prev_Group, XK_ISO_Next_Group
				};

static const int total_mod_keys = sizeof(mod_keys) / sizeof(mod_keys[0]);

extern struct _xneur_config *xconfig;
extern struct _window *main_window;

static Window get_root_window(Window window)
{
	Window root_window, parent_window, *childrens;
	unsigned int childrens_count;

	if (!XQueryTree(main_window->display, window, &root_window, &parent_window, &childrens, &childrens_count))
		return None;

	if (childrens)
		XFree(childrens);

	if (root_window == parent_window)
		return window;

	return parent_window;
}

static Window find_window_with_atom(Window window, Atom atom)
{
	Atom actual_type;
	int actual_format;
	unsigned long nitems;
	unsigned long bytes_after;
	unsigned char *data = NULL;

	int status = XGetWindowProperty(main_window->display, window, atom, 0, (MAXSTR + 3) / 4, False, AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_after, &data);
	if (status == Success)
	{
		XFree(data);
		if (data != NULL)
			return window;
	}

	Window root_window = get_root_window(window);
	if (root_window == None)
		return None;

	status = XGetWindowProperty(main_window->display, root_window, atom, 0, (MAXSTR + 3) / 4, False, AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_after, &data);
	if (status == Success)
	{
		XFree(data);
		if (data != NULL)
			return root_window;
	}

	return None;
}

void set_event_mask(Window window, int event_mask)
{
	//if (event_mask){};
	XSelectInput(main_window->display, window, event_mask);
}

void set_mask_to_window(Window window, int event_mask)
{
	if (window == None)
		return;
	
	set_event_mask(window, event_mask);
	
	unsigned int children_count;
	Window root, parent;
	Window *children;
	
	int is_same_screen = XQueryTree(main_window->display, window, &root, &parent, &children, &children_count);
	if (!is_same_screen)
		return;
	
	unsigned int i;
	for (i = 0; i < children_count; i++)
		set_mask_to_window(children[i], event_mask);
	
	XFree(children);
}

void grab_button(Window window, int is_grab)
{
	int status;
    int xi_opcode, event, error;

    if (!XQueryExtension(main_window->display, "XInputExtension", &xi_opcode, &event, &error)) 
	{
		log_message(WARNING, _("X Input extension not available."));
		if (is_grab)
		{
			status = XGrabButton(main_window->display, Button1, AnyModifier, DefaultRootWindow(main_window->display), FALSE, ButtonPressMask|ButtonReleaseMask, GrabModeSync, GrabModeAsync, None, None);
			XSync (main_window->display, FALSE);
		}
		else
			status = XUngrabButton(main_window->display, AnyButton, AnyModifier, DefaultRootWindow(main_window->display));
		
		if (status == BadCursor)
			log_message(ERROR, _("Failed to %s mouse with error BadCursor"), grab_ungrab[is_grab]);
		else if (status == BadValue)
			log_message(ERROR, _("Failed to %s mouse with error BadValue"), grab_ungrab[is_grab]);
		else if (status == BadWindow)
			log_message(ERROR, _("Failed to %s mouse with error BadWindow"), grab_ungrab[is_grab]);

		return;
    }
	
	XIEventMask mask;
	mask.deviceid = XIAllDevices;
	mask.mask_len = XIMaskLen(XI_ButtonPress);
	mask.mask = calloc(mask.mask_len, sizeof(char));
	if (is_grab)
	{

		XISetMask(mask.mask, XI_ButtonPress);
	}

    XISelectEvents(main_window->display, window, &mask, 1);

    free(mask.mask);
	
}

void grab_key(Window window, KeyCode kc, int is_grab)
{
	int status;
	if (is_grab)
		status = XGrabKey(main_window->display, kc, AnyModifier, window, TRUE, GrabModeAsync, GrabModeAsync);
	else
		status = XUngrabKey(main_window->display, kc, AnyModifier, window);

	if (status == BadValue)
		log_message(ERROR, _("Failed to %s keyboard with error BadValue"), grab_ungrab[is_grab]);
	else if (status == BadWindow)
		log_message(ERROR, _("Failed to %s keyboard with error BadWindow"), grab_ungrab[is_grab]);
}

void grab_modifier_keys(Window window, int is_grab)
{
	int i, k = 0;

	XModifierKeymap *modmap = XGetModifierMapping (main_window->display);
		
	for (i = 0; i < 8; i++) 
	{
		int j;

		for (j = 0; j < modmap->max_keypermod; j++) 
		{
			// Dirty hack for using Shift /*and Ctrl*/ on xneur
			if ((i == 0)/* || (i == 2)*/)
			{
				k++;
				continue;
			}
			
			if (modmap->modifiermap[k]) 
			{
				/*
				int keysyms_per_keycode_return;
				KeySym *keysym = XGetKeyboardMapping(main_window->display,
				modmap->modifiermap[k],
				1,
				&keysyms_per_keycode_return);
				log_message (ERROR, "Modifiers List:");
				log_message (ERROR, "%d %d %s", i, j, XKeysymToString(keysym[0]));
				XFree(keysym);
				*/

				if (is_grab)
					XGrabKey(main_window->display, modmap->modifiermap[k], AnyModifier, window, FALSE, GrabModeAsync, GrabModeAsync);
				else
					XUngrabKey(main_window->display, modmap->modifiermap[k], AnyModifier, window);	
			}
			k++;
		}
	}

	if (modmap)
		XFreeModifiermap (modmap);

	int menu_kc = XKeysymToKeycode(main_window->display, XK_Menu);
	if (is_grab)
		XGrabKey(main_window->display, menu_kc, AnyModifier, window, FALSE, GrabModeAsync, GrabModeAsync);
	else
		XUngrabKey(main_window->display, menu_kc, AnyModifier, window);
	
	return;
}
	
void grab_spec_keys(Window window, int is_grab)
{
	if (is_grab)
	{
		// Ungrab hotkeys in root window
		XUngrabKey(main_window->display, AnyKey, AnyModifier, DefaultRootWindow (main_window->display));
		// Grab all keys...
		XGrabKey(main_window->display, AnyKey, AnyModifier, window, FALSE, GrabModeAsync, GrabModeAsync);
		// ...without ModKeys.
		grab_modifier_keys(window, FALSE);
	}
	else
	{
		// Ungrab all keys in app window...
		XUngrabKey(main_window->display, AnyKey, AnyModifier, window);
		// ... and with hotkeys
		XUngrabKey(main_window->display, AnyKey, AnyModifier, DefaultRootWindow (main_window->display));
		grab_manual_action();
		grab_user_action();
		// ...without ModKeys.
		grab_modifier_keys(DefaultRootWindow (main_window->display), FALSE);
	}
}

void grab_keyboard(Window window, int is_grab)
{
	int status;
	if (is_grab)
		status = XGrabKeyboard(main_window->display, window, FALSE, GrabModeAsync, GrabModeAsync, CurrentTime);
	else
		status = XUngrabKeyboard(main_window->display, CurrentTime);

	if (status == BadValue)
		log_message(ERROR, _("Failed to %s keyboard with error BadValue"), grab_ungrab[is_grab]);
	else if (status == BadWindow)
		log_message(ERROR, _("Failed to %s keyboard with error BadWindow"), grab_ungrab[is_grab]);
}

unsigned char *get_win_prop(Window window, Atom atom, long *nitems, Atom *type, int *size) 
{
	Atom actual_type;
	int actual_format;
	unsigned long _nitems;
	unsigned long bytes_after; /* unused */
	unsigned char *prop;
	int status;

	status = XGetWindowProperty(main_window->display, window, atom, 0, (~0L),
                              FALSE, AnyPropertyType, &actual_type,
                              &actual_format, &_nitems, &bytes_after,
                              &prop);
	if ((status == BadWindow) || (status != Success)) 
		return NULL;

	
	*nitems = _nitems;
	*type = actual_type;
	*size = actual_format;
	return prop;
}

char* get_wm_class_name(Window window)
{
	if (window == None)
		return NULL;

	Window named_window = find_window_with_atom(window, XInternAtom(main_window->display, "WM_CLASS", True));
	if (named_window == None)
	{

		named_window = find_window_with_atom(window, XInternAtom(main_window->display, "WM_NAME", True));

		if (named_window == None)
			return NULL;
		
		Atom type;
		int size;
		long nitems;

		Atom request = XInternAtom(main_window->display, "WM_NAME", False);
		unsigned char *data = get_win_prop(named_window, request, &nitems, &type, &size);

		if (nitems > 0) 
			return (char *)data;

		return NULL;	
	}
	
	XClassHint *wm_class = XAllocClassHint();

	if (!XGetClassHint(main_window->display, named_window, wm_class))
	{
		XFree(wm_class);
		return NULL;
	}

	char *string = strdup(wm_class->res_class);

	XFree(wm_class->res_class);
	XFree(wm_class->res_name);
	XFree(wm_class);

	return string;
}
