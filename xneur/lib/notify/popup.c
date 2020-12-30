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

#ifdef WITH_LIBNOTIFY

#include <libnotify/notify.h>

#ifndef NOTIFY_CHECK_VERSION /* macro did not exist before libnotify-0.5.2 */
#  define NOTIFY_CHECK_VERSION(x,y,z) 0
#endif

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "xnconfig.h"

#include "debug.h"
#include "log.h"

#include "popup.h"

extern struct _xneur_config *xconfig;

static const char *icon = "xneur";
static const char *type = NULL;
static const NotifyUrgency urgency = NOTIFY_URGENCY_LOW;

time_t timestamp = 0;

void popup_init(void)
{
	if (!xconfig->show_popup)
		return;

	notify_init("xneur");

}

void popup_uninit(void)
{
	if (!xconfig->show_popup)
		return;

	notify_uninit();
}

static void popup_show_thread(struct _popup_body *popup_body)
{
	if (popup_body->header == NULL)
	{
		popup_body->header = popup_body->content;
		popup_body->content = NULL;
	}

#if NOTIFY_CHECK_VERSION(0,7,0)
	NotifyNotification *notify = notify_notification_new(popup_body->header, popup_body->content, icon);
#else
 	NotifyNotification *notify = notify_notification_new(popup_body->header, popup_body->content, icon, NULL);
#endif

	notify_notification_set_category(notify, type);
	notify_notification_set_urgency(notify, urgency);
	notify_notification_set_timeout(notify, xconfig->popup_expire_timeout);
	//notify_notification_set_timeout(notify, NOTIFY_EXPIRES_DEFAULT);

	notify_notification_show(notify, NULL);

	free(popup_body->header);
	free(popup_body->content);
	free(popup_body);

	notify_notification_clear_actions(notify);

	g_object_unref(G_OBJECT(notify));
}

void popup_show(int notify, char *command)
{
	if (!xconfig->show_popup)
		return;

	if ((xconfig->popups[notify].file == NULL) && (command == NULL))
		return;

	if (!xconfig->popups[notify].enabled)
		return;

	time_t curtime = time(NULL);
	if ((curtime - timestamp) < 2)
		return;

	timestamp = curtime;

	pthread_attr_t popup_thread_attr;
	pthread_attr_init(&popup_thread_attr);
	pthread_attr_setdetachstate(&popup_thread_attr, PTHREAD_CREATE_DETACHED);

	log_message(DEBUG, _("Show popup message \"%s\" with content \"%s\""), xconfig->popups[notify].file, command);

	struct _popup_body *popup_body = (struct _popup_body*) malloc(sizeof (struct _popup_body));
	popup_body->header = NULL;
	popup_body->content = NULL;
	if (xconfig->popups[notify].file != NULL)
		popup_body->header = strdup(xconfig->popups[notify].file);
	if (command != NULL)
		popup_body->content = strdup(command);

	pthread_t popup_thread;
	pthread_create(&popup_thread, &popup_thread_attr, (void*) &popup_show_thread, (void*)popup_body);

	pthread_attr_destroy(&popup_thread_attr);
}

#else /* WITH_LIBNOTIFY */

void popup_init(void)
{
	return;
}

void popup_show(int notify, char *command)
{
	if (notify || command) {};
	return;
}

void popup_uninit(void)
{
	return;
}
#endif /* WITH_LIBNOTIFY */
