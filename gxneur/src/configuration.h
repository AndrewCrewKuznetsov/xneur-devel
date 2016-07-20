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
 *  Copyright (C) 2006-2011 XNeur Team
 *
 */
 
#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#ifdef HAVE_APP_INDICATOR
	#define DEFAULT_RENDERING_ENGINE "AppIndicator"
#else
	#define DEFAULT_RENDERING_ENGINE "StatusIcon"
#endif

#define DEFAULT_SHOW_IN_THE_TRAY "Flag"
#define DEFAULT_DELAY 0
#define DEFAULT_KEYBOARD_PROPERTIES KB_PROP_COMMAND

#define CONFIG_NOT_SUPPORTED (-4)

typedef void (*gxneur_config_notify_callback)(void* payload);

int gxneur_config_read_int(const char* key, int* value);
int gxneur_config_read_str(const char* key, gchar** value);

int gxneur_config_write_int(const char* key, int value, gboolean send_notify);
int gxneur_config_write_str(const char* key, const char* value, gboolean send_notify);

int gxneur_config_add_notify(const char* key, gxneur_config_notify_callback callback, gpointer payload);

void gxneur_config_set_enabled(gboolean enabled);

#endif /* _CONFIGURATION_H_ */
