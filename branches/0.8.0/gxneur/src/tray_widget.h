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
 *  Based on egtrayicon's code (Copyright (C) 2002 Anders Carlsson <andersca@gnu.org>)
 *
 */

#ifndef _TRAY_WIDGET_H_
#define _TRAY_WIDGET_H_

#include <gdk/gdkx.h>

G_BEGIN_DECLS

enum _tray_widget_properties
{
	PROPERTY_ORIENTATION = 1
};

struct _tray_widget_icon
{
	GtkPlug parent_instance;

	guint stamp;

	Atom selection_atom;
	Atom manager_atom;
	Atom system_tray_opcode_atom;
	Atom orientation_atom;
	Window manager_window;

	GtkOrientation orientation;
};

struct _tray_widget
{
	GtkPlugClass parent_class;
};

struct _tray_widget* tray_widget_init(const gchar *name);

G_END_DECLS

#endif /* _TRAY_WIDGET_H_ */
