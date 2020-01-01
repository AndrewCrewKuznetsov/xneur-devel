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

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <X11/XKBlib.h>

#include "xneur.h"
#include "keymap.h"

struct _buffer_content
{
	char *content;
	char *content_unchanged;
	int *symbol_len;
	int *symbol_len_unchanged;
};

struct _buffer
{
	struct _xneur_handle *handle;

	struct _buffer_content *i18n_content;

	struct _keymap *keymap;

	char *content;		// String itself
	KeyCode *keycode;	// Array of string chars keycodes
	int *keycode_modifiers;	// Array of string chars keycodes modifiers

	int cur_size;		// Current size of content, keycode, keycodeModifiers fields
	int cur_pos;		// Current filled size

	void (*clear) (struct _buffer *p);
	void (*save_and_clear) (struct _buffer *p, Window window);
	void (*set_lang_mask) (struct _buffer *p, int group);
	void (*set_uncaps_mask) (struct _buffer *p);
	void (*set_caps_mask) (struct _buffer *p);
	void (*set_content) (struct _buffer *p, const char *new_content);
	void (*change_case) (struct _buffer *p);
	void (*rotate_layout) (struct _buffer *p);
	void (*add_symbol) (struct _buffer *p, char sym, KeyCode keycode, int modifier);
	void (*del_symbol) (struct _buffer *p);
	char*(*get_utf_string) (struct _buffer *p);
	char*(*get_utf_string_on_kbd_group) (struct _buffer *p, int group);
	void (*set_offset) (struct _buffer *p, int offset);
	void (*unset_offset) (struct _buffer *p, int offset);
	char*(*get_last_word) (struct _buffer *p, char *string);
	int (*get_last_word_offset) (struct _buffer *p, const char *string, int string_len);
	void (*uninit) (struct _buffer *p);
};

struct _buffer* buffer_init(struct _xneur_handle *handle, struct _keymap *keymap);

#endif /* _BUFFER_H_ */
