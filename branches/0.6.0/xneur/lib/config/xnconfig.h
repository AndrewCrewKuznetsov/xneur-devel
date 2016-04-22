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

#define CONFIG_NAME			"xneurrc"

#define DICT_NAME			"dict"
#define PROTO_NAME			"proto"
#define BIG_PROTO_NAME			"proto3"
#define LANGDEF_NAME			"langdef"
#define REGEXP_NAME			"regexp"

#define AUTO_MODE			0
#define MANUAL_MODE			1

#define MOUSE_GRAB_DISABLE		0
#define MOUSE_GRAB_ENABLE		1

#define EDUCATION_MODE_DISABLE		0
#define EDUCATION_MODE_ENABLE		1

#define LAYOUTE_REMEMBER_DISABLE	0
#define LAYOUTE_REMEMBER_ENABLE		1

#define SAVESELECTION_DISABLE		0
#define SAVESELECTION_ENABLE		1

#define NO_LANGUAGE			-1

#define MAX_HOTKEYS			6

enum _error
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
	ERROR_LANGDEF_NOT_FOUND,
	ERROR_INVALID_MOUSE_MODE,
	ERROR_INVALID_EDUCATION_MODE,
	ERROR_INVALID_CONFIG_VERSION,
	ERROR_INVALID_LAYOUTE_REMEMBER_MODE,
	ERROR_INVALID_SAVESELECTION_MODE,
	ERROR_CANT_CREATE_FILE,
};

enum _hotkey_action
{
	ACTION_CHANGE_WORD,
	ACTION_CHANGE_STRING,
	ACTION_CHANGE_MODE,
	ACTION_CHANGE_SELECTED,
	ACTION_TRANSLIT_SELECTED,
	ACTION_CHANGECASE_SELECTED,
	ACTION_NONE,
};

enum _hotkey_modifier
{
	MODIFIER_NONE,
	MODIFIER_SHIFT,
	MODIFIER_CONTROL,
	MODIFIER_ALT,
};

enum _hotkey_key
{
	KEY_NONE,
	KEY_BREAK,
	KEY_SCROLL_LOCK,
	KEY_PRINT_SCREEN,
};

struct _xneur_language
{
	char *dir;
	char *name;

	char *vowel_letter;
	char *consonant_letter;
	char *nofirst_letter;

	struct _list *temp_dicts;
	struct _list *dicts;
	struct _list *protos;
	struct _list *big_protos;
	struct _list *regexp;
};

struct _xneur_hotkey
{
	enum _hotkey_modifier modifier;
	enum _hotkey_key key;
};

const char* format_error_message(enum _error error_code);

struct _xneur_data
{
	int xneur_pid;
	int xneur_mode;
};

struct _xneur_config
{
	struct _list *excluded_apps;
	struct _list *auto_apps;
	struct _list *manual_apps;

	struct _list *window_layouts;

	struct _xneur_data *xneur_data;
	struct _xneur_language *languages;		// Array of languages used in program
	struct _xneur_hotkey hotkeys[MAX_HOTKEYS];	// Array of hotkeys used in program

	int   total_languages;				// Total languages to work with
	int   log_level;				// Maximum level of log messages to print
	int   *xkb_groups;				// XKB Group for each language
	
	int   mouse_processing_mode;			// Grab or not mouse
	int   education_mode;				// Education xneur
	int   layout_remember_mode;			// Remember layout for each of window
	int   save_selection_mode;			// Save selection after convert

	enum _error (*load) (struct _xneur_config *p);
	enum _error (*save) (struct _xneur_config *p);
	enum _error (*save_dicts) (struct _xneur_config *p);
	void  (*set_pid) (struct _xneur_config *p, int pid);
	int   (*get_pid) (struct _xneur_config *p);
	void  (*set_current_mode) (struct _xneur_config *p, int mode);
	int   (*get_current_mode) (struct _xneur_config *p);
	char* (*get_lang_dir) (struct _xneur_config *p, int lang);
	char* (*get_file_path_name) (const char *dir_name, const char *file_name);
	char* (*get_lang_name) (struct _xneur_config *p, int lang);
	int   (*find_group_lang) (struct _xneur_config *p, int group);
	int   (*get_lang_group) (struct _xneur_config *p, int lang);
	void  (*set_languages) (struct _xneur_config *p, char **lang_name, char **lang_dir, int *lang_group, int total_lang);
	void  (*uninit) (struct _xneur_config *p);
};

struct _xneur_config* xneur_config_init(void);

#endif /* _CONFIG_MAIN_H_ */
