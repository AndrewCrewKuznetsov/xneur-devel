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
 
#ifndef _MISC_H_
#define _MISC_H_

#include <glade/glade.h>

struct _tray_icon;

char* xneur_get_file_content(const char *path);
char* xneur_get_dict_path(GladeXML *gxml, int layout_no, const char *file_name);
char* xneur_get_home_dict_path(GladeXML *gxml, int layout_no, const char *file_name);
char* xneur_modifiers_to_string(unsigned int modifier);

void xneur_exit(void);
void xneur_start(void);
void xneur_preference(void);
void xneur_about(void);

void xneur_add_exclude_app(void);
void xneur_rem_exclude_app(GtkWidget *widget);

void xneur_add_auto_app(void);
void xneur_rem_auto_app(GtkWidget *widget);

void xneur_add_manual_app(void);
void xneur_rem_manual_app(GtkWidget *widget);

void xneur_add_layout_app(void);
void xneur_rem_layout_app(GtkWidget *widget);

void xneur_add_draw_flag_app(void);
void xneur_rem_draw_flag_app(GtkWidget *widget);

void xneur_save_preference(GladeXML *gxml);
void xneur_dontsave_preference(GladeXML *gxml);

void xneur_start_stop(GtkWidget *widget, struct _tray_icon *tray);
void xneur_auto_manual(GtkWidget *widget, struct _tray_icon *tray);

int is_correct_hotkey(gchar **key_stat);

char* modifiers_to_string(unsigned int modifier);

#endif /* _MISC_H_ */
