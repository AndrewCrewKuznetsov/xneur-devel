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

#include <X11/XKBlib.h>

#include <stdlib.h>
#include <string.h>

#include "xnconfig.h"

#include "xwindow.h"

#include "utils.h"
#include "log.h"
#include "types.h"

#include "xswitchlang.h"

extern struct _xneur_config *xconfig;
extern struct _xwindow *main_window;

int get_active_keyboard_group(void)
{
	XkbStateRec xkbState;
	XkbGetState(main_window->display, XkbUseCoreKbd, &xkbState);
	return xkbState.group;
}

int get_cur_lang(void)
{
	int group = get_active_keyboard_group();

	int lang = xconfig->find_group_lang(xconfig, group);
	if (lang != NO_LANGUAGE)
		return lang;

	log_message(ERROR, "Can't find language for this XKB Group");
	return 0;
}

void switch_lang(int new_lang)
{
	XkbLockGroup(main_window->display, XkbUseCoreKbd, xconfig->xkb_groups[new_lang]);
}

int check_keyboard_groups(void)
{
	XkbDescRec *kbd_desc_ptr = XkbAllocKeyboard();
	if (kbd_desc_ptr == NULL)
	{
		log_message(ERROR, "Failed to get keyboard descriptor");
		return FALSE;
	}

	Display *display = main_window->display;

	XkbGetControls(display, XkbAllControlsMask, kbd_desc_ptr);
	XkbGetNames(display, XkbSymbolsNameMask, kbd_desc_ptr);
	XkbGetNames(display, XkbGroupNamesMask, kbd_desc_ptr);

	if (kbd_desc_ptr->names == NULL)
	{
		log_message(ERROR, "Failed to get keyboard group names");
		return FALSE;
	}

	const Atom *group_source = kbd_desc_ptr->names->groups;
	int groups_count = 0;

	// And more bug patches
	if (kbd_desc_ptr->ctrls != NULL)
		groups_count = kbd_desc_ptr->ctrls->num_groups;
	else
	{
		for (; groups_count < XkbNumKbdGroups; groups_count++)
		{
			if (group_source[groups_count] == None)
				break;
		}
	}

	if (groups_count == 0)
	{
		log_message(ERROR, "No keyboard layout found");
		return FALSE;
	}

	log_message(LOG, "Keyboard layouts present in system:");

	int group, valid_count = 0;
	for (group = 0; group < groups_count; group++)
	{
		char *name = XGetAtomName(display, kbd_desc_ptr->names->groups[group]);

		int lang = xconfig->find_group_lang(xconfig, group);
		char *lang_name = xconfig->get_lang_name(xconfig, lang);

		if (lang_name == NULL)
		{
			log_message(ERROR, "   XKB Group '%s' not defined in configuration file (group %d)", name, group);
			continue;
		}

		log_message(LOG, "   XKB Group '%s' must be for '%s' language (group %d)", name, lang_name, group);
		valid_count++;
	}

	log_message(LOG, "Total %d of %d valid keyboard layouts detected", valid_count, groups_count);
	return TRUE;
}

int check_xkb_extension(void)
{
	if (!XkbQueryExtension(main_window->display, NULL, NULL, NULL, NULL, NULL))
	{
		log_message(ERROR, "Can't find XKB extension");
		return FALSE;
	}
	return TRUE;
}
