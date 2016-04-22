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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"
#include "types.h"
#include "log.h"

#include "list_void.h"

void list_void_uninit(struct _list_void *list)
{
	free(list->data);
	free(list);
}

void list_void_add(struct _list_void *list, void *value)
{
	list->data_count++;
	if (list->data == NULL)
		list->data = (struct _list_void_data *) xnmalloc(list->data_count * sizeof(struct _list_void_data));
	else
		list->data = (struct _list_void_data *) xnrealloc(list->data, list->data_count * sizeof(struct _list_void_data));

	list->data[list->data_count - 1].value = value;;
}

void list_void_rem(struct _list_void *list, void *value)
{
	int i;
	for (i = 0; i < list->data_count; i++)
	{
		if (list->data[i].value != value)
			continue;

		list->rem_by_id(list, i);
	}
}

void list_void_rem_by_id(struct _list_void *list, int id)
{
	if (id >= list->data_count)
		return;

	list->data_count--;

	if (list->data_count != id)
		list->data[id] = list->data[list->data_count];

	if (list->data_count != 0)
		list->data = (struct _list_void_data *) xnrealloc(list->data, list->data_count * sizeof(struct _list_void_data));
	else
	{
		free(list->data);
		list->data = NULL;
	}
}

struct _list_void_data* list_void_find(struct _list_void *list, void *value)
{
	int i;
	for (i = 0; i < list->data_count; i++)
	{
		struct _list_void_data *data = &list->data[i];

		if (data->value == value)
			return data;
	}

	return NULL;
}

struct _list_void* list_void_clone(struct _list_void *list)
{
	struct _list_void *list_copy = list_void_init();

	int i;
	for (i = 0; i < list->data_count; i++)
		list_copy->add(list_copy, list->data[i].value);

	return list_copy;
}

int list_void_exist(struct _list_void *list, void *value)
{
	struct _list_void_data *data = list->find(list, value);
	return (data != NULL);
}

struct _list_void* list_void_init(void)
{
	struct _list_void *list = (struct _list_void *) xnmalloc(sizeof(struct _list_void));
	bzero(list, sizeof(struct _list_void));

	list->uninit		= list_void_uninit;
	list->add		= list_void_add;
	list->rem		= list_void_rem;
	list->rem_by_id		= list_void_rem_by_id;
	list->find		= list_void_find;
	list->clone		= list_void_clone;
	list->exist		= list_void_exist;

	return list;
}
