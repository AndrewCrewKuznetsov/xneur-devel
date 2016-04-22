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

#ifndef _CONFIG_FILES_H_
#define _CONFIG_FILES_H_

struct _list_char* load_list_from_file(const char *dir_name, const char *file_name);
char* get_file_path_name(const char *dir_name, const char *file_name);
char* get_home_file_path_name(const char *dir_name, const char *file_name);
char* get_file_content_path(const char *dir_name, const char *file_name);
char* get_file_content(const char *file_name);
void save_list_to_file(struct _list_char *list, const char *file_name);

#endif /* _CONFIG_FILES_H_ */
