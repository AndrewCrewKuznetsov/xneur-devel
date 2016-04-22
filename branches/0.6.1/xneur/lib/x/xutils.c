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

#include <stdlib.h>
#include <string.h>

#include "xwindow.h"
#include "xdefines.h"

#include "utils.h"
#include "types.h"
#include "log.h"

#include "xutils.h"

extern struct _xwindow *main_window;

static Window get_root_window(Window window)
{
	Window backup_window = window;

	while (TRUE)
	{
		Window root_window, parent_window, *childrens;
		unsigned int childrens_count;

		if (!XQueryTree(main_window->display, window, &root_window, &parent_window, &childrens, &childrens_count))
			return None;

		if (childrens)
			XFree(childrens);

		if (parent_window == root_window)
			return backup_window;

		backup_window = window;
		window = parent_window;
	}
	return backup_window;
}

static Window find_child_window_byatom(Window window, Atom atom)
{
	Atom type = None;
	Window *childrens;
	unsigned int childrens_count;
	Window root_window = 0, parent_window = 0;

	if (!XQueryTree(main_window->display, window, &root_window, &parent_window, &childrens, &childrens_count))
		return None;

	Window child_window = None;
	int i;
	for (i = 0; i < (int) childrens_count; i++)
	{
		int format;
		unsigned long dummy;
		unsigned char *data = NULL;

		XGetWindowProperty(main_window->display, childrens[i], atom, 0, 0, False, AnyPropertyType, &type, &format, &dummy, &dummy, &data);

		if (data)
			XFree(data);

		if (type)
		{
			child_window = childrens[i];
			break;
		}
	}

	if (child_window == None)
	{
		for (i = 0; i < (int) childrens_count; i++)
		{
			child_window = find_child_window_byatom(childrens[i], atom);
			if (child_window != None)
				break;
		}
	}

	if (childrens)
		XFree(childrens);
	return child_window;
}

static Window find_window_byatom(Window window, char *atom)
{
	Display *display = main_window->display;

	if (RootWindow(display, DefaultScreen(display)) == window)
		return window;

	Atom xi_atom = XInternAtom(display, atom, True);
	if (!xi_atom)
		return window;

	Window root_window = get_root_window(window);
	if (root_window == None)
		return None;

	Atom type = None;
	int format;
	unsigned long dummy;
	unsigned char *data = NULL;
	XGetWindowProperty(display, root_window, xi_atom, 0, 0, False, AnyPropertyType, &type, &format, &dummy, &dummy, &data);

	if (data)
		XFree(data);

	if (type)
		return root_window;

	Window child_window = find_child_window_byatom(root_window, xi_atom);
	if (child_window != None)
		return child_window;
	return window;
}

static XClassHint* get_wm_class(Window window)
{
	Window named_window = find_window_byatom(window, "WM_CLASS");

	XClassHint *wm_class = XAllocClassHint();
	if (XGetClassHint(main_window->display, named_window, wm_class))
		return wm_class;

	XFree(wm_class->res_class);
	XFree(wm_class->res_name);
	XFree(wm_class);
	return NULL;
}

void set_event_mask(Window window, int event_mask)
{
	XSelectInput(main_window->display, window, event_mask);
}

void grab_button(Window window, int is_grab)
{
	int status;
	if (is_grab)
		status = XGrabButton(main_window->display, AnyButton, AnyModifier, window, TRUE, BUTTON_HANDLE_MASK, GrabModeAsync, GrabModeAsync, None, None);
	else
		status = XUngrabButton(main_window->display, AnyButton, AnyModifier, window);

	if (status == BadCursor)
		log_message(ERROR, "Fail grab/ungrab (%d) mouse with error BadCursor", is_grab);
	else if (status == BadValue)
		log_message(ERROR, "Fail grab/ungrab (%d) mouse with error BadValue", is_grab);
	else if (status == BadWindow)
		log_message(ERROR, "Fail grab/ungrab (%d) mouse with error BadWindow", is_grab);
}

char* get_wm_class_name(Window window)
{
	XClassHint *wm_class = get_wm_class(window);
	if (wm_class == NULL)
		return NULL;

	char *string = strdup(wm_class->res_class);

	XFree(wm_class->res_class);
	XFree(wm_class->res_name);
	XFree(wm_class);

	return string;
}
