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
#include <string.h>
#include <ctype.h>

#include "types.h"

#include "text.h"

static const char ch_up[] =
{
	'"', '{', '}', ':', '<', '>', '!', '@', '#', '$', '%', '^',
	'&', '*', '(', ')', '_', '+', '|', '?', '~'
};

static const char ch_down[] =
{
	'\'', '[', ']', ';', ',', '.', '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '0', '-', '=', '\\', '/', '`'
};

static const int ch_up_len = sizeof(ch_up) / sizeof(ch_up[0]);

int is_upper_non_alpha_cyr(char symbol)
{
	for (int i = 0; i < ch_up_len; i++)
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
	if (offset == -1)
		return NULL;

	return string + offset;
}

int trim_word(char *word, int len)
{
	while (len != 0)
	{
		if (!isspace(word[len - 1]))
			break;

		word[--len] = NULLSYM;
	}
	return len;
}

char full_tolower(char sym)
{
	if (!isalpha(sym))
	{
		for (int i = 0; i < ch_up_len; i++)
		{
			if (ch_up[i] == sym)
				return ch_down[i];
		}
	}
	return tolower(sym);
}

void lower_word_inplace(char *word)
{
	int len = strlen(word);

	for (int i = 0; i < len; i++)
		word[i] = full_tolower(word[i]);
}

char* lower_word(const char *word, int len)
{
	char *ret = (char *) malloc(len + 1);

	for (int i = 0; i < len; i++)
		ret[i] = full_tolower(word[i]);
	ret[len] = NULLSYM;

	return ret;
}
