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

#ifndef _XNEUR_H_
#define _XNEUR_H_

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef WITH_ASPELL
#	include <aspell.h>
#endif

#ifdef WITH_ENCHANT
#	include <enchant/enchant.h>
#endif

extern struct _window *main_window;

extern int has_x_input_extension;
extern int xi_opcode;

struct _xneur_language
{
	char *dir;
	char *name;
	int  group;
	int  excluded;
	int  disable_auto_detection;

	struct _list_char *temp_dictionary;
	struct _list_char *dictionary;
	struct _list_char *proto;
	struct _list_char *big_proto;
	struct _list_char *pattern;
};

// Main xneur structure
struct _xneur_handle
{
	struct _xneur_language *languages;
	int total_languages;

#ifdef WITH_ASPELL
	// global aspell dictionaries
	AspellConfig *spell_config;
	AspellSpeller **spell_checkers;
	int *has_spell_checker;
#endif

#ifdef WITH_ENCHANT
	// global enchant dictionaries
	EnchantBroker *enchant_broker;
	/// Array of dictionaries for each language
	EnchantDict **enchant_dicts;
#endif
};

// Initialyze structure (must be installed proto, proto3, dict and regexp)
// One application - one create.
struct _xneur_handle *xneur_handle_create (void);

// Destroy structure
void xneur_handle_destroy (struct _xneur_handle *handle);

// Check WORD. Library return alleged new word and it's layout
// 1. Layout
int xneur_get_layout (struct _xneur_handle *handle, char *word);
// 1. New word
char *xneur_get_word (struct _xneur_handle *handle, char *word);

#endif /* _XNEUR_H_ */
