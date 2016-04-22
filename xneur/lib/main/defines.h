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
 *  Copyright (C) 2006-2012 XNeur Team
 *
 */

#ifndef _DEFINES_H_
#define _DEFINES_H_

#define INPUT_HANDLE_MASK	PropertyChangeMask
#define EVENT_KEY_MASK		KeyPressMask|KeyReleaseMask
#define EVENT_PRESS_MASK	KeyPressMask
#define EVENT_RELEASE_MASK	KeyReleaseMask
#define FOCUS_CHANGE_MASK   FocusChangeMask|LeaveWindowMask|EnterWindowMask
#define BUTTON_HANDLE_MASK	ButtonPressMask|ButtonReleaseMask

static const int groups[4] = {0x00000000, 0x00002000, 0x00004000, 0x00006000};

#endif /* _DEFINIES_H_ */
