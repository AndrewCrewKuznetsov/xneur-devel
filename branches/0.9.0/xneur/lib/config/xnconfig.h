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
 *  Copyright (C) 2006-2008 XNeur Team
 *
 */

#ifndef _CONFIG_MAIN_H_
#define _CONFIG_MAIN_H_

#define CONFIG_NAME			"xneurrc"
#define CONFIG_BCK_NAME			"xneurrc~"
#define LOG_NAME			"xneurlog"

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

#define SELECTION_SAVE_DISABLED		0
#define SELECTION_SAVE_ENABLED		1

#define SOUND_DISABLED			0
#define SOUND_ENABLED			1

#define LOG_DISABLED			0
#define LOG_ENABLED				1

#define DRAW_FLAG_DISABLED			0
#define DRAW_FLAG_ENABLED			1

#define NO_LANGUAGE			-1

#define SOUNDDIR			"sounds"
#define PIXMAPDIR			"pixmaps"

#define MAX_SOUNDS			14
#define MAX_HOTKEYS			10
#define MAX_FLAGS			4

enum _flag_action
{
	FLAG_LAYOUT_0 = 0,
	FLAG_LAYOUT_1,
	FLAG_LAYOUT_2,
	FLAG_LAYOUT_3,
};

enum _sound_action
{
	SOUND_PRESS_KEY_LAYOUT_0 = 0,
	SOUND_PRESS_KEY_LAYOUT_1,
	SOUND_PRESS_KEY_LAYOUT_2,
	SOUND_PRESS_KEY_LAYOUT_3,
	SOUND_ENABLE_LAYOUT_0,
	SOUND_ENABLE_LAYOUT_1,
	SOUND_ENABLE_LAYOUT_2,
	SOUND_ENABLE_LAYOUT_3,
	SOUND_AUTOMATIC_CHANGE_WORD,
	SOUND_MANUAL_CHANGE_WORD,
	SOUND_CHANGE_STRING,
	SOUND_CHANGE_SELECTED,
	SOUND_TRANSLIT_SELECTED,
	SOUND_CHANGECASE_SELECTED,
	SOUND_NONE,
};

enum _hotkey_action
{
	ACTION_CHANGE_WORD = 0,
	ACTION_CHANGE_STRING,
	ACTION_CHANGE_MODE,
	ACTION_CHANGE_SELECTED,
	ACTION_TRANSLIT_SELECTED,
	ACTION_CHANGECASE_SELECTED,
	ACTION_ENABLE_LAYOUT_0,
	ACTION_ENABLE_LAYOUT_1,
	ACTION_ENABLE_LAYOUT_2,
	ACTION_ENABLE_LAYOUT_3,
	ACTION_NONE,
};

enum _hotkey_modifier
{
	MODIFIER_NONE = 0,
	MODIFIER_SHIFT,
	MODIFIER_CONTROL,
	MODIFIER_ALT,
};

enum _hotkey_key
{
	KEY_NONE = 0,
	KEY_BREAK,
	KEY_SCROLL_LOCK,
	KEY_PRINT_SCREEN,
};

struct _xneur_language
{
	char *dir;
	char *name;
	int  group;

	char *vowel_letter;
	char *consonant_letter;
	char *nofirst_letter;

	struct _list_char *temp_dicts;
	struct _list_char *dicts;
	struct _list_char *protos;
	struct _list_char *big_protos;
	struct _list_char *regexp;
};

struct _xneur_hotkey
{
	int modifiers; // Shift (0x1), Control (0x2), Alt (0x4), Super (0x8)
	char *key;
};

struct _xneur_file
{
	char *file;
};

struct _xneur_data
{
	int xneur_pid;
	int xneur_mode;
};

struct _xneur_config
{
	char *version;
	
	void (*get_library_api_version) (int *major_version, int *minor_version); // This function MUST be first

	struct _list_char *excluded_apps;
	struct _list_char *auto_apps;
	struct _list_char *manual_apps;
	struct _list_char *layout_remember_apps;
	struct _list_char *window_layouts;

	struct _xneur_data *xneur_data;
	struct _xneur_language *languages;		// Array of languages used in program
	struct _xneur_hotkey *hotkeys;			// Array of hotkeys used in program
	struct _xneur_file *sounds;			// Array of sounds for actions

	int   log_level;				// Maximum level of log messages to print
	int   send_delay;				// Delay before send event (in milliseconds)
	int   total_languages;				// Total languages to work with

	int   default_group;				// Initial keyboard layout for all new applications

	int   sound_mode;				// Play sound samples or not 
	int   mouse_processing_mode;			// Grab mouse or not
	int   education_mode;				// Education xneur
	int   layout_remember_mode;			// Remember layout for each of window
	int   save_selection_mode;			// Save selection after convert
	int   save_log_mode;				// Save keyboard log
	 
	struct _list_char *draw_flag_apps;
	struct _xneur_file *flags;			// Array of flag pixmaps for draw
		   
	int   (*load) (struct _xneur_config *p);
	void  (*clear) (struct _xneur_config *p);
	int   (*save) (struct _xneur_config *p);
	int   (*replace) (struct _xneur_config *p);
	void  (*reload) (struct _xneur_config *p);
	int   (*kill) (struct _xneur_config *p);
	void  (*save_dicts) (struct _xneur_config *p, int lang);
	void  (*set_pid) (struct _xneur_config *p, int pid);
	int   (*get_pid) (struct _xneur_config *p);
	void  (*set_current_mode) (struct _xneur_config *p, int mode);
	int   (*get_current_mode) (struct _xneur_config *p);
	char* (*get_lang_dir) (struct _xneur_config *p, int lang);
	char* (*get_lang_name) (struct _xneur_config *p, int lang);
	int   (*get_lang_group) (struct _xneur_config *p, int lang);
	int   (*find_group_lang) (struct _xneur_config *p, int group);
	char* (*get_dict_path) (const char *dir_name, const char *file_name);
	char* (*get_home_dict_path) (const char *dir_name, const char *file_name);
	void  (*add_language) (struct _xneur_config *p, const char *name, const char *dir, int group);
	void  (*uninit) (struct _xneur_config *p);
};

struct _xneur_config* xneur_config_init(void);

#endif /* _CONFIG_MAIN_H_ */
