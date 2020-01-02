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
 *  Copyright (C) 2006-2016 XNeur Team
 *
 */

#include <stdlib.h>
#include <string.h>
#include <X11/extensions/XInput2.h>

#include "window.h"
#include "defines.h"
#include "bind_table.h"
#include "xnconfig.h"

#include "types.h"
#include "utils.h"
#include "log.h"

#include "utils.h"

#define MAXSTR 10000

extern struct _xneur_config *xconfig;
extern struct _window *main_window;

Bool is_modifier(KeySym key_sym)
{
	XModifierKeymap *modmap = XGetModifierMapping(main_window->display);
	if (modmap == NULL) {
		log_message(ERROR, _("Can't get modifier mapping. Modifier keys are not ignored"));
		return False;
	}

	KeyCode key_code = XKeysymToKeycode(main_window->display, key_sym);

	int size = modmap->max_keypermod * 8;
	for (int i = 0; i < size; ++i) {
		if (key_code == modmap->modifiermap[i]) {
			return True;
		}
	}

	XFreeModifiermap(modmap);

	return False;
}

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

void grab_button(int is_grab)
{
	if (is_grab)
	{
		XIEventMask mask;
		mask.deviceid = XIAllMasterDevices;
		mask.mask_len = XIMaskLen(XI_RawButtonPress);
		mask.mask = (unsigned char *)calloc(mask.mask_len, sizeof(unsigned char));
		XISetMask(mask.mask, XI_RawButtonPress);
		XISelectEvents(main_window->display, DefaultRootWindow(main_window->display), &mask, 1);
		free(mask.mask);
	}
	else
	{
		XIEventMask mask;
		mask.deviceid = XIAllMasterDevices;
		mask.mask_len = XIMaskLen(XI_RawButtonPress);
		mask.mask = (unsigned char *)calloc(mask.mask_len, sizeof(unsigned char));
		XISetMask(mask.mask, 0);
		XISelectEvents(main_window->display, DefaultRootWindow(main_window->display), &mask, 1);
		free(mask.mask);
	}
}

void grab_all_keys(Window window, int is_grab)
{
	if (is_grab)
	{
		// Grab all keys...
		if (has_x_input_extension) {
			XIEventMask mask;
			mask.deviceid = XIAllDevices;
			mask.mask_len = XIMaskLen(XI_KeyPress)+
							XIMaskLen(XI_KeyRelease);
			mask.mask = (unsigned char *)calloc(mask.mask_len, sizeof(unsigned char));
			XISetMask(mask.mask, XI_KeyPress);
			XISetMask(mask.mask, XI_KeyRelease);
			XISelectEvents(main_window->display, DefaultRootWindow(main_window->display), &mask, 1);
			free(mask.mask);
		}
		else {
			XGrabKey(main_window->display, AnyKey, AnyModifier, window, FALSE, GrabModeAsync, GrabModeAsync);
		}
	}
	else
	{
		if (has_x_input_extension) {
			XIEventMask mask;
			mask.deviceid = XIAllMasterDevices;
			mask.mask_len = XIMaskLen(XI_KeyPress);
			mask.mask = (unsigned char *)calloc(mask.mask_len, sizeof(unsigned char));
			XISetMask(mask.mask, 0);
			XISelectEvents(main_window->display, DefaultRootWindow(main_window->display), &mask, 1);
			free(mask.mask);
		}
		else {
			// Ungrab all keys in app window...
			XUngrabKey(main_window->display, AnyKey, AnyModifier, window);
		}
	}

	XSelectInput(main_window->display, window, FOCUS_CHANGE_MASK);
}

unsigned char *get_win_prop(Window window, Atom atom, unsigned long *nitems)
{
	Atom actual_type;
	int actual_format;
	unsigned long bytes_after; /* unused */
	unsigned char *prop;
	int status;

	status = XGetWindowProperty(main_window->display, window, atom, 0, (~0L),
	                            False, AnyPropertyType, &actual_type,
	                            &actual_format, nitems, &bytes_after,
	                            &prop);
	if (status != Success)
		return NULL;

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

		unsigned long nitems;

		Atom request = XInternAtom(main_window->display, "WM_NAME", False);
		unsigned char *data = get_win_prop(named_window, request, &nitems);

		if (nitems > 0 && data != NULL) {
			// Returned string is freed by `free` function, but result from `get_win_prop` must be freed by `XFree` function
			// so just make copy
			char* result = strdup((char *)data);
			XFree(data);
			return result;
		}

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

void click_key(KeySym keysym)
{
	KeyCode keycode = XKeysymToKeycode(main_window->display, keysym);

    XTestFakeKeyEvent(main_window->display, keycode, TRUE, 0); // key press event
    XTestFakeKeyEvent(main_window->display, keycode ,FALSE, 0); // key release event
    XFlush(main_window->display);

    return;
}

void toggle_lock(int mask, int state)
{
	int xkb_opcode, xkb_event, xkb_error;
	int xkb_lmaj = XkbMajorVersion;
	int xkb_lmin = XkbMinorVersion;
	if (XkbLibraryVersion(&xkb_lmaj, &xkb_lmin) && XkbQueryExtension(main_window->display, &xkb_opcode, &xkb_event, &xkb_error, &xkb_lmaj, &xkb_lmin))
	{
		/*int status = */XkbLockModifiers (main_window->display, XkbUseCoreKbd, mask, state);
		//log_message(TRACE, _("Set lock state: %d %d, status: %d"), mask, state, status);
	}
}
