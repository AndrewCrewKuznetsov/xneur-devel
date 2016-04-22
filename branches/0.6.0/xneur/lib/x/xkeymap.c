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

#include "utils.h"
#include "log.h"
#include "types.h"

#include "xwindow.h"
#include "xevent.h"
#include "xkeymap.h"

extern struct _xwindow *main_window;

static int xkeymap_locale_create(struct _xkeymap *p)
{
	if (setlocale(LC_ALL,"") == NULL)
	{
		log_message(ERROR, "Сould not set default locale");
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
	int min_keycode, max_keycode, keysyms_per_keycode;
	XDisplayKeycodes (main_window->display, &min_keycode, &max_keycode);
	p->keymap = XGetKeyboardMapping (main_window->display, min_keycode, max_keycode - min_keycode + 1, &keysyms_per_keycode);
	if (!p->keymap) 
	{
		log_message(ERROR, "Unable to get keyboard mapping table");
		return FALSE;
	}
	return TRUE;
}

static void xkeymap_char_to_keycode(struct _xkeymap *p, char ch, KeyCode *kc, int *modifier)
{
	if ((ch == 10) || (ch == 13))
	{
		*kc = XKeysymToKeycode(main_window->display, XK_Return);
		*modifier = 0;
		return;
	}
	
	for (int i = p->min_keycode+1; i <= p->max_keycode; i++)
	{
		if (p->alphabet[p->latin_group][i].lower_sym[0] != NULLSYM)
			if (p->alphabet[p->latin_group][i].lower_sym[0] == ch)
			{
				*kc = i;
				*modifier = 0;
				return;
			}
		if (p->alphabet[p->latin_group][i].upper_sym[0] != NULLSYM)
			if (p->alphabet[p->latin_group][i].upper_sym[0] == ch)
			{
				*kc = i;
				*modifier = 1;
				return;
		}
	}
	kc = 0;
	modifier = 0;
	return;
}
	
char xkeymap_get_ascii(struct _xkeymap *p, const char *sym)
{
	for (int lang = 0; lang <= p->total_key_arrays; lang++)
	{
		if (lang == p->latin_group)
			continue;
		
		for (int i = p->min_keycode; i <= p->max_keycode; i++)
		{
			if (p->alphabet[lang][i].lower_sym[0] != NULLSYM)
				if (strncmp(p->alphabet[lang][i].lower_sym, sym, sizeof(char)*strlen(p->alphabet[lang][i].lower_sym)) == 0)
					return p->alphabet[p->latin_group][i].lower_sym[0];
				
			if (p->alphabet[lang][i].upper_sym[0] != NULLSYM)
				if (strncmp(p->alphabet[lang][i].upper_sym, sym, sizeof(char)*strlen(p->alphabet[lang][i].upper_sym)) == 0)
					return p->alphabet[p->latin_group][i].upper_sym[0];
		}
	}
	
	return NULLSYM;
}

char xkeymap_get_cur_ascii_char(struct _xkeymap *p, XEvent e)
{
	XKeyEvent *ke = (XKeyEvent *) &e;
	char sym;

	if (ke->state & ShiftMask)
	{
		sym = p->alphabet[p->latin_group][ke->keycode].upper_sym[0]; 
		if (sym != NULLSYM)
			return sym;
	}
	
	sym = p->alphabet[p->latin_group][ke->keycode].lower_sym[0]; 
	if (sym != NULLSYM)
		return sym;
	
	return ' ';
}

void xkeymap_convert_text_to_ascii(struct _xkeymap *p, char *text)
{
	int i, j, len = strlen(text);
	for(i = 0, j = 0; i < len; i++, j++)
	{
		if (isascii(text[i]) || isspace(text[i]))
		{
			text[j] = text[i];
			continue;
		}

		char new_symbol = p->get_ascii(p, &text[i]);

		for(; i < len - 1; i++)
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

static void xkeymap_store_keymaps(struct _xkeymap *p, int group)
{
	int min_keycode, max_keycode, keysyms_per_keycode;
	KeySym *keymap;
	XDisplayKeycodes (main_window->display, &min_keycode, &max_keycode);
	keymap = XGetKeyboardMapping (main_window->display, min_keycode, max_keycode - min_keycode + 1, &keysyms_per_keycode);
	
	p->min_keycode = min_keycode;
	p->max_keycode = max_keycode;
	if (p->total_key_arrays < group) 
		p->total_key_arrays = group;
	
	p->alphabet = xnrealloc(p->alphabet, (group + 1) * sizeof(struct _xkeymap_alpha));
	p->alphabet[group] = xnmalloc((max_keycode + 1) * sizeof(struct _xkeymap_alpha));
		
	for (int i = min_keycode; i <= max_keycode; i++) 
	{
		int  j;
		p->alphabet[group][i].lower_sym = xnmalloc(1);
		p->alphabet[group][i].lower_sym[0] = NULLSYM;
		p->alphabet[group][i].upper_sym = xnmalloc(1);
		p->alphabet[group][i].upper_sym[0] = NULLSYM;
		for (j = group*2; j <= group*2 + 1; j++) 
		{
			KeySym ks = keymap[j];

			int groups[4] = {0x00000000, 0x00002000, 0x00004000, 0x00006000};
			if (ks != NoSymbol)
			{
				XEvent event = create_basic_event();
				event.xkey.keycode		= i;
				event.xkey.state		= groups[group];
				if (j == group*2 + 1)
					event.xkey.state |= ShiftMask;
				int nbytes;
    			char str[256+1];
				nbytes = XLookupString ((XKeyEvent *) &event, str, 256, &ks, NULL);
				
				if (nbytes > 0)
				{
					if (j == group*2)
					{
						p->alphabet[group][i].lower_sym = xnmalloc(nbytes+1);
						strncpy(p->alphabet[group][i].lower_sym, str, nbytes+1);
						p->alphabet[group][i].lower_sym[nbytes] = NULLSYM;
					}
					else
					{
						p->alphabet[group][i].upper_sym = xnmalloc(nbytes+1);
						strncpy(p->alphabet[group][i].upper_sym, str, nbytes+1);
						p->alphabet[group][i].upper_sym[nbytes] = NULLSYM;
					}
				}
			}
		}
		keymap += keysyms_per_keycode;
    }
}

struct _xkeymap* xkeymap_init(void)
{
	struct _xkeymap *p = (struct _xkeymap *) xnmalloc(sizeof(struct _xkeymap));
	bzero(p, sizeof(struct _xkeymap));

	p->latin_group = 0;
	p->latin_group_mask = 0;				// Define latin group mask
	
	p->alphabet = xnmalloc(1);
	
	if (!xkeymap_init_keymaps(p))
		return NULL;
	
	if (!xkeymap_locale_create(p))
		return NULL;
	
	if (!xkeymap_define_latin_group(p))
		return NULL;
	
	p->get_ascii = xkeymap_get_ascii;
	p->get_cur_ascii_char = xkeymap_get_cur_ascii_char;
	p->convert_text_to_ascii = xkeymap_convert_text_to_ascii;
	p->store_keymaps = xkeymap_store_keymaps;
	p->char_to_keycode = xkeymap_char_to_keycode;
	return p;
}
