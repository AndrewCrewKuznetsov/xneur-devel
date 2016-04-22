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

#include <sys/types.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>

#include "config_files.h"

#include "utils.h"
#include "list.h"
#include "types.h"

#include "config_main.h"

#define SHM_KEY	0x34536234

static const char *option_names[] = {"DefaultMode", "ExcludeApp", "AddBind", "LogLevel", "SetXkbGroup", "AddLanguage"};
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
};
static const unsigned int total_error_messages = sizeof(error_messages) / sizeof(error_messages[0]);

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
	if(line[0] == '#')
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
				p->current_mode = MANUAL_MODE;
			else if (strcmp(param, "Auto") == 0)
				p->current_mode = AUTO_MODE;
			else
				error_code = ERROR_INVALID_DEFAULT_MODE;
			break;
		}
		case 2: // Get Applications Names
		{
			list_add(p->excluded_apps, param);
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
			else
			{
				error_code = ERROR_INVALID_ACTION_NAME;
				break;
			}

			free(param);

			param = get_word(&line);

			hotkey_modifier modifier;
			if (strcmp(param, "None") == 0)
				modifier = MODIFIER_NONE;
			else if (strcmp(param, "Shift") == 0)
				modifier = MODIFIER_SHIFT;
			else if (strcmp(param, "Ctrl") == 0)
				modifier = MODIFIER_CONTROL;
			else
			{
				error_code = ERROR_INVALID_MODIFIER_NAME;
				break;
			}

			free(param);

			param = get_word(&line);

			hotkey_key key;
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
				p->dicts	= (struct _list **) xnmalloc(1 * sizeof(struct _list *));
				p->protos	= (struct _list **) xnmalloc(1 * sizeof(struct _list *));
				p->xkb_groups	= (int *) xnmalloc(1 * sizeof(int));
			}
			else
			{
				p->languages	= (struct _xneur_language *) xnrealloc(p->languages, (p->total_languages + 1) * sizeof(struct _xneur_language));
				p->dicts	= (struct _list **) xnrealloc(p->dicts, (p->total_languages + 1) * sizeof(struct _list *));
				p->protos	= (struct _list **) xnrealloc(p->protos, (p->total_languages + 1) * sizeof(struct _list *));
				p->xkb_groups	= (int *) xnrealloc(p->xkb_groups, (p->total_languages + 1) * sizeof(int));
			}

			char *dir = get_word(&line);

			p->languages[p->total_languages].name	= strdup(param);
			p->languages[p->total_languages].dir	= strdup(dir);

			p->dicts[p->total_languages]		= NULL;
			p->protos[p->total_languages]		= NULL;
			p->xkb_groups[p->total_languages]	= p->total_languages;

			p->total_languages++;

			free(dir);
			break;
		}
	}

	free(param);
	return error_code;
}

static enum _error parse_config_file(struct _xneur_config *p)
{
	struct _list *list = load_file_to_list(NULL, CONFIG_NAME);
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

	list_uninit(list);
	return error_code;
}

static int* get_shared_mem(void)
{
	int PERM_RW = SHM_R | (SHM_R >> 3) | (SHM_R >> 6) | SHM_W | (SHM_W >> 3) | (SHM_W >> 6);

	int init = FALSE;

	int shm_id = shmget(SHM_KEY, 4, PERM_RW);
	if (shm_id == -1)
	{
		init = TRUE;
		shm_id = shmget(SHM_KEY, 4, IPC_CREAT | PERM_RW);
		if (shm_id == -1)
			return NULL;
	}

	int *mem = (int *) shmat(shm_id, NULL, 0);
	if (mem == NULL)
		return NULL;

	if (init == TRUE)
		*mem = -1;

	return mem;
}

static void release_shared_mem(int *mem)
{
	shmdt(mem);
}

static int is_process_exists(int pid)
{
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

char *xneur_config_get_config_file_name(void)
{
	return get_file_path_name(NULL, CONFIG_NAME);
}

int xneur_config_set_pid(int xneur_pid)
{
	int *pid = get_shared_mem();
	if (pid == NULL)
		return FALSE;

	*pid = xneur_pid;

	release_shared_mem(pid);

	return TRUE;
}

int xneur_config_get_pid(void)
{
	int *pid = get_shared_mem();
	if (pid == NULL)
		return -1;

	int xneur_pid = *pid;
	if (xneur_pid == 0)
		xneur_pid = -1;

	release_shared_mem(pid);

	if (is_process_exists(xneur_pid) != TRUE)
		xneur_pid = -1;

	return xneur_pid;
}

enum _error xneur_config_load(struct _xneur_config *p)
{
	enum _error error_code = parse_config_file(p);
	if (error_code != ERROR_SUCCESS)
		return error_code;

	if (p->total_languages == 0)
		return ERROR_NO_LANGUAGES;

	int lang;
	for (lang = 0; lang < p->total_languages; lang++)
	{
		char *lang_dir = p->get_lang_dir(p, lang);

		p->dicts[lang] = load_file_to_list(lang_dir, DICT_NAME);
		if (p->dicts[lang] == NULL)
			return ERROR_DICT_NOT_FOUND;

		p->protos[lang] = load_file_to_list(lang_dir, PROTO_NAME);
		if (p->protos[lang] == NULL)
			return ERROR_PROTO_NOT_FOUND;
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

char* xneur_config_get_lang_dir_full(struct _xneur_config *p, int lang)
{
	char *dir = p->get_lang_dir(p, lang);
	if (dir == NULL)
		return NULL;

	return get_file_path_name(NULL, dir);
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

void xneur_config_uninit(struct _xneur_config *p)
{
	list_uninit(p->excluded_apps);

	int lang;
	for (lang = 0; lang < p->total_languages; lang++)
	{
		if (p->dicts[lang] != NULL)
			list_uninit(p->dicts[lang]);

		if (p->protos[lang] != NULL)
			list_uninit(p->protos[lang]);

		free(p->languages[lang].name);
		free(p->languages[lang].dir);
	}

	if (p->total_languages != 0)
	{
		free(p->dicts);
		free(p->protos);
		free(p->xkb_groups);
		free(p->languages);
	}

	free(p);
}

struct _xneur_config* xneur_config_init(void)
{
	struct _xneur_config *p = (struct _xneur_config *) xnmalloc(sizeof(struct _xneur_config));
	bzero(p, sizeof(struct _xneur_config));

	p->current_mode			= AUTO_MODE;
	p->log_level			= LOG;
	p->excluded_apps		= list_init();

	// Function mapping
	p->load				= xneur_config_load;
	p->get_lang_dir			= xneur_config_get_lang_dir;
	p->get_lang_dir_full		= xneur_config_get_lang_dir_full;
	p->get_lang_name		= xneur_config_get_lang_name;
	p->find_group_lang		= xneur_config_find_group_lang;
	p->get_lang_group		= xneur_config_get_lang_group;
	p->uninit			= xneur_config_uninit;

	return p;
}
