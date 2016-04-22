 /*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <gtk/gtk.h>

#include "support.h"
#include "eggtrayicon.h"
#include "eggclock.h"
#include "misc.h"
#include "config_main.h"

#include "trayicon.h"

int xneur_old_pid = -1;

static gboolean tray_icon_release (GtkWidget *widget, GdkEventButton *event, struct _tray_icon *tray)
{	
	if (widget){};
	if (event->button != 3)
		return TRUE;

	gtk_menu_popdown(GTK_MENU(tray->popup_menu));
	return FALSE;
}

gboolean tray_icon_press(GtkWidget *widget, GdkEventButton *event, struct _tray_icon *tray)
{
	if (widget){};
	if (event->button == 1 || event->button == 2)
		return TRUE;

	gtk_menu_popup(GTK_MENU(tray->popup_menu), NULL, NULL, NULL, NULL, event->button, event->time);
	return FALSE;
}

GtkWidget* create_menu_icon(struct _tray_icon *tray)
{
	GtkWidget *menu = gtk_menu_new();
	
	GtkWidget *menuitem = gtk_image_menu_item_new_with_mnemonic(_("Вкл./Выкл. Xneur"));

	GtkWidget *image = gtk_image_new_from_stock("gtk-refresh", GTK_ICON_SIZE_MENU);
	gtk_widget_set_name(image, "image");
	gtk_widget_show(image);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuitem), image);

	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(xneur_start_stop), tray);
	
	menuitem = gtk_image_menu_item_new_from_stock("gtk-preferences", NULL);
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(xneur_preference), tray);
	
	menuitem = gtk_separator_menu_item_new();
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	gtk_widget_set_sensitive(menuitem, FALSE);
	
	menuitem = gtk_image_menu_item_new_from_stock("gtk-about", NULL);
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(xneur_about), tray);	

	menuitem = gtk_image_menu_item_new_from_stock("gtk-quit", NULL);
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(xneur_exit), tray);

 	return menu;
}

static void clock_check_xneur (EggClock *clock, int hours, int minutes, int seconds, gpointer data)
{
	if (clock || hours || minutes || seconds) {};
	struct _tray_icon *tray = data;
	
	int xneur_pid = xneur_config_get_pid();	
	
	if (xneur_pid == xneur_old_pid)
		return;
	
	xneur_old_pid = xneur_pid;
	if (xneur_pid == -1)
		create_tray_icon(tray, FALSE);
	else
		create_tray_icon(tray, TRUE);
}

void create_tray_icon(struct _tray_icon *tray, gboolean runned)
{
	gchar *hint;
	char *image_file;
	if (runned == TRUE)
	{
		hint = _("X Neural Switcher RUNNED");
		image_file = "gxneur-run.png";
	}
	else
	{
		hint = _("X Neural Switcher STOPPED");
		image_file = "gxneur-stop.png";
	}
	
	if (tray != NULL)
	{
		gtk_object_destroy (GTK_OBJECT (tray->tooltip));
		tray->tooltip = NULL;
		gtk_widget_destroy (GTK_WIDGET (tray->popup_menu));
		tray->popup_menu = NULL;
		gtk_widget_destroy (GTK_WIDGET (tray->image));
		tray->image = NULL;
		gtk_widget_destroy (GTK_WIDGET (tray->evbox));
		tray->evbox = NULL;
	}
	else
	{
		tray = g_new0(struct _tray_icon, 1);	
			
		tray->clock = egg_clock_new();
		g_signal_connect (G_OBJECT(tray->clock), "time-changed", G_CALLBACK (clock_check_xneur), tray);
		
		tray->eggtray = egg_tray_icon_new(_("X Neural Switcher"));
		g_signal_connect(G_OBJECT(tray->eggtray), "button_press_event", G_CALLBACK(tray_icon_press), tray);
		g_signal_connect(G_OBJECT(tray->eggtray), "button_release_event", G_CALLBACK(tray_icon_release), tray);
	}
		
	tray->tooltip		= gtk_tooltips_new();
	tray->popup_menu	= create_menu_icon(tray);
	tray->evbox		= gtk_event_box_new();

	gtk_event_box_set_visible_window(GTK_EVENT_BOX(tray->evbox), 0);
	gtk_widget_set_size_request(tray->evbox, 25, 20);

	gtk_tooltips_set_tip(tray->tooltip, GTK_WIDGET(tray->eggtray), hint, NULL);	
	tray->image = create_pixmap(tray->evbox, image_file);
		
	gtk_container_add(GTK_CONTAINER(tray->evbox), tray->image);
	gtk_container_add(GTK_CONTAINER(tray->eggtray), tray->evbox);
	gtk_widget_show_all (GTK_WIDGET (tray->eggtray));
	return;
}
