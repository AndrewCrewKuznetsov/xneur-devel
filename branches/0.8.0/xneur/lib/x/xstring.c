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

#include <X11/keysym.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "xnconfig.h"

#include "xwindow.h"
#include "xevent.h"
#include "xkeymap.h"

#include "conversion.h"

#include "utils.h"
#include "text.h"
#include "types.h"

#include "xstring.h"

#define INIT_STRING_LENGTH 64

extern struct _xneur_config *xconfig;
extern struct _xwindow *main_window;

void xstring_set_key_code(struct _xstring *p, int lang)
{
	int keycode_mod = get_keycode_mod(xconfig->xkb_groups[lang]);
	int languages_mask = get_languages_mask();

	// Set new language bit
	for (int i = 0; i < p->cur_pos; i++)
	{
		p->keycode_modifiers[i] = p->keycode_modifiers[i] & (~languages_mask);
		p->keycode_modifiers[i] = p->keycode_modifiers[i] | keycode_mod;
	}
}

void xstring_clear(struct _xstring *p)
{
	int i;
	for (i = 0; i < p->cur_pos; i++)
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
		p->content		= (char *) xnrealloc(p->content, p->cur_size * sizeof(char));
		p->keycode		= (KeyCode *) xnrealloc(p->keycode, p->cur_size * sizeof(KeyCode));
		p->keycode_modifiers	= (int *) xnrealloc(p->keycode_modifiers, p->cur_size * sizeof(int));
	}

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

		p->content		= (char *) xnrealloc(p->content, p->cur_size * sizeof(char));
		p->keycode		= (KeyCode *) xnrealloc(p->keycode, p->cur_size * sizeof(KeyCode));
		p->keycode_modifiers	= (int *) xnrealloc(p->keycode_modifiers, p->cur_size * sizeof(int));
	}

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

void xstring_uninit(struct _xstring *p)
{
	free(p->keycode_modifiers);
	free(p->keycode);
	free(p->content);
	free(p);
}

struct _xstring* xstring_init(void)
{
	struct _xstring *p = (struct _xstring *) xnmalloc(sizeof(struct _xstring));
	bzero(p, sizeof(struct _xstring));

	p->cur_size		= INIT_STRING_LENGTH;
	p->content		= (char *) xnmalloc(p->cur_size * sizeof(char));
	p->keycode		= (KeyCode *) xnmalloc(p->cur_size * sizeof(KeyCode));
	p->keycode_modifiers	= (int *) xnmalloc(p->cur_size * sizeof(int));

	bzero(p->content, p->cur_size * sizeof(char));
	bzero(p->keycode, p->cur_size * sizeof(KeyCode));
	bzero(p->keycode_modifiers, p->cur_size * sizeof(int));

	// Functions mapping
	p->clear		= xstring_clear;
	p->is_space_last	= xstring_is_space_last;
	p->set_key_code		= xstring_set_key_code;
	p->set_content		= xstring_set_content;
	p->changecase_content	= xstring_changecase_content;
	p->add_symbol		= xstring_add_symbol;
	p->del_symbol		= xstring_del_symbol;
	p->uninit		= xstring_uninit;

	return p;
}
