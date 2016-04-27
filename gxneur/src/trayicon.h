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

#ifndef _TRAYICON_H_
#define _TRAYICON_H_

#include "tray_widget.h"

#define MAX_LAYOUTS 4

#ifdef HAVE_APP_INDICATOR
#include <libappindicator/app-indicator.h>
#endif

struct _tray_icon
{
#ifdef HAVE_APP_INDICATOR
	// App Indicator part
	AppIndicator *app_indicator;
#endif
	
	// Tray part
	GtkTrayIcon *tray_icon;
	GtkTooltips *tooltip;
	GtkWidget *image;
	GtkWidget *evbox;
	
	// Status Icon part
	GtkStatusIcon *status_icon;

	// All
	//gchar  *images[MAX_LAYOUTS];
	GtkMenu *menu;
	GtkWidget *status;
	
	gint height;
};

void create_tray_icon(void);
void xneur_start_stop(void);
void xneur_auto_manual(void);
void xneur_exit(void);

#endif /* _TRAYICON_H_ */
