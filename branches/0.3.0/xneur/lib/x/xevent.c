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
#include <unistd.h>

#include "xstring.h"
#include "xswitchlang.h"
#include "xwindow.h"

#include "conversion.h"

#include "utils.h"
#include "log.h"
#include "types.h"

#include "xevent.h"

extern struct _xwindow *main_window;

void xevent_send_backspaces(struct _xevent *p, int count)
{
	int i;  
	for (i = 0; i < count; i++)
		p->send_xkey(p, p->backspace, None);
}

void xevent_set_owner_window(struct _xevent *p, Window window)
{
	p->owner_window = window;
}

void xevent_send_xkey(struct _xevent *p, KeyCode kc, int modifiers)
{
	Display *display		= main_window->display;
	Window window			= p->owner_window;

	p->event.type			= KeyPress;
	p->event.xkey.type		= KeyPress;
	p->event.xkey.window		= window;
	p->event.xkey.root		= RootWindow(display, DefaultScreen(display));
	p->event.xkey.subwindow		= None;
	p->event.xkey.same_screen	= True;
	p->event.xkey.display		= display;
	p->event.xkey.state		= modifiers;
	p->event.xkey.keycode		= kc;
	p->event.xkey.time		= CurrentTime;

	XSendEvent(display, window, TRUE, KeyPressMask, &p->event);

	p->event.type			= KeyRelease;
	p->event.xkey.type		= KeyRelease;
	p->event.xkey.time		= CurrentTime;

	usleep(1);
	XSendEvent(display, window, TRUE, KeyReleaseMask, &p->event);
}

void xevent_send_string(struct _xevent *p, struct _xstring *str)
{
	int i;
	for (i = 0; i < str->cur_pos; i++)
		p->send_xkey(p, str->keycode[i], str->keycode_modifiers[i]);
}

int get_key_state(int key)
{
	KeyCode key_code = XKeysymToKeycode(main_window->display, key);
	XModifierKeymap *map = XGetModifierMapping(main_window->display);

	if (key_code == NoSymbol)
		return 0;
	
	int i, key_mask = 0;
	for (i = 0; i < 8; i++)
		if (map->modifiermap[map->max_keypermod * i] == key_code)
			key_mask = 1 << i;

	if (key_mask == 0)
		return 0;

	XFreeModifiermap(map);

	Window		wDummy;
	int		iDummy;
	unsigned int	mask;
	XQueryPointer(main_window->display, DefaultRootWindow(main_window->display), &wDummy, &wDummy, &iDummy, &iDummy, &iDummy, &iDummy, &mask);

	return ((mask & key_mask) != 0);
}

char xevent_get_cur_char(struct _xevent *p)
{
	int mask = p->event.xkey.state & ShiftMask;
	if (get_key_state(XK_Caps_Lock) != 0)
		mask = !mask;
	
	KeySym ks = XKeycodeToKeysym(main_window->display, p->event.xkey.keycode, mask);
	
	return keysym_to_char(ks);
}

KeySym xevent_get_cur_keysym(struct _xevent *p)
{
	return XLookupKeysym(&p->event.xkey, 0);
}

KeySym xevent_get_cur_modifier(struct _xevent *p)
{
	if (p->event.xkey.state & ControlMask)
		return XK_Control_R;
	if (p->event.xkey.state & ShiftMask)
		return XK_Shift_R;
	if (p->event.xkey.state & Mod1Mask)
		return XK_Alt_R;
	return None;
}

int xevent_get_next_event(struct _xevent *p)
{
	XNextEvent(main_window->display, &p->event);
	return p->event.type;
}

void xevent_uninit(struct _xevent *p)
{
	free(p);
}

struct _xevent* xevent_init(void)
{
	struct _xevent *p = (struct _xevent *) xnmalloc(sizeof(struct _xevent));
	bzero(p, sizeof(struct _xevent));

	p->backspace		= XKeysymToKeycode(main_window->display, XK_BackSpace);

	// Functions mapping
	p->get_next_event	= xevent_get_next_event;
	p->set_owner_window	= xevent_set_owner_window;
	p->send_xkey		= xevent_send_xkey;
	p->send_string		= xevent_send_string;
	p->get_cur_char		= xevent_get_cur_char;
	p->get_cur_keysym	= xevent_get_cur_keysym;
	p->get_cur_modifier	= xevent_get_cur_modifier;
	p->send_backspaces	= xevent_send_backspaces;
	p->uninit		= xevent_uninit;

	return p;
}
