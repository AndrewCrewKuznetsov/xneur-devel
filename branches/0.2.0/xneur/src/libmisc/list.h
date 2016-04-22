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

#ifndef _LIST_ARRAY_H_
#define _LIST_ARRAY_H_

typedef struct _list_data
{
	char *string;
	int string_size;
	int sub_data;
} list_data;

typedef struct _list
{
	struct _list_data *data;
	int data_count;
} list;

struct _list* list_init();
void list_uninit(struct _list *list);
void list_add(struct _list *list, const char *string, int sub_data);
struct _list_data* list_find(struct _list *list, const char *string, int string_size);
bool list_exist(struct _list *list, const char *string, int string_size);

#endif /* _LIST_ARRAY_H_ */
