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

#include "config_main.h"

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

static int get_focus(struct _xfocus *p)
{
	if (xconfig->mouse_processing_mode == MOUSE_GRAB_ENABLE)
		if (p->last_parent_window != None)
			grab_button(p->last_parent_window, FALSE);

	int revert_to;
	Window new_window;

	XGetInputFocus(main_window->display, &new_window, &revert_to);
	if (new_window == None)
		return FOCUS_NONE;

	Window old_window = p->owner_window;
	if (new_window == old_window)
		return FOCUS_UNCHANGED;

	p->owner_window = new_window;

	char *new_app_name = get_wm_class_name(new_window);
	if (!new_app_name)
		return FOCUS_EXCLUDED;

	char *old_app_name = NULL;
	if (old_window != None)
		old_app_name = get_wm_class_name(old_window);

	int is_excluded = list_exist(xconfig->excluded_apps, new_app_name, strlen(new_app_name));

	if (!old_app_name || strcmp(new_app_name, old_app_name) != 0)
	{
		if (is_excluded)
			log_message(DEBUG, "Window with name '%s' found in exclusions list", new_app_name);
		else
			log_message(DEBUG, "Process new window with name '%s'", new_app_name);
	}

	if (old_app_name)
		free(old_app_name);
	free(new_app_name);
	
	if (is_excluded) 
		return FOCUS_EXCLUDED;	
	
	return FOCUS_CHANGED;
}

int xfocus_get_focus_status(struct _xfocus *p)
{
	int focus = get_focus(p);

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

	int mask = FOCUS_CHANGE_MASK;
	if (mode == LISTEN_FLUSH)
		mask = None;
	else if (mode == LISTEN_GRAB_INPUT)
		mask |= INPUT_HANDLE_MASK;

	char *app_name = get_wm_class_name(parent_window);
	if (app_name != NULL)
	{
		int is_dummy = list_exist(xconfig->dummy_apps, app_name, strlen(app_name));
		if (is_dummy) 
		{
			//log_message(DEBUG, "Window with name '%s' found in dummy list", app_name);
			set_event_mask(parent_window, mask);
			p->last_parent_window = parent_window;
			if (xconfig->mouse_processing_mode == MOUSE_GRAB_ENABLE)
				grab_button(parent_window, TRUE);
			return;
		}
	}
	
	//log_message(DEBUG, "New window is not dummy");
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
