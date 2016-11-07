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

#include <X11/XKBlib.h>
//#include <X11/Xlib.h>
//#include <X11/extensions/XTest.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "xnconfig.h"

#include "switchlang.h"
#include "buffer.h"
#include "window.h"
#include "utils.h"
#include "defines.h"

#include "types.h"
#include "log.h"
#include "conversion.h"
#include "list_char.h"

#include "event.h"

extern struct _xneur_config *xconfig;
extern struct _window *main_window;

int get_key_state(int key)
{
	if (main_window->display == NULL)
		return 0;
	
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

	XFreeModifiermap(map);

	if (key_mask == 0)
		return 0;
	
	Window wDummy;
	int iDummy;
	unsigned int mask;
	XQueryPointer(main_window->display, DefaultRootWindow(main_window->display), &wDummy, &wDummy, &iDummy, &iDummy, &iDummy, &iDummy, &mask);

	return ((mask & key_mask) != 0);
}

void event_send_xkey(struct _event *p, KeyCode kc, int modifiers)
{
	//XTestGrabControl (main_window->display, True);
	
	char *app_name = NULL;
	app_name = get_wm_class_name(p->owner_window);
	
	int is_delay = xconfig->delay_send_key_apps->exist(xconfig->delay_send_key_apps, app_name, BY_PLAIN);
	if (is_delay)
	{
		usleep(xconfig->send_delay * 1000);
	}
	
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
		
	if (xconfig->dont_send_key_release_apps->exist(xconfig->dont_send_key_release_apps, app_name, BY_PLAIN))
	{
		XSendEvent(main_window->display, p->owner_window, TRUE, NoEventMask, &p->event);
		XFlush(main_window->display);
		log_message(TRACE, _("The event KeyRelease is not sent to the window (ID %d) with name '%s'"), p->owner_window, app_name);
		if (app_name != NULL)
			free(app_name);
		return;
	}

	XSendEvent(main_window->display, p->owner_window, TRUE, NoEventMask, &p->event);
	XFlush(main_window->display);
	
	if (is_delay)
	{
		usleep(xconfig->send_delay * 1000);
	}
	
	p->event.type			= KeyRelease;
	p->event.xkey.type		= KeyRelease;
	p->event.xkey.time		= CurrentTime;

	XSendEvent(main_window->display, p->owner_window, TRUE, NoEventMask, &p->event);
	XFlush(main_window->display);
	//XTestGrabControl (main_window->display, False);
	if (app_name != NULL)
		free(app_name);
}

static void event_send_backspaces(struct _event *p, int count)
{
	for (int i = 0; i < count; i++)
	{
		p->send_xkey(p, p->backspace, None);
	}
}

static void event_send_spaces(struct _event *p, int count)
{
	for (int i = 0; i < count; i++)
	{
		p->send_xkey(p, p->space, None);
	}
}

static void event_send_selection(struct _event *p, int count)
{
	for (int i = 0; i < count; i++)
		p->send_xkey(p, p->left, None);

	for (int i = 0; i < count; i++)
		p->send_xkey(p, p->right, ShiftMask);
}

static void event_send_string(struct _event *p, struct _buffer *str)
{
	for (int i = 0; i < str->cur_pos; i++)
		p->send_xkey(p, str->keycode[i], str->keycode_modifiers[i]);
}

static void event_set_owner_window(struct _event *p, Window window)
{
	p->owner_window = window;
}

static KeySym event_get_cur_keysym(struct _event *p)
{
	//return XLookupKeysym(&p->event.xkey, 0);
	
	KeySym ks;

	/*int nbytes = 0;
    char str[256+1];
	XKeyEvent *e = (XKeyEvent *) &p->event;
    nbytes = XLookupString (e, str, 256, &ks, NULL);
	if (nbytes) {};
	return ks;*/

	XKeyEvent *e = (XKeyEvent *) &p->event;
	ks = XkbKeycodeToKeysym(main_window->display, e->keycode, main_window->keymap->latin_group, 0);
	if (ks == NoSymbol) 
		ks = XkbKeycodeToKeysym(main_window->display, e->keycode, 0, 0);
	return ks;
}

static int event_get_cur_modifiers(struct _event *p)
{
	Window wDummy;
	int iDummy;
	unsigned int mask;
	XQueryPointer(main_window->display,
				(Window)p->event.xkey.window,
				&wDummy, &wDummy, &iDummy, &iDummy, &iDummy, &iDummy,
				&mask);
	mask = mask & (~get_languages_mask ());
	/*int mask = 0;
	
	if (p->event.xkey.state & ShiftMask)  // Shift
		mask += (1 << 0); // 1
	if (p->event.xkey.state & LockMask)   // CapsLock
		mask += (1 << 1); // 2
	if (p->event.xkey.state & ControlMask)// Control
		mask += (1 << 2); // 4
	if (p->event.xkey.state & Mod1Mask)   // Alt
		mask += (1 << 3); // 8
	if (p->event.xkey.state & Mod2Mask)   // NumLock
		mask += (1 << 4); // 16
	if (p->event.xkey.state & Mod3Mask)
		mask += (1 << 5); // 32
	if (p->event.xkey.state & Mod4Mask)   // Super (Win)
		mask += (1 << 6); // 64
	if (p->event.xkey.state & Mod5Mask)   // ISO_Level3_Shift
		mask += (1 << 7); // 128*/
	return (int)mask;
}

static int event_get_cur_modifiers_by_keysym(struct _event *p)
{
	unsigned int mask = 0;
	int key_sym = p->get_cur_keysym(p);
	
	if (key_sym == XK_Shift_L || key_sym == XK_Shift_R)
		mask += (1 << 0);
	if (key_sym == XK_Caps_Lock)
		mask += (1 << 1);
	if (key_sym == XK_Control_L || key_sym == XK_Control_R)
		mask += (1 << 2);
	if (key_sym == XK_Alt_L || key_sym == XK_Alt_R)
		mask += (1 << 3);
	if (key_sym == XK_Meta_L || key_sym == XK_Meta_R)
		mask += (1 << 4);
	if (key_sym == XK_Num_Lock)
		mask += (1 << 5);
	if (key_sym == XK_Super_L || key_sym == XK_Super_R)
		mask += (1 << 6);
	if (key_sym == XK_Hyper_L || key_sym == XK_Hyper_R || key_sym == XK_ISO_Level3_Shift)
		mask += (1 << 7);

	return mask;
}

static int event_get_next_event(struct _event *p)
{
	XNextEvent(main_window->display, &(p->event));
	return p->event.type;
}

static void event_send_next_event(struct _event *p)
{
	p->event.xkey.state = p->get_cur_modifiers(p) | groups[get_curr_keyboard_group()];
	XSendEvent(main_window->display, p->event.xany.window,FALSE, NoEventMask, &p->event);
}

static void event_uninit(struct _event *p)
{
	if (p != NULL)
		free(p);

	log_message(DEBUG, _("Event is freed"));
}

struct _event* event_init(void)
{
	struct _event *p = (struct _event *) malloc(sizeof(struct _event));
	bzero(p, sizeof(struct _event));

	p->backspace		= XKeysymToKeycode(main_window->display, XK_BackSpace);
	p->left			= XKeysymToKeycode(main_window->display, XK_Left);
	p->right		= XKeysymToKeycode(main_window->display, XK_Right);
	p->space		= XKeysymToKeycode(main_window->display, XK_space);
	
	// Functions mapping
	p->get_next_event	= event_get_next_event;
	p->send_next_event	= event_send_next_event;
	p->set_owner_window	= event_set_owner_window;
	p->send_xkey		= event_send_xkey;
	p->send_string		= event_send_string;
	p->get_cur_keysym	= event_get_cur_keysym;
	p->get_cur_modifiers	= event_get_cur_modifiers;
	p->get_cur_modifiers_by_keysym	= event_get_cur_modifiers_by_keysym;
	p->send_backspaces	= event_send_backspaces;
	p->send_selection	= event_send_selection;
	p->send_spaces	= event_send_spaces;
	p->uninit		= event_uninit;

	return p;
}
