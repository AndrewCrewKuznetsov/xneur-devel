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

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <string.h>
#include <stdlib.h>

#include "misc.h"
#include "interface.h"
#include "support.h"

#include "callbacks.h"

GtkWidget *wDict = NULL;

static void set_dialog_text(int index, const char *name)
{
	char *text_path = xneur_get_dict_path(index, name);
	char *text = xneur_get_file_content(text_path);
	free(text_path);
	text_path = xneur_get_home_dict_path(index, name);
	
	if (text == NULL)
		return;

	wDict = create_dialog1();
	gtk_widget_show(wDict);

	GtkWidget *widgetPtrToBefound = lookup_widget(wDict, "textview1");
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgetPtrToBefound));
	gtk_text_buffer_set_text(buffer, text, strlen(text));

	widgetPtrToBefound = lookup_widget(wDict, "entry10");
	gtk_entry_set_text(GTK_ENTRY(widgetPtrToBefound), text_path);
}

void on_button6_clicked(GtkButton *button, gpointer user_data)
{
	if (button || user_data){};
	set_dialog_text(0, "dict");
}

void on_button7_clicked(GtkButton *button, gpointer user_data)
{
	if (button || user_data){};
	set_dialog_text(1, "dict");
}

void on_button8_clicked(GtkButton *button, gpointer user_data)
{
	if (button || user_data){};
	set_dialog_text(2, "dict");
}

void on_button9_clicked(GtkButton *button, gpointer user_data)
{
	if (button || user_data){};
	set_dialog_text(3, "dict");
}

void on_button23_clicked(GtkButton *button, gpointer user_data)
{
	if (button || user_data){};
	set_dialog_text(0, "regexp");
}

void on_button24_clicked(GtkButton *button, gpointer user_data)
{
	if (button || user_data){};
	set_dialog_text(1, "regexp");
}

void on_button25_clicked(GtkButton *button, gpointer user_data)
{
	if (button || user_data){};
	set_dialog_text(2, "regexp");
}


void on_button26_clicked(GtkButton *button, gpointer user_data)
{
	if (button || user_data){};
	set_dialog_text(3, "regexp");
}

// Save dictionary
void on_okbutton1_clicked(GtkButton *button, gpointer user_data)
{
	if (button||user_data){};

	GtkWidget *widgetPtrToBefound = lookup_widget(wDict, "textview1");
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgetPtrToBefound));

	GtkTextIter begin, end;
	gtk_text_buffer_get_bounds(buffer, &begin, &end);

	char *text = gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);

	widgetPtrToBefound = lookup_widget(wDict, "entry10");

	char *path = g_strdup_printf("%s",gtk_entry_get_text(GTK_ENTRY(widgetPtrToBefound)));

	FILE *stream = fopen(path, "w");
	if (stream != NULL)
	{
		fputs(text, stream);
		fclose(stream);
	}

	gtk_widget_destroy(wDict);
}

// Parse keyboard binds
gboolean on_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	if (user_data) {};
		
	char str[1000];
	char *mkey;

	xneur_modifiers_to_string(event->state, str);

	mkey=g_strdup_printf ("%s%s%s",str,str[0] ? "+" : "",
		      (XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), event->hardware_keycode, 0)) != NULL)
		      ? XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),event->hardware_keycode, 0))
		      : g_strdup_printf ("%d",event->hardware_keycode));

	gtk_entry_set_text(GTK_ENTRY(widget), mkey);
	g_free(mkey);
		
	return FALSE;
}

gboolean on_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	if (user_data || event) {};
	guint i = 0;
	char **key_stat = g_strsplit(g_strdup(gtk_entry_get_text(GTK_ENTRY(widget))), "+", 4);

	while (key_stat[i])
		i++;

	if ((g_strcasecmp (key_stat[i-1],"Control")==0) || 
		(g_strcasecmp (key_stat[i-1],"Shift")==0) || 
		(g_strcasecmp (key_stat[i-1],"Alt")==0) || 
		(g_strcasecmp (key_stat[i-1],"Control_R")==0) || 
		(g_strcasecmp (key_stat[i-1],"Shift_R")==0) ||
		(g_strcasecmp (key_stat[i-1],"Alt_R")==0) ||
		(g_strcasecmp (key_stat[i-1],"Control_L")==0) || 
		(g_strcasecmp (key_stat[i-1],"Shift_L")==0) || 
		(g_strcasecmp (key_stat[i-1],"Alt_L")==0))
	{
		gtk_entry_set_text(GTK_ENTRY(widget),_("Press any key"));
	}
	
	return FALSE;
}

void on_window2_delete_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	if (widget || event || user_data) {};
	gtk_widget_destroy(widget);
	xneur_start();
}
