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

#include <X11/keysym.h>

#include <stdlib.h>
#include <string.h>

#include "xobject.h"
#include "xstring.h"
#include "xswitchlang.h"
#include "xdefines.h"

#include "utils.h"
#include "conv.h"
#include "log.h"

#include "xevent.h"

void xevent_send_convert_string(struct _xevent *p, struct _xstr *str, struct _xswitchlang *swlng, int lang)
{
	swlng->switch_lang(swlng, lang);
	str->set_key_code(str, lang);
	p->send_string(p, str);
}

void xevent_del_nchrbck(struct _xevent *p, int n)
{
	int i;  
	for (i = 0; i < n; i++)
		p->send_xkey(p, XKeysymToKeycode(p->communicator->xDisplay, XK_BackSpace), None);
	XFlush(p->communicator->xDisplay);
}

void xevent_set_communicator(struct _xevent *p, struct _xobj *communicator)
{
	*p->communicator = *communicator;
}

Window xevent_get_event_win(struct _xevent *p)
{
	return p->event.xkey.window;
}

void xevent_send_xkey(struct _xevent *p, KeyCode kc, int modifiers)
{
	Display *comm_display = p->communicator->xDisplay;
	if (!comm_display)
		return;

	Window comm_window		= p->communicator->xWindow; 

	p->event.type			= KeyPress;
	p->event.xkey.type		= KeyPress;
	p->event.xkey.window		= comm_window;
	p->event.xkey.root		= RootWindow(comm_display, DefaultScreen(comm_display));
	p->event.xkey.subwindow		= None;
	p->event.xkey.same_screen	= True;
	p->event.xkey.display		= comm_display;
	p->event.xkey.state		= modifiers;
	p->event.xkey.keycode		= kc;
	p->event.xkey.time		= CurrentTime;

	if (!XSendEvent(comm_display, comm_window, TRUE, CLIENT_EVENT_MASK, &p->event))
		return;

	p->event.type			= KeyRelease;
	p->event.xkey.type		= KeyRelease;
	p->event.xkey.time		= CurrentTime;

	XSendEvent(comm_display, comm_window, TRUE, KeyReleaseMask, &p->event);
}

void xevent_send_string(struct _xevent *p, struct _xstr *str)
{
	int i, len = strlen(str->content);
	for (i = 0; i < len; i++)
		p->send_xkey(p, str->keycode[i], str->keycode_modifiers[i]);

	XFlush(p->communicator->xDisplay);
}

bool xevent_check_key_press(struct _xevent *p)
{
	return XCheckMaskEvent(p->communicator->xDisplay, KeyPressMask, &p->event);
}

bool xevent_is_native(struct _xevent *p)
{
	return !p->event.xkey.send_event;
}

char xevent_get_cur_char(struct _xevent *p)
{
	KeySym ks = XKeycodeToKeysym(p->communicator->xDisplay, p->event.xkey.keycode, p->event.xkey.state & ShiftMask);
	return keysym_to_char(ks);
}

KeySym xevent_get_cur_keysym(struct _xevent *p)
{
	KeySym ks;

	char *str = (char *) xnmalloc(INIT_WORD_LENGTH);
	XLookupString(&p->event.xkey, str, INIT_WORD_LENGTH, &ks, NULL);
	free(str);

	return ks;
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
 
void xevent_uninit(struct _xevent *p)
{
	p->communicator->uninit(p->communicator);
	free(p);
}

struct _xevent* xevent_init(void)
{
	struct _xevent *p = (struct _xevent *) xnmalloc(sizeof(struct _xevent));
	bzero(p, sizeof(struct _xevent));

	p->communicator		= xobj_init();
  
	// Functions mapping
	p->set_communicator	= xevent_set_communicator;
	p->get_event_win	= xevent_get_event_win;
	p->send_xkey		= xevent_send_xkey;
	p->send_convert_string	= xevent_send_convert_string;
	p->send_string		= xevent_send_string;
	p->check_key_press	= xevent_check_key_press;
	p->is_native		= xevent_is_native;
	p->get_cur_char		= xevent_get_cur_char;
	p->get_cur_keysym	= xevent_get_cur_keysym;
	p->get_cur_modifier	= xevent_get_cur_modifier;
	p->del_nchrbck		= xevent_del_nchrbck;
	p->uninit		= xevent_uninit;

	return p;
}
