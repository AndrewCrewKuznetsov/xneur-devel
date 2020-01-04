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

#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include "detection.h"

struct _program
{
	struct _event *event;
	struct _focus *focus;
	struct _buffer *buffer;
	struct _plugin *plugin;

	int  last_action;
	int  changed_manual;
	int  app_forced_mode;
	int  app_excluded;
	int  app_autocompletion_mode;

	int  action_mode;

	int  last_layout;

	int user_action;
	enum _hotkey_action action;

	enum _correction_action correction_action;
	struct _buffer *correction_buffer;

	int last_pattern_id;

	int has_x_input_extension;
	int xi_opcode;

	/// Set with active layouts for each window
	/// Set of pairs (Window name, active layout), data in pair delimited by space
	struct _list_char *window_layouts;

	void (*process_input) (struct _program *p);
	void (*uninit) (struct _program *p);
};

struct _program* program_init(void);

#endif /* _PROGRAM_H_ */
