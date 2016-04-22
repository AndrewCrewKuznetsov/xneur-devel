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

#include "utils.h"
#include "text.h"
#include "list.h"
#include "types.h"

#define NEW_LANG_DIR "new"
#define NEW_LANG_TEXT "new.text"
#define NEW_LANG_ALPH "new.alphabet"

extern struct _xwindow *main_window;
	
static void lower_by_keymap(char *text, int group)
{
	int text_len = strlen(text);

	int i;
	for (i = 0; i < text_len; i++)
	{
		int j;
		for (j = main_window->xkeymap->min_keycode+1; j <= main_window->xkeymap->max_keycode; j++)
		{
			if (main_window->xkeymap->alphabet[group][j].upper_sym[0] == NULLSYM) 
				continue;
			
			if (memcmp(text + i, main_window->xkeymap->alphabet[group][j].upper_sym, 
						strlen(main_window->xkeymap->alphabet[group][j].upper_sym)) == 0)
			{
				memcpy(text + i, main_window->xkeymap->alphabet[group][j].lower_sym, 
						strlen(main_window->xkeymap->alphabet[group][j].lower_sym));
				i += strlen(main_window->xkeymap->alphabet[group][j].lower_sym) - 1;
				continue;
			}
		}
	}
}

void generate_protos(void)
{
	int new_lang_group;
	
	printf("THIS OPTION FOR DEVELOPERS ONLY!\n");
	printf("\n1) Please, define new language group and press Enter to continue...\n");
	printf("(see above keyboard layouts groups presented in system): \n");

	scanf("%d", &new_lang_group);

	if ((3 < new_lang_group) || (new_lang_group < 0))
	{
		printf("New language group is bad! Aborting!\n");
		exit(EXIT_SUCCESS);
	}

	printf("\nSpecified new language group: %d\n", new_lang_group);

	printf("\n2) Please, place new language sample text file to ~/.xneur/%s/%s\n", NEW_LANG_DIR, NEW_LANG_TEXT);
	printf("Press Enter to continue...\n");

	getchar();

	char *text = get_file_content_path(NEW_LANG_DIR, NEW_LANG_TEXT);
	if (text == NULL)
	{
		printf("New language text file not find! Aborting!\n");
		return;
	}

	lower_by_keymap(text, new_lang_group);
	printf("%s\n", text);
	//Here need check text for undef symbols!
	main_window->xkeymap->convert_text_to_ascii(main_window->xkeymap, text);
	
	struct _list *proto  = list_init();
	struct _list *proto3 = list_init();

	char *syll = (char*) xnmalloc((sizeof(char) * 3 + 1) * sizeof(char));

	// Check character combination
	int i;
	for (i = main_window->xkeymap->min_keycode+1; i <= main_window->xkeymap->max_keycode; i++)
	{
		if (main_window->xkeymap->alphabet[new_lang_group][i].lower_sym[0] == NULLSYM) 
			continue;
		
		int j;
		for (j = main_window->xkeymap->min_keycode+1; j <= main_window->xkeymap->max_keycode; j++)
		{
			if (main_window->xkeymap->alphabet[new_lang_group][j].lower_sym[0] == NULLSYM) 
				continue;
			
			strcpy(syll, main_window->xkeymap->alphabet[new_lang_group][i].lower_sym);
			strcat(syll, main_window->xkeymap->alphabet[new_lang_group][j].lower_sym);
			main_window->xkeymap->convert_text_to_ascii(main_window->xkeymap, syll);
			
			if (proto->find(proto, syll, sizeof(char) * 2, BY_PLAIN))
				continue;
			
			if (strstr(text, syll) == NULL)
			{
				proto->add(proto, syll);
				continue;
			}
			
			int k;
			for (k = main_window->xkeymap->min_keycode+1; k <= main_window->xkeymap->max_keycode; k++)
			{
				if (main_window->xkeymap->alphabet[new_lang_group][k].lower_sym[0] == NULLSYM) 
					continue;
				
				strcpy(syll, main_window->xkeymap->alphabet[new_lang_group][i].lower_sym);
				strcat(syll, main_window->xkeymap->alphabet[new_lang_group][j].lower_sym);
				strcat(syll, main_window->xkeymap->alphabet[new_lang_group][k].lower_sym);			
				main_window->xkeymap->convert_text_to_ascii(main_window->xkeymap, syll);
				
				if (proto3->find(proto3, syll, sizeof(char) * 3, BY_PLAIN))
					continue;
				
				if (strstr(text, syll) != NULL)
					continue;
				
				proto3->add(proto3, syll);
			}
		}
	}


	free(text);
	free(syll);

	struct _list *proto_temp = list_init();
	for (i = 0; i < proto->data_count; i++)
		if (!proto_temp->find(proto_temp, proto->data[i].string, proto->data[i].string_size, BY_PLAIN))
			proto_temp->add(proto_temp, proto->data[i].string);

	char *proto_file_path = get_file_path_name(NEW_LANG_DIR, "proto");
	save_list_to_file(proto_temp, proto_file_path);
	free(proto_file_path);
	printf("Short proto writed\n");

	proto_temp->uninit(proto_temp);

	proto_temp = list_init();
	for (i = 0; i < proto3->data_count; i++)
		if (!proto_temp->find(proto_temp, proto3->data[i].string, proto3->data[i].string_size, BY_PLAIN))
			proto_temp->add(proto_temp, proto3->data[i].string);

	char *proto3_file_path = get_file_path_name(NEW_LANG_DIR, "proto3");
	save_list_to_file(proto_temp, proto3_file_path);
	free(proto3_file_path);
	printf("Big proto writed\n");

	proto_temp->uninit(proto_temp);

	proto->uninit(proto);
	proto3->uninit(proto3);
		
	exit(EXIT_SUCCESS);
}
