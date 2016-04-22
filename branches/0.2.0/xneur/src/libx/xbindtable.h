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

#ifndef _XBINDING_H_
#define _XBINDING_H_

#include <X11/Xutil.h>

#include "types.h"

typedef struct _xbinding
{
	KeySym key;
	KeySym key_alt;
	KeySym modifier;
} xbinding;

typedef struct _xbindingtable
{
	struct _xbinding *bindlist;
	int binds_count;
	int current_action;

	void (*set_action) (struct _xbindingtable *p, int action);
	int  (*get_action) (struct _xbindingtable *p);
	void (*bind_key) (struct _xbindingtable *p, KeySym key, KeySym alt, KeySym modifier, int action);
	void (*bind) (struct _xbindingtable *p, int key, int modifier, int action);
	int  (*get_bind_action) (struct _xbindingtable *p, KeySym key, KeySym modifier);
	void (*uninit) (struct _xbindingtable *p);
} xbindingtable;

struct _xbindingtable* xbindingtable_init(int ibind);

#endif /* _XBINDING_H_ */
