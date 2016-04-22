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

#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "utils.h"
#include "log.h"
#include "list.h"

#include "files.h"

extern char *HOME;
extern int MAX_PATH_LEN;
extern char *CNF_DIR;

char* get_file_path_name(const char *file_name)
{
	char *path_file = (char *) xnmalloc(MAX_PATH_LEN + 1);
	snprintf(path_file, MAX_PATH_LEN, "%s/%s/%s", HOME, CNF_DIR, file_name);
	return path_file;
}

static char* get_file_content(const char *file_name)
{
	struct stat sb;

	if (stat(file_name, &sb) != 0 || sb.st_size < 0)
	{
		log_message(ERROR, "Can't stat file %s", file_name);
		return NULL;
	}

	FILE *stream = fopen(file_name, "r");
	if (stream == NULL)
	{
		log_message(ERROR, "Can't open file %s", file_name);
		return NULL;
	}

	unsigned int file_len = sb.st_size;

	char *content = (char *) xnmalloc((file_len + 2) * sizeof(char)); // + 1 '\n' + 1 '\0'
	if (fread(content, 1, file_len, stream) != file_len)
	{
		free(content);
		fclose(stream);
		log_message(ERROR, "Can't read file %s", file_name);
		return NULL;
	}

	content[file_len] = '\n';
	content[file_len + 1] = NULLSYM;
	fclose(stream);

	return content;
}

struct _list* load_file_to_list(const char *file_name)
{
	char *file_path_name = get_file_path_name(file_name);
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

		content[sym] = NULLSYM;
		list_add(data, content + last_sym, 0);

		last_sym = sym + 1;
	}

	free(content);
	return data;
}

void delete_file(const char *file_name)
{
	char *file_path_name = get_file_path_name(file_name);

	unlink(file_path_name);

	free(file_path_name);
}
