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


#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <gio/gio.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gtk/gtk.h>
#pragma GCC diagnostic pop
#include <gdk/gdkx.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

#include "configuration.h"

GSettings *_gsettingsSet = NULL;

static GSettings* gsettingsSet(void)
{
	if (!_gsettingsSet) {
		_gsettingsSet = g_settings_new(PACKAGE_GSETTINGS_DIR);
	}
	return _gsettingsSet;
}

static gboolean gxneur_config_enabled = TRUE;

int gxneur_config_read_int(const char* key, int* value)
{
	if (!gxneur_config_enabled)
		return CONFIG_NOT_SUPPORTED;

	if (!key || !value)
		return -2;

	*value = (int) g_settings_get_int(gsettingsSet(), key);
	return 0;
}

int gxneur_config_read_str(const char* key, gchar** value)
{
	if (!gxneur_config_enabled)
		return CONFIG_NOT_SUPPORTED;

	if (!key || !value)
		return -2;

	*value = g_settings_get_string(gsettingsSet(), key);
	return 0;
}

int gxneur_config_write_int(const char* key, int value)
{
	if (!gxneur_config_enabled)
		return CONFIG_NOT_SUPPORTED;

	if (!key)
		return -2;


	if(!g_settings_set_int(gsettingsSet(), key, value))
	{
		    g_warning("Failed to set %s (%d)\n", key, value);
		    return -1;
	}
	return 0;
}

int gxneur_config_write_str(const char* key, const char* value)
{
	if (!gxneur_config_enabled)
		return CONFIG_NOT_SUPPORTED;

	if (!key || !value)
		return -2;

	int result = 0;

	if(!g_settings_set_string(gsettingsSet(), key, value))
		    g_warning("Failed to set %s (%s)\n", key, value),
		    result = -1;

	return result;
}
typedef struct _callback_t
    {
    gxneur_config_notify_callback callback;
    gpointer payload;
	
    } callback_t;
static void gconf_callback(GConfClient* client,
                            guint cnxn_id,
                            GConfEntry* entry,
                            gpointer user_data)
    {
    UNUSED(client || cnxn_id || entry);
    callback_t *callback = (callback_t*)user_data;
    g_assert(callback != NULL);
	
    (callback->callback)(callback->payload);
    }

int gxneur_config_add_notify(const char* key, void* callback)
{
	if (!gxneur_config_enabled)
		return CONFIG_NOT_SUPPORTED;

	gchar* k = g_strdup_printf("changed::%s", key);
	g_assert(k != NULL);

	g_signal_connect (
				gsettingsSet(),
				k,
				G_CALLBACK (callback),
				NULL);

	g_free(k);

	return 0;
}

void gxneur_config_set_enabled(gboolean enabled)
{
	gxneur_config_enabled = enabled;
}
