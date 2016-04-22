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

#include <X11/XKBlib.h>

#include <stdlib.h>
#include <string.h>

#include "xconfig.h"
#include "xdefines.h"

#include "utils.h"
#include "log.h"

#include "xswitchlang.h"

extern struct _xconf *xconfig;

int xswitchlang_get_cur_lang(struct _xswitchlang *p)
{
	int lang, xkb_group = p->get_active_keyboard(p);
	for (lang = 0; lang < xconfig->TotalLanguages; lang++)
	{
		if (xconfig->xkbGroup[lang] == xkb_group)
			return lang;
	}

	log_message(ERROR, "Can't find language for this xkb group");
	return ENGLISH;
}

void xswitchlang_print_keyboard_list(const xswitchlang *p)
{
	XkbDescRec *kbd_desc_ptr = XkbAllocKeyboard();
	if (kbd_desc_ptr == NULL)
	{
		log_message(ERROR, "Failed to get keyboard descriptor");
		return;
	}

	XkbGetControls(p->xDisplay, XkbAllControlsMask, kbd_desc_ptr);
	XkbGetNames(p->xDisplay, XkbSymbolsNameMask, kbd_desc_ptr);
	XkbGetNames(p->xDisplay, XkbGroupNamesMask, kbd_desc_ptr);
	
	if (kbd_desc_ptr->names == NULL)
	{
		log_message(ERROR, "Failed to get keyboard description");
		return;
	}
	
	const Atom *group_source = kbd_desc_ptr->names->groups;
	int group_count = 0;

	// And more bug patches
	if (kbd_desc_ptr->ctrls != NULL)
		group_count = kbd_desc_ptr->ctrls->num_groups;
	else
		while(group_count < XkbNumKbdGroups && group_source[group_count++] != None);

	if (group_count == 0)
	{
		log_message(ERROR, "No keyboard layout found");
		return;
	}
		  
	log_message(DEBUG, "Keyboard layout present in system (total %d):", group_count);

	int i;
	for (i = 0; i < group_count; i++)
	{
		char *name = XGetAtomName(p->xDisplay, kbd_desc_ptr->names->groups[i]);

		if (i == ENGLISH)
			log_message(DEBUG, "  '%s' - must be ENGLISH (group %d in configuration file) (if NOT, see configuration file)!", name, i);
		else if (i == RUSSIAN)
			log_message(DEBUG, "  '%s' - must be RUSSIAN (group %d in configuration file) (if NOT, see configuration file)!", name, i);
		else
			log_message(DEBUG, "  '%s' - (group is not defined in configuration file)!", name);
	}
}

void xswitchlang_set_active_keyboard(struct _xswitchlang *p, int group)
{
	XkbLockGroup(p->xDisplay, XkbUseCoreKbd, group);
}

int xswitchlang_get_active_keyboard(struct _xswitchlang *p)
{
	XkbStateRec xkbState;
	XkbGetState(p->xDisplay, XkbUseCoreKbd, &xkbState);
	return xkbState.group;
}

void xswitchlang_switch_lang(struct _xswitchlang *p, int new_lang)
{
	p->set_active_keyboard(p, xconfig->xkbGroup[new_lang]);
	p->cur_lang = new_lang;
}

void xswitchlang_uninit(struct _xswitchlang *p)
{
	free(p);
}

struct _xswitchlang* xswitchlang_init(Display *xDisplay)
{
	struct _xswitchlang *p = (struct _xswitchlang *) xnmalloc(sizeof(struct _xswitchlang));
	bzero(p, sizeof(struct _xswitchlang));

	p->xDisplay		= xDisplay;

	int err = XkbQueryExtension(xDisplay, NULL, NULL, NULL, NULL, NULL);
	if (!err)
		return NULL;

	// Functions mapping
	p->switch_lang		= xswitchlang_switch_lang;
	p->get_cur_lang		= xswitchlang_get_cur_lang;
	p->print_keyboard_list	= xswitchlang_print_keyboard_list;
	p->set_active_keyboard	= xswitchlang_set_active_keyboard;
	p->get_active_keyboard	= xswitchlang_get_active_keyboard;
	p->uninit		= xswitchlang_uninit;

	return p;
}
