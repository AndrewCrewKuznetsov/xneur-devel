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
	int min_keycode, max_keycode;
	XDisplayKeycodes(main_window->display, &min_keycode, &max_keycode);

	int keysyms_per_keycode;
	p->keymap = XGetKeyboardMapping(main_window->display, min_keycode, max_keycode - min_keycode + 1, &keysyms_per_keycode);
	if (!p->keymap)
	{
		log_message(ERROR, "Unable to get keyboard mapping table");
		return FALSE;
	}
	return TRUE;
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

	for (int i = p->min_keycode + 1; i <= p->max_keycode; i++)
	{
		if (p->alphabet[p->latin_group][i].lower_sym[0] == ch)
		{
			*kc = i;
			*modifier = 0;
			return;
		}
	}
	
	for (int i = p->min_keycode + 1; i <= p->max_keycode; i++)
	{
		if (p->alphabet[p->latin_group][i].upper_sym[0] == ch)
		{
			*kc = i;
			*modifier = 1;
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

int get_languages_mask(void)
{
	int languages_mask = 0;
	for (int group = 0; group < groups_size; group++)
		languages_mask = languages_mask | groups[group];
	return languages_mask;
}

char xkeymap_get_ascii(struct _xkeymap *p, const char *sym)
{
	for (int lang = 0; lang < p->total_key_arrays; lang++)
	{
		if (lang == p->latin_group)
			continue;

		for (int i = p->min_keycode; i <= p->max_keycode; i++)
		{
			if (p->alphabet[lang][i].lower_sym[0] != NULLSYM && strncmp(p->alphabet[lang][i].lower_sym, sym, strlen(p->alphabet[lang][i].lower_sym)) == 0)
				return p->alphabet[p->latin_group][i].lower_sym[0];

			if (p->alphabet[lang][i].upper_sym[0] != NULLSYM && strncmp(p->alphabet[lang][i].upper_sym, sym, strlen(p->alphabet[lang][i].upper_sym)) == 0)
				return p->alphabet[p->latin_group][i].upper_sym[0];
		}
	}

	return NULLSYM;
}

char xkeymap_get_cur_ascii_char(struct _xkeymap *p, XEvent e)
{
	XKeyEvent *ke = (XKeyEvent *) &e;

	if (ke->state & ShiftMask)
	{
		char sym = p->alphabet[p->latin_group][ke->keycode].upper_sym[0];
		if (sym != NULLSYM)
			return sym;
	}

	char sym = p->alphabet[p->latin_group][ke->keycode].lower_sym[0];
	if (sym != NULLSYM)
		return sym;

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

static void xkeymap_store_keymaps(struct _xkeymap *p)
{
	int min_keycode, max_keycode;
	XDisplayKeycodes(main_window->display, &min_keycode, &max_keycode);

	int keysyms_per_keycode;
	KeySym *keymap = XGetKeyboardMapping(main_window->display, min_keycode, max_keycode - min_keycode + 1, &keysyms_per_keycode);

	p->min_keycode = min_keycode;
	p->max_keycode = max_keycode;

	int group = p->total_key_arrays;

	if (p->alphabet == NULL)
		p->alphabet = (struct _xkeymap_alpha **) xnmalloc((group + 1) * sizeof(struct _xkeymap_alpha *));
	else
		p->alphabet = (struct _xkeymap_alpha **) xnrealloc(p->alphabet, (group + 1) * sizeof(struct _xkeymap_alpha *));

	p->alphabet[group] = (struct _xkeymap_alpha *) xnmalloc((max_keycode + 1) * sizeof(struct _xkeymap_alpha));
	bzero(p->alphabet[group], (max_keycode + 1) * sizeof(struct _xkeymap_alpha));

	char *symbol = (char *) malloc((256 + 1) * sizeof(char));

	for (int i = min_keycode; i <= max_keycode; i++)
	{
		p->alphabet[group][i].lower_sym		= (char *) xnmalloc(1);
		p->alphabet[group][i].lower_sym[0]	= NULLSYM;
		p->alphabet[group][i].upper_sym		= (char *) xnmalloc(1);
		p->alphabet[group][i].upper_sym[0]	= NULLSYM;

		for (int j = 0; j < 2; j++)
		{
			KeySym ks = keymap[j + group * 2];

			if (ks == NoSymbol)
				continue;

			XEvent event		= create_basic_event();
			event.xkey.keycode	= i;
			event.xkey.state	= get_keycode_mod(group);

			if (j == 1)
				event.xkey.state |= ShiftMask;

			int nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, &ks, NULL);
			if (nbytes <= 0)
				continue;

			symbol[nbytes] = NULLSYM;

			if (j == 0)
			{
				free(p->alphabet[group][i].lower_sym);
				p->alphabet[group][i].lower_sym = strdup(symbol);
			}
			else
			{
				free(p->alphabet[group][i].upper_sym);
				p->alphabet[group][i].upper_sym = strdup(symbol);
			}
		}

		keymap += keysyms_per_keycode;
	}

	free(symbol);

	p->total_key_arrays++;
}

void xkeymap_uninit(struct _xkeymap *p)
{
	if (p->alphabet != NULL)
	{
		int group;
		for (group = 0; group < p->total_key_arrays; group++)
		{
			if (p->alphabet[group] == NULL)
				continue;

			int keycode;
			for (keycode = p->min_keycode; keycode <= p->max_keycode; keycode++)
			{
				if (p->alphabet[group][keycode].lower_sym != NULL)
					free(p->alphabet[group][keycode].lower_sym);

				if (p->alphabet[group][keycode].upper_sym != NULL)
					free(p->alphabet[group][keycode].upper_sym);
			}

			free(p->alphabet[group]);
		}

		free(p->alphabet);
	}

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
	p->uninit			= xkeymap_uninit;

	return p;
}
