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

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "xconfig.h"
#include "xprogram.h"
#include "xdefines.h"

#include "utils.h"
#include "log.h"
#include "files.h"

#define DICT_RU_NAME		"ru/dict"
#define DICT_EN_NAME		"en/dict"
#define PROTO_RU_NAME		"ru/proto"
#define PROTO_EN_NAME		"en/proto"
#define SYLLABLE_RU_NAME	"ru/syllable"

#define CNF_NAME		"xneurrc"
#define LOCK_FILE		"xneur.pid"
#define VERSION			"0.2.0"

static char *PROGRAM;
static bool EXIT_FLAG;

char *HOME;
int MAX_PATH_LEN;
char *CNF_DIR = ".xneur";
struct _xconf *xconfig = NULL;

void xneur_exit()
{
	EXIT_FLAG = TRUE;
}

bool xneur_isexiting()
{
	return EXIT_FLAG;
}

bool xneur_check_lock_exists(const char *lock_file)
{
	FILE *fpid = fopen(lock_file, "r");
	if (!fpid)
		return FALSE;

	char *buffer = (char *) xnmalloc(32 + 1);

	if (fgets(buffer, 32, fpid) == NULL)
	{
		fclose(fpid);
		return FALSE;
	}
	fclose(fpid);

	int locked_pid = atoi(buffer);
	free(buffer);

	if (locked_pid == 0)
		return FALSE;

	char *process = (char *) xnmalloc(MAX_PATH_LEN + 1);
	snprintf(process, MAX_PATH_LEN, "/proc/%d/cmdline", locked_pid);

	FILE *cmdline = fopen(process, "r");
	free(process);

	if (!cmdline)
	{
		log_message(LOG, "Lock file %s exists, but no process with pid %d", lock_file, locked_pid);
		return FALSE;
	}

	fclose(cmdline);
	log_message(ERROR, "Lock file %s is already exists, locked process pid is %d", lock_file, locked_pid);

	return TRUE;
}

void xneur_set_lock(void)
{
	int xneur_pid = getpid();

	char *lock_file = get_file_path_name(LOCK_FILE);
	if (xneur_check_lock_exists(lock_file))
	{
		free(lock_file);
		exit(EXIT_FAILURE);
	}

	FILE *fpid = fopen(lock_file, "w");
	if (!fpid)
	{
		log_message(ERROR, "Failed to create lock file %s", lock_file);
		free(lock_file);
		exit(EXIT_FAILURE);
	}

	fprintf(fpid, "%d", xneur_pid);
	fclose(fpid);

	log_message(LOG, "Set PID %d to lock file %s", xneur_pid, lock_file);
	free(lock_file);
}

void xneur_init(int argc, char *argv[])
{
	EXIT_FLAG = 0;

	if (argc == 0)
	{
		log_message(ERROR, "Error in executing program");
		exit(EXIT_FAILURE);
	}
	PROGRAM = argv[0];

	HOME = getenv("HOME");
	if (HOME == NULL)
	{
		log_message(ERROR, "You must set HOME enviroment variable");
		exit(EXIT_FAILURE);
	}

	MAX_PATH_LEN = pathconf(HOME, _PC_PATH_MAX);
	if (MAX_PATH_LEN <= 0)
	{
		log_message(ERROR, "Can't determine max length of path");
		exit(EXIT_FAILURE);
	}
}

bool xneur_load_config(struct _xconf *xnc)
{
	if (!xnc->load_config_file(xnc, CNF_NAME))
	{
		log_message(ERROR, "Can't load configuration file %s/%s/%s", HOME, CNF_DIR, CNF_NAME);
		return FALSE;
	}

	log_message(LOG, "Configuration file load complete!");

	xnc->dicts[ENGLISH] = load_file_to_list(DICT_EN_NAME);
	if (xnc->dicts[ENGLISH] == NULL)
		log_message(LOG, "English dictionary not defined!");
	else
		log_message(DEBUG, "English dictionary load complete!");

	xnc->dicts[RUSSIAN] = load_file_to_list(DICT_RU_NAME);
	if (xnc->dicts[RUSSIAN] == NULL)
		log_message(LOG, "Russian dictionary not defined!");
	else
		log_message(DEBUG, "Russian dictionary load complete!");

	xnc->protos[ENGLISH] = load_file_to_list(PROTO_EN_NAME);
	if (xnc->protos[ENGLISH] == NULL)
	{
		log_message(ERROR, "English proto language not defined!");
		return FALSE;
	}

	log_message(DEBUG, "English Proto language load complete!");

	xnc->protos[RUSSIAN] = load_file_to_list(PROTO_RU_NAME);
	if (xnc->protos[RUSSIAN] == NULL)
	{
		log_message(ERROR, "Russian proto language not defined!");
		return FALSE;
	}

	log_message(DEBUG, "Russian proto language load complete!");

	xnc->syllable = load_file_to_list(SYLLABLE_RU_NAME);
	if (xnc->syllable == NULL)
	{
		log_message(ERROR, "Syllables weight not defined!");
		return FALSE;
	}
	xnc->parse_syllable_list(xnc);

	log_message(DEBUG, "Syllables weight load complete!");

	log_message(LOG, "Configuration init complete!");

	return TRUE;
}

void xneur_delete_lock(void)
{
	delete_file(LOCK_FILE);
	log_message(DEBUG, "Removed lock file %s/%s/%s", HOME, CNF_DIR, LOCK_FILE);
}

void xneur_signal(int status)
{
	status = status;
	log_message(DEBUG, "Caught signal, terminating", signal);
	xneur_exit();
}

void xneur_usage(void)
{
	printf("\nXneur - automatic keyboard switcher (version %s) \n", VERSION);
	printf("usage: xneur [options]\n");
	printf("  where options are:\n");
	printf("\n");
	printf("  -v, --version           Print version and exit\n");
	printf("  -h, --help              This help!\n");
	printf("  -a, --about             About for Sven\n");
}

void xneur_version(void)
{
	printf("\nXneur - automatic keyboard switcher (version %s) \n", VERSION);
	printf("mailto: andrew.crew.kuznetsov@gmail.com\n\n");
	printf("web: http://www.xneur.ru/\n");
}

void xneur_about(void)
{
	printf("\nXneur\n");
	printf("Automatic keyboard switcher (version %s) \n", VERSION);
	printf("It's program like Punto Switcher. \n");
	printf("This utility is made for X Window System.\n\n");
	printf("mailto: andrew.crew.kuznetsov@gmail.com\n");
	printf("web: http://www.xneur.ru/\n");
}

void xneur_get_options(int argc, char *argv[])
{
	static struct option longopts[] =
	{
             { "version",	no_argument,	NULL,	'v' },
             { "help",		no_argument,	NULL,	'h' },
             { "about",		no_argument,	NULL,	'a' },
             { NULL,		0,		NULL,	0 }
	};

	bool opted = FALSE;
	int opt;
	while ((opt = getopt_long(argc, argv, "vha", longopts, NULL)) != -1)
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

int main(int argc, char *argv[])
{
	// Parse command line options
	xneur_get_options(argc, argv); 

	// Init static variables
	xneur_init(argc, argv);

	// Init configuration
	xconfig = xconf_init();

	// Set locking
	xneur_set_lock();

	atexit(xneur_delete_lock);
	xntrap(SIGTERM, xneur_signal);
	xntrap(SIGINT, xneur_signal);

	// Load configuration
	if (!xneur_load_config(xconfig))
	{
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}

	// Init program structures
	struct _xprograminfo *xinfo = xprogram_init();
	if (xinfo == NULL)
	{
		log_message(ERROR, "Failed to init program structure");
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}

	log_message(DEBUG, "Init program structure complete!");

	// Init current mode value
	xconfig->CurrentMode = xconfig->DefaultMode;
	log_message(LOG, "Current mode is set to %d", xconfig->CurrentMode);

	while (!xneur_isexiting())
	{
		// Pause between iterations
		usleep(xconfig->ScaningDelay);

		// Update current xclient info
		if (!xinfo->update(xinfo))
			continue;

		// So client has met our conditions. Let's parse input keys
		int word_len = xinfo->process_input(xinfo);

		// Perform bind actions if any			
		if (xinfo->perform_current_bind(xinfo))
			continue;

		// So we have no bind during this iteration. Perform mode actions if any.
		if (!word_len)
			continue;

		xinfo->perform_current_mode(xinfo);
	}
  
	xconfig->uninit(xconfig);
	log_message(DEBUG, "Current config is freed");

	xinfo->uninit(xinfo);
	log_message(DEBUG, "Current program info is freed");

	return EXIT_SUCCESS;
}
