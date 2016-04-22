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

#ifndef _XPROGRAM_H_
#define _XPROGRAM_H_

// BufferActions
#define KLB_NO_ACTION		0 // Modifier, function etc
#define KLB_ADD_SYM		1 // Alpha
#define KLB_DEL_SYM_RIGHT	2 // Del
#define KLB_DEL_SYM_LEFT	3 // Backspace
#define KLB_MOVE_RIGHT		4 // Cursor right
#define KLB_MOVE_LEFT		5 // Cursor left
#define KLB_SPACE		6 // Word end (space etc)
#define KLB_CLEAR		7 // Home, End etc
#define KLB_SWITCH_MODE		8 // Insert
#define KLB_SWITCH_LANG		9 // Switch

// Bind actions
#define BA_NONE			0
#define BA_CHANGE_WORD		1
#define BA_CHANGE_STRING	2
#define BA_CHANGE_MODE		3
#define BA_CHANGE_SELECTED	4

typedef struct _xprograminfo
{
	struct _xobj *xobject;
	struct _xswitchlang *xswitchlng;
	struct _xselection *selection;
	struct _xevent *xevnt;
	struct _xifocus *ifocus;
	struct _xstr *xstring;
	struct _xbindingtable *xbtable;

	bool (*update) (struct _xprograminfo *p);
	int  (*process_input) (struct _xprograminfo *p);
	bool (*perform_current_bind) (struct _xprograminfo *p);
	void (*perform_current_mode) (struct _xprograminfo *p);
	void (*change_word) (struct _xprograminfo *p, int lang);
	void (*change_string) (struct _xprograminfo *p, int lang);
	void (*set_convert_text) (struct _xprograminfo *p, const char *text);
	int  (*get_cur_lang) (struct _xprograminfo *p);
	const char* (*get_string) (struct _xprograminfo *p);
	void (*uninit) (struct _xprograminfo *p);
} xprograminfo;

struct _xprograminfo* xprogram_init(void);

#endif /* _XPROGRAM_H_ */
