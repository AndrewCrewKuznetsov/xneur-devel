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
#include <stdlib.h>

#include "xconfig.h"
#include "xdefines.h"

#include "utils.h"
#include "conv.h"
#include "log.h"
#include "list.h"

#include "detection.h"

extern struct _xconf *xconfig;

static int get_syllable_weight(const char *word)
{
	struct _list_data *data = list_find(xconfig->syllable, word, 2);
	if (data == NULL)
		return 0;

	return data->sub_data;
}

static int get_syllable_lang(const char *word, int len)
{
	if (len == 1)
	{
		switch (word[0])
		{
			case 'e':
			case 'r':
			case 'd':
			case 'f':
			case 'j':
			case 'z':
			case 'c':
			case 'b':
				return RUSSIAN;
			case 'a':
				return ENGLISH;
		}
		return NO_LANGUAGE;
	}

	int i, weight = 0;
	for (i = 0; i <= len - 1; i++)
		weight += get_syllable_weight(word + i);

	log_message(DEBUG, "Word '%s' has weight %d", word, weight);

	if (weight > 0)
		return RUSSIAN;
	else if (weight < 0)
		return ENGLISH;
	return NO_LANGUAGE;
}

static int get_proto_lang(const char *word, int len, int cur_lang)
{
	if (len < 3)
		return NO_LANGUAGE;

	int *langs_hit = (int *) xnmalloc(xconfig->TotalLanguages * sizeof(int));
	bzero(langs_hit, xconfig->TotalLanguages * sizeof(int));

	int i;
	for (i = 0; i <= len - 3; i++)
	{
		const char *syllable = word + i;

		int lang;
		for (lang = 0; lang < xconfig->TotalLanguages; lang++)
		{
			if (!list_exist(xconfig->protos[lang], syllable, 3))
				continue;

			langs_hit[lang]++;
		}
	}

	int best_lang = cur_lang;

	if (langs_hit[cur_lang] != 0)
	{
		best_lang = NO_LANGUAGE;

		int lang;
		for (lang = 0; lang < xconfig->TotalLanguages; lang++)
		{
			if (langs_hit[lang] != 0)		// Have hits in proto file for this language
				continue;

			best_lang = lang;
			break;
		}
	}

	if (best_lang != NO_LANGUAGE)
		log_message(DEBUG, "Found language %d for word '%s'", best_lang, word);

	free(langs_hit);
	return best_lang;
}

static int get_dict_lang(const char *word, int len)
{
	int lang;
	for (lang = 0; lang < xconfig->TotalLanguages; lang++)
	{
		if (list_exist(xconfig->dicts[lang], word, len))
		{
			log_message(DEBUG, "Word '%s' found in language %d dictionary", word, lang);
			return lang;
		}
	}
	return NO_LANGUAGE;
}

int get_word_lang(const char *word, int cur_lang)
{
	char *low_word = lower_word(word);
	if (low_word == NULL)
		return NO_LANGUAGE;

	int len = strlen(low_word);

	int lang = get_dict_lang(low_word, len);

	// If not found in dictionary, try to find in proto
	if (lang == NO_LANGUAGE)
		lang = get_proto_lang(low_word, len, cur_lang);

	// If not found in proto and dictionary, try to detect by syllables
	if (lang == NO_LANGUAGE)
		lang = get_syllable_lang(low_word, len);

	free(low_word);

	return lang;
}

int get_next_lang(int cur_lang)
{
	int next_lang = cur_lang + 1;
	if (next_lang >= xconfig->TotalLanguages)
		next_lang = 0;

	return next_lang;
}
