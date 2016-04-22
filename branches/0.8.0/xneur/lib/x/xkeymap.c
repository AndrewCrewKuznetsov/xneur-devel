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
 *  (c) XNeur Team 2007
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <X11/Xlocale.h>
#include <X11/keysym.h>

#include "utils.h"
#include "log.h"
#include "types.h"

#include "xwindow.h"
#include "xevent.h"
#include "xkeymap.h"

static const int groups[4] = {0x00000000, 0x00002000, 0x00004000, 0x00006000};
static const int groups_size = sizeof(groups) / sizeof(groups[0]);
static const unsigned int masktable[3] = {0x00, 0x01, 0x80};

extern struct _xwindow *main_window;

static int xkeymap_locale_create(struct _xkeymap *p)
{
	if (setlocale(LC_ALL, "ru_RU.UTF-8") == NULL)
	{
		log_message(ERROR, "Couldn't set default locale");
		return FALSE;
	}
	return TRUE;
}

static int xkeymap_define_latin_group(struct _xkeymap *p)
{
	p->latin_group = 0;
	p->latin_group_mask = 0;
	return TRUE;

	// Check latin group
	/*XEvent event 		= create_basic_event();
	event.xkey.keycode	= XKeysymToKeycode(main_window->display, XK_k);

	int groups[4] = {0x00000000, 0x00002000, 0x00004000, 0x00006000};
	KeySym ks;

	for (int i = 0; i < 4; i++)
	{
		event.xkey.state = groups[i];
		int nbytes;
    			char str[256+1];
				nbytes = XLookupString ((XKeyEvent *) &event, str, 256, &ks, NULL);

		if ((nbytes > 0) && (isascii(str[0])))
		{
			p->latin_group = i;
			p->latin_group_mask = groups[i];
			log_message(DEBUG, "Latin Group Mask Defined As 0x%x For Group %d", p->latin_group_mask, p->latin_group);
			return TRUE;
		}
	}
	return FALSE;*/
}

static int xkeymap_init_keymaps(struct _xkeymap *p)
{
	// Define all key codes and key symbols
	XDisplayKeycodes(main_window->display, &(p->min_keycode), &(p->max_keycode));

	p->keymap = XGetKeyboardMapping(main_window->display, p->min_keycode, p->max_keycode - p->min_keycode + 1, &(p->keysyms_per_keycode));
	if (!p->keymap)
	{
		log_message(ERROR, "Unable to get keyboard mapping table");
		return FALSE;
	}
	return TRUE;
}

int get_languages_mask(void)
{
	int languages_mask = 0;
	for (int group = 0; group < groups_size; group++)
		languages_mask = languages_mask | groups[group];
	return languages_mask;
}

char* keycode_to_symbol(KeyCode kc, int group, int state)
{
	XEvent event = create_basic_event();
	event.xkey.keycode = kc;
	event.xkey.state = groups[group];
	event.xkey.state |= state;

	KeySym ks;
	char *symbol = (char *) xnmalloc((256 + 1) * sizeof(char));

	int nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, &ks, NULL);
	if (nbytes > 0)
		return symbol;
	
	return NULL;		
}

static void xkeymap_char_to_keycode(struct _xkeymap *p, char ch, KeyCode *kc, int *modifier)
{
	*modifier = 0;
	*kc = 0;
	
	if (ch == 10 || ch == 13)
	{
		*kc = XKeysymToKeycode(main_window->display, XK_Return);
		return;
	}

	XEvent event = create_basic_event();
	char *symbol = (char *) xnmalloc((256 + 1) * sizeof(char));

	for (int i = p->min_keycode + 1; i <= p->max_keycode; i++)
	{
		event.xkey.keycode = i;
		event.xkey.state = 0;

		KeySym ks;
		int nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, &ks, NULL);
		if (nbytes <= 0)
			continue;
		
		if (symbol[0] == ch)
		{
			*kc = i;
			*modifier = 0;
			free(symbol);
			return;
		}
	}
	
	for (int i = p->min_keycode + 1; i <= p->max_keycode; i++)
	{
		event.xkey.keycode = i;
		event.xkey.state = ShiftMask;

		KeySym ks;
		int nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, &ks, NULL);
		if (nbytes <= 0)
			continue;
		
		if (symbol[0] == ch)
		{
			*kc = i;
			*modifier = 1;
			free(symbol);
			return;
		}
	}
}

int get_keycode_mod(int group)
{
	if (group >= groups_size)
		group = 0;

	return groups[group];
}

char xkeymap_get_ascii(struct _xkeymap *p, const char *sym)
{
	char *symbol		= (char *) xnmalloc((256 + 1) * sizeof(char));
	char *prev_symbols	= (char *) xnmalloc((256 + 1) * sizeof(char));
	
	for (int lang = 0; lang < p->total_key_arrays; lang++)
	{
		if (lang == p->latin_group)
			continue;
		
		KeySym *keymap = p->keymap;	
		for (int i = p->min_keycode; i <= p->max_keycode; i++)
		{		
			int max = p->keysyms_per_keycode - 1;
			while ((max >= 0) && (keymap[max] == NoSymbol))
				max--;
	
			prev_symbols[0] = NULLSYM;
	
			for (int j = 0; j <= max; j++) 
			{
				KeySym ks = keymap[j];
		
				if (ks == NoSymbol)
					continue;	

				for (int n = 0; n < 3; n++)
				{
					for (int m = 0; m < 3; m++) // Modifiers
					{
						XEvent event		= create_basic_event();
						event.xkey.keycode	= i;
						event.xkey.state	= get_keycode_mod(lang);
						event.xkey.state	|= masktable[m];
						event.xkey.state	|= masktable[n];
						
						int nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, &ks, NULL);
						if (nbytes <= 0)
							continue;
	
						symbol[nbytes] = NULLSYM;
				
						if (strstr(prev_symbols, symbol) != NULL)
							continue;
			
						strcat(prev_symbols, symbol);

						if (strncmp(sym, symbol, strlen(symbol)) != 0)
							continue;

						event.xkey.state = 0;
						event.xkey.state |= masktable[m];
						event.xkey.state |= masktable[n];

						nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, &ks, NULL);
						if (nbytes <= 0)
							continue;

						char sym = symbol[0];

						free(prev_symbols);
						free(symbol);

						return sym;
					}
				}
			}
			keymap += p->keysyms_per_keycode;
		}
	}

	free(prev_symbols);
	free(symbol);

	return NULLSYM;
}

char xkeymap_get_cur_ascii_char(struct _xkeymap *p, XEvent e)
{
	XKeyEvent *ke = (XKeyEvent *) &e;

	char *symbol = (char *) xnmalloc((256 + 1) * sizeof(char));
	
	if (ke->state & ShiftMask)
	{
		ke->state = get_keycode_mod(p->latin_group);
		ke->state |= ShiftMask;

		KeySym ks;
		int nbytes = XLookupString(ke, symbol, 256, &ks, NULL);
		if (nbytes > 0)
		{
			char sym = symbol[0];
			free(symbol);
			return sym;
		}
	}

	ke->state = get_keycode_mod(p->latin_group);

	KeySym ks;
	int nbytes = XLookupString(ke, symbol, 256, &ks, NULL);
	if (nbytes > 0)
	{
		char sym = symbol[0];
		free(symbol);
		return sym;
	}

	return ' ';
}

void xkeymap_convert_text_to_ascii(struct _xkeymap *p, char *text)
{
	int text_len = strlen(text);

	int i, j;
	for(i = 0, j = 0; i < text_len; i++, j++)
	{
		if (isascii(text[i]) || isspace(text[i]))
		{
			text[j] = text[i];
			continue;
		}

		char new_symbol = p->get_ascii(p, &text[i]);

		for(; i < text_len - 1; i++)
		{
			if (isascii(text[i + 1]) || isspace(text[i + 1]))
				break;

			if (p->get_ascii(p, &text[i + 1]) != NULLSYM)
				break;
		}

		text[j] = new_symbol;
	}

	text[j] = NULLSYM;
}

void get_keysyms_by_string(char *keyname, KeySym *Lower, KeySym *Upper)
{
	KeySym inbound_key = XStringToKeysym(keyname);
	
	int min_keycode, max_keycode;
	XDisplayKeycodes(main_window->display, &min_keycode, &max_keycode);

	int keysyms_per_keycode;
	KeySym *keymap = XGetKeyboardMapping(main_window->display, min_keycode, max_keycode - min_keycode + 1, &keysyms_per_keycode);

	for (int i = min_keycode; i <= max_keycode; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			KeySym ks = keymap[j];

			if (ks == NoSymbol)
				continue;

			if (inbound_key == ks)
			{
				
				if (j == 1)
				{
					*Upper = ks;
					*Lower = keymap[0];
				}

				if (j == 0)
				{
					*Lower = ks;
					*Upper = keymap[1];
				}
				return;
			}
		}

		keymap += keysyms_per_keycode;
	}
}

static void xkeymap_store_keymaps(struct _xkeymap *p)
{
	p->total_key_arrays++;
}

void xkeymap_print_keymaps(struct _xkeymap *p)
{
	char *symbol		= (char *) xnmalloc((256 + 1) * sizeof(char));
	char *prev_symbols	= (char *) xnmalloc((256 + 1) * sizeof(char));

	for (int gr = 0; gr < p->total_key_arrays; gr++)
	{	
		printf("New Language!\n");

		KeySym *keymap = p->keymap;		
		for (int i = p->min_keycode; i <= p->max_keycode; i++)
		{		
			int max = p->keysyms_per_keycode - 1;
			while (max >= 0 && keymap[max] == NoSymbol)
				max--;
			
			prev_symbols[0] = NULLSYM;
		
			for (int j = 0; j <= max; j++) 
			{
				KeySym ks = keymap[j];
			
				if (ks == NoSymbol)
					continue;	

				for (int n = 0; n < 3; n++)
				{
					for (int m = 0; m < 3; m++) // Modifiers
					{
						XEvent event		= create_basic_event();
						event.xkey.keycode	= i;
						event.xkey.state	= get_keycode_mod(gr);
						event.xkey.state	|= masktable[m];
						event.xkey.state	|= masktable[n];

						int nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, &ks, NULL);
						if (nbytes <= 0)
							continue;
		
						symbol[nbytes] = NULLSYM;
					
						if (strstr(prev_symbols, symbol) != NULL)
							continue;
				
						strcat(prev_symbols, symbol);
						printf(" %s", strdup(symbol));
					}
				}
			}

			keymap += p->keysyms_per_keycode;
			printf("\n");
		}
	}
	
	free(prev_symbols);
	free(symbol);
}
	
char* xkeymap_lower_by_keymaps(struct _xkeymap *p, int gr, char *sym, int *len)
{
	char *symbol		= (char *) xnmalloc((256 + 1) * sizeof(char));
	char *prev_symbols	= (char *) xnmalloc((256 + 1) * sizeof(char));
	
	KeySym *keymap = p->keymap;		
	for (int i = p->min_keycode; i <= p->max_keycode; i++)
	{		
		int max = p->keysyms_per_keycode - 1;
		while ((max >= 0) && (keymap[max] == NoSymbol))
			max--;
	
		prev_symbols[0] = NULLSYM;
	
		for (int j = 1; j <= max; j += 2) 
		{
			KeySym ks = keymap[j];
		
			if (ks == NoSymbol)
				continue;	

			for (int m = 0; m < 3; m++) // Modifiers
			{
				XEvent event		= create_basic_event();
				event.xkey.keycode	= i;
				event.xkey.state	= get_keycode_mod(gr);
				event.xkey.state	|= masktable[m];
				event.xkey.state	|= ShiftMask;

				int nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, &ks, NULL);
				if (nbytes <= 0)
					continue;
	
				symbol[nbytes] = NULLSYM;
				
				if (strstr(prev_symbols, symbol) != NULL)
					continue;
			
				strcat(prev_symbols, symbol);
				if (memcmp(sym, symbol, strlen(symbol)) != 0)
					continue;

				*len = strlen(symbol);
				event.xkey.state &= ~ShiftMask;

				nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, &ks, NULL);
				if (nbytes <= 0)
					continue;

				free(prev_symbols);
				return symbol;
			}
		}
		keymap += p->keysyms_per_keycode;
	}
	
	free(prev_symbols);
	free(symbol);

	*len = 1;
	return NULL;
}
	
void xkeymap_uninit(struct _xkeymap *p)
{
	if (p->keymap != NULL)
		XFree(p->keymap);

	free(p);
}

struct _xkeymap* xkeymap_init(void)
{
	struct _xkeymap *p = (struct _xkeymap *) xnmalloc(sizeof(struct _xkeymap));
	bzero(p, sizeof(struct _xkeymap));

	if (!xkeymap_init_keymaps(p) || !xkeymap_locale_create(p) || !xkeymap_define_latin_group(p))
	{
		free(p);
		return NULL;
	}

	p->get_ascii			= xkeymap_get_ascii;
	p->get_cur_ascii_char		= xkeymap_get_cur_ascii_char;
	p->convert_text_to_ascii	= xkeymap_convert_text_to_ascii;
	p->store_keymaps		= xkeymap_store_keymaps;
	p->char_to_keycode		= xkeymap_char_to_keycode;
	p->print_keymaps		= xkeymap_print_keymaps;
	p->lower_by_keymaps		= xkeymap_lower_by_keymaps;
	p->uninit			= xkeymap_uninit;

	return p;
}

