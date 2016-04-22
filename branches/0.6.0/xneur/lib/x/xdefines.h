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

#ifndef _XDEFINES_H_
#define _XDEFINES_H_

#define INPUT_HANDLE_MASK	KeyReleaseMask  | PropertyChangeMask
#define FOCUS_CHANGE_MASK	LeaveWindowMask | EnterWindowMask | FocusChangeMask
#define MAIN_WINDOW_MASK	StructureNotifyMask
#define BUTTON_HANDLE_MASK	Button1MotionMask | ButtonPressMask | ButtonReleaseMask

#endif /* _XDEFINIES_H_ */
