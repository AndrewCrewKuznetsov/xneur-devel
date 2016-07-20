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

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <string.h>
#include <stdlib.h>

#include <X11/XKBlib.h>

#include "misc.h"

#include "support.h"

#include "callbacks.h"


const char *conditions_names1[]				= {"contains", "begins", "ends", "coincides"};

FILE *stream = NULL;

// Save new regexp rule
void new_rule_to_treeview(GtkButton *button, gpointer user_data)
{
	if (button){};

	GtkBuilder* builder = ((xyz_t *) user_data)->x;
	// Remove leader and last space
	GtkWidget *widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "entry1"));
	char *letters = strtok((char *) gtk_entry_get_text(GTK_ENTRY(widgetPtrToBefound)), " ");
	printf("--->> '%s'\n", letters);
	if (letters == NULL)
	{
		GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));		gtk_widget_destroy(window);	
		printf("--->> 'DESTROY'\n");
		return;
	}

	GtkTreeIter iter;
	GtkListStore *store = ((xyz_t *) user_data)->y;
	gtk_list_store_append(GTK_LIST_STORE(store), &iter);

	gtk_list_store_set(GTK_LIST_STORE(store), &iter, 0, letters, -1);
	// write new rule
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton1"));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		gtk_list_store_set(GTK_LIST_STORE(store), &iter, 2, TRUE, -1);
	}
		
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "radiobutton1"));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		gtk_list_store_set(GTK_LIST_STORE(store), &iter, 1, _(conditions_names1[0]), -1);
	}

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "radiobutton2"));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		gtk_list_store_set(GTK_LIST_STORE(store), &iter, 1, _(conditions_names1[1]), -1);
	}

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "radiobutton3"));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		gtk_list_store_set(GTK_LIST_STORE(store), &iter, 1, _(conditions_names1[2]), -1);
	}

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "radiobutton4"));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		gtk_list_store_set(GTK_LIST_STORE(store), &iter, 1, _(conditions_names1[3]), -1);
	}
		
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));
	gtk_widget_destroy(window);
}	

void on_addbutton_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
	GError* error = NULL;
	GtkBuilder* builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, UI_FILE_RULE_ADD, &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));
	
	gtk_widget_show(window);

	xyz_t *ud = malloc(sizeof(xyz_t));
	ud->x = builder;
	ud->y = ((xyz_t *) user_data)->y;
	
	GtkWidget *widget= GTK_WIDGET(gtk_builder_get_object (builder, "button1"));
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (new_rule_to_treeview), ud);
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button2"));
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_cancelbutton_clicked), ud);
	
}

// Save new regexp rule
void edit_rule_to_treeview(GtkButton *button, gpointer user_data)
{
	if (button){};

	GtkBuilder* builder = ((xyz_t *) user_data)->x;
	// Remove leader and last space
	GtkWidget *widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "entry1"));
	char *letters = strtok((char *) gtk_entry_get_text(GTK_ENTRY(widgetPtrToBefound)), " ");
	if (letters == NULL)
	{
		GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));		gtk_widget_destroy(window);	
		return;
	}

	GtkWidget *treeview = GTK_WIDGET(gtk_builder_get_object (((xyz_t *) user_data)->z, "treeview1"));
	
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	GtkTreeIter iter;
	GtkTreeModel *store = GTK_TREE_MODEL(((xyz_t *) user_data)->y);
	gtk_tree_selection_get_selected(select, &store, &iter);

	gtk_list_store_set(GTK_LIST_STORE(store), &iter, 0, letters, -1);
	// write new rule
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton1"));
	gboolean insensitive = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	gtk_list_store_set(GTK_LIST_STORE(store), &iter, 2, insensitive, -1);
		
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "radiobutton1"));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		gtk_list_store_set(GTK_LIST_STORE(store), &iter, 1, _(conditions_names1[0]), -1);
	}

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "radiobutton2"));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		gtk_list_store_set(GTK_LIST_STORE(store), &iter, 1, _(conditions_names1[1]), -1);
	}

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "radiobutton3"));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		gtk_list_store_set(GTK_LIST_STORE(store), &iter, 1, _(conditions_names1[2]), -1);
	}

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "radiobutton4"));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		gtk_list_store_set(GTK_LIST_STORE(store), &iter, 1, _(conditions_names1[3]), -1);
	}
		
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));
	gtk_widget_destroy(window);
}			

void on_editbutton_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};

	GtkWidget *treeview = GTK_WIDGET(gtk_builder_get_object (((xyz_t *) user_data)->x, "treeview1"));
	
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL(((xyz_t *) user_data)->y);
	if (gtk_tree_selection_get_selected(select, &model, &iter))
	{
		gchar *letters;
		gchar *condition;
		gboolean insensitive;
		gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &letters, 
	                                                     1, &condition,
	                                                     2, &insensitive,
	                                                     -1);

		GError* error = NULL;
		GtkBuilder* builder = gtk_builder_new ();
		if (!gtk_builder_add_from_file (builder, UI_FILE_RULE_ADD, &error))
		{
			g_warning ("Couldn't load builder file: %s", error->message);
			g_error_free (error);
		}

		GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));

		GtkWidget *entry = GTK_WIDGET(gtk_builder_get_object (builder, "entry1"));
		gtk_entry_set_text(GTK_ENTRY(entry), letters);

		GtkWidget *checkbutton = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton1"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton), insensitive);
		
		if (strcmp((const char*)condition, _(conditions_names1[0])) == 0)
		{
			GtkWidget *radiobutton = GTK_WIDGET(gtk_builder_get_object (builder, "radiobutton1"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton), TRUE);
		}
		if (strcmp((const char*)condition, _(conditions_names1[1])) == 0)
		{
			GtkWidget *radiobutton = GTK_WIDGET(gtk_builder_get_object (builder, "radiobutton2"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton), TRUE);
		}
		if (strcmp((const char*)condition, _(conditions_names1[2])) == 0)
		{
			GtkWidget *radiobutton = GTK_WIDGET(gtk_builder_get_object (builder, "radiobutton3"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton), TRUE);
		}
		if (strcmp((const char*)condition, _(conditions_names1[3])) == 0)
		{
			GtkWidget *radiobutton = GTK_WIDGET(gtk_builder_get_object (builder, "radiobutton4"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton), TRUE);
		}	
		
		gtk_widget_show(window);

		xyz_t *ud = malloc(sizeof(xyz_t));
		ud->x = builder;
		ud->y = ((xyz_t *) user_data)->y;
		ud->z = ((xyz_t *) user_data)->x;
		
		GtkWidget *widget= GTK_WIDGET(gtk_builder_get_object (builder, "button1"));
		g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (edit_rule_to_treeview), ud);
		widget = GTK_WIDGET(gtk_builder_get_object (builder, "button2"));
		g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_cancelbutton_clicked), ud);
	}
}

void on_deletebutton_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};
	
	GtkWidget *treeview = GTK_WIDGET(gtk_builder_get_object (((xyz_t *) user_data)->x, "treeview1"));
	
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL(((xyz_t *) user_data)->y);
	if (gtk_tree_selection_get_selected(select, &model, &iter))
		gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
}

void on_extension_install_check(gpointer user_data)
{
	GtkWidget *checkbutton = (GtkWidget *)user_data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (checkbutton)))
	{
		char *buffer = _("To enable the extension you need to start the session again.");

		GtkWidget *dialog = gtk_message_dialog_new (NULL,
											GTK_DIALOG_DESTROY_WITH_PARENT,
											GTK_MESSAGE_INFO,
											GTK_BUTTONS_OK,
											"%s", buffer);
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	}
}

// Save dictionary/regexp list
static gboolean save_regexp(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};
	
	gchar *letters;
	gchar *condition;
	gboolean insensitive;
	gtk_tree_model_get(GTK_TREE_MODEL(model), iter, 0, &letters, 
	                                                1, &condition,
	                                                2, &insensitive,
	                                                -1);
	// write new rule
	if (insensitive)
	{
		fputs("(?i)", stream);
	}
		
	if (strcmp((const char*)condition, _(conditions_names1[0])) == 0)
	{
		fputs(letters, stream);
	}
	if (strcmp((const char*)condition, _(conditions_names1[1])) == 0)
	{
		fputs("^", stream);
		fputs(letters, stream);
	}
	if (strcmp((const char*)condition, _(conditions_names1[2])) == 0)
	{
		fputs(letters, stream);
		fputs("$", stream);
	}
	if (strcmp((const char*)condition, _(conditions_names1[3])) == 0)
	{
		fputs("^", stream);
		fputs(letters, stream);
		fputs("$", stream);
	}
	fputs("\n", stream);

	// end write
	if (letters != NULL)
		g_free(letters);
	if (condition != NULL)
		g_free(condition);

	return FALSE;
}

void on_okbutton_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};

	GtkWidget *entry = GTK_WIDGET(gtk_builder_get_object (((xyz_t *) user_data)->x, "entry10"));
	char *path_to_save = g_strdup_printf("%s", gtk_entry_get_text(GTK_ENTRY(entry)));

	stream = fopen(path_to_save, "w");
	if (stream != NULL)
	{
		gtk_tree_model_foreach(GTK_TREE_MODEL(((xyz_t *) user_data)->y), save_regexp, user_data);
		fclose(stream);
	}
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (((xyz_t *) user_data)->x, "dialog1"));
	gtk_widget_destroy(window);
	if (user_data!= NULL)
		g_free(user_data);
}

void on_cancelbutton_clicked(GtkButton *button, gpointer user_data)
{
	if (button){};

	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (((xyz_t *) user_data)->x, "dialog1"));
	gtk_widget_destroy(window);
	if (user_data != NULL)
		g_free(user_data);
}

// Parse keyboard binds
gboolean on_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	if (user_data){};
		
	char *string		= XKeysymToString(XkbKeycodeToKeysym(gdk_x11_get_default_xdisplay(), event->hardware_keycode, 0, 0));
	gchar *modifiers	= modifiers_to_string(event->state);
	gchar *keycode		= g_strdup_printf("%d", event->hardware_keycode);
	gchar *mkey		= g_strdup_printf("%s%s", modifiers, (string != NULL) ? string : keycode);

	gtk_entry_set_text(GTK_ENTRY(widget), mkey);

	if (mkey != NULL)
		g_free(mkey);
	if (keycode != NULL)
		g_free(keycode);
	if (modifiers != NULL)
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

void on_window2_delete_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	if (widget || event || user_data){};

	gtk_widget_destroy(widget);
	xneur_start();
}
