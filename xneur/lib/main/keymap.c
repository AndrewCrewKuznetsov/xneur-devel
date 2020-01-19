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
 *  Copyright (C) 2006-2018 XNeur Team
 *
 */

#include <X11/XKBlib.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <locale.h>

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

static const int KEYBOARD_GROUPS[]	= {0x00000000, 0x00002000, 0x00004000, 0x00006000};
static const int STATE_MASKS[]		= {0x00, 0x01, 0x80}; // None, NumLock, Alt

static int locale_create(void)
{
	char *locale = setlocale(LC_ALL, "");
	if (locale == NULL)
	{
		log_message(ERROR, _("Couldn't set default locale"));
		return FALSE;
	}

	if ((strstr(locale, "UTF") == NULL) && (strstr(locale, "utf") == NULL))
		log_message(WARNING, _("Your default locale is not UTF-8"));

	log_message(DEBUG, _("Using locale %s"), locale);
	return TRUE;
}

int get_languages_mask(void)
{
	int languages_mask = 0;
	for (int group = 0; group < sizeof(KEYBOARD_GROUPS) / sizeof(KEYBOARD_GROUPS[0]); ++group)
		languages_mask = languages_mask | KEYBOARD_GROUPS[group];
	return ~languages_mask;
}
int get_keycode_mod(int group)
{
	return KEYBOARD_GROUPS[group];
}

static char* keymap_keycode_to_symbol_real(struct _keymap *p, KeyCode kc, int state)
{
	XKeyEvent event;
	event.type        = KeyPress;
	event.root        = RootWindow(p->display, DefaultScreen(p->display));
	event.subwindow   = None;
	event.same_screen = True;
	event.display     = p->display;
	event.keycode     = kc;
	event.state       = state;
	event.time        = CurrentTime;

	char *symbol = (char *) malloc((256 + 1) * sizeof(char));
	symbol[0] = NULLSYM;

	int nbytes = XLookupString(&event, symbol, 256, NULL, NULL);

	if (nbytes <= 0)
	{
		struct _list_char *locales = list_char_init();
		locales->add(locales, "C");
		locales->add(locales, "POSIX");


		for (int i = 0; i < locales->data_count; i++)
		{
			if (setlocale(LC_ALL, locales->data[i].string) != NULL)
			{
				event.root    = RootWindow(p->display, DefaultScreen(p->display));
				event.display = p->display;
				nbytes = XLookupString(&event, symbol, 256, NULL, NULL);

				setlocale(LC_ALL, "");

				if (nbytes > 0)
				{
					symbol[nbytes] = NULLSYM;
					locales->uninit(locales);
					return symbol;
				}
			}
		}

		log_message(ERROR, _("Failed to look up symbol for keycode %d and modifier 0x%x!"), event.keycode, event.state);
		log_message(ERROR, _("Try run the program with command \"env LC_ALL=<LOCALE> %s\", \nwhere LOCALE available over command \"locale -a\""), PACKAGE);
		symbol[0] = NULLSYM;
		strncat(symbol, " ", 1);

		locales->uninit(locales);

	}
	else
	{
		symbol[nbytes] = NULLSYM;
	}

	return symbol;
}

static char* keymap_keycode_to_symbol(struct _keymap *p, KeyCode kc, int group, int state)
{
	struct keycode_to_symbol_pair *pr = NULL;

	/* Look up cache. */
	for (int i = 0; i < keycode_to_symbol_cache_size; i++) {
		pr = p->keycode_to_symbol_cache + i;
		if (pr->symbol && pr->kc == kc && pr->group == group && pr->state == state)
			goto ret;
	}

	/* Miss. */
	char *symbol = keymap_keycode_to_symbol_real(p, kc, group >= 0 ? (state | KEYBOARD_GROUPS[group]) : state);

	/* Just use next cache entry. LRU makes no sense here. */
	p->keycode_to_symbol_cache_pos = (p->keycode_to_symbol_cache_pos + 1) % keycode_to_symbol_cache_size;

	pr = p->keycode_to_symbol_cache + p->keycode_to_symbol_cache_pos;

	pr->symbol_size = (strlen(symbol) + 1) * sizeof(char);
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

static void keymap_get_keysyms_by_string(struct _keymap *p, char *keyname, KeySym *lower, KeySym *upper)
{
	*lower = NoSymbol;
	*upper = NoSymbol;
	if (keyname == NULL)
	{
		return;
	}

	KeySym inbound_key = XStringToKeysym(keyname);

	KeySym *keymap = p->keymap;

	for (int i = p->min_keycode; i <= p->max_keycode; i++)
	{
		for (int j = 0; j < p->keysyms_per_keycode; j++)
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
static char keymap_get_ascii_real(struct _keymap *p, const char *sym, int* preferred_lang, KeyCode *kc, int *modifier, size_t* symbol_len)
{
	if (*sym == 10 || *sym == 13)
	{
		*kc		= XKeysymToKeycode(p->display, XK_Return);
		*modifier	= 0;
		*symbol_len = 1;
		return *sym;
	}

	int count = p->handle->total_languages;
	int lang = *preferred_lang;
	// Loop through all languages, starting from `preferred_lang`
	while (count-- > 0) {
		KeySym *keymap = p->keymap;
		// Check all physical keys of the keyboard
		for (int keycode = p->min_keycode; keycode <= p->max_keycode; ++keycode)
		{
			// Symbols, that has text in one language, but doesn't have in `p->latin_group`
			char no_text_in_latin_lang[256 + 1];
			no_text_in_latin_lang[0] = '\0';
			// Available space in prev_symbols
			size_t avail_space = sizeof(no_text_in_latin_lang) / sizeof(no_text_in_latin_lang[0]) - 1;

			// Loop through all symbols on the physical key
			for (int j = 0; j <= p->keysyms_per_keycode; j++)
			{
				if (keymap[j] == NoSymbol)
					continue;

				// TODO: Why Shift and Ctrl not checked in the original code?
				const size_t MOD_COUNT = sizeof(STATE_MASKS) / sizeof(STATE_MASKS[0]);
				// Check all modifier pairs
				for (size_t n = 0; n < MOD_COUNT; ++n)
				{
					for (size_t m = 0; m < MOD_COUNT; ++m) // Modifiers
					{
						// No need to check "<X> + Alt + Alt" and so on
						if (n == m) continue;

						int mask = STATE_MASKS[n] | STATE_MASKS[m];
						int mod = KEYBOARD_GROUPS[lang] | mask;

						XKeyEvent event;
						event.type        = KeyPress;
						event.root        = RootWindow(p->display, DefaultScreen(p->display));
						event.subwindow   = None;
						event.same_screen = True;
						event.display     = p->display;
						event.time        = CurrentTime;
						event.keycode     = keycode;

						// Get text on the key produced by pressing key with specified modifiers in the specified keyboard layout
						event.state = mod;
						char symbol[256 + 1];
						int nbytes = XLookupString(&event, symbol, sizeof(symbol)/sizeof(symbol[0]) - 1, NULL, NULL);
						if (nbytes <= 0)
							continue;

						symbol[nbytes] = '\0';


						// If that is not the searched symbol, go next
						if (strncmp(sym, symbol, nbytes) != 0)
							continue;

						// If such symbol already produced by the another combination, go next
						if (strstr(no_text_in_latin_lang, symbol) != NULL)
							continue;
						strncat(no_text_in_latin_lang, symbol, avail_space);
						avail_space -= nbytes;

						// Symbol `sym` is produced by that key combination. Check that symbol exists
						// in the layout with latin symbols that and return it if true. If symbol doesn't
						// exists in the latin layout, go next
						event.state = KEYBOARD_GROUPS[p->latin_group] | mask;
						if (XLookupString(&event, symbol, sizeof(symbol)/sizeof(symbol[0]) - 1, NULL, NULL) <= 0) {
							continue;
						}

						*kc = keycode;
						*modifier = mod;
						*symbol_len = nbytes;
						*preferred_lang = lang;
						return symbol[0];
					}
				}
			}
			keymap += p->keysyms_per_keycode;
		}
		// Try to check next keyboard layout
		lang = (lang + 1) % p->handle->total_languages;
	}

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
	{
		return ascii; // Return empty
	}

	p->symbol_to_keycode_cache_pos = (p->symbol_to_keycode_cache_pos + 1) % symbol_to_keycode_cache_size;

	pr = p->symbol_to_keycode_cache + p->symbol_to_keycode_cache_pos;

	char *tmp = realloc(pr->symbol, sym_size + 1);
	if (tmp == NULL)
		return ascii; // Return symbol, but cache not update
	pr->symbol = tmp;
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

static char keymap_get_cur_ascii_char(struct _keymap *p, XEvent *e)
{
	XKeyEvent *ke = (XKeyEvent *)e;

	int mod = 0;
	if (ke->state & ShiftMask)
		mod = ShiftMask;
	if (ke->state & NumlockMask)
		mod |= NumlockMask;
	if (ke->state & LockMask)
		mod = LockMask;

	ke->state = KEYBOARD_GROUPS[p->latin_group];
	ke->state |= mod;

	char symbol[256 + 1];
	int nbytes = XLookupString(ke, symbol, sizeof(symbol)/sizeof(symbol[0]) - 1, NULL, NULL);
	return nbytes > 0 ? symbol[0] : ' ';
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


static void get_offending_modifiers (struct _keymap *p)
{
	p->numlock_mask = 0;
	p->scrolllock_mask = 0;
	p->capslock_mask = LockMask;

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

	if (modmap)
		XFreeModifiermap (modmap);
}

static void keymap_purge_caches(struct _keymap *p)
{
	for (int i = 0; i < keycode_to_symbol_cache_size; i++)
	{
		struct keycode_to_symbol_pair* pr = p->keycode_to_symbol_cache + i;
		free(pr->symbol);
		pr->symbol = NULL;
		pr->symbol_size = 0;
	}

	for (int i = 0; i < symbol_to_keycode_cache_size; i++)
	{
		struct symbol_to_keycode_pair* pr = p->symbol_to_keycode_cache + i;
		free(pr->symbol);
		pr->symbol = NULL;
		pr->symbol_size = 0;
	}

}

static void keymap_uninit(struct _keymap *p)
{
	keymap_purge_caches(p);

	if (p->keymap != NULL)
		XFree(p->keymap);

	free(p->keycode_to_symbol_cache);
	free(p->symbol_to_keycode_cache);

	free(p);
}


struct _keymap* keymap_init(struct _xneur_handle *handle, Display *display)
{
	if (!locale_create())
	{
		return NULL;
	}

	int min_keycode;
	int max_keycode;
	int keysyms_per_keycode;
	// Define all key codes and key symbols
	XDisplayKeycodes(display, &min_keycode, &max_keycode);
	KeySym* keymap = XGetKeyboardMapping(display, min_keycode, max_keycode - min_keycode + 1, &keysyms_per_keycode);

	if (!keymap)
	{
		log_message(ERROR, _("Unable to get keyboard mapping table"));
		return NULL;
	}

	struct _keymap *p = (struct _keymap *) malloc(sizeof(struct _keymap));
	memset(p, 0, sizeof(struct _keymap));

	p->handle = handle;
	p->display = display;
	p->keymap = keymap;
	p->min_keycode = min_keycode;
	p->max_keycode = max_keycode;
	p->keysyms_per_keycode = keysyms_per_keycode;


	p->keycode_to_symbol_cache = (struct keycode_to_symbol_pair *)calloc(keycode_to_symbol_cache_size, sizeof(struct keycode_to_symbol_pair));
	p->symbol_to_keycode_cache = (struct symbol_to_keycode_pair *)calloc(symbol_to_keycode_cache_size, sizeof(struct symbol_to_keycode_pair));
	p->keycode_to_symbol_cache_pos = 0;
	p->symbol_to_keycode_cache_pos = 0;

	get_offending_modifiers(p);
	for (int i = 0; i < p->handle->total_languages; i++)
	{
		// FIXME Replace hardcode "us" to setting
		if (p->handle->languages[i].dir != NULL)
		{
			if (strcmp(p->handle->languages[i].dir, "us") == 0)
			{
				p->latin_group = i;
				//log_message(LOG, _("language dir %s"), p->handle->languages[i].dir);
			}
		}
	}

	p->get_keysyms_by_string	= keymap_get_keysyms_by_string;
	p->keycode_to_symbol		= keymap_keycode_to_symbol;
	p->get_ascii			= keymap_get_ascii;
	p->get_cur_ascii_char		= keymap_get_cur_ascii_char;
	p->convert_text_to_ascii	= keymap_convert_text_to_ascii;
	p->uninit			= keymap_uninit;

	return p;
}


