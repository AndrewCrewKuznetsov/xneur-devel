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
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "xnconfig_files.h"

#include "xwindow.h"
#include "xkeymap.h"

#include "utils.h"
#include "list_char.h"
#include "text.h"
#include "types.h"

#define NEW_LANG_DIR "new"
#define NEW_LANG_TEXT "new.text"
#define NEW_LANG_ALPH "new.alphabet"

extern struct _xwindow *main_window;

/*static void lower_by_keymap(char *text, int group)
{
	int text_len = strlen(text);

	char *text_new = (char *) xnmalloc(1);
	text_new[0] = NULLSYM;
	
	int i, len;
	for (i = 0; i < text_len; i++)
	{
		char *ret_sym = main_window->xkeymap->lower_by_keymaps(main_window->xkeymap, group, text + i, &len);
		if (ret_sym == NULL)
		{
			text_new = (char *) xnrealloc(text_new, (strlen(text_new) + 2) * sizeof(char));
			strncat(text_new, text + i, 1 * sizeof(char)); 
			continue;
		}

		text_new = (char *) xnrealloc(text_new, (strlen(text_new) + strlen(ret_sym) + 1) * sizeof(char));
		strcat(text_new, ret_sym);
		free(ret_sym);

		i += len - 1;
	}

	text = (char *) xnrealloc(text, (strlen(text_new) + 1) * sizeof(char));
	strcpy(text, text_new);
}*/

void generate_protos(void)
{
	int new_lang_group;

	printf("THIS OPTION FOR DEVELOPERS ONLY!\n");
	printf("\n1) Please, define new language group and press Enter to continue...\n");
	printf("(see above keyboard layouts groups presented in system): \n");

	scanf("%d", &new_lang_group);

	if (new_lang_group < 0 || new_lang_group > 3)
	{
		printf("New language group is bad! Aborting!\n");
		exit(EXIT_SUCCESS);
	}

	printf("\nSpecified new language group: %d\n", new_lang_group);

	//printf("\n2) Please, place new language sample text file to ~/.xneur/%s/%s\n", NEW_LANG_DIR, NEW_LANG_TEXT);
	//printf("Press Enter to continue...\n");

	//getchar();

	//main_window->xkeymap->print_keymaps(main_window->xkeymap);
	
	char *text = get_file_content_path(NEW_LANG_DIR, NEW_LANG_TEXT);
	if (text == NULL)
	{
		printf("New language text file not find! Aborting!\n");
		return;
	}

	/*lower_by_keymap(text, new_lang_group);
	printf("Text in low symbols :\n %s\n", text);

	//Here need check text for undef symbols!
	main_window->xkeymap->convert_text_to_ascii(main_window->xkeymap, text);*/

	printf("Text in latin symbols :\n %s\n", text);

	struct _list_char *proto  = list_char_init();
	struct _list_char *proto3 = list_char_init();

	char *syll = (char *) xnmalloc((3 + 1) * sizeof(char));

	printf("Check character combination...\n");
	// Check character combination
	for (int i = main_window->xkeymap->min_keycode + 1; i <= main_window->xkeymap->max_keycode; i++)
	{
		char *sym_i = keycode_to_symbol(i, main_window->xkeymap->latin_group, 0);
		if (sym_i == NULLSYM || iscntrl(sym_i[0]) || isspace(sym_i[0])) 
			continue;

		for (int j = main_window->xkeymap->min_keycode + 1; j <= main_window->xkeymap->max_keycode; j++)
		{
			char *sym_j = keycode_to_symbol(j, main_window->xkeymap->latin_group, 0);
			if (sym_j == NULLSYM || iscntrl(sym_j[0]) || isspace(sym_j[0])) 
				continue;

			strcpy(syll, sym_i);
			strcat(syll, sym_j);

			main_window->xkeymap->convert_text_to_ascii(main_window->xkeymap, syll);

			if (proto->find(proto, syll, 2 * sizeof(char), BY_PLAIN))
				continue;

			if (strstr(text, syll) == NULL)
			{
				proto->add(proto, syll);
				continue;
			}

			for (int k = main_window->xkeymap->min_keycode + 1; k <= main_window->xkeymap->max_keycode; k++)
			{
				char *sym_k = keycode_to_symbol(k, main_window->xkeymap->latin_group, 0);
				if (sym_k == NULL || iscntrl(sym_k[0]) || isspace(sym_k[0])) 
					continue;

				strcpy(syll, sym_i);
				strcat(syll, sym_j);
				strcat(syll, sym_k);

				main_window->xkeymap->convert_text_to_ascii(main_window->xkeymap, syll);

				if (proto3->find(proto3, syll, 3 * sizeof(char), BY_PLAIN))
					continue;

				if (strstr(text, syll) != NULL)
					continue;

				proto3->add(proto3, syll);
			}
		}
	}

	struct _list_char *proto_temp = list_char_init();
	for (int i = 0; i < proto->data_count; i++)
		if (!proto_temp->find(proto_temp, proto->data[i].string, proto->data[i].string_size, BY_PLAIN))
			proto_temp->add(proto_temp, proto->data[i].string);

	char *proto_file_path = get_file_path_name(NEW_LANG_DIR, "proto");
	save_list_to_file(proto, proto_file_path);
	printf("Short proto writed (%d) to %s\n", proto->data_count, proto_file_path);
	free(proto_file_path);
	
	proto_temp->uninit(proto_temp);

	struct _list_char *proto3_temp = list_char_init();
	for (int i = 0; i < proto3->data_count; i++)
		if (!proto3_temp->find(proto3_temp, proto3->data[i].string, proto3->data[i].string_size, BY_PLAIN))
			proto3_temp->add(proto3_temp, proto3->data[i].string);

	char *proto3_file_path = get_file_path_name(NEW_LANG_DIR, "proto3");
	save_list_to_file(proto3, proto3_file_path);
	printf("Big proto writed (%d) to %s\n", proto3->data_count, proto3_file_path);
	free(proto3_file_path);

	proto3_temp->uninit(proto3_temp);

	proto->uninit(proto);
	proto3->uninit(proto3);
	printf("End of generation!\n");
	//free(text);
	//free(syll);
		
	exit(EXIT_SUCCESS);
}
