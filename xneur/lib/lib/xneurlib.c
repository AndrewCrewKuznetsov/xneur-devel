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
 *  Copyright (C) 2006-2016 XNeur Team
 *
 */


#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef WITH_ASPELL
#	include <aspell.h>
#endif

#ifdef WITH_ENCHANT
#	include <enchant/enchant.h>
#endif

#include "xneur.h"
#include "window.h"

#include "xnconfig_files.h"

#include "list_char.h"
#include "types.h"

#include "switchlang.h"

#include "detection.h"

#include "log.h"

#include "buffer.h"

extern struct _window *main_window;

struct _xneur_config *xconfig				= NULL;

/*static int get_group(Display *dpy) {
	XkbStateRec state[1];
	memset(state, 0, sizeof(state));
	XkbGetState(dpy, XkbUseCoreKbd, state);
	return state->group;
}

static int get_layout(Display *dpy, char **names) {
	XkbDescRec desc[1];
	int gc;
	memset(desc, 0, sizeof(desc));
	desc->device_spec = XkbUseCoreKbd;
	XkbGetControls(dpy, XkbGroupsWrapMask, desc);
	XkbGetNames(dpy, XkbGroupNamesMask, desc);
	XGetAtomNames(dpy, desc->names->groups, gc = desc->ctrls->num_groups, names);
	XkbFreeControls(desc, XkbGroupsWrapMask, True);
	XkbFreeNames(desc, XkbGroupNamesMask, True);
	return gc;
}

static void free_layout(char **names, int gc) {
	for (; gc--; ++names)
		if (*names) {
			XFree(*names);
			*names = NULL;
		}
}*/

#if  defined(WITH_ASPELL) || defined(WITH_ENCHANT)
static char *layout_names[] =
{
	"am","bg","by","cz","de","gr","ee","en","es","fr","ge","gb","kz","lt","lv",
	"pl","ro","ru","ua","us","uz"
};

static char *spell_names[] =
{
	"hy","bg","be","cs","de","el","et","en","es","fr","ka","en","kk","lt","lv",
	"pl","ro","ru","uk","en","uz"
};

static const int names_len = sizeof(layout_names) / sizeof(layout_names[0]);
#endif

static long get_next_property_value (unsigned char **pointer, long *length, int size, char **string)
{
    if (size != 8)
		return 0;

    int len = 0; *string = (char *)*pointer;
    while ((len++, --*length, *((*pointer)++)) && *length>0);

    return len;
}

struct _xneur_handle *xneur_handle_create (void)
{
	struct _xneur_handle *handle = (struct _xneur_handle *) malloc(sizeof(struct _xneur_handle));;
	if (handle == NULL)
		return NULL;
	
	XkbDescPtr kbd_desc_ptr;

	if (!(kbd_desc_ptr = XkbAllocKeyboard()))
	{
		free(handle);
		return NULL;
	}
	
	Display *display = XOpenDisplay(NULL);

	/*char *names[XkbNumKbdGroups];
	int gc = get_layout(display, names);
	int g = get_group(display);
	for (int i = 0; i < gc; i++)
	{
		printf("%d) %c %s\n", i, i == g ? '*' : ' ', names[i]);
	}
	free_layout(names, gc);*/
	
	XkbGetNames(display, XkbAllNamesMask, kbd_desc_ptr);

	if (kbd_desc_ptr->names == NULL)
	{
		XCloseDisplay(display);
		XkbFreeKeyboard(kbd_desc_ptr, XkbAllComponentsMask, True);
		free(handle);
		return NULL;
	}

	int groups_count = 0;
	for (; groups_count < XkbNumKbdGroups; groups_count++)
	{
		if (kbd_desc_ptr->names->groups[groups_count] == None)
			break;
	}

	if (groups_count == 0)
	{
		XCloseDisplay(display);
		XkbFreeKeyboard(kbd_desc_ptr, XkbAllComponentsMask, True);
		free(handle);
		return NULL;
	}

	Atom _XKB_RULES_NAMES = XInternAtom(display, "_XKB_RULES_NAMES", 1);
	if (_XKB_RULES_NAMES == None) 
	{
		XCloseDisplay(display);
		XkbFreeKeyboard(kbd_desc_ptr, XkbAllComponentsMask, True);
		free(handle);
		return NULL;
	}
	Window rw = RootWindow(display, DefaultScreen(display));
	Atom type;
    int size;
    unsigned long nitems;
    unsigned long nbytes;
    unsigned long bytes_after;
    unsigned char *prop;
    int status;
	
    status = XGetWindowProperty(display, rw, _XKB_RULES_NAMES, 0, (10000+3)/4,
				False, AnyPropertyType, &type,
				&size, &nitems, &bytes_after,
				&prop);
	if (status != Success)
	{
		XCloseDisplay(display);
		XkbFreeKeyboard(kbd_desc_ptr, XkbAllComponentsMask, True);
		free(handle);
		return NULL;
	}
	
	if (size == 32)
		nbytes = sizeof(long);
	else if (size == 16)
		nbytes = sizeof(short);
	else if (size == 8)
		nbytes = 1;
	else if (size == 0)
		nbytes = 0;
	else
	{
		XCloseDisplay(display);
		XkbFreeKeyboard(kbd_desc_ptr, XkbAllComponentsMask, True);
		free(handle);
		return NULL;
	}
	
	int prop_count = 0;
	char *prop_value = NULL;
    long length = nitems * nbytes;
	while (length >= size/8) 
	{
		int prop_value_len = get_next_property_value(&prop, &length, size, &prop_value);
		if (prop_value_len == 0)
		{
			XCloseDisplay(display);
			XkbFreeKeyboard(kbd_desc_ptr, XkbAllComponentsMask, True);
			free(handle);
			return NULL;
		}

		prop_count++;
		// 1 - Keyboard Driver
		// 2 - Keyboard Model
		// 3 - Keyboard Layouts
		// 4 - Keyboard Variants
		// 5 - Keyboard Led and Grp
		if (prop_count == 3)
			break;
	}
	if (prop_count != 3)
	{
		XCloseDisplay(display);
		XkbFreeKeyboard(kbd_desc_ptr, XkbAllComponentsMask, True);
		free(handle);
		return NULL;
	}
	//log_message(ERROR, "%s", 
	handle->languages = (struct _xneur_language *) malloc(sizeof(struct _xneur_language));
	handle->total_languages = 0;

	for (int group = 0; group < groups_count; group++)
	{
		Atom group_atom = kbd_desc_ptr->names->groups[group];
			
		if (group_atom == None)
			continue;

		char *group_name = XGetAtomName(display, group_atom);
		//log_message (ERROR, "%s", group_name);
		char *short_name = strsep(&prop_value, ",");
		//log_message (ERROR, "%s", short_name);

		// Check double layout
		//
		for (int lang = 0; lang < handle->total_languages; lang++)
	    {
			if (strcmp(handle->languages[lang].dir, short_name) == 0)
			{
				goto function_end;
			}
		}

		void *tmp = realloc(handle->languages, (handle->total_languages + 1) * sizeof(struct _xneur_language));
		if (tmp == NULL)
			continue;
		handle->languages = (struct _xneur_language *) tmp;
		bzero(&(handle->languages[handle->total_languages]), sizeof(struct _xneur_language));

		handle->languages[handle->total_languages].name	= strdup(group_name);
		handle->languages[handle->total_languages].dir	= strdup(short_name);
		tmp = realloc (handle->languages[handle->total_languages].dir, sizeof(char) * 3); // 'xx' + '\0'
		if (tmp == NULL)
			continue;
		handle->languages[handle->total_languages].dir	= (char *)tmp;
		handle->languages[handle->total_languages].dir[2] = NULLSYM;
		handle->languages[handle->total_languages].group	= group;
		handle->languages[handle->total_languages].excluded	= FALSE;
		handle->languages[handle->total_languages].disable_auto_detection	= FALSE;
		handle->total_languages++;

		//if (group_name != NULL)
			//free(group_name);
		
		if (prop_value == NULL)
			break;
		function_end:;
	}

	XCloseDisplay(display);
	XkbFreeKeyboard(kbd_desc_ptr, XkbAllComponentsMask, True);
	
	if (handle->total_languages == 0)
	{
		free(handle);
		return NULL;
	}

#ifdef WITH_ASPELL
	// init aspell spellers
	handle->spell_checkers = (AspellSpeller **) malloc(handle->total_languages * sizeof(AspellSpeller*));
	handle->has_spell_checker = (int *) malloc(handle->total_languages * sizeof(int));
	handle->spell_config = new_aspell_config();
#endif

#ifdef WITH_ENCHANT
	// init enchant brocker and dicts
	handle->enchant_dicts = (EnchantDict **) malloc(handle->total_languages * sizeof(EnchantDict *));
	handle->has_enchant_checker = (int *) malloc(handle->total_languages * sizeof(int));
	handle->enchant_broker = enchant_broker_init ();
#endif

	for (int lang = 0; lang < handle->total_languages; lang++)
	{
		int path_len = strlen(LANGUAGEDIR) + strlen(handle->languages[lang].dir) + 2;
		char *lang_dir = (char *) malloc(path_len * sizeof(char));
		snprintf(lang_dir, path_len, "%s/%s", LANGUAGEDIR, handle->languages[lang].dir);

		handle->languages[lang].dictionary = load_list(lang_dir, DICT_NAME, TRUE);		
		if (handle->languages[lang].dictionary == NULL)
		{
			handle->languages[lang].dictionary->data_count = 0;
		}
		else	
		{
			handle->languages[lang].dictionary->rem(handle->languages[lang].dictionary, "(?i)^.$");
		}
		
		handle->languages[lang].proto = load_list(lang_dir, PROTO_NAME, TRUE);
		if (handle->languages[lang].proto == NULL)
			handle->languages[lang].proto->data_count = 0;

		handle->languages[lang].big_proto = load_list(lang_dir, BIG_PROTO_NAME, TRUE);
		if (handle->languages[lang].big_proto == NULL)
			handle->languages[lang].big_proto->data_count = 0;

		handle->languages[lang].pattern = load_list(lang_dir, PATTERN_NAME, TRUE);
		if (handle->languages[lang].pattern == NULL)
			handle->languages[lang].pattern->data_count = 0;
		
		handle->languages[lang].temp_dictionary = handle->languages[lang].dictionary->clone(handle->languages[lang].dictionary);

		if (lang_dir != NULL)
			free(lang_dir);
	}

#ifdef WITH_ASPELL
	for (int lang = 0; lang < handle->total_languages; lang++)
	{
		// initialize aspell checker for current language
		int i = 0;
		for (i = 0; i < names_len; i++)
		{
			if (strcmp(layout_names[i], handle->languages[lang].dir) == 0)
				break;

		}
		if (i != names_len)
		{
			aspell_config_replace(handle->spell_config, "lang", spell_names[i]);
			AspellCanHaveError *possible_err = new_aspell_speller(handle->spell_config);

			int aspell_error = aspell_error_number(possible_err);

			if (aspell_error != 0)
			{
				//printf("   [!] Error initialize %s aspell dictionary\n", handle->languages[lang].name);
				delete_aspell_can_have_error(possible_err);
				handle->has_spell_checker[lang] = 0;
			}
			else
			{
				//printf("   [!] Initialize %s aspell dictionary\n", handle->languages[lang].name);
				handle->spell_checkers[lang] = to_aspell_speller(possible_err);
				handle->has_spell_checker[lang] = 1;
			}
		}
		else
		{
			//printf("   [!] Now we don't support aspell dictionary for %s layout\n", handle->languages[lang].dir);
			handle->has_spell_checker[lang] = 0;
		}
	}
#endif

#ifdef WITH_ENCHANT
	for (int lang = 0; lang < handle->total_languages; lang++)
	{
		// initialize enchant checker for current language
		int j = 0;
		for (j = 0; j < names_len; j++)
		{
			if (strcmp(layout_names[j], handle->languages[lang].dir) == 0)
				break;
		}
		if (j != names_len)
		{
			handle->enchant_dicts[lang] = NULL;
			char *dict_name = malloc (2 * strlen(spell_names[j]) + 2);
			if (dict_name == NULL)
				continue;
			dict_name[0] = NULLSYM;
			strcat(dict_name, spell_names[j]);
			strcat(dict_name, "_");
			strcat(dict_name, handle->languages[lang].dir);
			dict_name[3] = toupper(dict_name[3]);
			dict_name[4] = toupper(dict_name[4]);
			//printf("   [!] Try load dict %s\n", dict_name);
			if (enchant_broker_dict_exists(handle->enchant_broker, dict_name) == FALSE)
			{
				dict_name[2] = NULLSYM;
				//printf("   [!] Try load dict %s\n", dict_name);
				if (enchant_broker_dict_exists(handle->enchant_broker, dict_name) == FALSE)
				{
					handle->has_enchant_checker[lang] = 0;
					free(dict_name);
					continue;
				}
			}

			//printf("   [!] Loaded dict %s\n", dict_name);
			handle->enchant_dicts[lang] = enchant_broker_request_dict (handle->enchant_broker, dict_name);
			handle->has_enchant_checker[lang] = 1;

			free(dict_name);
		}
		else
		{
			handle->has_enchant_checker[lang] = 0;
		}
	}
#endif

	for (int lang = 0; lang < handle->total_languages; lang++)
	{
		handle->languages[lang].disable_auto_detection |=
			handle->languages[lang].excluded || (
				handle->languages[lang].dictionary->data_count == 0 &&
				handle->languages[lang].proto->data_count == 0 &&
				handle->languages[lang].big_proto->data_count == 0
#ifdef WITH_ASPELL
				&& handle->has_spell_checker[lang] == 0
#endif
#ifdef WITH_ENCHANT
				&& handle->has_enchant_checker[lang] == 0
#endif
			);
	}

	return handle;	
}

void xneur_handle_destroy (struct _xneur_handle *handle)
{
	if (handle == NULL) 
		return;
	
	for (int lang = 0; lang < handle->total_languages; lang++)
	{
#ifdef WITH_ASPELL
		if (handle->has_spell_checker[lang])
			delete_aspell_speller(handle->spell_checkers[lang]);
#endif
		
#ifdef WITH_ENCHANT
		if ((handle->enchant_dicts[lang] != NULL) && (handle->has_enchant_checker[lang]))
			enchant_broker_free_dict (handle->enchant_broker, handle->enchant_dicts[lang]);
#endif

		if (handle->languages == NULL)
			continue;
		
		if (handle->languages[lang].temp_dictionary != NULL)
			handle->languages[lang].temp_dictionary->uninit(handle->languages[lang].temp_dictionary);

		if (handle->languages[lang].dictionary != NULL)
			handle->languages[lang].dictionary->uninit(handle->languages[lang].dictionary);

		if (handle->languages[lang].proto != NULL)
			handle->languages[lang].proto->uninit(handle->languages[lang].proto);

		if (handle->languages[lang].big_proto != NULL)
			handle->languages[lang].big_proto->uninit(handle->languages[lang].big_proto);

		if (handle->languages[lang].pattern != NULL)
			handle->languages[lang].pattern->uninit(handle->languages[lang].pattern);

		if (handle->languages[lang].name != NULL)
			free(handle->languages[lang].name);
		if (handle->languages[lang].dir != NULL)
			free(handle->languages[lang].dir);
	}
	handle->total_languages = 0;
	free(handle->languages);
		
#ifdef WITH_ASPELL
	delete_aspell_config(handle->spell_config);
	if (handle->spell_checkers != NULL)
		free(handle->spell_checkers);
	if (handle->has_spell_checker != NULL)
		free(handle->has_spell_checker);
#endif
	
#ifdef WITH_ENCHANT
	enchant_broker_free (handle->enchant_broker);
	if (handle->enchant_dicts != NULL)
		free(handle->enchant_dicts);
	if (handle->has_enchant_checker != NULL)
		free(handle->has_enchant_checker);
#endif
	
	free(handle);

}

int xneur_get_layout (struct _xneur_handle *handle, char *word)
{
	if (!word || handle == NULL)
		return -1;
	struct _buffer *buffer = buffer_init(handle, main_window->keymap);

	buffer->set_content(buffer, word);
	int cur_lang = get_curr_keyboard_group();
	int new_lang = check_lang(handle, buffer, cur_lang);

	buffer->uninit(buffer);

	// The word is suitable for all languages, return -1
	if (new_lang == NO_LANGUAGE)
		new_lang = -1;

	return new_lang;
}

char *xneur_get_word (struct _xneur_handle *handle, char *word)
{
	if (!word || handle == NULL)
		return NULL;

	char *result = NULL;

	struct _buffer *buffer = buffer_init(handle, main_window->keymap);

	buffer->set_content(buffer, word);
	int cur_lang = get_curr_keyboard_group();
	int new_lang = check_lang(handle, buffer, cur_lang);
	if (new_lang == NO_LANGUAGE)
		result = strdup(word);
	else
		buffer->set_lang_mask(buffer, new_lang),
		result = buffer->get_utf_string(buffer);

	buffer->uninit(buffer);

	return result;
}


