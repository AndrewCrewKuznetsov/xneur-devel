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
 *  Copyright (C) 2006-2016 XNeur Team
 *
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <X11/Xatom.h>

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "window.h"

#include "types.h"
#include "utils.h"
#include "log.h"

#include "selection.h"

static Atom utf8_atom;
static Atom compound_text_atom;

extern struct _window *main_window;
extern struct _xneur_config *xconfig;

static unsigned char *wait_selection (Atom selection)
{
	XEvent event;
	Atom target;

	int format;
	unsigned long bytesafter, length;
	unsigned char * value, * retval = NULL;
	int keep_waiting = True;

	while (keep_waiting) 
	{
		XNextEvent (main_window->display, &event);

		switch (event.type) 
		{
			case SelectionNotify:
				if (event.xselection.selection != selection) 
					break;

				if (event.xselection.property == None) 
				{
					log_message(WARNING, _("Convert to selection target error"));
					value = NULL;
					keep_waiting = FALSE;
				} 
				else 
				{
					XGetWindowProperty (event.xselection.display,
						event.xselection.requestor,
						event.xselection.property, 0L, 1000000,
						False, (Atom)AnyPropertyType, &target, &format, &length, &bytesafter, &value);
					
					if (target != utf8_atom && target != XA_STRING && target != compound_text_atom) 
					{
						/* Report non-TEXT atoms */
						log_message(WARNING, _("Selection is not a string"));
						free (retval);
						retval = NULL;
						keep_waiting = FALSE;
					} 
					else 
					{
						retval = (unsigned char *)strdup ((char *)value);
						XFree (value);
						keep_waiting = FALSE;
					}

					XDeleteProperty (event.xselection.display, event.xselection.requestor, event.xselection.property);
		    	}
				break;
    		default:
     			 break;
    	}
	}
	return retval;
}

static Time get_timestamp (void)
{
  XEvent event;

  XChangeProperty (main_window->display, main_window->window, XA_WM_NAME, XA_STRING, 8,
                   PropModeAppend, NULL, 0);

  while (1) {
    XNextEvent (main_window->display, &event);

    if (event.type == PropertyNotify)
      return event.xproperty.time;
  }
}

static unsigned char *get_selection (Atom selection, Atom request_target)
{
	unsigned char * retval;

	// Get a timestamp 
	XSelectInput (main_window->display, main_window->window, PropertyChangeMask);

	Atom prop = XInternAtom (main_window->display, "XSEL_DATA", FALSE);
	Time timestamp = get_timestamp ();
	
	XConvertSelection (main_window->display, selection, request_target, prop, main_window->window, timestamp);
	XSync (main_window->display, FALSE);

	retval = wait_selection (selection);
	
	return retval;
}

unsigned char *get_selection_text (enum _selection_type sel_type)
{
	Atom selection = 0;
	switch (sel_type)
	{
		case SELECTION_PRIMARY:
		{
			selection = XInternAtom(main_window->display, "PRIMARY", FALSE);
			break;
		}
		case SELECTION_SECONDARY:
		{
			selection = XInternAtom(main_window->display, "SECONDARY", FALSE);
			break;
		}
		case SELECTION_CLIPBOARD:
		{
			selection = XInternAtom(main_window->display, "CLIPBOARD", FALSE);
			break;
		}
	}

	utf8_atom = XInternAtom (main_window->display, "UTF8_STRING", FALSE);
	compound_text_atom = XInternAtom (main_window->display, "COMPOUND_TEXT", FALSE);

	unsigned char * retval;
	if ((retval = get_selection (selection, utf8_atom)) == NULL)
		retval = get_selection (selection, XA_STRING);

	return retval;
}
