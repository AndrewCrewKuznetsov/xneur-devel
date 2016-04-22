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

#ifndef _XCONFIG_H_
#define _XCONFIG_H_

#include "types.h"

typedef struct _bind
{
	int key;
	int key_modifier;
} bind;

typedef struct _xconf
{
	struct _list *apps;
	struct _list *dicts[2];		// 0 - English, 1 - Russian
	struct _list *protos[2];	// 0 - English, 1 - Russian
	struct _list *syllable;
	struct _bind *btable;

	int DefaultMode;
	int CurrentMode;		// AUTO_BY_KEYLOG, MANUAL_ON_SHORTCUT
	int ScaningDelay;		// Microsecs
	int LogLevel;			// ERROR - Error, WARNING - Warning, DEBUG - Debug, LOG - Log
	int TotalLanguages;		// Total languages to work with
	int xkbGroup[2];
	
	bool (*load_config_file) (struct _xconf *p, const char *file_name);
	void (*parse_line) (struct _xconf *p, char *line);
	void (*parse_syllable_list) (struct _xconf *p);
	void (*uninit) (struct _xconf *p);
} xconf;

struct _xconf* xconf_init();

#endif /* _XCONFIG_H_ */
