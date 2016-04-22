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
#include <gdk/gdkx.h>
#include <string.h>
#include <stdlib.h>

#include "misc.h"

#include "support.h"

#include "callbacks.h"

#include <glade/glade.h>
#define GLADE_FILE_LIST PACKAGE_GLADE_FILE_DIR"/list.glade"

static void set_dialog_text(int index, const char *name, GladeXML *gxml_config)
{
	GladeXML *gxml = glade_xml_new (GLADE_FILE_LIST, NULL, NULL);
	
	char *text_path		= xneur_get_dict_path(gxml_config, index, name);
	char *text_home_path	= xneur_get_home_dict_path(gxml_config, index, name);
	char *text		= xneur_get_file_content(text_path);

	if (text == NULL)
	{
		free(text_home_path);
		free(text_path);
		return;
	}
	
	GtkWidget *window = glade_xml_get_widget (gxml, "dialog1");
	GdkPixbuf *window_icon_pixbuf = create_pixbuf ("gxneur.png");
	if (window_icon_pixbuf)
	{
		gtk_window_set_icon (GTK_WINDOW (window), window_icon_pixbuf);
		gdk_pixbuf_unref (window_icon_pixbuf);
	}
	gtk_widget_show(window);

	GtkWidget *widget = glade_xml_get_widget (gxml, "textview1");
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
	gtk_text_buffer_set_text(buffer, text, strlen(text));

	widget = glade_xml_get_widget (gxml, "entry10");
	gtk_entry_set_text(GTK_ENTRY(widget), text_home_path);

	widget= glade_xml_get_widget (gxml, "okbutton1");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_okbutton1_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "cancelbutton1");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_cancelbutton1_clicked), gxml);
	
	free(text);
	free(text_home_path);
	free(text_path);
}

void on_button6_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
	set_dialog_text(0, "dict", user_data);
}

void on_button7_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
	set_dialog_text(1, "dict", user_data);
}

void on_button8_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
	set_dialog_text(2, "dict", user_data);
}

void on_button9_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
	set_dialog_text(3, "dict", user_data);
}

void on_button23_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
	set_dialog_text(0, "regexp", user_data);
}

void on_button24_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
	set_dialog_text(1, "regexp", user_data);
}

void on_button25_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
	set_dialog_text(2, "regexp", user_data);
}


void on_button26_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
	set_dialog_text(3, "regexp", user_data);
}

// Save dictionary
void on_okbutton1_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};

	GladeXML *gxml = user_data;
	GtkWidget *widgetPtrToBefound = glade_xml_get_widget (gxml, "textview1");
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgetPtrToBefound));

	GtkTextIter begin, end;
	gtk_text_buffer_get_bounds(buffer, &begin, &end);

	char *text = gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);

	widgetPtrToBefound = glade_xml_get_widget (gxml, "entry10");

	char *path = g_strdup_printf("%s", gtk_entry_get_text(GTK_ENTRY(widgetPtrToBefound)));

	FILE *stream = fopen(path, "w");
	if (stream != NULL)
	{
		fputs(text, stream);
		fclose(stream);
	}

	GtkWidget *window = glade_xml_get_widget (gxml, "dialog1");
	gtk_widget_destroy(window);
}

void on_cancelbutton1_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
	
	GladeXML *gxml = user_data;
	GtkWidget *window = glade_xml_get_widget (gxml, "dialog1");
	gtk_widget_destroy(window);
}

// Parse keyboard binds
gboolean on_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	if (user_data){};
		
	char *string		= XKeysymToString(XKeycodeToKeysym(GDK_DISPLAY(), event->hardware_keycode, 0));
	gchar *modifiers	= modifiers_to_string(event->state);
	gchar *keycode		= g_strdup_printf("%d", event->hardware_keycode);
	gchar *mkey		= g_strdup_printf("%s%s", modifiers, (string != NULL) ? string : keycode);

	gtk_entry_set_text(GTK_ENTRY(widget), mkey);

	g_free(mkey);
	g_free(keycode);
	g_free(modifiers);
		
	return FALSE;
}

gboolean on_key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	if (user_data || event){};

	gchar **key_stat = g_strsplit(gtk_entry_get_text(GTK_ENTRY(widget)), "+", 4);

	if (is_correct_hotkey(key_stat) == -1)
		gtk_entry_set_text(GTK_ENTRY(widget), _("Press any key"));

	g_strfreev(key_stat);
	return FALSE;
}

static void clear_hotkey_text(const char *name, GladeXML *gxml)
{
	GtkWidget *widget = glade_xml_get_widget (gxml, name);
	gtk_entry_set_text(GTK_ENTRY(widget), "");
}

void on_button11_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
	
	clear_hotkey_text("entry11", user_data);
}

void on_button12_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
		
	clear_hotkey_text("entry12", user_data);
}

void on_button13_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
		
	clear_hotkey_text("entry13", user_data);
}

void on_button14_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
		
	clear_hotkey_text("entry14", user_data);
}

void on_button15_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
		
	clear_hotkey_text("entry15", user_data);
}

void on_button16_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
		
	clear_hotkey_text("entry16", user_data);
}

void on_button17_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
		
	clear_hotkey_text("entry17", user_data);
}

void on_button18_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
		
	clear_hotkey_text("entry18", user_data);
}

void on_button29_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
		
	clear_hotkey_text("entry19", user_data);
}

void on_button30_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
		
	clear_hotkey_text("entry20", user_data);
}

void on_window2_delete_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	if (widget || event || user_data){};

	gtk_widget_destroy(widget);
	xneur_start();
}
