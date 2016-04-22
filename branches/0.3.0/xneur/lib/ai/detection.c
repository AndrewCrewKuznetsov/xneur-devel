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
#include <stdlib.h>
#include <ctype.h>

#include "config_main.h"

#include "utils.h"
#include "log.h"
#include "list.h"
#include "types.h"

#include "detection.h"

extern struct _xneur_config *xconfig;

static char* lower_word(const char *word, int len)
{
	char *ret = (char *) xnmalloc(len + 1);

	int i;
	for (i = 0; i < len; i++)
		ret[i] = tolower(word[i]);
	ret[len] = NULLSYM;
	return ret;
}

static int trim_word(char *word, int len)
{
	while (len != 0)
	{
		if (!isspace(word[len - 1]))
			break;

		word[--len] = NULLSYM;
	}
	return len;
}

static int get_proto_hits(const char *word, int len, int lang)
{
	int i, hits = 0;
	for (i = 0; i <= len - 3; i++)
	{
		if (list_exist(xconfig->protos[lang], word + i, 3))
			hits++;
	}
	return hits;
}

static int get_proto_lang(const char *word, int len, int cur_lang)
{
	if (len < 3)
		return NO_LANGUAGE;

	int hits = get_proto_hits(word, len, cur_lang);
	if (hits == 0)
	{
		log_message(DEBUG, "   This word is ok for it's proto");
		return NO_LANGUAGE;
	}

	int lang;
	for (lang = 0; lang < xconfig->total_languages; lang++)
	{
		if (lang == cur_lang)
			continue;

		int hits = get_proto_hits(word, len, lang);
		if (hits != 0)
			continue;

		log_message(DEBUG, "   This word has no hits for %s language proto", xconfig->get_lang_name(xconfig, lang));
		return lang;
	}

	log_message(DEBUG, "   This word has hits in all languages proto");
	return NO_LANGUAGE;
}

static int get_dict_lang(const char *word, int len)
{
	int lang;
	for (lang = 0; lang < xconfig->total_languages; lang++)
	{
		if (list_exist(xconfig->dicts[lang], word, len))
		{
			log_message(DEBUG, "   Found this word in %s language dictionary", xconfig->get_lang_name(xconfig, lang));
			return lang;
		}
	}

	return NO_LANGUAGE;
}

int get_word_lang(const char *word, int cur_lang)
{
	int len = strlen(word);

	char *low_word = lower_word(word, len);
	len = trim_word(low_word, len);

	if (len == 0)
		return NO_LANGUAGE;

	log_message(DEBUG, "Processing word '%s'", low_word);

	int lang = get_dict_lang(low_word, len);

	// If not found in dictionary, try to find in proto
	if (lang == NO_LANGUAGE)
		lang = get_proto_lang(low_word, len, cur_lang);

	log_message(DEBUG, "End word processing");

	free(low_word);

	return lang;
}

int get_next_lang(int cur_lang)
{
	int next_lang = cur_lang + 1;
	if (next_lang >= xconfig->total_languages)
		next_lang = 0;

	return next_lang;
}
