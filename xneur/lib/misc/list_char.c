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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "log.h"
#include "regexp.h"

#include "list_char.h"

static void swap_data(struct _list_char *list, int first, int second)
{
	struct _list_char_data temp = list->data[first];

	list->data[first] = list->data[second];
	list->data[second] = temp;
}

static void rem_by_id(struct _list_char *list, int id)
{
	free(list->data[id].string);

	if (id != list->data_count - 1)
		memmove(list->data + id, list->data + id + 1, (list->data_count - id - 1) * sizeof(struct _list_char_data));

	list->data_count--;
	struct _list_char_data *tmp = realloc(list->data, list->data_count * sizeof(struct _list_char_data));
	if (tmp != NULL) 
		list->data = tmp;
}

static int get_add_id(struct _list_char *list, const char *string)
{
	int first = 0;
	int last = list->data_count - 1;

	while (first <= last)
	{
		int cur = (first + last) / 2;

		struct _list_char_data *data = &list->data[cur];

		int cmp_result = strcmp(data->string, string);
		if (cmp_result == 0)
			return cur;

		if (cmp_result < 0)
			first = cur + 1;
		else
			last = cur - 1;

		if (first > last)
			return (first + last + 1) / 2;
	}
	return 0;
}

static int find_id(struct _list_char *list, const char *string, int mode)
{
	if (list->data_count == 0)
		return -1;
	
	if (mode == BY_PLAIN)
	{
		// Print all list for debug
		/*for (int i = 0; i < list->data_count - 1; i++)
		{
			struct _list_char_data *data = &list->data[0];
			data = &list->data[i];
			log_message (ERROR, "%d %s", i, data->string);
		}*/ 
		
		int first = 0;
		int last = list->data_count - 1;

		while (first <= last)
		{
			int cur = (first + last) / 2;
			struct _list_char_data *data = &list->data[cur];

			int cmp_result = strcmp(data->string, string);
			if (cmp_result == 0)
				return cur;

			if (cmp_result < 0)
				first = cur + 1;
			else
				last = cur - 1;
		}
		return -1;
	}

	if (mode == BY_REGEXP)
	{
		int len = 0;

		for (int i = 0; i < list->data_count; i++)
		{
			struct _list_char_data *data = &list->data[i];
			len += strlen(data->string) + 2;
		}

		char *full_str = malloc(len * sizeof(char));
		if (full_str == NULL)
			return -1;
		full_str[0] = '\0';

		for (int i = 0; i < list->data_count - 1; i++)
		{
			struct _list_char_data *data = &list->data[i];
			//data = &list->data[i];
			strcat(full_str, data->string);
			strcat(full_str, "|");
		} 

		struct _list_char_data *data;
		data = &list->data[list->data_count - 1];
		strcat(full_str, data->string);			

		if (check_regexp_match(string, full_str))
		{
			free(full_str);
			return 1;
		}

		free(full_str);
	}

	return -1;
}

static struct _list_char_data* add_last(struct _list_char *list, const char *string)
{	
	list->data_count++;
	struct _list_char_data *tmp = (struct _list_char_data *)realloc(list->data, list->data_count * sizeof(struct _list_char_data));
	if (tmp == NULL)
	{
		list->data_count--;
		log_message (ERROR, _("Function realloc return NULL."));
		return NULL;
	}

	list->data = tmp;
	struct _list_char_data *data = &list->data[list->data_count - 1];
	data->string = strdup(string);
	return data;
}

struct _list_char_data* list_char_add(struct _list_char *list, const char *string)
{
	int id = get_add_id(list, string);

	struct _list_char_data *tmp = realloc(list->data, (list->data_count + 1) * sizeof(struct _list_char_data)); 
	if (tmp == NULL)
	{
		log_message (ERROR, _("Function realloc return NULL."));
		return NULL;
	}
	
	list->data = (struct _list_char_data *)tmp;
	if (id != list->data_count)
		memmove(list->data + id + 1, list->data + id, (list->data_count - id) * sizeof(struct _list_char_data));
	list->data_count++;
	struct _list_char_data *data = &list->data[id];
	data->string = strdup(string);
	return data;
}

void list_char_rem(struct _list_char *list, const char *string)
{
	int id = find_id(list, string, BY_PLAIN);
	if (id == -1)
		return;

	rem_by_id(list, id);
}

struct _list_char_data* list_char_find(struct _list_char *list, const char *string, int mode)
{
	int id = find_id(list, string, mode);
	if (id == -1)
		return NULL;

	return &list->data[id];
}

struct _list_char_data* list_char_find_alike(struct _list_char *list, const char *string)
{
	size_t len = strlen(string);
	if (len < 4)
		return NULL;
	
	int id = get_add_id(list, string);
	if ((id == -1) || (id == list->data_count))
		return NULL;

	if (strncmp(list->data[id].string, string, len) != 0)
		return NULL;

	return &list->data[id];
}

struct _list_char* list_char_alike(struct _list_char *list, const char *string)
{
	size_t len = strlen(string);
	if (len < 4)
		return NULL;
	
	int id = get_add_id(list, string);
	if ((id == -1) || (id == list->data_count))
		return NULL;

	if (strncmp(list->data[id].string, string, len) != 0)
		return NULL;

	struct _list_char* list_alike = list_char_init();
	
	for (int i = id; i < list->data_count; i++)
	{
		if (strncmp(list->data[i].string, string, len) != 0)
			break;
		add_last(list_alike, list->data[i].string);
	}
	return list_alike;
}

int list_char_exist(struct _list_char *list, const char *string, int mode)
{
	struct _list_char_data *data = list->find(list, string, mode);
	return (data != NULL);
}

struct _list_char* list_char_clone(struct _list_char *list)
{
	struct _list_char *list_copy = list_char_init();

	for (int i = 0; i < list->data_count; i++)
		add_last(list_copy, list->data[i].string);

	list_copy->sort(list_copy);

	return list_copy;
}

void list_char_sort(struct _list_char *list)
{
	if (list->data_count <= 1)
		return;

	char *data1, *data2;

	// Insertion sort
	int i, j;
	struct _list_char_data temp;
	for (i = 1; i < list->data_count; i++) 
	{
		temp = list->data[i];
		for (j = i - 1; j >= 0; j--) 
		{
			data1 = list->data[j].string;
			data2 = temp.string;
			if (strcmp(data1, data2) < 0)
				break;
			swap_data(list, j+1, j);
		}
		list->data[j+1] = temp;
	}

	// Bubble sort
	/*for(int i = list->data_count-1 ; i > 0 ; i--)
	{
		for(int j = 0 ; j < i ; j++)
		{
			data1 = list->data[j].string;
			data2 = list->data[j+1].string;
            if (strcmp(data1, data2) >= 0)
				 swap_data(list, j, j+1);
         }
     }*/

	// Quick sort (with error!!!)
	/*int i = 2;

	do
	{
		int t = i;
		while (t != 1)
		{
			int k = t / 2;

			data1 = list->data[k - 1].string;
			data2 = list->data[t - 1].string;

			if (strcmp(data1, data2) >= 0)
				break;

			swap_data(list, k - 1, t - 1);
			t = k;
		}
		i++;
	}
	while (i <= list->data_count);

	i = list->data_count - 1;

	do
	{
		swap_data(list, i, 0);

		int t = 1;
		while (t != 0)
		{
			int k = 2 * t;
			if (k > i)
				break;

			if (k < i)
			{
				data1 = list->data[k].string;
				data2 = list->data[k - 1].string;

				if (strcmp(data1, data2) > 0)
					k++;
			}

			data1 = list->data[t - 1].string;
			if (strcmp(data1, data2) >= 0)
				break;

			swap_data(list, k - 1, t - 1);
			t = k;
		}
		i--;
	}
	while (i >= 1);
	*/
	
	for (int i = 0; i < list->data_count - 1; i++)
	{
		if (strcmp(list->data[i].string, list->data[i+1].string) > 0)
			log_message (ERROR, _("Sorting error: %s > %s"), list->data[i].string, list->data[i+1].string);
	}
}

void list_char_load(struct _list_char *list, char *content)
{
	while (1)
	{
		char *line = strsep(&content, "\n");
		if (line == NULL)
			break;

		if (line[0] == '\0')
			continue;

		add_last(list, line);
	}
}

void list_char_save(struct _list_char *list, FILE *stream)
{
	for (int i = 0; i < list->data_count; i++)
	{
		fputs(list->data[i].string, stream);
		fputs("\n", stream);
	}
}

void list_char_uninit(struct _list_char *list)
{
	for (int i = 0; i < list->data_count; i++)
		free(list->data[i].string);

	free(list->data);
	free(list);
}

struct _list_char* list_char_init(void)
{
	struct _list_char *list = (struct _list_char *) malloc(sizeof(struct _list_char));
	bzero(list, sizeof(struct _list_char));

	list->data_count = 0;
	
	list->uninit	= list_char_uninit;
	list->add	= list_char_add;
	list->rem	= list_char_rem;
	list->find	= list_char_find;
	list->find_alike	= list_char_find_alike;
	list->load	= list_char_load;
	list->save	= list_char_save;
	list->clone	= list_char_clone;
	list->alike	= list_char_alike;
	list->sort	= list_char_sort;
	list->exist	= list_char_exist;
	return list;
}
