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

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

#include "configuration.h"


#define UNUSED(x) (void)(x)

#ifdef HAVE_GCONF

#include <gconf/gconf-client.h>

static GConfClient* _gconfClient = NULL;

static GConfClient* gconfClient(void)
{
	if (!_gconfClient) {
		_gconfClient = gconf_client_get_default();
		g_assert(GCONF_IS_CLIENT(_gconfClient));
		gconf_client_add_dir(_gconfClient, PACKAGE_GCONF_DIR, GCONF_CLIENT_PRELOAD_ONELEVEL, NULL);
	}
	return _gconfClient;
}


static gboolean gxneur_config_enabled = TRUE;

int gxneur_config_read_int(const char* key, int* value)
{
	if (!gxneur_config_enabled)
		return CONFIG_NOT_SUPPORTED;

	if (!key || !value)
		return -2;

	int result = -1;

	gchar* k = g_strdup_printf("%s/%s", PACKAGE_GCONF_DIR, key);
	g_assert(k != NULL);

	GConfValue* gcValue = gconf_client_get_without_default(gconfClient(),  k, NULL);

	if(gcValue != NULL) 
	{
		if(gcValue->type == GCONF_VALUE_INT)
			*value = gconf_value_get_int(gcValue),
			result = 0;
		gconf_value_free(gcValue);
	}

	g_free(k);
	
	return result;
}

int gxneur_config_read_str(const char* key, gchar** value)
{
	if (!gxneur_config_enabled)
		return CONFIG_NOT_SUPPORTED;

	if (!key || !value)
		return -2;

	int result = -1;

	gchar* k = g_strdup_printf("%s/%s", PACKAGE_GCONF_DIR, key);
	g_assert(k != NULL);

	GConfValue* gcValue = gconf_client_get_without_default(gconfClient(),  k, NULL);

	if(gcValue != NULL) 
	{
		if(gcValue->type == GCONF_VALUE_STRING)
			g_free(*value),
			*value = g_strdup(gconf_value_get_string(gcValue));
			result = 0;
		gconf_value_free(gcValue);
	}

	g_free(k);
	
	return result;
}

int gxneur_config_write_int(const char* key, int value, gboolean send_notify)
{
	if (!gxneur_config_enabled)
		return CONFIG_NOT_SUPPORTED;

	if (!key)
		return -2;

	int result = 0;

	gchar* k = g_strdup_printf("%s/%s", PACKAGE_GCONF_DIR, key);
	g_assert(k != NULL);

	if(!gconf_client_set_int(gconfClient(), k, value, NULL)) 
		    g_warning("Failed to set %s (%d)\n", k, value),
		    result = -1;
	else if (send_notify)
		gconf_client_notify(gconfClient(), k);

	g_free(k);

	return result;
}

int gxneur_config_write_str(const char* key, const char* value, gboolean send_notify)
{
	if (!gxneur_config_enabled)
		return CONFIG_NOT_SUPPORTED;

	if (!key || !value)
		return -2;

	int result = 0;

	gchar* k = g_strdup_printf("%s/%s", PACKAGE_GCONF_DIR, key);
	g_assert(k != NULL);

	if(!gconf_client_set_string(gconfClient(), k, value, NULL)) 
		    g_warning("Failed to set %s (%s)\n", k, value),
		    result = -1;
	else if (send_notify)
		gconf_client_notify(gconfClient(), k);

	g_free(k);

	return result;
}

typedef struct _callback_t
{
	gxneur_config_notify_callback callback;
	gpointer payload;
	
} callback_t;

static
void gconf_callback(GConfClient* client,
                            guint cnxn_id,
                            GConfEntry* entry,
                            gpointer user_data)
{
	UNUSED(client || cnxn_id || entry);

	callback_t *callback = (callback_t*)user_data;
	g_assert(callback != NULL);
	
	(callback->callback)(callback->payload);
}


int gxneur_config_add_notify(const char* key, gxneur_config_notify_callback callback, gpointer payload)
{
	if (!gxneur_config_enabled)
		return CONFIG_NOT_SUPPORTED;

	gchar* k = g_strdup_printf("%s/%s", PACKAGE_GCONF_DIR, key);
	g_assert(k != NULL);

	callback_t* callback_data = (callback_t*)malloc(sizeof(callback_t));
	g_assert(callback_data != NULL);

	callback_data->callback = callback;
	callback_data->payload = payload;

	gconf_client_notify_add(gconfClient(),
                          k,
                          gconf_callback,
                          callback_data,
                          NULL,
                          NULL);

	g_free(k);

	return 0;
}

void gxneur_config_set_enabled(gboolean enabled)
{
	gxneur_config_enabled = enabled;
}

#else

int gxneur_config_read_int(const char* key, int* value)
{
	UNUSED(key || value);
	return CONFIG_NOT_SUPPORTED;
}

int gxneur_config_read_str(const char* key, gchar** value)
{
	UNUSED(key || value);
	return CONFIG_NOT_SUPPORTED;
}


int gxneur_config_write_int(const char* key, int value, gboolean send_notify)
{
	UNUSED(key || value || send_notify);
	return CONFIG_NOT_SUPPORTED;
}

int gxneur_config_write_str(const char* key, const char* value, gboolean send_notify)
{
	UNUSED(key || value || send_notify);
	return CONFIG_NOT_SUPPORTED;
}

int gxneur_config_add_notify(const char* key, gxneur_config_notify_callback callback, gpointer payload)
{
	UNUSED(key || callback || payload);
	return CONFIG_NOT_SUPPORTED;
}

void gxneur_config_set_enabled(gboolean enabled)
{
	UNUSED(enabled);
}

#endif

