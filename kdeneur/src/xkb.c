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
 *  Copyright (C) 2006-2010 XNeur Team
 *
 */
 

#include "xkb.h"
#include <string.h>

int get_active_kbd_group(Display *dpy)
{
	if (dpy == NULL)
		return -1;
	XkbStateRec xkbState;
	XkbGetState(dpy, XkbUseCoreKbd, &xkbState);

	return xkbState.group;
}

int get_kbd_group_count(Display *dpy) 
{
	if (dpy == NULL)
		return -1;

    XkbDescRec desc[1];
    int gc;
    memset(desc, 0, sizeof(desc));
    desc->device_spec = XkbUseCoreKbd;
    XkbGetControls(dpy, XkbGroupsWrapMask, desc);
    XkbGetNames(dpy, XkbGroupNamesMask, desc);
	gc = desc->ctrls->num_groups;
    XkbFreeControls(desc, XkbGroupsWrapMask, True);
    XkbFreeNames(desc, XkbGroupNamesMask, True);

    return gc;
}

int set_next_kbd_group(Display *dpy)
{
	if (dpy == NULL)
		return -1;

	int active_layout_group = get_active_kbd_group(dpy);
	
	int new_layout_group = active_layout_group + 1;
	if (new_layout_group == get_kbd_group_count(dpy))
		new_layout_group = 0;

	XkbLockGroup(dpy, XkbUseCoreKbd, new_layout_group);

	return 1;
}
