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

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _Clock		Clock;
typedef struct _ClockClass	ClockClass;

typedef void (callback_func) (Clock *clock);

struct _Clock
{
	GtkObject parent;

	struct _tray_icon *tray;

	void (*on_timer) (Clock *clock);
};

struct _ClockClass
{
	GtkObjectClass parent_class;
};

GtkWidget *clock_new(callback_func callback, struct _tray_icon *data);
	
G_END_DECLS

#endif /* _CLOCK_H_ */
