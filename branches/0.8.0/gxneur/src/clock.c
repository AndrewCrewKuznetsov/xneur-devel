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

#include <gtk/gtk.h>

#include "trayicon.h"

#include "clock.h"

#define CLOCK(obj)		G_TYPE_CHECK_INSTANCE_CAST((obj), clock_get_type(), Clock)
#define TIMER_PERIOD		100

static callback_func *on_timer = NULL;
static struct _tray_icon *tray = NULL;

G_DEFINE_TYPE(Clock, clock, GTK_TYPE_OBJECT);

gboolean clock_update(gpointer data)
{
	Clock *clock = CLOCK(data);

	clock->on_timer(clock);
	
	return TRUE;
}

void clock_class_init(ClockClass *klass)
{
	if (klass){}
}

void clock_init(Clock *clock)
{
	clock->on_timer	= on_timer;
	clock->tray	= tray;

	clock_update(clock);
	g_timeout_add(TIMER_PERIOD, clock_update, clock);
}

GtkWidget *clock_new(callback_func *callback, struct _tray_icon *data)
{
	on_timer = callback;
	tray = data;

	return g_object_new(clock_get_type(), NULL);
}
