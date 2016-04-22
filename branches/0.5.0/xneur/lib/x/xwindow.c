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
#include <strings.h>

#include "xutils.h"
#include "xdefines.h"

#include "utils.h"
#include "log.h"
#include "types.h"

#include "xwindow.h"

static int error_handler(Display *d, XErrorEvent *e)
{
        d = d;
        e = e;
        return FALSE;
}

void xwindow_move_window(struct _xwindow *p, int x, int y)
{
	XMoveWindow(p->display, p->window, x, y);
}

int xwindow_create(struct _xwindow *p)
{
	XSetErrorHandler(error_handler);

	Display *display = XOpenDisplay(NULL);
	if (!display)
	{
		log_message(ERROR, "Can't connect to XServer");
		return FALSE;
	}

	Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, 0, 0);
	if (!window)
	{
		log_message(ERROR, "Can't create program window");
		return FALSE;
	}

	p->display	= display;
	p->window	= window;

	set_event_mask(window, MAIN_WINDOW_MASK);

	XSynchronize(display, TRUE);
	XFlush(display);

	return TRUE;
}

void xwindow_destroy(struct _xwindow *p)
{
	if (p->window == None)
		return;

	XDestroyWindow(p->display, p->window);
	XCloseDisplay(p->display);

	p->window = None;
}

void xwindow_uninit(struct _xwindow *p)
{
	free(p);
}

struct _xwindow* xwindow_init(void)
{
	struct _xwindow *p = (struct _xwindow *) xnmalloc(sizeof(struct _xwindow));
	bzero(p, sizeof(struct _xwindow));

	// Function mapping
	p->create	= xwindow_create;
	p->destroy	= xwindow_destroy;
	p->move_window	= xwindow_move_window;

	return p;
}
