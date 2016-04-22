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
#include "regexp.h"

#include "list_char.h"

void list_char_uninit(struct _list_char *list)
{
	int i;
	for (i = 0; i < list->data_count; i++)
		free(list->data[i].string);

	free(list->data);
	free(list);
}

void list_char_add(struct _list_char *list, const char *string)
{
	list->data_count++;
	if (list->data == NULL)
		list->data = (struct _list_char_data *) xnmalloc(list->data_count * sizeof(struct _list_char_data));
	else
		list->data = (struct _list_char_data *) xnrealloc(list->data, list->data_count * sizeof(struct _list_char_data));

	struct _list_char_data *data = &list->data[list->data_count - 1];

	data->string		= strdup(string);
	data->string_size	= strlen(string);
}

void list_char_rem(struct _list_char *list, const char *string, int string_size)
{
	int i;
	for (i = 0; i < list->data_count; i++)
	{
		if (list->data[i].string_size != string_size || strncmp(list->data[i].string, string, string_size) != 0)
			continue;

		list->rem_by_id(list, i);
	}
}

void list_char_rem_by_id(struct _list_char *list, int id)
{
	if (id >= list->data_count)
		return;

	list->data_count--;
	free(list->data[id].string);

	if (list->data_count != id)
		list->data[id] = list->data[list->data_count];

	if (list->data_count != 0)
		list->data = (struct _list_char_data *) xnrealloc(list->data, list->data_count * sizeof(struct _list_char_data));
	else
	{
		free(list->data);
		list->data = NULL;
	}
}

struct _list_char_data* list_char_find(struct _list_char *list, const char *string, int string_size, int mode)
{
	if (mode == BY_PLAIN)
	{
		int i;
		for (i = 0; i < list->data_count; i++)
		{
			struct _list_char_data *data = &list->data[i];

			if (data->string_size == string_size && strncmp(data->string, string, string_size) == 0)
				return data;
		}
	}
	else if (mode == BY_REGEXP)
	{
		int i;
		for (i = 0; i < list->data_count; i++)
		{
			struct _list_char_data *data = &list->data[i];

			if (check_regexp_match(string, data->string))
				return data;
		}
	}

	return NULL;
}

struct _list_char* list_char_clone(struct _list_char *list)
{
	struct _list_char *list_copy = list_char_init();

	int i;
	for (i = 0; i < list->data_count; i++)
		list_copy->add(list_copy, list->data[i].string);

	return list_copy;
}

int list_char_exist(struct _list_char *list, const char *string, int string_size)
{
	struct _list_char_data *data = list->find(list, string, string_size, BY_PLAIN);
	return (data != NULL);
}

int list_char_exist_regexp(struct _list_char *list, const char *string, int string_size)
{
	struct _list_char_data *data = list->find(list, string, string_size, BY_REGEXP);
	return (data != NULL);
}

struct _list_char* list_char_init(void)
{
	struct _list_char *list = (struct _list_char *) xnmalloc(sizeof(struct _list_char));
	bzero(list, sizeof(struct _list_char));

	list->uninit		= list_char_uninit;
	list->add		= list_char_add;
	list->rem		= list_char_rem;
	list->rem_by_id		= list_char_rem_by_id;
	list->find		= list_char_find;
	list->clone		= list_char_clone;
	list->exist		= list_char_exist;
	list->exist_regexp	= list_char_exist_regexp;

	return list;
}
