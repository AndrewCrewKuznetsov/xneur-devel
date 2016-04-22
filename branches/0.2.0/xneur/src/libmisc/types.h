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

#ifndef _TYPES_H_
#define _TYPES_H_

// General types
typedef char bool;

#define TRUE 1
#define FALSE 0

#define NULLSYM '\0'

// Options
#define AUTO_BY_KEYLOG 0
#define MANUAL_ON_SHORTCUT 1

// Initialize value
#define WRONG_SIZE -1

// General messages
#define ERROR			-1
#define SUCCESS			0
#define WARNING			1
#define LOG			2
#define DEBUG			3

// Text constants
#define INIT_WORD_LENGTH	64
#define INIT_STRING_LENGTH	1024

// Keys
#define NONE_KEY		0
#define PAUSE_KEY		1
#define SCROLL_LOCK_KEY		2
#define SYS_RQ_KEY		3

// Modifiers
#define SHIFT_KEY		10
#define CONTROL_KEY		11

#define DEFAULT_BIND_TABLE_SIZE	5

#endif /*_TYPES_H_ */
