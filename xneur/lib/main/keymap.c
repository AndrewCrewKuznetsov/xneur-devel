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
 *  Copyright (C) 2006-2012 XNeur Team
 *
 */

#include <X11/XKBlib.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "xnconfig.h"

#include "event.h"
#include "switchlang.h"
#include "window.h"

#include "types.h"
#include "text.h"
#include "log.h"
#include "list_char.h"

#include "keymap.h"

#define keycode_to_symbol_cache_size 64
#define symbol_to_keycode_cache_size 64

struct symbol_to_keycode_pair 
{
	char* symbol; size_t symbol_size; int preferred_lang;
	KeyCode kc; int modifier; char ascii; int preferred_lang_result;
};

struct keycode_to_symbol_pair 
{
	KeyCode kc; int group; int state;
	char* symbol; size_t symbol_size;
};


#define NumlockMask 0x10

static const int keyboard_groups[]	= {0x00000000, 0x00002000, 0x00004000, 0x00006000};
static const int state_masks[]		= {0x00, 0x01, 0x80, 0x10}; // None, NumLock, Alt, Shift

static int locale_create(void)
{
	if (setlocale(LC_ALL, "") == NULL)
	{
		log_message(ERROR, _("Couldn't set default locale"));
		return FALSE;
	}

	char *locale = setlocale(LC_ALL, "");
	if (locale == NULL || (strstr(locale, "UTF") == NULL && strstr(locale, "utf") == NULL) )
		log_message(WARNING, _("Your default locale is not UTF-8"));

	log_message(DEBUG, _("Using locale %s"), locale);
	return TRUE;
}

int get_languages_mask(void)
{
	int languages_mask = 0;
	for (int group = 0; group < 4; group++)
		languages_mask = languages_mask | keyboard_groups[group];
	return languages_mask;
}

static char* keymap_keycode_to_symbol_real(struct _keymap *p, KeyCode kc, int group, int state)
{
	XEvent event;
	event.type		= KeyPress;
	event.xkey.type		= KeyPress;
	event.xkey.root		= RootWindow(p->display, DefaultScreen(p->display));
	event.xkey.subwindow	= None;
	event.xkey.same_screen	= True;
	event.xkey.display	= p->display;
	event.xkey.keycode = kc;
	event.xkey.state = 0;
	event.xkey.time		= CurrentTime;
	
	if (group >= 0)
		event.xkey.state = keyboard_groups[group];
	event.xkey.state |= state;

	char *symbol = (char *) malloc((256 + 1) * sizeof(char));

	int nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, NULL, NULL);
		
	if (nbytes <= 0)
	{
		struct _list_char *locales = list_char_init();
		locales->add(locales, "C");
		locales->add(locales, "POSIX");

		
		/*FILE *fp = popen("locale -a", "r");
		if (fp != NULL)
		{
			char buffer[1024];

			while(fgets(buffer, 1024, fp) != NULL)
			{
				buffer[strlen(buffer) - 1] = NULLSYM;
				//log_message(ERROR, "%s", buffer);
				locales->add(locales, buffer);
			}
			pclose(fp);
		}*/

		//log_message(ERROR, _("Not find symbol for keycode %d and modifier 0x%x!"), event.xkey.keycode, event.xkey.state);
		
		for (int i = 0; i < locales->data_count; i++)
		{
			if (setlocale(LC_ALL, locales->data[i].string) != NULL)
			{
				event.xkey.root		= RootWindow(p->display, DefaultScreen(p->display));
				event.xkey.display  = p->display;
				nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, NULL, NULL);
				
				setlocale(LC_ALL, ""); 
				
				if (nbytes > 0)
				{
					symbol[nbytes] = NULLSYM;
					locales->uninit(locales);
					return symbol;
				}
			}
		}

		log_message(ERROR, _("Failed to look up symbol for keycode %d and modifier 0x%x!"), event.xkey.keycode, event.xkey.state);
		log_message(ERROR, _("Try run the program with command \"env LC_ALL=<LOCALE> %s\", \nwhere LOCALE available over command \"locale -a\""), PACKAGE);
		symbol[0] = NULLSYM;
		strcat(symbol, " ");

		locales->uninit(locales);
		
	}
	else
		symbol[nbytes] = NULLSYM;

	return symbol;
}

static char* keymap_keycode_to_symbol(struct _keymap *p, KeyCode kc, int group, int state)
{
	char *symbol;
	struct keycode_to_symbol_pair *pr = NULL;

	/* Look up cache. */
	for (int i = 0; i < keycode_to_symbol_cache_size; i++) {
		pr = p->keycode_to_symbol_cache + i;
		if (pr->symbol && pr->kc == kc && pr->group == group && pr->state == state)
			goto ret;
	}

	/* Miss. */

	symbol = keymap_keycode_to_symbol_real(p, kc, group, state);
	if (!symbol)
		return symbol;

	/* Just use next cache entry. LRU makes no sense here. */
	p->keycode_to_symbol_cache_pos = (p->keycode_to_symbol_cache_pos + 1) % keycode_to_symbol_cache_size;

	pr = p->keycode_to_symbol_cache + p->keycode_to_symbol_cache_pos;

	pr->symbol_size = (strlen(symbol) + 1) * sizeof(char);
	if (pr->symbol)
		free(pr->symbol);
	pr->symbol = symbol;
	pr->kc     = kc;
	pr->group  = group;
	pr->state  = state;

	ret:

	symbol = (char *) malloc(pr->symbol_size);
	memcpy(symbol, pr->symbol, pr->symbol_size);
	return symbol;
}

int get_keycode_mod(int group)
{
	return keyboard_groups[group];
}

static void keymap_get_keysyms_by_string(struct _keymap *p, char *keyname, KeySym *lower, KeySym *upper)
{
	if (keyname == NULL)
	{
		*lower = NoSymbol;
		*upper = NoSymbol;
		return;
	}

	KeySym inbound_key = XStringToKeysym(keyname);

	KeySym *keymap = p->keymap;

	for (int i = p->min_keycode; i <= p->max_keycode; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if (p->keymap[j] == NoSymbol)
				continue;

			if (p->keymap[j] != inbound_key)
				continue;

			*lower = p->keymap[0];
			*upper = p->keymap[1];

			p->keymap = keymap;
			return;
		}

		p->keymap += p->keysyms_per_keycode;
	}

	p->keymap = keymap;
}

// Private
static int init_keymaps(struct _keymap *p)
{
	// Define all key codes and key symbols
	XDisplayKeycodes(p->display, &(p->min_keycode), &(p->max_keycode));
	p->keymap = XGetKeyboardMapping(p->display, p->min_keycode, p->max_keycode - p->min_keycode + 1, &(p->keysyms_per_keycode));
	
	if (!p->keymap)
	{
		log_message(ERROR, _("Unable to get keyboard mapping table"));
		return FALSE;
	}
	return TRUE;
}

static char keymap_get_ascii_real(struct _keymap *p, const char *sym, int* preferred_lang, KeyCode *kc, int *modifier, size_t* symbol_len)
{
	if (*sym == 10 || *sym == 13)
	{
		*kc		= XKeysymToKeycode(p->display, XK_Return);
		*modifier	= 0;
		if (symbol_len)
			*symbol_len = 1;
		return *sym;
	}

	XEvent event;
	event.type		= KeyPress;
	event.xkey.type		= KeyPress;
	event.xkey.root		= RootWindow(p->display, DefaultScreen(p->display));
	event.xkey.subwindow	= None;
	event.xkey.same_screen	= True;
	event.xkey.display	= p->display;
	event.xkey.state	= 0;
	event.xkey.keycode	= XKeysymToKeycode(p->display, XK_space);
	event.xkey.time		= CurrentTime;
	
	char *symbol		= (char *) malloc((256 + 1) * sizeof(char));
	char *prev_symbols	= (char *) malloc((256 + 1) * sizeof(char));

	int _preferred_lang = 0;
	if (preferred_lang)
		_preferred_lang = *preferred_lang;

	for (int _lang = 0; _lang < p->handle->total_languages; _lang++)
	{
		int lang = _lang;
		if (lang == 0)
			lang = _preferred_lang;
		else if (lang <= _preferred_lang)
			lang--;

		KeySym *keymap = p->keymap;
		for (int i = p->min_keycode; i <= p->max_keycode; i++)
		{
			int max = p->keysyms_per_keycode - 1;
			while (max >= 0 && keymap[max] == NoSymbol)
				max--;

			prev_symbols[0] = NULLSYM;

			for (int j = 0; j <= max; j++)
			{
				if (keymap[j] == NoSymbol)
					continue;

				for (int n = 0; n < 3; n++)
				{
					for (int m = 0; m < 3; m++) // Modifiers
					{
						event.xkey.keycode	= i;

						event.xkey.state	= get_keycode_mod(lang);
						event.xkey.state	|= state_masks[m];
						event.xkey.state	|= state_masks[n];
						int nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, NULL, NULL);
						if (nbytes <= 0)
							continue;

						symbol[nbytes] = NULLSYM;

						if (strstr(prev_symbols, symbol) != NULL)
							continue;
						strcat(prev_symbols, symbol);

						if (strncmp(sym, symbol, strlen(symbol)) != 0)
							continue;

						size_t _symbol_len = strlen(symbol);

						event.xkey.state = 0;
						event.xkey.state |= state_masks[m];
						event.xkey.state |= state_masks[n];

						nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, NULL, NULL);
						if (nbytes <= 0)
							continue;

						char sym = symbol[0];

						free(prev_symbols);
						free(symbol);
						*kc		= event.xkey.keycode;
						*modifier	= get_keycode_mod(lang) | event.xkey.state;
						if (symbol_len)
							*symbol_len = _symbol_len;
						if (preferred_lang)
							*preferred_lang = lang;
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

static char keymap_get_ascii(struct _keymap *p, const char *sym, int* preferred_lang, KeyCode *kc, int *modifier, size_t* symbol_len)
{
	struct symbol_to_keycode_pair *pr = NULL;

	int _preferred_lang = 0;
	if (preferred_lang)
		_preferred_lang = *preferred_lang;

	size_t sym_size = strlen(sym);

	/* Look up cache */

	for (int i = 0; i < symbol_to_keycode_cache_size; i++) {
		pr = p->symbol_to_keycode_cache + i;
		if (pr->symbol &&
		    pr->symbol_size <= sym_size &&
		    pr->preferred_lang == _preferred_lang &&
		    memcmp(pr->symbol, sym, pr->symbol_size) == 0)
			goto ret;
	}

	/* Miss */

	int preferred_lang_result = _preferred_lang;
	char ascii = keymap_get_ascii_real(p, sym, &preferred_lang_result, kc, modifier, &sym_size);
	if (!ascii)
		return ascii;

	p->symbol_to_keycode_cache_pos = (p->symbol_to_keycode_cache_pos + 1) % symbol_to_keycode_cache_size;

	pr = p->symbol_to_keycode_cache + p->symbol_to_keycode_cache_pos;

	pr->symbol = realloc(pr->symbol, sym_size + 1);
	memcpy(pr->symbol, sym, sym_size);
	pr->symbol[sym_size] = 0;
	pr->symbol_size = sym_size;
	pr->preferred_lang = _preferred_lang;
	pr->kc = *kc;
	pr->modifier = *modifier;
	pr->preferred_lang_result = preferred_lang_result;
	pr->ascii = ascii;

	ret:

	if (preferred_lang)
		*preferred_lang = pr->preferred_lang_result;
	if (symbol_len)
		*symbol_len = pr->symbol_size;
	*kc = pr->kc;
	*modifier = pr->modifier;
	return pr->ascii;
}

static char keymap_get_cur_ascii_char(struct _keymap *p, XEvent e)
{
	XKeyEvent *ke = (XKeyEvent *) &e;

	int mod = 0;
	if (ke->state & ShiftMask)
		mod = ShiftMask;
	if (ke->state & NumlockMask)
		mod |= NumlockMask;
	
	char *symbol = (char *) malloc((256 + 1) * sizeof(char));

	ke->state = get_keycode_mod(p->latin_group);
	ke->state |= mod;

	int nbytes = XLookupString(ke, symbol, 256, NULL, NULL);
	if (nbytes > 0)
	{
		char sym = symbol[0];
		free(symbol);
		return sym;
	}

	free(symbol);
	return ' ';
}

static void keymap_convert_text_to_ascii(struct _keymap *p, char *text, KeyCode *kc, int *kc_mod)
{
	int text_len = strlen(text);

	int j = 0;
	size_t symbol_len = 0;
	int preferred_lang = 0;
	for (int i = 0; i < text_len; i += symbol_len)
	{
		char new_symbol = p->get_ascii(p, &text[i], &preferred_lang, &kc[j], &kc_mod[j], &symbol_len);

		if (new_symbol != NULLSYM && symbol_len > 0)
			text[j++] = new_symbol;
		else
			symbol_len = 1;
	}

	text[j] = NULLSYM;
}

static char* keymap_lower_by_keymaps(struct _keymap *p, int gr, char *text)
{
	if (text == NULL)
		return NULL;

	char *symbol_old	= (char *) malloc((256 + 1) * sizeof(char));
	char *symbol_new	= (char *) malloc((256 + 1) * sizeof(char));
	char *prev_symbols	= (char *) malloc((256 + 1) * sizeof(char));

	char *newtext = strdup(text);


	KeySym *keymap = p->keymap;
	for (int i = p->min_keycode; i <= p->max_keycode; i++)
	{
		int max = p->keysyms_per_keycode - 1;
		while (max >= 0 && keymap[max] == NoSymbol)
			max--;

		prev_symbols[0] = NULLSYM;

		for (int j = 1; j <= max; j += 2)
		{
			if (keymap[j] == NoSymbol)
				continue;

			for (int m = 0; m < 4; m++) // Modifiers
			{
				for (int n = 0; n < 4; n++)
				{
					if (n == m)
						continue;

					// Get BIG symbol from keymap
					XEvent event;
					event.type		= KeyPress;
					event.xkey.type		= KeyPress;
					event.xkey.root		= RootWindow(p->display, DefaultScreen(p->display));
					event.xkey.subwindow	= None;
					event.xkey.same_screen	= True;
					event.xkey.display	= p->display;
					event.xkey.keycode	= i;
					event.xkey.state	= get_keycode_mod(gr);
					event.xkey.state	|= state_masks[m];
					event.xkey.state	|= state_masks[n];
					event.xkey.time		= CurrentTime;
					
					int nbytes = XLookupString((XKeyEvent *) &event, symbol_old, 256, NULL, NULL);
					if (nbytes <= 0)
						continue;

					symbol_old[nbytes] = NULLSYM;

					if (strstr(prev_symbols, symbol_old) != NULL)
						continue;

					// Get small symbol
					event.xkey.state	= 0;
					event.xkey.state	|= state_masks[m];
					event.xkey.state	|= state_masks[n];
					event.xkey.state	&= ~ShiftMask;

					nbytes = XLookupString((XKeyEvent *) &event, symbol_new, 256, NULL, NULL);
					if (nbytes <= 0)
						continue;

					symbol_new[nbytes] = NULLSYM;

					char *replaced = str_replace(newtext, symbol_old, symbol_new);

					free(newtext);
					newtext = replaced;
				}
			}
		}
		keymap += p->keysyms_per_keycode;
	}

	free(prev_symbols);
	free(symbol_new);
	free(symbol_old);

	return newtext;
}

static void keymap_uninit(struct _keymap *p)
{
	p->purge_caches(p);
	
	if (p->keymap != NULL)
		XFree(p->keymap);

	free(p->keycode_to_symbol_cache);
	free(p->symbol_to_keycode_cache);

	free(p);
}

static void get_offending_modifiers (struct _keymap *p)
{
	p->numlock_mask = 0;
	p->scrolllock_mask = 0;
	p->capslock_mask = 0;
	
	XModifierKeymap *modmap;
	KeyCode nlock, slock;
	static int mask_table[8] = {
								ShiftMask, LockMask, ControlMask, Mod1Mask,
								Mod2Mask, Mod3Mask, Mod4Mask, Mod5Mask
								};

	nlock = XKeysymToKeycode (p->display, XK_Num_Lock);
	slock = XKeysymToKeycode (p->display, XK_Scroll_Lock);

	/*
	* Find out the masks for the NumLock and ScrollLock modifiers,
	* so that we can bind the grabs for when they are enabled too.
	*/
	modmap = XGetModifierMapping (p->display);

	if (modmap != NULL && modmap->max_keypermod > 0)
	{
		for (int i = 0; i < 8 * modmap->max_keypermod; i++)
		{
			if (modmap->modifiermap[i] == nlock && nlock != 0)
				p->numlock_mask = mask_table[i / modmap->max_keypermod];
			else if (modmap->modifiermap[i] == slock && slock != 0)
				p->scrolllock_mask = mask_table[i / modmap->max_keypermod];
		}
	}

	p->capslock_mask = LockMask;

	if (modmap)
		XFreeModifiermap (modmap);
}

static void keymap_purge_caches(struct _keymap *p)
{
	for (int i = 0; i < keycode_to_symbol_cache_size; i++) 
	{
		struct keycode_to_symbol_pair* pr = p->keycode_to_symbol_cache + i;
		if (pr->symbol)
			free(pr->symbol),
			pr->symbol = NULL,
			pr->symbol_size = 0;
	}

	for (int i = 0; i < symbol_to_keycode_cache_size; i++) 
	{
		struct symbol_to_keycode_pair* pr = p->symbol_to_keycode_cache + i;
		if (pr->symbol)
			free(pr->symbol),
			pr->symbol = NULL,
			pr->symbol_size = 0;
	}

}

struct _keymap* keymap_init(struct _xneur_handle *handle, Display *display)
{
	struct _keymap *p = (struct _keymap *) malloc(sizeof(struct _keymap));
	bzero(p, sizeof(struct _keymap));

	p->handle = handle;
	p->display = display;
	
	if (!locale_create() || !init_keymaps(p))
	{
		free(p);
		return NULL;
	}

	p->keycode_to_symbol_cache = (struct keycode_to_symbol_pair *)calloc(keycode_to_symbol_cache_size, sizeof(struct keycode_to_symbol_pair));
	p->symbol_to_keycode_cache = (struct symbol_to_keycode_pair *)calloc(symbol_to_keycode_cache_size, sizeof(struct symbol_to_keycode_pair));
	p->keycode_to_symbol_cache_pos = 0;
	p->symbol_to_keycode_cache_pos = 0;

	get_offending_modifiers(p);

	p->purge_caches			= keymap_purge_caches;
	p->get_keysyms_by_string	= keymap_get_keysyms_by_string;
	p->keycode_to_symbol		= keymap_keycode_to_symbol;
	p->get_ascii			= keymap_get_ascii;
	p->get_cur_ascii_char		= keymap_get_cur_ascii_char;
	p->convert_text_to_ascii	= keymap_convert_text_to_ascii;
	p->lower_by_keymaps		= keymap_lower_by_keymaps;
	p->uninit			= keymap_uninit;

	return p;
}


