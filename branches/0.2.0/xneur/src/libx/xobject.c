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
 *  (c) Crew IT Research Labs 2004-2006
 *
 */

#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "log.h"

#include "xobject.h"

int xobj_connect(struct _xobj *p)
{
	p->xDisplay = XOpenDisplay(NULL);
	if (!p->xDisplay)
	{
		log_message(ERROR, "Cannot open display");
		return ERROR;
	}
	return SUCCESS;
}

bool xobj_create_win(struct _xobj *p)
{
	p->xWindow = XCreateSimpleWindow(p->xDisplay, DefaultRootWindow(p->xDisplay), 0, 0, 1, 1, 0, 0, 0);
	if (p->xWindow)
		return TRUE;
	return FALSE;
}

Window xobj_get_app_root_win(struct _xobj *p)
{
	Window window = p->xWindow;
	Window backup_window = window;
	Window root_window;
	Window parent_window;
	Window *childrens;
	unsigned int nchildren;
  
	while (1)
	{
		if (!XQueryTree(p->xDisplay, window, &root_window, &parent_window, &childrens, &nchildren))
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

Window xobj_find_win_byatom(struct _xobj *p, char *atom)
{
	Window window = p->xWindow;
	Display *display = p->xDisplay;

	if (RootWindow(display, DefaultScreen(display)) == window)
		return window;

	Atom xi_atom = XInternAtom(display, atom, True);
	if (!xi_atom)
		return window;

	Window app_root_window = p->get_app_root_win(p);
	if (app_root_window == None)
		return None;

	Atom type = None;
	int format;
	unsigned long nitems, after;
	unsigned char *data = NULL;
	XGetWindowProperty(display, app_root_window, xi_atom, 0, 0, False, AnyPropertyType, &type, &format, &nitems, &after, &data);

	if (data)
		XFree(data);

	if (type)
		return app_root_window;

	Window tmp_window = p->find_child_byatom(p, app_root_window, xi_atom);
	if (tmp_window != None)
		return tmp_window;
	return window;
}

Window xobj_find_child_byatom(struct _xobj *p, Window xwin, Atom atom)
{
	Atom type = None;
	Window *childrens;
	unsigned int nchildren;
	Window root_window, parent_window;

	if (!XQueryTree(p->xDisplay, xwin, &root_window, &parent_window, &childrens, &nchildren))
		return None;

	Window tmp_window = None;
	unsigned int i;
	for (i = 0; i < nchildren; i++)
	{
		int format;
		unsigned long nitems, after;
		unsigned char *data = NULL;

		XGetWindowProperty(p->xDisplay, childrens[i], atom, 0, 0, False, AnyPropertyType, &type, &format, &nitems, &after, &data);

		if (data)
			XFree(data);

		if (type)
		{
			tmp_window = childrens[i];
			break;
		}
	}

	if (tmp_window == None)
	{
		for (i = 0; i < nchildren; i++)
		{
			tmp_window = p->find_child_byatom(p, childrens[i], atom);
			if (tmp_window != None)
				break;
		}
	}

	if (childrens)
		XFree(childrens);
	return tmp_window;
}

XClassHint* xobj_get_wm_class(struct _xobj *p)
{
	Window window = p->find_win_byatom(p, "WM_CLASS");
	if (window == None)
		window = p->xWindow;

	XClassHint *wm_class = XAllocClassHint();
	if (!XGetClassHint(p->xDisplay, window, wm_class))
	{
		log_message(ERROR, "Can't get WM_CLASS");

		XFree(wm_class->res_class);
		XFree(wm_class->res_name);
		XFree(wm_class);
		return NULL;
	}
	return wm_class;
}

char* xobj_get_wmclass_class(struct _xobj *p)
{
	XClassHint *wm_class = p->get_wm_class(p);
	if (!wm_class)
		return NULL;

	int len = strlen(wm_class->res_class);

	char *string = (char *) xnmalloc(len + 1);
	memcpy(string, wm_class->res_class, len);
	string[len] = NULLSYM;

	XFree(wm_class->res_class);
	XFree(wm_class->res_name);
	XFree(wm_class);

	return string;
}

bool xobj_set_event_mask(struct _xobj *p, int event_mask)
{  
	return XSelectInput(p->xDisplay, p->xWindow, event_mask);
}

void xobj_uninit(struct _xobj *p)
{
	free(p);
}

struct _xobj* xobj_init(void)
{
	struct _xobj *p = (struct _xobj *) xnmalloc(sizeof(struct _xobj));
	bzero(p, sizeof(struct _xobj));

	p->xWindow		= None;

	// Functions mapping
	p->connect		= xobj_connect;
	p->create_win		= xobj_create_win;
	p->get_app_root_win	= xobj_get_app_root_win;
	p->find_win_byatom	= xobj_find_win_byatom;
	p->find_child_byatom	= xobj_find_child_byatom;
	p->get_wm_class		= xobj_get_wm_class;
	p->get_wmclass_name	= xobj_get_wmclass_class;
	p->get_wmclass_class	= xobj_get_wmclass_class;
	p->set_event_mask	= xobj_set_event_mask;
	p->uninit		= xobj_uninit;

	return p;
}
