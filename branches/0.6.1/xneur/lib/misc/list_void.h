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

#ifndef _LIST_VOID_H_
#define _LIST_VOID_H_

struct _list_void_data
{
	void *value;
};

struct _list_void
{
	struct _list_void_data *data;
	int data_count;

	void (*uninit) (struct _list_void *list);
	void (*add) (struct _list_void *list, void *value);
	void (*rem) (struct _list_void *list, void *value);
	void (*rem_by_id) (struct _list_void *list, int index);
	struct _list_void_data* (*find) (struct _list_void *list, void *value);
	struct _list_void* (*clone) (struct _list_void *list);
	int  (*exist) (struct _list_void *list, void *value);
};

struct _list_void* list_void_init(void);

#endif /* _LIST_VOID_H_ */
