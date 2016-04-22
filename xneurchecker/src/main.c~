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
 *  Copyright (C) 2010 XNeur Team
 *
 */

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <stdlib.h>
#include <locale.h>
#include <stdio.h>
#include <getopt.h>

#include <xneur/xnconfig.h>
#include <xneur/list_char.h>
#include <xneur/xneur.h>

int main(int argc, char *argv[])
{
	int layouts = 0;
	int translates = 0;
    static struct option longopts[] =
	{
			{ "help",		no_argument,	NULL,	'h' },
			{ "layouts",		no_argument,	NULL,	'l' },
			{ "words",	no_argument,	NULL,	'w' },
			{ NULL,			0,		NULL,	0 }
	};

	int opt;
	while ((opt = getopt_long(argc, argv, "hlw", longopts, NULL)) != -1)
	{
		switch (opt)
		{
			case 'l':
			{
				layouts++;
				break;
			}
			case 'w':
			{
				translates++;
				break;
			}
			case '?':
			case 'h':
			{
			    printf("\nxneurchecker - manual text checker based on xneur library (version %s) \n", VERSION);
				printf("usage: xneurchecker [options] \"<word_1> <...word_n>\"\n");
				printf("  where options are:\n");
				printf("\n");
				printf("  -h, --help		This help!\n");
				printf("  -l, --layout		Get probable layouts.\n");
				printf("  -w, --word		Get probable translate.\n");
				exit(EXIT_SUCCESS);
				break;
			}
		}
	}

	int words_count = argc - layouts - translates - 1;
	if (words_count == 0)
		exit(EXIT_SUCCESS);
	
	struct _xneur_handle *xnh;
	xnh = xneur_handle_create();
    printf("Xneur handler created\n");
	
	if ((!layouts) && (!translates))
		translates = 1;
	
	if (layouts)
	{
		for (int i = argc - words_count; i < argc; i++)
		{
			printf("Xneur processing '%s'...\n", argv[i]);
			int layout = xneur_get_layout(xnh, argv[i]);
			printf("%d \n", layout);
		}
		printf("\n");
	}
	
	if (translates)
	{
		for (int i = argc - words_count; i < argc; i++)
		{
			char *nw = xneur_get_word(xnh, argv[i]);
			printf("%s ", nw);
			free(nw);
		}
		printf("\n");
	}

	xneur_handle_destroy(xnh);

	return EXIT_SUCCESS;
}
