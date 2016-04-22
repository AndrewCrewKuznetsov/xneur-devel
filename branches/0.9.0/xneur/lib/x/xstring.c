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

#include <X11/Xlocale.h>
#include <X11/keysym.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>

#include "xnconfig.h"
#include "xnconfig_files.h"

#include "xwindow.h"
#include "xevent.h"
#include "xkeymap.h"
#include "xutils.h"

#include "types.h"
#include "text.h"
#include "conversion.h"
#include "log.h"

#include "xstring.h"

#define INIT_STRING_LENGTH 64

extern struct _xneur_config *xconfig;
extern struct _xwindow *main_window;

Window last_log_window = 0;

void xstring_set_key_code(struct _xstring *p, int lang)
{
	int keycode_mod = get_keycode_mod(xconfig->get_lang_group(xconfig, lang));
	int languages_mask = get_languages_mask();

	// Set new language bit
	for (int i = 0; i < p->cur_pos; i++)
	{
		p->keycode_modifiers[i] = p->keycode_modifiers[i] & (~languages_mask);
		p->keycode_modifiers[i] = p->keycode_modifiers[i] | keycode_mod;
	}
}

void xstring_savelog(struct _xstring *p, char *file_name, Window window)
{
	if ((xconfig->save_log_mode != LOG_ENABLED) || (p->cur_pos == 0) || (file_name == NULL))
		return;

	char *file_path_name = get_home_file_path_name(NULL, file_name);
	FILE *stream = fopen(file_path_name, "a");
	free(file_path_name);
	if (stream == NULL)
		return;
	
	// Get app name
	if (window != last_log_window)
	{
		last_log_window = window;
		char *app_name = get_wm_class_name(window);
		fprintf(stream, "[%s]\n", app_name);
		free(app_name);
	}
	
	// Get Date and Time stamp
	setlocale(LC_ALL, "");
	
	time_t curtime = time(NULL);
	struct tm *loctime = localtime(&curtime);
	char buffer[256];
	buffer[0] = NULLSYM;
	if (loctime != NULL)	
		strftime(buffer, 256, "%c", loctime);
	fprintf(stream, "  (%s): ", buffer);
	
	setlocale(LC_ALL, "en_US.UTF-8");
	
	for (int i=0; i<p->cur_pos; i++)
	{
		char *symbol = keycode_to_symbol(p->keycode[i], -1, p->keycode_modifiers[i]);
		
		if (symbol != NULL)
		{
			if (p->keycode[i] == 36)			// Return
				fprintf(stream, "\n");
			else if (p->keycode[i] == 23)		// Tab
				fprintf(stream, "\t");
			else								//Other symbols
				fprintf(stream, "%s", symbol);
		}
		else 
		{
			fprintf(stream, "<?>");
		}
		free(symbol);
	}
	fprintf(stream, "\n");
	
	fclose(stream);
}

void xstring_clear(struct _xstring *p)
{
	for (int i = 0; i < p->cur_pos; i++)
	{
		p->keycode[i] = 0;
		p->keycode_modifiers[i] = 0;
	}

	p->cur_pos = 0;
	p->content[0] = NULLSYM;
}

int xstring_is_space_last(struct _xstring *p)
{
	if (p->cur_pos <= 0)
		return FALSE;

	if (isspace(p->content[p->cur_pos - 1]))
		return TRUE;

	return FALSE;
}

void xstring_set_content(struct _xstring *p, const char *new_content)
{
	p->cur_pos = strlen(new_content);
	if (p->cur_pos >= p->cur_size)
	{
		p->cur_size		= p->cur_pos + 1;
		p->content		= (char *) realloc(p->content, p->cur_size * sizeof(char));
		p->keycode		= (KeyCode *) realloc(p->keycode, p->cur_size * sizeof(KeyCode));
		p->keycode_modifiers	= (int *) realloc(p->keycode_modifiers, p->cur_size * sizeof(int));
	}

	if ((p->content == NULL) || (p->keycode == NULL) || (p->keycode_modifiers == NULL))
		return;
	
	p->content[p->cur_pos] = NULLSYM;
	if (!p->cur_pos)
		return;

	memcpy(p->content, new_content, p->cur_pos);

	for (int i = 0; i < p->cur_pos; i++)
		main_window->xkeymap->char_to_keycode(main_window->xkeymap, p->content[i], &p->keycode[i], &p->keycode_modifiers[i]);
}

void xstring_changecase_content(struct _xstring *p)
{
	for (int i = 0; i < p->cur_pos; i++)
		if (p->keycode_modifiers[i] & ShiftMask)
			p->keycode_modifiers[i] = (p->keycode_modifiers[i] & ~ShiftMask);
		else
			p->keycode_modifiers[i] = (p->keycode_modifiers[i] | ShiftMask);
}

void xstring_add_symbol(struct _xstring *p, char sym, KeyCode keycode, int modifier)
{
	if (p->cur_pos == p->cur_size - 1)
	{
		p->cur_size *= 2;

		p->content		= (char *) realloc(p->content, p->cur_size * sizeof(char));
		p->keycode		= (KeyCode *) realloc(p->keycode, p->cur_size * sizeof(KeyCode));
		p->keycode_modifiers	= (int *) realloc(p->keycode_modifiers, p->cur_size * sizeof(int));
	}

	if ((p->content == NULL) || (p->keycode == NULL) || (p->keycode_modifiers == NULL))
		return;
	
	p->content[p->cur_pos] = sym;
	p->keycode[p->cur_pos] = keycode;
	p->keycode_modifiers[p->cur_pos] = modifier;

	p->cur_pos++;
	p->content[p->cur_pos] = NULLSYM;
}


void xstring_del_symbol(struct _xstring *p)
{
	if (p->cur_pos == 0)
		return;

	p->cur_pos--;
	p->content[p->cur_pos] = NULLSYM;
}

char *xstring_get_utf_string(struct _xstring *p)
{
	char *symbol		= (char *) malloc((256 + 1) * sizeof(char));
	char *prev_symbols	= (char *) malloc(sizeof(char));
	prev_symbols[0] = NULLSYM;
	int byte_count = 1;
	XEvent event		= create_basic_event();
	for (int i = 0; i < p->cur_pos; i++)
	{
		event.xkey.keycode	= p->keycode[i];
		event.xkey.state	= p->keycode_modifiers[i];
		
		int nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, NULL, NULL);
		if (nbytes <= 0)
			continue;
		
		symbol[nbytes] = NULLSYM;
		
		byte_count += nbytes;
		prev_symbols = (char *) realloc(prev_symbols, byte_count);
		if (prev_symbols != NULL)
			strcat(prev_symbols, symbol);
	}
	free(symbol);
	return prev_symbols;
}

void xstring_uninit(struct _xstring *p)
{
	free(p->keycode_modifiers);
	free(p->keycode);
	free(p->content);
	free(p);

	log_message(DEBUG, "Current string is freed");
}

struct _xstring* xstring_init(void)
{
	struct _xstring *p = (struct _xstring *) malloc(sizeof(struct _xstring));
	bzero(p, sizeof(struct _xstring));

	p->cur_size		= INIT_STRING_LENGTH;

	p->content		= (char *) malloc(p->cur_size * sizeof(char));
	p->keycode		= (KeyCode *) malloc(p->cur_size * sizeof(KeyCode));
	p->keycode_modifiers	= (int *) malloc(p->cur_size * sizeof(int));

	bzero(p->content, p->cur_size * sizeof(char));
	bzero(p->keycode, p->cur_size * sizeof(KeyCode));
	bzero(p->keycode_modifiers, p->cur_size * sizeof(int));

	// Functions mapping
	p->clear		= xstring_clear;
	p->savelog		= xstring_savelog;
	p->is_space_last	= xstring_is_space_last;
	p->set_key_code		= xstring_set_key_code;
	p->set_content		= xstring_set_content;
	p->changecase_content	= xstring_changecase_content;
	p->add_symbol		= xstring_add_symbol;
	p->del_symbol		= xstring_del_symbol;
	p->get_utf_string	= xstring_get_utf_string;
	p->uninit		= xstring_uninit;

	return p;
}
