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

#ifndef _XFOCUS_H_
#define _XFOCUS_H_

#include "types.h"

typedef struct _xifocus
{
	struct _xobj *owner;	// Input focus window
	struct _xobj **ptr;	// Pointer windows
	int ptr_count;
	int ptr_size;
	int changed;		// TRUE if after update owner of input focus changed. By init is FALSE

	bool (*update) (struct _xifocus *p);
	bool (*is_changed) (struct _xifocus *p);
	bool (*listen_client) (struct _xifocus *p);
	void (*uninit) (struct _xifocus *p);
} xifocus;

struct _xifocus* xifocus_init(void);

#endif /* _XFOCUS_H_ */
