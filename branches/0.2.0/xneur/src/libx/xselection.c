/*
 *  Ths program is free software; you can redistribute it and/or modify
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

#include <X11/Xatom.h>

#include <stdlib.h>
#include <string.h>

#include "xobject.h"

#include "utils.h"
#include "log.h"

#include "xselection.h"

void xselection_request_to_convert(struct _xselection *p)
{
	XConvertSelection(p->owner->xDisplay, p->selection, p->target, None, p->owner->xWindow, CurrentTime);
}

XSelectionEvent* xselection_wait_for_notify(struct _xselection *p)
{
	XEvent event;
	do
	{
		XNextEvent(p->owner->xDisplay, &event);
	}
	while (event.type != SelectionNotify);

	if (event.xselection.property == None)
	{
		log_message(DEBUG, "Convert to selection target return None answer");
		return NULL;
	}

	XSelectionEvent *sevent = (XSelectionEvent *) xnmalloc(sizeof(XSelectionEvent));
	*sevent = event.xselection;
	return sevent;
}

char* xselection_get_to_string(struct _xselection *p, XSelectionEvent *xselt)
{
	unsigned long len, bytes_left, dummy;
	unsigned char *data = NULL;
	int format;
	Atom type;
	
	XGetWindowProperty(p->owner->xDisplay, p->owner->xWindow, xselt->property, 0, 0, 0, AnyPropertyType, &type, &format, &len, &bytes_left, &data);
	if (bytes_left == 0)
	{
		log_message(DEBUG, "Selected text length is 0");
		return NULL;
	}

	if (XGetWindowProperty(p->owner->xDisplay, p->owner->xWindow, xselt->property, 0, bytes_left, 0, AnyPropertyType, &type,&format, &len, &dummy, &data) != Success)
	{
		log_message(ERROR, "Failed to get selected text data");
		return NULL;
	}

	log_message(DEBUG, "Selected text data is '%s'", (char *) data);
	return (char *)data;
}

char* xselection_get_selected_text(struct _xselection *p)
{
	p->request_to_convert(p);

	XSelectionEvent *event = p->wait_for_notify(p);
	if (event == NULL)
		return NULL;

	char *string = p->get_to_string(p, event);
	if (string == NULL)
		return NULL;
	
	free(event);
	return string;
}

void xselection_uninit(xselection *p)
{
	p->owner->uninit(p->owner);
	free(p);
}

struct _xselection* xselection_init(Display *ManagedDisplay, Window ManagedWindow)
{
	struct _xselection *p = (struct _xselection *) xnmalloc(sizeof(struct _xselection));
	bzero(p, sizeof(struct _xselection));

	p->owner		= xobj_init();

	p->selection		= XA_PRIMARY;
	p->target		= XInternAtom(ManagedDisplay, "UTF8_STRING", FALSE);
	p->owner->xDisplay	= ManagedDisplay;
	p->owner->xWindow	= ManagedWindow;

	// functions mapping
	p->get_selected_text	= xselection_get_selected_text;
	p->wait_for_notify	= xselection_wait_for_notify;
	p->get_to_string	= xselection_get_to_string;
	p->request_to_convert	= xselection_request_to_convert;
	p->uninit		= xselection_uninit;

	return p;
}
