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

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

#include <xneur/xnconfig.h>
#include "list.h"

#include "support.h"
#include "trayicon.h"
#include "interface.h"

#include "misc.h"

#define MAX_LANGUAGES 4
	
struct _xneur_config *xconfig = NULL;
	
static GtkWidget *wPreference = NULL;
static GtkListStore *store_exclude_app = NULL;
static GtkListStore *store_auto_app = NULL;
static GtkListStore *store_manual_app = NULL;

void xneur_config_reload(void)
{
	if (xconfig != NULL)
		xconfig->uninit(xconfig);
	xconfig = xneur_config_init();
	enum _error error_code = xconfig->load(xconfig);
	if (error_code != ERROR_SUCCESS)
	{
		xconfig->uninit(xconfig);
		printf("XNeur config broken!\nPlease, remove ~/.xneur/xneurrc and reinstall package!\n");
		exit(EXIT_FAILURE);
	}
}
static gboolean xneur_start(gboolean is_sync)
{
	// Init configuration
	xneur_config_reload();
	
	gboolean retval;
	if (is_sync == TRUE)
		retval = g_spawn_command_line_sync("xneur", NULL, NULL, NULL, NULL);
	else
		retval = g_spawn_command_line_async("xneur", NULL);

	if (retval != FALSE)
		return TRUE;

	fprintf(stderr, "ERROR: Couldn't start xneur\nVerify that it installed\n");
	return FALSE;
}

static gboolean xneur_stop(void)
{
	if (xconfig == NULL)
		return TRUE;
	
	int xneur_pid = xconfig->get_pid(xconfig);
	if (xneur_pid == -1)
		return FALSE;

	if (kill(xneur_pid, SIGTERM) == -1)
		return FALSE;
	
	xneur_config_reload();
	return TRUE;
}

void xneur_restart(void)
{
	if (xneur_stop() == TRUE)
		usleep(20000);

	xneur_start(FALSE);
}

void xneur_exit(void)
{
	xneur_stop();
	gtk_main_quit();
}

void xneur_start_stop(GtkWidget *widget, struct _tray_icon *tray)
{
	if (widget){};

	if (xneur_stop() == TRUE)
	{
		create_tray_icon(tray, FALSE);
		return;
	}

	xneur_start(FALSE);
	create_tray_icon(tray, TRUE);
}

void xneur_auto_manual(GtkWidget *widget, struct _tray_icon *tray)
{
	if (widget || tray){};

	if (xconfig->get_current_mode(xconfig) == 0)
	{
		xconfig->set_current_mode(xconfig, 1);
		return;
	}

	xconfig->set_current_mode(xconfig, 0);
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
	xneur_config_reload();
	GtkWidget *widgetPtrToBefound;
	wPreference = create_window2();
	gtk_widget_show(wPreference);
	
	// Mode set
	widgetPtrToBefound = lookup_widget(wPreference, "combobox1");
	gtk_combo_box_set_active(GTK_COMBO_BOX(widgetPtrToBefound), xconfig->get_current_mode(xconfig));
	
	// Exclude App set
	GtkCellRenderer *cell;
	GtkTreeViewColumn *column;
	GtkTreeIter	 iter;
	
	widgetPtrToBefound = lookup_widget(wPreference, "treeview1");
	
	store_exclude_app = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (widgetPtrToBefound), GTK_TREE_MODEL (store_exclude_app));
	gtk_widget_show (widgetPtrToBefound);	

	int i;
	for (i = 0; i < xconfig->excluded_apps->data_count; i++)
	{
		gtk_list_store_append(GTK_LIST_STORE(store_exclude_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_exclude_app), &iter, 0, xconfig->excluded_apps->data[i].string, -1);
	}				

	cell = gtk_cell_renderer_text_new ();					 
	
	column = gtk_tree_view_column_new_with_attributes (_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (widgetPtrToBefound), GTK_TREE_VIEW_COLUMN (column));
	
	// Adding/Removing Exclude App
	widgetPtrToBefound = lookup_widget(wPreference, "button2");
	
	g_signal_connect_swapped (G_OBJECT (widgetPtrToBefound), "clicked",
															G_CALLBACK (xneur_add_exclude_app),
															G_OBJECT (wPreference));
	widgetPtrToBefound = lookup_widget(wPreference, "button3");
	g_signal_connect_swapped (G_OBJECT (widgetPtrToBefound), "clicked",
															G_CALLBACK (xneur_rem_exclude_app),
															G_OBJECT (wPreference));
	
	// Auto App Set
	widgetPtrToBefound = lookup_widget(wPreference, "treeview2");
	
	store_auto_app = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (widgetPtrToBefound), GTK_TREE_MODEL (store_auto_app));
	gtk_widget_show (widgetPtrToBefound);	

	for (i = 0; i < xconfig->auto_apps->data_count; i++)
	{
		gtk_list_store_append(GTK_LIST_STORE(store_auto_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_auto_app), &iter, 0, xconfig->auto_apps->data[i].string, -1);
	}				

	cell = gtk_cell_renderer_text_new ();					 
	
	column = gtk_tree_view_column_new_with_attributes (_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (widgetPtrToBefound), GTK_TREE_VIEW_COLUMN (column));
	
	// Adding/Removing Auto App
	widgetPtrToBefound = lookup_widget(wPreference, "button19");
	
	g_signal_connect_swapped (G_OBJECT (widgetPtrToBefound), "clicked",
															G_CALLBACK (xneur_add_auto_app),
															G_OBJECT (wPreference));
	widgetPtrToBefound = lookup_widget(wPreference, "button20");
	g_signal_connect_swapped (G_OBJECT (widgetPtrToBefound), "clicked",
															G_CALLBACK (xneur_rem_auto_app),
															G_OBJECT (wPreference));
					
	// Manual App Set
	widgetPtrToBefound = lookup_widget(wPreference, "treeview3");
	
	store_manual_app = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (widgetPtrToBefound), GTK_TREE_MODEL (store_manual_app));
	gtk_widget_show (widgetPtrToBefound);	

	for (i = 0; i < xconfig->manual_apps->data_count; i++)
	{
		gtk_list_store_append(GTK_LIST_STORE(store_manual_app), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store_manual_app), &iter, 0, xconfig->manual_apps->data[i].string, -1);
	}				

	cell = gtk_cell_renderer_text_new ();					 
	
	column = gtk_tree_view_column_new_with_attributes (_("Application"), cell, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (widgetPtrToBefound), GTK_TREE_VIEW_COLUMN (column));
		
	// Adding/Removing Manual App
	widgetPtrToBefound = lookup_widget(wPreference, "button21");
	
	g_signal_connect_swapped (G_OBJECT (widgetPtrToBefound), "clicked",
															G_CALLBACK (xneur_add_manual_app),
															G_OBJECT (wPreference));
	widgetPtrToBefound = lookup_widget(wPreference, "button22");
	g_signal_connect_swapped (G_OBJECT (widgetPtrToBefound), "clicked",
															G_CALLBACK (xneur_rem_manual_app),
															G_OBJECT (wPreference));
	// Keyboard Bind set
	// ChangeWord
	widgetPtrToBefound = lookup_widget(wPreference, "combobox3");
	if (xconfig->hotkeys[0].key == KEY_NONE)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 0);	
	}
	else if (xconfig->hotkeys[0].key == KEY_BREAK)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 1);	
	}
	else if (xconfig->hotkeys[0].key == KEY_SCROLL_LOCK)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 2);	
	}
	else if (xconfig->hotkeys[0].key == KEY_PRINT_SCREEN)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);	
	}
	else 
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	
	widgetPtrToBefound = lookup_widget(wPreference, "combobox10");
	if (xconfig->hotkeys[0].modifier == MODIFIER_NONE)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 0);
	}
	else if (xconfig->hotkeys[0].modifier == MODIFIER_SHIFT)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 1);
	}
	else if (xconfig->hotkeys[0].modifier == MODIFIER_CONTROL)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 2);
	}
	else if (xconfig->hotkeys[0].modifier == MODIFIER_ALT)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	else 
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	
	
	// ChangeString
	widgetPtrToBefound = lookup_widget(wPreference, "combobox5");
	if (xconfig->hotkeys[1].key == KEY_NONE)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 0);	
	}
	else if (xconfig->hotkeys[1].key == KEY_BREAK)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 1);	
	}
	else if (xconfig->hotkeys[1].key == KEY_SCROLL_LOCK)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 2);	
	}
	else if (xconfig->hotkeys[1].key == KEY_PRINT_SCREEN)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);	
	}
	else 
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	
	widgetPtrToBefound = lookup_widget(wPreference, "combobox11");
	if (xconfig->hotkeys[1].modifier == MODIFIER_NONE)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 0);
	}
	else if (xconfig->hotkeys[1].modifier == MODIFIER_SHIFT)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 1);
	}
	else if (xconfig->hotkeys[1].modifier == MODIFIER_CONTROL)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 2);
	}
	else if (xconfig->hotkeys[1].modifier == MODIFIER_ALT)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	else 
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	
	// ChangeMode
	widgetPtrToBefound = lookup_widget(wPreference, "combobox7");
	if (xconfig->hotkeys[2].key == KEY_NONE)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 0);	
	}
	else if (xconfig->hotkeys[2].key == KEY_BREAK)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 1);	
	}
	else if (xconfig->hotkeys[2].key == KEY_SCROLL_LOCK)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 2);	
	}
	else if (xconfig->hotkeys[2].key == KEY_PRINT_SCREEN)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);	
	}
	else 
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	
	widgetPtrToBefound = lookup_widget(wPreference, "combobox6");
	if (xconfig->hotkeys[2].modifier == MODIFIER_NONE)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 0);
	}
	else if (xconfig->hotkeys[2].modifier == MODIFIER_SHIFT)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 1);
	}
	else if (xconfig->hotkeys[2].modifier == MODIFIER_CONTROL)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 2);
	}
	else if (xconfig->hotkeys[2].modifier == MODIFIER_ALT)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	else 
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	
	// ChangeSelected
	widgetPtrToBefound = lookup_widget(wPreference, "combobox9");
	if (xconfig->hotkeys[3].key == KEY_NONE)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 0);	
	}
	else if (xconfig->hotkeys[3].key == KEY_BREAK)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 1);	
	}
	else if (xconfig->hotkeys[3].key == KEY_SCROLL_LOCK)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 2);	
	}
	else if (xconfig->hotkeys[3].key == KEY_PRINT_SCREEN)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);	
	}
	else 
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	
	widgetPtrToBefound = lookup_widget(wPreference, "combobox8");
	if (xconfig->hotkeys[3].modifier == MODIFIER_NONE)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 0);
	}
	else if (xconfig->hotkeys[3].modifier == MODIFIER_SHIFT)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 1);
	}
	else if (xconfig->hotkeys[3].modifier == MODIFIER_CONTROL)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 2);
	}
	else if (xconfig->hotkeys[3].modifier == MODIFIER_ALT)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	else 
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	
	// TranslitSelected
	widgetPtrToBefound = lookup_widget(wPreference, "combobox18");
	if (xconfig->hotkeys[4].key == KEY_NONE)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 0);	
	}
	else if (xconfig->hotkeys[4].key == KEY_BREAK)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 1);	
	}
	else if (xconfig->hotkeys[4].key == KEY_SCROLL_LOCK)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 2);	
	}
	else if (xconfig->hotkeys[4].key == KEY_PRINT_SCREEN)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);	
	}
	else 
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	
	widgetPtrToBefound = lookup_widget(wPreference, "combobox17");
	if (xconfig->hotkeys[4].modifier == MODIFIER_NONE)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 0);
	}
	else if (xconfig->hotkeys[4].modifier == MODIFIER_SHIFT)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 1);
	}
	else if (xconfig->hotkeys[4].modifier == MODIFIER_CONTROL)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 2);
	}
	else if (xconfig->hotkeys[4].modifier == MODIFIER_ALT)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	else 
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	
	// ChangecaseSelected
	widgetPtrToBefound = lookup_widget(wPreference, "combobox20");
	if (xconfig->hotkeys[5].key == KEY_NONE)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 0);	
	}
	else if (xconfig->hotkeys[5].key == KEY_BREAK)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 1);	
	}
	else if (xconfig->hotkeys[5].key == KEY_SCROLL_LOCK)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 2);	
	}
	else if (xconfig->hotkeys[5].key == KEY_PRINT_SCREEN)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);	
	}
	else 
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	
	widgetPtrToBefound = lookup_widget(wPreference, "combobox19");
	if (xconfig->hotkeys[5].modifier == MODIFIER_NONE)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 0);
	}
	else if (xconfig->hotkeys[5].modifier == MODIFIER_SHIFT)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 1);
	}
	else if (xconfig->hotkeys[5].modifier == MODIFIER_CONTROL)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 2);
	}
	else if (xconfig->hotkeys[5].modifier == MODIFIER_ALT)
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	else 
	{
		gtk_combo_box_set_active (GTK_COMBO_BOX (widgetPtrToBefound), 3);
	}
	
	// Languages
	const char *language_name_boxes[MAX_LANGUAGES] = {"combobox21", "combobox22", "combobox23", "combobox24"};
	const char *language_combo_boxes[MAX_LANGUAGES] = {"combobox13", "combobox14", "combobox15", "combobox16"};

	int lang;
	for (lang = 0; lang < xconfig->total_languages && lang < MAX_LANGUAGES; lang++)
	{
		//GtkWidget *dir = lookup_widget(wPreference, file_choose_buttons[lang]);
		GtkWidget *name = lookup_widget(wPreference, language_name_boxes[lang]);
		widgetPtrToBefound = lookup_widget(wPreference, language_combo_boxes[lang]);

		gtk_combo_box_set_active(GTK_COMBO_BOX(widgetPtrToBefound), xconfig->get_lang_group(xconfig, lang));
		if (strcmp(xconfig->get_lang_name(xconfig, lang), "English") == 0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(name), 1);
		if (strcmp(xconfig->get_lang_name(xconfig, lang), "Russian") == 0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(name), 2);
		if (strcmp(xconfig->get_lang_name(xconfig, lang), "Ukrainian") == 0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(name), 3);
		if (strcmp(xconfig->get_lang_name(xconfig, lang), "Belarussian") == 0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(name), 4);
		if (strcmp(xconfig->get_lang_name(xconfig, lang), "French") == 0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(name), 5);
		if (strcmp(xconfig->get_lang_name(xconfig, lang), "Romanian") == 0)
			gtk_combo_box_set_active(GTK_COMBO_BOX(name), 6);
	}
	
	// Mouse Grab 
	widgetPtrToBefound = lookup_widget(wPreference, "checkbutton1");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widgetPtrToBefound), xconfig->mouse_processing_mode);
	
	// Education Mode
	widgetPtrToBefound = lookup_widget(wPreference, "checkbutton2");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widgetPtrToBefound), xconfig->education_mode);
	
	// Layout Remember Mode
	widgetPtrToBefound = lookup_widget(wPreference, "checkbutton3");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widgetPtrToBefound), xconfig->layout_remember_mode);
	
	// Saving Selection Mode
	widgetPtrToBefound = lookup_widget(wPreference, "checkbutton4");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widgetPtrToBefound), xconfig->save_selection_mode);
	
	// Button OK
	widgetPtrToBefound = lookup_widget(wPreference, "button5");
	g_signal_connect_swapped((gpointer)widgetPtrToBefound, "clicked", G_CALLBACK(xneur_save_preference), GTK_OBJECT(widgetPtrToBefound));
}

void xneur_add_exclude_app(void)
{
	GtkTreeIter	 iter;
	FILE* fp = popen("xprop WM_CLASS", "r");
	char buffer[NAME_MAX];
	const char* cap = "WM_CLASS(STRING)";
	
	if (fp == NULL)
		return;

	if (fgets(buffer, NAME_MAX, fp) == NULL)
		return;

	if (fclose(fp))
		return;

	if (strncmp(buffer, cap, strlen(cap)) != 0)
		return;

	char* p = strstr(buffer, "\", \"");
	if (p == NULL)
		return;
	if (strlen(p += 4) < 2)
		return;
	p[strlen(p) - 2] = '\0';
	
	gtk_list_store_append(GTK_LIST_STORE(store_exclude_app), &iter);
	gtk_list_store_set (GTK_LIST_STORE(store_exclude_app), &iter, 0, p, -1);
}

void xneur_add_auto_app(void)
{
	GtkTreeIter	 iter;
	FILE* fp = popen("xprop WM_CLASS", "r");
	char buffer[NAME_MAX];
	const char* cap = "WM_CLASS(STRING)";
	
	if (fp == NULL)
		return;

	if (fgets(buffer, NAME_MAX, fp) == NULL)
		return;

	if (fclose(fp))
		return;

	if (strncmp(buffer, cap, strlen(cap)) != 0)
		return;

	char* p = strstr(buffer, "\", \"");
	if (p == NULL)
		return;
	if (strlen(p += 4) < 2)
		return;
	p[strlen(p) - 2] = '\0';
	
	gtk_list_store_append(GTK_LIST_STORE(store_auto_app), &iter);
	gtk_list_store_set (GTK_LIST_STORE(store_auto_app), &iter, 0, p, -1);
}

void xneur_add_manual_app(void)
{
	GtkTreeIter	 iter;
	FILE* fp = popen("xprop WM_CLASS", "r");
	char buffer[NAME_MAX];
	const char* cap = "WM_CLASS(STRING)";
	
	if (fp == NULL)
		return;

	if (fgets(buffer, NAME_MAX, fp) == NULL)
		return;

	if (fclose(fp))
		return;

	if (strncmp(buffer, cap, strlen(cap)) != 0)
		return;

	char* p = strstr(buffer, "\", \"");
	if (p == NULL)
		return;
	if (strlen(p += 4) < 2)
		return;
	p[strlen(p) - 2] = '\0';
	
	gtk_list_store_append(GTK_LIST_STORE(store_manual_app), &iter);
	gtk_list_store_set (GTK_LIST_STORE(store_manual_app), &iter, 0, p, -1);
}

void xneur_rem_exclude_app(void)
{
	GtkTreeIter	 iter;
	GtkTreeSelection *select;	
	GtkTreeModel *model = GTK_TREE_MODEL (store_exclude_app);
	GtkWidget *treeview1;
	treeview1 = lookup_widget(wPreference, "treeview1");
	
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview1));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	if (gtk_tree_selection_get_selected (select, &model, &iter))				
		gtk_list_store_remove(GTK_LIST_STORE(store_exclude_app), &iter);
}

void xneur_rem_auto_app(void)
{
	GtkTreeIter	 iter;
	GtkTreeSelection *select;	
	GtkTreeModel *model = GTK_TREE_MODEL (store_auto_app);
	GtkWidget *treeview1;
	treeview1 = lookup_widget(wPreference, "treeview2");
	
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview1));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	if (gtk_tree_selection_get_selected (select, &model, &iter))				
		gtk_list_store_remove(GTK_LIST_STORE(store_auto_app), &iter);
}

void xneur_rem_manual_app(void)
{
	GtkTreeIter	 iter;
	GtkTreeSelection *select;	
	GtkTreeModel *model = GTK_TREE_MODEL (store_manual_app);
	GtkWidget *treeview1;
	treeview1 = lookup_widget(wPreference, "treeview3");
	
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview1));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	if (gtk_tree_selection_get_selected (select, &model, &iter))				
		gtk_list_store_remove(GTK_LIST_STORE(store_manual_app), &iter);
}

gboolean save_exclude_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	gchar *ptr;
	if (model || path || user_data){};
	gtk_tree_model_get(GTK_TREE_MODEL(store_exclude_app), iter, 0, &ptr, -1);
	
	xconfig->excluded_apps->add(xconfig->excluded_apps, ptr);

	g_free(ptr);

	return FALSE;
}

gboolean save_auto_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	gchar *ptr;
	if (model || path || user_data){};
	gtk_tree_model_get(GTK_TREE_MODEL(store_auto_app), iter, 0, &ptr, -1);

	xconfig->auto_apps->add(xconfig->auto_apps, ptr);

	g_free(ptr);

	return FALSE;
}

gboolean save_manual_app(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	gchar *ptr;
	if (model || path || user_data){};
	gtk_tree_model_get(GTK_TREE_MODEL(store_manual_app), iter, 0, &ptr, -1);

	xconfig->manual_apps->add(xconfig->manual_apps, ptr);

	g_free(ptr);

	return FALSE;
}

gchar *get_dir_by_index(int index)
{
	if (index == 0)
		return NULL;
	if (index == 1)
		return "en";
	if (index == 2)
		return "ru";
	if (index == 3)
		return "uk";
	if (index == 4)
		return "be";
	if (index == 5)
		return "fr";
	if (index == 6)
		return "ro";
	return NULL;
}

void xneur_save_preference(void)
{
	GtkWidget *widgetPtrToBefound1;
	GtkWidget *widgetPtrToBefound3;
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox1");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0)
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
		lang_name[total_lang] = (char *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(widgetPtrToBefound1));
		lang_dir[total_lang] = (char *)get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)));
		lang_group[total_lang] = gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound3));
		total_lang++;
	}
	
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox22");
	widgetPtrToBefound3 = lookup_widget(wPreference, "combobox14");
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)) > 0)
	{
		lang_name[total_lang] = (char *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(widgetPtrToBefound1));
		lang_dir[total_lang] = get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)));
		lang_group[total_lang] = gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound3));
		total_lang++;
	}
	
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox23");
	widgetPtrToBefound3 = lookup_widget(wPreference, "combobox15");
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)) > 0)
	{
		lang_name[total_lang] = (char *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(widgetPtrToBefound1));
		lang_dir[total_lang] = get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)));
		lang_group[total_lang] = gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound3));
		total_lang++;
	}
	
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox24");
	widgetPtrToBefound3 = lookup_widget(wPreference, "combobox16");
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)) > 0)
	{
		lang_name[total_lang] = (char *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(widgetPtrToBefound1));
		lang_dir[total_lang] = get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound1)));
		lang_group[total_lang] = gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound3));
		total_lang++;
	}
	xconfig->set_languages(xconfig, lang_name, lang_dir, lang_group, total_lang);

	xconfig->excluded_apps->uninit(xconfig->excluded_apps);
	xconfig->excluded_apps = list_init();
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_exclude_app), save_exclude_app, NULL);
		
	xconfig->auto_apps->uninit(xconfig->auto_apps);
	xconfig->auto_apps = list_init();
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_auto_app), save_auto_app, NULL);
		
	xconfig->manual_apps->uninit(xconfig->manual_apps);
	xconfig->manual_apps = list_init();
	gtk_tree_model_foreach(GTK_TREE_MODEL(store_manual_app), save_manual_app, NULL);
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox3");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0)
	{
		xconfig->hotkeys[0].key = KEY_NONE;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 1) 
	{
		xconfig->hotkeys[0].key = KEY_BREAK;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 2)
	{
		xconfig->hotkeys[0].key = KEY_SCROLL_LOCK;
	}	
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 3) 
	{
		xconfig->hotkeys[0].key = KEY_PRINT_SCREEN;
	}	
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox10");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0) 
	{
		xconfig->hotkeys[0].modifier = MODIFIER_NONE;
	}	
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 1)
	{
		xconfig->hotkeys[0].modifier = MODIFIER_SHIFT;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 2)
	{
		xconfig->hotkeys[0].modifier = MODIFIER_CONTROL;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 3)
	{
		xconfig->hotkeys[0].modifier = MODIFIER_ALT;	
	}
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox5");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0)
	{
		xconfig->hotkeys[1].key = KEY_NONE;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 1) 
	{
		xconfig->hotkeys[1].key = KEY_BREAK;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 2)
	{
		xconfig->hotkeys[1].key = KEY_SCROLL_LOCK;
	}	
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 3) 
	{
		xconfig->hotkeys[1].key = KEY_PRINT_SCREEN;
	}	
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox11");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0) 
	{
		xconfig->hotkeys[1].modifier = MODIFIER_NONE;
	}	
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 1)
	{
		xconfig->hotkeys[1].modifier = MODIFIER_SHIFT;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 2)
	{
		xconfig->hotkeys[1].modifier = MODIFIER_CONTROL;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 3)
	{
		xconfig->hotkeys[1].modifier = MODIFIER_ALT;	
	}
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox7");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0)
	{
		xconfig->hotkeys[2].key = KEY_NONE;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 1) 
	{
		xconfig->hotkeys[2].key = KEY_BREAK;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 2)
	{
		xconfig->hotkeys[2].key = KEY_SCROLL_LOCK;
	}	
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 3) 
	{
		xconfig->hotkeys[2].key = KEY_PRINT_SCREEN;
	}	
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox6");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0) 
	{
		xconfig->hotkeys[2].modifier = MODIFIER_NONE;
	}	
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 1)
	{
		xconfig->hotkeys[2].modifier = MODIFIER_SHIFT;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 2)
	{
		xconfig->hotkeys[2].modifier = MODIFIER_CONTROL;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 3)
	{
		xconfig->hotkeys[2].modifier = MODIFIER_ALT;	
	}
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox9");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0)
	{
		xconfig->hotkeys[3].key = KEY_NONE;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 1) 
	{
		xconfig->hotkeys[3].key = KEY_BREAK;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 2)
	{
		xconfig->hotkeys[3].key = KEY_SCROLL_LOCK;
	}	
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 3) 
	{
		xconfig->hotkeys[3].key = KEY_PRINT_SCREEN;
	}	
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox8");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0) 
	{
		xconfig->hotkeys[3].modifier = MODIFIER_NONE;
	}	
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 1)
	{
		xconfig->hotkeys[3].modifier = MODIFIER_SHIFT;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 2)
	{
		xconfig->hotkeys[3].modifier = MODIFIER_CONTROL;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 3)
	{
		xconfig->hotkeys[3].modifier = MODIFIER_ALT;	
	}
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox18");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0)
	{
		xconfig->hotkeys[4].key = KEY_NONE;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 1) 
	{
		xconfig->hotkeys[4].key = KEY_BREAK;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 2)
	{
		xconfig->hotkeys[4].key = KEY_SCROLL_LOCK;
	}	
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 3) 
	{
		xconfig->hotkeys[4].key = KEY_PRINT_SCREEN;
	}	
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox17");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0) 
	{
		xconfig->hotkeys[4].modifier = MODIFIER_NONE;
	}	
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 1)
	{
		xconfig->hotkeys[4].modifier = MODIFIER_SHIFT;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 2)
	{
		xconfig->hotkeys[4].modifier = MODIFIER_CONTROL;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 3)
	{
		xconfig->hotkeys[4].modifier = MODIFIER_ALT;	
	}
		
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox20");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0)
	{
		xconfig->hotkeys[5].key = KEY_NONE;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 1) 
	{
		xconfig->hotkeys[5].key = KEY_BREAK;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 2)
	{
		xconfig->hotkeys[5].key = KEY_SCROLL_LOCK;
	}	
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 3) 
	{
		xconfig->hotkeys[5].key = KEY_PRINT_SCREEN;
	}	
	
	widgetPtrToBefound1 = lookup_widget(wPreference, "combobox19");
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 0) 
	{
		xconfig->hotkeys[5].modifier = MODIFIER_NONE;
	}	
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 1)
	{
		xconfig->hotkeys[5].modifier = MODIFIER_SHIFT;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 2)
	{
		xconfig->hotkeys[5].modifier = MODIFIER_CONTROL;
	}
	else if (gtk_combo_box_get_active (GTK_COMBO_BOX(widgetPtrToBefound1)) == 3)
	{
		xconfig->hotkeys[5].modifier = MODIFIER_ALT;	
	}
		
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
		xconfig->save_selection_mode = SAVESELECTION_ENABLE;
	else
		xconfig->save_selection_mode = SAVESELECTION_DISABLE;
		
	xconfig->save(xconfig);
	xneur_restart();
	gtk_widget_destroy(wPreference);
}

gchar *xneur_get_file_path(int layout_no, const char *file_name)
{
	GtkWidget *widgetPtrToBefound = NULL;
	char *dir = malloc(3 * sizeof(char));
	const char *language_name_boxes[MAX_LANGUAGES] = {"combobox21", "combobox22", "combobox23", "combobox24"};
	widgetPtrToBefound = lookup_widget(wPreference, language_name_boxes[layout_no]);
	dir = get_dir_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(widgetPtrToBefound)));			
	return xconfig->get_file_path_name(dir, file_name);
}

gchar *xneur_get_file_content(const char *path)
{
	if (path == NULL)
		return NULL;
	
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
