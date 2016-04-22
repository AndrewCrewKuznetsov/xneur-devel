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

#ifndef _LIST_CHAR_H_
#define _LIST_CHAR_H_

#include <stdio.h>

#define BY_PLAIN	0 // Search by plain text
#define BY_REGEXP	1 // Search by regular expression

struct _list_char_data
{
	char *string;

#ifdef WITH_DEBUG
	int  debug_value;
	char *debug_string;
#endif
};

struct _list_char
{
	struct _list_char_data *data;
	int data_count;

	void (*uninit) (struct _list_char *list);
	struct _list_char_data* (*add) (struct _list_char *list, const char *string);
	struct _list_char_data* (*add_last) (struct _list_char *list, const char *string);
	struct _list_char_data* (*find) (struct _list_char *list, const char *string, int mode);
	struct _list_char* (*clone) (struct _list_char *list);
	void (*load)  (struct _list_char *list, char *content);
	void (*save)  (struct _list_char *list, FILE *stream);
	int  (*exist) (struct _list_char *list, const char *string, int mode);
	void (*rem)   (struct _list_char *list, const char *string);
	void (*sort)  (struct _list_char *list);
};

struct _list_char* list_char_init(void);

#endif /* _LIST_CHAR_H_ */
