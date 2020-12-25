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
 *  Copyright (C) 2006-2016 XNeur Team
 *
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include "plugin.h"

#ifdef WITH_PLUGINS

#include "types.h"
#include "log.h"

extern struct _xneur_config *xconfig;

void plugin_add(struct _plugin *p, char* plugin_name)
{
	void *tmp = realloc(p->plugin, (p->plugin_count + 1) * sizeof(struct _plugin_functions));
	if (tmp == NULL)
		return;
	p->plugin = (struct _plugin_functions *)tmp;

	size_t len = strlen(XNEUR_PLUGIN_DIR) + strlen(plugin_name) + 2;
	char * plugin_file = malloc(sizeof(char)*len);
	if (plugin_file == NULL)
		return;
	snprintf(plugin_file, len, "%s/%s", XNEUR_PLUGIN_DIR, plugin_name);
	p->plugin[p->plugin_count].module = dlopen(plugin_file, RTLD_NOW);

	if(!p->plugin[p->plugin_count].module)
	{
		log_message (ERROR, "Can't load module %s", plugin_file);
		free(plugin_file);
		return;
	}

	free(plugin_file);

	/* Get functions adresses */
	p->plugin[p->plugin_count].on_init = NULL;
	p->plugin[p->plugin_count].on_init = dlsym(p->plugin[p->plugin_count].module, "on_init");

	p->plugin[p->plugin_count].on_fini = NULL;
	p->plugin[p->plugin_count].on_fini = dlsym(p->plugin[p->plugin_count].module, "on_fini");

	p->plugin[p->plugin_count].on_xneur_start = NULL;
	p->plugin[p->plugin_count].on_xneur_start = dlsym(p->plugin[p->plugin_count].module, "on_xneur_start");

	p->plugin[p->plugin_count].on_xneur_reload = NULL;
	p->plugin[p->plugin_count].on_xneur_reload = dlsym(p->plugin[p->plugin_count].module, "on_xneur_reload");

	p->plugin[p->plugin_count].on_xneur_stop = NULL;
	p->plugin[p->plugin_count].on_xneur_stop = dlsym(p->plugin[p->plugin_count].module, "on_xneur_stop");

	p->plugin[p->plugin_count].on_key_press = NULL;
	p->plugin[p->plugin_count].on_key_press = dlsym(p->plugin[p->plugin_count].module, "on_key_press");

	p->plugin[p->plugin_count].on_key_release = NULL;
	p->plugin[p->plugin_count].on_key_release = dlsym(p->plugin[p->plugin_count].module, "on_key_release");

	p->plugin[p->plugin_count].on_hotkey_action = NULL;
	p->plugin[p->plugin_count].on_hotkey_action = dlsym(p->plugin[p->plugin_count].module, "on_hotkey_action");

	p->plugin[p->plugin_count].on_change_action = NULL;
	p->plugin[p->plugin_count].on_change_action = dlsym(p->plugin[p->plugin_count].module, "on_change_action");

	p->plugin[p->plugin_count].on_plugin_reload = NULL;
	p->plugin[p->plugin_count].on_plugin_reload = dlsym(p->plugin[p->plugin_count].module, "on_plugin_reload");

	p->plugin[p->plugin_count].on_plugin_configure = NULL;
	p->plugin[p->plugin_count].on_plugin_configure = dlsym(p->plugin[p->plugin_count].module, "on_plugin_configure");

	p->plugin[p->plugin_count].on_plugin_about = NULL;
	p->plugin[p->plugin_count].on_plugin_about = dlsym(p->plugin[p->plugin_count].module, "on_plugin_about");

	p->plugin[p->plugin_count].on_plugin_info = NULL;
	p->plugin[p->plugin_count].on_plugin_info = dlsym(p->plugin[p->plugin_count].module, "on_plugin_info");

	// Run init of plugin
	p->plugin[p->plugin_count].on_init();

	p->plugin_count++;
}

void plugin_xneur_start(struct _plugin *p)
{
	for (int i=0; i<p->plugin_count; i++)
	{
		if (p->plugin[i].on_xneur_start == NULL)
			continue;

		p->plugin[i].on_xneur_start();
	}
}

void plugin_xneur_reload(struct _plugin *p)
{
	for (int i=0; i<p->plugin_count; i++)
	{
		if (p->plugin[i].on_xneur_reload == NULL)
			continue;

		p->plugin[i].on_xneur_reload();
	}
}

void plugin_xneur_stop(struct _plugin *p)
{
	for (int i=0; i<p->plugin_count; i++)
	{
		if (p->plugin[i].on_xneur_stop == NULL)
			continue;

		p->plugin[i].on_xneur_stop();
	}
}

void plugin_key_press(struct _plugin *p, KeySym key, int modifier_mask)
{
	for (int i=0; i<p->plugin_count; i++)
	{
		if (p->plugin[i].on_key_press == NULL)
			continue;

		p->plugin[i].on_key_press(key, modifier_mask);
	}
}

void plugin_key_release(struct _plugin *p, KeySym key, int modifier_mask)
{
	for (int i=0; i<p->plugin_count; i++)
	{
		if (p->plugin[i].on_key_release == NULL)
			continue;

		p->plugin[i].on_key_release(key, modifier_mask);
	}
}

void plugin_hotkey_action(struct _plugin *p, enum _hotkey_action ha)
{
	for (int i=0; i<p->plugin_count; i++)
	{
		if (p->plugin[i].on_hotkey_action == NULL)
			continue;

		p->plugin[i].on_hotkey_action(ha);
	}
}

void plugin_change_action(struct _plugin *p, enum _change_action ca)
{
	for (int i=0; i<p->plugin_count; i++)
	{
		if (p->plugin[i].on_change_action == NULL)
			continue;

		p->plugin[i].on_change_action(ca);
	}
}

void plugin_plugin_reload(struct _plugin *p)
{
	for (int i=0; i<p->plugin_count; i++)
	{
		if (p->plugin[i].on_plugin_reload == NULL)
			continue;

		p->plugin[i].on_plugin_reload();
		dlclose(p->plugin[i].module);
	}

	free(p->plugin);
}

void plugin_plugin_configure(struct _plugin *p)
{
	for (int i=0; i<p->plugin_count; i++)
	{
		if (p->plugin[i].on_plugin_configure == NULL)
			continue;

		p->plugin[i].on_plugin_configure();
	}
}

void plugin_plugin_about(struct _plugin *p)
{
	for (int i=0; i<p->plugin_count; i++)
	{
		if (p->plugin[i].on_plugin_about == NULL)
			continue;

		p->plugin[i].on_plugin_about();
	}
}

void plugin_plugin_info(struct _plugin *p)
{
	for (int i=0; i<p->plugin_count; i++)
	{
		if (p->plugin[i].on_plugin_info == NULL)
			continue;

		p->plugin[i].on_plugin_info();
	}
}

void plugin_uninit(struct _plugin *p)
{
	for (int i=0; i<p->plugin_count; i++)
	{
		if (p->plugin[i].on_fini != NULL)
			p->plugin[i].on_fini();

		dlclose(p->plugin[i].module);
	}

	free(p->plugin);
	free(p);

	log_message(DEBUG, _("Plugins is freed"));
}

#else /* WITH_PLUGINS */

void plugin_add(struct _plugin *p, char* plugin_name)
{
	if (p || plugin_name) {};
}

void plugin_xneur_start(struct _plugin *p)
{
	if (p) {};
}

void plugin_xneur_reload(struct _plugin *p)
{
	if (p) {};
}

void plugin_xneur_stop(struct _plugin *p)
{
	if (p) {};
}

void plugin_key_press(struct _plugin *p, KeySym key, int modifier_mask)
{
	if (p || key || modifier_mask) {};
}

void plugin_key_release(struct _plugin *p, KeySym key, int modifier_mask)
{
	if (p || key || modifier_mask) {};
}

void plugin_hotkey_action(struct _plugin *p, enum _hotkey_action ha)
{
	if (p || ha) {};
}

void plugin_change_action(struct _plugin *p, enum _change_action ca)
{
	if (p || ca) {};
}

void plugin_plugin_reload(struct _plugin *p)
{
	if (p) {};
}

void plugin_plugin_configure(struct _plugin *p)
{
	if (p) {};
}

void plugin_plugin_about(struct _plugin *p)
{
	if (p) {};
}

void plugin_plugin_info(struct _plugin *p)
{
	if (p) {};
}

void plugin_uninit(struct _plugin *p)
{
	if (p) {};
}

#endif /* WITH_PLUGINS */

struct _plugin* plugin_init(void)
{
	struct _plugin *p = (struct _plugin *) malloc(sizeof(struct _plugin));
	memset(p, 0, sizeof(struct _plugin));

	p->plugin = (struct _plugin_functions *) malloc(sizeof(struct _plugin_functions));
	memset(p->plugin, 0, sizeof(struct _plugin_functions));

	// Function mapping

	p->add = plugin_add;

	p->xneur_start = plugin_xneur_start;
	p->xneur_reload = plugin_xneur_reload;
	p->xneur_stop = plugin_xneur_stop;
	p->key_press = plugin_key_press;
	p->key_release = plugin_key_release;
	p->hotkey_action = plugin_hotkey_action;
	p->change_action = plugin_change_action;
	p->plugin_reload = plugin_plugin_reload;
	p->plugin_configure = plugin_plugin_configure;
	p->plugin_about = plugin_plugin_about;

	p->uninit		= plugin_uninit;

	return p;
}
