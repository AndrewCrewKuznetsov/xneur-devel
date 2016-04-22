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
 
#include <X11/XKBlib.h>

#include "xkb.h"

int get_active_keyboard_group(void)
{
	Display *dpy = XOpenDisplay(NULL);

	XkbStateRec xkbState;
	XkbGetState(dpy, XkbUseCoreKbd, &xkbState);
	XCloseDisplay(dpy);

	return xkbState.group;
}

int set_next_keyboard_group(void)
{
	Display *dpy = XOpenDisplay(NULL);

	int active_layout_group = get_active_keyboard_group();
	
	XkbDescRec *kbd_desc_ptr = XkbAllocKeyboard();
	if (kbd_desc_ptr == NULL)
		return 0;

	XkbGetControls(dpy, XkbAllControlsMask, kbd_desc_ptr);
	XkbGetNames(dpy, XkbSymbolsNameMask, kbd_desc_ptr);
	XkbGetNames(dpy, XkbGroupNamesMask, kbd_desc_ptr);	
	if (kbd_desc_ptr->names == NULL)
		return 0;

	const Atom *group_source = kbd_desc_ptr->names->groups;
	int groups_count = 0;

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
		return 0;

	int new_layout_group = active_layout_group + 1;
	if (new_layout_group == groups_count)
		new_layout_group = 0;

	XkbLockGroup(dpy, XkbUseCoreKbd, new_layout_group);
	XCloseDisplay(dpy);
	return 1;
}
