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

#ifndef _FOCUS_H_
#define _FOCUS_H_

#include <X11/XKBlib.h>

#define FORCE_MODE_NORMAL	0
#define FORCE_MODE_MANUAL	1
#define FORCE_MODE_AUTO		2

#define AUTOCOMPLETION_INCLUDED	0
#define AUTOCOMPLETION_EXCLUDED	1

struct _focus
{
	Window owner_window;		// Input focus window
	Window parent_window;		// Parent widget in window
	int last_excluded;			// Last focus status

	int  (*get_focus_status) (struct _focus *p, int *forced_mode, int *excluded, int *autocompletion_mode);
	int  (*is_focus_changed) (struct _focus *p);
	void (*update_grab_events) (struct _focus *p, int grab);
	void (*click_key) (struct _focus *p, KeySym keysym);
	void (*uninit) (struct _focus *p);
};

struct _focus* focus_init(void);

#endif /* _FOCUS_H_ */
