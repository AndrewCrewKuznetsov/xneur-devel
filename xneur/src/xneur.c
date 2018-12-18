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
 *  Copyright (C) 2006-2018 XNeur Team
 *
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>

#include "xnconfig.h"

#include "bind_table.h"
#include "switchlang.h"
#include "program.h"
#include "buffer.h"
#include "keymap.h"

#include "detection.h"

#include "types.h"
#include "text.h"
#include "list_char.h"
#include "log.h"
#include "colors.h"
#include "sound.h"
#include "popup.h"
#include "notify.h"
#include "plugin.h"

#include "newlang_creation.h"

typedef void (*sg_handler)(int);

struct _xneur_config *xconfig = NULL;
static struct _program *program = NULL;

static int xneur_check_lock = TRUE;
static int xneur_generate_proto = FALSE;

static void xneur_reload(int status);

/*static void xneur_check_config_version(void)
{
	
	log_message(LOG, _("Checking configuration file version..."));

	if (xconfig->version != NULL && strcmp(xconfig->version, VERSION) == 0)
	{
		log_message(LOG, _("User configuration file version is OK!"));
		return;
	}

	log_message(ERROR, _("Configuration file version is out of date!"));

	if (final)
	{
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}

	if (!xconfig->replace(xconfig))
	{
		log_message(ERROR, _("Default configuration file not founded in system! Please, reinstall XNeur!"));
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}

	log_message(LOG, _("Configuration file replaced to default one"));
	
	xneur_reload(0);
	
}*/

static void xneur_init(void)
{
	bind_actions();
	bind_user_actions();
}

static void xneur_load_config(void)
{
	log_message(LOG, _("Loading configuration"));

	if (!xconfig->load(xconfig))
	{
		log_message(ERROR, _("Configuration file damaged! Please, remove old file before starting xneur!"));
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}

	//xneur_check_config_version;

	log_message(LOG, _("Log level is set to %s"), xconfig->get_log_level_name(xconfig));
	
	log_message(DEBUG, _("Configuration load complete"));

	log_message(LOG, _("Keyboard layouts present in system:"));
	for (int lang = 0; lang < xconfig->handle->total_languages; lang++)
	{
		if (xconfig->handle->languages[lang].excluded)
			log_message(LOG, _("   Excluded XKB Group '%s', layout '%s', group '%d'"), xconfig->handle->languages[lang].name, xconfig->handle->languages[lang].dir, lang);
		else
			log_message(LOG, _("   Included XKB Group '%s', layout '%s', group '%d'"), xconfig->handle->languages[lang].name, xconfig->handle->languages[lang].dir, lang);

		char *lang_name = xconfig->handle->languages[lang].name;

		log_message(DEBUG, _("      %s dictionary has %d records"), lang_name, xconfig->handle->languages[lang].dictionary->data_count);
		log_message(DEBUG, _("      %s proto has %d records"), lang_name, xconfig->handle->languages[lang].proto->data_count);
		log_message(DEBUG, _("      %s big proto has %d records"), lang_name, xconfig->handle->languages[lang].big_proto->data_count);
#ifdef WITH_ASPELL
		if (xconfig->handle->has_spell_checker[lang])
		{
			log_message(DEBUG, _("      %s aspell dictionary loaded"), lang_name);
		}
		else
		{
			log_message(DEBUG, _("      %s aspell dictionary not found"), lang_name);
		}
#endif	
#ifdef WITH_ENCHANT
		if (xconfig->handle->enchant_dicts[lang])
		{
			log_message(DEBUG, _("      %s enchant wrapper dictionary loaded"), lang_name);
		}
		else
		{
			log_message(DEBUG, _("      %s enchant wrapper dictionary not found"), lang_name);
		}
#endif	
	}
	log_message(LOG, _("Total %d keyboard layouts detected"), xconfig->handle->total_languages);

	if (xconfig->handle->total_languages < 2)
	{
		log_message(ERROR, _("For correct operation of the program in the system should be set 2 or more keyboard layouts!"));
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}
	
	log_message(LOG, _("Default keyboard group for all new windows set to %d"), xconfig->default_group);
	log_message(LOG, _("Manual mode set to %s"), _(xconfig->get_bool_name(xconfig->manual_mode)));
	log_message(LOG, _("Education mode set to %s"), _(xconfig->get_bool_name(xconfig->educate)));
	log_message(LOG, _("Check similar words mode set to %s"), _(xconfig->get_bool_name(xconfig->check_similar_words)));
	log_message(LOG, _("Layout remember mode set to %s"), _(xconfig->get_bool_name(xconfig->remember_layout)));
	log_message(LOG, _("Save selection mode set to %s"), _(xconfig->get_bool_name(xconfig->save_selection_after_convert)));
	log_message(LOG, _("Rotate layout after convert selected text mode set to %s"), _(xconfig->get_bool_name(xconfig->rotate_layout_after_convert)));
	log_message(LOG, _("Sound playing mode set to %s"), _(xconfig->get_bool_name(xconfig->play_sounds)));
	log_message(LOG, _("Sound playing volume percentage set to %d"), xconfig->volume_percent);
	log_message(LOG, _("Logging keyboard mode set to %s"), _(xconfig->get_bool_name(xconfig->save_keyboard_log)));
	log_message(LOG, _("Max file size of keyboard log set to %d bytes"), xconfig->size_keyboard_log);
	log_message(LOG, _("Keyboard log will send to %s via %s:%d host"), xconfig->mail_keyboard_log, xconfig->host_keyboard_log, xconfig->port_keyboard_log);
	log_message(LOG, _("Ignore keyboard layout for abbreviations mode set to %s"), _(xconfig->get_bool_name(xconfig->abbr_ignore_layout)));
	log_message(LOG, _("Correct of iNCIDENTAL CapsLock mode set to %s"), _(xconfig->get_bool_name(xconfig->correct_incidental_caps)));
	log_message(LOG, _("Correct of two CApital letter mode set to %s"), _(xconfig->get_bool_name(xconfig->correct_two_capital_letter)));
	log_message(LOG, _("Correct of spaces with punctuation mode set to %s"), _(xconfig->get_bool_name(xconfig->correct_space_with_punctuation)));
	log_message(LOG, _("Correct of small letter to capital letter after dot mode set to %s"), _(xconfig->get_bool_name(xconfig->correct_capital_letter_after_dot)));
	log_message(LOG, _("Correct two space with a comma and a space mode set to %s"), _(xconfig->get_bool_name(xconfig->correct_two_space_with_comma_and_space)));
	log_message(LOG, _("Correct two minus with a dash mode set to %s"), _(xconfig->get_bool_name(xconfig->correct_two_minus_with_dash)));
	log_message(LOG, _("Correct dash with a emdash mode set to %s"), _(xconfig->get_bool_name(xconfig->correct_dash_with_emdash)));
	log_message(LOG, _("Correct (c) with a copyright sign mode set to %s"), _(xconfig->get_bool_name(xconfig->correct_c_with_copyright)));
	log_message(LOG, _("Correct (tm) with a trademark sign mode set to %s"), _(xconfig->get_bool_name(xconfig->correct_tm_with_trademark)));
	log_message(LOG, _("Correct (r) with a registered sign mode set to %s"), _(xconfig->get_bool_name(xconfig->correct_r_with_registered)));
	log_message(LOG, _("Correct three points with a ellipsis mode set to %s"), _(xconfig->get_bool_name(xconfig->correct_three_points_with_ellipsis)));
	log_message(LOG, _("Correct misprint mode set to %s"), _(xconfig->get_bool_name(xconfig->correct_misprint)));	
	log_message(LOG, _("Disable CapsLock use mode set to %s"), _(xconfig->get_bool_name(xconfig->disable_capslock)));
	log_message(LOG, _("Flush internal buffer when pressed Escape mode set to %s"), _(xconfig->get_bool_name(xconfig->flush_buffer_when_press_escape)));
	log_message(LOG, _("Flush internal buffer when pressed Enter or Tab mode set to %s"), _(xconfig->get_bool_name(xconfig->flush_buffer_when_press_enter)));
	log_message(LOG, _("Show OSD mode set to %s"), _(xconfig->get_bool_name(xconfig->show_osd)));
	log_message(LOG, _("Show popup messages mode set to %s"), _(xconfig->get_bool_name(xconfig->show_popup)));
	log_message(LOG, _("Popup expire timeout set to %d"), xconfig->popup_expire_timeout);
	log_message(LOG, _("Check language on input process mode set to %s"), _(xconfig->get_bool_name(xconfig->check_lang_on_process)));
	log_message(LOG, _("Pattern minig and recognition (autocompletion) mode set to %s"), _(xconfig->get_bool_name(xconfig->autocompletion)));
	log_message(LOG, _("Add space after autocompletion mode set to %s"), _(xconfig->get_bool_name(xconfig->add_space_after_autocompletion)));
	log_message(LOG, _("Disable autoswitching if pressed backspace mode set to %s"), _(xconfig->get_bool_name(xconfig->troubleshoot_backspace)));
	log_message(LOG, _("Disable autoswitching if pressed left arrow mode set to %s"), _(xconfig->get_bool_name(xconfig->troubleshoot_left_arrow)));
	log_message(LOG, _("Disable autoswitching if pressed right arrow mode set to %s"), _(xconfig->get_bool_name(xconfig->troubleshoot_right_arrow)));
	log_message(LOG, _("Disable autoswitching if pressed up arrow mode set to %s"), _(xconfig->get_bool_name(xconfig->troubleshoot_up_arrow)));
	log_message(LOG, _("Disable autoswitching if pressed down arrow mode set to %s"), _(xconfig->get_bool_name(xconfig->troubleshoot_down_arrow)));
	log_message(LOG, _("Disable autoswitching if pressed delete mode set to %s"), _(xconfig->get_bool_name(xconfig->troubleshoot_delete)));
	log_message(LOG, _("Disable autoswitching if pressed enter mode set to %s"), _(xconfig->get_bool_name(xconfig->troubleshoot_enter)));
	log_message(LOG, _("Disable autoswitching if pressed tab mode set to %s"), _(xconfig->get_bool_name(xconfig->troubleshoot_tab)));
	log_message(LOG, _("Disable autoswitching if layout switched mode set to %s"), _(xconfig->get_bool_name(xconfig->troubleshoot_switch)));
	log_message(LOG, _("Disable autoswitching for full screen apps mode set to %s"), _(xconfig->get_bool_name(xconfig->troubleshoot_full_screen)));
	log_message(LOG, _("Compatibility with the completion mode set to %s"), _(xconfig->get_bool_name(xconfig->compatibility_with_completion)));
	log_message(LOG, _("Tracking input mode set to %s"), _(xconfig->get_bool_name(xconfig->tracking_input)));
	log_message(LOG, _("Tracking mouse mode set to %s"), _(xconfig->get_bool_name(xconfig->tracking_mouse)));
	log_message(LOG, _("Delay before sendind events to application set to (in milliseconds) %d"), xconfig->send_delay);

	// Delimeters
	log_message(LOG, _("Word Delimeters:"));
	for (int i = 0; i < xconfig->delimeters_count; i++)
	{
		log_message(LOG, "    %s (0x%x)", XKeysymToString(xconfig->delimeters[i]), xconfig->delimeters[i]);
	}
}

static void xneur_set_lock(void)
{
	if (xneur_check_lock == TRUE)
	{
		pid_t locked_pid = xconfig->get_pid(xconfig);
		if (locked_pid != -1)
		{
			log_message(ERROR, _(PACKAGE " already running with pid %ld"), locked_pid);
			exit(EXIT_FAILURE);
		}
	}

	pid_t process_id = getpid();
	pid_t result = xconfig->set_pid(xconfig, process_id);
	if (result == -1)
		exit(EXIT_FAILURE);
}

static void xneur_cleanup(void)
{
	popup_uninit();
	sound_uninit();
	log_message(DEBUG, _("Current sound data is freed"));

	unbind_actions();
	log_message(DEBUG, _("Current keybinds is freed"));
	unbind_user_actions();
	log_message(DEBUG, _("Current user keybinds is freed"));
	
	if (program != NULL)
		program->uninit(program);

	log_message(DEBUG, _("Current program info is freed"));

	if (xconfig != NULL)
	{
		xconfig->set_pid(xconfig, 0);
		xconfig->uninit(xconfig);
	}
	log_message(DEBUG, _("Current configuration data is freed"));
	
#ifdef WITH_DEBUG
	xndebug_uninit();
#endif
}

static void xneur_terminate(int status)
{
	if (status){}

	log_message(DEBUG, _("Caught SIGTERM/SIGINT, terminating"));
	show_notify(NOTIFY_XNEUR_STOP, NULL);
	sleep(1);

	program->plugin->xneur_stop(program->plugin);

	xneur_cleanup();
	
	exit(EXIT_SUCCESS);
}

static void xneur_zombie(int status)
{
	if (status){}
	//Kills all the zombie processes
	log_message(DEBUG, _("Caught SIGCHLD, kill zombie process"));
	while(waitpid(-1, NULL, WNOHANG) > 0);
} 

static void xneur_reload(int status)
{
	if (status){}

	log_message(LOG, _("Caught SIGHUP, reloading configuration file"));
	show_notify(NOTIFY_XNEUR_RELOAD, NULL);
	
	sound_uninit();
	unbind_actions();
	unbind_user_actions();
	program->plugin->xneur_reload(program->plugin);

	if (xconfig != NULL)
		xconfig->uninit(xconfig);

	xconfig = xneur_config_init();

	if (xconfig == NULL)
	{
		log_message(ERROR, _("Can't init libxnconfig"));
		exit(EXIT_FAILURE);
	}

	xneur_load_config();

	xneur_init();
	sound_init();
	popup_init();

	for (int i=0; i<xconfig->plugins->data_count; i++)
	{
		program->plugin->add(program->plugin, xconfig->plugins->data[i].string);
	}

	program->buffer->handle = xconfig->handle;
	program->correction_buffer->handle = xconfig->handle;
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
	printf("  -g, --generate          Generate proto for new language. THIS OPTION FOR DEVELOPERS ONLY!\n");
}

static void xneur_version(void)
{
	printf("\nXneur - automatic keyboard switcher (version %s) \n", VERSION);
	printf("mailto: andrewcrew@rambler.ru\n\n");
	printf("web: http://www.xneur.ru/\n");
}

static void xneur_about(void)
{
	printf("\nXneur\n");
	printf("Automatic keyboard switcher (version %s) \n", VERSION);
	printf("It's program like Punto Switcher. \n");
	printf("This utility is made for X Window System.\n\n");
	printf("mailto: andrewcrew@rambler.ru\n");
	printf("web: http://www.xneur.ru/\n");
}

static void xneur_get_options(int argc, char *argv[])
{
	static struct option longopts[] =
	{
			{ "version",		no_argument,	NULL,	'v' },
			{ "help",		no_argument,	NULL,	'h' },
			{ "about",		no_argument,	NULL,	'a' },
			{ "force",		no_argument,	NULL,	'f' },
			{ "generate",	no_argument,	NULL,	'g' },
			{ NULL,			0,		NULL,	0 }
	};

	int opted = FALSE;
	int opt;
	while ((opt = getopt_long(argc, argv, "vhafg", longopts, NULL)) != -1)
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
			case 'g':
			{
				xneur_generate_proto = TRUE;
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
	printf(LIGHT_PURPLE_COLOR "====================================================" NORMAL_COLOR "\n");
	printf(LIGHT_PURPLE_COLOR ">>> " LIGHT_PURPLE_COLOR "Please visit " RED_COLOR "http://www.xneur.ru" LIGHT_BLUE_COLOR " for support" LIGHT_PURPLE_COLOR " <<<" NORMAL_COLOR "\n");
	printf(LIGHT_PURPLE_COLOR "====================================================" NORMAL_COLOR "\n");
	printf("\n");
}

static void xneur_trap(int sig, sg_handler handler)
{
	struct sigaction sa;
	bzero(&sa, sizeof(sa));
	sa.sa_handler = handler;

	if (sigaction(sig, &sa, NULL) == -1)
	{
		log_message(ERROR, _("Can't trap signal %d"), sig);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
#ifdef ENABLE_NLS
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif	
	xneur_trap(SIGTERM, xneur_terminate);
	xneur_trap(SIGINT, xneur_terminate);
	xneur_trap(SIGHUP, xneur_reload);
	xneur_trap(SIGCHLD, xneur_zombie);
	xneur_trap(SIGTSTP, xneur_terminate);
	
	xneur_get_options(argc, argv);
	
	xneur_reklama();
	
	xconfig = xneur_config_init();
	if (xconfig == NULL)
	{
		log_message(ERROR, _("Can't init libxnconfig"));
		exit(EXIT_FAILURE);
	}

	xneur_set_lock();
	xneur_load_config();

	int process_id = xconfig->get_pid(xconfig);
	//int priority = getpriority(PRIO_PROCESS, process_id);

	setpriority(PRIO_PROCESS, process_id, -19);
	int priority = getpriority(PRIO_PROCESS, process_id);
	
	log_message(TRACE, _("Xneur process identificator is %d with nice %d"), process_id, priority);
	
	program = program_init();
	if (program == NULL)
	{
		log_message(DEBUG, _("Failed to init program structure"));
		xconfig->set_pid(xconfig, 0);
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}

	sound_init();
	popup_init();
	xneur_init();

	log_message(DEBUG, _("Init program structure complete"));
	show_notify(NOTIFY_XNEUR_START, NULL);

	program->plugin->xneur_start(program->plugin);

	if (xneur_generate_proto)
		generate_protos();
	else	
		program->process_input(program);

	xneur_cleanup();

	exit(EXIT_SUCCESS);
}
