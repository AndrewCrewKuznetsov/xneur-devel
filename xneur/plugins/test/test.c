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
 *  Copyright (C) 2006-2010 XNeur Team
 *
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef WITH_PLUGINS

#include <X11/keysym.h>
#include <X11/XKBlib.h>

#include <stdio.h>

#include "xnconfig.h"

extern struct _xneur_config *xconfig;

int on_init(void)
{
	printf("[PLG] Plugin for test receive initialisation\n");
	return (0);
}

int on_xneur_start(void)
{
	printf("[PLG] Plugin receive xneur start\n");
	return (0);
}

int on_xneur_reload(void)
{
	printf("[PLG] Plugin receive xneur reload\n");
	return (0);
}

int on_xneur_stop(void)
{
	printf("[PLG] Plugin receive xneur stop\n");
	return (0);
}

int on_key_press(KeySym key, int modifier_mask)
{
	printf("[PLG] Plugin receive KeyPress '%s' with mask %d\n", XKeysymToString(key), modifier_mask);
	return (0);
}

int on_key_release(KeySym key, int modifier_mask)
{
	printf("[PLG] Plugin receive KeyRelease '%s' with mask %d\n", XKeysymToString(key), modifier_mask);
	return (0);
}

int on_hotkey_action(enum _hotkey_action ha)
{
	printf("[PLG] Plugin receive Hotkey Action '%d'\n", ha);
	return (0);
}

int on_change_action(enum _change_action ca)
{
	printf("[PLG] Plugin receive Change Action '%d'\n", ca);
	return (0);
}

int on_plugin_reload(void)
{
	printf("[PLG] Plugin reload\n");
	return (0);
}

int on_plugin_configure(void)
{
	printf("[PLG] Plugin configure\n");
	return (0);
}

int on_plugin_about(void)
{
	printf("[PLG] Plugin about\n");
	return (0);
}

char *on_plugin_info(void)
{
	return ("Test plugin");
}

int on_fini(void)
{
	printf("[PLG] Plugin receive finalisation\n");
	return (0);
}

#endif /* WITH_PLUGINS */
