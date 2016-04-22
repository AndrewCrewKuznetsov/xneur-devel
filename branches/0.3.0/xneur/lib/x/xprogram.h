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
 *  (c) XNeur Team 2006
 *
 */

#ifndef _XPROGRAM_H_
#define _XPROGRAM_H_

void close_program(void);

typedef struct _xprogram
{
	struct _xswitchlang *switchlang;
	struct _xselection *selection;
	struct _xevent *event;
	struct _xfocus *focus;
	struct _xstring *string;

	int  last_action;
	int  changed_manual;

	void (*update) (struct _xprogram *p, int *do_update);
	void (*on_key_press) (struct _xprogram *p);
	void (*process_input) (struct _xprogram *p);
	int  (*perform_manual_action) (struct _xprogram *p, enum _hotkey_action action);
	void (*perform_auto_action) (struct _xprogram *p, int action);
	void (*check_last_word) (struct _xprogram *p);
	void (*change_word) (struct _xprogram *p, int lang);
	void (*convert_selection) (struct _xprogram *p);
	void (*send_string_silent) (struct _xprogram *p, int lang, int send_backspaces);
	void (*uninit) (struct _xprogram *p);
} xprogram;

struct _xprogram* xprogram_init(void);

#endif /* _XPROGRAM_H_ */
