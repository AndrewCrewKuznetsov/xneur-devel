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

#ifndef _TRAYICON_H_
#define _TRAYICON_H_

struct _tray_icon
{
	struct _tray_widget *widget;

	GtkTooltips *tooltip;
	GtkWidget *image;
	GtkWidget *popup_menu;
	GtkWidget *evbox;
	GtkWidget *clock;
};

void create_tray_icon(struct _tray_icon *tray, gboolean runned);

#endif /* _TRAYICON_H_ */
