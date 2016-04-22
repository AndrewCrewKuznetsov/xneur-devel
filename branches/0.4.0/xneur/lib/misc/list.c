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

#include "utils.h"
#include "types.h"
#include "log.h"

#include "list.h"

struct _list* list_init()
{
	struct _list *element = (struct _list *) xnmalloc(sizeof(struct _list));
	bzero(element, sizeof(struct _list));

	return element;
}

void list_uninit(struct _list *list)
{
	int i;
	for (i = 0; i < list->data_count; i++)
		free(list->data[i].string);

	free(list->data);
	free(list);
}

void list_add(struct _list *list, const char *string)
{
	list->data_count++;
	if (list->data == NULL)
		list->data = (struct _list_data *) xnmalloc(list->data_count * sizeof(struct _list_data));
	else
		list->data = (struct _list_data *) xnrealloc(list->data, list->data_count * sizeof(struct _list_data));

	struct _list_data *data = &list->data[list->data_count - 1];

	data->string		= strdup(string);
	data->string_size	= strlen(string);
}

void list_rem(struct _list *list, const char *string, int string_size)
{
	int i;
	for (i = 0; i < list->data_count; i++)
	{
		struct _list_data *data = &list->data[i];

		if (data->string_size == string_size && strncmp(data->string, string, string_size) == 0)
		{
			int j = i;
			if (j < list->data_count - 1)
			{
				for (int j=i; j < list->data_count-1; j++)
				{
					struct _list_data *prev_data = &list->data[j];
					struct _list_data *next_data = &list->data[j+1];
					memcpy(prev_data, next_data, sizeof(struct _list_data));
				}				
			}
			list->data_count--;	
			list->data = (struct _list_data *) xnrealloc(list->data, list->data_count * sizeof(struct _list_data));
		}
	}
}

struct _list_data* list_find(struct _list *list, const char *string, int string_size)
{
	int i;
	for (i = 0; i < list->data_count; i++)
	{
		struct _list_data *data = &list->data[i];

		if (data->string_size == string_size && strncmp(data->string, string, string_size) == 0)
			return data;
	}
	return NULL;
}

int list_exist(struct _list *list, const char *string, int string_size)
{
	struct _list_data *data = list_find(list, string, string_size);
	return (data != NULL);
}
