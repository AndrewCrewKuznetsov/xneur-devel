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

#include <stdlib.h>
#include <string.h>

#include "xwindow.h"
#include "xdefines.h"
#include "xnconfig.h"

#include "types.h"
#include "utils.h"
#include "log.h"

#include "xutils.h"

#define MAXSTR 10000

static const char *grab_ungrab[2] = {"ungrab", "grab"};

const KeySym spec_keys[] =  {   
								XK_Return, XK_Tab, XK_space, XK_slash, XK_backslash, 
								XK_question, XK_minus, XK_plus, XK_equal
							};
static const int total_spec_keys = sizeof(spec_keys) / sizeof(spec_keys[0]);

extern struct _xneur_config *xconfig;
extern struct _xwindow *main_window;

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
	if (window == main_window->flag_window)
		return;
	
	XSelectInput(main_window->display, window, event_mask);
}

void grab_button(Window window, int is_grab)
{
	if (xconfig->mouse_processing_mode != MOUSE_GRAB_ENABLE)
		return;
	
	int status;
	if (is_grab)
		status = XGrabButton(main_window->display, AnyButton, AnyModifier, window, TRUE, BUTTON_HANDLE_MASK, GrabModeSync, GrabModeAsync, None, None);
	else
		status = XUngrabButton(main_window->display, AnyButton, AnyModifier, window);

	if (status == BadCursor)
		log_message(ERROR, "Failed to %s mouse with error BadCursor", grab_ungrab[is_grab]);
	else if (status == BadValue)
		log_message(ERROR, "Failed to %s mouse with error BadValue", grab_ungrab[is_grab]);
	else if (status == BadWindow)
		log_message(ERROR, "Failed to %s mouse with error BadWindow", grab_ungrab[is_grab]);
}

void grab_key(Window window, KeyCode kc, int is_grab)
{
	int status;
	if (is_grab)
		status = XGrabKey(main_window->display, kc, AnyModifier, window, TRUE, GrabModeAsync, GrabModeAsync);
	else
		status = XUngrabKey(main_window->display, kc, AnyModifier, window);
	
	if (status == BadValue)
		log_message(ERROR, "Failed to %s keyboard with error BadValue", grab_ungrab[is_grab]);
	else if (status == BadWindow)
		log_message(ERROR, "Failed to %s keyboard with error BadWindow", grab_ungrab[is_grab]);
}

void grab_spec_keys(Window window, int is_grab)
{
	for (int i = 0; i < total_spec_keys; i++)
	{
		KeyCode kc = XKeysymToKeycode(main_window->display, spec_keys[i]);
		if (kc == 0)
			continue;
		grab_key(window, kc, is_grab);		
	}
}

void grab_keyboard(Window window, int is_grab)
{
	int status;
	if (is_grab)
		status = XGrabKey(main_window->display, AnyKey, AnyModifier, window, TRUE, GrabModeAsync, GrabModeAsync);
	else
		status = XUngrabKey(main_window->display, AnyKey, AnyModifier, window);
	
	if (status == BadValue)
		log_message(ERROR, "Failed to %s keyboard with error BadValue", grab_ungrab[is_grab]);
	else if (status == BadWindow)
		log_message(ERROR, "Failed to %s keyboard with error BadWindow", grab_ungrab[is_grab]);
}

char* get_wm_class_name(Window window)
{
	if (window == None)
		return NULL;

	Window named_window = find_window_with_atom(window, XInternAtom(main_window->display, "WM_CLASS", True));
	if (named_window == None)
		return NULL;

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
