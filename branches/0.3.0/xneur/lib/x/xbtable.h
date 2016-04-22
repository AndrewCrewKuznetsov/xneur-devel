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

#ifndef _XBTABLE_H_
#define _XBTABLE_H_

#include <X11/Xutil.h>

void bind_manual_actions(void);

enum _hotkey_action get_manual_action(KeySym key_sym, KeySym key_modifier);

typedef struct _xbtable
{
	KeySym key_sym;
	KeySym key_sym_alt;
	KeySym key_modifier;
} xbtable;

#endif /* _XBTABLE_H_ */
