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

#ifndef _MISC_H_
#define _MISC_H_

struct _tray_icon;

void xneur_restart(void);

gchar *xneur_get_dict(int layout_no);
gchar *xneur_get_dict_path(int layout_no);

// Callback functions
void xneur_exit(void);
void xneur_start_stop(GtkWidget *widget, struct _tray_icon *tray);
void xneur_preference(void);
void xneur_about(void);

void xneur_add_application(void);
void xneur_rem_application(void);

void xneur_save_preference(void);

#endif /* _MISC_H_ */
