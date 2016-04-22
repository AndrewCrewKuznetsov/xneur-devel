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

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "types.h"
#include "list_char.h"
#include "log.h"

#include "xnconfig_files.h"

#define HOME_CONF_DIR		".xneur"
#define DEFAULT_MAX_PATH	4096

static int get_max_path_len(void)
{
	int max_path_len = pathconf(PACKAGE_SYSCONFDIR_DIR, _PC_PATH_MAX);
	if (max_path_len <= 0)
		return DEFAULT_MAX_PATH;
	return max_path_len;
}

static char* get_file_content(const char *file_name)
{
	struct stat sb;

	if (stat(file_name, &sb) != 0 || sb.st_size < 0)
		return NULL;

	FILE *stream = fopen(file_name, "rb");
	if (stream == NULL)
		return NULL;

	unsigned int file_len = sb.st_size;
	
	char *content = (char *) malloc((file_len + 2) * sizeof(char)); // + 1 '\0'
	if (fread(content, 1, file_len, stream) != file_len)
	{
		free(content);
		fclose(stream);
		return NULL;
	}

	content[file_len] = '\0';
	fclose(stream);

	return content;
}

char* get_file_path_name(const char *dir_name, const char *file_name)
{
#define SEARCH_IN(DIRECTORY) \
	if (dir_name == NULL)\
		snprintf(path_file, max_path_len, "%s/%s", DIRECTORY, file_name);\
	else\
		snprintf(path_file, max_path_len, "%s/%s/%s", DIRECTORY, dir_name, file_name);\
	stream = fopen(path_file, "r");\
	if (stream != NULL)\
	{\
		fclose(stream);\
		return path_file;\
	}

	int max_path_len = get_max_path_len();

	char *path_file = (char *) malloc((max_path_len + 1) * sizeof(char));
	
	// Search conf in ~/.xneur
	if (dir_name == NULL)
		snprintf(path_file, max_path_len, "%s/%s/%s", getenv("HOME"), HOME_CONF_DIR, file_name);
	else
		snprintf(path_file, max_path_len, "%s/%s/%s/%s", getenv("HOME"), HOME_CONF_DIR, dir_name, file_name);

	FILE *stream = fopen(path_file, "r");
	if (stream != NULL)
	{
		fclose(stream);
		return path_file;
	}

	SEARCH_IN(PACKAGE_SYSCONFDIR_DIR);
	SEARCH_IN("/etc/xneur");
	SEARCH_IN(PACKAGE_SHAREDIR_DIR);

	// Returning default in ~/.xneur
	if (dir_name == NULL)
		snprintf(path_file, max_path_len, "%s/%s/%s", getenv("HOME"), HOME_CONF_DIR, file_name);
	else
		snprintf(path_file, max_path_len, "%s/%s/%s/%s", getenv("HOME"), HOME_CONF_DIR, dir_name, file_name);

	return path_file;
}

char* get_home_file_path_name(const char *dir_name, const char *file_name)
{
	int max_path_len = get_max_path_len();

	char *path_file = (char *) malloc((max_path_len + 1) * sizeof(char));
	
	if (dir_name == NULL)
	{
		snprintf(path_file, max_path_len, "%s/%s", getenv("HOME"), HOME_CONF_DIR); 
		mkdir(path_file, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); 

		snprintf(path_file, max_path_len, "%s/%s/%s", getenv("HOME"), HOME_CONF_DIR, file_name);
	}
	else
	{
		snprintf(path_file, max_path_len, "%s/%s/%s", getenv("HOME"), HOME_CONF_DIR, dir_name);
		mkdir(path_file, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

		snprintf(path_file, max_path_len, "%s/%s/%s/%s", getenv("HOME"), HOME_CONF_DIR, dir_name, file_name);
	}
	return path_file;
}

struct _list_char* load_list(const char *dir_name, const char *file_name, int sort_flag)
{
	char *file_path_name = get_file_path_name(dir_name, file_name);

	char *content = get_file_content(file_path_name);
	if (content == NULL)
	{
		free(file_path_name);
		return NULL;
	}

	struct _list_char *list = list_char_init();

	list->load(list, content);

	free(content);
	free(file_path_name);

	if (sort_flag == TRUE)
		list->sort(list);

	return list;
}

int save_list(struct _list_char *list, const char *dir_name, const char *file_name)
{
	char *file_path_name = get_home_file_path_name(dir_name, file_name);

	FILE *stream = fopen(file_path_name, "w");
	if (stream == NULL)
	{
		free(file_path_name);
		return FALSE;
	}

	list->save(list, stream);

	fclose(stream);
	free(file_path_name);

	return TRUE;
}
