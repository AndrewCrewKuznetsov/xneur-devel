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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "switchlang.h"

#include "keymap.h"
#include "window.h"

#include "types.h"
#include "utils.h"
#include "list_char.h"
#include "log.h"
#include "text.h"

#include "detection.h"

#define PROTO_LEN	2
#define BIG_PROTO_LEN	3
#define LEVENSHTEIN_LEN	3

static int get_dictionary_lang(struct _xneur_handle *handle, char **word)
{
	for (int lang = 0; lang < handle->total_languages; lang++)
	{
		if (handle->languages[lang].disable_auto_detection || handle->languages[lang].excluded)
			continue;
		if (strlen(word[lang]) == 0)
			continue;
		if (handle->languages[lang].dictionary->exist(handle->languages[lang].dictionary, word[lang], BY_REGEXP))
		{
			log_message(DEBUG, _("   [+] Found this word in %s language dictionary"), handle->languages[lang].name);
			return lang;
		}
	}

	log_message(DEBUG, _("   [-] This word not found in any dictionaries"));
	return NO_LANGUAGE;
}

#ifdef WITH_ASPELL
static int get_aspell_hits(struct _xneur_handle *handle, char **word, int **sym_len, int cur_lang)
{
	// check for current language first
	if (handle->has_spell_checker[cur_lang])
	{
		if (strlen(word[cur_lang]) > 0)
		{
			if (strlen(word[cur_lang]) / sym_len[cur_lang][0] > 1)
			{
				if (!handle->languages[cur_lang].disable_auto_detection && !handle->languages[cur_lang].excluded &&
					aspell_speller_check(handle->spell_checkers[cur_lang], word[cur_lang], strlen(word[cur_lang])))
				{
					log_message(DEBUG, _("   [+] Found this word in %s aspell dictionary"), handle->languages[cur_lang].name);
					return cur_lang;
				}
			}
		}
	}
	else
	{
		log_message(DEBUG, _("   [!] Now we don't support aspell dictionary for %s layout"), handle->languages[cur_lang].name);
	}

	// check for another languages
	for (int lang = 0; lang < handle->total_languages; lang++)
	{
		if (handle->languages[lang].disable_auto_detection || handle->languages[lang].excluded || lang == cur_lang)
			continue;

		if (strlen(word[lang]) == 0)
			continue;

		if (!handle->has_spell_checker[lang])
		{
			log_message(DEBUG, _("   [!] Now we don't support aspell dictionary for %s layout"), handle->languages[lang].name);
			continue;
		}

		if (strlen(word[lang]) / sym_len[lang][0] > 1)
		{
			if (aspell_speller_check(handle->spell_checkers[lang], word[lang], strlen(word[lang])))
			{
				log_message(DEBUG, _("   [+] Found this word in %s aspell dictionary"), handle->languages[lang].name);
				return lang;
			}
		}
	}


	log_message(DEBUG, _("   [-] This word has no hits for all aspell dictionaries"));
	return NO_LANGUAGE;
}
#endif

#ifdef WITH_ENCHANT
static int get_enchant_hits(struct _xneur_handle *handle, char **word, int **sym_len, int cur_lang)
{
	// check for current language first
	if (handle->has_enchant_checker[cur_lang])
	{
		if (strlen(word[cur_lang]) > 0)
		{
			if ((int)(strlen(word[cur_lang]) / sym_len[cur_lang][0]) > 1)
			{
				if (!handle->languages[cur_lang].disable_auto_detection && !handle->languages[cur_lang].excluded &&
					!enchant_dict_check(handle->enchant_dicts[cur_lang], word[cur_lang], strlen(word[cur_lang])))
				{
					log_message(DEBUG, _("   [+] Found this word in %s enchant wrapper dictionary"), handle->languages[cur_lang].name);
					return cur_lang;
				}
			}
		}
	}
	else
	{
		log_message(DEBUG, _("   [!] Now we don't support enchant wrapper dictionary for %s layout"), handle->languages[cur_lang].name);
	}

	// check for another languages
	for (int lang = 0; lang < handle->total_languages; lang++)
	{
		if (handle->languages[lang].disable_auto_detection || handle->languages[lang].excluded || lang == cur_lang || (strlen(word[lang]) <= 0)) 
			continue;

		if (strlen(word[lang]) == 0)
			continue;

		if (!handle->has_enchant_checker[lang])
		{
			log_message(DEBUG, _("   [!] Now we don't support enchant wrapper dictionary for %s layout"), handle->languages[lang].name);
			continue;
		}

		if (strlen(word[lang]) / sym_len[lang][0] > 1)
		{
			if (!enchant_dict_check(handle->enchant_dicts[lang], word[lang], strlen(word[lang])))
			{
				log_message(DEBUG, _("   [+] Found this word in %s enchant wrapper dictionary"), handle->languages[lang].name);
				return lang;
			}
		}
	}

	log_message(DEBUG, _("   [-] This word has no hits for all enchant wrapper dictionaries"));
	return NO_LANGUAGE;
}
#endif				

static int get_proto_hits(struct _xneur_handle *handle, char *word, int *sym_len, int len, int offset, int lang)
{
	int n_bytes = 0;
	for (int i = 0; i < PROTO_LEN; i++)
		n_bytes += sym_len[i];

	char *proto = (char *) malloc((n_bytes + 1) * sizeof(char));

	int local_offset = 0;
	for (int i = 0; i <= len - offset - PROTO_LEN; i++)
	{
		strncpy(proto, word + local_offset, n_bytes);
		proto[n_bytes] = NULLSYM;

		if (handle->languages[lang].proto->exist(handle->languages[lang].proto, proto, BY_PLAIN))
		{
			free(proto);
			return TRUE;
		}

		local_offset += sym_len[i];
	}

	free(proto);
	return FALSE;
}

static int get_big_proto_hits(struct _xneur_handle *handle, char *word, int *sym_len, int len, int offset, int lang)
{
	int n_bytes = 0;
	for (int i = 0; i < BIG_PROTO_LEN; i++)
		n_bytes += sym_len[i];

	char *proto = (char *) malloc((n_bytes + 1) * sizeof(char));

	int local_offset = 0;
	for (int i = 0; i <= len - offset - BIG_PROTO_LEN; i++)
	{
		strncpy(proto, word+local_offset, n_bytes);
		proto[n_bytes] = NULLSYM;

		if (handle->languages[lang].proto->exist(handle->languages[lang].big_proto, proto, BY_PLAIN))
		{
			free(proto);
			return TRUE;
		}

		local_offset += sym_len[i];
	}

	free(proto);
	return FALSE;
}

static int get_proto_lang(struct _xneur_handle *handle, char **word, int **sym_len, int len, int offset, int cur_lang, int proto_len)
{
	int (*get_proto_hits_function) (struct _xneur_handle *handle, char *word, int *sym_len, int len, int offset, int lang);

	if (proto_len == PROTO_LEN)
		get_proto_hits_function = get_proto_hits;
	else
		get_proto_hits_function = get_big_proto_hits;

	if (len < proto_len)
	{
		log_message(DEBUG, _("   [-] Skip checking by language proto of size %d (word is very short)"), proto_len);
		return NO_LANGUAGE;
	}

	int hits = get_proto_hits_function(handle, word[cur_lang], sym_len[cur_lang], len, offset, cur_lang);
	if (hits == 0)
	{
		log_message(DEBUG, _("   [-] This word is ok for %s proto of size %d"), handle->languages[cur_lang].name, proto_len);
		return cur_lang;
	}

	log_message(DEBUG, _("   [*] This word has hits for %s proto of size %d"), handle->languages[cur_lang].name, proto_len);

	for (int lang = 0; lang < handle->total_languages; lang++)
	{
		if ((lang == cur_lang) || (handle->languages[lang].disable_auto_detection) || (handle->languages[lang].excluded))
			continue;

		if (strlen(word[lang]) == 0)
			continue;

		int hits = get_proto_hits_function(handle, word[lang], sym_len[lang], len, offset, lang);
		if (hits != 0)
		{
			log_message(DEBUG, _("   [*] This word has hits for %s language proto of size %d"), handle->languages[lang].name, proto_len);
			continue;
		}

		log_message(DEBUG, _("   [+] This word has no hits for %s language proto of size %d"), handle->languages[lang].name, proto_len);
	}

	log_message(DEBUG, _("   [-] This word has hits in all languages proto of size %d"), proto_len);
	return NO_LANGUAGE;
}

static int get_similar_words(struct _xneur_handle *handle, struct _buffer *p)
{
	int min_levenshtein = LEVENSHTEIN_LEN;
	char *possible_words = NULL;
	int possible_lang = NO_LANGUAGE;
	
	int lang = 0;
	for (lang = 0; lang < handle->total_languages; lang++)
	{
		char *word = strdup(p->get_last_word(p, p->i18n_content[lang].content));
		if (word == NULL)
			continue;
		
		del_final_numeric_char(word);
		
		if (handle->languages[lang].disable_auto_detection || handle->languages[lang].excluded)
		{
			if (possible_words != NULL)
				free (possible_words);
			free(word);
			continue;
		}

		int word_len = strlen(p->get_last_word(p, p->content));
		
		if ((word_len > 250) || (word_len < 2))
		{
			if (possible_words != NULL)
				free (possible_words);
			free(word);
			continue;
		}

		word_len = strlen(word); 
		
		int offset = 0;
		for (offset = 0; offset < word_len; offset++)
		{
			if (!ispunct(word[offset]))
				break;
		}
		
#ifdef WITH_ENCHANT 
		size_t count = 0;
		
		if (!handle->has_enchant_checker[lang])
		{
			if (possible_words != NULL)
				free (possible_words);
			free(word);
			continue;
		}

		char **suggs = enchant_dict_suggest (handle->enchant_dicts[lang], word+offset, strlen(word+offset), &count); 
		if (count > 0)
		{
			for (unsigned int i = 0; i < count; i++)
			{
				int tmp_levenshtein = levenshtein(word+offset, suggs[i]);
				if (tmp_levenshtein < min_levenshtein)
				{
					min_levenshtein = tmp_levenshtein;
					if (possible_words != NULL)
						free(possible_words);
					possible_words = strdup(suggs[i]);
					possible_lang = lang;
					
				}
				//log_message (ERROR, "    %d. %s (%d)", i+1, suggs[i], levenshtein(word, suggs[i]));	
			}			
		}
		enchant_dict_free_string_list(handle->enchant_dicts[lang], suggs);
#endif

#ifdef WITH_ASPELL
		if (!handle->has_spell_checker[lang])
		{
			if (possible_words != NULL)
				free (possible_words);
			if (word != NULL)
				free(word);
			continue;
		}
		const AspellWordList *suggestions = aspell_speller_suggest (handle->spell_checkers[lang], (const char *) word+offset, strlen(word+offset));
		if (! suggestions)
		{
			if (possible_words != NULL)
				free (possible_words);
			if (word != NULL)
				free(word);
			continue;
		}

		AspellStringEnumeration *elements = aspell_word_list_elements(suggestions);
		const char *sugg_word;
		int i = 0;
		while ((sugg_word = aspell_string_enumeration_next (elements)) != NULL)
		{
			int tmp_levenshtein = levenshtein(word+offset, sugg_word);
			if (tmp_levenshtein < min_levenshtein)
			{
				min_levenshtein = tmp_levenshtein;
				if (possible_words != NULL)
					free(possible_words);
				possible_words = strdup(sugg_word);
				possible_lang = lang;
				
			}
			i++;
			//log_message (ERROR, "    %d. %s (%d) (%d)", i, sugg_word, levenshtein(word, sugg_word), damerau_levenshtein(word, sugg_word, 1, 1, 1, 1));	
		}

		delete_aspell_string_enumeration (elements);
#endif
		free(word);
	}
	
	if (possible_words == NULL)
	{
		log_message(DEBUG, _("   [-] This word has no suggest for all dictionaries")); 
		return possible_lang;

	}
	
	log_message(DEBUG, _("   [+] Found suggest word '%s' in %s dictionary (Levenshtein distance = %d)"),  
						possible_words, handle->languages[possible_lang].name, min_levenshtein);
	free (possible_words); 
	return possible_lang;
}

int check_lang(struct _xneur_handle *handle, struct _buffer *p, int cur_lang)
{
	char **word = (char **) malloc((handle->total_languages + 1) * sizeof(char *));
	char **word_base = (char **) malloc((handle->total_languages + 1) * sizeof(char *));
	char **word_unchanged = (char **) malloc((handle->total_languages + 1) * sizeof(char *));
	char **word_unchanged_base = (char **) malloc((handle->total_languages + 1) * sizeof(char *));
	int **sym_len = (int **) malloc((handle->total_languages + 1) * sizeof(int *));

	log_message(DEBUG, _("Processing word:"));
	for (int i = 0; i < handle->total_languages; i++)
	{
		word[i] = strdup(p->get_last_word(p, p->i18n_content[i].content));
		word_base[i] = word[i];
		del_final_numeric_char(word[i]);

		unsigned int offset = 0;
		for (offset = 0; offset < strlen(word[i]); offset++)
		{
			if (!ispunct(word[i][offset]) && (!isdigit(word[i][offset])))
				break;
		}
		word[i] = word[i] + offset;
		
		word_unchanged[i] = strdup(p->get_last_word(p, p->i18n_content[i].content_unchanged));
		word_unchanged_base[i] = word_unchanged[i];
		word_unchanged[i] = word_unchanged[i] + offset;
		del_final_numeric_char(word_unchanged[i]);
		
		log_message(DEBUG, _("   '%s' on layout '%s'"), word_unchanged[i], handle->languages[i].dir);

		sym_len[i] = p->i18n_content[i].symbol_len + p->get_last_word_offset(p, p->content, strlen(p->content));
	}

	log_message(DEBUG, _("Start word processing..."));

	// Check by dictionary
	int lang = get_dictionary_lang(handle, word);

#ifdef WITH_ENCHANT
	// Check by enchant wrapper
	if (lang == NO_LANGUAGE)
		lang = get_enchant_hits(handle, word, sym_len, cur_lang);
#endif

#ifdef WITH_ASPELL
	// Check by aspell
	if (lang == NO_LANGUAGE)
		lang = get_aspell_hits(handle, word, sym_len, cur_lang);
#endif
	
	// If not found in dictionary, try to find in proto
	int len = strlen(p->content);
	int offset = p->get_last_word_offset(p, p->content, len);
	if (lang == NO_LANGUAGE)
		lang = get_proto_lang(handle, word, sym_len, len, offset, cur_lang, PROTO_LEN);

	if (lang == NO_LANGUAGE)
		lang = get_proto_lang(handle, word, sym_len, len, offset, cur_lang, BIG_PROTO_LEN);

	log_message(DEBUG, _("End word processing."));

	for (int i = 0; i < handle->total_languages; i++)
	{
		free(word_base[i]);
		free(word_unchanged_base[i]);
	}
	
	free(word);
	free(word_unchanged);
	free(word_base);
	free(word_unchanged_base);
	free(sym_len);
	return lang;
}

int check_lang_with_similar_words (struct _xneur_handle *handle, struct _buffer *p, int cur_lang)
{
	char **word = (char **) malloc((handle->total_languages + 1) * sizeof(char *));
	char **word_base = (char **) malloc((handle->total_languages + 1) * sizeof(char *));
	char **word_unchanged = (char **) malloc((handle->total_languages + 1) * sizeof(char *));
	char **word_unchanged_base = (char **) malloc((handle->total_languages + 1) * sizeof(char *));
	int **sym_len = (int **) malloc((handle->total_languages + 1) * sizeof(int *));

	log_message(DEBUG, _("Processing word:"));
	for (int i = 0; i < handle->total_languages; i++)
	{
		word[i] = strdup(p->get_last_word(p, p->i18n_content[i].content));
		word_base[i] = word[i];
		del_final_numeric_char(word[i]);

		unsigned int offset = 0;
		for (offset = 0; offset < strlen(word[i]); offset++)
		{
			if (!ispunct(word[i][offset]) && (!isdigit(word[i][offset])))
				break;
		}
		word[i] = word[i] + offset;
		
		word_unchanged[i] = strdup(p->get_last_word(p, p->i18n_content[i].content_unchanged));
		word_unchanged_base[i] = word_unchanged[i];
		word_unchanged[i] = word_unchanged[i] + offset;
		del_final_numeric_char(word_unchanged[i]);
		
		log_message(DEBUG, _("   '%s' on layout '%s'"), word_unchanged[i], handle->languages[i].dir);

		sym_len[i] = p->i18n_content[i].symbol_len + p->get_last_word_offset(p, p->content, strlen(p->content));
	}

	log_message(DEBUG, _("Start word processing..."));

	// Check by dictionary
	int lang = get_dictionary_lang(handle, word);

#ifdef WITH_ENCHANT
	// Check by enchant wrapper
	if (lang == NO_LANGUAGE)
		lang = get_enchant_hits(handle, word, sym_len, cur_lang);
#endif

#ifdef WITH_ASPELL
	// Check by aspell
	if (lang == NO_LANGUAGE)
		lang = get_aspell_hits(handle, word, sym_len, cur_lang);
#endif

	// Check misprint
	if (lang == NO_LANGUAGE)
		lang = get_similar_words (handle, p);
	
	// If not found in dictionary, try to find in proto
	int len = strlen(p->content);
	int offset = p->get_last_word_offset(p, p->content, len);
	if (lang == NO_LANGUAGE)
		lang = get_proto_lang(handle, word, sym_len, len, offset, cur_lang, PROTO_LEN);

	if (lang == NO_LANGUAGE)
		lang = get_proto_lang(handle, word, sym_len, len, offset, cur_lang, BIG_PROTO_LEN);

	log_message(DEBUG, _("End word processing."));

	for (int i = 0; i < handle->total_languages; i++)
	{
		free(word_base[i]);
		free(word_unchanged_base[i]);
	}
	free(word);
	free(word_unchanged);
	free(word_base);
	free(word_unchanged_base);
	free(sym_len);
	return lang;
} 

