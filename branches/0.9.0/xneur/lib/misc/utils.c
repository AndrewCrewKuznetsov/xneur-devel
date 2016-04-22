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

#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "types.h"
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

char* xnreplace(char *str, char *old, char *new)
{
	if (str == NULL)
		return NULL;

	int len = strlen(str);
	int newlen = strlen(new);
	int oldlen = strlen(old);

	int max_multiplier = newlen / oldlen + 1;
	
	char *ret = (char *) malloc((len * max_multiplier + 1) * sizeof(char));
	ret[0] = NULLSYM;

	char *ret_orig = ret;

	while (TRUE)
	{
		char *old_start = strstr(str, old);
		if (old_start == NULL)
		{
			strcat(ret, str);
			break;
		}

		if (old_start != str)
			memcpy(ret, str, old_start - str);

		strcat(ret, new);
		str = old_start + oldlen;
	}

	return ret_orig;
}
