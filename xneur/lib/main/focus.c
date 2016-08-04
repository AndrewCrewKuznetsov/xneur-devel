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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xnconfig.h"

#include "utils.h"
#include "window.h"
#include "defines.h"

#include "types.h"
#include "list_char.h"
#include "log.h"

#include "focus.h"

extern struct _xneur_config *xconfig;
extern struct _window *main_window;

const char *verbose_forced_mode[]	= {"Default", "Manual", "Automatic"};
const char *verbose_focus_status[]	= {"Processed", "Changed Focus", "Unchanged Focus", "Excluded"};


// Private
int focus_get_focused_window(struct _focus *p)
{
	if (p) {};
	Window new_window;
	int revert_to;
	XGetInputFocus(main_window->display, &new_window, &revert_to);

	return new_window;
}

static int get_focus(struct _focus *p, int *forced_mode, int *focus_status, int *autocompletion_mode)
{
	*forced_mode	= FORCE_MODE_NORMAL;
	*focus_status	= FOCUS_NONE;
	*autocompletion_mode	= AUTOCOMPLETION_INCLUDED;

	char *new_app_name = NULL;
		
	// Clear masking on unfocused window
	//p->update_grab_events(p, LISTEN_DONTGRAB_INPUT);

	Window new_window;
	int show_message = TRUE;
	while (TRUE)
	{
		// Wait for new window was focused.
		usleep(500000);
		
		// This code commented be cause function XGrabKey for _NET_ACTIVE_WINDOW 
		// dont process modifier keys (see utils.h)
		/*if (main_window->_NET_SUPPORTED)
		{
			Atom type;
			int size;
			long nitems;

			Atom request = XInternAtom(main_window->display, "_NET_ACTIVE_WINDOW", False);
			Window root = XDefaultRootWindow(main_window->display);
			unsigned char *data = get_win_prop(root, request, &nitems, &type, &size);

			if (nitems > 0) 
				new_window = *((Window*)data);
			else 
				new_window = None;

			free(data);
		}
		else
		{*/
			int revert_to;
			XGetInputFocus(main_window->display, &new_window, &revert_to);
		//}

		// Catch not empty and not system window
		if (new_window != None && new_window > 1000)
		{
			new_app_name = get_wm_class_name(new_window);
			if (new_app_name != NULL)
				break;
		}
		
		if (show_message)
		{
			log_message(DEBUG, _("New window empty"));
			show_message = FALSE;
		}
		usleep(100000);
	}

	//char *new_app_name = get_wm_class_name(new_window);
	//if (new_app_name != NULL)
	//{
		if (xconfig->excluded_apps->exist(xconfig->excluded_apps, new_app_name, BY_PLAIN))
			*focus_status = FOCUS_EXCLUDED;
		
		if (xconfig->auto_apps->exist(xconfig->auto_apps, new_app_name, BY_PLAIN))
			*forced_mode = FORCE_MODE_AUTO;
		else if (xconfig->manual_apps->exist(xconfig->manual_apps, new_app_name, BY_PLAIN))
			*forced_mode = FORCE_MODE_MANUAL;

		if (xconfig->autocompletion_excluded_apps->exist(xconfig->autocompletion_excluded_apps, new_app_name, BY_PLAIN))
			*autocompletion_mode	= AUTOCOMPLETION_EXCLUDED;
	//}
	//else
	//	*focus_status = FOCUS_EXCLUDED;

	Window old_window = p->owner_window;
	if (new_window == old_window)
	{
		if (new_app_name != NULL)
			free(new_app_name);
		if (xconfig->troubleshoot_full_screen)
		{
			Window root_return;
			int x_return, y_return, root_x_return, root_y_return;
			unsigned int width_return, height_return, root_width_return, root_height_return;
			unsigned int border_width_return;
			unsigned int depth_return;
			XGetGeometry(main_window->display, p->parent_window, &root_return, &x_return, &y_return, &width_return, 
							&height_return, &border_width_return, &depth_return);
			XGetGeometry(main_window->display, root_return, &root_return, &root_x_return, &root_y_return, &root_width_return, 
							&root_height_return, &border_width_return, &depth_return);
			if ((x_return == 0) && (y_return == 0) && 
			    (width_return == root_width_return) && (height_return == root_height_return))
				*forced_mode = FORCE_MODE_MANUAL;
		}
		return FOCUS_UNCHANGED;
	}

	log_message(DEBUG, _("Focused window %d"), new_window);

	// Up to heighted window
	p->parent_window = new_window;
	while (TRUE)
	{
		unsigned int children_count;
		Window root_window, parent_window;
		Window *children_return = NULL;

		int is_same_screen = XQueryTree(main_window->display, p->parent_window, &root_window, &parent_window, &children_return, &children_count);
		if (children_return != NULL)
			XFree(children_return);
		if (!is_same_screen || parent_window == None || parent_window == root_window)
			break;

		p->parent_window = parent_window;
	}

	// Replace unfocused window to focused window
	p->owner_window = new_window;

	if (xconfig->troubleshoot_full_screen)
	{
		Window root_return;
		int x_return, y_return, root_x_return, root_y_return;
		unsigned int width_return, height_return, root_width_return, root_height_return;
		unsigned int border_width_return;
		unsigned int depth_return;
		XGetGeometry(main_window->display, p->parent_window, &root_return, &x_return, &y_return, &width_return, 
						&height_return, &border_width_return, &depth_return);
		XGetGeometry(main_window->display, root_return, &root_return, &root_x_return, &root_y_return, &root_width_return, 
						&root_height_return, &border_width_return, &depth_return);
		if ((x_return == 0) && (y_return == 0) && 
			(width_return == root_width_return) && (height_return == root_height_return))
			*forced_mode = FORCE_MODE_MANUAL;
	}
	
	log_message(DEBUG, _("Process new window (ID %d) with name '%s' (status %s, mode %s)"), new_window, new_app_name, _(verbose_focus_status[*focus_status]), _(verbose_forced_mode[*forced_mode]));
	
	if (new_app_name != NULL)
		free(new_app_name);
	return FOCUS_CHANGED;
}

static int focus_get_focus_status(struct _focus *p, int *forced_mode, int *focus_status, int *autocompletion_mode)
{
	int focus = get_focus(p, forced_mode, focus_status, autocompletion_mode);
	
	p->last_focus = *focus_status;
	if (!xconfig->tracking_input)
		p->last_focus = FOCUS_EXCLUDED;
	
	return focus;
}

static void focus_update_grab_events(struct _focus *p, int mode)
{
	char *owner_window_name = get_wm_class_name(p->owner_window);

	if ((mode == LISTEN_DONTGRAB_INPUT) || (p->last_focus == FOCUS_EXCLUDED))
	{
		grab_button(p->parent_window, FALSE);
		grab_all_keys(p->owner_window, FALSE);
	}
	else
	{
		if (xconfig->tracking_mouse)
			grab_button(p->parent_window, TRUE);
		grab_all_keys(p->owner_window, TRUE);
	}
	
	/*
	if (mode == LISTEN_DONTGRAB_INPUT)
	{
		log_message (DEBUG, _("Interception of events in the window (ID %d) with name '%s' OFF"), p->owner_window, owner_window_name);
		
		// Event unmasking
		grab_button(p->owner_window, FALSE);
		grab_all_keys(p->owner_window, FALSE);
	}
	else
	{
		log_message (DEBUG, _("Interception of events in the window (ID %d) with name '%s' ON"), p->owner_window, owner_window_name);
		
		// Event masking
		// Grabbing key and button
		if (p->last_focus != FOCUS_EXCLUDED)
		{
			if (xconfig->tracking_mouse)
			  grab_button(p->parent_window, TRUE);
			grab_all_keys(p->owner_window, TRUE);
		}
		else
		{
			grab_button(p->owner_window, FALSE);
			grab_all_keys(p->owner_window, FALSE);
		}
	}
	*/
	
	p->last_parent_window = p->parent_window;
	
	if (owner_window_name != NULL)
		free(owner_window_name);
}

static void focus_uninit(struct _focus *p)
{
	free(p);

	log_message(DEBUG, _("Focus is freed"));
}

struct _focus* focus_init(void)
{
	struct _focus *p = (struct _focus *) malloc(sizeof(struct _focus));
	bzero(p, sizeof(struct _focus));

	// Functions mapping
	p->get_focus_status	= focus_get_focus_status;
	p->get_focused_window = focus_get_focused_window;
	p->update_grab_events	= focus_update_grab_events;
	p->uninit		= focus_uninit;

	return p;
}
