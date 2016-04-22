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

#include <X11/keysym.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xnconfig.h"

#include "xstring.h"
#include "xswitchlang.h"
#include "xwindow.h"
#include "xutils.h"
#include "xdefines.h"

#include "types.h"
#include "log.h"
#include "conversion.h"

#include "xevent.h"

extern struct _xwindow *main_window;
extern struct _xneur_config *xconfig;

static void send_xkey(struct _xevent *p, KeyCode kc, int modifiers)
{
	p->event.type			= KeyPress;
	p->event.xkey.type		= KeyPress;
	p->event.xkey.window		= p->owner_window;
	p->event.xkey.root		= RootWindow(main_window->display, DefaultScreen(main_window->display));
	p->event.xkey.subwindow		= None;
	p->event.xkey.same_screen	= True;
	p->event.xkey.display		= main_window->display;
	p->event.xkey.state		= modifiers;
	p->event.xkey.keycode		= kc;
	p->event.xkey.time		= CurrentTime;

	XSendEvent(main_window->display, p->owner_window, TRUE, KeyPressMask, &p->event);

	p->event.type			= KeyRelease;
	p->event.xkey.type		= KeyRelease;
	p->event.xkey.time		= CurrentTime;

	if (xconfig->send_delay > 0)
		usleep(xconfig->send_delay);

	XSendEvent(main_window->display, p->owner_window, TRUE, KeyReleaseMask, &p->event);
}

void xevent_send_backspaces(struct _xevent *p, int count)
{
	for (int i = 0; i < count; i++)
		send_xkey(p, p->backspace, None);
}

void xevent_send_selection(struct _xevent *p, int count)
{
	for (int i = 0; i < count; i++)
		send_xkey(p, p->left, None);
	for (int i = 0; i < count; i++)
		send_xkey(p, p->right, ShiftMask);
}

void xevent_send_string(struct _xevent *p, struct _xstring *str)
{
	for (int i = 0; i < str->cur_pos; i++)
		send_xkey(p, str->keycode[i], str->keycode_modifiers[i]);
}

void xevent_set_owner_window(struct _xevent *p, Window window)
{
	p->owner_window = window;
}

int get_key_state(int key)
{
	KeyCode key_code = XKeysymToKeycode(main_window->display, key);
	if (key_code == NoSymbol)
		return 0;

	XModifierKeymap *map = XGetModifierMapping(main_window->display);

	int key_mask = 0;
	for (int i = 0; i < 8; i++)
	{
		if (map->modifiermap[map->max_keypermod * i] == key_code)
			key_mask = (1 << i);
	}

	if (key_mask == 0)
		return 0;

	XFreeModifiermap(map);

	Window wDummy;
	int iDummy;
	unsigned int mask;
	XQueryPointer(main_window->display, DefaultRootWindow(main_window->display), &wDummy, &wDummy, &iDummy, &iDummy, &iDummy, &iDummy, &mask);

	return ((mask & key_mask) != 0);
}

XEvent create_basic_event(void)
{
	XEvent event;

	event.type		= KeyPress;
	event.xkey.type		= KeyPress;
	event.xkey.root		= RootWindow(main_window->display, DefaultScreen(main_window->display));
	event.xkey.subwindow	= None;
	event.xkey.same_screen	= True;
	event.xkey.display	= main_window->display;
	event.xkey.state	= 0;
	event.xkey.keycode	= XKeysymToKeycode(main_window->display, XK_space);
	event.xkey.time		= CurrentTime;

	return event;
}

KeySym xevent_get_cur_keysym(struct _xevent *p)
{
	return XLookupKeysym(&p->event.xkey, 0);
}

int xevent_get_cur_modifiers(struct _xevent *p)
{
	int mask = 0;
	if (p->event.xkey.state & ShiftMask)
		mask += (1 << 0);
	if (p->event.xkey.state & ControlMask)
		mask += (1 << 2);
	if (p->event.xkey.state & Mod1Mask)
		mask += (1 << 3);
	if (p->event.xkey.state & Mod4Mask)
		mask += (1 << 6);
	return mask;
}

int xevent_get_next_event(struct _xevent *p)
{
	XNextEvent(main_window->display, &(p->event));
	return p->event.type;
}

void xevent_send_next_event(struct _xevent *p)
{
	if (p->event.type == KeyPress || p->event.type == KeyRelease)
		p->event.xkey.state = p->get_cur_modifiers(p) | groups[get_cur_lang()];

	XSendEvent(main_window->display, p->event.xany.window, TRUE, NoEventMask, &p->event);
}

void xevent_uninit(struct _xevent *p)
{
	free(p);

	log_message(DEBUG, "Current event is freed");
}

struct _xevent* xevent_init(void)
{
	struct _xevent *p = (struct _xevent *) malloc(sizeof(struct _xevent));
	bzero(p, sizeof(struct _xevent));

	p->backspace		= XKeysymToKeycode(main_window->display, XK_BackSpace);
	p->left			= XKeysymToKeycode(main_window->display, XK_Left);
	p->right		= XKeysymToKeycode(main_window->display, XK_Right);

	// Functions mapping
	p->get_next_event	= xevent_get_next_event;
	p->send_next_event	= xevent_send_next_event;
	p->set_owner_window	= xevent_set_owner_window;
	p->send_string		= xevent_send_string;
	p->get_cur_keysym	= xevent_get_cur_keysym;
	p->get_cur_modifiers	= xevent_get_cur_modifiers;
	p->send_backspaces	= xevent_send_backspaces;
	p->send_selection	= xevent_send_selection;
	p->uninit		= xevent_uninit;

	return p;
}
