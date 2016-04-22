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

#include <sys/types.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "xnconfig_files.h"
#include "xnconfig_memory.h"

#include "utils.h"
#include "list.h"
#include "types.h"
#include "log.h"

#include "xnconfig.h"

static const char *option_names[] = 	{
						"DefaultMode", "ExcludeApp", "AddBind", "LogLevel", "SetXkbGroup", "AddLanguage",
						"VowelLetter", "ConsonantLetter", "NoFirstLetter",
						"SetAutoApp", "SetManualApp", "GrabMouse", "EducationMode",
						"Version", "LayoutRememberMode", "SaveSelectionMode"
					};

static const int total_options = sizeof(option_names) / sizeof(option_names[0]);

static const char *error_messages[] =
{
	"Operation completed succsefully",
	"Unrecognized option detected",
	"Invalid value for default mode specified",
	"Invalid value for action name specified",
	"Invalid value for modifier name specified",
	"Invalid value for key name specified",
	"Invalid value for log level specified",
	"Undefined language name in group binding detected",
	"Can't load config file",
	"No languages specified in config file",
	"Can't open dictionary file",
	"Can't open proto file",
	"Can't open language definition file",
	"Invalid value for mouse processing mode",
	"Invalid value for education mode",
	"Invalid configuration file",
	"Invalid layout remember mode",
	"Invalid save selection mode",
	"Can't create file",
};

static const unsigned int total_error_messages = sizeof(error_messages) / sizeof(error_messages[0]);

static int glob_lang = -1;

static char* get_word(char **string)
{
	char *str = *string;

	while (TRUE)
	{
		if (*str != ' ')
			break;
		str++;
	}
	*string = str;

	int i, len = strlen(str);
	for (i = 0; i < len; i++)
	{
		if (str[i] == ' ')
			break;
	}

	*string += i + 1;

	char *word = (char *) xnmalloc(i + 1);
	memcpy(word, str, i);
	word[i] = NULLSYM;
	return word;
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

static enum _error parse_line(struct _xneur_config *p, char *line)
{
	if (line[0] == '#')
		return ERROR_SUCCESS;

	char *option = get_word(&line);

	int i, index = 0;
	for (i = 0; i < total_options; i++)
	{
		if (strcmp(option, option_names[i]) == 0)
		{
			index = i + 1;
			break;
		}
	}

	if (index == 0)
	{
		free(option);
		return ERROR_UNRECOGNIZED_OPTION;
	}

	free(option);

	char *param = get_word(&line);

	enum _error error_code = ERROR_SUCCESS;
	switch (index)
	{
		case 1: // Get Default Mode (Manual/Auto)
		{
			if (strcmp(param, "Manual") == 0)
				p->set_current_mode(p, MANUAL_MODE);
			else if (strcmp(param, "Auto") == 0)
				p->set_current_mode(p, AUTO_MODE);
			else
				error_code = ERROR_INVALID_DEFAULT_MODE;
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
			else
			{
				error_code = ERROR_INVALID_ACTION_NAME;
				break;
			}

			free(param);

			param = get_word(&line);

			enum _hotkey_modifier modifier;
			if (strcmp(param, "None") == 0)
				modifier = MODIFIER_NONE;
			else if (strcmp(param, "Shift") == 0)
				modifier = MODIFIER_SHIFT;
			else if (strcmp(param, "Ctrl") == 0)
				modifier = MODIFIER_CONTROL;
			else if (strcmp(param, "Alt") == 0)
				modifier = MODIFIER_ALT;
			else
			{
				error_code = ERROR_INVALID_MODIFIER_NAME;
				break;
			}

			free(param);

			param = get_word(&line);

			enum _hotkey_key key;
			if (strcmp(param, "None") == 0)
				key = KEY_NONE;
			else if (strcmp(param, "Break") == 0)
				key = KEY_BREAK;
			else if (strcmp(param, "ScrollLock") == 0)
				key = KEY_SCROLL_LOCK;
			else if (strcmp(param, "PrtSc") == 0)
				key = KEY_PRINT_SCREEN;
			else
			{
				error_code = ERROR_INVALID_KEY_NAME;
				break;
			}

			p->hotkeys[action].modifier = modifier;
			p->hotkeys[action].key = key;

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
			else
				error_code = ERROR_INVALID_LOG_LEVEL;
			break;
		}
		case 5:
		{
			int lang = find_lang_index(p, param);
			if (lang == NO_LANGUAGE)
			{
				error_code = ERROR_UNDEFINED_LANGUAGE_NAME;
				break;
			}

			free(param);

			param = get_word(&line);
			p->xkb_groups[lang] = atoi(param);
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

			char *dir = get_word(&line);

			p->languages[p->total_languages].name		= strdup(param);
			p->languages[p->total_languages].dir		= strdup(dir);

			p->languages[p->total_languages].temp_dicts	= NULL;
			p->languages[p->total_languages].dicts		= NULL;
			p->languages[p->total_languages].protos		= NULL;
			p->languages[p->total_languages].big_protos	= NULL;
			p->languages[p->total_languages].regexp		= NULL;

			p->xkb_groups[p->total_languages]		= p->total_languages;

			p->total_languages++;

			free(dir);
			break;
		}
		case 7: // Get Vowel Letter
		{
			p->languages[glob_lang].vowel_letter = xnmalloc(strlen(param));
			strcpy(p->languages[glob_lang].vowel_letter, param);
			break;
		}
		case 8: // Get Consonant Letter
		{
			p->languages[glob_lang].consonant_letter = xnmalloc(strlen(param));
			strcpy(p->languages[glob_lang].consonant_letter, param);
			break;
		}
		case 9: // Get No First Letter
		{
			p->languages[glob_lang].nofirst_letter = xnmalloc(strlen(param));
			strcpy(p->languages[glob_lang].nofirst_letter, param);
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
				error_code = ERROR_INVALID_MOUSE_MODE;
			break;
		}
		case 13: // Get Education Mode
		{
			if (strcmp(param, "Yes") == 0)
				p->education_mode = EDUCATION_MODE_ENABLE;
			else if (strcmp(param, "No") == 0)
				p->education_mode = EDUCATION_MODE_DISABLE;
			else
				error_code = ERROR_INVALID_EDUCATION_MODE;
			break;
		}
		case 14: // Get config version
		{
			break;
		}
		case 15: // Get Layout Remember Mode
		{
			if (strcmp(param, "Yes") == 0)
				p->layout_remember_mode = LAYOUTE_REMEMBER_ENABLE;
			else if (strcmp(param, "No") == 0)
				p->layout_remember_mode = LAYOUTE_REMEMBER_DISABLE;
			else
				error_code = ERROR_INVALID_LAYOUTE_REMEMBER_MODE;
			break;
		}
		case 16: // Get Save Selection Mode
		{
			if (strcmp(param, "Yes") == 0)
				p->save_selection_mode = SAVESELECTION_ENABLE;
			else if (strcmp(param, "No") == 0)
				p->save_selection_mode = SAVESELECTION_DISABLE;
			else
				error_code = ERROR_INVALID_SAVESELECTION_MODE;
			break;
		}
	}

	free(param);
	return error_code;
}

static enum _error parse_config_file(struct _xneur_config *p, const char *dir_name, const char *file_name)
{
	struct _list *list = load_list_from_file(dir_name, file_name);
	if (list == NULL)
		return ERROR_CANT_LOAD_FILE;

	enum _error error_code = ERROR_SUCCESS;
	int i;
	for (i = 0; i < list->data_count; i++)
	{
		error_code = parse_line(p, list->data[i].string);
		if (error_code != ERROR_SUCCESS)
			break;
	}

	list->uninit(list);
	return error_code;
}

static int is_process_exists(int pid)
{
	if (pid <= 0)
		return FALSE;

	int cmdline_file_name_len = 64;
	char *cmdline_file_name = (char *) xnmalloc((cmdline_file_name_len + 1) * sizeof(char));

	snprintf(cmdline_file_name, cmdline_file_name_len, "/proc/%d/cmdline", pid);

	FILE *cmdline_file = fopen(cmdline_file_name, "r");
	free(cmdline_file_name);

	if (!cmdline_file)
		return FALSE;

	fclose(cmdline_file);

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

void xneur_config_set_pid(struct _xneur_config *p, int pid)
{
	p->xneur_data->xneur_pid = pid;
}

int xneur_config_get_pid(struct _xneur_config *p)
{
	int xneur_pid = p->xneur_data->xneur_pid;

	if (is_process_exists(xneur_pid) != TRUE)
		return -1;

	return xneur_pid;
}

void xneur_set_current_mode(struct _xneur_config *p, int mode)
{
	p->xneur_data->xneur_mode = mode;
}

int xneur_get_current_mode(struct _xneur_config *p)
{
	return p->xneur_data->xneur_mode;
}

enum _error xneur_load(struct _xneur_config *p)
{
	enum _error error_code = parse_config_file(p, NULL, CONFIG_NAME);
	if (error_code != ERROR_SUCCESS)
		return error_code;

	if (p->total_languages == 0)
		return ERROR_NO_LANGUAGES;

	int lang;
	for (lang = 0; lang < p->total_languages; lang++)
	{
		char *lang_dir = p->get_lang_dir(p, lang);

		p->languages[lang].temp_dicts = load_list_from_file(lang_dir, DICT_NAME);
		if (p->languages[lang].temp_dicts == NULL)
			return ERROR_DICT_NOT_FOUND;

		p->languages[lang].dicts = load_list_from_file(lang_dir, DICT_NAME);
		if (p->languages[lang].dicts == NULL)
			return ERROR_DICT_NOT_FOUND;

		p->languages[lang].protos = load_list_from_file(lang_dir, PROTO_NAME);
		if (p->languages[lang].protos == NULL)
			return ERROR_PROTO_NOT_FOUND;

		p->languages[lang].big_protos = load_list_from_file(lang_dir, BIG_PROTO_NAME);
		if (p->languages[lang].big_protos == NULL)
			return ERROR_PROTO_NOT_FOUND;

		p->languages[lang].regexp = load_list_from_file(lang_dir, REGEXP_NAME);
		if (p->languages[lang].regexp == NULL)
			return ERROR_PROTO_NOT_FOUND;

		glob_lang = lang;
		error_code = parse_config_file(p, lang_dir, LANGDEF_NAME);
		if (error_code != ERROR_SUCCESS)
			return error_code;
	}

	return ERROR_SUCCESS;
}

enum _error xneur_save(struct _xneur_config *p)
{
	char *config_file_path_name = get_file_path_name(NULL, CONFIG_NAME);

	log_message(LOG, "Saving main config to %s", config_file_path_name);

	FILE *stream = fopen(config_file_path_name, "w");

	free(config_file_path_name);

	if (stream == NULL)
		return ERROR_CANT_CREATE_FILE;

	fputs("# It's a X Neural Switcher configuration file by XNeur\n# All values writted XNeur\n\n", stream);

	fprintf(stream, "# Config version \nVersion %s\n\n", VERSION);

	if (p->xneur_data->xneur_mode == AUTO_MODE)
		fprintf(stream, "# Default work mode\nDefaultMode Auto\n\n");
	else
		fprintf(stream, "# Default work mode\nDefaultMode Manual\n\n");

	fputs("# Level of messages program will write to output\n", stream);
	fputs("# Example:\n", stream);
	fputs("#LogLevel Error\n", stream);
	fputs("#LogLevel Log\n", stream);
	fputs("#LogLevel Debug\n", stream);
	switch (p->log_level)
	{
		case LOG:
		{
			fprintf(stream, "LogLevel Log\n\n");
			break;
		}
		case DEBUG:
		{
			fprintf(stream, "LogLevel Debug\n\n");
			break;
		}
		case ERROR:
		{
			fprintf(stream, "LogLevel Error\n\n");
			break;
		}
	}

	fputs("# Define used languages\n", stream);
	fputs("# See Settings page on http://www.xneur.ru for details\n", stream);

	int lang;
	for (lang = 0; lang < p->total_languages; lang++)
	{
		fprintf(stream, "AddLanguage %s %s\n", p->languages[lang].name, p->languages[lang].dir);
		fprintf(stream, "SetXkbGroup %s %d\n", p->languages[lang].name, p->xkb_groups[lang]);
	}
	fputs("\n", stream);

	fputs("# Add Applications names to exclude it from procces with xneur\n", stream);
	fputs("# Xneur will not process the input for this applications\n", stream);
	fputs("# Example:\n", stream);
	fputs("#ExcludeApp Gaim\n", stream);
	for (int i = 0; i < p->excluded_apps->data_count; i++)
		fprintf(stream, "ExcludeApp %s\n", p->excluded_apps->data[i].string);
	fputs("\n", stream);

	fputs("# Use this parameter to force set work mode in current application to Auto.\n", stream);
	fputs("# Example:\n", stream);
	fputs("#SetAutoApp Gedit\n", stream);
	for (int i = 0; i < p->auto_apps->data_count; i++)
		fprintf(stream, "SetAutoApp %s\n", p->auto_apps->data[i].string);
	fputs("\n", stream);

	fputs("# Use this parameter to force set work mode in current application to Manual.\n", stream);
	fputs("# Example:\n", stream);
	fputs("#SetManualApp Anjuta\n", stream);
	for (int i = 0; i < p->manual_apps->data_count; i++)
		fprintf(stream, "SetManualApp %s\n", p->manual_apps->data[i].string);
	fputs("\n", stream);

	fputs("# Binds hotkeys for some actions\n", stream);
	for (int action = 0; action < MAX_HOTKEYS; action++)
	{
		switch (action)
		{
			case ACTION_CHANGE_WORD:
			{
				fputs("AddBind ChangeWord ", stream);
				break;
			}
			case ACTION_CHANGE_STRING:
			{
				fputs("AddBind ChangeString ", stream);
				break;
			}
			case ACTION_CHANGE_MODE:
			{
				fputs("AddBind ChangeMode ", stream);
				break;
			}
			case ACTION_CHANGE_SELECTED:
			{
				fputs("AddBind ChangeSelected ", stream);
				break;
			}
			case ACTION_TRANSLIT_SELECTED:
			{
				fputs("AddBind TranslitSelected ", stream);
				break;
			}
			case ACTION_CHANGECASE_SELECTED:
			{
				fputs("AddBind ChangecaseSelected ", stream);
				break;
			}
		}

		switch (p->hotkeys[action].modifier)
		{
			case MODIFIER_NONE:
			{
				fputs("None ", stream);
				break;
			}
			case MODIFIER_SHIFT:
			{
				fputs("Shift ", stream);
				break;
			}
			case MODIFIER_CONTROL:
			{
				fputs("Ctrl ", stream);
				break;
			}
			case MODIFIER_ALT:
			{
				fputs("Alt ", stream);
				break;
			}
		}

		switch (p->hotkeys[action].key)
		{
			case KEY_NONE:
			{
				fputs("None \n", stream);
				break;
			}
			case KEY_BREAK:
			{
				fputs("Break \n", stream);
				break;
			}
			case KEY_SCROLL_LOCK:
			{
				fputs("ScrollLock \n", stream);
				break;
			}
			case KEY_PRINT_SCREEN:
			{
				fputs("PrtSc \n", stream);
				break;
			}
		}
	}
	fputs("\n", stream);

	fputs("# This option enable or disable mouse processing\n", stream);
	fputs("# Example:\n", stream);
	fputs("#GrabMouse Yes\n", stream);
	if (p->mouse_processing_mode)
		fprintf(stream, "GrabMouse Yes\n\n");
	else
		fprintf(stream, "GrabMouse No\n\n");

	fputs("# This option enable or disable self education of xneur\n", stream);
	fputs("# Example:\n", stream);
	fputs("#EducationMode No\n", stream);
	if (p->education_mode)
		fprintf(stream, "EducationMode Yes\n\n");
	else
		fprintf(stream, "EducationMode No\n\n");

	fputs("# This option enable or disable layout remember for each window\n", stream);
	fputs("# Example:\n", stream);
	fputs("#LayoutRememberMode No\n", stream);
	if (p->layout_remember_mode)
		fprintf(stream, "LayoutRememberMode Yes\n\n");
	else
		fprintf(stream, "LayoutRememberMode No\n\n");

	fputs("# This option enable or disable saving selection text\n", stream);
	fputs("# Example:\n", stream);
	fputs("#SaveSelectionMode No\n", stream);
	if (p->save_selection_mode)
		fprintf(stream, "SaveSelectionMode Yes\n\n");
	else
		fprintf(stream, "SaveSelectionMode No\n\n");

	fputs("# That's all\n", stream);

	fclose(stream);

	return ERROR_SUCCESS;
}

enum _error xneur_save_dicts(struct _xneur_config *p)
{
	if (p->education_mode == EDUCATION_MODE_DISABLE)
		return ERROR_SUCCESS;

	int lang;
	for (lang = 0; lang < p->total_languages; lang++)
	{
		char *dict_path = get_file_path_name(p->get_lang_dir(p, lang), DICT_NAME);

		log_message(LOG, "Saving %s dictionary to %s", p->get_lang_name(p, lang), dict_path);

		save_list_to_file(p->languages[lang].dicts, dict_path);

		free(dict_path);
	}

	return ERROR_SUCCESS;
}

const char* format_error_message(enum _error error_code)
{
	if (error_code >= total_error_messages)
		return "Unrecognized error";
	return error_messages[error_code];
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
	int lang;
	for (lang = 0; lang < p->total_languages; lang++)
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
	int lang;
	for (lang = 0; lang < p->total_languages; lang++)
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
	for (lang = 0; lang < p->total_languages; lang++)
	{
		p->languages[lang].name	= strdup(lang_name[lang]);
		p->languages[lang].dir	= strdup(lang_dir[lang]);

		p->languages[lang].temp_dicts		= NULL;
		p->languages[lang].dicts			= NULL;
		p->languages[lang].protos			= NULL;
		p->languages[lang].big_protos		= NULL;
		p->languages[lang].regexp			= NULL;

		p->xkb_groups[lang]	= lang_group[lang];
	}

}

void xneur_config_uninit(struct _xneur_config *p)
{
	p->excluded_apps->uninit(p->excluded_apps);

	int lang;
	for (lang = 0; lang < p->total_languages; lang++)
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

	free(p);
}

struct _xneur_config* xneur_config_init(void)
{
	struct _xneur_config *p = (struct _xneur_config *) xnmalloc(sizeof(struct _xneur_config));
	bzero(p, sizeof(struct _xneur_config));

	if (check_memory_attached(p) != TRUE)
	{
		free(p);
		return NULL;
	}

	p->log_level			= LOG;
	p->excluded_apps		= list_init();
	p->auto_apps			= list_init();
	p->manual_apps			= list_init();

	p->window_layouts		= list_init();

	// Function mapping
	p->load				= xneur_load;
	p->save				= xneur_save;
	p->save_dicts			= xneur_save_dicts;
	p->set_pid			= xneur_config_set_pid;
	p->get_pid			= xneur_config_get_pid;
	p->set_current_mode		= xneur_set_current_mode;
	p->get_current_mode		= xneur_get_current_mode;
	p->get_lang_dir			= xneur_config_get_lang_dir;
	p->get_lang_name		= xneur_config_get_lang_name;
	p->find_group_lang		= xneur_config_find_group_lang;
	p->get_lang_group		= xneur_config_get_lang_group;
	p->set_languages		= xneur_set_languages;
	p->get_file_path_name		= get_file_path_name;
	p->uninit			= xneur_config_uninit;

	p->set_current_mode(p, AUTO_MODE);

	return p;
}
