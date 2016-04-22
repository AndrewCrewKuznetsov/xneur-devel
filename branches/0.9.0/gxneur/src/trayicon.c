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
 *  Copyright (C) 2006-2008 XNeur Team
 *
 */

#include <gtk/gtk.h>

#include "support.h"

#include "clock.h"
#include "xkb.h"
#include "misc.h"
#include "tray_widget.h"

#include <xneur/xnconfig.h>

#include "trayicon.h"

extern struct _xneur_config *xconfig;

static int xneur_old_pid = -1;
static int xneur_old_state = -1;
static int xneur_old_group = -1;

static gboolean tray_icon_release(GtkWidget *widget, GdkEventButton *event, struct _tray_icon *tray)
{	
	if (widget){};
		
	if (event->button == 1 || event->button == 2)
		return TRUE;

	gtk_menu_popdown(GTK_MENU(tray->popup_menu));

	return FALSE;
}

gboolean tray_icon_press(GtkWidget *widget, GdkEventButton *event, struct _tray_icon *tray)
{
	if (event->button == 2)
	{
		xneur_start_stop(widget, tray);
		return FALSE;
	}
	
	if (event->button == 1)
	{
		set_next_keyboard_group();
		return FALSE;
	}
	
	gtk_menu_popup(GTK_MENU(tray->popup_menu), NULL, NULL, NULL, NULL, event->button, event->time);
	return FALSE;
}

GtkWidget* create_menu_icon(struct _tray_icon *tray, gboolean runned, int state)
{
	GtkWidget *menu = gtk_menu_new();
	
	// Start/stop
	gchar *menu_text;
	gchar *menu_icon;
	if (runned == TRUE)
	{
		menu_text = _("Stop Daemon");
		menu_icon = "gtk-stop";
	}
	else
	{
		menu_text = _("Run Daemon");
		menu_icon = "gtk-execute";
	}

	GtkWidget *menuitem = gtk_image_menu_item_new_with_mnemonic(menu_text);
	GtkWidget *image = gtk_image_new_from_stock(menu_icon, GTK_ICON_SIZE_MENU);

	gtk_widget_set_name(image, "image");
	gtk_widget_show(image);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuitem), image);
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(xneur_start_stop), tray);
	
	// State
	if (state == 0)
	{
		menu_text = _("Disable Input Autocorrection");
		menu_icon = "gtk-disconnect";
	}
	else
	{
		menu_text = _("Enable Input Autocorrection");
		menu_icon = "gtk-connect";
	}

	menuitem = gtk_image_menu_item_new_with_mnemonic(menu_text);
	image = gtk_image_new_from_stock(menu_icon, GTK_ICON_SIZE_MENU);
	gtk_widget_set_name(image, "image");
	gtk_widget_show(image);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuitem), image);
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(xneur_auto_manual), tray);

	if (runned == FALSE)
		gtk_widget_set_sensitive(menuitem, FALSE);
	
	// Separator
	menuitem = gtk_separator_menu_item_new();
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	gtk_widget_set_sensitive(menuitem, FALSE);
	
	// Preference
	menuitem = gtk_image_menu_item_new_from_stock("gtk-preferences", NULL);
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(xneur_preference), tray);
	
	// About
	menuitem = gtk_image_menu_item_new_from_stock("gtk-about", NULL);
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(xneur_about), tray);	

	// Exit
	menuitem = gtk_image_menu_item_new_from_stock("gtk-quit", NULL);
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(xneur_exit), tray);

 	return menu;
}

void clock_check(Clock *clock)
{
	struct _tray_icon *tray = clock->tray;
	
	int xneur_pid = xconfig->get_pid(xconfig);
	int xneur_state = xconfig->get_current_mode(xconfig);
	int xneur_group = get_active_keyboard_group();

	if (xneur_pid == xneur_old_pid && xneur_state == xneur_old_state && xneur_group == xneur_old_group)
		return;

	xneur_old_pid = xneur_pid;
	xneur_old_state = xneur_state;
	xneur_old_group = xneur_group;

	if (xneur_pid == -1)
		create_tray_icon(tray, FALSE);
	else
		create_tray_icon(tray, TRUE);
}

void create_tray_icon(struct _tray_icon *tray, gboolean runned)
{
	char *layout_name = xconfig->get_lang_name(xconfig, xconfig->find_group_lang(xconfig, get_active_keyboard_group()));
	if (layout_name == NULL) 
		layout_name = "gxneur";

	char *image_file = g_strdup_printf("%s%s", layout_name, ".png");
	
	gchar *hint;
	float saturation;
	if (runned == TRUE)
	{
		saturation = 1.0;
		hint = g_strdup_printf("%s%s%s", _("X Neural Switcher RUNNING ("), layout_name, ")");
	}
	else
	{
		saturation = 0.25;
		hint = g_strdup_printf("%s%s%s", _("X Neural Switcher STOPPED ("), layout_name, ")");
	}

	int init_clock = 0;
	if (tray != NULL)
	{
		gtk_object_destroy(GTK_OBJECT(tray->tooltip));
		tray->tooltip = NULL;

		gtk_widget_destroy(GTK_WIDGET(tray->popup_menu));
		tray->popup_menu = NULL;

		gtk_widget_destroy(GTK_WIDGET(tray->image));
		tray->image = NULL;

		gtk_widget_destroy(GTK_WIDGET(tray->evbox));
		tray->evbox = NULL;
	}
	else
	{
		tray = g_new0(struct _tray_icon, 1);	

		tray->widget = tray_widget_init(_("X Neural Switcher"));
		g_signal_connect(G_OBJECT(tray->widget), "button_press_event", G_CALLBACK(tray_icon_press), tray);
		g_signal_connect(G_OBJECT(tray->widget), "button_release_event", G_CALLBACK(tray_icon_release), tray);

		init_clock = 1;
	}
		
	tray->tooltip		= gtk_tooltips_new();
	tray->popup_menu	= create_menu_icon(tray, runned, xconfig->get_current_mode(xconfig));
	tray->evbox		= gtk_event_box_new();

	gtk_event_box_set_visible_window(GTK_EVENT_BOX(tray->evbox), 0);
	gtk_widget_set_size_request(tray->evbox, 27, 18);

	gtk_tooltips_set_tip(tray->tooltip, GTK_WIDGET(tray->widget), hint, NULL);
	
	GdkPixbuf *pb = create_pixbuf(image_file);
	if (pb == NULL)
	{
		layout_name = "gxneur";
		image_file = g_strdup_printf("%s%s", layout_name, ".png");
		pb = create_pixbuf(image_file);
	}
	gdk_pixbuf_saturate_and_pixelate(pb, pb, saturation, FALSE);
	tray->image = gtk_image_new_from_pixbuf(pb);
		
	gtk_container_add(GTK_CONTAINER(tray->evbox), tray->image);
	gtk_container_add(GTK_CONTAINER(tray->widget), tray->evbox);
	gtk_widget_show_all(GTK_WIDGET(tray->widget));

	if (init_clock == 1)
		tray->clock = clock_new(clock_check, tray);
}
