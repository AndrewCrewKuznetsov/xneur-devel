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
#   include "config.h"
#endif

#ifdef WITH_DEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list_char.h"
#include "log.h"

static struct _list_char *allocates = NULL;

static const int POINTER_LEN = sizeof(void *) * 2 + 3; // + 0x + NULLSYM

void xndebug_init(void)
{
	if (allocates != NULL)
		return;

	allocates = list_char_init();
}

void* xndebug_malloc(int len, char *file, int line)
{
	xndebug_init();

	void *mem = malloc(len);

	char pointer[POINTER_LEN];
	sprintf(pointer, "%p", mem);

	struct _list_char_data *data = allocates->add(allocates, pointer);
	data->debug_value = line;
	data->debug_string = file;
	return mem;
}

void* xndebug_calloc(int n, int size, char *file, int line)
{
	xndebug_init();

	void *mem = calloc(n, size);

	char pointer[POINTER_LEN];
	sprintf(pointer, "%p", mem);

	struct _list_char_data *data = allocates->add(allocates, pointer);
	data->debug_value = line;
	data->debug_string = file;

	return mem;
}	

void xndebug_free(void *mem, char *file, int line)
{
	xndebug_init();

	char pointer[POINTER_LEN];
	sprintf(pointer, "%p", mem);

	if (!allocates->exist(allocates, pointer, BY_PLAIN))
		log_message(ERROR, _("Freeing invalid memory pointer %p (at %s:%d)"), mem, file, line);

	allocates->rem(allocates, pointer);

	free(mem);
}

char* xndebug_strdup(const char *str, char *file, int line)
{
	xndebug_init();

	char *mem = strdup(str);

	char pointer[POINTER_LEN];
	sprintf(pointer, "%p", mem);

	struct _list_char_data *data = allocates->add(allocates, pointer);
	data->debug_value = line;
	data->debug_string = file;

	return mem;
}

void* xndebug_realloc(void *mem, int len, char *file, int line)
{
	xndebug_init();

	char pointer[POINTER_LEN];

	if (mem != NULL)
	{
		sprintf(pointer, "%p", mem);

		if (!allocates->exist(allocates, pointer, BY_PLAIN))
			log_message(ERROR, _("Reallocating invalid memory pointer %p (at %s:%d)"), mem, file, line);
		allocates->rem(allocates, pointer);
	}

	void *new_mem = realloc(mem, len);

	if (len != 0)
	{
		sprintf(pointer, "%p", new_mem);

		struct _list_char_data *data = allocates->add(allocates, pointer);
		data->debug_value = line;
		data->debug_string = file;
	}

	return new_mem;
}

void xndebug_uninit(void)
{
	if (allocates == NULL)
		return;

	if (allocates->data_count == 0)
	{
		allocates->uninit(allocates);
		return;
	}

	log_message(WARNING, _("Total %d unfreed memory pointers"), allocates->data_count);

	for (int i = 0; i < allocates->data_count; i++)
		log_message(WARNING, _("Unfreed memory pointer %s (allocated at file %s:%d)"), allocates->data[i].string, allocates->data[i].debug_string, allocates->data[i].debug_value);

	allocates->uninit(allocates);
}
#endif /* WITH_DEBUG */
