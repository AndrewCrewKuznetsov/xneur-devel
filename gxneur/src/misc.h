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
 
#ifndef _MISC_H_
#define _MISC_H_

#define UI_FILE_ABOUT PACKAGE_UI_FILE_DIR"/about.ui"
#define UI_FILE_CONFIG PACKAGE_UI_FILE_DIR"/config.ui"
#define UI_FILE_ABBREVIATION_ADD PACKAGE_UI_FILE_DIR"/abbr_add.ui"
#define UI_FILE_CHOOSE PACKAGE_UI_FILE_DIR"/choose_file.ui"
#define UI_FILE_USR_ACTION_ADD PACKAGE_UI_FILE_DIR"/usr_action_add.ui"
#define UI_FILE_ACTION_ADD PACKAGE_UI_FILE_DIR"/action_add.ui"
#define UI_FILE_APP_ADD PACKAGE_UI_FILE_DIR"/app_add.ui"
#define UI_FILE_RULE_ADD PACKAGE_UI_FILE_DIR"/rule_add.ui"
#define UI_FILE_LIST PACKAGE_UI_FILE_DIR"/list.ui"

char* xneur_get_file_content(const char *path);
char* xneur_get_dict_path(GtkBuilder* builder, int layout_no, const char *file_name);
char* xneur_get_home_dict_path(GtkBuilder* builder, int layout_no, const char *file_name);
char* xneur_modifiers_to_string(unsigned int modifier);

//void xneur_exit(void);
void xneur_start(void);
void xneur_preference(void);
void xneur_kb_preference(void);
void xneur_get_logfile(void);
void xneur_about(void);

void xneur_add_exclude_app(void);
void xneur_rem_exclude_app(GtkWidget *widget);

void xneur_add_auto_app(void);
void xneur_rem_auto_app(GtkWidget *widget);

void xneur_add_manual_app(void);
void xneur_rem_manual_app(GtkWidget *widget);

void xneur_add_dont_send_keyrelease_app(void);
void xneur_rem_dont_send_keyrelease_app(GtkWidget *widget);

void xneur_add_delay_send_key_app(void);
void xneur_rem_delay_send_key_app(GtkWidget *widget);

void xneur_add_layout_app(void);
void xneur_rem_layout_app(GtkWidget *widget);

void xneur_add_draw_flag_app(void);
void xneur_rem_draw_flag_app(GtkWidget *widget);

void xneur_add_abbreviation(void);
void xneur_rem_abbreviation(GtkWidget *widget);

void xneur_add_user_action(void);
void xneur_rem_user_action(GtkWidget *widget);
void xneur_edit_user_action(GtkWidget *treeview);

void xneur_add_autocompletion_exclude_app(void);
void xneur_rem_autocompletion_exclude_app(GtkWidget *widget);

void xneur_add_action(void);
void xneur_edit_action(GtkWidget *treeview);
void xneur_clear_action(GtkWidget *treeview);
void xneur_rem_action(GtkWidget *widget);

void xneur_save_preference(GtkBuilder* builder);
void xneur_dontsave_preference(GtkBuilder* builder);

void xneur_edit_sound(GtkWidget *treeview);

int is_correct_hotkey(gchar **key_stat);

char* modifiers_to_string(unsigned int modifier);

void error_msg(const char *msg, ...);

#endif /* _MISC_H_ */
