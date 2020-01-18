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
 *  Copyright (C) 2006-2010 XNeur Team
 *
 */

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "xnconfig_files.h"

#include "window.h"
#include "keymap.h"

#include "types.h"
#include "list_char.h"
#include "text.h"

#define NEW_LANG_DIR	"new"
#define NEW_LANG_TEXT	"new.text"

extern struct _window *main_window;

void generate_protos(void)
{
	printf("THIS OPTION FOR DEVELOPERS ONLY!\n");
	printf("\nPlease, define new language group and press Enter to continue...\n");
	printf("(see above keyboard layouts groups presented in system): \n");

	struct _keymap *keymap = main_window->keymap;

	int new_lang_group;
	if (!scanf("%d", &new_lang_group))
		exit(EXIT_SUCCESS);

	if (new_lang_group < 0 || new_lang_group > 3)
	{
		printf("New language group is bad! Aborting!\n");
		exit(EXIT_SUCCESS);
	}

	printf("\nSpecified new language group: %d\n", new_lang_group);
	char *path = get_file_path_name(NEW_LANG_DIR, NEW_LANG_TEXT);
	if (path == NULL)
		return;
	char *text = get_file_content(path);
	free(path);
	if (text == NULL)
	{
		printf("New language text file not find! Aborting!\n");
		exit(EXIT_FAILURE);
	}

	struct _list_char *proto  = list_char_init();//-V656
	struct _list_char *proto3 = list_char_init();//-V656

	char *syll = (char *) malloc((256 + 1) * sizeof(char));

	for (int i = 0; i < 100; i++)
	{
		printf("%d\n", i);

		char *sym_i = keymap->keycode_to_symbol(keymap, i, new_lang_group, 0);
		if (isblank(sym_i[0]) || iscntrl(sym_i[0]) || isspace(sym_i[0]) || ispunct(sym_i[0]) || isdigit(sym_i[0]))
			continue;
		for (int j = 0; j < 100; j++)
		{
			char *sym_j = keymap->keycode_to_symbol(keymap, j, new_lang_group, 0);
			if (isblank(sym_j[0]) || iscntrl(sym_j[0]) || isspace(sym_j[0]) || ispunct(sym_j[0]) || isdigit(sym_j[0]))
				continue;

			strcpy(syll, sym_i);
			strcat(syll, sym_j);

			if (proto->exist(proto, syll, BY_PLAIN))
				continue;

			if (strstr(text, syll) == NULL)
			{
				proto->add(proto, syll);
				continue;
			}

			for (int k = 0; k < 100; k++)
			{
				char *sym_k = keymap->keycode_to_symbol(keymap, k, new_lang_group, 0);
				if (isblank(sym_k[0]) || iscntrl(sym_k[0]) || isspace(sym_k[0]) || ispunct(sym_k[0]) || isdigit(sym_k[0]))
					continue;

				sprintf(syll, "%s%s%s", sym_i, sym_j, sym_k);

				if (proto3->exist(proto3, syll, BY_PLAIN))
					continue;

				if (strstr(text, syll) != NULL)
					continue;

				proto3->add(proto3, syll);
			}
		}
	}

	for (int i = 0; i < 100; i++)
	{
		char *sym_i = keymap->keycode_to_symbol(keymap, i, new_lang_group, 1 << 7);
		if (isblank(sym_i[0]) || iscntrl(sym_i[0]) || isspace(sym_i[0]) || ispunct(sym_i[0]) || isdigit(sym_i[0]))
			continue;
		for (int j = 0; j < 100; j++)
		{
			char *sym_j = keymap->keycode_to_symbol(keymap, j, new_lang_group, 1 << 7);
			if (isblank(sym_j[0]) || iscntrl(sym_j[0]) || isspace(sym_j[0]) || ispunct(sym_j[0]) || isdigit(sym_j[0]))
				continue;

			strcpy(syll, sym_i);
			strcat(syll, sym_j);

			if (proto->exist(proto, syll, BY_PLAIN))
				continue;

			if (strstr(text, syll) == NULL)
			{
				proto->add(proto, syll);
				continue;
			}

			for (int k = 0; k < 100; k++)
			{
				char *sym_k = keymap->keycode_to_symbol(keymap, k, new_lang_group, 1 << 7);
				if (isblank(sym_k[0]) || iscntrl(sym_k[0]) || isspace(sym_k[0]) || ispunct(sym_k[0]) || isdigit(sym_k[0]))
					continue;

				sprintf(syll, "%s%s%s", sym_i, sym_j, sym_k);

				if (proto3->exist(proto3, syll, BY_PLAIN))
					continue;

				if (strstr(text, syll) != NULL)
					continue;

				proto3->add(proto3, syll);
			}
		}
	}

	free(text);

	char *proto_file_path = get_file_path_name(NEW_LANG_DIR, "proto");
	FILE *stream = fopen(proto_file_path, "w");
	if (stream == NULL)
	{
		free(syll);
		free(proto_file_path);
		proto->uninit(proto);
		proto3->uninit(proto3);
		return;
	}
	proto->save(proto, stream);
	printf("Short proto writed (%d) to %s\n", proto->data_count, proto_file_path);
	fclose(stream);
	free(proto_file_path);

	char *proto3_file_path = get_file_path_name(NEW_LANG_DIR, "proto3");
	stream = fopen(proto3_file_path, "w");
	if (stream == NULL)
	{
		free(syll);
		free(proto3_file_path);
		proto->uninit(proto);
		proto3->uninit(proto3);
		return;
	}
	proto3->save(proto3, stream);
	printf("Big proto writed (%d) to %s\n", proto3->data_count, proto3_file_path);
	fclose(stream);
	free(proto3_file_path);

	proto->uninit(proto);
	proto3->uninit(proto3);

	free(syll);

	printf("End of generation!\n");
}
