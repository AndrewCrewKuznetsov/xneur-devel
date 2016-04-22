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

#include <X11/keysym.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "xconfig.h"
#include "xobject.h"
#include "xdefines.h"

#include "utils.h"
#include "conv.h"

#include "xstring.h"

static const int groups[4] = {0x00000000, 0x00002000, 0x00004000, 0x00006000};
static const int groups_size = sizeof(groups) / sizeof(groups[0]);

extern struct _xconf *xconfig;

int get_keycode_mod(int group)
{
	if (group >= groups_size)
		group = 0;

	return groups[group];
}

void xstr_set_key_code(struct _xstr *p, int lang)
{
	int keycode_mod = get_keycode_mod(xconfig->xkbGroup[lang]);

	int i, len = strlen(p->content);
	for (i = 0; i < len; i++)
	{
		char sym = p->content[i];

		KeySym ks = char_to_keysym(sym);

		int shift_mod = None;
		if (is_upper_non_alpha_cyr(sym) || isupper(sym))
			shift_mod = ShiftMask;
		
		p->keycode[i] = XKeysymToKeycode(p->home->xDisplay, ks);
		p->keycode_modifiers[i] = (keycode_mod | shift_mod);

		// Xlib (error '<') patch >>
		if (ks == XK_less && p->keycode[i] == 94)
			p->keycode[i] = 59;
	}
}

void xstr_clear(struct _xstr *p)
{
	p->cur_pos = 0;
	p->content[0] = NULLSYM;
}

void xstr_set_content(struct _xstr *p, const char *src)
{
	int size_old = p->cur_size;
	if (!src)
	{
		p->cur_pos = 0;
		if (size_old)
			p->content[0] = NULLSYM;
		return;
	}

	p->cur_pos = strlen(src);
	if (p->cur_pos >= size_old)
	{
		p->cur_size = p->cur_pos + 1;

		p->content		= (char *) xnrealloc(p->content, p->cur_size * sizeof(char));
		p->keycode		= (KeyCode *) xnrealloc(p->keycode, p->cur_size * sizeof(KeyCode));
		p->keycode_modifiers	= (int *) xnrealloc(p->keycode_modifiers, p->cur_size * sizeof(int));
	}

	if (p->cur_pos)
		memcpy(p->content, src, p->cur_pos);
	p->content[p->cur_pos] = NULLSYM;
}

void xstr_add_symbol(struct _xstr *p, char sym)
{
	if (p->cur_pos == p->cur_size - 1)
	{
		p->cur_size *= 2;

		p->content		= (char *) xnrealloc(p->content, p->cur_size * sizeof(char));
		p->keycode		= (KeyCode *) xnrealloc(p->keycode, p->cur_size * sizeof(KeyCode));
		p->keycode_modifiers	= (int *) xnrealloc(p->keycode_modifiers, p->cur_size * sizeof(int));
	}

	p->content[p->cur_pos] = sym;
	p->keycode[p->cur_pos] = NULLSYM;
	p->keycode_modifiers[p->cur_pos] = NULLSYM;

	p->cur_pos++;
	p->content[p->cur_pos] = NULLSYM;
}

void xstr_del_symbol(struct _xstr *p)
{
	if (p->cur_pos == 0)
		return;

	p->cur_pos--;
	p->content[p->cur_pos] = NULLSYM;
}

void xstr_uninit(struct _xstr *p)
{
	p->home->uninit(p->home);

	free(p->keycode_modifiers);
	free(p->keycode);
	free(p->content);
	free(p);
}

struct _xstr* xstr_init(void)
{
	struct _xstr *p = (struct _xstr *) xnmalloc(sizeof(struct _xstr));
	bzero(p, sizeof(struct _xstr));

	p->cur_size	= INIT_STRING_LENGTH;
	p->home		= xobj_init();

	p->content = (char *) xnmalloc(p->cur_size * sizeof(char));
	bzero(p->content, p->cur_size * sizeof(char));

	p->keycode = (KeyCode *) xnmalloc(p->cur_size * sizeof(KeyCode));
	bzero(p->keycode, p->cur_size * sizeof(KeyCode));

	p->keycode_modifiers = (int *) xnmalloc(p->cur_size * sizeof(int));
	bzero(p->keycode_modifiers, p->cur_size * sizeof(int));

        // Functions mapping
	p->clear		= xstr_clear;
	p->set_key_code		= xstr_set_key_code;
	p->set_content		= xstr_set_content;
	p->add_symbol		= xstr_add_symbol;
	p->del_symbol		= xstr_del_symbol;
	p->uninit		= xstr_uninit;

	return p;
}
