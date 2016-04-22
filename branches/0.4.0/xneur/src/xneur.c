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
 *  (c) XNeur Team 2006
 *
 */

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

#include "config_main.h"

#include "xprogram.h"

#include "utils.h"
#include "list.h"
#include "log.h"
#include "colors.h"
#include "types.h"

int xneur_check_lock = TRUE;

struct _xneur_config *xconfig = NULL;

static void xneur_exit(void)
{
	close_program();
}

static void xneur_set_lock(void)
{
	if (xneur_check_lock == TRUE)
	{
		int locked_pid = xconfig->get_pid(xconfig);
		if (locked_pid != -1)
		{
			log_message(ERROR, PACKAGE " already running with pid %d", locked_pid);
			exit(EXIT_FAILURE);
		}
	}

	int xneur_pid = getpid();

	xconfig->set_pid(xconfig, xneur_pid);

	log_message(DEBUG, PACKAGE " pid is %d", xneur_pid);
}

static void xneur_signal(int status)
{
	status = status;
	log_message(DEBUG, "Caught signal, terminating", signal);
	xneur_exit();
}

static void xneur_usage(void)
{
	printf("\nXneur - automatic keyboard switcher (version %s) \n", VERSION);
	printf("usage: xneur [options]\n");
	printf("  where options are:\n");
	printf("\n");
	printf("  -v, --version           Print version and exit\n");
	printf("  -h, --help              This help!\n");
	printf("  -a, --about             About for " PACKAGE "\n");
	printf("  -f, --force             Skip check for other instances of " PACKAGE " runned\n");
}

static void xneur_version(void)
{
	printf("\nXneur - automatic keyboard switcher (version %s) \n", VERSION);
	printf("mailto: andrew.crew.kuznetsov@gmail.com\n\n");
	printf("web: http://www.xneur.ru/\n");
}

static void xneur_about(void)
{
	printf("\nXneur\n");
	printf("Automatic keyboard switcher (version %s) \n", VERSION);
	printf("It's program like Punto Switcher. \n");
	printf("This utility is made for X Window System.\n\n");
	printf("mailto: andrew.crew.kuznetsov@gmail.com\n");
	printf("web: http://www.xneur.ru/\n");
}

static void xneur_get_options(int argc, char *argv[])
{
	static struct option longopts[] =
	{
             { "version",	no_argument,	NULL,	'v' },
             { "help",		no_argument,	NULL,	'h' },
             { "about",		no_argument,	NULL,	'a' },
	         { "force",		no_argument,	NULL,	'f' },
             { NULL,		0,		NULL,	0 }
	};

	int opted = FALSE;
	int opt;
	while ((opt = getopt_long(argc, argv, "vhaf", longopts, NULL)) != -1)
	{
		opted = TRUE;
		switch (opt)
		{
			case 'v':
			{
				xneur_version();
				break;
			}
			case 'a':
			{
				xneur_about();
				break;
			}
			case 'f':
			{
				xneur_check_lock = FALSE;
				opted = FALSE;
				break;
			}
			case '?':
			case 'h':
			{
				xneur_usage();
				break;
			}
		}
	}

	if (opted)
		exit(EXIT_SUCCESS);
}

static void xneur_reklama(void)
{
	printf("\n");
	printf(LIGHT_PURPLE_COLOR "=============================================================" NORMAL_COLOR "\n");
	printf(LIGHT_PURPLE_COLOR ">>> " LIGHT_BLUE_COLOR "Special thanks to " RED_COLOR "http://www.softodrom.ru" LIGHT_BLUE_COLOR " for support" LIGHT_PURPLE_COLOR " <<<" NORMAL_COLOR "\n");
	printf(LIGHT_PURPLE_COLOR "=============================================================" NORMAL_COLOR "\n");
	printf("\n");
}

int main(int argc, char *argv[])
{
	xneur_reklama();

	// Parse command line options
	xneur_get_options(argc, argv); 

	// Init configuration
	xconfig = xneur_config_init();
	if (xconfig == NULL)
	{
		log_message(ERROR, "Can't init libxnconfig");
		return EXIT_FAILURE;
	}

	// Set lock pid
	xneur_set_lock();

	log_message(LOG, "Loading configuration");

	// Load configuration
	enum _error error_code = xconfig->load(xconfig);
	if (error_code != ERROR_SUCCESS)
	{
		log_message(ERROR, format_error_message(error_code));
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}

	log_set_level(xconfig->log_level);
	log_message(LOG, "Log level is set to %d", xconfig->log_level);

	log_message(LOG, "Total detected %d languages", xconfig->total_languages);

	int lang;
	for (lang = 0; lang < xconfig->total_languages; lang++)
	{
		char *lang_name = xconfig->get_lang_name(xconfig, lang);

		log_message(DEBUG, "%s dictionary has %d records", lang_name, xconfig->languages[lang].dicts->data_count);
		log_message(DEBUG, "%s proto has %d records", lang_name, xconfig->languages[lang].protos->data_count);
		log_message(DEBUG, "%s big proto has %d records", lang_name, xconfig->languages[lang].big_protos->data_count);
	}

	log_message(DEBUG, "Configuration load complete");

	// Init program structures
	struct _xprogram *xprogram = xprogram_init();
	if (xprogram == NULL)
	{
		log_message(ERROR, "Failed to init program structure");
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}

	log_message(DEBUG, "Init program structure complete");

	char *current_mode = "auto";
	if (xconfig->get_current_mode(xconfig) == MANUAL_MODE)
		current_mode = "manual";
	log_message(LOG, "Current mode set to %s", current_mode);
	
	current_mode = "Yes";
	if (xconfig->mouse_processing_mode == MOUSE_GRAB_DISABLE)
		current_mode = "No";
	log_message(LOG, "Mouse processing mode set to %s", current_mode);
	
	current_mode = "Yes";
	if (xconfig->education_mode == EDUCATION_MODE_DISABLE)
		current_mode = "No";
	log_message(LOG, "Education mode set to %s", current_mode);

	xntrap(SIGTERM, xneur_signal);
	xntrap(SIGINT, xneur_signal);

	xprogram->process_input(xprogram);

	xprogram->uninit(xprogram);
	log_message(DEBUG, "Current program info is freed");

	xconfig->uninit(xconfig);
	log_message(DEBUG, "Current configuration data is freed");

	return EXIT_SUCCESS;
}
