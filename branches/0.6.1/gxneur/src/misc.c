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

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <gtk/gtk.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>
//#include <unistd.h>

#include <xneur/xnconfig.h>
#include <xneur/list_char.h>

#include "support.h"

#include "trayicon.h"
#include "interface.h"

#include "misc.h"

#define MAX_LANGUAGES			4
#define XNEUR_NEEDED_MAJOR_VERSION	2
#define XNEUR_BUILD_MINOR_VERSION	0
	
struct _xneur_config *xconfig = NULL;
	
static GtkWidget *wPreference = NULL;
static GtkListStore *store_exclude_app = NULL;
static GtkListStore *store_auto_app = NULL;
static GtkListStore *store_manual_app = NULL;

static void fill_binds(int action, const char *combobox_modifier, const char *combobox_key, gboolean fill_combobox)
{
	GtkWidget *widget_modifier = lookup_widget(wPreference, combobox_modifier);
	if (fill_combobox)
		gtk_combo_box_set_active(GTK_COMBO_BOX(widget_modifier), xconfig->hotkeys[action].modifier);
	else
		xconfig->hotkeys[action].modifier = gtk_combo_box_get_active(GTK_COMBO_BOX(widget_modifier));

	GtkWidget *widget_key = lookup_widget(wPreference, combobox_key);
	if (fill_combobox)
		gtk_combo_box_set_active(GTK_COMBO_BOX(widget_key), xconfig->hotkeys[action].key);
	else
		xconfig->hotkeys[action].key = gtk_combo_box_get_active(GTK_COMBO_BOX(widget_key));
}

static void add_item(GtkListStore *store)
{
	FILE* fp = popen("xprop WM_CLASS", "r");
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

static void remove_item(char *view_name, GtkListStore *store)
{
	GtkTreeModel *model = GTK_TREE_MODEL(store);
	GtkWidget *treeview1 = lookup_widget(wPreference, view_name);
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1));

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

static char *get_dir_by_index(int index)
{
	if (index == 0)
		return NULL;

	static char *dirs[] = {"en", "ru", "uk", "be", "fr", "ro"};
	return dirs[index -1];
}

static void init_libxnconfig(void)
{
	if (xconfig != NULL)
		return;

	int major_version, minor_version;

	// Init configuration
	xconfig = xneur_config_init();
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
	GtkWidget *wAbout = create_window1();
	GtkWidget *widgetPtrToBefound = lookup_widget(wAbout, "label44");
	gchar *text = g_strdup_printf("%s %s", _("Current Version"), VERSION);
	gtk_label_set_text(GTK_LABEL(widgetPtrToBefound), text);
	gtk_widget_show(wAbout);
}

void xneur_preference(void)
{
	wPreference = create_window2();
	gtk_widget_show(wPreference);

	// Mode set
	GtkWidget *widgetPtrToBefound = lookup_widget(wPreference, "combobox1");
	gtk_combo_box_set_active(GTK_COMBO_BOX(widgetPtrToBefound), xconfig->get_current_mode(xconfig));

	// Exclude App set
	widgetPtrToBefound = lookup_widget(wPreference, "treeview1");

	store_exclude_app = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(widgetPtrToBefound), GTK_TREE_MODEL(store_exclude_app));
	gtk_widget_show(widgetPtrToBefound);

	for (int i = 0; i < xconfig->excluded_apps->data_count; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_exclude_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_exclude_app), &iter, 0, xconfig->excluded_apps->data[i].string, -1);
	}

	GtkCellRenderer *cell = gtk_cell_renderer_text_new();

	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(widgetPtrToBefound), GTK_TREE_VIEW_COLUMN(column));

	// Adding/Removing Exclude App
	widgetPtrToBefound = lookup_widget(wPreference, "button2");

	g_signal_connect_swapped(G_OBJECT(widgetPtrToBefound), "clicked", G_CALLBACK(xneur_add_exclude_app), G_OBJECT(wPreference));

	widgetPtrToBefound = lookup_widget(wPreference, "button3");
	g_signal_connect_swapped(G_OBJECT(widgetPtrToBefound), "clicked", G_CALLBACK(xneur_rem_exclude_app), G_OBJECT(wPreference));

	// Auto App Set
	widgetPtrToBefound = lookup_widget(wPreference, "treeview2");

	store_auto_app = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(widgetPtrToBefound), GTK_TREE_MODEL(store_auto_app));
	gtk_widget_show(widgetPtrToBefound);

	for (int i = 0; i < xconfig->auto_apps->data_count; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_auto_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_auto_app), &iter, 0, xconfig->auto_apps->data[i].string, -1);
	}

	cell = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes(_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(widgetPtrToBefound), GTK_TREE_VIEW_COLUMN(column));

	// Adding/Removing Auto App
	widgetPtrToBefound = lookup_widget(wPreference, "button19");

	g_signal_connect_swapped(G_OBJECT(widgetPtrToBefound), "clicked", G_CALLBACK(xneur_add_auto_app), G_OBJECT(wPreference));
	widgetPtrToBefound = lookup_widget(wPreference, "button20");

	g_signal_connect_swapped(G_OBJECT(widgetPtrToBefound), "clicked", G_CALLBACK(xneur_rem_auto_app), G_OBJECT(wPreference));

	// Manual App Set
	widgetPtrToBefound = lookup_widget(wPreference, "treeview3");
	
	store_manual_app = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(widgetPtrToBefound), GTK_TREE_MODEL(store_manual_app));
	gtk_widget_show(widgetPtrToBefound);	

	for (int i = 0; i < xconfig->manual_apps->data_count; i++)
	{
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(store_manual_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_manual_app), &iter, 0, xconfig->manual_apps->data[i].string, -1);
	}				

	cell = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes(_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(widgetPtrToBefound), GTK_TREE_VIEW_COLUMN(column));

	// Adding/Removing Manual App
	widgetPtrToBefound = lookup_widget(wPreference, "button21");
	g_signal_connect_swapped(G_OBJECT(widgetPtrToBefound), "clicked", G_CALLBACK(xneur_add_manual_app), G_OBJECT(wPreference));

	widgetPtrToBefound = lookup_widget(wPreference, "button22");
	g_signal_connect_swapped(G_OBJECT(widgetPtrToBefound), "clicked", G_CALLBACK(xneur_rem_manual_app), G_OBJECT(wPreference));

	// Keyboard Bind set
	fill_binds(0, "combobox10", "combobox3", TRUE);
	fill_binds(1, "combobox11", "combobox5", TRUE);
	fill_binds(2, "combobox6", "combobox7", TRUE);
	fill_binds(3, "combobox8", "combobox9", TRUE);
	fill_binds(4, "combobox17", "combobox18", TRUE);
	fill_binds(5, "combobox19", "combobox20", TRUE);

	// Languages
	static const char *language_name_boxes[MAX_LANGUAGES] = {"combobox21", "combobox22", "combobox23", "combobox24"};
	static const char *language_combo_boxes[MAX_LANGUAGES] = {"combobox13", "combobox14", "combobox15", "combobox16"};

	for (int lang = 0; lang < xconfig->total_languages && lang < MAX_LANGUAGES; lang++)
	{
		//GtkWidget *dir = lookup_widget(wPreference, file_choose_buttons[lang]);
		GtkWidget *name = lookup_widget(wPreference, language_name_boxes[lang]);
		widgetPtrToBefound = lookup_widget(wPreference, language_combo_boxes[lang]);

		gtk_combo_box_set_active(GTK_COMBO_BOX(widgetPtrToBefound), xconfig->get_lang_group(xconfig, lang));

		const char *lang_name = xconfig->get_lang_name(xconfig, lang);
		if (strcmp(lang_name, "English") == 0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(name), 1);
		if (strcmp(lang_name, "Russian") == 0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(name), 2);
		if (strcmp(lang_name, "Ukrainian") == 0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(name), 3);
		if (strcmp(lang_name, "Belarussian") == 0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(name), 4);
		if (strcmp(lang_name, "French") == 0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(name), 5);
		if (strcmp(lang_name, "Romanian") == 0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(name), 6);
	}
	
	// Mouse Grab 
	widgetPtrToBefound = lookup_widget(wPreference, "checkbutton1");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgetPtrToBefound), xconfig->mouse_processing_mode);
	
	// Education Mode
	widgetPtrToBefound = lookup_widget(wPreference, "checkbutton2");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgetPtrToBefound), xconfig->education_mode);
	
	// Layout Remember Mode
	widgetPtrToBefound = lookup_widget(wPreference, "checkbutton3");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgetPtrToBefound), xconfig->layout_remember_mode);
	
	// Saving Selection Mode
	widgetPtrToBefound = lookup_widget(wPreference, "checkbutton4");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgetPtrToBefound), xconfig->save_selection_mode);
	
	// Button OK
	widgetPtrToBefound = lookup_widget(wPreference, "button5");
	g_signal_connect_swapped((gpointer) widgetPtrToBefound, "clicked", G_CALLBACK(xneur_save_preference), GTK_OBJECT(widgetPtrToBefound));
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

void xneur_rem_exclude_app(void)
{
	remove_item("treeview1", store_exclude_app);
}

void xneur_rem_auto_app(void)
{
	remove_item("treeview2", store_auto_app);
}

void xneur_rem_manual_app(void)
{
	remove_item("treeview3", store_manual_app);
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

void xneur_save_preference(void)
{
	GtkWidget *widgetPtrToBefound1;
	GtkWidget *widgetPtrToBefound3;
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox1");
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)) == 0)
		xconfig->xneur_data->xneur_mode = AUTO_MODE;
	else
		xconfig->xneur_data->xneur_mode = MANUAL_MODE;
		
	int total_lang = 0;
	char **lang_name = malloc(4 * sizeof(char*));
	char **lang_dir = malloc(4 * sizeof(char*)); 
	int *lang_group = malloc(4 * sizeof(int));
	
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox21");
	widgetPtrToBefound3 = lookup_widget(wPreference, "combobox13");
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)) > 0)
	{
		lang_name[total_lang] = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widgetPtrToBefound1));
		lang_dir[total_lang] = get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)));
		lang_group[total_lang] = gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound3));
		total_lang++;
	}
	
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox22");
	widgetPtrToBefound3 = lookup_widget(wPreference, "combobox14");
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)) > 0)
	{
		lang_name[total_lang] = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widgetPtrToBefound1));
		lang_dir[total_lang] = get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)));
		lang_group[total_lang] = gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound3));
		total_lang++;
	}
	
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox23");
	widgetPtrToBefound3 = lookup_widget(wPreference, "combobox15");
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)) > 0)
	{
		lang_name[total_lang] = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widgetPtrToBefound1));
		lang_dir[total_lang] = get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)));
		lang_group[total_lang] = gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound3));
		total_lang++;
	}
	
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox24");
	widgetPtrToBefound3 = lookup_widget(wPreference, "combobox16");
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)) > 0)
	{
		lang_name[total_lang] = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widgetPtrToBefound1));
		lang_dir[total_lang] = get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)));
		lang_group[total_lang] = gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound3));
		total_lang++;
	}

	xconfig->set_languages(xconfig, lang_name, lang_dir, lang_group, total_lang);

	xconfig->excluded_apps->uninit(xconfig->excluded_apps);
	xconfig->excluded_apps = list_char_init();

	gtk_tree_model_foreach(GTK_TREE_MODEL(store_exclude_app), save_exclude_app, NULL);
		
	xconfig->auto_apps->uninit(xconfig->auto_apps);
	xconfig->auto_apps = list_char_init();

	gtk_tree_model_foreach(GTK_TREE_MODEL(store_auto_app), save_auto_app, NULL);
		
	xconfig->manual_apps->uninit(xconfig->manual_apps);
	xconfig->manual_apps = list_char_init();

	gtk_tree_model_foreach(GTK_TREE_MODEL(store_manual_app), save_manual_app, NULL);

	fill_binds(0, "combobox10", "combobox3", FALSE);
	fill_binds(1, "combobox11", "combobox5", FALSE);
	fill_binds(2, "combobox6", "combobox7", FALSE);
	fill_binds(3, "combobox8", "combobox9", FALSE);
	fill_binds(4, "combobox17", "combobox18", FALSE);
	fill_binds(5, "combobox19", "combobox20", FALSE);

	widgetPtrToBefound1 = lookup_widget(wPreference, "checkbutton1");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound1)))
		xconfig->mouse_processing_mode = MOUSE_GRAB_ENABLE;
	else
		xconfig->mouse_processing_mode = MOUSE_GRAB_DISABLE;
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "checkbutton2");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound1)))
		xconfig->education_mode = EDUCATION_MODE_ENABLE;
	else
		xconfig->education_mode = EDUCATION_MODE_DISABLE;

	widgetPtrToBefound1 = lookup_widget(wPreference, "checkbutton3");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound1)))
		xconfig->layout_remember_mode = LAYOUTE_REMEMBER_ENABLE;
	else
		xconfig->layout_remember_mode = LAYOUTE_REMEMBER_DISABLE;
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "checkbutton4");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widgetPtrToBefound1)))
		xconfig->save_selection_mode = SELECTION_SAVE_ENABLED;
	else
		xconfig->save_selection_mode = SELECTION_SAVE_DISABLED;
		
	gtk_widget_destroy(wPreference);

	xconfig->save(xconfig);
	xconfig->reload_config(xconfig);
}

char *xneur_get_dict_path(int layout_no, const char *file_name)
{
	const char *language_name_boxes[MAX_LANGUAGES] = {"combobox21", "combobox22", "combobox23", "combobox24"};

	GtkWidget *widgetPtrToBefound = lookup_widget(wPreference, language_name_boxes[layout_no]);

	char *dir_name = get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound)));

	return xconfig->get_dict_path(dir_name, file_name);
}

char *xneur_get_file_content(const char *path)
{
	struct stat sb;

	if (stat(path, &sb) != 0 || sb.st_size < 0)
		return NULL;

	FILE *stream = fopen(path, "r");
	if (stream == NULL)
		return NULL;

	unsigned int file_len = sb.st_size;

	char *content = (char *) malloc((file_len + 2) * sizeof(char)); // + 1 '\n' + 1 '\0'
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
