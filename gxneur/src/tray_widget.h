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

#ifndef __GTK_TRAY_ICON_H__
#define __GTK_TRAY_ICON_H__

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gtk/gtkplug.h>
#pragma GCC diagnostic pop

#include <X11/Xatom.h>
#include <gdk/gdkx.h>

G_BEGIN_DECLS

#define GTK_TYPE_TRAY_ICON		(gtk_tray_icon_get_type ())
#define GTK_TRAY_ICON(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_TRAY_ICON, GtkTrayIcon))
#define GTK_TRAY_ICON_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_TRAY_ICON, GtkTrayIconClass))
#define GTK_IS_TRAY_ICON(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_TRAY_ICON))
#define GTK_IS_TRAY_ICON_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_TRAY_ICON))
#define GTK_TRAY_ICON_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_TRAY_ICON, GtkTrayIconClass))

// https://sigquit.wordpress.com/2009/02/13/avoid-g_type_instance_get_private-in-gobjects/
#define GTK_TRAY_ICON_GET_PRIVATE(obj) ((GtkTrayIconPrivate *)((GTK_TRAY_ICON(obj))->priv))

typedef struct _GtkTrayIcon	   GtkTrayIcon;
typedef struct _GtkTrayIconPrivate GtkTrayIconPrivate;
typedef struct _GtkTrayIconClass   GtkTrayIconClass;

struct _GtkTrayIcon
    {
    /** Parent object */
	GtkPlug parent_instance;
    /** Private data pointer */
	GtkTrayIconPrivate *priv;
    };

// перенесена из .c в header
struct _GtkTrayIconPrivate
    {
	guint stamp;

	Atom selection_atom;
	Atom manager_atom;
	Atom system_tray_opcode_atom;
	Atom orientation_atom;
	Atom visual_atom;
	Window manager_window;
	GdkVisual *manager_visual;
	gboolean manager_visual_rgba;

	GtkOrientation orientation;

    /** Parent object */
	GObject parent_instance;
    /** Private data pointer */
	gpointer priv;
    };

struct _GtkTrayIconClass
    {
	GtkPlugClass parent_class;

	void (*__gtk_reserved1);
	void (*__gtk_reserved2);
	void (*__gtk_reserved3);
	void (*__gtk_reserved4);
	void (*__gtk_reserved5);
	void (*__gtk_reserved6);
    };

GType gtk_tray_icon_get_type (void) G_GNUC_CONST;

GtkTrayIcon* _gtk_tray_icon_new_for_screen(GdkScreen *screen, const gchar *name);

GtkTrayIcon* _gtk_tray_icon_new(const gchar *name);

guint _gtk_tray_icon_send_message(GtkTrayIcon *icon, gint timeout, const gchar *message, gint len);
void _gtk_tray_icon_cancel_message(GtkTrayIcon *icon, guint id);

GtkOrientation _gtk_tray_icon_get_orientation (GtkTrayIcon *icon);

G_END_DECLS

#endif /* __GTK_TRAY_ICON_H__ */
