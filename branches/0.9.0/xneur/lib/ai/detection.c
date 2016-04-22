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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#ifdef WITH_ASPELL
#  include <aspell.h>
#endif

#include "xnconfig.h"

#include "xkeymap.h"
#include "xwindow.h"

#include "types.h"
#include "utils.h"
#include "list_char.h"
#include "log.h"
#include "text.h"

#include "detection.h"

#define PROTO_LEN	2
#define BIG_PROTO_LEN	3

extern struct _xneur_config *xconfig;
extern struct _xwindow *main_window;
	
static int get_proto_hits(const char *word, int len, int lang)
{
	char *proto = (char *) malloc((PROTO_LEN + 1) * sizeof(char));
	proto[PROTO_LEN] = NULLSYM;

	int hits = 0;
	for (int i = 0; i <= len - PROTO_LEN; i++)
	{
		memcpy(proto, word, PROTO_LEN);

		if (xconfig->languages[lang].protos->exist(xconfig->languages[lang].protos, proto, BY_PLAIN))
			hits++;
	}

	free(proto);
	return hits;
}

static int get_big_proto_hits(const char *word, int len, int lang)
{
	char *proto = (char *) malloc((BIG_PROTO_LEN + 1) * sizeof(char));
	proto[BIG_PROTO_LEN] = NULLSYM;

	int hits = 0;
	for (int i = 0; i < len - BIG_PROTO_LEN; i++)
	{
		memcpy(proto, word, BIG_PROTO_LEN);

		if (xconfig->languages[lang].big_protos->exist(xconfig->languages[lang].big_protos, proto, BY_PLAIN))
			hits++;
	}

	free(proto);
	return hits;
}

static int get_proto_lang(const char *word, int len, int cur_lang, int proto_len)
{
	int (*get_proto_hits_function) (const char *word, int len, int lang);

	if (proto_len == PROTO_LEN)
		get_proto_hits_function = get_proto_hits;
	else
		get_proto_hits_function = get_big_proto_hits;

	if (len < proto_len)
	{
		log_message(DEBUG, "   [-]Skip checking by language proto of size %d (word is very short)", proto_len);
		return NO_LANGUAGE;
	}
	
	int hits = get_proto_hits_function(word, len, cur_lang);
	if (hits == 0)
	{
		log_message(DEBUG, "   [-]This word is ok for %s proto of size %d", xconfig->get_lang_name(xconfig, cur_lang), proto_len);
		return NO_LANGUAGE;
	}

	for (int lang = 0; lang < xconfig->total_languages; lang++)
	{
		if (lang == cur_lang)
			continue;

		int hits = get_proto_hits_function(word, len, lang);
		if (hits != 0)
			continue;

		log_message(DEBUG, "   [+]This word has no hits for %s language proto of size %d", xconfig->get_lang_name(xconfig, lang), proto_len);
		return lang;
	}

	log_message(DEBUG, "   [-]This word has hits in all languages proto of size %d", proto_len);
	return NO_LANGUAGE;
}

static int get_dict_lang(const char *word)
{
	for (int lang = 0; lang < xconfig->total_languages; lang++)
	{
		if (xconfig->languages[lang].dicts->exist(xconfig->languages[lang].dicts, word, BY_PLAIN))
		{
			log_message(DEBUG, "   [+]Found this word in %s language dictionary", xconfig->get_lang_name(xconfig, lang));
			return lang;
		}
	}
	
	log_message(DEBUG, "   [-]This word not found in any dictionaries");		
	return NO_LANGUAGE;
}

static int get_regexp_lang(const char *word)
{
	for (int lang = 0; lang < xconfig->total_languages; lang++)
	{
		if (xconfig->languages[lang].regexp->exist(xconfig->languages[lang].regexp, word, BY_REGEXP))
		{
			log_message(DEBUG, "   [+]Found this word in %s language regular expressions file", xconfig->get_lang_name(xconfig, lang));
			return lang;
		}
	}
	
	log_message(DEBUG, "   [-]This word not found in any regular expressions files");
	return NO_LANGUAGE;
}

static int get_letter_order_hits(const char *word, int len, int cur_lang)
{
	for (int i = 0; i < len; i++)
	{
		if (strrchr(xconfig->languages[cur_lang].consonant_letter, word[i]) == NULL)
			return TRUE;
	}
	return FALSE;
}

static int get_letter_order_lang(const char *word, int len, int cur_lang)
{
	if (len < 2)
		return NO_LANGUAGE;

	if (get_letter_order_hits(word, len, cur_lang) == TRUE)
	{
		log_message(DEBUG, "   [-]This word is ok for %s letter order", xconfig->get_lang_name(xconfig, cur_lang));
		return NO_LANGUAGE;
	}

	for (int lang = 0; lang < xconfig->total_languages; lang++)
	{
		if (lang == cur_lang)
			continue;

		if (get_letter_order_hits(word, len, lang) == FALSE)
			continue;

		log_message(DEBUG, "   [+]This word has no hits by letter order for %s language", xconfig->get_lang_name(xconfig, lang));
		return lang;
	}
	
	log_message(DEBUG, "   [-]This word has hits by letter order for all languages");
	return NO_LANGUAGE;
}

static int get_nofirst_letter_lang(const char *word, int cur_lang)
{
	if (strrchr(xconfig->languages[cur_lang].nofirst_letter, word[0]) == NULL)
	{
		log_message(DEBUG, "   [-]This word is ok for %s first letter check", xconfig->get_lang_name(xconfig, cur_lang));
		return NO_LANGUAGE;
	}

	for (int lang = 0; lang < xconfig->total_languages; lang++)
	{
		if (lang == cur_lang)
			continue;

		if (strrchr(xconfig->languages[cur_lang].nofirst_letter, word[0]) == NULL)
			continue;

		log_message(DEBUG, "   [+]This word has no hits by no first letter for %s language proto", xconfig->get_lang_name(xconfig, lang));
		return lang;
	}
	
	log_message(DEBUG, "   [-]This word has hits by no first letter for all languages");
	return NO_LANGUAGE;
}

#ifdef WITH_ASPELL 
static int get_aspell_hits(const char *word, int len)
{
	if (len < 2) 
	{
		log_message(DEBUG, "   [-]Skip aspell checking (word is very short)");
		return NO_LANGUAGE;	
	}

	AspellConfig *spell_config = new_aspell_config();
	
	for (int lang = 0; lang < xconfig->total_languages; lang++)
	{
		char *lang_word = (char *) malloc(1 * sizeof(char));
		if (lang_word == NULL)
			continue;
		lang_word[0] = NULLSYM;

		for (int i = 0; i < len; i++)
		{
			KeyCode kc;
			int modifier;
			main_window->xkeymap->char_to_keycode(main_window->xkeymap, word[i], &kc, &modifier);

			char *symbol = keycode_to_symbol(kc, lang, modifier);
			if ((symbol == NULL) || (lang_word == NULL))
				continue;
			lang_word = (char *) realloc(lang_word, (strlen(lang_word) + strlen(symbol) + 1) * sizeof(char));
			if (lang_word != NULL)
				strcat(lang_word, symbol);

			free(symbol);
		}
		
		if (lang_word == NULL)
			continue;

		aspell_config_replace(spell_config, "lang", xconfig->languages[lang].dir);
		AspellCanHaveError *possible_err = new_aspell_speller(spell_config);
		AspellSpeller *spell_checker = 0;

		if (aspell_error_number(possible_err) == 0)
		{
			spell_checker = to_aspell_speller(possible_err);
			int correct = aspell_speller_check(spell_checker, lang_word, strlen(lang_word));
			if (correct)
			{
				log_message(DEBUG, "   [+]Found this word in %s aspell dictionary", xconfig->get_lang_name(xconfig, lang));
				delete_aspell_speller(spell_checker);
				free(lang_word);
				return lang;
			}
		}
		else
		{
			log_message(DEBUG, "   [!]Error aspell checking for %s aspell dictionary", xconfig->get_lang_name(xconfig, lang));
		}
	
		delete_aspell_speller(spell_checker);
		free(lang_word);
	}
	
	log_message(DEBUG, "   [-]This word has no hits for all aspell dictionaries");
	return NO_LANGUAGE;
}
#endif

int get_word_lang(const char *word, int cur_lang)
{
	int len = strlen(word);

	char *low_word = lower_word(word, len);
	len = trim_word(low_word, len);

	if (len == 0)
	{
		free(low_word);
		return NO_LANGUAGE;
	}

	log_message(DEBUG, "Processing word '%s'", low_word);

	// Check by regexp
	int lang = get_regexp_lang(low_word);
	
	// Check by dictionary
	if (lang == NO_LANGUAGE)
		lang = get_dict_lang(low_word);

	// Check by aspell
#ifdef WITH_ASPELL 
	if (lang == NO_LANGUAGE)
		lang = get_aspell_hits(low_word, len);
#endif
	
	// If not found lang by proto, try to find in all letter order
	if (lang == NO_LANGUAGE)
		lang = get_letter_order_lang(low_word, len, cur_lang);
	
	// Try to find in no first symbol
	if (lang == NO_LANGUAGE)
		lang = get_nofirst_letter_lang(low_word, cur_lang);
	
	// If not found in dictionary, try to find in proto
	if (lang == NO_LANGUAGE)
		lang = get_proto_lang(low_word, len, cur_lang, BIG_PROTO_LEN);

	if (lang == NO_LANGUAGE)
		lang = get_proto_lang(low_word, len, cur_lang, PROTO_LEN);
	
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
