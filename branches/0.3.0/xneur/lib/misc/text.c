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
#include <ctype.h>

#include "utils.h"
#include "types.h"

#include "text.h"

static const char ch_up[] =
{
	'"', '{', '}', ':', '<', '>', '!', '@', '#', '$', '%', '^',
	'&', '*', '(', ')', '_', '+', '|', '?', '~'
};

static const int ch_up_len = sizeof(ch_up) / sizeof(ch_up[0]);

int is_upper_non_alpha_cyr(char symbol)
{
	int i;
	for (i = 0; i < ch_up_len; i++)
	{
		if (ch_up[i] == symbol)
			return TRUE;
	}
	return FALSE;
}

int get_last_word_offset(const char *string, int string_len)
{
	int len = string_len;
	while (len != 0 && isspace(string[len - 1]))
		len--;

	if (len == 0)
		return string_len;

	while (len != 0 && !isspace(string[len - 1]))
		len--;

	return len;
}

char* get_last_word(char *string)
{
	int len = strlen(string);

	int offset = get_last_word_offset(string, len);
	if(offset == -1)
		return NULL;

	return string + offset;
}
