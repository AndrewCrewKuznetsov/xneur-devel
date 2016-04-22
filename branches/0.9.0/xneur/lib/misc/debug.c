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
 *  Copyright (C) 2006-2008 XNeur Team
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

static const int pointer_len = sizeof(int) * 2 + 3; // + 0x + NULLSYM

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

	char *pointer = (char *) malloc(pointer_len * sizeof(char));
	sprintf(pointer, "%p", mem);

	log_message(TRACE, "Allocating memory pointer %p (at %s:%d)", mem, file, line);

	struct _list_char_data *data = allocates->add(allocates, pointer);
	data->debug_value = line;
	data->debug_string = file;

	free(pointer);

	return mem;	
}

void xndebug_free(void *mem, char *file, int line)
{
	xndebug_init();

	char *pointer = (char *) malloc(pointer_len * sizeof(char));
	sprintf(pointer, "%p", mem);

	if (!allocates->exist(allocates, pointer, BY_PLAIN))
		log_message(ERROR, "Freeing invalid memory pointer %p (at %s:%d)", mem, file, line);
	else
		log_message(TRACE, "Freeing memory pointer %p (at %s:%d)", mem, file, line);

	allocates->rem(allocates, pointer);

	free(pointer);

	free(mem);
}

char* xndebug_strdup(const char *str, char *file, int line)
{
	xndebug_init();

	char *mem = strdup(str);

	char *pointer = (char *) malloc(pointer_len * sizeof(char));
	sprintf(pointer, "%p", mem);

	log_message(TRACE, "Duping memory pointer %p (at %s:%d)", str, file, line);

	struct _list_char_data *data = allocates->add(allocates, pointer);
	data->debug_value = line;
	data->debug_string = file; 

	free(pointer);

	return mem;	
}

void* xndebug_realloc(void *mem, int len, char *file, int line)
{
	xndebug_init();

	char *pointer = (char *) malloc(pointer_len * sizeof(char));

	if (mem != NULL)
	{
		sprintf(pointer, "%p", mem);

		if (!allocates->exist(allocates, pointer, BY_PLAIN))
			log_message(ERROR, "Reallocating invalid memory pointer %p (at %s:%d)", mem, file, line);
		else
			log_message(TRACE, "(Re)Freeing memory pointer %p (at %s:%d)", mem, file, line);
		allocates->rem(allocates, pointer);
	}

	void *new_mem = realloc(mem, len);

	if (len != 0)
	{
		sprintf(pointer, "%p", new_mem);

		log_message(TRACE, "(Re)Allocating memory pointer %p (at %s:%d)", new_mem, file, line);

		struct _list_char_data *data = allocates->add(allocates, pointer);
		data->debug_value = line;
		data->debug_string = file; 
	}

	free(pointer);

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

	log_message(WARNING, "Total %d unfreed memory pointers", allocates->data_count);

	for (int i = 0; i < allocates->data_count; i++)
		log_message(WARNING, "Unfreed memory pointer %s (allocated at file %s:%d)", allocates->data[i].string, allocates->data[i].debug_string, allocates->data[i].debug_value);

	allocates->uninit(allocates);
}
#endif /* WITH_DEBUG */
