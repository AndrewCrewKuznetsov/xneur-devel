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
 *  (c) Crew IT Research Labs 2004-2006
 *
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "xdefines.h"

#include "utils.h"
#include "log.h"
#include "list.h"
#include "files.h"

#include "xconfig.h"

#define DEFAULT_SCANNING_DELAY 20000    // microsec

const struct _bind DEFAULT_BT[] = { {NONE_KEY,NONE_KEY}, {PAUSE_KEY, NONE_KEY}, {PAUSE_KEY, SHIFT_KEY}, {SYS_RQ_KEY, CONTROL_KEY} };
const char *conf_names[] = {"DefaultMode", "AddApp", "AddBind", "ScanDelay", "LogLevel", "EngXkbGroup", "RusXkbGroup"};
const int total_confs = sizeof(conf_names) / sizeof(conf_names[0]);

void xconf_parse_line(struct _xconf *ptr, char *line)
{
	int len = strlen(line);
	while (len != 0 && (line[len - 1] == '\n' || line[len - 1] == ' '))
		line[--len] = NULLSYM;

        if (line[0] == '#' || line[0] == NULLSYM)
                return;

	char *conf = (char *) xnmalloc((len + 1) * sizeof(char));

	int i = 0;
	while (i < len && line[i] != ' ' && line[i] != NULLSYM)
	{
		conf[i] = line[i];
		i++;
	}
	conf[i] = NULLSYM;

	if (i >= len - 1)
	{
		log_message(WARNING, "Option '%s' must be with params", conf);
		free(conf);
		return;
	}

	char *params = line + i + 1;
	len = strlen(params);

	int cp = 0;
	for (i = 0; i < total_confs; i++)
	{
		if (strcmp(conf, conf_names[i]) == 0)
		{
			cp = i + 1;
			break;
		}
	}

	if (cp == 0)
	{
		log_message(WARNING, "Unrecognized option '%s' in config file", conf);
		free(conf);
		return;
	}

	free(conf);

	switch (cp)
	{
		case 1: // Get Default Mode (Manual/Auto)
		{
			int mode = atoi(params);
			if (mode == 0 || mode == 1)
				ptr->DefaultMode = mode;
			else
				log_message(WARNING, "Bad value '%s' for DefaultMode (values must 0 or 1), using default", params);
			break;
		}
		case 2: // Get Applications Names
		{
			list_add(ptr->apps, params, 0);
			break;
		}
		case 3: // Get Keyboard Binds
		{
			char *temp = (char *) xnmalloc(len + 1);

			i = 0;
			while (i < len && params[i] != ' ' && params[i] != NULLSYM)
			{
				temp[i] = params[i];
				i++;
			}
			temp[i] = NULLSYM;

			int action;
			if (strcmp(temp, "None") == 0 || strcmp(temp, "0") == 0)
				action = 0;
			else if (strcmp(temp, "Undo") == 0 || strcmp(temp, "1") == 0)
				action = 1;
			else if (strcmp(temp, "ExLastStr") == 0 || strcmp(temp, "2") == 0)
				action = 2;
			else if (strcmp(temp, "ChgMode") == 0 || strcmp(temp, "3") == 0)
				action = 3;
			else if (strcmp(temp, "ChgSelected") == 0 || strcmp(temp, "4") == 0)
				action = 4;
			else
			{
				log_message(WARNING, "Invalid action name '%s'", temp);
				free(temp);
				break;
			}

			params += i + 1;
			len -= i + 1;

			i = 0;
			while (i < len && params[i] != ' ' && params[i] != NULLSYM)
			{
				temp[i] = params[i];
				i++;
			}
			temp[i] = NULLSYM;

			int key;
			if (strcmp(temp, "None") == 0 || strcmp(temp, "0") == 0)
				key = 0;
			else if (strcmp(temp, "Break") == 0 || strcmp(temp, "Pause") == 0 || strcmp(temp, "1") == 0)
				key = 1;
			else if (strcmp(temp, "ScrollLock") == 0 || strcmp(temp, "2") == 0)
				key = 2;
			else if (strcmp(temp, "PrtSc") == 0 || strcmp(temp, "3") == 0)
				key = 3;
			else
			{
				log_message(WARNING, "Invalid key name '%s'", temp);
				free(temp);
				break;
			}
			
			params += i + 1;
			len -= i + 1;

			i = 0;
			while (i < len && params[i] != ' ' && params[i] != NULLSYM)
			{
				temp[i] = params[i];
				i++;
			}
			temp[i] = NULLSYM;

			int modifier;
			if (strcmp(temp, "None") == 0 || strcmp(temp, "0") == 0)
				modifier = 0;
			else if (strcmp(temp, "Shift") == 0 || strcmp(temp, "10") == 0)
				modifier = 10;
			else if (strcmp(temp, "Ctrl") == 0 || strcmp(temp, "11") == 0)
				modifier = 11;
			else
			{
				log_message(WARNING, "Invalid modifier name '%s'", temp);
				free(temp);
				break;
			}

			free(temp);

			ptr->btable[action].key = key;
			ptr->btable[action].key_modifier = modifier;

			break;
		}
		case 4: // Get Scaning Delay
		{
			int delay = atoi(params);
			if (delay >= 1000 && delay <= 30000)
				ptr->ScaningDelay = delay;
			else
				log_message(WARNING, "Bad value '%s' for ScaningDelay (values must be in 1000-30000), using default", params);
			break;
		}
		case 5: // Get Debug Mode
		{
			if (strcmp(params, "Error") == 0)
				ptr->LogLevel = ERROR;
			else if (strcmp(params, "Warning") == 0)
				ptr->LogLevel = WARNING;
			else if (strcmp(params, "Log") == 0)
				ptr->LogLevel = LOG;
			else if (strcmp(params, "Debug") == 0)
				ptr->LogLevel = DEBUG;
			else
				log_message(WARNING, "Illegal option for log level '%s', using default", params);
			break;
		}
		case 6:
		{
			ptr->xkbGroup[0] = atoi(params);
			break;
		}
		case 7:
		{
			ptr->xkbGroup[1] = atoi(params);
			break;
		}
		default:
		{
			log_message(WARNING, "Ignored option N %d", cp);
			break;
		}
	}
}

bool xconf_load_config_file(struct _xconf *p, const char *file_name)
{
	struct _list *list = load_file_to_list(file_name);
	if (list == NULL)
		return FALSE;

	int i;
	for (i = 0; i < list->data_count; i++)
		p->parse_line(p, list->data[i].string);

	list_uninit(list);
	return TRUE;
}

void xconf_parse_syllable_list(struct _xconf *p)
{
	struct _list *list = p->syllable;

	int i;
	for (i = 0; i < list->data_count; i++)
	{
		struct _list_data *data = &list->data[i];

		data->string[2]		= NULLSYM;
		data->sub_data		= atoi(data->string + 3);
		data->string_size	= 2;
	}
}

void xconf_uninit(struct _xconf *p)
{
	list_uninit(p->apps);

	list_uninit(p->dicts[0]);
	list_uninit(p->dicts[1]);

	list_uninit(p->protos[0]);
	list_uninit(p->protos[1]);

	list_uninit(p->syllable);

	free(p->btable);
	free(p);
}

struct _xconf* xconf_init()
{
	struct _xconf *ptr = (struct _xconf *) xnmalloc(sizeof(struct _xconf));
	bzero(ptr, sizeof(struct _xconf));

	ptr->btable = (struct _bind *) xnmalloc(DEFAULT_BIND_TABLE_SIZE * sizeof(struct _bind));
	memcpy(ptr->btable, DEFAULT_BT, DEFAULT_BIND_TABLE_SIZE * sizeof(struct _bind));

	ptr->DefaultMode		= AUTO_BY_KEYLOG;
	ptr->CurrentMode		= AUTO_BY_KEYLOG;
        ptr->ScaningDelay		= DEFAULT_SCANNING_DELAY;
	ptr->LogLevel			= LOG;
	ptr->TotalLanguages		= 2;	// Only Russian and English

	ptr->xkbGroup[ENGLISH]		= 0;	// Default english keyboard group
	ptr->xkbGroup[RUSSIAN]		= 1;	// Default russian keyboard group

	ptr->apps			= list_init();

	// Function mapping
	ptr->load_config_file		= xconf_load_config_file;
	ptr->parse_line			= xconf_parse_line;
	ptr->parse_syllable_list	= xconf_parse_syllable_list;
	ptr->uninit			= xconf_uninit;

	return ptr;
}
