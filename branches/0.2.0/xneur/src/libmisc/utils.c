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

#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>

#include "log.h"
#include "utils.h"

void xntrap(int sig, sg_handler handler)
{
	struct sigaction sa;
	bzero(&sa, sizeof(sa));
	sa.sa_handler = handler;

	if (sigaction(sig, &sa, NULL) == -1)
	{
		log_message(ERROR, "Can't trap signal");
		exit(EXIT_FAILURE);
	}
}

void* xnmalloc(int bytes)
{
	void *ptr = malloc(bytes);
	if (!ptr)
	{
		log_message(ERROR, "Can't allocate %d bytes", bytes);
		exit(EXIT_FAILURE);
	}
	return ptr;
}

void* xnrealloc(void *ptr, int bytes)
{
	ptr = realloc(ptr, bytes);
	if (!ptr)
	{
		log_message(ERROR, "Can't allocate %d bytes", bytes);
		exit(EXIT_FAILURE);
	}
	return ptr;
}

int get_last_word_offset(const char *string)
{
	int len = strlen(string);
	while (len != 0 && string[len - 1] == ' ')
		len--;

	if (len == 0)
		return -1;

	while (len != 0 && string[len - 1] != ' ')
		len--;

	return len;
}

const char* get_last_word(const char *string)
{
	int offset = get_last_word_offset(string);
	if(offset == -1)
		return NULL;

	return string + offset;
}
