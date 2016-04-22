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

#include <X11/Xutil.h>

#include <stdlib.h>
#include <string.h>

#include "xobject.h"
#include "xdefines.h"

#include "log.h"
#include "utils.h"

#include "xfocus.h"

bool xifocus_listen_client(struct _xifocus *p)
{
	if (!p->owner->set_event_mask(p->owner, CLIENT_EVENT_MASK))
		return FALSE;

	int i;
	for (i = 0; i < p->ptr_count; i++)
	{
		if (!p->ptr[i]->set_event_mask(p->ptr[i], CLIENT_EVENT_MASK))
			return FALSE;
	}
	return TRUE;
}

bool xifocus_update(struct _xifocus *p)
{
	struct _xobj *owner = p->owner;

	Window old_window = owner->xWindow;

	char *old_app_name = NULL;
	if (old_window != None)
		old_app_name = owner->get_wmclass_name(owner);

	int revert_to;
	Window new_window;

	XGetInputFocus(owner->xDisplay, &new_window, &revert_to);
	if (new_window == None) // If no window focused
	{
		p->ptr_count = 0;
		return FALSE;
	}

	p->changed = FALSE;
	if (new_window != old_window)
	{
		p->changed = TRUE;
		owner->xWindow = new_window;

		char *new_app_name = owner->get_wmclass_name(owner);

		if (!new_app_name || !old_app_name || strcmp(new_app_name, old_app_name) != 0)
			log_message(DEBUG, "New processed window with name '%s' (%d)", new_app_name, new_window);
		if (new_app_name)
			free(new_app_name);
	}

	if (old_app_name)
		free(old_app_name);

	p->ptr_count = 0;

	if (revert_to != RevertToPointerRoot)
		return TRUE;

	while (1)
	{
		int dummy;
		unsigned int dummyU;
		Window root_window;

		bool is_same_screen = XQueryPointer(owner->xDisplay, new_window, &root_window, &new_window, &dummy, &dummy, &dummy, &dummy, &dummyU);
		if (!is_same_screen || new_window == None)
			return TRUE;

		p->ptr_count++;
		if (p->ptr_count >= p->ptr_size)
		{
			p->ptr_size = p->ptr_count;
			p->ptr = (struct _xobj **) xnrealloc(p->ptr, p->ptr_size * sizeof(struct _xobj *));
			p->ptr[p->ptr_count - 1] = xobj_init();
		}

		struct _xobj *new_obj = p->ptr[p->ptr_count - 1];
		new_obj->xDisplay = owner->xDisplay;
		new_obj->xWindow = new_window;
	}
}

bool xifocus_is_changed(struct _xifocus *p)
{
	return p->changed;
}

void xifocus_uninit(struct _xifocus *p)
{
	p->owner->uninit(p->owner);

	while (p->ptr_count > 0)
	{
		p->ptr_count--;

		struct _xobj *ptr = p->ptr[p->ptr_count];
		ptr->uninit(ptr);
	}

	free(p->ptr);
	free(p);
}

struct _xifocus* xifocus_init(void)
{
	struct _xifocus *p = (struct _xifocus *) xnmalloc(sizeof(struct _xifocus));
	bzero(p, sizeof(struct _xifocus));

	p->ptr_size = 16;
	p->ptr = (struct _xobj **) xnmalloc(p->ptr_size * sizeof(struct _xobj *));

	int i;
	for (i = 0; i < p->ptr_size; i++)
		p->ptr[i] = xobj_init();

	p->owner		= xobj_init();

	// Functions mapping
	p->is_changed		= xifocus_is_changed;
	p->update		= xifocus_update;
	p->listen_client	= xifocus_listen_client;
	p->uninit		= xifocus_uninit;

	return p;
}
