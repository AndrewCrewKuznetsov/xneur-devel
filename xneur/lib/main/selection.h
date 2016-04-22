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

#ifndef _SELECTION_H_
#define _SELECTION_H_

enum _selection_type
{
	SELECTION_PRIMARY,
	SELECTION_SECONDARY,
	SELECTION_CLIPBOARD,
};

/*char* get_selected_text(XSelectionEvent *event);
void  on_selection_converted(enum _selection_type sel_type);
void  do_selection_notify(enum _selection_type sel_type);*/

unsigned char *get_selection_text (enum _selection_type sel_type);

#endif /* _SELECTION_H_ */
