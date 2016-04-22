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

#ifndef _CONFIG_MAIN_H_
#define _CONFIG_MAIN_H_

#define PROGRAM_NAME	"XNeur"

#define CONFIG_DIR	"xneur"
#define CONFIG_NAME	"xneurrc"
#define PID_FILE_NAME	"xneur.pid"

#define DICT_NAME	"dict"
#define PROTO_NAME	"proto"

#define AUTO_MODE	0
#define MANUAL_MODE	1

#define NO_LANGUAGE	-1

#define MAX_HOTKEYS	4

typedef enum _error
{
	ERROR_SUCCESS,
	ERROR_UNRECOGNIZED_OPTION,
	ERROR_INVALID_DEFAULT_MODE,
	ERROR_INVALID_ACTION_NAME,
	ERROR_INVALID_MODIFIER_NAME,
	ERROR_INVALID_KEY_NAME,
	ERROR_INVALID_LOG_LEVEL,
	ERROR_UNDEFINED_LANGUAGE_NAME,
	ERROR_CANT_LOAD_FILE,
	ERROR_NO_LANGUAGES,
	ERROR_DICT_NOT_FOUND,
	ERROR_PROTO_NOT_FOUND,
} error;

enum _hotkey_action
{
	ACTION_CHANGE_WORD,
	ACTION_CHANGE_STRING,
	ACTION_CHANGE_MODE,
	ACTION_CHANGE_SELECTED,
	ACTION_NONE,
} hotkey_action;

typedef enum _hotkey_modifier
{
	MODIFIER_NONE,
	MODIFIER_SHIFT,
	MODIFIER_CONTROL,
} hotkey_modifier;

typedef enum _hotkey_key
{
	KEY_NONE,
	KEY_BREAK,
	KEY_SCROLL_LOCK,
	KEY_PRINT_SCREEN,
} hotkey_key;

typedef struct _xneur_language
{
        char *dir;
        char *name;
} xneur_language;

typedef struct _xneur_hotkey
{
	hotkey_modifier modifier;
	hotkey_key key;
} xneur_hotkey;

const char* format_error_message(enum _error error_code);
int xneur_config_get_pid(void);
char *xneur_config_get_config_file_name(void);

typedef struct _xneur_config
{
	struct _list *excluded_apps;
	struct _list **dicts;
	struct _list **protos;

	struct _xneur_language *languages;		// Array of languages used in program
	struct _xneur_hotkey hotkeys[MAX_HOTKEYS];	// Array of hotkeys used in program

	int   total_languages;				// Total languages to work with
	int   current_mode;				// Current mode, auto or manual
	int   log_level;				// Maximum level of log messages to print
	int   *xkb_groups;				// XKB Group for each language
	
	enum _error (*load) (struct _xneur_config *p);
	char* (*get_lang_dir) (struct _xneur_config *p, int lang);
	char* (*get_lang_dir_full) (struct _xneur_config *p, int lang);
	char* (*get_lang_name) (struct _xneur_config *p, int lang);
	int   (*find_group_lang) (struct _xneur_config *p, int group);
	int   (*get_lang_group) (struct _xneur_config *p, int lang);
	void  (*uninit) (struct _xneur_config *p);
} xneurconfig;

struct _xneur_config* xneur_config_init(void);

#endif /* _CONFIG_MAIN_H_ */
