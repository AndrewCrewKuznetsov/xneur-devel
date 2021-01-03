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
#include <ctype.h>

#include "types.h"
#include "log.h"

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

/*int get_last_word_offset(const char *string, int string_len)
{
	int len = string_len;
	while (len != 0 && (isspace(string[len - 1]) || (string[len - 1] == '-')))
		len--;

	if (len == 0)
		return string_len;

	while (len != 0 && !isspace(string[len - 1]) && !(string[len - 1] == '-'))
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
}*/

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

char* str_replace(const char *source, const char *search, const char *replace)
{
	if (source == NULL)
		return NULL;

	int source_len = strlen(source);
	int search_len = strlen(search);
	int replace_len = strlen(replace);

	int max_multiplier = replace_len / search_len + 1;

	size_t avail_space = source_len * max_multiplier;
	char *result = (char *) malloc((avail_space + 1) * sizeof(char));
	result[0] = '\0';

	while (TRUE)
	{
		char *found = strstr(source, search);
		if (found == NULL)
		{
			strncat(result, source, avail_space);
			break;
		}

		// Copy to result all data from source to found
		if (found != source) {
			size_t len = found - source;
			strncat(result, source, len);
			avail_space -= len;
		}

		strncat(result, replace, avail_space);
		source = found + search_len;
		avail_space -= replace_len;
	}

	return result;
}

char* real_sym_to_escaped_sym(const char *source)
{
	char *string = strdup(source);
	if (string == NULL)
		return NULL;

	char *dummy = str_replace(string, "\\", "\\\\");
	if (dummy != NULL)
	{
		free(string);
		string = dummy;
	}

	dummy = str_replace(string, "\t", "\\t");
	if (dummy != NULL)
	{
		free(string);
		string = dummy;
	}

	dummy = str_replace(string, "\n", "\\n");
	free(string);
	return dummy;
}

char* escaped_sym_to_real_sym(const char *source)
{
	char *string = strdup(source);
	if (string == NULL)
		return NULL;

	// Replace escaped-symbols
	char *dummy = str_replace(string, "\\n", "\n");
	if (dummy != NULL)
	{
		free(string);
		string = dummy;
	}

	dummy = str_replace(string, "\\t", "\t");
	if (dummy != NULL)
	{
		free(string);
		string = dummy;
	}

	dummy = str_replace(string, "\\\\", "\\");
	free(string);
	return dummy;
}

void del_final_numeric_char(char *word)
{
	int offset = 0;
	int len = strlen(word);
	for (int i = len; i>0; i--)
	{
		switch (word[i-1])
		{
			case '=':
			case '+':
			case '-':
			case '_':
			case '/':
			case '|':
			case '\\':
			case '?':
			case ';':
			case ',':
			case '.':
			case '"':
			case '@':
			case '#':
			case '$':
			case '%':
			case '^':
			case '&':
			case '*':
			case ':':
			case '!':
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			case '\'':
			case '~':
			case '`':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '0':
			case ' ':
			case 13:	// Return
			case 9:		// Tab
			{
				offset++;
				break;
			}
			default:
			{
				i = 0;
				break;
			}
		}
	}
	//if (offset == len)
		//return;
	word[len - offset] = NULLSYM;
}

/// https://en.wikipedia.org/wiki/Levenshtein_distance#Iterative_with_two_matrix_rows
static size_t distance(const char *s, size_t sLen, const char *t, size_t tLen) {
	// create two work vectors of integer distances
	size_t* v0 = (size_t*)calloc(tLen + 1, sizeof(size_t));
	size_t* v1 = (size_t*)calloc(tLen + 1, sizeof(size_t));

	// initialize v0 (the previous row of distances)
	// this row is A[0][i]: edit distance for an empty s
	// the distance is just the number of characters to delete from t
	for (size_t i = 0; i <= tLen; ++i) {
		v0[i] = i;
	}

	for (size_t i = 0; i < sLen; ++i) {
		// calculate v1 (current row distances) from the previous row v0

		// first element of v1 is A[i+1][0]
		//   edit distance is delete (i+1) chars from s to match empty t
		v1[0] = i + 1;

		size_t s_i = s[i];
		// use formula to fill in the rest of the row
		for (size_t j = 0; j < tLen; ++j) {
			// calculating costs for A[i+1][j+1]
			size_t deletionCost     = v0[j + 1] + 1;
			size_t insertionCost    = v1[j] + 1;
			size_t substitutionCost = v0[j] + (s_i == t[j] ? 0 : 1);

			size_t min = deletionCost < insertionCost ? deletionCost : insertionCost;

			v1[j + 1] = min < substitutionCost ? min : substitutionCost;
		}
		// copy v1 (current row) to v0 (previous row) for next iteration
		// since data in v1 is always invalidated, a swap without copy could be more efficient
		size_t* tmp = v0;
		v0 = v1;
		v1 = tmp;
	}
	// after the last swap, the results of v1 are now in v0
	size_t result = v0[tLen];
	free(v0);
	free(v1);

	return result;
}

size_t levenshtein(const char *s, const char *t)
{
	if (s == t) return 0;

	const size_t sLen = strlen(s);
	const size_t tLen = strlen(t);

	if (sLen == 0) return tLen;
	if (tLen == 0) return sLen;

	return distance(s, sLen, t, tLen);
}
