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

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>

#include <xneur/xnconfig.h>
#include <xneur/list_char.h>

#define GLADE_FILE_ABOUT PACKAGE_GLADE_FILE_DIR"/about.glade"
#define GLADE_FILE_CONFIG PACKAGE_GLADE_FILE_DIR"/config.glade"

#include "support.h"
#include "callbacks.h"
#include "trayicon.h"

#include "misc.h"

#define MAX_LANGUAGES			4
#define XNEUR_NEEDED_MAJOR_VERSION	3
#define XNEUR_BUILD_MINOR_VERSION	0
	
struct _xneur_config *xconfig				= NULL;
	
static GtkListStore *store_exclude_app			= NULL;
static GtkListStore *store_auto_app			= NULL;
static GtkListStore *store_manual_app			= NULL;
static GtkListStore *store_layout_app			= NULL;
static GtkListStore *store_draw_flag_app			= NULL;

static const char *modifier_names[]			= {"Shift", "Control", "Alt", "Super"};
static const char *all_modifiers[]			= {"Control", "Shift", "Alt", "Super", "Control_R", "Shift_R", "Alt_R", "Super_R", "Control_L", "Shift_L", "Alt_L", "Super_L"};
static const char *language_names[]			= {"English", "Russian", "Ukrainian", "Belarusian", "French", "Romanian", "Kazakh", "German"};
static char *dirs[]					= {"en", "ru", "uk", "be", "fr", "ro", "kz", "de"};

static const char *language_name_boxes[MAX_LANGUAGES]	= {"combobox21", "combobox22", "combobox23", "combobox24"};
static const char *language_combo_boxes[MAX_LANGUAGES]	= {"combobox13", "combobox14", "combobox15", "combobox16"};

static const int total_modifiers			= sizeof(modifier_names) / sizeof(modifier_names[0]); 
static const int total_all_modifiers			= sizeof(all_modifiers) / sizeof(all_modifiers[0]);
static const int total_languages			= sizeof(language_names) / sizeof(language_names[0]);

static char* concat_bind(int action)
{
	char *text = (char *) malloc((24 + 1 + strlen(xconfig->hotkeys[action].key)) * sizeof(char));
	text[0] = '\0';

	for (int i = 0; i < total_modifiers; i++)
	{
		if ((xconfig->hotkeys[action].modifiers & (0x1 << i)) == 0)
			continue;

		strcat(text, modifier_names[i]);
		strcat(text, "+");
	}

	strcat(text, xconfig->hotkeys[action].key);
	
	return text;
}

static void split_bind(char *text, int action)
{
	char **key_stat = g_strsplit(text, "+", 4);

	int last = is_correct_hotkey(key_stat);
	if (last == -1)
	{
		g_strfreev(key_stat);
		return;
	}
	
	xconfig->hotkeys[action].modifiers = 0;
	
	for (int i = 0; i <= last; i++)
	{
		int assigned = FALSE;
		for (int j = 0; j < total_modifiers; j++) 
 		{ 
			if (g_strcasecmp(key_stat[i], modifier_names[j]) != 0) 
				continue; 

			assigned = TRUE;
			xconfig->hotkeys[action].modifiers |= (0x1 << j); 
			break; 
		} 

		if (assigned == FALSE)
			xconfig->hotkeys[action].key = strdup(key_stat[i]); 
	}

	g_strfreev(key_stat);
}

static void fill_binds(int action, GladeXML *gxml, const char *label, gboolean fill_combobox)
{
	GtkWidget *widget_bind = glade_xml_get_widget (gxml, label);
	if (!fill_combobox)
	{
		split_bind((char *) gtk_entry_get_text(GTK_ENTRY(widget_bind)), action);
		return;
	}

	if (xconfig->hotkeys[action].key == NULL)
		return;

	char *binds = concat_bind(action);
	gtk_entry_set_text(GTK_ENTRY(widget_bind), binds);
	free(binds);
}

static void fill_sounds(int sound, GladeXML *gxml, const char *label, gboolean fill_combobox)
{
	GtkWidget *widget_bind = glade_xml_get_widget (gxml, label);
	if (fill_combobox)
	{
		gtk_entry_set_text(GTK_ENTRY(widget_bind), xconfig->sounds[sound].file);
		return;
	}

	if (xconfig->sounds[sound].file != NULL)
		free(xconfig->sounds[sound].file);

	xconfig->sounds[sound].file = strdup(gtk_entry_get_text(GTK_ENTRY(widget_bind)));
}

static void fill_pixmaps(int pixmap, GladeXML *gxml, const char *label, gboolean fill_combobox)
{
	GtkWidget *widget_bind = glade_xml_get_widget (gxml, label);
	if (fill_combobox)
	{
		gtk_entry_set_text(GTK_ENTRY(widget_bind), xconfig->flags[pixmap].file);
		return;
	}

	if (xconfig->flags[pixmap].file != NULL)
		free(xconfig->flags[pixmap].file);

	xconfig->flags[pixmap].file = strdup(gtk_entry_get_text(GTK_ENTRY(widget_bind)));
}

static void add_item(GtkListStore *store)
{
	FILE *fp = popen("xprop WM_CLASS", "r");
	if (fp == NULL)
		return;

	char buffer[NAME_MAX];
	if (fgets(buffer, NAME_MAX, fp) == NULL)
		return;

	if (fclose(fp))
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
	
	GtkTreeIter iter;
	gtk_list_store_append(GTK_LIST_STORE(store), &iter);
	gtk_list_store_set(GTK_LIST_STORE(store), &iter, 0, p, -1);
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

	g_free(ptr);
}

static const char* get_dir_by_index(int index)
{
	if (index == 0)
		return NULL;

	return dirs[index - 1];
}

static void init_libxnconfig(void)
{
	if (xconfig != NULL)
		return;

	// Init configuration
	xconfig = xneur_config_init();

	int major_version, minor_version;
	xconfig->get_library_api_version(&major_version, &minor_version);

	if (major_version != XNEUR_NEEDED_MAJOR_VERSION)
	{
		printf("Wrong XNeur configuration library api version.\nPlease install libxnconfig version %d.x\n", XNEUR_NEEDED_MAJOR_VERSION);
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}

	printf("Using libxnconfig API version %d.%d (build with %d.%d)\n", major_version, minor_version, XNEUR_NEEDED_MAJOR_VERSION, XNEUR_BUILD_MINOR_VERSION);

	if (!xconfig->load(xconfig))
	{
		printf("XNeur config broken!\nPlease, remove ~/.xneur/xneurrc and reinstall package!\n");
		xconfig->uninit(xconfig);
		exit(EXIT_FAILURE);
	}
}

void xneur_start(void)
{
	init_libxnconfig();

	if (!g_spawn_command_line_async("xneur", NULL))
		fprintf(stderr, "ERROR: Couldn't start xneur\nVerify that it installed\n");
}

void xneur_exit(void)
{
	xconfig->kill(xconfig);
	gtk_main_quit();
}

void xneur_start_stop(GtkWidget *widget, struct _tray_icon *tray)
{
	if (widget){};

	if (xconfig->kill(xconfig) == TRUE)
	{
		create_tray_icon(tray, FALSE);
		return;
	}

	xneur_start();
	create_tray_icon(tray, TRUE);
}

void xneur_auto_manual(GtkWidget *widget, struct _tray_icon *tray)
{
	if (widget || tray){};

	int current_mode = xconfig->get_current_mode(xconfig);
	xconfig->set_current_mode(xconfig, 1 - current_mode);
}

void xneur_about(void)
{ 
	GladeXML *gxml = glade_xml_new (GLADE_FILE_ABOUT, NULL, NULL);
	
	glade_xml_signal_autoconnect (gxml);
	GtkWidget *window = glade_xml_get_widget (gxml, "window1");

	GdkPixbuf *window_icon_pixbuf = create_pixbuf ("gxneur.png");
	if (window_icon_pixbuf)
	{
		gtk_window_set_icon (GTK_WINDOW (window), window_icon_pixbuf);
		
		GtkWidget *image = glade_xml_get_widget (gxml, "image1");	
		gtk_image_set_from_pixbuf (GTK_IMAGE (image), window_icon_pixbuf);
		
		gdk_pixbuf_unref (window_icon_pixbuf);
	}
	
	GtkWidget *widget = glade_xml_get_widget(gxml, "label44");
	gchar *text = g_strdup_printf("%s %s", _("Current Version"), VERSION);
	gtk_label_set_text(GTK_LABEL(widget), text);	

	gtk_widget_show(window);
}

void xneur_preference(void)
{
	GladeXML *gxml = glade_xml_new (GLADE_FILE_CONFIG, NULL, NULL);
	
	glade_xml_signal_autoconnect (gxml);
	GtkWidget *window = glade_xml_get_widget (gxml, "window2");

	GdkPixbuf *window_icon_pixbuf = create_pixbuf ("gxneur.png");
	if (window_icon_pixbuf)
	{
		gtk_window_set_icon (GTK_WINDOW (window), window_icon_pixbuf);
		gdk_pixbuf_unref (window_icon_pixbuf);
	}
	
	gtk_widget_show(window);

	// Mode set
	GtkWidget *widget = glade_xml_get_widget (gxml, "combobox1");
	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), xconfig->get_current_mode(xconfig));

	// Exclude App set
	GtkWidget *treeview = glade_xml_get_widget (gxml, "treeview1");

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
	widget = glade_xml_get_widget (gxml, "button2");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_exclude_app), G_OBJECT(window));

	widget = glade_xml_get_widget (gxml, "button3");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_exclude_app), G_OBJECT(treeview));

	// Auto App Set
	treeview = glade_xml_get_widget (gxml, "treeview2");

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
	widget = glade_xml_get_widget (gxml, "button19");

	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_auto_app), G_OBJECT(window));
	widget = glade_xml_get_widget (gxml, "button20");

	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_auto_app), G_OBJECT(treeview));

	// Manual App Set
	treeview = glade_xml_get_widget (gxml, "treeview3");
	
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
	widget = glade_xml_get_widget (gxml, "button21");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_manual_app), G_OBJECT(window));

	widget = glade_xml_get_widget (gxml, "button22");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_manual_app), G_OBJECT(treeview));

	// Layout Remember App Set
	treeview = glade_xml_get_widget (gxml, "treeview4");
	
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
	widget = glade_xml_get_widget (gxml, "button27");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_layout_app), G_OBJECT(window));

	widget = glade_xml_get_widget (gxml, "button28");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_layout_app), G_OBJECT(treeview));

	// Keyboard Bind set
	fill_binds(0, gxml, "entry11", TRUE);
	fill_binds(1, gxml, "entry12", TRUE);
	fill_binds(2, gxml, "entry13", TRUE);
	fill_binds(3, gxml, "entry14", TRUE);
	fill_binds(4, gxml, "entry15", TRUE);
	fill_binds(5, gxml, "entry16", TRUE);
	fill_binds(6, gxml, "entry17", TRUE);
	fill_binds(7, gxml, "entry18", TRUE);
	fill_binds(8, gxml, "entry19", TRUE);
	fill_binds(9, gxml, "entry20", TRUE);

	// Set Callbacks for hotkeys entry
	widget= glade_xml_get_widget (gxml, "entry11");
	g_signal_connect ((gpointer) widget, "key-press-event", G_CALLBACK (on_key_press_event), gxml);
	widget = glade_xml_get_widget (gxml, "entry11");
	g_signal_connect ((gpointer) widget, "key-release-event", G_CALLBACK (on_key_release_event), gxml);
	widget= glade_xml_get_widget (gxml, "entry12");
	g_signal_connect ((gpointer) widget, "key-press-event", G_CALLBACK (on_key_press_event), gxml);
	widget = glade_xml_get_widget (gxml, "entry12");
	g_signal_connect ((gpointer) widget, "key-release-event", G_CALLBACK (on_key_release_event), gxml);
	widget= glade_xml_get_widget (gxml, "entry13");
	g_signal_connect ((gpointer) widget, "key-press-event", G_CALLBACK (on_key_press_event), gxml);
	widget = glade_xml_get_widget (gxml, "entry13");
	g_signal_connect ((gpointer) widget, "key-release-event", G_CALLBACK (on_key_release_event), gxml);
	widget= glade_xml_get_widget (gxml, "entry14");
	g_signal_connect ((gpointer) widget, "key-press-event", G_CALLBACK (on_key_press_event), gxml);
	widget = glade_xml_get_widget (gxml, "entry14");
	g_signal_connect ((gpointer) widget, "key-release-event", G_CALLBACK (on_key_release_event), gxml);
	widget= glade_xml_get_widget (gxml, "entry15");
	g_signal_connect ((gpointer) widget, "key-press-event", G_CALLBACK (on_key_press_event), gxml);
	widget = glade_xml_get_widget (gxml, "entry15");
	g_signal_connect ((gpointer) widget, "key-release-event", G_CALLBACK (on_key_release_event), gxml);
	widget= glade_xml_get_widget (gxml, "entry16");
	g_signal_connect ((gpointer) widget, "key-press-event", G_CALLBACK (on_key_press_event), gxml);
	widget = glade_xml_get_widget (gxml, "entry16");
	g_signal_connect ((gpointer) widget, "key-release-event", G_CALLBACK (on_key_release_event), gxml);
	widget= glade_xml_get_widget (gxml, "entry17");
	g_signal_connect ((gpointer) widget, "key-press-event", G_CALLBACK (on_key_press_event), gxml);
	widget = glade_xml_get_widget (gxml, "entry17");
	g_signal_connect ((gpointer) widget, "key-release-event", G_CALLBACK (on_key_release_event), gxml);
	widget= glade_xml_get_widget (gxml, "entry18");
	g_signal_connect ((gpointer) widget, "key-press-event", G_CALLBACK (on_key_press_event), gxml);
	widget = glade_xml_get_widget (gxml, "entry18");
	g_signal_connect ((gpointer) widget, "key-release-event", G_CALLBACK (on_key_release_event), gxml);
	widget= glade_xml_get_widget (gxml, "entry19");
	g_signal_connect ((gpointer) widget, "key-press-event", G_CALLBACK (on_key_press_event), gxml);
	widget = glade_xml_get_widget (gxml, "entry19");
	g_signal_connect ((gpointer) widget, "key-release-event", G_CALLBACK (on_key_release_event), gxml);
	widget= glade_xml_get_widget (gxml, "entry20");
	g_signal_connect ((gpointer) widget, "key-press-event", G_CALLBACK (on_key_press_event), gxml);
	widget = glade_xml_get_widget (gxml, "entry20");
	g_signal_connect ((gpointer) widget, "key-release-event", G_CALLBACK (on_key_release_event), gxml);
	
	// Set callbacks for clear buttons
	widget = glade_xml_get_widget (gxml, "button11");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button11_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button12");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button12_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button13");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button13_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button14");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button14_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button15");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button15_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button16");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button16_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button17");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button17_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button18");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button18_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button29");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button29_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button30");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button30_clicked), gxml);
			
	
	// Languages
	for (int lang = 0; lang < xconfig->total_languages && lang < MAX_LANGUAGES; lang++)
	{
		GtkWidget *name = glade_xml_get_widget (gxml,  language_name_boxes[lang]);
		widget = glade_xml_get_widget (gxml, language_combo_boxes[lang]);

		gtk_combo_box_set_active(GTK_COMBO_BOX(widget), xconfig->get_lang_group(xconfig, lang));

		const char *lang_name = xconfig->get_lang_name(xconfig, lang);
		for (int j = 0; j < total_languages; j++)
		{
			if (strcmp(lang_name, language_names[j]) != 0)
				continue;

			gtk_combo_box_set_active(GTK_COMBO_BOX(name), j + 1);
			break;
		}
	}
		
	// Default Layout Group
	widget = glade_xml_get_widget (gxml, "combobox25");
	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), xconfig->default_group);
	
	// Mouse Grab 
	widget = glade_xml_get_widget (gxml, "checkbutton1");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->mouse_processing_mode);
	
	// Education Mode
	widget = glade_xml_get_widget (gxml, "checkbutton2");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->education_mode);
	
	// Layout Remember Mode
	widget = glade_xml_get_widget (gxml, "checkbutton3");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->layout_remember_mode);
	
	// Saving Selection Mode
	widget = glade_xml_get_widget (gxml, "checkbutton4");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->save_selection_mode);
	
	// Sound Playing Mode
	widget = glade_xml_get_widget (gxml, "checkbutton5");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->sound_mode);
	
	// Logging Keyboard Mode
	widget = glade_xml_get_widget (gxml, "checkbutton6");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), xconfig->save_log_mode);
	
	// Sound Paths Preference
	fill_sounds(0, gxml, "entry21", TRUE);
	fill_sounds(1, gxml, "entry22", TRUE);
	fill_sounds(2, gxml, "entry23", TRUE);
	fill_sounds(3, gxml, "entry24", TRUE);
	fill_sounds(4, gxml, "entry25", TRUE);
	fill_sounds(5, gxml, "entry26", TRUE);
	fill_sounds(6, gxml, "entry27", TRUE);
	fill_sounds(7, gxml, "entry28", TRUE);
	fill_sounds(8, gxml, "entry29", TRUE);
	fill_sounds(9, gxml, "entry30", TRUE);
	fill_sounds(10, gxml, "entry31", TRUE);
	fill_sounds(11, gxml, "entry32", TRUE);
	fill_sounds(12, gxml, "entry33", TRUE);
	fill_sounds(13, gxml, "entry34", TRUE);
	
	// Set Callbacks for Dict and Regexp
	widget= glade_xml_get_widget (gxml, "button8");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button8_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button9");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button9_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button7");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button7_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button6");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button6_clicked), gxml);
	widget= glade_xml_get_widget (gxml, "button23");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button23_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button24");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button24_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button25");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button25_clicked), gxml);
	widget = glade_xml_get_widget (gxml, "button26");
	g_signal_connect ((gpointer) widget, "clicked", G_CALLBACK (on_button26_clicked), gxml);
	
	// Flags Paths Preference
	fill_pixmaps(0, gxml, "entry35", TRUE);
	fill_pixmaps(1, gxml, "entry36", TRUE);
	fill_pixmaps(2, gxml, "entry37", TRUE);
	fill_pixmaps(3, gxml, "entry38", TRUE);
	
	// Draw Flag App Set
	treeview = glade_xml_get_widget (gxml, "treeview5");
	
	store_draw_flag_app = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store_draw_flag_app));
	gtk_widget_show(treeview);	

	for (int i = 0; i < xconfig->draw_flag_apps->data_count; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_draw_flag_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_draw_flag_app), &iter, 0, xconfig->draw_flag_apps->data[i].string, -1);
	}				

	cell = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes(_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(column));

	// Adding/Removing Draw Flag App
	widget = glade_xml_get_widget (gxml, "button1");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_add_draw_flag_app), G_OBJECT(window));

	widget = glade_xml_get_widget (gxml, "button10");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_rem_draw_flag_app), G_OBJECT(treeview));
	
	// Button OK
	widget = glade_xml_get_widget (gxml, "button5");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_save_preference), gxml);

	// Button Cancel
	widget = glade_xml_get_widget (gxml, "button4");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(xneur_dontsave_preference), gxml);

}

void xneur_add_exclude_app(void)
{
	add_item(store_exclude_app);
}

void xneur_add_auto_app(void)
{
	add_item(store_auto_app);
}

void xneur_add_manual_app(void)
{
	add_item(store_manual_app);
}

void xneur_add_layout_app(void)
{
	add_item(store_layout_app);
}

void xneur_add_draw_flag_app(void)
{
	add_item(store_draw_flag_app);
}

void xneur_rem_exclude_app(GtkWidget *widget)
{
	remove_item(widget, store_exclude_app);
}

void xneur_rem_auto_app(GtkWidget *widget)
{
	remove_item(widget, store_auto_app);
}

void xneur_rem_manual_app(GtkWidget *widget)
{
	remove_item(widget, store_manual_app);
}

void xneur_rem_layout_app(GtkWidget *widget)
{
	remove_item(widget, store_layout_app);
}

void xneur_rem_draw_flag_app(GtkWidget *widget)
{
	remove_item(widget, store_draw_flag_app);
}

gboolean save_exclude_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	save_list(store_exclude_app, xconfig->excluded_apps, iter);

	return FALSE;
}

gboolean save_auto_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	save_list(store_auto_app, xconfig->auto_apps, iter);

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

gboolean save_draw_flag_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	if (model || path || user_data){};

	save_list(store_draw_flag_app, xconfig->draw_flag_apps, iter);

	return FALSE;
}

void xneur_save_preference(GladeXML *gxml)
{
	xconfig->clear(xconfig);

	for (int i = 0; i < MAX_LANGUAGES; i++)
	{
		GtkWidget *widgetPtrToBefound = glade_xml_get_widget (gxml, language_name_boxes[i]);
		GtkWidget *widgetPtrToBefound2 = glade_xml_get_widget (gxml, language_combo_boxes[i]);

		if (gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound)) == -1)
			continue;

		const char *lang_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widgetPtrToBefound));
		const char *lang_dir = get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound)));
		int lang_group = gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound2));

		xconfig->add_language(xconfig, lang_name, lang_dir, lang_group);
	}
	
	GtkWidget *widgetPtrToBefound = glade_xml_get_widget (gxml, "combobox25");
	xconfig->default_group = gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound));

	gtk_tree_model_foreach(GTK_TREE_MODEL(store_exclude_app), save_exclude_app, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_auto_app), save_auto_app, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_manual_app), save_manual_app, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_layout_app), save_layout_app, NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_draw_flag_app), save_draw_flag_app, NULL);
	
	fill_binds(0, gxml, "entry11", FALSE);
	fill_binds(1, gxml, "entry12", FALSE);
	fill_binds(2, gxml, "entry13", FALSE);
	fill_binds(3, gxml, "entry14", FALSE);
	fill_binds(4, gxml, "entry15", FALSE);
	fill_binds(5, gxml, "entry16", FALSE);
	fill_binds(6, gxml, "entry17", FALSE);
	fill_binds(7, gxml, "entry18", FALSE);
	fill_binds(8, gxml, "entry19", FALSE);
	fill_binds(9, gxml, "entry20", FALSE);

	widgetPtrToBefound = glade_xml_get_widget (gxml, "combobox1");
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound)) == 0)
		xconfig->set_current_mode(xconfig, AUTO_MODE);
	else
		xconfig->set_current_mode(xconfig, MANUAL_MODE);

	widgetPtrToBefound = glade_xml_get_widget (gxml, "checkbutton1");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
		xconfig->mouse_processing_mode = MOUSE_GRAB_ENABLE;
	else
		xconfig->mouse_processing_mode = MOUSE_GRAB_DISABLE;
	
	widgetPtrToBefound = glade_xml_get_widget (gxml, "checkbutton2");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
		xconfig->education_mode = EDUCATION_MODE_ENABLE;
	else
		xconfig->education_mode = EDUCATION_MODE_DISABLE;

	widgetPtrToBefound = glade_xml_get_widget (gxml, "checkbutton3");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
		xconfig->layout_remember_mode = LAYOUTE_REMEMBER_ENABLE;
	else
		xconfig->layout_remember_mode = LAYOUTE_REMEMBER_DISABLE;
	
	widgetPtrToBefound = glade_xml_get_widget (gxml, "checkbutton4");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
		xconfig->save_selection_mode = SELECTION_SAVE_ENABLED;
	else
		xconfig->save_selection_mode = SELECTION_SAVE_DISABLED;
	
	widgetPtrToBefound = glade_xml_get_widget (gxml, "checkbutton5");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
		xconfig->sound_mode = SOUND_ENABLED;
	else
		xconfig->sound_mode = SOUND_DISABLED;
	
	widgetPtrToBefound = glade_xml_get_widget (gxml, "checkbutton6");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound)))
		xconfig->save_log_mode = LOG_ENABLED;
	else
		xconfig->save_log_mode = LOG_DISABLED;
	
	// Sound Paths Preference
	fill_sounds(0, gxml, "entry21", FALSE);
	fill_sounds(1, gxml, "entry22", FALSE);
	fill_sounds(2, gxml, "entry23", FALSE);
	fill_sounds(3, gxml, "entry24", FALSE);
	fill_sounds(4, gxml, "entry25", FALSE);
	fill_sounds(5, gxml, "entry26", FALSE);
	fill_sounds(6, gxml, "entry27", FALSE);
	fill_sounds(7, gxml, "entry28", FALSE);
	fill_sounds(8, gxml, "entry29", FALSE);
	fill_sounds(9, gxml, "entry30", FALSE);
	fill_sounds(10, gxml, "entry31", FALSE);
	fill_sounds(11, gxml, "entry32", FALSE);
	fill_sounds(12, gxml, "entry33", FALSE);
	fill_sounds(13, gxml, "entry34", FALSE);
	
	fill_pixmaps(0, gxml, "entry35", FALSE);
	fill_pixmaps(1, gxml, "entry36", FALSE);
	fill_pixmaps(2, gxml, "entry37", FALSE);
	fill_pixmaps(3, gxml, "entry38", FALSE);
	
	GtkWidget *window = glade_xml_get_widget (gxml, "window2");
	gtk_widget_destroy(window);

	xconfig->save(xconfig);
	xconfig->reload(xconfig);
}

void xneur_dontsave_preference(GladeXML *gxml)
{
	GtkWidget *window = glade_xml_get_widget (gxml, "window2");
	gtk_widget_destroy(window);
}

char* xneur_get_dict_path(GladeXML *gxml, int layout_no, const char *file_name)
{
	GtkWidget *widgetPtrToBefound = glade_xml_get_widget (gxml, language_name_boxes[layout_no]);

	const char *dir_name = get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound)));

	return xconfig->get_dict_path(dir_name, file_name);
}

char* xneur_get_home_dict_path(GladeXML *gxml, int layout_no, const char *file_name)
{
	GtkWidget *widgetPtrToBefound = glade_xml_get_widget (gxml, language_name_boxes[layout_no]);

	const char *dir_name = get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound)));

	return xconfig->get_home_dict_path(dir_name, file_name);
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
		free(content);
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
	for (int i = 0; i < total_all_modifiers; i++)
	{
		if (g_strcasecmp(key_stat[last], all_modifiers[i]) == 0)
			return -1;
	}

	if (g_strcasecmp(key_stat[last], _("Press any key")) == 0)
		return -1;

	return last;
}
