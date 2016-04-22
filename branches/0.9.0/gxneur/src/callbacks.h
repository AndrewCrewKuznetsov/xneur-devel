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

#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include <gtk/gtk.h>

void on_button6_clicked(GtkButton *button, gpointer user_data);
void on_button7_clicked(GtkButton *button, gpointer user_data);
void on_button8_clicked(GtkButton *button, gpointer user_data);
void on_button9_clicked(GtkButton *button, gpointer user_data);
void on_button23_clicked(GtkButton *button, gpointer user_data);
void on_button24_clicked(GtkButton *button, gpointer user_data);
void on_button25_clicked(GtkButton *button, gpointer user_data);
void on_button26_clicked(GtkButton *button, gpointer user_data);
void on_okbutton1_clicked(GtkButton *button, gpointer user_data);
void on_cancelbutton1_clicked(GtkButton *button, gpointer user_data);

gboolean on_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean on_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data);

void on_button11_clicked(GtkButton *button, gpointer user_data);
void on_button12_clicked(GtkButton *button, gpointer user_data);
void on_button13_clicked(GtkButton *button, gpointer user_data);
void on_button14_clicked(GtkButton *button, gpointer user_data);
void on_button15_clicked(GtkButton *button, gpointer user_data);
void on_button16_clicked(GtkButton *button, gpointer user_data);
void on_button17_clicked(GtkButton *button, gpointer user_data);
void on_button18_clicked(GtkButton *button, gpointer user_data);
void on_button29_clicked(GtkButton *button, gpointer user_data);
void on_button30_clicked(GtkButton *button, gpointer user_data);

void on_window2_delete_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

#endif /* _CALLBACKS_H_ */
