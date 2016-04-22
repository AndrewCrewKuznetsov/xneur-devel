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

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>

#include <strings.h>
#include <stdlib.h>

#include "tray_widget.h"

#define TRAY_WIDGET_ICON(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), widget_type, struct _tray_widget_icon))

static const char *type_name = "gXNeurTrayIcon";
static GtkPlugClass *parent_class = NULL;
static GType widget_type = 0;

static GdkFilterReturn tray_widget_filter(GdkXEvent *xevent, GdkEvent *event, gpointer user_data);

static void tray_widget_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	if (pspec){}

	struct _tray_widget_icon *widget_icon = TRAY_WIDGET_ICON(object);

	if (prop_id == PROPERTY_ORIENTATION)
		g_value_set_enum(value, widget_icon->orientation);
}

static void tray_widget_send_dock_request(struct _tray_widget_icon *widget_icon)
{
	XClientMessageEvent ev;
	ev.type = ClientMessage;
	ev.window =  widget_icon->manager_window;
	ev.message_type = widget_icon->system_tray_opcode_atom;
	ev.format = 32;
	ev.data.l[0] = gdk_x11_get_server_time(GTK_WIDGET(widget_icon)->window);
	ev.data.l[1] = 0;
	ev.data.l[2] = gtk_plug_get_id(GTK_PLUG(widget_icon));
	ev.data.l[3] = 0;
	ev.data.l[4] = 0;

	Display *display = GDK_DISPLAY_XDISPLAY(gtk_widget_get_display(GTK_WIDGET(widget_icon)));

	gdk_error_trap_push();

	XSendEvent(display, widget_icon->manager_window, False, NoEventMask, (XEvent *) &ev);
	XSync(display, False);

	gdk_error_trap_pop();
}

static void tray_widget_get_orientation_property(struct _tray_widget_icon *widget_icon)
{
	g_return_if_fail(widget_icon->manager_window != None);

	Display *display = GDK_DISPLAY_XDISPLAY(gtk_widget_get_display(GTK_WIDGET(widget_icon)));

	gdk_error_trap_push();

	union
	{
		gulong *prop;
		guchar *prop_ch;
	} prop = { NULL };

	Atom type = None;
	gulong nitems;
	gulong bytes_after;
	int format;

	int result = XGetWindowProperty(display, widget_icon->manager_window, widget_icon->orientation_atom, 0, G_MAXLONG, FALSE, XA_CARDINAL, &type, &format, &nitems, &bytes_after, &(prop.prop_ch));

	int error = gdk_error_trap_pop();

	if (error || result != Success)
		return;

	if (type == XA_CARDINAL)
	{
		GtkOrientation orientation = (prop.prop[0] == 0) ? GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL;

		if (widget_icon->orientation != orientation)
		{
			widget_icon->orientation = orientation;
			g_object_notify(G_OBJECT(widget_icon), "orientation");
		}
	}

	if (prop.prop)
		XFree(prop.prop);
}

static void tray_widget_update_manager_window(struct _tray_widget_icon *widget_icon, gboolean dock_if_realized)
{
	if (widget_icon->manager_window != None)
		return;

	Display *display = GDK_DISPLAY_XDISPLAY(gtk_widget_get_display(GTK_WIDGET(widget_icon)));

	XGrabServer(display);
  
	widget_icon->manager_window = XGetSelectionOwner(display, widget_icon->selection_atom);

	if (widget_icon->manager_window != None)
		XSelectInput(display, widget_icon->manager_window, StructureNotifyMask | PropertyChangeMask);

	XUngrabServer(display);
	XFlush(display);
  
	if (widget_icon->manager_window == None)
		return;

	GdkWindow *gdk_window = gdk_window_lookup_for_display(gtk_widget_get_display(GTK_WIDGET(widget_icon)), widget_icon->manager_window);

	gdk_window_add_filter(gdk_window, tray_widget_filter, widget_icon);

	if (dock_if_realized && GTK_WIDGET_REALIZED(widget_icon))
		tray_widget_send_dock_request(widget_icon);

	tray_widget_get_orientation_property(widget_icon);
}

static void tray_widget_manager_window_destroyed(struct _tray_widget_icon *widget_icon)
{
	g_return_if_fail(widget_icon->manager_window != None);

	GdkWindow *gdk_window = gdk_window_lookup_for_display(gtk_widget_get_display(GTK_WIDGET(widget_icon)), widget_icon->manager_window);

	gdk_window_remove_filter(gdk_window, tray_widget_filter, widget_icon);

	widget_icon->manager_window = None;

	tray_widget_update_manager_window(widget_icon, TRUE);
}

static GdkFilterReturn tray_widget_filter(GdkXEvent *xevent, GdkEvent *event, gpointer user_data)
{
	if (event){}

	struct _tray_widget_icon *widget_icon = user_data;
	XEvent *xev = (XEvent *) xevent;

	if (xev->xany.type == ClientMessage && xev->xclient.message_type == widget_icon->manager_atom && xev->xclient.data.l[1] == (int) widget_icon->selection_atom)
	{
		tray_widget_update_manager_window(widget_icon, TRUE);
		return GDK_FILTER_CONTINUE;
	}

	if (xev->xany.window != widget_icon->manager_window)
		return GDK_FILTER_CONTINUE;

	if (xev->xany.type == PropertyNotify && xev->xproperty.atom == widget_icon->orientation_atom)
		tray_widget_get_orientation_property(widget_icon);
	if (xev->xany.type == DestroyNotify)
		tray_widget_manager_window_destroyed(widget_icon);

	return GDK_FILTER_CONTINUE;
}

static gboolean tray_widget_transparent_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
	if (user_data){}

	gdk_window_clear_area(widget->window, event->area.x, event->area.y, event->area.width, event->area.height);

	return FALSE;
}

static void tray_widget_make_transparent_again(GtkWidget *widget, GtkStyle *previous_style, gpointer user_data)
{
	if (user_data || previous_style){}

	gdk_window_set_back_pixmap(widget->window, NULL, TRUE);
}

static void tray_widget_make_transparent(GtkWidget *widget, gpointer user_data)
{
	if (user_data){}

	if (GTK_WIDGET_NO_WINDOW(widget) || GTK_WIDGET_APP_PAINTABLE(widget))
		return;

	gtk_widget_set_app_paintable(widget, TRUE);
	gtk_widget_set_double_buffered(widget, FALSE);
	gdk_window_set_back_pixmap(widget->window, NULL, TRUE);

	g_signal_connect(widget, "expose_event", G_CALLBACK(tray_widget_transparent_expose_event), NULL);
	g_signal_connect_after(widget, "style_set", G_CALLBACK(tray_widget_make_transparent_again), NULL);
}

static void tray_widget_realize(GtkWidget *widget)
{
	struct _tray_widget_icon *widget_icon = TRAY_WIDGET_ICON(widget);

	if (GTK_WIDGET_CLASS(parent_class)->realize)
		GTK_WIDGET_CLASS(parent_class)->realize(widget);

	tray_widget_make_transparent(widget, NULL);

	int screen_number = gdk_screen_get_number(gtk_widget_get_screen(widget));
	Display *display = GDK_DISPLAY_XDISPLAY(gtk_widget_get_display(widget));

	char *buffer = g_strdup_printf("_NET_SYSTEM_TRAY_S%d", screen_number);

	widget_icon->selection_atom		= XInternAtom(display, buffer, False);
	widget_icon->manager_atom		= XInternAtom(display, "MANAGER", False);
	widget_icon->system_tray_opcode_atom	= XInternAtom(display, "_NET_SYSTEM_TRAY_OPCODE", False);
	widget_icon->orientation_atom		= XInternAtom(display, "_NET_SYSTEM_TRAY_ORIENTATION", False);

	free(buffer);

	tray_widget_update_manager_window(widget_icon, FALSE);
	tray_widget_send_dock_request(widget_icon);

	GdkWindow *root_window = gdk_screen_get_root_window(gtk_widget_get_screen(widget));
	gdk_window_add_filter(root_window, tray_widget_filter, widget_icon);
}

static void tray_widget_unrealize(GtkWidget *widget)
{
	struct _tray_widget_icon *widget_icon = TRAY_WIDGET_ICON(widget);

	if (widget_icon->manager_window != None)
	{
		GdkWindow *gdk_window = gdk_window_lookup_for_display(gtk_widget_get_display(widget), widget_icon->manager_window);
		gdk_window_remove_filter(gdk_window, tray_widget_filter, widget_icon);
	}

	GdkWindow *gdk_window = gdk_screen_get_root_window(gtk_widget_get_screen(widget));
	gdk_window_remove_filter(gdk_window, tray_widget_filter, widget_icon);

	if (GTK_WIDGET_CLASS(parent_class)->unrealize)
		(* GTK_WIDGET_CLASS(parent_class)->unrealize)(widget);
}

static void tray_widget_add(GtkContainer *container, GtkWidget *widget)
{
	g_signal_connect(widget, "realize", G_CALLBACK(tray_widget_make_transparent), NULL);
	GTK_CONTAINER_CLASS(parent_class)->add(container, widget);
}

static void tray_widget_class_init(struct _tray_widget *tray_widget)
{
	parent_class = g_type_class_peek_parent(tray_widget);

	GObjectClass *gobject_class		= (GObjectClass *) tray_widget;
	GtkWidgetClass *widget_class		= (GtkWidgetClass *) tray_widget;
	GtkContainerClass *container_class	= (GtkContainerClass *) tray_widget;

	gobject_class->get_property		= tray_widget_get_property;
	widget_class->realize			= tray_widget_realize;
	widget_class->unrealize			= tray_widget_unrealize;
	container_class->add			= tray_widget_add;

	g_object_class_install_property(gobject_class, PROPERTY_ORIENTATION, g_param_spec_enum("orientation", "Orientation", "The orientation of the tray.", GTK_TYPE_ORIENTATION, GTK_ORIENTATION_HORIZONTAL, G_PARAM_READABLE));
}

static void tray_widget_instance_init(struct _tray_widget_icon *widget_icon)
{
	widget_icon->stamp		= 1;
	widget_icon->orientation	= GTK_ORIENTATION_HORIZONTAL;
  
	gtk_widget_add_events(GTK_WIDGET(widget_icon), GDK_PROPERTY_CHANGE_MASK);
}

static void tray_widget_init_type(void)
{
	if (widget_type != 0)
		return;

	widget_type = g_type_from_name(type_name);

	if (widget_type == 0)
	{
		GTypeInfo type_info;
		bzero(&type_info, sizeof(GTypeInfo));

		type_info.class_size	= sizeof(struct _tray_widget);
		type_info.class_init	= (GClassInitFunc) tray_widget_class_init;

		type_info.instance_size = sizeof(struct _tray_widget_icon);
		type_info.instance_init	= (GInstanceInitFunc) tray_widget_instance_init;

		widget_type = g_type_register_static(GTK_TYPE_PLUG, type_name, &type_info, 0);
	}
	else if (parent_class == NULL)
		tray_widget_class_init((struct _tray_widget *) g_type_class_peek(widget_type));
}

struct _tray_widget* tray_widget_init(const gchar *name)
{
	tray_widget_init_type();
	return g_object_new(widget_type, "title", name, NULL);
}
