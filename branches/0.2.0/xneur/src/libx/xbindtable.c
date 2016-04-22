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

#include "utils.h"

#include "xbindtable.h"

void xbindingtable_bind(struct _xbindingtable *p, int key, int modifier, int action)
{
	KeySym key_ks = None, key_ks_alt = None;
	switch (key)
	{
		case PAUSE_KEY:
		{
			key_ks = XK_Pause;
			key_ks_alt = XK_Break;
			break;
		}
		case SCROLL_LOCK_KEY:
		{
			key_ks = XK_Scroll_Lock;
			key_ks_alt = None;
			break;
		}
		case SYS_RQ_KEY:
		{
			key_ks = XK_Print;
			key_ks_alt = XK_Sys_Req;
			break;
		}
		case None:
		{
			key_ks = None;
			key_ks_alt = None;
			break;
		}
	}

	KeySym modifier_ks = None;
	switch (modifier)
	{
		case SHIFT_KEY:
		{
			modifier_ks = XK_Shift_R;
			break;
		}
		case CONTROL_KEY:
		{
			modifier_ks = XK_Control_R;
			break;
		}
		case None:
		{
			modifier_ks = None;
			break;
		}
	}
  
	p->bind_key(p, key_ks, key_ks_alt, modifier_ks, action);
}

void xbindingtable_set_action(struct _xbindingtable *p, int action)
{
	if (action < 0 || action > p->binds_count - 1)
		return;

	p->current_action = action;
}

int xbindingtable_get_action(struct _xbindingtable *p)
{
	return p->current_action;
}

void xbindingtable_bind_key(struct _xbindingtable *p, KeySym key, KeySym alt, KeySym modifier, int action)
{
	if (action < 0 || action > p->binds_count - 1)
		return;

	p->bindlist[action].key = key;
	p->bindlist[action].key_alt = alt;
	p->bindlist[action].modifier = modifier;
}

int xbindingtable_get_bind_action(struct _xbindingtable *p, KeySym key, KeySym modifier)
{
	int i;
	for (i = 1; i < p->binds_count; i++)
	{
		if (p->bindlist[i].modifier != modifier)
			continue;

		if (p->bindlist[i].key == key || p->bindlist[i].key_alt == key)
			return i;
	}
	return 0;
}

void xbindingtable_uninit(struct _xbindingtable *p)
{
	free(p->bindlist);
	free(p);
}

struct _xbindingtable* xbindingtable_init(int binds_count)
{
	struct _xbindingtable *p = (struct _xbindingtable *) xnmalloc(sizeof(struct _xbindingtable));
	bzero(p, sizeof(struct _xbindingtable));

	p->binds_count = binds_count;

	p->bindlist = (struct _xbinding *) xnmalloc(binds_count * sizeof(struct _xbinding));
	bzero(p->bindlist, binds_count * sizeof(struct _xbinding));

	p->current_action = -1;

	// Functions mapping
	p->set_action		= xbindingtable_set_action;
	p->get_action		= xbindingtable_get_action;
	p->bind			= xbindingtable_bind;
	p->bind_key		= xbindingtable_bind_key;
	p->get_bind_action	= xbindingtable_get_bind_action;
	p->uninit		= xbindingtable_uninit;

	return p;
}
