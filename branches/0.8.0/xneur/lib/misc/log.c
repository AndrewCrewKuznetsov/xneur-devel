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

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "types.h"
#include "colors.h"

#include "log.h"

static int LOG_LEVEL = DEBUG;

void log_set_level(int level)
{
	LOG_LEVEL = level;
}

void log_message(int level, const char *string, ...)
{
	if (level > LOG_LEVEL)
		return;

	char *modifier;
	FILE *stream = stdout;
	switch (level)
	{
		case ERROR:
		{
			modifier = RED_COLOR "[ERR]: " NORMAL_COLOR;
			stream = stderr;
			break;
		}
		case WARNING:
		{
			modifier = ORANGE_COLOR "[WRN]: " NORMAL_COLOR;
			break;
		}
		case LOG:
		{
			modifier = GREEN_COLOR "[LOG]: " NORMAL_COLOR;
			break;
		}
		default:
		case DEBUG:
		{
			modifier = TEAL_COLOR "[DBG]: " NORMAL_COLOR;
			break;
		}
	}

	int len = strlen(string) + strlen(modifier) + 2;

	char *buffer = (char *) malloc(len + 1);
	snprintf(buffer, len, "%s%s\n", modifier, string);
	buffer[len] = NULLSYM;

	va_list ap;
	va_start(ap, string);

	vfprintf(stream, buffer, ap);

	free(buffer);

	va_end(ap);
}
