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

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <gtk/gtk.h>

#include <signal.h>
#include <stdlib.h>
#include <locale.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "support.h"

#include "trayicon.h"
#include "misc.h"

#include "configuration.h"


int arg_delay = -1;
const char* arg_keyboard_properties = NULL;
const char* arg_show_in_the_tray = NULL;
const char* arg_rendering_engine = NULL;

static  void trap_child(int status) 
{
	if (status){}
	/*Kills all the zombie processes*/
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
#ifdef ENABLE_NLS
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif

	setlocale(LC_ALL, "");
	gtk_init(&argc, &argv);
	
	static struct option longopts[] =
	{
			{ "help",	no_argument,	NULL,	'h' },
			{ "configure",	no_argument,	NULL,	'c' },
			{ "delay",	required_argument,	NULL,	'D' },
			{ "keyboard-properties",	required_argument,	NULL,	1000 },
			{ "rendering-engine",	required_argument,	NULL,	'E' },
			{ "show",	required_argument,	NULL,	'S' },
			{ "disable-settings-storage",	no_argument,	NULL,	1001 },
			{ "store",	no_argument,	NULL,	'e' },
			{ NULL,			0,		NULL,	0 }
	};

	gboolean store = FALSE;

	int opt;
	while ((opt = getopt_long(argc, argv, "hcS:D:E:e", longopts, NULL)) != -1)
	{
		switch (opt)
		{
			case 'D':
			{
				arg_delay = atoi(optarg);
				if (store)
					gxneur_config_write_int("delay", arg_delay, TRUE);
				break;
			}
			case 'S':
			{
				arg_show_in_the_tray = optarg;
				if (store)
					gxneur_config_write_str("show_in_the_tray", arg_show_in_the_tray, TRUE);
				break;
			}
			case 1000:
			{
				arg_keyboard_properties = optarg;
				if (store)
					gxneur_config_write_str("keyboard_properties", arg_keyboard_properties, TRUE);
				break;
			}
			case 1001:
			{
				gxneur_config_set_enabled(FALSE);
				break;
			}
			case 'E':
			{
				arg_rendering_engine = optarg;
				if (store)
					gxneur_config_write_str("rendering_engine", arg_rendering_engine, TRUE);
				break;
			}
			case 'c':
			{
				printf(_("\nThis option under construction. Sorry.\n"));
				return EXIT_SUCCESS;
			}
			case 'e':
			{
				store = TRUE;
				break;
			}
			case '?':
			case 'h':
			{
				printf(_("\nGTK2 frontend for XNeur (version %s) \n"), VERSION);
				printf(_("usage: gxneur [options]\n"));
				printf(_("  where options are:\n"));
				printf("\n");
				printf(_("  -D, --delay=<seconds>                Seconds to wait before starting xneur\n"));
				printf(_("  -E, --rendering-engine=<engine>      Rendering engine to use (Built-in, StatusIcon, AppIndicator. Default is %s.)\n"), DEFAULT_RENDERING_ENGINE);
				printf(_("  -S, --show=<mode>                    Icon display mode (Icon, Flag, Text, Directory). Default is %s.)\n"), DEFAULT_SHOW_IN_THE_TRAY);
				printf(_("      --keyboard-properties=<command>  Command to run on \"Keyboard Properties\" menu item. Default is %s.\n"), KB_PROP_COMMAND);
				printf(_("  -e  --store                          Save settings from the command line to the storage and exit\n"));
				printf(_("      --disable-settings-storage       Disable reading and saving gxneur settings to the persistent storage\n"));
				printf(_("  -c, --configure                      Configure xneur and gxneur\n"));
				printf(_("  -h, --help                           Display this help and exit\n"));
				exit(EXIT_SUCCESS);
				break;
			}
		}
	}

	if (store)
		exit(EXIT_SUCCESS);

	int value = -1;
	gxneur_config_read_int("delay", &value);
	if (arg_delay >= 0)
		value = arg_delay;
	if (value <= 0)
		value = DEFAULT_DELAY;
	
	sleep (value);

	signal(SIGCHLD, trap_child);
	
	xneur_start();

	create_tray_icon();	

	gtk_main();
	
	return EXIT_SUCCESS;
}
