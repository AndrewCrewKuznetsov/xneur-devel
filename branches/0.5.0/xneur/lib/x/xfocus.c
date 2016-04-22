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

#include <X11/Xutil.h>

#include <stdlib.h>
#include <string.h>

#include "xnconfig.h"

#include "xutils.h"
#include "xwindow.h"
#include "xdefines.h"

#include "log.h"
#include "list.h"
#include "utils.h"
#include "types.h"

#include "xfocus.h"

extern struct _xneur_config *xconfig;
extern struct _xwindow *main_window;

static int get_focus(struct _xfocus *p, int *forced_mode, int *focus_status)
{
	*forced_mode = FORCE_MODE_NORMAL;

	if (xconfig->mouse_processing_mode == MOUSE_GRAB_ENABLE && p->last_parent_window != None)
		grab_button(p->last_parent_window, FALSE);

	int revert_to;
	Window new_window;

	XGetInputFocus(main_window->display, &new_window, &revert_to);
	// Get window type
	//log_message(DEBUG, "%d %s", new_window, XGetAtomName(main_window->display, get_wm_window_type(new_window)));
	if (new_window == None)
		return FOCUS_NONE;
	
	char *new_app_name = get_wm_class_name(new_window);
	
	if (!new_app_name)
		return FOCUS_EXCLUDED;
	
	int new_app_name_len = strlen(new_app_name);
	
	*focus_status = FOCUS_NONE;
	
	if (xconfig->excluded_apps->exist(xconfig->excluded_apps, new_app_name, new_app_name_len))
		*focus_status = FOCUS_EXCLUDED;

	if (xconfig->dummy_apps->exist(xconfig->dummy_apps, new_app_name, new_app_name_len))
		*focus_status = FOCUS_DUMMY;
	
	Window old_window = p->owner_window;
	if (new_window == old_window)
		return FOCUS_UNCHANGED;

	p->owner_window = new_window;

	if (xconfig->auto_apps->exist(xconfig->auto_apps, new_app_name, new_app_name_len))
		*forced_mode = FORCE_MODE_AUTO;
	else if (xconfig->manual_apps->exist(xconfig->manual_apps, new_app_name, new_app_name_len))
		*forced_mode = FORCE_MODE_MANUAL;

	log_message(DEBUG, "Process new window with name '%s'", new_app_name);
	free(new_app_name);
	return FOCUS_CHANGED;
}

int xfocus_get_focus_status(struct _xfocus *p, int *forced_mode, int *focus_status)
{
	int focus = get_focus(p, forced_mode, focus_status);

	if (focus == FOCUS_UNCHANGED)
		return p->last_focus;

	if (focus == FOCUS_CHANGED)
		p->last_focus = FOCUS_UNCHANGED;
	else
		p->last_focus = focus;

	return focus;
}

void xfocus_update_events(struct _xfocus *p, int mode)
{
	Window parent_window = p->owner_window;

	if (mode == LISTEN_DUMMY)
	{
		set_event_mask(parent_window, FOCUS_CHANGE_MASK | INPUT_HANDLE_MASK);
		p->last_parent_window = parent_window;
		if (xconfig->mouse_processing_mode == MOUSE_GRAB_ENABLE)
			grab_button(parent_window, TRUE);
		return;
	}

	int mask = FOCUS_CHANGE_MASK;
	if (mode == LISTEN_FLUSH)
		mask = None;
	else if (mode == LISTEN_GRAB_INPUT)
		mask |= INPUT_HANDLE_MASK;

	while (TRUE)
	{
		set_event_mask(parent_window, mask);

		int dummy;
		unsigned int dummyU;
		Window root_window, child_window;

		int is_same_screen = XQueryPointer(main_window->display, parent_window, &root_window, &child_window, &dummy, &dummy, &dummy, &dummy, &dummyU);
		if (!is_same_screen || child_window == None)
			break;
		 
		parent_window = child_window;
	}

	p->last_parent_window = parent_window;
	if (xconfig->mouse_processing_mode == MOUSE_GRAB_ENABLE)
		grab_button(parent_window, TRUE);
}

void xfocus_send_button_event(struct _xfocus *p, XEvent *event)
{
	grab_button(p->last_parent_window, FALSE);

	XSendEvent(main_window->display, event->xany.window, FALSE, BUTTON_HANDLE_MASK, event);

	grab_button(p->last_parent_window, TRUE);
}

void xfocus_uninit(struct _xfocus *p)
{
	free(p);
}

struct _xfocus* xfocus_init(void)
{
	struct _xfocus *p = (struct _xfocus *) xnmalloc(sizeof(struct _xfocus));
	bzero(p, sizeof(struct _xfocus));

	// Functions mapping
	p->get_focus_status	= xfocus_get_focus_status;
	p->update_events	= xfocus_update_events;
	p->send_button_event	= xfocus_send_button_event;
	p->uninit		= xfocus_uninit;

	return p;
}
