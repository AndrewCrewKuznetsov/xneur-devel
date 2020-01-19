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
 *  Copyright (C) 2006-2016 XNeur Team
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "colors.h"

#include "log.h"

static int LOG_LEVEL = TRACE;

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
			modifier = RED_COLOR "[ERR] " NORMAL_COLOR;
			stream = stderr;
			break;
		}
		case WARNING:
		{
			modifier = ORANGE_COLOR "[WRN] " NORMAL_COLOR;
			break;
		}
		case LOG:
		{
			modifier = GREEN_COLOR "[LOG] " NORMAL_COLOR;
			break;
		}
		default:
		case DEBUG:
		{
			modifier = TEAL_COLOR "[DBG] " NORMAL_COLOR;
			break;
		}
		case TRACE:
		{
			modifier = BLUE_COLOR "[TRA] " NORMAL_COLOR;
			break;
		}
		case NONE:
		{
			modifier = "[NON] ";
			break;
		}
	}

	time_t curtime = time(NULL);
	struct tm *loctime = localtime(&curtime);
	char time_buffer[64];
	time_buffer[0] = 0;
	size_t time_len = loctime != NULL
		? strftime(time_buffer, sizeof(time_buffer)/sizeof(time_buffer[0]), "%T ", loctime)
		: 0;
	int len = strlen(modifier) + time_len + strlen(string) + 2;// \n + \0

	char *buffer = (char *) malloc(len + 1);
	snprintf(buffer, len, "%s%s%s\n", modifier, time_buffer, string);
	buffer[len] = 0;

	va_list ap;
	va_start(ap, string);

	vfprintf(stream, buffer, ap);

	free(buffer);
	va_end(ap);
}
