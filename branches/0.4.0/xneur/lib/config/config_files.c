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

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "utils.h"
#include "list.h"
#include "log.h"
#include "types.h"

#include "config_files.h"

#define DEFAULT_MAX_PATH 4096
#define HOME_CONF_DIR ".xneur"

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

	FILE *stream = fopen(file_name, "r");
	if (stream == NULL)
		return NULL;

	unsigned int file_len = sb.st_size;

	char *content = (char *) xnmalloc((file_len + 2) * sizeof(char)); // + 1 '\n' + 1 '\0'
	if (fread(content, 1, file_len, stream) != file_len)
	{
		free(content);
		fclose(stream);
		return NULL;
	}

	content[file_len] = '\n';
	content[file_len + 1] = '\0';
	fclose(stream);

	return content;
}

char* get_file_path_name(const char *dir_name, const char *file_name)
{
	int max_path_len = get_max_path_len();

	char *path_file = (char *) xnmalloc((max_path_len + 1) * sizeof(char));

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
	
	// Search conf in /{prefix}/etc/xneur
	log_message(DEBUG, "Not found file %s in user home directory! Try to find file in %s.", path_file, PACKAGE_SYSCONFDIR_DIR);
	if (dir_name == NULL)
		snprintf(path_file, max_path_len, PACKAGE_SYSCONFDIR_DIR "/%s", file_name);
	else
		snprintf(path_file, max_path_len, PACKAGE_SYSCONFDIR_DIR "/%s/%s", dir_name, file_name);
	stream = fopen(path_file, "r");
	if (stream != NULL)	
	{
		fclose(stream);
		return path_file;
	}
	
	// Search conf in /etc/xneur
	log_message(DEBUG, "Not found file %s in %s directory! Try to find file in %s.", path_file, PACKAGE_SYSCONFDIR_DIR, "/etc/xneur");
	if (dir_name == NULL)
		snprintf(path_file, max_path_len, "/etc/xneur/%s", file_name);
	else
		snprintf(path_file, max_path_len, "/etc/xneur/%s/%s", dir_name, file_name);
	stream = fopen(path_file, "r");
	if (stream != NULL)	
	{
		fclose(stream);
		return path_file;
	}
	
	return path_file;
}

struct _list* load_file_to_list(const char *dir_name, const char *file_name)
{
	char *file_path_name = get_file_path_name(dir_name, file_name);
	char *content = get_file_content(file_path_name);

	free(file_path_name);
	if (content == NULL)
		return NULL;

	struct _list *data = list_init();

	int len = strlen(content);

	int sym, last_sym = 0;
	for (sym = 0; sym < len; sym++)
	{
		if (content[sym] != '\n')
			continue;

		if (last_sym == sym)
		{
			last_sym++;
			continue;
		}

		content[sym] = '\0';
		list_add(data, content + last_sym);

		last_sym = sym + 1;
	}

	free(content);
	return data;
}
