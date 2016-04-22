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

#include <X11/Xutil.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xnconfig.h"

#include "xutils.h"
#include "xwindow.h"
#include "xdefines.h"

#include "types.h"
#include "list_char.h"
#include "log.h"

#include "xfocus.h"

extern struct _xneur_config *xconfig;
extern struct _xwindow *main_window;

const char *verbose_forced_mode[]	= {"Default", "Manual", "Automatic"};
const char *verbose_focus_status[]	= {"Processed", "Changed Focus", "Unchanged Focus", "Excluded"};

static int get_focus(struct _xfocus *p, int *forced_mode, int *focus_status)
{
	*forced_mode = FORCE_MODE_NORMAL;
	*focus_status = FOCUS_NONE;

	Window new_window;
	while (TRUE)
	{
		int revert_to;
		XGetInputFocus(main_window->display, &new_window, &revert_to);

		// Catch not empty and not system window 
		if (new_window != None && new_window > 1000)
			break;

		log_message(DEBUG, "New window empty");
		usleep(500);
	}
	
	char *new_app_name = get_wm_class_name(new_window);
	if (new_app_name != NULL)
	{
		if (xconfig->excluded_apps->exist(xconfig->excluded_apps, new_app_name, BY_PLAIN))
			*focus_status = FOCUS_EXCLUDED;

		if (xconfig->auto_apps->exist(xconfig->auto_apps, new_app_name, BY_PLAIN))
			*forced_mode = FORCE_MODE_AUTO;
		else if (xconfig->manual_apps->exist(xconfig->manual_apps, new_app_name, BY_PLAIN))
			*forced_mode = FORCE_MODE_MANUAL;
	}
	
	Window old_window = p->owner_window;
	if (new_window == old_window)
	{
		if (new_app_name != NULL)
			free(new_app_name);
		return FOCUS_UNCHANGED;
	}

	// Up to heighted window
	p->parent_window = new_window;
	while (TRUE)
	{
		unsigned int children_count;
		Window root_window, parent_window;
		Window *children_return;

		int is_same_screen = XQueryTree(main_window->display, p->parent_window, &root_window, &parent_window, &children_return, &children_count);
		if (!is_same_screen || parent_window == None || parent_window == root_window)
			break;
		
		p->parent_window = parent_window;
		XFree(children_return);
	}	
	
	// Replace unfocused window to focused window
	p->owner_window = new_window;

	log_message(DEBUG, "Process new window (ID %d) with name '%s' (status %s, mode %s)", new_window, new_app_name, verbose_focus_status[*focus_status], verbose_forced_mode[*forced_mode]);
	
	if (new_app_name != NULL)
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

static void set_mask_to_window(Window current_window, int mask)
{
	if (current_window == None)
		return;
	
	set_event_mask(current_window, mask);
	
	if (mask == None)
		grab_button(current_window, FALSE);
	else
		grab_button(current_window, TRUE);
	
	unsigned int children_count;
	Window root_window, parent_window;
	Window *children_return;
	
	int is_same_screen = XQueryTree(main_window->display, current_window, &root_window, &parent_window, &children_return, &children_count);
	if (!is_same_screen)
		return;
	
	for (int i = 0; i < children_count; i++)
		set_mask_to_window(children_return[i], mask);
	
	XFree(children_return);
}

void xfocus_update_events(struct _xfocus *p, int mode)
{
	// Mouse pointer motion masking and focus event masking every time for all windows
	const int mask = POINTER_MOTION_MASK;
	
	// Grabbing ONLY after masking!!!
	if (mode == LISTEN_DONTGRAB_INPUT)
	{
		// Event unmasking
		set_mask_to_window(p->parent_window, mask);
		set_event_mask(p->owner_window, mask | FOCUS_CHANGE_MASK);

		// Ungrabbing special key (Enter, Tab and other)
		grab_spec_keys(p->owner_window, FALSE);
	}
	else
	{
		// Event masking
		set_mask_to_window(p->parent_window, mask | EVENT_PRESS_MASK);
		set_event_mask(p->owner_window, mask | INPUT_HANDLE_MASK | FOCUS_CHANGE_MASK | EVENT_PRESS_MASK);

		// Grabbing special key (Enter, Tab and other)
		grab_spec_keys(p->owner_window, TRUE);
	}

	p->last_parent_window = p->parent_window;
}

int xfocus_draw_flag(struct _xfocus *p, Window event_window)
{
	char *app_name = get_wm_class_name(p->owner_window);
	if (app_name == NULL)
		return FALSE;
	
	if (!xconfig->draw_flag_apps->exist(xconfig->draw_flag_apps, app_name, BY_PLAIN))
	{
		free(app_name);
		return FALSE;
	}
	free(app_name);

	Window root_window, parent_window;
	Window *children_return;
	unsigned int dummyU;

	// Get parent window for window over pointer
	Window current_event_window = event_window;
	while (TRUE)
	{
		int is_same_screen = XQueryTree(main_window->display, current_event_window, &root_window, &parent_window, &children_return, &dummyU);
		if (!is_same_screen || parent_window == None || parent_window == root_window)
			break;

		current_event_window = parent_window;
		XFree(children_return);
	}

	return (p->parent_window == current_event_window);
}

void xfocus_uninit(struct _xfocus *p)
{
	free(p);

	log_message(DEBUG, "Current focus is freed");
}

struct _xfocus* xfocus_init(void)
{
	struct _xfocus *p = (struct _xfocus *) malloc(sizeof(struct _xfocus));
	bzero(p, sizeof(struct _xfocus));

	// Functions mapping
	p->get_focus_status	= xfocus_get_focus_status;
	p->update_events	= xfocus_update_events;
	p->draw_flag		= xfocus_draw_flag;
	p->uninit		= xfocus_uninit;

	return p;
}
