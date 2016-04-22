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
 *  (c) XNeur Team 2006, 2007
 *
 */

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <signal.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "xnconfig_files.h"
#include "xnconfig_memory.h"

#include "utils.h"
#include "list_char.h"
#include "types.h"
#include "log.h"

#include "xnconfig.h"

#define LIBRARY_API_VERSION_MAJOR	2
#define LIBRARY_API_VERSION_MINOR	0

static const char *option_names[] = 	{
						"DefaultMode", "ExcludeApp", "AddBind", "LogLevel", "SetXkbGroup", "AddLanguage",
						"VowelLetter", "ConsonantLetter", "NoFirstLetter",
						"SetAutoApp", "SetManualApp", "GrabMouse", "EducationMode",
						"Version", "LayoutRememberMode", "SaveSelectionMode",
						"DefaultXkbGroup", "AddSound", "PlaySound", "SendDelay"
					};

static const int total_options = sizeof(option_names) / sizeof(option_names[0]);

static const char *log_types[] = {"Error", "Warning", "Log", "Debug"};
static const char *action_names[] = {"ChangeWord", "ChangeString", "ChangeMode", 
									"ChangeSelected", "TranslitSelected", "ChangecaseSelected",
									"EnableLayout1", "EnableLayout2", "EnableLayout3", "EnableLayout4"};

static const char *sound_names[] = {"PressKeyLayout1", "PressKeyLayout2", "PressKeyLayout3", "PressKeyLayout4",
									"EnableLayout1", "EnableLayout2", "EnableLayout3", "EnableLayout4",
									"AutomaticChangeWord", "ManualChangeWord", "ChangeString", 
									"ChangeSelected", "TranslitSelected", "ChangecaseSelected"};
									
static const char *modifier_names[] = {"Shift", "Control", "Alt", "Super"};

static int glob_lang = -1;

static char* get_word(char **string)
{
	char *str = *string;
	char quote = ' ';

	while (TRUE)
	{
		char sym = *str;
		if (sym == ' ')
		{
			str++;
			continue;
		}

		if (sym == '"' || sym == '\'')
		{
			quote = sym;
			str++;
		}
		break;
	}

	*string = str;

	int i, len = strlen(str);
	for (i = 0; i < len; i++)
	{
		if (str[i] == quote)
		{
			str[i++] = NULLSYM;
			break;
		}
	}

	*string += i;

	return str;
}

static int find_lang_index(struct _xneur_config *p, const char *name)
{
	int lang;
	for (lang = 0; lang < p->total_languages; lang++)
	{
		if (strcmp(p->languages[lang].name, name) == 0)
			return lang;
	}
	return NO_LANGUAGE;
}

static int get_option_index(char *option)
{
	for (int i = 0; i < total_options; i++)
	{
		if (strcmp(option, option_names[i]) == 0)
			return i + 1;
	}
	return 0;
}

static void parse_line(struct _xneur_config *p, char *line)
{
	if (line[0] == '#')
		return;

	char *option = get_word(&line);

	int index = get_option_index(option);
	if (index == 0)
	{
		log_message(WARNING, "Unrecognized option \"%s\" detected", option);
		return;
	}

	char *param = get_word(&line);

	switch (index)
	{
		case 1: // Get Default Mode (Manual/Auto)
		{
			if (strcmp(param, "Manual") == 0)
				p->set_current_mode(p, MANUAL_MODE);
			else if (strcmp(param, "Auto") == 0)
				p->set_current_mode(p, AUTO_MODE);
			else
				log_message(WARNING, "Invalid value for default mode specified");
			break;
		}
		case 2: // Get Applications Names
		{
			p->excluded_apps->add(p->excluded_apps, param);
			break;
		}
		case 3: // Get Keyboard Binds
		{
			int action;
			if (strcmp(param, "ChangeWord") == 0)
				action = ACTION_CHANGE_WORD;
			else if (strcmp(param, "ChangeString") == 0)
				action = ACTION_CHANGE_STRING;
			else if (strcmp(param, "ChangeMode") == 0)
				action = ACTION_CHANGE_MODE;
			else if (strcmp(param, "ChangeSelected") == 0)
				action = ACTION_CHANGE_SELECTED;
			else if (strcmp(param, "TranslitSelected") == 0)
				action = ACTION_TRANSLIT_SELECTED;
			else if (strcmp(param, "ChangecaseSelected") == 0)
				action = ACTION_CHANGECASE_SELECTED;
			else if (strcmp(param, "EnableLayout1") == 0)
				action = ACTION_ENABLE_LAYOUT_0;
			else if (strcmp(param, "EnableLayout2") == 0)
				action = ACTION_ENABLE_LAYOUT_1;
			else if (strcmp(param, "EnableLayout3") == 0)
				action = ACTION_ENABLE_LAYOUT_2;
			else if (strcmp(param, "EnableLayout4") == 0)
				action = ACTION_ENABLE_LAYOUT_3;
			else
			{
				log_message(WARNING, "Invalid value for action name specified");
				break;
			}

			char *str = malloc(strlen(line) * sizeof(char));
			char *str1;
			strcpy(str, line);
			const char *delim = " ";
			str1 = strtok(str, delim);
			while (str1 != NULL)
			{
				if (strcmp(str1, modifier_names[0]) == 0)
					p->hotkeys[action].modifier1 = strdup(str1);
				else if (strcmp(str1, modifier_names[1]) == 0)
					p->hotkeys[action].modifier2 = strdup(str1);
				else if (strcmp(str1, modifier_names[2]) == 0)
					p->hotkeys[action].modifier3 = strdup(str1);
				else if (strcmp(str1, modifier_names[3]) == 0)
					p->hotkeys[action].modifier4 = strdup(str1);
				else
					p->hotkeys[action].key = strdup(str1);
				
				str1 = strtok(NULL, delim);
			}
			break;
		}
		case 4: // Get Debug Mode
		{
			if (strcmp(param, "Error") == 0)
				p->log_level = ERROR;
			else if (strcmp(param, "Log") == 0)
				p->log_level = LOG;
			else if (strcmp(param, "Debug") == 0)
				p->log_level = DEBUG;
			else if (strcmp(param, "Warning") == 0)
				p->log_level = WARNING;
			else
			{
				log_message(WARNING, "Invalid value for log level specified");
				break;
			}

			log_set_level(p->log_level);
			break;
		}
		case 5:
		{
			int lang = find_lang_index(p, param);
			if (lang == NO_LANGUAGE)
			{
				log_message(WARNING, "Can't find language with name %s", param);
				break;
			}

			p->xkb_groups[lang] = atoi(get_word(&line));
			break;
		}
		case 6:
		{
			if (p->total_languages == 0)
			{
				p->languages	= (struct _xneur_language *) xnmalloc(1 * sizeof(struct _xneur_language));
				p->xkb_groups	= (int *) xnmalloc(1 * sizeof(int));
			}
			else
			{
				p->languages	= (struct _xneur_language *) xnrealloc(p->languages, (p->total_languages + 1) * sizeof(struct _xneur_language));
				p->xkb_groups	= (int *) xnrealloc(p->xkb_groups, (p->total_languages + 1) * sizeof(int));
			}

			p->languages[p->total_languages].name		= strdup(param);
			p->languages[p->total_languages].dir		= strdup(get_word(&line));

			p->languages[p->total_languages].temp_dicts	= NULL;
			p->languages[p->total_languages].dicts		= NULL;
			p->languages[p->total_languages].protos		= NULL;
			p->languages[p->total_languages].big_protos	= NULL;
			p->languages[p->total_languages].regexp		= NULL;

			p->xkb_groups[p->total_languages]		= p->total_languages;

			p->total_languages++;
			break;
		}
		case 7: // Get Vowel Letter
		{
			p->languages[glob_lang].vowel_letter = strdup(param);
			break;
		}
		case 8: // Get Consonant Letter
		{
			p->languages[glob_lang].consonant_letter = strdup(param);
			break;
		}
		case 9: // Get No First Letter
		{
			p->languages[glob_lang].nofirst_letter = strdup(param);
			break;
		}
		case 10: // Get Auto Processing Applications
		{
			p->auto_apps->add(p->auto_apps, param);
			break;
		}
		case 11: // Get Manual Processing Applications
		{
			p->manual_apps->add(p->manual_apps, param);
			break;
		}
		case 12: // Get Mouse Processing Mode
		{
			if (strcmp(param, "Yes") == 0)
				p->mouse_processing_mode = MOUSE_GRAB_ENABLE;
			else if (strcmp(param, "No") == 0)
				p->mouse_processing_mode = MOUSE_GRAB_DISABLE;
			else
				log_message(WARNING, "Invalid value for mouse processing mode specified");
			break;
		}
		case 13: // Get Education Mode
		{
			if (strcmp(param, "Yes") == 0)
				p->education_mode = EDUCATION_MODE_ENABLE;
			else if (strcmp(param, "No") == 0)
				p->education_mode = EDUCATION_MODE_DISABLE;
			else
				log_message(WARNING, "Invalid value for education mode specified");
			break;
		}
		case 14: // Get config version
		{
			p->version = strdup(param);
			break;
		}
		case 15: // Get Layout Remember Mode
		{
			if (strcmp(param, "Yes") == 0)
				p->layout_remember_mode = LAYOUTE_REMEMBER_ENABLE;
			else if (strcmp(param, "No") == 0)
				p->layout_remember_mode = LAYOUTE_REMEMBER_DISABLE;
			else
				log_message(WARNING, "Invalid value for layoute remember mode specified");
			break;
		}
		case 16: // Get Save Selection Mode
		{
			if (strcmp(param, "Yes") == 0)
				p->save_selection_mode = SELECTION_SAVE_ENABLED;
			else if (strcmp(param, "No") == 0)
				p->save_selection_mode = SELECTION_SAVE_DISABLED;
			else
				log_message(WARNING, "Invalid value for selection save mode specified");
			break;
		}
		case 17: // Get Initial Xkb Group for all new windows
		{
			p->default_group = atoi(get_word(&param));
			break;
		}
		case 18: // Sounds
		{
			int sound;
			if (strcmp(param, "PressKeyLayout1") == 0)
				sound = SOUND_PRESS_KEY_LAYOUT_0;
			else if (strcmp(param, "PressKeyLayout2") == 0)
				sound = SOUND_PRESS_KEY_LAYOUT_1;
			else if (strcmp(param, "PressKeyLayout3") == 0)
				sound = SOUND_PRESS_KEY_LAYOUT_2;
			else if (strcmp(param, "PressKeyLayout4") == 0)
				sound = SOUND_PRESS_KEY_LAYOUT_3;
			else if (strcmp(param, "EnableLayout1") == 0)
				sound = SOUND_ENABLE_LAYOUT_0;
			else if (strcmp(param, "EnableLayout2") == 0)
				sound = SOUND_ENABLE_LAYOUT_1;
			else if (strcmp(param, "EnableLayout3") == 0)
				sound = SOUND_ENABLE_LAYOUT_2;
			else if (strcmp(param, "EnableLayout4") == 0)
				sound = SOUND_ENABLE_LAYOUT_3;
			else if (strcmp(param, "AutomaticChangeWord") == 0)
				sound = SOUND_AUTOMATIC_CHANGE_WORD;
			else if (strcmp(param, "ManualChangeWord") == 0)
				sound = SOUND_MANUAL_CHANGE_WORD;
			else if (strcmp(param, "ChangeString") == 0)
				sound = SOUND_CHANGE_STRING;
			else if (strcmp(param, "ChangeSelected") == 0)
				sound = SOUND_CHANGE_SELECTED;
			else if (strcmp(param, "TranslitSelected") == 0)
				sound = SOUND_TRANSLIT_SELECTED;
			else if (strcmp(param, "ChangecaseSelected") == 0)
				sound = SOUND_CHANGECASE_SELECTED;
			else
			{
				log_message(WARNING, "Invalid value for sound action name specified");
				break;
			}
			
			p->sounds[sound] = strdup(get_word(&line));
			break;
		}
		case 19: // Play Sound
		{
			if (strcmp(param, "Yes") == 0)
				p->sound_mode = SOUND_ENABLED;
			else if (strcmp(param, "No") == 0)
				p->sound_mode = SOUND_DISABLED;
			else
				log_message(WARNING, "Invalid value for sound playing mode specified");
			break;
		}
		case 20: // Backevent Delay
		{
			p->send_delay = atoi(param);
			if (p->send_delay < 0 || p->send_delay > 50)
			{
				log_message(WARNING, "Send dealy must be between 0 and 50");
				p->send_delay = 0;
			}
			break;
		}
	}
}

static int parse_config_file(struct _xneur_config *p, const char *dir_name, const char *file_name)
{
	
	struct _list_char *list = load_list_from_file(dir_name, file_name);
	if (list == NULL)
	{
		log_message(ERROR, "Can't find config file %s", file_name);
		return FALSE;
	}

	for (int i = 0; i < list->data_count; i++)
		parse_line(p, list->data[i].string);

	list->uninit(list);
	return TRUE;
}

static int check_memory_attached(struct _xneur_config *p)
{
	if (p->xneur_data != NULL)
		return TRUE;

	p->xneur_data = (struct _xneur_data *) attach_memory_segment(sizeof(struct _xneur_data));
	if (p->xneur_data == NULL)
		return FALSE;

	return TRUE;
}

void xneur_reload_config(struct _xneur_config *p)
{
	int xneur_pid = p->xneur_data->xneur_pid;
	if (xneur_pid <= 0)
		return;

	kill(xneur_pid, SIGHUP);
}

void xneur_config_set_pid(struct _xneur_config *p, int pid)
{
	p->xneur_data->xneur_pid = pid;
}

int xneur_kill(struct _xneur_config *p)
{
	int xneur_pid = p->xneur_data->xneur_pid;
	if (xneur_pid <= 0)
		return FALSE;

	if (kill(xneur_pid, SIGTERM) == -1)
		return FALSE;

	xneur_config_set_pid(p, 0);

	return TRUE;
}

int xneur_config_get_pid(struct _xneur_config *p)
{
	int xneur_pid = p->xneur_data->xneur_pid;
	if (xneur_pid <= 0)
		return -1;

	if (getsid(xneur_pid) == -1)
		return -1;

	return p->xneur_data->xneur_pid;
}

void xneur_set_current_mode(struct _xneur_config *p, int mode)
{
	p->xneur_data->xneur_mode = mode;
}

int xneur_get_current_mode(struct _xneur_config *p)
{
	return p->xneur_data->xneur_mode;
}

int xneur_load(struct _xneur_config *p)
{
	if (!parse_config_file(p, NULL, CONFIG_NAME))
		return FALSE;

	if (p->total_languages == 0)
	{
		log_message(ERROR, "No languages specified in config file");
		return FALSE;
	}

	for (int lang = 0; lang < p->total_languages; lang++)
	{
		char *lang_dir = p->get_lang_dir(p, lang);
		char *lang_name = p->get_lang_name(p, lang);

		p->languages[lang].dicts = load_list_from_file(lang_dir, DICT_NAME);
		if (p->languages[lang].dicts == NULL)
		{
			log_message(ERROR, "Can't find dictionary file for %s language", lang_name);
			return FALSE;
		}

		p->languages[lang].protos = load_list_from_file(lang_dir, PROTO_NAME);
		if (p->languages[lang].protos == NULL)
		{
			log_message(ERROR, "Can't find protos file for %s language", lang_name);
			return FALSE;
		}

		p->languages[lang].big_protos = load_list_from_file(lang_dir, BIG_PROTO_NAME);
		if (p->languages[lang].big_protos == NULL)
		{
			log_message(ERROR, "Can't find big protos file for %s language", lang_name);
			return FALSE;
		}

		p->languages[lang].regexp = load_list_from_file(lang_dir, REGEXP_NAME);
		if (p->languages[lang].regexp == NULL)
		{
			log_message(ERROR, "Can't find regexp file for %s language", lang_name);
			return FALSE;
		}

		p->languages[lang].temp_dicts = p->languages[lang].dicts->clone(p->languages[lang].dicts);

		glob_lang = lang;
		if (!parse_config_file(p, lang_dir, LANGDEF_NAME))
			return FALSE;
	}

	return TRUE;
}

int xneur_save(struct _xneur_config *p)
{
 	char *config_file_path_name = get_home_file_path_name(NULL, CONFIG_NAME);

	log_message(LOG, "Saving main config to %s", config_file_path_name);

	FILE *stream = fopen(config_file_path_name, "w");
	if (stream == NULL)
	{
		log_message(ERROR, "Can't create file %s", config_file_path_name);
		free(config_file_path_name);
		return FALSE;
	}

	free(config_file_path_name);

	fprintf(stream, "# It's a X Neural Switcher configuration file by XNeur\n# All values writted XNeur\n\n");
	fprintf(stream, "# Config version\nVersion %s\n\n", VERSION);

	if (p->xneur_data->xneur_mode == AUTO_MODE)
		fprintf(stream, "# Default work mode\nDefaultMode Auto\n\n");
	else
		fprintf(stream, "# Default work mode\nDefaultMode Manual\n\n");

	fprintf(stream, "# Level of messages program will write to output\n");
	fprintf(stream, "#LogLevel Error\n");
	fprintf(stream, "#LogLevel Warning\n");
	fprintf(stream, "#LogLevel Log\n");
	fprintf(stream, "#LogLevel Debug\n");
	fprintf(stream, "LogLevel %s\n", log_types[p->log_level]);
	fprintf(stream, "\n");
	fprintf(stream, "# Define used languages\n");
	fprintf(stream, "# See Settings page on http://www.xneur.ru for details\n");

	for (int lang = 0; lang < p->total_languages; lang++)
	{
		fprintf(stream, "AddLanguage %s %s\n", p->languages[lang].name, p->languages[lang].dir);
		fprintf(stream, "SetXkbGroup %s %d\n", p->languages[lang].name, p->xkb_groups[lang]);
	}
	fprintf(stream, "\n");

	fprintf(stream, "# Define initial keyboard layout for all new applications\n");
	fprintf(stream, "DefaultXkbGroup %d\n", p->default_group);
	fprintf(stream, "\n");
	
	fprintf(stream, "# Add Applications names to exclude it from procces with xneur\n");
	fprintf(stream, "# Xneur will not process the input for this applications\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#ExcludeApp Gaim\n");
	for (int i = 0; i < p->excluded_apps->data_count; i++)
		fprintf(stream, "ExcludeApp %s\n", p->excluded_apps->data[i].string);
	fprintf(stream, "\n");

	fprintf(stream, "# Use this parameter to force set work mode in current application to Auto.\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#SetAutoApp Gedit\n");
	for (int i = 0; i < p->auto_apps->data_count; i++)
		fprintf(stream, "SetAutoApp %s\n", p->auto_apps->data[i].string);
	fprintf(stream, "\n");

	fprintf(stream, "# Use this parameter to force set work mode in current application to Manual.\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#SetManualApp Anjuta\n");
	for (int i = 0; i < p->manual_apps->data_count; i++)
		fprintf(stream, "SetManualApp %s\n", p->manual_apps->data[i].string);
	fprintf(stream, "\n");

	fprintf(stream, "# Binds hotkeys for some actions\n");
	for (int action = 0; action < MAX_HOTKEYS; action++)
	{
		fprintf(stream, "AddBind %s ", action_names[action]);
		if (p->hotkeys[action].modifier1 != NULL)
			fprintf(stream, "%s ", p->hotkeys[action].modifier1);
		if (p->hotkeys[action].modifier2 != NULL)
			fprintf(stream, "%s ", p->hotkeys[action].modifier2);
		if (p->hotkeys[action].modifier3 != NULL)
			fprintf(stream, "%s ", p->hotkeys[action].modifier3);
		if (p->hotkeys[action].modifier4 != NULL)
			fprintf(stream, "%s ", p->hotkeys[action].modifier4);
		fprintf(stream, "%s\n", p->hotkeys[action].key);
	}
	fprintf(stream, "\n");

	fprintf(stream, "# This option enable or disable sound playing\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#PlaySound No\n");
	if (p->sound_mode)
		fprintf(stream, "PlaySound Yes\n\n");
	else
		fprintf(stream, "PlaySound No\n\n");
	
	fprintf(stream, "# Binds sounds for some actions\n");
	for (int sound = 0; sound < MAX_SOUNDS; sound++)
	{
		if (p->sounds[sound] != NULL)
		{
			fprintf(stream, "AddSound %s ", sound_names[sound]);
			fprintf(stream, "%s\n", p->sounds[sound]);
		}		
	}
	fprintf(stream, "\n");
	
	fprintf(stream, "# This option enable or disable mouse processing\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#GrabMouse Yes\n");
	if (p->mouse_processing_mode)
		fprintf(stream, "GrabMouse Yes\n\n");
	else
		fprintf(stream, "GrabMouse No\n\n");

	fprintf(stream, "# This option enable or disable self education of xneur\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#EducationMode No\n");
	if (p->education_mode)
		fprintf(stream, "EducationMode Yes\n\n");
	else
		fprintf(stream, "EducationMode No\n\n");

	fprintf(stream, "# This option enable or disable layout remember for each window\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#LayoutRememberMode No\n");
	if (p->layout_remember_mode)
		fprintf(stream, "LayoutRememberMode Yes\n\n");
	else
		fprintf(stream, "LayoutRememberMode No\n\n");

	fprintf(stream, "# This option enable or disable saving selection text\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#SaveSelectionMode No\n");
	if (p->save_selection_mode)
		fprintf(stream, "SaveSelectionMode Yes\n\n");
	else
		fprintf(stream, "SaveSelectionMode No\n\n");

	fprintf(stream, "# This option define delay before sendind events to application (in milliseconds between 0 to 50).\n");
	fprintf(stream, "SendDelay %d\n\n", p->send_delay);
	
	fprintf(stream, "# That's all\n");

	fclose(stream);

	return TRUE;
}

int xneur_replace(struct _xneur_config *p)
{
	char *config_file_path_name = get_file_path_name(NULL, CONFIG_NAME);
	char *config_backup_file_path_name = get_file_path_name(NULL, CONFIG_BCK_NAME);

	log_message(LOG, "Moving config file from %s to %s", config_file_path_name, config_backup_file_path_name);
	
	if (rename(config_file_path_name, config_backup_file_path_name) != 0)
	{
		log_message(ERROR, "Can't move file, maybe file %s already exist", config_backup_file_path_name);
		free(config_file_path_name);
		free(config_backup_file_path_name);
		return FALSE;
	}

	free(config_file_path_name);
	free(config_backup_file_path_name);

	return p->load(p);
}

void xneur_save_dicts(struct _xneur_config *p, int lang)
{
	if (p->education_mode == EDUCATION_MODE_DISABLE)
		return;
	
	char *dict_path = get_home_file_path_name(p->get_lang_dir(p, lang), DICT_NAME);

	log_message(LOG, "Saving %s dictionary to %s", p->get_lang_name(p, lang), dict_path);

	save_list_to_file(p->languages[lang].dicts, dict_path);

	free(dict_path);
}

char* xneur_config_get_lang_dir(struct _xneur_config *p, int lang)
{
	if (lang < 0 || lang >= p->total_languages)
		return NULL;
	return p->languages[lang].dir;
}

char* xneur_config_get_lang_name(struct _xneur_config *p, int lang)
{
	if (lang < 0 || lang >= p->total_languages)
		return NULL;
	return p->languages[lang].name;
}

int xneur_config_find_group_lang(struct _xneur_config *p, int group)
{
	for (int lang = 0; lang < p->total_languages; lang++)
	{
		if (p->xkb_groups[lang] == group)
			return lang;
	}
	return NO_LANGUAGE;
}

int xneur_config_get_lang_group(struct _xneur_config *p, int lang)
{
	if (lang < 0 || lang >= p->total_languages)
		return -1;
	return p->xkb_groups[lang];
}

void xneur_set_languages(struct _xneur_config *p, char **lang_name, char **lang_dir, int *lang_group, int total_lang)
{
	for (int lang = 0; lang < p->total_languages; lang++)
	{
		if (p->languages[lang].temp_dicts != NULL)
			p->languages[lang].temp_dicts->uninit(p->languages[lang].temp_dicts);

		if (p->languages[lang].dicts != NULL)
			p->languages[lang].dicts->uninit(p->languages[lang].dicts);

		if (p->languages[lang].protos != NULL)
			p->languages[lang].protos->uninit(p->languages[lang].protos);

		if (p->languages[lang].big_protos != NULL)
			p->languages[lang].big_protos->uninit(p->languages[lang].big_protos);

		if (p->languages[lang].regexp != NULL)
			p->languages[lang].regexp->uninit(p->languages[lang].regexp);

		free(p->languages[lang].name);
		free(p->languages[lang].dir);
	}

	if (p->total_languages != 0)
	{
		free(p->xkb_groups);
		free(p->languages);
	}

	p->total_languages = total_lang;
	p->languages	= (struct _xneur_language *) xnmalloc(p->total_languages * sizeof(struct _xneur_language));
	p->xkb_groups	= (int *) xnmalloc(p->total_languages * sizeof(int));

	for (int lang = 0; lang < p->total_languages; lang++)
	{
		p->languages[lang].name		= strdup(lang_name[lang]);
		p->languages[lang].dir		= strdup(lang_dir[lang]);

		p->languages[lang].temp_dicts	= NULL;
		p->languages[lang].dicts	= NULL;
		p->languages[lang].protos	= NULL;
		p->languages[lang].big_protos	= NULL;
		p->languages[lang].regexp	= NULL;

		p->xkb_groups[lang]		= lang_group[lang];
	}

}

void xneur_get_library_api_version(int *major_version, int *minor_version)
{
	*major_version = LIBRARY_API_VERSION_MAJOR;
	*minor_version = LIBRARY_API_VERSION_MINOR;
}

void xneur_config_uninit(struct _xneur_config *p)
{
	p->window_layouts->uninit(p->window_layouts);
	p->manual_apps->uninit(p->manual_apps);
	p->auto_apps->uninit(p->auto_apps);
	p->excluded_apps->uninit(p->excluded_apps);

	if (p->version != NULL)
		free(p->version);
	
	for (int lang = 0; lang < p->total_languages; lang++)
	{
		if (p->languages[lang].temp_dicts != NULL)
			p->languages[lang].temp_dicts->uninit(p->languages[lang].temp_dicts);

		if (p->languages[lang].dicts != NULL)
			p->languages[lang].dicts->uninit(p->languages[lang].dicts);

		if (p->languages[lang].protos != NULL)
			p->languages[lang].protos->uninit(p->languages[lang].protos);

		if (p->languages[lang].big_protos != NULL)
			p->languages[lang].big_protos->uninit(p->languages[lang].big_protos);

		if (p->languages[lang].regexp != NULL)
			p->languages[lang].regexp->uninit(p->languages[lang].regexp);

		free(p->languages[lang].name);
		free(p->languages[lang].dir);
	}

	if (p->total_languages != 0)
	{
		free(p->xkb_groups);
		free(p->languages);
	}

	for (int sound = 0; sound < MAX_SOUNDS; sound++)
	{
		if (p->sounds[sound] != NULL)
			free(p->sounds[sound]);
	}
	
	free(p->sounds);

	for (int action = 0; action < MAX_HOTKEYS; action++)
	{
		if (p->hotkeys[action].modifier1 != NULL)
			free(p->hotkeys[action].modifier1);
		if (p->hotkeys[action].modifier2 != NULL)
			free(p->hotkeys[action].modifier2);
		if (p->hotkeys[action].modifier3 != NULL)
			free(p->hotkeys[action].modifier3);
		if (p->hotkeys[action].modifier4 != NULL)
			free(p->hotkeys[action].modifier4);
		if (p->hotkeys[action].key != NULL)
			free(p->hotkeys[action].key);
	}
		
	free(p);
}

struct _xneur_config* xneur_config_init(void)
{
	struct _xneur_config *p = (struct _xneur_config *) xnmalloc(sizeof(struct _xneur_config));
	bzero(p, sizeof(struct _xneur_config));
		
	if (!check_memory_attached(p))
	{
		free(p);
		return NULL;
	}

	p->sounds = (char **) xnmalloc(MAX_SOUNDS * sizeof(char *));
	bzero(p->sounds, MAX_SOUNDS * sizeof(char *));
	
	p->log_level			= LOG;
	p->excluded_apps		= list_char_init();
	p->auto_apps			= list_char_init();
	p->manual_apps			= list_char_init();
	p->window_layouts		= list_char_init();

	p->send_delay			= 0;
	p->default_group		= 0;
	
	p->sound_mode			= -1;
	p->mouse_processing_mode	= -1;
	p->education_mode		= -1;
	p->layout_remember_mode		= -1;
	p->save_selection_mode		= -1;
	
	// Function mapping
	p->load				= xneur_load;
	p->save				= xneur_save;
	p->replace			= xneur_replace;
	p->save_dicts			= xneur_save_dicts;
	p->set_pid			= xneur_config_set_pid;
	p->get_pid			= xneur_config_get_pid;
	p->set_current_mode		= xneur_set_current_mode;
	p->get_current_mode		= xneur_get_current_mode;
	p->get_lang_dir			= xneur_config_get_lang_dir;
	p->get_lang_name		= xneur_config_get_lang_name;
	p->find_group_lang		= xneur_config_find_group_lang;
	p->get_lang_group		= xneur_config_get_lang_group;
	p->reload_config		= xneur_reload_config;
	p->kill				= xneur_kill;
	p->set_languages		= xneur_set_languages;
	p->get_library_api_version	= xneur_get_library_api_version;
	p->get_dict_path		= get_file_path_name;
	p->get_home_dict_path		= get_home_file_path_name;
	p->uninit			= xneur_config_uninit;

	p->set_current_mode(p, AUTO_MODE);
	
	return p;
}
