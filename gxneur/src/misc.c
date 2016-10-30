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

#include <glib/gstdio.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>

#include <xneur/xnconfig.h>
#include <xneur/list_char.h>
#include <xneur/xneur.h>

#define LANGUAGES_DIR "languages"
#define DIR_SEPARATOR		"/"

#define AUTOSTART_PATH ".config/autostart"  
#define AUTOSTART_FILE PACKAGE ".desktop"

#define GXNEUR_DESKTOP "[Desktop Entry]\nType=Application\nExec="  PACKAGE "\nHidden=false\nX-GNOME-Autostart-enabled=true\nName=GTK UI for X Neural Switcher\n"

#define GNOME3_EXT_PATH ".local/share/gnome-shell/extensions/" PACKAGE "@xneur.ru"  
#define GNOME3_EXT_JS_FILE "extension.js"
#define GNOME3_EXT_JSON_FILE "metadata.json"

#define DEFAULT_MAX_PATH	4096

#include "support.h"
#include "callbacks.h"
#include "trayicon.h"

#include "misc.h"

#include "configuration.h"

extern int arg_delay;
extern const char* arg_keyboard_properties;
extern const char* arg_show_in_the_tray;
extern const char* arg_rendering_engine;

void xneur_edit_regexp(GtkWidget *treeview);
void xneur_edit_rule(GtkWidget *treeview);
void xneur_edit_dictionary(GtkWidget *treeview);

#define MAX_LANGUAGES			4
#define XNEUR_NEEDED_MAJOR_VERSION	19
#define XNEUR_BUILD_MINOR_VERSION	0
	
struct _xneur_config *xconfig				= NULL;
	
static GtkListStore *store_exclude_app		= NULL;
static GtkListStore *store_auto_app			= NULL;
static GtkListStore *store_manual_app		= NULL;
static GtkListStore *store_layout_app		= NULL;
static GtkListStore *store_abbreviation		= NULL;
static GtkListStore *store_sound			= NULL;
static GtkListStore *store_osd				= NULL;
static GtkListStore *store_popup			= NULL;
static GtkListStore *store_user_action			= NULL;
static GtkListStore *store_action			= NULL;
static GtkListStore *store_autocompletion_exclude_app		= NULL;
static GtkListStore *store_dont_send_keyrelease_app		= NULL;
static GtkListStore *store_delay_send_key_app		= NULL;
static GtkListStore *store_plugin			= NULL;
static GtkListStore *store_language			= NULL;

static GtkWidget *tmp_widget	= NULL;

const char *conditions_names[]				= {"contains", "begins", "ends", "coincides"};

static const char *modifier_names[]			= {"Shift", "Control", "Alt", "Super"};
//static const char *all_modifiers[]			= {"Control", "Shift", "Alt", "Super", "Control_R", "Shift_R", "Alt_R", "Super_R", "Control_L", "Shift_L", "Alt_L", "Super_L"};

static const char *notify_names[]			=   {
										"Xneur started", "Xneur reloaded", "Xneur stopped",
										"Keypress on layout 1", "Keypress on layout 2", "Keypress on layout 3",
										"Keypress on layout 4", "Switch to layout 1", "Switch to layout 2",
										"Switch to layout 3", "Switch to layout 4", "Correct word automatically",
										"Correct last word manually", "Transliterate last word manually", "Change case of last word manually", "Preview correction of last word manually", 
										"Correct last line", 
										"Correct selected text", "Transliterate selected text", "Change case of selected text", "Preview correction of selected text",
										"Correct clipboard text", "Transliterate clipboard text", "Change case of clipboard text", "Preview correction of clipboard text",
	                                    "Expand abbreviations",
										"Correct aCCIDENTAL caps", "Correct TWo INitial caps", "Correct two space with a comma and a space",
										"Correct two minus with a dash", "Correct (c) with a copyright sign", "Correct (tm) with a trademark sign",
										"Correct (r) with a registered sign", "Correct three points with a ellipsis sign", 
										"Correct misprint",
										"Execute user action", "Block keyboard and mouse events", "Unblock keyboard and mouse events"
										};

static const char *action_names[]			=   {
										"Correct/Undo correction", "Transliterate", "Change case", "Preview correction", 
										"Correct last line",  
										"Correct selected text", "Transliterate selected text", "Change case of selected text", "Preview correction of selected text",
	                                    "Correct clipboard text", "Transliterate clipboard text", "Change case of clipboard text", "Preview correction of clipboard text",
										"Switch to layout 1", "Switch to layout 2", "Switch to layout 3", "Switch to layout 4",
		                                "Rotate layouts", "Rotate layouts back", "Expand abbreviations", "Autocompletion confirmation", 
										"Rotate autocompletion", "Block/Unblock keyboard and mouse events", "Insert date"
                                        };

static const int total_notify_names = sizeof(notify_names) / sizeof(notify_names[0]);

static const int total_modifiers			= sizeof(modifier_names) / sizeof(modifier_names[0]); 
//static const int total_all_modifiers			= sizeof(all_modifiers) / sizeof(all_modifiers[0]);

void error_msg(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	
	char *buffer = (char *) malloc(sizeof(char)*1024*10);
	bzero(buffer, sizeof(char)*1024*10); 
	vsprintf(buffer, msg, ap);

	GtkWidget *dialog = gtk_message_dialog_new (NULL,
											GTK_DIALOG_DESTROY_WITH_PARENT,
											GTK_MESSAGE_ERROR,
											GTK_BUTTONS_CLOSE,
											"%s", buffer);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);	

	if (buffer != NULL)
		g_free(buffer);
	va_end(ap);
}

static char* concat_bind(int action)
{
	char *text = (char *) malloc((24 + 1 + strlen(xconfig->actions[action].hotkey.key)) * sizeof(char));
	text[0] = '\0';

	for (int i = 0; i < total_modifiers; i++)
	{
		if ((xconfig->actions[action].hotkey.modifiers & (0x1 << i)) == 0)
			continue;

		strcat(text, modifier_names[i]);
		strcat(text, "+");
	}

	strcat(text, xconfig->actions[action].hotkey.key);
	
	return text;
}

/*static void split_bind(char *text, int action)
{
	char **key_stat = g_strsplit(text, "+", 4);

	int last = is_correct_hotkey(key_stat);
	if (last == -1)
	{
		g_strfreev(key_stat);
		return;
	}
	
	xconfig->actions[action].hotkey.modifiers = 0;
	
	for (int i = 0; i <= last; i++)
	{
		int assigned = FALSE;
		for (int j = 0; j < total_modifiers; j++) 
 		{ 
			if (g_utf8_collate (g_utf8_casefold(key_stat[i], strlen (key_stat[i])), 
			                    g_utf8_casefold(modifier_names[j], strlen (modifier_names[j]))) != 0) 
				continue; 

			assigned = TRUE;
			xconfig->actions[action].hotkey.modifiers |= (0x1 << j); 
			break; 
		} 

		if (assigned == FALSE)
			xconfig->actions[action].hotkey.key = strdup(key_stat[i]); 
	}
 
	g_strfreev(key_stat);
}*/

static void get_xprop_name(GtkBuilder* builder)
{
	FILE *fp = popen("xprop WM_CLASS", "r");
	if (fp == NULL)
		return;

	char buffer[NAME_MAX];
	if (fgets(buffer, NAME_MAX, fp) == NULL)
		return;
	
	if (pclose(fp))
		return;

	const char* cap = "WM_CLASS(STRING)";
	if (strncmp(buffer, cap, strlen(cap)) != 0)
		return;

	char* p = strstr(buffer, "\", \"");
	if (p == NULL)
		return;

	p += 4;

	int len = strlen(p);
	if (len < 2)
		return;

	p[len - 2] = '\0';

	GtkWidget *entry1 = GTK_WIDGET(gtk_builder_get_object (builder, "entry1"));
	gtk_entry_set_text(GTK_ENTRY(entry1), p);
}

void xneur_get_logfile()
{
	char *log_home_path	= xconfig->get_home_dict_path(NULL, "xneurlog.html");
	char *command = malloc ((strlen("xdg-open ") + strlen(log_home_path) + strlen(" 2> /dev/stdout") + 1) * sizeof(char));
	command[0] = '\0';
	strcat(command, "xdg-open ");
	strcat(command, log_home_path);
	strcat(command, " 2> /dev/stdout");

	FILE *fp = popen(command, "r");
	if (log_home_path != NULL)
		g_free(log_home_path);
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

static void xneur_insert_application(GtkBuilder* builder)
{
	GtkWidget *entry1 = GTK_WIDGET(gtk_builder_get_object (builder, "entry1"));

	if (gtk_entry_get_text(GTK_ENTRY(entry1)) != NULL)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(tmp_widget), &iter);
		gtk_list_store_set(GTK_LIST_STORE(tmp_widget), &iter, 0, gtk_entry_get_text(GTK_ENTRY(entry1)), -1);
	}

	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));
	gtk_widget_destroy(window);
}

static void xneur_add_application(GtkListStore *store)
{
	GError* error = NULL;
	GtkBuilder* builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, UI_FILE_APP_ADD, &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}
	gtk_builder_connect_signals(builder, NULL);
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));

	gtk_widget_show(window);

	GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object (builder, "button3"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(get_xprop_name), builder);

	// Button OK
	tmp_widget = GTK_WIDGET(store);
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button2"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_insert_application), builder);
	
	if (store) {};
}

static void remove_item(GtkWidget *treeview, GtkListStore *store)
{
	GtkTreeModel *model = GTK_TREE_MODEL(store);
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(select, &model, &iter))
		gtk_list_store_remove(GTK_LIST_STORE(store), &iter);
}

static void save_list(GtkListStore *store, struct _list_char *list, GtkTreeIter *iter)
{
	gchar *ptr;
	gtk_tree_model_get(GTK_TREE_MODEL(store), iter, 0, &ptr, -1);
	
	list->add(list, ptr);

	if (ptr != NULL)
		g_free(ptr);
}

static void init_libxnconfig(void)
{
	if (xconfig != NULL)
		return;

	// Init configuration
	xconfig = xneur_config_init();
				
	int major_version, minor_version;
	xconfig->get_library_version(&major_version, &minor_version);

	if ((major_version != XNEUR_NEEDED_MAJOR_VERSION) || (minor_version != XNEUR_BUILD_MINOR_VERSION))
	{
		error_msg(_("Wrong XNeur configuration library api version.\nPlease, install libxnconfig version 0.%d.%d\n"), XNEUR_NEEDED_MAJOR_VERSION, XNEUR_BUILD_MINOR_VERSION);
		printf(_("Wrong XNeur configuration library api version.\nPlease, install libxnconfig version 0.%d.%d\n"), XNEUR_NEEDED_MAJOR_VERSION, XNEUR_BUILD_MINOR_VERSION);
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}

	//error_msg(_("Using libxnconfig API version 0.%d.%d (build with %d.%d)\n"), major_version, minor_version, XNEUR_NEEDED_MAJOR_VERSION, XNEUR_BUILD_MINOR_VERSION);
	printf(_("Using libxnconfig API version 0.%d.%d (build with 0.%d.%d)\n"), major_version, minor_version, XNEUR_NEEDED_MAJOR_VERSION, XNEUR_BUILD_MINOR_VERSION);

	if (!xconfig->load(xconfig))
	{
		error_msg(_("XNeur's config broken or was created with old version!\nPlease, remove ~/.xneur/. It should solve the problem!\nIf you don't want to loose your configuration, back it up\nand manually patch new configuration which will be created after first run.\n"));
		printf(_("XNeur's config broken or was created with old version!\nPlease, remove ~/.xneur/. It should solve the problem!\nIf you don't want to loose your configuration, back it up\nand manually patch new configuration which will be created after first run.\n"));
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}
}

void xneur_start(void)
{
	init_libxnconfig();

	if (!g_spawn_command_line_async("xneur", NULL))
	{
		error_msg(_("Couldn't start %s\nVerify that it installed\n"), "xneur");
		fprintf(stderr, _("Couldn't start %s\nVerify that it installed\n"), "xneur");
	}
}

void xneur_about(void)
{
	GError* error = NULL;
	GtkBuilder* builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, UI_FILE_ABOUT, &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}
	gtk_builder_connect_signals(builder, NULL);
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "window1"));

	GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object (builder, "label44"));
	gchar *text = g_strdup_printf("%s %s", _("Current Version"), VERSION);
	gtk_label_set_text(GTK_LABEL(widget), text);	

	gtk_widget_show(window);
}

static void column_0_edited (GtkCellRendererText *renderer, gchar *path, gchar *new_text, GtkTreeView *treeview)
{
	if (renderer) {};
	GtkTreeIter iter;
	GtkTreeModel *model;

	model = gtk_tree_view_get_model (treeview);
	if (gtk_tree_model_get_iter_from_string (model, &iter, path))
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, new_text, -1);
}

static void column_1_edited (GtkCellRendererText *renderer, gchar *path, gchar *new_text, GtkTreeView *treeview)
{
	if (renderer) {};
	GtkTreeIter iter;
	GtkTreeModel *model;

	model = gtk_tree_view_get_model (treeview);
	if (gtk_tree_model_get_iter_from_string (model, &iter, path))
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, 1, new_text, -1);
}

static void column_2_edited (GtkCellRendererText *renderer, gchar *path, gchar *new_text, GtkTreeView *treeview)
{
	if (renderer) {};
	GtkTreeIter iter;
	GtkTreeModel *model;

	model = gtk_tree_view_get_model (treeview);
	if (gtk_tree_model_get_iter_from_string (model, &iter, path))
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, 2, new_text, -1);
}
static void plug_enable (GtkCellRendererToggle *renderer, gchar *path, GtkTreeView *treeview)
{
	GtkTreeIter iter;
	GtkTreeModel *model;

	gboolean value = !gtk_cell_renderer_toggle_get_active (renderer);

	model = gtk_tree_view_get_model (treeview);
	if (gtk_tree_model_get_iter_from_string (model, &iter, path))
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, value, -1);
}

static void notify_enable (GtkCellRendererToggle *renderer, gchar *path, GtkTreeView *treeview)
{
	GtkTreeIter iter;
	GtkTreeModel *model;

	gboolean value = !gtk_cell_renderer_toggle_get_active (renderer);

	model = gtk_tree_view_get_model (treeview);
	if (gtk_tree_model_get_iter_from_string (model, &iter, path))
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, 2, value, -1);
}

static void xneur_restore_keyboard_properties(GtkBuilder* builder)
{
	GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object (builder, "entry5"));

	gtk_entry_set_text(GTK_ENTRY(widget), DEFAULT_KEYBOARD_PROPERTIES);
}

static void xneur_replace_keyboard_properties(GtkBuilder* builder)
{
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "filechooserdialog1"));

	gtk_entry_set_text(GTK_ENTRY(tmp_widget), gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (window)));
	
	gtk_widget_destroy(window);
}	

static void xneur_edit_icons_directory(GtkBuilder* parent_builder)
{
	if (parent_builder) {};
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
	gint res;

	dialog = gtk_file_chooser_dialog_new ("Open Directory",
		                                  NULL,
		                                  action,
		                                  _("_Cancel"),
		                                  GTK_RESPONSE_CANCEL,
		                                  _("_Open"),
		                                  GTK_RESPONSE_ACCEPT,
		                                  NULL);

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	  {
		char *dirname;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		dirname = gtk_file_chooser_get_filename (chooser);
		tmp_widget = GTK_WIDGET(gtk_builder_get_object (parent_builder, "entry1"));
		gtk_entry_set_text(GTK_ENTRY(tmp_widget), dirname);
		if (dirname != NULL)
			g_free (dirname);
	  }

	gtk_widget_destroy (dialog);
}

static void xneur_edit_keyboard_properties(GtkBuilder* parent_builder)
{
	GError* error = NULL;
	GtkBuilder* builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, UI_FILE_CHOOSE, &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}
	gtk_builder_connect_signals(builder, NULL);
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "filechooserdialog1"));

	tmp_widget = GTK_WIDGET(gtk_builder_get_object (parent_builder, "entry5"));
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(window), gtk_entry_get_text(GTK_ENTRY(tmp_widget)));
		
	gtk_widget_show(window);
		
	// Button OK
	GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object (builder, "button5"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_replace_keyboard_properties), builder);
}

void xneur_kb_preference(void)
{
	gchar *string_value = NULL;
	gxneur_config_read_str("keyboard_properties", &string_value);
	if (arg_keyboard_properties) {
		if (string_value != NULL)
			g_free(string_value);
		string_value = g_strdup(arg_keyboard_properties);
	}

	gchar *cmd = string_value ? string_value : DEFAULT_KEYBOARD_PROPERTIES;

	if (!g_spawn_command_line_async(cmd, NULL))
	{
		error_msg(_("Couldn't start %s\nVerify that it installed\n"), cmd);
		fprintf(stderr, _("Couldn't start %s\nVerify that it installed\n"), cmd);
	}
	
	if (string_value != NULL)
		g_free(string_value);
}

void xneur_preference(void)
{
	GError* error = NULL;
	GtkBuilder* builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, UI_FILE_CONFIG, &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}
	gtk_builder_connect_signals(builder, NULL);
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "window2"));

	gtk_widget_show(window);

	// Mode set
	GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton7"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->manual_mode);

	// Exclude App set
	GtkWidget *treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview1"));

	store_exclude_app = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_exclude_app));
	gtk_widget_show(treeview);

	for (int i = 0; i < xconfig->excluded_apps->data_count; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_exclude_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_exclude_app), &iter, 0, xconfig->excluded_apps->data[i].string, -1);
	}

	GtkCellRenderer *cell = gtk_cell_renderer_text_new();

	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	// Adding/Removing Exclude App
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button2"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_exclude_app), G_OBJECT(window));

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button3"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_exclude_app), G_OBJECT(treeview));

	// Auto App Set
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview2"));

	store_auto_app = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_auto_app));
	gtk_widget_show(treeview);

	for (int i = 0; i < xconfig->auto_apps->data_count; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_auto_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_auto_app), &iter, 0, xconfig->auto_apps->data[i].string, -1);
	}

	cell = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes(_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	// Adding/Removing Auto App
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button19"));

	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_auto_app), G_OBJECT(window));
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button20"));

	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_auto_app), G_OBJECT(treeview));

	// Manual App Set
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview3"));
	
	store_manual_app = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_manual_app));
	gtk_widget_show(treeview);	

	for (int i = 0; i < xconfig->manual_apps->data_count; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_manual_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_manual_app), &iter, 0, xconfig->manual_apps->data[i].string, -1);
	}				

	cell = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes(_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	// Adding/Removing Manual App
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button21"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_manual_app), G_OBJECT(window));

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button22"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_manual_app), G_OBJECT(treeview));

	
	// DontSendKeyRelease App Set
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview14"));
	
	store_dont_send_keyrelease_app = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_dont_send_keyrelease_app));
	gtk_widget_show(treeview);	

	for (int i = 0; i < xconfig->dont_send_key_release_apps->data_count; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_dont_send_keyrelease_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_dont_send_keyrelease_app), &iter, 0, xconfig->dont_send_key_release_apps->data[i].string, -1);
	}				

	cell = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes(_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	// Adding/Removing DontSendKeyRelease App
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button7"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_dont_send_keyrelease_app), G_OBJECT(window));

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button9"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_dont_send_keyrelease_app), G_OBJECT(treeview));
	
	
	// Layout Remember App Set
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview4"));
	
	store_layout_app = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_layout_app));
	gtk_widget_show(treeview);	

	for (int i = 0; i < xconfig->layout_remember_apps->data_count; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_layout_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_layout_app), &iter, 0, xconfig->layout_remember_apps->data[i].string, -1);
	}				

	cell = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes(_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	// Adding/Removing Layour Remember App
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button27"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_layout_app), G_OBJECT(window));

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button28"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_layout_app), G_OBJECT(treeview));
	
	// Languages
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview13"));

	store_language = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_language));
	gtk_widget_show(treeview);

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Description"), cell, "text", 0, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Layout"), cell, "text", 1, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	cell = gtk_cell_renderer_toggle_new();
	column = gtk_tree_view_column_new_with_attributes(_("Excluded"), cell, "active", 2, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));
	g_object_set (cell, "activatable", TRUE, NULL);
	g_signal_connect (G_OBJECT (cell), "toggled",
						G_CALLBACK (notify_enable),
						(gpointer) treeview);
	
	for (int lang = 0; lang < xconfig->handle->total_languages && lang < MAX_LANGUAGES; lang++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_language), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_language), &iter, 
												0, xconfig->handle->languages[lang].name,
												1, xconfig->handle->languages[lang].dir,
		    									2, xconfig->handle->languages[lang].excluded,
												-1);
	}

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button6"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_edit_dictionary), G_OBJECT(treeview));

	// Default Layout Group
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "combobox25"));
	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), xconfig->default_group);
	
	// Education Mode
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton2"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->educate);

	// Check similar words
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton43"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->check_similar_words);
	
	// Layout Remember Mode
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton3"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->remember_layout);
	
	// Saving Selection Mode
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton4"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->save_selection_after_convert);

	// Rotate Layout after correction selected text Mode
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton30"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->rotate_layout_after_convert);
	
	// Sound Playing Mode
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton5"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->play_sounds);
	// Volume Percent
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "spinbutton3"));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), xconfig->volume_percent);

	// Logging Keyboard Mode
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton6"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->save_keyboard_log);

	// Log size
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "spinbutton2"));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), xconfig->size_keyboard_log);

	// Log send to e-mail
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "entry3"));
	if (xconfig->mail_keyboard_log != NULL)
		gtk_entry_set_text(GTK_ENTRY(widget), xconfig->mail_keyboard_log);
	
	// Log send via host
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "entry4"));
	if (xconfig->host_keyboard_log != NULL)
		gtk_entry_set_text(GTK_ENTRY(widget), xconfig->host_keyboard_log);

	// Log port
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "spinbutton4"));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), xconfig->port_keyboard_log);

	// View log
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button8"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_get_logfile), NULL);
	
	// Ignore Keyboard Layout Mode
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton8"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->abbr_ignore_layout);

	// Check language on input process 
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton18"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->check_lang_on_process);

	// Disable CapsLock use 
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton19"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->disable_capslock);

	// 
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton20"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->correct_space_with_punctuation);

	// 
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton31"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->correct_capital_letter_after_dot);

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton29"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->correct_two_space_with_comma_and_space);

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton37"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->correct_two_minus_with_dash);

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton41"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->correct_dash_with_emdash);

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton38"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->correct_c_with_copyright);

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton39"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->correct_tm_with_trademark);

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton40"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->correct_r_with_registered);

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton42"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->correct_three_points_with_ellipsis);

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton44"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->correct_misprint);
	
	// Autocompletion
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton21"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->autocompletion);

	// Space after autocompletion
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton1"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->add_space_after_autocompletion);

	// Exclude autocompletion App set
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview8"));

	store_autocompletion_exclude_app = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_autocompletion_exclude_app));
	gtk_widget_show(treeview);

	for (int i = 0; i < xconfig->autocompletion_excluded_apps->data_count; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_autocompletion_exclude_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_autocompletion_exclude_app), &iter, 0, xconfig->autocompletion_excluded_apps->data[i].string, -1);
	}

	cell = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes(_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	// Adding/Removing Exclude App
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button11"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_autocompletion_exclude_app), G_OBJECT(window));

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button13"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_autocompletion_exclude_app), G_OBJECT(treeview));

	// Hotkeys List set
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview5"));

	store_action = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_action));
	gtk_widget_show(treeview);

	// Standart Actions
	for (int action = 0; action < xconfig->actions_count; action++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_action), &iter);

		char *binds = "";
		if (xconfig->actions[action].hotkey.key != NULL)
			binds = concat_bind(action);

		gtk_list_store_set(GTK_LIST_STORE(store_action), &iter, 
												0, _(action_names[xconfig->actions[action].action]),
												1, binds, 
												-1);
	}

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Action"), cell, "text", 0, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column), GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(column), 400);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));
	
	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Key bind"), cell, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	g_signal_connect (G_OBJECT (treeview), "row-activated",
						G_CALLBACK (xneur_edit_action),
						(gpointer) treeview);
	
	// Button Edit Action
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button10"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_edit_action), G_OBJECT(treeview));

	// Button Clear Action
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button1"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_clear_action), G_OBJECT(treeview));
	
	// Abbreviations List set
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview6"));

	store_abbreviation = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_abbreviation));
	gtk_widget_show(treeview);

	for (int i = 0; i < xconfig->abbreviations->data_count; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_abbreviation), &iter);
		char *string		= strdup(xconfig->abbreviations->data[i].string);
		char *replacement	= strsep(&string, " ");
		gtk_list_store_set(GTK_LIST_STORE(store_abbreviation), &iter, 
												0, replacement,
												1, string, 
												-1);
		if (replacement != NULL)
			g_free(replacement);
	}

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Abbreviation"), cell, "text", 0, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	g_object_set (cell, "editable", TRUE, "editable-set", TRUE, NULL);
	g_signal_connect (G_OBJECT (cell), "edited",
						G_CALLBACK (column_0_edited),
						(gpointer) treeview);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Expansion text"), cell, "text", 1, NULL);
	g_object_set (cell, "editable", TRUE, "editable-set", TRUE, NULL);
	g_signal_connect (G_OBJECT (cell), "edited",
						G_CALLBACK (column_1_edited),
						(gpointer) treeview);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	// Button Add Abbreviation
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button32"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_abbreviation), G_OBJECT(treeview));
	
	// Button Remove Abbreviation
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button33"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_abbreviation), G_OBJECT(treeview));

	// Sound Paths Preference
	// Sound List set
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview7"));

	store_sound = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_sound));
	gtk_widget_show(treeview);

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Action"), cell, "text", 0, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Sound"), cell, "text", 1, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	g_object_set (cell, "editable", TRUE, "editable-set", TRUE, NULL);
	g_signal_connect (G_OBJECT (cell), "edited",
						G_CALLBACK (column_1_edited),
						(gpointer) treeview);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	cell = gtk_cell_renderer_toggle_new();
	column = gtk_tree_view_column_new_with_attributes(_("Enabled"), cell, "active", 2, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));
	g_object_set (cell, "activatable", TRUE, NULL);
	g_signal_connect (G_OBJECT (cell), "toggled",
						G_CALLBACK (notify_enable),
						(gpointer) treeview);

	for (int i = 0; i < total_notify_names; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_sound), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_sound), &iter, 
												0, _(notify_names[i]),
												1, xconfig->sounds[i].file,
		    									2, xconfig->sounds[i].enabled, 
												-1);
	}

	g_signal_connect (G_OBJECT (treeview), "row-activated",
						G_CALLBACK (xneur_edit_sound),
						(gpointer) treeview);
	                  
	// Button Edit Sound
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button12"));
	tmp_widget = GTK_WIDGET(treeview);
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_edit_sound), G_OBJECT(treeview));

	// Delay Before Send
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "spinbutton1"));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), xconfig->send_delay);

	// Delay App Set
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview15"));
	
	store_delay_send_key_app = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_delay_send_key_app));
	gtk_widget_show(treeview);	

	for (int i = 0; i < xconfig->delay_send_key_apps->data_count; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_delay_send_key_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_delay_send_key_app), &iter, 0, xconfig->delay_send_key_apps->data[i].string, -1);
	}				

	cell = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes(_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	// Adding/Removing Delay App
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button14"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_delay_send_key_app), G_OBJECT(window));

	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button17"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_delay_send_key_app), G_OBJECT(treeview));
	
	// Log Level
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "combobox1"));
	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), xconfig->log_level);
	
	// Correct two capital letter mode
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton10"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->correct_two_capital_letter);

	// Correct iNCIDENTAL CapsLock mode
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton9"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->correct_incidental_caps);

	// Flush internal buffer when pressed Escape mode
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton32"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->flush_buffer_when_press_escape);

	// Flush internal buffer when pressed Enter mode
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton11"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->flush_buffer_when_press_enter);

	// Compatibility with completion
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton33"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->compatibility_with_completion);
	
	// User Actions List set
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview9"));

	store_user_action = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_user_action));
	gtk_widget_show(treeview);

	for (int action = 0; action < xconfig->user_actions_count; action++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_user_action), &iter);
		
		char *text = (char *) malloc((24 + 1 + strlen(xconfig->user_actions[action].hotkey.key)) * sizeof(char));
		text[0] = '\0';

		for (int i = 0; i < total_modifiers; i++)
		{
			if ((xconfig->user_actions[action].hotkey.modifiers & (0x1 << i)) == 0)
				continue;

			strcat(text, modifier_names[i]);
			strcat(text, "+");
		}

		strcat(text, xconfig->user_actions[action].hotkey.key);
		
		gtk_list_store_set(GTK_LIST_STORE(store_user_action), &iter, 
												0, xconfig->user_actions[action].name,
												1, text,
												2, xconfig->user_actions[action].command,
												-1);
		if (text != NULL)
			g_free(text);
	}

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Action"), cell, "text", 0, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	g_object_set (cell, "editable", TRUE, "editable-set", TRUE, NULL);
	g_signal_connect (G_OBJECT (cell), "edited",
						G_CALLBACK (column_0_edited),
						(gpointer) treeview);
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column), GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(column), 200);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Key bind"), cell, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("User action"), cell, "text", 2, NULL);
	g_object_set (cell, "editable", TRUE, "editable-set", TRUE, NULL);
	g_signal_connect (G_OBJECT (cell), "edited",
						G_CALLBACK (column_2_edited),
						(gpointer) treeview);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column), GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(column), 200);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	g_signal_connect (G_OBJECT (treeview), "row-activated",
						G_CALLBACK (xneur_edit_user_action),
						(gpointer) treeview);
	
	// Button Add User Action
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button36"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_user_action), G_OBJECT(treeview));
	
	// Button Remove User Action
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button37"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_user_action), G_OBJECT(treeview));

	// Button Edit User Action
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button38"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_edit_user_action), G_OBJECT(treeview));
			
	// Show OSD
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton13"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->show_osd);
	
	// OSD Text Preference
	// OSD List set
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview10"));

	store_osd = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_osd));
	gtk_widget_show(treeview);

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Action"), cell, "text", 0, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("OSD text"), cell, "text", 1, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	g_object_set (cell, "editable", TRUE, "editable-set", TRUE, NULL);
	g_signal_connect (G_OBJECT (cell), "edited",
						G_CALLBACK (column_1_edited),
						(gpointer) treeview);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	cell = gtk_cell_renderer_toggle_new();
	column = gtk_tree_view_column_new_with_attributes(_("Enabled"), cell, "active", 2, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));
	g_object_set (cell, "activatable", TRUE, NULL);
	g_signal_connect (G_OBJECT (cell), "toggled",
						G_CALLBACK (notify_enable),
						(gpointer) treeview);
	
	for (int i = 0; i < total_notify_names; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_osd), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_osd), &iter, 
												0, _(notify_names[i]),
												1, xconfig->osds[i].file,
		    									2, xconfig->osds[i].enabled,
												-1);
	}
	
	// OSD Font
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "entry2"));
	gtk_entry_set_text(GTK_ENTRY(widget), xconfig->osd_font);

	// Show popup
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton22"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->show_popup);

	// Popup expiration delay
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "spinbutton6"));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), xconfig->popup_expire_timeout);
	
	// Popup Text Preference
	// Popup List set
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview12"));

	store_popup = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_popup));
	gtk_widget_show(treeview);
	
	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Action"), cell, "text", 0, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Popup message text"), cell, "text", 1, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	g_object_set (cell, "editable", TRUE, "editable-set", TRUE, NULL);
	g_signal_connect (G_OBJECT (cell), "edited",
						G_CALLBACK (column_1_edited),
						(gpointer) treeview);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	cell = gtk_cell_renderer_toggle_new();
	column = gtk_tree_view_column_new_with_attributes(_("Enabled"), cell, "active", 2, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));
	g_object_set (cell, "activatable", TRUE, NULL);
	g_signal_connect (G_OBJECT (cell), "toggled",
						G_CALLBACK (notify_enable),
						(gpointer) treeview);

	for (int i = 0; i < total_notify_names; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_popup), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_popup), &iter, 
												0, _(notify_names[i]),
												1, xconfig->popups[i].file, 
		    									2, xconfig->popups[i].enabled,
												-1);
	}

	// Troubleshooting
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton14"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->troubleshoot_backspace);
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton15"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->troubleshoot_left_arrow);
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton16"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->troubleshoot_right_arrow);
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton17"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->troubleshoot_up_arrow);
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton23"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->troubleshoot_down_arrow);
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton24"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->troubleshoot_delete);
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton25"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->troubleshoot_switch);
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton35"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->troubleshoot_full_screen);
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton12"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->troubleshoot_enter);
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton45"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->troubleshoot_tab);
	
	// Tracking input mode set
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton34"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->tracking_input);

	// Tracking mouse mode set
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton28"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->tracking_mouse);
	
	// Plugins
	treeview = GTK_WIDGET(gtk_builder_get_object (builder, "treeview11"));
	store_plugin = gtk_list_store_new(3, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_plugin));
	gtk_widget_show(treeview);

	cell = gtk_cell_renderer_toggle_new();
	column = gtk_tree_view_column_new_with_attributes(_("Enabled"), cell, "active", 0, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));
	g_object_set (cell, "activatable", TRUE, NULL);
	g_signal_connect (G_OBJECT (cell), "toggled",
						G_CALLBACK (plug_enable),
						(gpointer) treeview);
	
	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Description"), cell, "text", 1, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));
	
	cell = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("File name"), cell, "text", 2, NULL);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));
	
	DIR *dp;
	struct dirent *ep;

	dp = opendir (XNEUR_PLUGIN_DIR);
	if (dp != NULL)
	{
		ep = readdir (dp);
		while (ep)
		{
			if (strncmp(ep->d_name + strlen(ep->d_name) - 3, ".so", 3) != 0)
			{
				ep = readdir (dp);
				continue;
			}

			size_t len = strlen(XNEUR_PLUGIN_DIR) + strlen(ep->d_name) + 2;
			char * plugin_file = malloc(sizeof(char)*len);
			snprintf(plugin_file, len, "%s/%s", XNEUR_PLUGIN_DIR, ep->d_name);
			void *module = dlopen(plugin_file, RTLD_NOW);
			if (plugin_file != NULL)
				g_free(plugin_file);
			if(!module)
			{
				ep = readdir (dp);
				continue;
			}

			gboolean state = FALSE;
			for (int i = 0; i < xconfig->plugins->data_count; i++)
			{
				if (strcmp(xconfig->plugins->data[i].string, ep->d_name) == 0)
				{
					state = TRUE;
					break;
				}
			}

			char* mod_info = "";
			char* (*module_info)(void);
			module_info = dlsym(module, "on_plugin_info");
			if (module_info != NULL) 
			{
				mod_info = (gchar *)module_info();
			}
			
			GtkTreeIter iter;
			gtk_list_store_append(GTK_LIST_STORE(store_plugin), &iter);
			gtk_list_store_set(GTK_LIST_STORE(store_plugin), &iter, 
												0, state,
												1, mod_info,
												2, ep->d_name,
												-1);

			ep = readdir (dp);
			dlclose(module);
		}
 		(void) closedir (dp);
	}

	// Gxneur Properties

	// Autostart
	gchar *path_file = g_build_filename(getenv("HOME"), AUTOSTART_PATH, AUTOSTART_FILE, NULL);
	FILE *stream = fopen(path_file, "r");
	if (path_file != NULL)
		g_free(path_file);
	if (stream != NULL)
	{
		fclose(stream);
		widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton27"));
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
	}

	// Gnome 3 shell
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton36"));
	path_file = g_build_filename(getenv("HOME"), GNOME3_EXT_PATH, NULL);
	if (g_file_test(path_file, G_FILE_TEST_IS_DIR))
	{
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
	}
	if (path_file != NULL)
		g_free(path_file);
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(on_extension_install_check), G_OBJECT(widget));
	
	// Delay before start
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "spinbutton5"));
	int value = DEFAULT_DELAY;
	if (gxneur_config_read_int("delay", &value) == CONFIG_NOT_SUPPORTED)
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);
	if (arg_delay >= 0)
		value = arg_delay;
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), value);

	// Show on tray
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "combobox2"));
	int show_in_the_tray = 0;
	gchar *string_value = NULL;
	if (gxneur_config_read_str("show_in_the_tray", &string_value) == CONFIG_NOT_SUPPORTED)
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);
	if (arg_show_in_the_tray) {
		if (string_value != NULL)
			g_free(string_value);
		string_value = g_strdup(arg_show_in_the_tray);
	}
	if (!string_value)
		string_value = g_strdup(DEFAULT_SHOW_IN_THE_TRAY);
	if (strcasecmp(string_value, "Flag") == 0)
		show_in_the_tray = 0;
	else if (strcasecmp(string_value, "Text") == 0)
		show_in_the_tray = 1;
	else if (strcasecmp(string_value, "Icon") == 0)
		show_in_the_tray = 2;
	else
		show_in_the_tray = 3;
	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), show_in_the_tray);
	if (string_value != NULL)
		g_free(string_value);

	// Define icons directory
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "entry1"));
	int icons_directory_config_not_supported = 0;
	string_value = NULL;
	if (gxneur_config_read_str("icons_directory", &string_value) == CONFIG_NOT_SUPPORTED)
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE), icons_directory_config_not_supported = 1;
	gtk_entry_set_text(GTK_ENTRY(widget), string_value ? string_value : "");
	if (string_value != NULL)
		g_free(string_value);

	// Button Icons Directory Edit
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button18"));
	if (!icons_directory_config_not_supported)
		g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_edit_icons_directory), builder);
	else
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);
	
	// Define rendering engine
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "combobox3"));
	int rendering_engine = 0;
	string_value = NULL;
	if (gxneur_config_read_str("rendering_engine", &string_value) == CONFIG_NOT_SUPPORTED)
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);
	
	if (arg_rendering_engine) {
		if (string_value != NULL)
			g_free(string_value);
		string_value = g_strdup(arg_rendering_engine);
	}
	if (!string_value)
		string_value = g_strdup(DEFAULT_RENDERING_ENGINE);
	if (strcasecmp(string_value, "Built-in") == 0)
		rendering_engine = 0;
	else if (strcasecmp(string_value, "StatusIcon") == 0)
		rendering_engine = 1;
	else
		rendering_engine = 2;
	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), rendering_engine);
	
	// Keyboard properties
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "entry5"));
	int keyboard_properties_config_not_supported = 0;
	string_value = NULL;
	if (gxneur_config_read_str("keyboard_properties", &string_value) == CONFIG_NOT_SUPPORTED)
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE),
		keyboard_properties_config_not_supported = 1;
	if (arg_keyboard_properties) {
		if (string_value != NULL)
			g_free(string_value);
		string_value = g_strdup(arg_keyboard_properties);
	}
	gtk_entry_set_text(GTK_ENTRY(widget), string_value ? string_value : DEFAULT_KEYBOARD_PROPERTIES);
	if (string_value != NULL)
		g_free(string_value);

	// Button Keyboard properties Edit
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button15"));
	if (!keyboard_properties_config_not_supported)
		g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_edit_keyboard_properties), builder);
	else
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);

	// Button Keyboard properties Restore
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button16"));
	if (!keyboard_properties_config_not_supported)
		g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_restore_keyboard_properties), builder);
	else
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);

	// Button OK
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button5"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_save_preference), builder);

	// Button Cancel
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button4"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_dontsave_preference), builder);
}

void xneur_add_exclude_app(void)
{
	xneur_add_application(store_exclude_app);
}

void xneur_add_autocompletion_exclude_app(void)
{
	xneur_add_application(store_autocompletion_exclude_app);
}

void xneur_add_auto_app(void)
{
	xneur_add_application(store_auto_app);
}

void xneur_add_manual_app(void)
{
	xneur_add_application(store_manual_app);
}

void xneur_add_delay_send_key_app(void)
{
	xneur_add_application(store_delay_send_key_app);
}

void xneur_add_dont_send_keyrelease_app(void)
{
	xneur_add_application(store_dont_send_keyrelease_app);
}

void xneur_add_layout_app(void)
{
	xneur_add_application(store_layout_app);
}

static void xneur_insert_abbreviation(GtkBuilder* builder)
{
	GtkWidget *entry1 = GTK_WIDGET(gtk_builder_get_object (builder, "entry1"));
	GtkWidget *entry2 = GTK_WIDGET(gtk_builder_get_object (builder, "entry2"));
	const gchar *abbreviation = gtk_entry_get_text(GTK_ENTRY(entry1));
	const gchar *full_text = gtk_entry_get_text(GTK_ENTRY(entry2));
	if (strlen(abbreviation) == 0) 
	{
		error_msg(_("Abbreviation field is empty!"));
		return;
	}
	if (strlen(full_text) == 0) 
	{
		error_msg(_("Expansion text field is empty!"));
		return;
	}
	
	GtkTreeIter iter;
	gtk_list_store_append(GTK_LIST_STORE(store_abbreviation), &iter);
	gtk_list_store_set(GTK_LIST_STORE(store_abbreviation), &iter, 
											0, gtk_entry_get_text(GTK_ENTRY(entry1)),
											1, gtk_entry_get_text(GTK_ENTRY(entry2)), 
										   -1);
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));
	gtk_widget_destroy(window);
}

void xneur_add_abbreviation(void)
{
	GError* error = NULL;
	GtkBuilder* builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, UI_FILE_ABBREVIATION_ADD, &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}
	gtk_builder_connect_signals(builder, NULL);
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));

	gtk_widget_show(window);
	
	// Button OK
	GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object (builder, "button1"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_insert_abbreviation), builder);
}

static void xneur_insert_user_action(GtkBuilder* builder)
{
	GtkWidget *entry1 = GTK_WIDGET(gtk_builder_get_object (builder, "entry1"));
	GtkWidget *entry2 = GTK_WIDGET(gtk_builder_get_object (builder, "entry2"));
	GtkWidget *entry3 = GTK_WIDGET(gtk_builder_get_object (builder, "entry3"));
	const gchar *action = gtk_entry_get_text(GTK_ENTRY(entry1));
	const gchar *key_bind = gtk_entry_get_text(GTK_ENTRY(entry2));
	if (strlen(key_bind) == 0) 
	{
		error_msg(_("Key bind field is empty!"));
		return;
	}
	if (strlen(action) == 0) 
	{
		error_msg(_("User action field is empty!"));
		return;
	}
	
	GtkTreeIter iter;
	gtk_list_store_append(GTK_LIST_STORE(store_user_action), &iter);
	gtk_list_store_set(GTK_LIST_STORE(store_user_action), &iter, 
	                   						0, gtk_entry_get_text(GTK_ENTRY(entry3)),
											1, gtk_entry_get_text(GTK_ENTRY(entry2)),
											2, gtk_entry_get_text(GTK_ENTRY(entry1)), 
										   -1);
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));
	gtk_widget_destroy(window);
}

void xneur_add_user_action(void)
{
	GError* error = NULL;
	GtkBuilder* builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, UI_FILE_ACTION_ADD, &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}
	gtk_builder_connect_signals(builder, NULL);
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));
	
	GtkWidget *widget= GTK_WIDGET(gtk_builder_get_object (builder, "entry2"));
	g_signal_connect ((gpointer) widget, "key-press-event", G_CALLBACK (on_key_press_event), builder);
	
	gtk_widget_show(window);
	
	// Button OK
	widget = GTK_WIDGET(gtk_builder_get_object (builder, "button1"));
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_insert_user_action), builder);
}

static void xneur_replace_user_action(GtkBuilder* builder)
{
	GtkTreeModel *model = GTK_TREE_MODEL(store_user_action);
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tmp_widget));

	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(select, &model, &iter))
	{
		GtkWidget *widget1= GTK_WIDGET(gtk_builder_get_object (builder, "entry1"));
		GtkWidget *widget2= GTK_WIDGET(gtk_builder_get_object (builder, "entry2"));
		GtkWidget *widget3= GTK_WIDGET(gtk_builder_get_object (builder, "entry3"));
		gtk_list_store_set(GTK_LIST_STORE(store_user_action), &iter, 
		                   					0, gtk_entry_get_text(GTK_ENTRY(widget3)),
											1, gtk_entry_get_text(GTK_ENTRY(widget2)),
											2, gtk_entry_get_text(GTK_ENTRY(widget1)), 
										   -1);
	}
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));
	gtk_widget_destroy(window);
}

void xneur_edit_user_action(GtkWidget *treeview)
{
	tmp_widget = GTK_WIDGET(treeview);
	GtkTreeModel *model = GTK_TREE_MODEL(store_user_action);
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(select, &model, &iter))
	{
		GError* error = NULL;
		GtkBuilder* builder = gtk_builder_new ();
		if (!gtk_builder_add_from_file (builder, UI_FILE_ACTION_ADD, &error))
		{
			g_warning ("Couldn't load builder file: %s", error->message);
			g_error_free (error);
		}
		gtk_builder_connect_signals(builder, NULL);
		
		GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));
		
		char *key_bind;
		char *user_action;
		char *action_name;
		gtk_tree_model_get(GTK_TREE_MODEL(store_user_action), &iter, 0, &action_name, 1, &key_bind, 2, &user_action, -1);

		GtkWidget *widget= GTK_WIDGET(gtk_builder_get_object (builder, "entry1"));
		gtk_entry_set_text(GTK_ENTRY(widget), user_action);
		
		widget= GTK_WIDGET(gtk_builder_get_object (builder, "entry2"));
		g_signal_connect ((gpointer) widget, "key_press_event", G_CALLBACK (on_key_press_event), builder);
		g_signal_connect ((gpointer) widget, "key_release_event", G_CALLBACK (on_key_release_event), builder);
		gtk_entry_set_text(GTK_ENTRY(widget), key_bind);

		widget= GTK_WIDGET(gtk_builder_get_object (builder, "entry3"));
		gtk_entry_set_text(GTK_ENTRY(widget), action_name);
		
		gtk_widget_show(window);
		
		// Button OK
		widget = GTK_WIDGET(gtk_builder_get_object (builder, "button1"));
		g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_replace_user_action), builder);
	}
}

static void xneur_replace_action(GtkBuilder* builder)
{
	GtkTreeModel *model = GTK_TREE_MODEL(store_action);
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tmp_widget));

	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(select, &model, &iter))
	{
		GtkWidget *widget1= GTK_WIDGET(gtk_builder_get_object (builder, "entry3"));
		GtkWidget *widget2= GTK_WIDGET(gtk_builder_get_object (builder, "entry2"));
		gtk_list_store_set(GTK_LIST_STORE(store_action), &iter, 
											0, gtk_entry_get_text(GTK_ENTRY(widget1)),
											1, gtk_entry_get_text(GTK_ENTRY(widget2)), 
										   -1);
	}
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));
	gtk_widget_destroy(window);
}

void xneur_clear_action(GtkWidget *treeview)
{
	tmp_widget = GTK_WIDGET(treeview);
	GtkTreeModel *model = GTK_TREE_MODEL(store_action);
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tmp_widget));

	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(select, &model, &iter))
	{
		gtk_list_store_set(GTK_LIST_STORE(store_action), &iter, 
											1, "",
											-1);
	}
}

void xneur_edit_action(GtkWidget *treeview)
{
	tmp_widget = GTK_WIDGET(treeview);
	GtkTreeModel *model = GTK_TREE_MODEL(store_action);
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(select, &model, &iter))
	{
		GError* error = NULL;
		GtkBuilder* builder = gtk_builder_new ();
		if (!gtk_builder_add_from_file (builder, UI_FILE_ACTION_ADD, &error))
		{
			g_warning ("Couldn't load builder file: %s", error->message);
			g_error_free (error);
		}
		gtk_builder_connect_signals(builder, NULL);
		
		GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));
		
		char *key_bind;
		char *action;
		gtk_tree_model_get(GTK_TREE_MODEL(store_action), &iter, 0, &action, 1, &key_bind, -1);

		GtkWidget *widget= GTK_WIDGET(gtk_builder_get_object (builder, "entry3"));
		gtk_editable_set_editable(GTK_EDITABLE(widget), FALSE);
		gtk_entry_set_text(GTK_ENTRY(widget), action);
		
		widget= GTK_WIDGET(gtk_builder_get_object (builder, "entry2"));
		gtk_widget_grab_focus(widget);
		g_signal_connect ((gpointer) widget, "key-press-event", G_CALLBACK (on_key_press_event), builder);
		g_signal_connect ((gpointer) widget, "key-release-event", G_CALLBACK (on_key_release_event), builder);
		gtk_entry_set_text(GTK_ENTRY(widget), key_bind);

		widget= GTK_WIDGET(gtk_builder_get_object (builder, "entry1"));
		gtk_widget_hide (widget);
		widget= GTK_WIDGET(gtk_builder_get_object (builder, "label1"));
		gtk_widget_hide (widget);
		
		gtk_widget_show(window);
		
		// Button OK
		widget = GTK_WIDGET(gtk_builder_get_object (builder, "button1"));
		g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_replace_action), builder);
	}
}

static void xneur_replace_sound(GtkBuilder* builder)
{
	GtkTreeModel *model = GTK_TREE_MODEL(store_sound);
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tmp_widget));

	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(select, &model, &iter))
	{
		GtkWidget *filechooser = GTK_WIDGET(gtk_builder_get_object (builder, "filechooserdialog1"));
	
		gtk_list_store_set(GTK_LIST_STORE(store_sound), &iter, 
											1, gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser)), 
										   -1);
	}
	
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "filechooserdialog1"));
	gtk_widget_destroy(window);
}

void xneur_edit_sound(GtkWidget *treeview)
{
	tmp_widget = GTK_WIDGET(treeview);
	GtkTreeModel *model = GTK_TREE_MODEL(store_sound);
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(select, &model, &iter))
	{
		GError* error = NULL;
		GtkBuilder* builder = gtk_builder_new ();
		if (!gtk_builder_add_from_file (builder, UI_FILE_CHOOSE, &error))
		{
			g_warning ("Couldn't load builder file: %s", error->message);
			g_error_free (error);
		}
		gtk_builder_connect_signals(builder, NULL);
		
		GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "filechooserdialog1"));
		
		char *file;
		gtk_tree_model_get(GTK_TREE_MODEL(store_sound), &iter, 1, &file, -1);
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(window), file);
	
		gtk_widget_show(window);
		
		// Button OK
		GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object (builder, "button5"));
		g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_replace_sound), builder);
	}
}

void xneur_edit_dictionary(GtkWidget *treeview)
{
	tmp_widget = GTK_WIDGET(treeview);
	GtkTreeModel *model = GTK_TREE_MODEL(store_language);
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(select, &model, &iter))
	{
		char *dir;
		gtk_tree_model_get(GTK_TREE_MODEL(store_language), &iter, 1, &dir, -1);

		GError* error = NULL;
		GtkBuilder* builder = gtk_builder_new ();
		if (!gtk_builder_add_from_file (builder, UI_FILE_LIST, &error))
		{
			g_warning ("Couldn't load builder file: %s", error->message);
			g_error_free (error);
		}
		gtk_builder_connect_signals(builder, NULL);
		
		int dir_len = strlen(LANGUAGES_DIR) + strlen(DIR_SEPARATOR) + strlen(dir) + 1;
		char *dir_name = (char *) malloc(dir_len * sizeof(char));
		snprintf(dir_name, dir_len, "%s%s%s", LANGUAGES_DIR, DIR_SEPARATOR, dir);
		
		char *text_path		= xconfig->get_global_dict_path(dir_name, "dictionary");
		char *text_home_path	= xconfig->get_home_dict_path(dir_name, "dictionary");
		char *text		= xneur_get_file_content(text_path);

		if (text == NULL)
		{
			if (text_home_path != NULL)
				g_free(text_home_path);
			if (text_path != NULL)
				g_free(text_path);
			return;
		}
	
		GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "dialog1"));

		gtk_widget_show(window);

		GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object (builder, "treeview1"));
		GtkListStore *store_dict = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
		gtk_tree_view_set_model(GTK_TREE_VIEW(widget), GTK_TREE_MODEL(store_dict));

		GtkCellRenderer *cell = gtk_cell_renderer_text_new();
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(_("Combination of letters"), cell, "text", 0, NULL);
		//gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
		gtk_tree_view_append_column(GTK_TREE_VIEW(widget), GTK_TREE_VIEW_COLUMN(column));

		cell = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Condition"), cell, "text", 1, NULL);
		gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
		gtk_tree_view_append_column(GTK_TREE_VIEW(widget), GTK_TREE_VIEW_COLUMN(column));

		cell = gtk_cell_renderer_toggle_new();
		column = gtk_tree_view_column_new_with_attributes(_("Insensitive"), cell, "active", 2, NULL);
		gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), True);
		gtk_tree_view_append_column(GTK_TREE_VIEW(widget), GTK_TREE_VIEW_COLUMN(column));

		while (1)
		{
			char *line = strsep(&text, "\n");
			if (line == NULL)
				break;

			if (line[0] == '\0')
				continue;

			const char *condition = conditions_names[0];
			gboolean insensitive = FALSE;
			if (strncmp(line, "(?i)", 4 * sizeof(char)) == 0)
			{
				insensitive = TRUE;
				memmove(line, line + 4 * sizeof(char), (strlen(line)-3) * sizeof(char)); 
			}

			if ((line[0] == '^') && (line[strlen(line)-1] == '$'))
			{
				condition = conditions_names[3];
				memmove(line, line + 1 * sizeof(char), strlen(line));
				line[strlen(line) - 1] = '\0';
			}
			else if (line[0] == '^')
			{
				condition = conditions_names[1];
				memmove(line, line + 1 * sizeof(char), strlen(line));
			}
			else if (line[strlen(line)] == '$')
			{
				condition = conditions_names[2];
				line[strlen(line) - 1] = '\0';
			}
			
			GtkTreeIter iter;
			gtk_list_store_append(GTK_LIST_STORE(store_dict), &iter);
			gtk_list_store_set(GTK_LIST_STORE(store_dict), &iter, 
												0, line,
			                   					1, _(condition),
			                   					2, insensitive,
												-1);
		}

		widget = GTK_WIDGET(gtk_builder_get_object (builder, "entry10"));
		gtk_entry_set_text(GTK_ENTRY(widget), text_home_path);

		xyz_t *ud = malloc(sizeof(xyz_t));
		ud->x = builder;
		ud->y = store_dict;

		widget= GTK_WIDGET(gtk_builder_get_object (builder, "addbutton"));
		g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK(on_addbutton_clicked), ud);

		widget= GTK_WIDGET(gtk_builder_get_object (builder, "editbutton"));
		g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK(on_editbutton_clicked), ud);

		widget= GTK_WIDGET(gtk_builder_get_object (builder, "deletebutton"));
		g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK(on_deletebutton_clicked), ud);
	
		widget= GTK_WIDGET(gtk_builder_get_object (builder, "okbutton"));
		g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_okbutton_clicked), ud);

		widget = GTK_WIDGET(gtk_builder_get_object (builder, "cancelbutton"));
		g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_cancelbutton_clicked), ud);

		if (text != NULL)
			g_free(text);
		if (text_home_path != NULL)
			g_free(text_home_path);
		if (text_path != NULL)
			g_free(text_path);
	}
}	

void xneur_rem_exclude_app(GtkWidget *widget)
{
	remove_item(widget, store_exclude_app);
}

void xneur_rem_autocompletion_exclude_app(GtkWidget *widget)
{
	remove_item(widget, store_autocompletion_exclude_app);
}

void xneur_rem_auto_app(GtkWidget *widget)
{
	remove_item(widget, store_auto_app);
}

void xneur_rem_manual_app(GtkWidget *widget)
{
	remove_item(widget, store_manual_app);
}

void xneur_rem_dont_send_keyrelease_app(GtkWidget *widget)
{
	remove_item(widget, store_dont_send_keyrelease_app);
}

void xneur_rem_delay_send_key_app(GtkWidget *widget)
{
	remove_item(widget, store_delay_send_key_app);
}

void xneur_rem_layout_app(GtkWidget *widget)
{
	remove_item(widget, store_layout_app);
}

void xneur_rem_abbreviation(GtkWidget *widget)
{
	remove_item(widget, store_abbreviation);
}

void xneur_rem_user_action(GtkWidget *widget)
{
	remove_item(widget, store_user_action);
}

gboolean save_exclude_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	save_list(store_exclude_app, xconfig->excluded_apps, iter);

	return FALSE;
}

gboolean save_autocompletion_exclude_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	save_list(store_autocompletion_exclude_app, xconfig->autocompletion_excluded_apps, iter);

	return FALSE;
}

gboolean save_auto_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	save_list(store_auto_app, xconfig->auto_apps, iter);

	return FALSE;
}

gboolean save_dont_send_keyrelease_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	save_list(store_dont_send_keyrelease_app, xconfig->dont_send_key_release_apps, iter);

	return FALSE;
}

gboolean save_delay_send_key_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	save_list(store_delay_send_key_app, xconfig->delay_send_key_apps, iter);

	return FALSE;
}

gboolean save_manual_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	save_list(store_manual_app, xconfig->manual_apps, iter);

	return FALSE;
}

gboolean save_layout_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	save_list(store_layout_app, xconfig->layout_remember_apps, iter);

	return FALSE;
}

gboolean save_abbreviation(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	gchar *abbreviation;
	gchar *full_text;
	gchar *ptr = NULL;
	gtk_tree_model_get(GTK_TREE_MODEL(store_abbreviation), iter, 0, &abbreviation, 1, &full_text, -1);

	if (abbreviation != NULL && full_text != NULL)
	{
		int ptr_len = strlen(abbreviation) + strlen(full_text) + 2;
		ptr = malloc(ptr_len* sizeof(gchar));

		snprintf(ptr, ptr_len, "%s %s", abbreviation, full_text);
		xconfig->abbreviations->add(xconfig->abbreviations, ptr);
	}

	if (abbreviation != NULL)
		g_free(abbreviation);
	if (full_text != NULL)
		g_free(full_text);
	if (ptr != NULL)
		g_free(ptr);

	return FALSE;
}

gboolean save_user_action(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	gchar *key_bind;
	gchar *action_text;
	gchar *action_name;
	
	gtk_tree_model_get(GTK_TREE_MODEL(store_user_action), iter, 0, &action_name, 1, &key_bind, 2, &action_text, -1);
	
	char **key_stat = g_strsplit(key_bind, "+", 4);

	int last = is_correct_hotkey(key_stat);
	if (last == -1)
	{
		g_strfreev(key_stat);
		return FALSE;
	}
	
	int action = atoi(gtk_tree_path_to_string(path));
	xconfig->user_actions = (struct _xneur_user_action *) realloc(xconfig->user_actions, (action + 1) * sizeof(struct _xneur_user_action));
	bzero(&xconfig->user_actions[action], sizeof(struct _xneur_user_action));		
	xconfig->user_actions[action].hotkey.modifiers = 0;
	
	for (int i = 0; i <= last; i++)
	{
		int assigned = FALSE;
		for (int j = 0; j < total_modifiers; j++) 
 		{ 
			if (g_utf8_collate (g_utf8_casefold(key_stat[i], strlen (key_stat[i])), 
								g_utf8_casefold(modifier_names[j], strlen (modifier_names[j]))) != 0) 
				continue; 

			assigned = TRUE;
			xconfig->user_actions[action].hotkey.modifiers |= (0x1 << j); 
			break; 
		} 

		if (assigned == FALSE)
		{
			xconfig->user_actions[action].hotkey.key = strdup(key_stat[i]); 
			if (action_text != NULL)
			  xconfig->user_actions[action].command = strdup(action_text);
			if (action_name != NULL)
			  xconfig->user_actions[action].name = strdup(action_name);
		}
	}

	xconfig->user_actions_count = action + 1;
	
	g_strfreev(key_stat);

	if (key_bind != NULL)
		g_free(key_bind);
	if (action_text != NULL)
		g_free(action_text);
	if (action_name != NULL)
		g_free(action_name);

	return FALSE;
}

gboolean save_action(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	/*if (model || path || user_data){};

	gchar *key_bind;
	gchar *action_text;
	gtk_tree_model_get(GTK_TREE_MODEL(store_action), iter, 0, &action_text, 1, &key_bind, -1);

	int i = atoi(gtk_tree_path_to_string(path));
	split_bind((char *) key_bind, i);
	
	return FALSE;*/
	
	if (model || path || user_data){};

	gchar *key_bind;
	gchar *action_text;
	
	gtk_tree_model_get(GTK_TREE_MODEL(store_action), iter, 0, &action_text, 1, &key_bind, -1);
	
	char **key_stat = g_strsplit(key_bind, "+", 4);

	int last = is_correct_hotkey(key_stat);
	/*if (last == -1)
	{
		g_strfreev(key_stat);
		return FALSE;
	}*/
	
	int action = atoi(gtk_tree_path_to_string(path));
	xconfig->actions = (struct _xneur_action *) realloc(xconfig->actions, (action + 1) * sizeof(struct _xneur_action));
	bzero(&xconfig->actions[action], sizeof(struct _xneur_action));		
	xconfig->actions[action].hotkey.modifiers = 0;
	for (int i = 0; i < MAX_HOTKEYS; i++)
	{
		if (strcmp(_(action_names[i]), action_text) == 0)
		{
			xconfig->actions[action].action = i;
		}
	}

	for (int i = 0; i <= last; i++)
	{
		int assigned = FALSE;
		for (int j = 0; j < total_modifiers; j++) 
 		{ 
			if (g_utf8_collate (g_utf8_casefold(key_stat[i], strlen (key_stat[i])), 
								g_utf8_casefold(modifier_names[j], strlen (modifier_names[j]))) != 0) 
				continue; 

			assigned = TRUE;
			xconfig->actions[action].hotkey.modifiers |= (0x1 << j); 
			break; 
		} 

		if (assigned == FALSE)
			xconfig->actions[action].hotkey.key = strdup(key_stat[i]); 
	}

	xconfig->actions_count = action + 1;
	
	g_strfreev(key_stat);

	if (key_bind != NULL)
		g_free(key_bind);
	if (action_text != NULL)
		g_free(action_text);

	return FALSE;
}

gboolean save_sound(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	gchar *file_path;
	gboolean enabled;
	gtk_tree_model_get(GTK_TREE_MODEL(store_sound), iter, 1, &file_path, 2, &enabled, -1);
	
	int i = atoi(gtk_tree_path_to_string(path));
	if (xconfig->sounds[i].file != NULL)
		g_free(xconfig->sounds[i].file);
	
	if (file_path != NULL)
	{
		xconfig->sounds[i].file = strdup(file_path);
		g_free(file_path);
	}

	xconfig->sounds[i].enabled = enabled;
	
	return FALSE;
}

gboolean save_osd(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	gchar *string;
	gboolean enabled;
	gtk_tree_model_get(GTK_TREE_MODEL(store_osd), iter, 1, &string, 2, &enabled, -1);
	
	int i = atoi(gtk_tree_path_to_string(path));
	if (xconfig->osds[i].file != NULL)
		g_free(xconfig->osds[i].file);
	
	if (string != NULL)
	{
		xconfig->osds[i].file = strdup(string);
		g_free(string);
	}

	xconfig->osds[i].enabled = enabled;
	
	return FALSE;
}

gboolean save_popup(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	gchar *string;
	gboolean enabled;
	gtk_tree_model_get(GTK_TREE_MODEL(store_popup), iter, 1, &string, 2, &enabled, -1);
	
	int i = atoi(gtk_tree_path_to_string(path));
	if (xconfig->popups[i].file != NULL)
		g_free(xconfig->popups[i].file);
	
	if (string != NULL)
	{
		xconfig->popups[i].file = strdup(string);
		g_free(string);
	}

	xconfig->popups[i].enabled = enabled;
	
	return FALSE;
}

gboolean save_plugin(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	gchar *string;
	gboolean state = FALSE;
	gtk_tree_model_get(GTK_TREE_MODEL(store_plugin), iter, 0, &state, 2, &string, -1);
	
	if ((string != NULL) && state)
	{
		xconfig->plugins->add(xconfig->plugins, string);
		g_free(string);
	}
	
	return FALSE;
}

gboolean save_language(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	gchar *name;
	gchar *dir;	
	gboolean state = FALSE;
	gtk_tree_model_get(GTK_TREE_MODEL(store_language), iter, 0, &name, 1, &dir, 2, &state, -1);

	int i = atoi(gtk_tree_path_to_string(path));

	xconfig->handle->languages[i].excluded	= state;
	
	return FALSE;
}

void xneur_save_preference(GtkBuilder* builder)
{
	xconfig->clear(xconfig);

	gtk_tree_model_foreach(GTK_TREE_MODEL(store_language), save_language, NULL);
	
	GtkWidget *widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "combobox25"));
	xconfig->default_group = gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound));

	gtk_tree_model_foreach(GTK_TREE_MODEL(store_exclude_app), save_exclude_app, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_auto_app), save_auto_app, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_manual_app), save_manual_app, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_dont_send_keyrelease_app), save_dont_send_keyrelease_app, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_delay_send_key_app), save_delay_send_key_app, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_layout_app), save_layout_app, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_abbreviation), save_abbreviation, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_sound), save_sound, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_osd), save_osd, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_popup), save_popup, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_action), save_action, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_user_action), save_user_action, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_autocompletion_exclude_app), save_autocompletion_exclude_app, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_plugin), save_plugin, NULL);

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton7"));
	xconfig->manual_mode = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton2"));
	xconfig->educate = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton43"));
	xconfig->check_similar_words = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton3"));
	xconfig->remember_layout = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton4"));
	xconfig->save_selection_after_convert = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton30"));
	xconfig->rotate_layout_after_convert = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton5"));
	xconfig->play_sounds = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "spinbutton3"));
	xconfig->volume_percent = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton6"));
	xconfig->save_keyboard_log = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	// Log size
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "spinbutton2"));
	xconfig->size_keyboard_log = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widgetPtrToBefound));

	// Log send to e-mail
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "entry3"));
	if (xconfig->mail_keyboard_log != NULL)
		g_free(xconfig->mail_keyboard_log);
	xconfig->mail_keyboard_log = strdup((char *) gtk_entry_get_text(GTK_ENTRY(widgetPtrToBefound)));
	
	// Log send via host
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "entry4"));
	if (xconfig->host_keyboard_log != NULL)
		g_free(xconfig->host_keyboard_log);
	xconfig->host_keyboard_log = strdup((char *) gtk_entry_get_text(GTK_ENTRY(widgetPtrToBefound)));

	// Log port
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "spinbutton4"));
	xconfig->port_keyboard_log = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton8"));
	xconfig->abbr_ignore_layout = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	
	// Delay Before Send
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "spinbutton1"));
	xconfig->send_delay = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widgetPtrToBefound));
	
	// Log Level
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "combobox1"));
	xconfig->log_level = gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound));
	
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton10"));
	xconfig->correct_two_capital_letter = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton9"));
	xconfig->correct_incidental_caps = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton32"));
	xconfig->flush_buffer_when_press_escape = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton11"));
	xconfig->flush_buffer_when_press_enter = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton33"));
	xconfig->compatibility_with_completion = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton13"));
	xconfig->show_osd = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton18"));
	xconfig->check_lang_on_process = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton19"));
	xconfig->disable_capslock = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton20"));
	xconfig->correct_space_with_punctuation = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton31"));
	xconfig->correct_capital_letter_after_dot = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton29"));
	xconfig->correct_two_space_with_comma_and_space = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton37"));
	xconfig->correct_two_minus_with_dash = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton41"));
	xconfig->correct_dash_with_emdash = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton38"));
	xconfig->correct_c_with_copyright = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton39"));
	xconfig->correct_tm_with_trademark = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton40"));
	xconfig->correct_r_with_registered = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton42"));
	xconfig->correct_three_points_with_ellipsis = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton44"));
	xconfig->correct_misprint = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton21"));
	xconfig->autocompletion = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton1"));
	xconfig->add_space_after_autocompletion = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	// Show popup
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton22"));
	xconfig->show_popup = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "spinbutton6"));
	xconfig->popup_expire_timeout = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widgetPtrToBefound));
	
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "entry2"));
	if (xconfig->osd_font != NULL)
		g_free(xconfig->osd_font);
	xconfig->osd_font = strdup((char *) gtk_entry_get_text(GTK_ENTRY(widgetPtrToBefound)));

	// Troubleshooting
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton14"));
	xconfig->troubleshoot_backspace = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton15"));
	xconfig->troubleshoot_left_arrow = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton16"));
	xconfig->troubleshoot_right_arrow = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton17"));
	xconfig->troubleshoot_up_arrow = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton23"));
	xconfig->troubleshoot_down_arrow = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton24"));
	xconfig->troubleshoot_delete = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton25"));
	xconfig->troubleshoot_switch = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton35"));
	xconfig->troubleshoot_full_screen = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton12"));
	xconfig->troubleshoot_enter = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton45"));
	xconfig->troubleshoot_tab = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));
	
	// Tracking input mode
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton34"));
	xconfig->tracking_input = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	// Tracking input mode
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton28"));
	xconfig->tracking_mouse = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound));

	// Save
	xconfig->save(xconfig);
	printf("Config Saved!\n");
	xconfig->reload(xconfig);
	
	// GXNEUR Preferences

	// Autostart
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton27"));

	gchar *path_file = g_build_filename(getenv("HOME"), AUTOSTART_PATH, NULL);
	if (!g_file_test(path_file, G_FILE_TEST_IS_DIR))
		g_mkdir_with_parents(path_file, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if (path_file != NULL)
		g_free(path_file);
	
	path_file = g_build_filename(getenv("HOME"), AUTOSTART_PATH, AUTOSTART_FILE, NULL);
	FILE *stream = fopen(path_file, "r");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		if (stream != NULL)
		{
			fclose(stream);
			remove(path_file);
		}
		// Create .desktop file
		stream = fopen(path_file, "w");
		if (stream != NULL)
		{
			fprintf(stream, GXNEUR_DESKTOP);
			fclose(stream);
		}
	}
	else
	{
		if (stream != NULL)
		{
			// delete .desktop file
			fclose(stream);
			remove(path_file);
		}
	}
	if (path_file != NULL)
		g_free(path_file);

	// Gnome 3 Shell Area
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "checkbutton36"));

	path_file = g_build_filename(getenv("HOME"), GNOME3_EXT_PATH, NULL);
	if (!g_file_test(path_file, G_FILE_TEST_IS_DIR))
		g_mkdir_with_parents(path_file, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if (path_file != NULL)
		g_free(path_file);
	
	path_file = g_build_filename(getenv("HOME"), GNOME3_EXT_PATH, GNOME3_EXT_JS_FILE, NULL);
	stream = fopen(path_file, "r");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		if (stream != NULL)
		{
			fclose(stream);
			remove(path_file);
		}
		// Create .js file
		stream = fopen(path_file, "w");
		if (stream != NULL)
		{
			fprintf(stream, "const StatusIconDispatcher = imports.ui.statusIconDispatcher;\n\n");
			fprintf(stream, "function enable() {\n");
			fprintf(stream, "    StatusIconDispatcher.STANDARD_TRAY_ICON_IMPLEMENTATIONS['gxneur'] = 'gxneur';\n");
			fprintf(stream, "}\n\n");
			fprintf(stream, "function disable() {\n");
			fprintf(stream, "    StatusIconDispatcher.STANDARD_TRAY_ICON_IMPLEMENTATIONS['gxneur'] = '';\n");
			fprintf(stream, "}\n\n");
			fprintf(stream, "function init() {\n");
			fprintf(stream, "   StatusIconDispatcher.STANDARD_TRAY_ICON_IMPLEMENTATIONS['gxneur'] = 'gxneur';\n");
			fprintf(stream, "}\n\n");
			fprintf(stream, "function main() {\n");
			fprintf(stream, "   StatusIconDispatcher.STANDARD_TRAY_ICON_IMPLEMENTATIONS['gxneur'] = 'gxneur';\n");
			fprintf(stream, "}\n\n");
			fclose(stream);
		}
	}
	else
	{
		if (stream != NULL)
		{
			fclose(stream);
			remove(path_file);
		}
	}
	if (path_file != NULL)
		g_free(path_file);

	path_file = g_build_filename(getenv("HOME"), GNOME3_EXT_PATH, GNOME3_EXT_JSON_FILE, NULL);
	stream = fopen(path_file, "r");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		if (stream != NULL)
		{
			fclose(stream);
			remove(path_file);
		}
		// Create .json file
		stream = fopen(path_file, "w");
		if (stream != NULL)
		{
			fprintf(stream, "{\n");
			fprintf(stream, "   \"shell-version\": [\"3.2\"],\n");
			fprintf(stream, "   \"uuid\": \"gxneur@xneur.ru\",\n");
			fprintf(stream, "   \"name\": \"Gxneur\",\n");
			fprintf(stream, "   \"description\": \"Move the gxneur icon in the shell area of the Gnome 3.\"\n");
			fprintf(stream, "}\n");
			fclose(stream);
		}
	}
	else
	{
		if (stream != NULL)
		{
			fclose(stream);
			remove(path_file);
		}
	}
	if (path_file != NULL)
		g_free(path_file);

	path_file = g_build_filename(getenv("HOME"), GNOME3_EXT_PATH, NULL);
	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
	{
		g_rmdir(path_file);
	}
	if (path_file != NULL)
		g_free(path_file);
	
	// Delay
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "spinbutton5"));
	if (!gxneur_config_write_int("delay", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widgetPtrToBefound)), FALSE))
		arg_delay = -1;

	// Show on the tray
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "combobox2"));
	int show_in_the_tray = gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound));
	const char *string_value = "Flag";
	if (show_in_the_tray == 1)
		string_value = "Text";
	else if (show_in_the_tray == 2)
		string_value = "Icon";
	else if (show_in_the_tray == 3)
		string_value = "Directory";
	if (!gxneur_config_write_str("show_in_the_tray", string_value, FALSE))
		arg_show_in_the_tray = NULL,
		gxneur_config_write_str("show_in_the_tray", string_value, TRUE);
	
	// Icons directory
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "entry1"));
	if (!gxneur_config_write_str("icons_directory", gtk_entry_get_text(GTK_ENTRY(widgetPtrToBefound)), FALSE))
		arg_keyboard_properties = NULL;
	//add_pixmap_directory(gtk_entry_get_text(GTK_ENTRY(widgetPtrToBefound)));

	
	// Rendering engine
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "combobox3"));
	int rendering_engine = gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound));
	string_value = "Built-in";
	if (rendering_engine == 1)
		string_value = "StatusIcon";
	else if (rendering_engine == 2)
		string_value = "AppIndicator";
	if (!gxneur_config_write_str("rendering_engine", string_value, FALSE))
		arg_rendering_engine = NULL,
		gxneur_config_write_str("rendering_engine", string_value, TRUE);

	// Keyboard properties
	widgetPtrToBefound = GTK_WIDGET(gtk_builder_get_object (builder, "entry5"));
	if (!gxneur_config_write_str("keyboard_properties", gtk_entry_get_text(GTK_ENTRY(widgetPtrToBefound)), FALSE))
		arg_keyboard_properties = NULL;
	//add_pixmap_directory(gtk_entry_get_text(GTK_ENTRY(widgetPtrToBefound)));

	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "window2"));
	gtk_widget_destroy(window);
}

void xneur_dontsave_preference(GtkBuilder* builder)
{
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "window2"));
	gtk_widget_destroy(window);
}

char* xneur_get_file_content(const char *path)
{
	struct stat sb;

	if (stat(path, &sb) != 0 || sb.st_size < 0)
		return NULL;

	FILE *stream = fopen(path, "r");
	if (stream == NULL)
		return NULL;

	unsigned int file_len = sb.st_size;

	char *content = (char *) malloc((file_len + 2) * sizeof(char)); // + '\n' + '\0'
	if (fread(content, 1, file_len, stream) != file_len)
	{
		if (content != NULL)
			g_free(content);
		fclose(stream);
		return NULL;
	}

	content[file_len] = '\n';
	content[file_len + 1] = '\0';
	fclose(stream);

	return content;
}

char* modifiers_to_string(unsigned int modifiers)
{
	char *text = (char *) malloc((24 + 1) * sizeof(char));
	text[0] = '\0';

	if (modifiers & ShiftMask)
	{
		strcat(text, modifier_names[0]);
		strcat(text, "+");
	}
  
	if (modifiers & ControlMask)
	{
		strcat(text, modifier_names[1]);
		strcat(text, "+");
	}

	if (modifiers & Mod1Mask)
	{
		strcat(text, modifier_names[2]);
		strcat(text, "+");
	}
	
	if (modifiers & Mod4Mask)
	{
		strcat(text, modifier_names[3]);
		strcat(text, "+");
	}

	return text;
}

int is_correct_hotkey(gchar **key_stat)
{
	int last = 0;
	while (key_stat[last] != NULL)
		last++;

	if (last == 0)
		return -1;

	last--;
	if (g_utf8_collate (g_utf8_casefold(key_stat[last], strlen (key_stat[last])), 
	                    g_utf8_casefold(_("Press any key"), strlen (_("Press any key")))) == 0)
		return -1;

	return last;
}
