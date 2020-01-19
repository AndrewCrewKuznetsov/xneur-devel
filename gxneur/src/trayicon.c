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

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <gtk/gtk.h>

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "support.h"

#include "xkb.h"
#include "misc.h"

#include <xneur/xnconfig.h>

#include "trayicon.h"

#include "configuration.h"

extern const char* arg_show_in_the_tray;
extern const char* arg_rendering_engine;
extern const char* arg_icons_directory;

extern struct _xneur_config *xconfig;

struct _tray_icon *tray;

gchar *show_in_the_tray = NULL;
gchar *rendering_engine = NULL;

Display *dpy = NULL;

static int xneur_old_pid = -1;
static int xneur_old_state = -1;
static int xneur_old_group = -1;
static char *xneur_old_symbol = NULL;

static int force_update = FALSE;

//#define ICON_SIZE 24

static void exec_user_action(char *cmd)
{
	static const char[] TO_STDOUT = " 2> /dev/stdout";
	printf("%s\n", cmd);
	size_t len = strlen(cmd);
	// sizeof(TO_STDOUT) already includes trailing zero, so +1 not needed
	char *command = malloc((len + sizeof(TO_STDOUT)) * sizeof(char));
	command[0] = '\0';
	strncat(command, cmd, len);
	strncat(command, TO_STDOUT, sizeof(TO_STDOUT)-1);

	FILE *fp = popen(command, "r");
	if (command != NULL)
		g_free(command);
	if (fp == NULL)
		return;

	char buffer[NAME_MAX];
	if (fgets(buffer, NAME_MAX, fp) == NULL)
	{
		pclose(fp);
		return;
	}

	pclose(fp);

	error_msg(buffer);
}

GtkMenu* create_menu(struct _tray_icon *tray, int state)
{
	if (state) {};
	GtkMenu *menu = GTK_MENU(gtk_menu_new());
	GtkWidget *menuitem;
	GtkWidget *image;

	// Start/stop
	gchar *status_text;
	int xneur_pid = xconfig->get_pid(xconfig);
	if (xneur_pid != -1)
	{
		status_text = g_strdup_printf("%s", _("Stop daemon"));
	}
	else
	{
		status_text = g_strdup_printf("%s", _("Start daemon"));
	}
	xneur_old_pid = xneur_pid;

	if (tray->status == NULL)
	{
		tray->status = gtk_menu_item_new_with_mnemonic(status_text);
		gtk_widget_show(tray->status);
		gtk_container_add(GTK_CONTAINER(menu), tray->status);
		g_signal_connect(G_OBJECT(tray->status), "activate", G_CALLBACK(xneur_start_stop), tray);
	}
	else
	{
		gtk_menu_item_set_label(GTK_MENU_ITEM(tray->status), status_text);
	}

	if (status_text != NULL)
		g_free(status_text);

	// Separator
	menuitem = gtk_separator_menu_item_new();
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	gtk_widget_set_sensitive(menuitem, FALSE);

	// User Actions Submenu
	GtkWidget *action_submenu = gtk_menu_new();

	for (int action = 0; action < xconfig->user_actions_count; action++)
	{
		menuitem = gtk_menu_item_new_with_mnemonic(xconfig->user_actions[action].name);
		gtk_widget_show(menuitem);
		g_signal_connect_swapped(G_OBJECT(menuitem), "activate", G_CALLBACK(exec_user_action), xconfig->user_actions[action].command);
		gtk_container_add(GTK_CONTAINER(action_submenu), menuitem);
	}
	menuitem = gtk_image_menu_item_new_with_mnemonic(_("User action"));

	image = gtk_image_new_from_stock("gtk-execute", GTK_ICON_SIZE_MENU);
	gtk_widget_set_name(image, "image");
	gtk_widget_show(image);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuitem), image);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), action_submenu);
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	if (xconfig->actions_count < 1)
		gtk_widget_set_sensitive(menuitem, FALSE);

	// View log
	menuitem = gtk_menu_item_new_with_mnemonic(_("View log..."));
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(xneur_get_logfile), tray);

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

	// Keyboard Preference
	menuitem = gtk_menu_item_new_with_mnemonic(_("Keyboard Properties"));
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER(menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(xneur_kb_preference), tray);

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


	//gtk_widget_show (GTK_WIDGET(menu));
 	return GTK_MENU(menu);
}

gboolean tray_icon_press(GtkWidget *widget, GdkEventButton *event)
{
	if (widget){};

	if (event->button == 2)
	{
		xneur_start_stop();
		return FALSE;
	}

	if (event->button == 1)
	{
		set_next_kbd_group(dpy);
		return FALSE;
	}

	gtk_menu_popup(GTK_MENU(tray->menu), NULL, NULL, NULL, NULL, event->button, event->time);

	return FALSE;
}

void status_icon_on_click(GtkStatusIcon *status_icon,
                        gpointer user_data)
{
	if (status_icon || user_data){};
	set_next_kbd_group(dpy);
}

void status_icon_on_menu(GtkStatusIcon *status_icon, guint button,
                       guint activate_time, gpointer user_data)
{
	if (status_icon || user_data){};

	gtk_menu_popup(GTK_MENU(tray->menu), NULL, NULL, NULL, NULL, button, activate_time);
}

GdkPixbuf *text_to_gtk_pixbuf (gchar *text)
{
	GtkWidget *scratch = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_realize (scratch);
	//GtkStyle *style = gtk_widget_get_style(scratch);
	//gchar *bgcolor = gdk_color_to_string(&style->bg[GTK_STATE_NORMAL]);
	//gchar *textcolor = gdk_color_to_string(&style->text[GTK_STATE_NORMAL]);
	PangoLayout *layout = gtk_widget_create_pango_layout (scratch, NULL);
	//g_object_unref(style);
	gtk_widget_destroy (scratch);

	//gchar *markup = g_strdup_printf ("<span bgcolor='%s' color='%s'>%s</span>", bgcolor, textcolor, text);
	//gchar *markup = g_strdup_printf ("<span color='%s'>%s</span>", textcolor, text);
	gchar *markup = g_strdup_printf ("%s", text);
	pango_layout_set_markup (layout, markup, -1);
	if (markup != NULL)
		g_free (markup);
	//g_free (bgcolor);
	//g_free (textcolor);

	int width = 0;
	int heigth = 0;
	pango_layout_get_size (layout, &width, &heigth);
	width = width/PANGO_SCALE;
	heigth = heigth/PANGO_SCALE;

	GdkPixbuf *pb = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width, heigth);
	gdk_pixbuf_fill(pb, 0xffffffff);

	GdkPixmap *pm = gdk_pixmap_new (NULL, width, heigth, 24);
	GdkGC *gc = gdk_gc_new (pm);
	gdk_draw_pixbuf (pm, gc, pb, 0, 0, 0, 0, width, heigth, GDK_RGB_DITHER_NONE, 0, 0);

	gdk_draw_layout (pm, gc, 0, 0, layout);


	g_object_unref(layout);
	g_object_unref(gc);

	GdkPixbuf *ret = gdk_pixbuf_get_from_drawable (NULL, pm, NULL, 0, 0, 0, 0, width, heigth);
	g_object_unref(pb);
	return ret;
}

static const char *get_tray_icon_name (char *name)
{
	const char * icon_name;
	char *full_name = g_strdup_printf("%s-%s", PACKAGE, name);

	if (strcasecmp(show_in_the_tray, "Icon") == 0)
	{
		icon_name = PACKAGE;
		return icon_name;
	}

	if (strcasecmp(show_in_the_tray, "Directory") == 0)
	{
		gchar *string_value = NULL;
		gxneur_config_read_str("icons-directory", &string_value);
		if (string_value != NULL)
		{
			icon_name = g_strdup_printf ("%s%s%s.png", string_value,
                                         G_DIR_SEPARATOR_S, name);
			printf("Icons '%s'\n",icon_name);
  			if (g_file_test (icon_name, G_FILE_TEST_EXISTS))
				return icon_name;
			icon_name = g_strdup_printf ("%s%s%s.png", string_value,
                                         G_DIR_SEPARATOR_S, full_name);
			if (g_file_test (icon_name, G_FILE_TEST_EXISTS))
				return icon_name;
			icon_name = g_strdup_printf ("%s%s%s.png", string_value,
                                         G_DIR_SEPARATOR_S, PACKAGE);
			if (g_file_test (icon_name, G_FILE_TEST_EXISTS))
				return icon_name;
		}
		if (string_value != NULL)
 			g_free (string_value);
		icon_name = PACKAGE;
		return icon_name;
	}

	//return "/home/crew/develop/xneur-devel/gxneur/pixmaps/all pixmaps/switcher/gxneur.png";

	GtkIconTheme * theme = gtk_icon_theme_get_default( );

	// if the tray's icon is a 48x48 file, use it;
	// otherwise, use the fallback builtin icon
	if (!gtk_icon_theme_has_icon (theme, full_name))
	{
		icon_name = PACKAGE;
	}
	else
	{
		GtkIconInfo * icon_info = gtk_icon_theme_lookup_icon (theme, full_name, 48, GTK_ICON_LOOKUP_USE_BUILTIN);
		const gboolean icon_is_builtin = gtk_icon_info_get_filename (icon_info) == NULL;
		gtk_icon_info_free (icon_info);
		icon_name = icon_is_builtin ? PACKAGE : full_name;
	}

	return icon_name;
}

gboolean clock_check(gpointer dummy)
{
	if (dummy) {};

	int xneur_pid = -1;
	if (xneur_old_pid == -1)
		xneur_old_pid = 1;
	char ps_command[20 + 30];// 20 - length of format string, 30 - more, than maximum length of int in decimal format
	snprintf(ps_command, sizeof(ps_command)/sizeof(ps_command[0]), "ps -p %d | grep xneur", xneur_old_pid);
	FILE *fp = popen(ps_command, "r");
	if (fp != NULL)
	{
		char buffer[NAME_MAX];
		if (fgets(buffer, NAME_MAX, fp) != NULL)
			xneur_pid = xneur_old_pid;

		pclose(fp);
	}
	if (xneur_pid == -1)
		xneur_pid = xconfig->get_pid(xconfig);

	int xneur_state = xconfig->manual_mode;
	int xneur_group = get_active_kbd_group(dpy);
	char *xneur_symbol = get_active_kbd_symbol(dpy);

	/*if (get_kbd_group_count(dpy) != xconfig->handle->total_languages)
	{
		gtk_widget_destroy(GTK_WIDGET(tray->menu));
		tray->menu = NULL;

		g_spawn_command_line_async(PACKAGE, NULL);

		gtk_main_quit();
	}*/


	if  (xneur_pid == xneur_old_pid &&
		xneur_state == xneur_old_state &&
		xneur_group == xneur_old_group &&
	    strcmp(xneur_old_symbol, xneur_symbol) == 0 &&
		force_update == FALSE)
	{
		if (xneur_symbol != NULL)
			g_free(xneur_symbol);
		return TRUE;
	}
	force_update = FALSE;

	xneur_old_pid = xneur_pid;
	xneur_old_state = xneur_state;
	xneur_old_group = xneur_group;
	if (xneur_old_symbol != NULL)
	{
		g_free(xneur_old_symbol);
	}
	xneur_old_symbol = xneur_symbol;

	gchar *hint;
	gchar *status_text;
	//float saturation = 1.0;
	if (xneur_pid != -1)
	{
		//saturation = 1.0;
		hint = g_strdup_printf("%s%s%s", _("X Neural Switcher running ("), xneur_symbol, ")");
		status_text = g_strdup_printf("%s", _("Stop daemon"));
	}
	else
	{
		//saturation = 0.25;
		hint = g_strdup_printf("%s%s%s", _("X Neural Switcher stopped ("), xneur_symbol, ")");
		status_text = g_strdup_printf("%s", _("Start daemon"));
	}

	gtk_menu_item_set_label(GTK_MENU_ITEM(tray->status), status_text);

	const char *icon_name = get_tray_icon_name(xneur_symbol);
	if (tray->tray_icon)
	{
		gtk_widget_hide_all(GTK_WIDGET(tray->tray_icon));
		gtk_widget_destroy (tray->image);
		if (strcasecmp(show_in_the_tray, "Text") == 0)
		{
			char *layout_name = get_active_kbd_symbol(dpy);
			for (unsigned int i=0; i < strlen(layout_name); i++)
				layout_name[i] = toupper(layout_name[i]);
			tray->image = gtk_label_new ((const gchar *)layout_name);
			gtk_label_set_justify (GTK_LABEL(tray->image), GTK_JUSTIFY_CENTER);
			if (layout_name != NULL)
				g_free(layout_name);
		}
		else
		{
			if ((strcasecmp(show_in_the_tray, "Directory") != 0))
				tray->image = gtk_image_new_from_icon_name(icon_name, GTK_ICON_SIZE_LARGE_TOOLBAR);
			else
				tray->image = gtk_image_new_from_file(icon_name);
		}
		gtk_container_add(GTK_CONTAINER(tray->evbox), tray->image);
		gtk_widget_show_all(GTK_WIDGET(tray->tray_icon));
	}
	else if (tray->status_icon)
	{
		if (gtk_status_icon_is_embedded(tray->status_icon))
		{
			if (strcasecmp(show_in_the_tray, "Text") == 0)
			{
				char *layout_name = get_active_kbd_symbol(dpy);
				for (unsigned int i=0; i < strlen(layout_name); i++)
					layout_name[i] = toupper(layout_name[i]);
				GdkPixbuf *pb = text_to_gtk_pixbuf (layout_name);
				if (layout_name != NULL)
					g_free(layout_name);
				pb = gdk_pixbuf_add_alpha(pb, TRUE, 255, 255, 255);
				gtk_status_icon_set_from_pixbuf(tray->status_icon, pb);
				g_object_unref(pb);
			}
			else
			{
				if ((strcasecmp(show_in_the_tray, "Directory") != 0))
					gtk_status_icon_set_from_icon_name(tray->status_icon, icon_name);
				else
					gtk_status_icon_set_from_file(tray->status_icon, icon_name);
			}

			gtk_status_icon_set_tooltip(tray->status_icon, hint);
		}
	}
#ifdef HAVE_APP_INDICATOR
	else if (tray->app_indicator)
	{
		if (strcasecmp(show_in_the_tray, "Text") == 0)
		{
#ifdef HAVE_DEPREC_APP_INDICATOR
			app_indicator_set_icon (tray->app_indicator, icon_name);
#else
			char *layout_name = get_active_kbd_symbol(dpy);
			for (unsigned int i=0; i < strlen(layout_name); i++)
				layout_name[i] = toupper(layout_name[i]);
			app_indicator_set_label (tray->app_indicator, layout_name, layout_name);
			if (layout_name != NULL)
				g_free(layout_name);
			app_indicator_set_icon_full (tray->app_indicator, "", "");
#endif
		}
		else
		{
#ifdef HAVE_DEPREC_APP_INDICATOR
			app_indicator_set_icon (tray->app_indicator, icon_name);
#else
			app_indicator_set_icon_full (tray->app_indicator, icon_name, icon_name);
			app_indicator_set_label (tray->app_indicator,"", "");
#endif
		}
	}
#endif

	if (hint != NULL)
		g_free (hint);
	if (status_text != NULL)
		g_free (status_text);

	return TRUE;
}

void xneur_start_stop(void)
{
	if (xconfig->kill(xconfig) == TRUE)
	{
		clock_check(0);
		return;
	}

	xneur_start();
	clock_check(0);
}

void xneur_exit(void)
{
	/*for (int i = 0; i < MAX_LAYOUTS; i++)
	{
		if (tray->images[i] != NULL)
			g_free(tray->images[i]);
	}*/

	gtk_widget_destroy(GTK_WIDGET(tray->menu));
	tray->menu = NULL;

	xconfig->kill(xconfig);
	XCloseDisplay(dpy);
	gtk_main_quit();
}

static
void show_in_the_tray_callback(gpointer user_data)
{
	if (user_data) {};

	if (arg_show_in_the_tray)
		return;

	gxneur_config_read_str("show-in-the-tray", &show_in_the_tray);

	force_update = TRUE;
}

static
void rendering_engine_callback(gpointer user_data)
{
	if (user_data) {};

	if (arg_rendering_engine)
		return;

	gchar *new_engine = g_strdup(rendering_engine);
	gxneur_config_read_str("rendering-engine", &new_engine);

	if (strcasecmp(new_engine, rendering_engine) == 0)
		return;

	/*for (int i = 0; i < MAX_LAYOUTS; i++)
	{
		if (tray->images[i] != NULL)
			g_free(tray->images[i]);
	}*/

	gtk_widget_destroy(GTK_WIDGET(tray->menu));
	tray->menu = NULL;

	g_spawn_command_line_async(PACKAGE, NULL);

	gtk_main_quit();
}

void create_tray_icon (void)
{
	dpy = XOpenDisplay(NULL);

	gxneur_config_read_str("show-in-the-tray", &show_in_the_tray);
	gxneur_config_read_str("rendering-engine", &rendering_engine);

	gxneur_config_add_notify("show-in-the-tray", show_in_the_tray_callback);
	gxneur_config_add_notify("rendering-engine", rendering_engine_callback);

	if (arg_show_in_the_tray)
	{
		if (show_in_the_tray != NULL)
			g_free(show_in_the_tray);
		show_in_the_tray = g_strdup(arg_show_in_the_tray);
	}
	if (arg_rendering_engine)
	{
		if (rendering_engine != NULL)
			g_free(rendering_engine);
		rendering_engine = g_strdup(arg_rendering_engine);
	}

	if (!show_in_the_tray)
		show_in_the_tray = g_strdup(DEFAULT_SHOW_IN_THE_TRAY);
	if (!rendering_engine)
		rendering_engine = g_strdup(DEFAULT_RENDERING_ENGINE);


	tray = g_new0(struct _tray_icon, 1);
#ifdef HAVE_APP_INDICATOR
	tray->app_indicator = NULL;
#endif
	tray->status_icon = NULL;
	tray->tray_icon = NULL;

	// Init pixbuf array
	/*for (int i = 0; i < MAX_LAYOUTS; i++)
	{
		tray->images[i] = NULL;
	}*/

	// Load images names
	/*for (int i = 0; i < xconfig->handle->total_languages; i++)
	{
		char *layout_name = strdup(xconfig->handle->languages[i].dir);
		tray->images[i] = g_strdup_printf("%s-%s", PACKAGE, layout_name);
		free(layout_name);
	}*/

	tray->menu = create_menu(tray, xconfig->manual_mode);

	int tray_icon_created = 0;
	int tray_icon_failed = 0;

	if (strcasecmp(rendering_engine, "AppIndicator") == 0)
	{
#ifdef HAVE_APP_INDICATOR
		// App indicator
		tray->app_indicator = app_indicator_new ("X Neural Switcher",
		                           PACKAGE,
		                           APP_INDICATOR_CATEGORY_APPLICATION_STATUS);

		if (tray->app_indicator)
		{
			app_indicator_set_status (tray->app_indicator, APP_INDICATOR_STATUS_ACTIVE);
			app_indicator_set_menu (tray->app_indicator, tray->menu);

			tray_icon_created = 1;
			tray_icon_failed = 0;
		}
		else
		{
			tray_icon_failed = 1;
		}
#else
		tray_icon_failed = 1;
#endif
	}


	// Tray icon
	if (strcasecmp(rendering_engine, "Built-in") == 0 /*|| tray_icon_failed*/)
	{
		tray->tray_icon = _gtk_tray_icon_new(_("X Neural Switcher"));

		if (tray->tray_icon)
		{

			g_signal_connect(G_OBJECT(tray->tray_icon), "button_press_event", G_CALLBACK(tray_icon_press), NULL);

			tray->evbox = gtk_event_box_new();
			gtk_event_box_set_visible_window(GTK_EVENT_BOX(tray->evbox), 0);
			if (strcasecmp(show_in_the_tray, "Text") == 0)
			{
				char *layout_name = get_active_kbd_symbol (dpy);
				for (unsigned int i=0; i < strlen(layout_name); i++)
					layout_name[i] = toupper(layout_name[i]);
				tray->image = gtk_label_new ((const gchar *)layout_name);
				gtk_label_set_justify (GTK_LABEL(tray->image), GTK_JUSTIFY_CENTER);
				if (layout_name != NULL)
					g_free(layout_name);
			}
			else
			{
				char *layout_name = get_active_kbd_symbol (dpy);
				tray->image = gtk_image_new_from_icon_name(layout_name, GTK_ICON_SIZE_LARGE_TOOLBAR);
				//tray->image = gtk_image_new_from_icon_name(tray->images[kbd_gr], GTK_ICON_SIZE_LARGE_TOOLBAR);
				if (layout_name != NULL)
					g_free(layout_name);
			}
			gtk_container_add(GTK_CONTAINER(tray->evbox), tray->image);
			gtk_container_add(GTK_CONTAINER(tray->tray_icon), tray->evbox);

			gtk_widget_show_all(GTK_WIDGET(tray->tray_icon));

			tray_icon_created = 1;
			tray_icon_failed = 0;
		}
		else
		{
			tray_icon_failed = 1;
		}
	}

	// Status Icon
	if (tray_icon_failed || !tray_icon_created || strcasecmp(rendering_engine, "StatusIcon") == 0 )
	{
		tray->status_icon = gtk_status_icon_new();

		g_signal_connect(G_OBJECT(tray->status_icon), "activate", G_CALLBACK(status_icon_on_click), NULL);
		g_signal_connect(G_OBJECT(tray->status_icon), "popup-menu", G_CALLBACK(status_icon_on_menu), NULL);

		gtk_status_icon_set_from_icon_name(tray->status_icon,  PACKAGE);
		gtk_status_icon_set_tooltip_text(tray->status_icon, "X Neural Switcher");
		gtk_status_icon_set_visible(tray->status_icon, TRUE);

		if (strcasecmp(show_in_the_tray, "Text") == 0)
		{
			char *layout_name = get_active_kbd_symbol (dpy);
			for (unsigned int i=0; i < strlen(layout_name); i++)
				layout_name[i] = toupper(layout_name[i]);

			GdkPixbuf *pb = text_to_gtk_pixbuf (layout_name);
			if (layout_name != NULL)
				g_free(layout_name);
			pb = gdk_pixbuf_add_alpha(pb, TRUE, 255, 255, 255);
			gtk_status_icon_set_from_pixbuf(tray->status_icon, pb);
			g_object_unref(pb);
		}
		else
		{
			char *layout_name = get_active_kbd_symbol (dpy);
			//gtk_status_icon_set_from_icon_name(tray->status_icon, tray->images[kbd_gr]);
			gtk_status_icon_set_from_icon_name(tray->status_icon, layout_name);
			if (layout_name != NULL)
				g_free(layout_name);
		}
	}

	force_update = TRUE;

	g_timeout_add(1000, clock_check, 0);
}
