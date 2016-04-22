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

#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xdefs.h>
#include <X11/Xmu/Atoms.h>

#include <stdlib.h>
#include <ctype.h>

#include "xconfig.h"
#include "xobject.h"
#include "xstring.h"
#include "xfocus.h"
#include "xswitchlang.h"
#include "xselection.h"
#include "xbindtable.h"
#include "xevent.h"
#include "xdefines.h"

#include "utils.h"
#include "conv.h"
#include "log.h"
#include "list.h"
#include "detection.h"

#include "xprogram.h"

extern struct _xconf *xconfig;

int handle_x_errors(Display *d, XErrorEvent *e)
{
	d = d;
	e = e;
	return FALSE;
}

int get_buffer_action(KeySym key, KeySym modifier)
{
	// Cursor keys
	if (IsCursorKey(key))
	{
		switch (key)
		{
			case XK_Right:
				return KLB_MOVE_RIGHT;
			case XK_Left:
				return KLB_MOVE_LEFT;
		}
		return KLB_CLEAR;
	}

	// KeyPad keys
	if (IsKeypadKey(key))
	{
		switch (key)
		{
			case XK_KP_Right:
				return KLB_MOVE_RIGHT;
			case XK_KP_Left:
				return KLB_MOVE_LEFT;
			case XK_KP_Space:
				return KLB_SPACE;
			case XK_KP_Delete:
				return KLB_DEL_SYM_RIGHT;
			case XK_KP_Insert:
				return KLB_SWITCH_MODE;
		}
		return KLB_CLEAR;
	}

	// Func, Mod, PF, PrivateKeypad keys
	if (IsFunctionKey(key) || IsModifierKey(key) || IsPFKey(key) || IsPrivateKeypadKey(key))
		return KLB_NO_ACTION;

	// MiscFunc keys
	if (IsMiscFunctionKey(key))
	{
		if (key == XK_Insert)
		{
			if (modifier == XK_Control_R || modifier == XK_Alt_R || modifier == XK_Shift_R)
				return KLB_CLEAR;
			return KLB_SWITCH_MODE;
		}
		return KLB_NO_ACTION;
	}

	// Del, bkspace, tab, return, alpha & num keys
	switch (key)
	{
		case XK_Delete:
			return KLB_DEL_SYM_RIGHT;
		case XK_BackSpace:
			return KLB_DEL_SYM_LEFT;
		case XK_Tab:
			return KLB_CLEAR;
		case XK_Return:
			return KLB_CLEAR;
		case XK_Pause:
		case XK_Escape:
		case XK_Sys_Req:
			return KLB_NO_ACTION;
		case XK_space:
			return KLB_SPACE;
		case XK_exclam:
		case XK_at:
		case XK_numbersign:
		case XK_ssharp:
		case XK_dollar:
		case XK_percent:
		case XK_asciicircum:
		case XK_ampersand:
		case XK_asterisk:
		case XK_parenleft:
		case XK_parenright:
		case XK_hyphen:
		case XK_underscore:
		case XK_equal:
		case XK_plus:
		case XK_minus:
		case XK_slash:
		case XK_bar:
		case XK_backslash:
		case XK_question:
			return KLB_SPACE;
	}

	if (modifier == XK_Control_R || modifier == XK_Alt_R)
		return KLB_NO_ACTION;
	return KLB_ADD_SYM;
}

// Get info about current input focus owner
bool xprogram_update(struct _xprograminfo *p)
{
	if (!p->ifocus->update(p->ifocus))		// Setting xEvent reciever params to updated focus window params
		return FALSE;

	if (p->ifocus->is_changed(p->ifocus))
		p->xevnt->set_communicator(p->xevnt, p->ifocus->owner);

	char *app_name = p->ifocus->owner->get_wmclass_name(p->ifocus->owner);
	if (app_name == NULL)
		return FALSE;

	bool is_registered = list_exist(xconfig->apps, app_name, strlen(app_name));
	free(app_name);

	if (!is_registered)
		return FALSE;

	if (!p->ifocus->listen_client(p->ifocus))			// Begin listening application
		return FALSE;

	return TRUE;
}

const char* xprogram_get_string(struct _xprograminfo *p)
{
	struct _xstr *xstring = p->xstring;
	struct _xevent *evnt = p->xevnt;

	if (p->ifocus->is_changed(p->ifocus))
		xstring->clear(xstring);

	bool changed = FALSE;
	while (evnt->check_key_press(evnt))
	{
		if (!evnt->is_native(evnt))
		{              
			changed = FALSE;
			continue;
		}

		KeySym key = evnt->get_cur_keysym(evnt);
		KeySym modifier = evnt->get_cur_modifier(evnt);

		int action = p->xbtable->get_bind_action(p->xbtable, key, modifier);
		p->xbtable->set_action(p->xbtable, action);

		if (action > 0)
			return xstring->content;

		action = get_buffer_action(key, modifier);
		switch (action)
		{
			case KLB_CLEAR:
			case KLB_MOVE_LEFT:
			case KLB_MOVE_RIGHT:
			case KLB_DEL_SYM_RIGHT:
			{
				xstring->clear(xstring);
				break;
			}
			case KLB_DEL_SYM_LEFT:
			{
				xstring->del_symbol(xstring);
				break;
			}
			case KLB_ADD_SYM:
			case KLB_SPACE:
			{
				char sym = evnt->get_cur_char(evnt);
				xstring->add_symbol(xstring, sym);

				p->xswitchlng->cur_lang = p->xswitchlng->get_cur_lang(p->xswitchlng);
				changed = TRUE;
				break;
			}
		}
	}

	if (!changed)
		return NULL;
	return xstring->content;
}

int xprogram_process_input(struct _xprograminfo *p)
{
	// Return early if bind detected
	const char *res_str = p->get_string(p);

	if (!res_str)
		return 0;
	return strlen(res_str);
}

bool xprogram_perform_current_bind(struct _xprograminfo *p)
{
	int action = p->xbtable->get_action(p->xbtable);
	if (action == BA_NONE)
		return FALSE;

	p->xbtable->set_action(p->xbtable, BA_NONE);

	switch (action)
	{
		case BA_CHANGE_WORD: // User needs to cancel last change
		{
			int cur_lang = p->get_cur_lang(p);
			int next_lang = get_next_lang(cur_lang);

			log_message(DEBUG, "Change current word language from %d to %d", cur_lang, next_lang);

			p->change_word(p, next_lang);
			return TRUE;
		}
		case BA_CHANGE_MODE:
		{
			if (xconfig->CurrentMode == AUTO_BY_KEYLOG)
			{
				xconfig->CurrentMode = MANUAL_ON_SHORTCUT;
				log_message(DEBUG, "Changed current mode to manual");
			}
			else
			{
				xconfig->CurrentMode = AUTO_BY_KEYLOG;
				log_message(DEBUG, "Changed current mode to auto");
			}

			return TRUE;
		}
		case BA_CHANGE_STRING: // User needs to change current phrase
		{
			int cur_lang = p->get_cur_lang(p);
			int next_lang = get_next_lang(cur_lang);

			log_message(DEBUG, "Change current phrase language from %d to %d", cur_lang, next_lang);

			p->change_string(p, next_lang);
			return TRUE;
		}
		case BA_CHANGE_SELECTED:
		{
			log_message(DEBUG, "Change selected text");

			struct _xselection *selection = p->selection;

			char *seltext = selection->get_selected_text(selection);
			if (!seltext)
				return TRUE;

			p->set_convert_text(p, seltext);
			XSetSelectionOwner(selection->owner->xDisplay, selection->selection, None, CurrentTime);
			XFree(seltext);

			return TRUE;
		}
	}
	return FALSE;
}

// Perform mode actions
void xprogram_perform_current_mode(struct _xprograminfo *p)
{
	if (xconfig->CurrentMode == MANUAL_ON_SHORTCUT)
		return;

	const char *word = get_last_word(p->xstring->content);
	if (!word)
	{
		log_message(DEBUG, "Nothing to change");
		return;
	}

	int cur_lang = p->get_cur_lang(p);

	log_message(DEBUG, "Processing word '%s' (language %d)", word, cur_lang);

	int new_lang = get_word_lang(word, cur_lang);
	if (new_lang == NO_LANGUAGE)
	{
		log_message(DEBUG, "Nothing to change");
		return;
	}

	if (new_lang == cur_lang)
		return;

	log_message(DEBUG, "Changing lang from %d to %d", cur_lang, new_lang);

	p->change_word(p, new_lang);
}

void xprogram_change_word(struct _xprograminfo *p, int lang)
{
	struct _xstr *xstring = p->xstring;

	int offset = get_last_word_offset(xstring->content);
	if (offset == -1)
	{
		log_message(DEBUG, "Nothing to change");
		return;
	}

	// Shift fields to point to begin of word
	xstring->content		+= offset;
	xstring->keycode		+= offset;
	xstring->keycode_modifiers	+= offset;
	xstring->cur_pos		-= offset;

	log_message(DEBUG, "Processing word '%s'", xstring->content);

	p->xevnt->del_nchrbck(p->xevnt, xstring->cur_pos);
	p->xevnt->send_convert_string(p->xevnt, xstring, p->xswitchlng, lang);

	// Revert fields back
	xstring->content		-= offset;
	xstring->keycode		-= offset;
	xstring->keycode_modifiers	-= offset;
	xstring->cur_pos		+= offset;
}

void xprogram_change_string(struct _xprograminfo *p, int lang)
{
	struct _xstr *xstring = p->xstring;

	int str_len = xstring->cur_pos;
	if (str_len == 0)
	{
		log_message(DEBUG, "Nothing to change");
		return;
	}

	log_message(DEBUG, "Processing string '%s'", xstring->content);
  
	p->xevnt->del_nchrbck(p->xevnt, str_len);
	p->xevnt->send_convert_string(p->xevnt, xstring, p->xswitchlng, lang);
}

int xprogram_get_cur_lang(struct _xprograminfo *p)
{
	return p->xswitchlng->cur_lang;
}

void xprogram_set_convert_text(struct _xprograminfo *p, const char *text)
{
	int str_len = strlen(text);
	if (str_len == 0)
		return;

	char *string		= (char *) xnmalloc((str_len + 1) * sizeof(char));
	KeyCode *keycode	= (KeyCode *) xnmalloc((str_len + 1) * sizeof(KeyCode));
	int *keycode_modifiers	= (int *) xnmalloc((str_len + 1) * sizeof(int));

	int keycode_mod_eng	= get_keycode_mod(xconfig->xkbGroup[ENGLISH]);
	int keycode_mod_rus	= get_keycode_mod(xconfig->xkbGroup[RUSSIAN]);

	int i, j;
	for(i = 0, j = 0; i < str_len; i++, j++)
	{
		int keycode_mod;
		if (isascii(text[i]))
		{
			string[j] = text[i];
			keycode_mod = keycode_mod_rus;
		}	
		else
		{
			string[j] = get_ascii(&text[i]);
			keycode_mod = keycode_mod_eng;
			i++;

			if (string[j] == NULLSYM)
			{
				string[j] = '#';
				i++;
			}
		}

		char sym = string[j];

		int shift_mod = None;
		if (is_upper_non_alpha_cyr(sym) || isupper(sym))
			shift_mod = ShiftMask;
		
		keycode_modifiers[j] = (keycode_mod | shift_mod);
		KeySym ks = char_to_keysym(sym);
		
		keycode[j] = XKeysymToKeycode(p->xobject->xDisplay, ks);

		// Xlib (error '<') patch >>
		if (ks == XK_less && keycode[j] == 94)
			keycode[j] = 59;
	}

	string[j] = NULLSYM;
  
	struct _xstr *xstring = p->xstring;

	xstring->set_content(xstring, string);

	for (i = 0; i < j; i++)
	{
		xstring->keycode[i] = keycode[i];
		xstring->keycode_modifiers[i] = keycode_modifiers[i];
	}

	p->xevnt->send_string(p->xevnt, xstring);
}

void xprogram_uninit(struct _xprograminfo *p)
{
	XDestroyWindow(p->xobject->xDisplay,  p->xobject->xWindow);
	XCloseDisplay(p->xobject->xDisplay);

	p->xobject->uninit(p->xobject);
	p->selection->uninit(p->selection);
	p->xevnt->uninit(p->xevnt);
	p->ifocus->uninit(p->ifocus);
	p->xswitchlng->uninit(p->xswitchlng);
	p->xstring->uninit(p->xstring);
	p->xbtable->uninit(p->xbtable);

	free(p);
}

struct _xprograminfo* xprogram_init(void)
{	
	// Allocate & init huge xProgramInfo struct
	struct _xprograminfo *xinfo = (struct _xprograminfo*) xnmalloc(sizeof(struct _xprograminfo));
	bzero(xinfo, sizeof(struct _xprograminfo));
  
	xinfo->xobject = xobj_init();  // Program window
  
	XSetErrorHandler(handle_x_errors);

	// Connect to X Server
	if (xinfo->xobject->connect(xinfo->xobject) == ERROR)
	{
		log_message(ERROR, "Can't connect to XServer");
		return NULL;
	}
 
	// Create program window
	if (xinfo->xobject->create_win(xinfo->xobject) == ERROR)
	{
		log_message(ERROR, "Can't create program window");
		return NULL;
	} 

	Display *display	= xinfo->xobject->xDisplay;

	xinfo->selection	= xselection_init(display, xinfo->xobject->xWindow);	// X selection processor
	xinfo->xswitchlng	= xswitchlang_init(display);				// KBD-based layout switch
	xinfo->xevnt		= xevent_init();					// X event processor
	xinfo->ifocus		= xifocus_init();					// X Input Focus and Pointer processor
	xinfo->xstring		= xstr_init();						// Input word buffer
	xinfo->xbtable		= xbindingtable_init(DEFAULT_BIND_TABLE_SIZE);		// Key Bind Table
  
	// Selecting no core event mask. Remember: selection notify event have no mask.
	xinfo->xobject->set_event_mask(xinfo->xobject, None);

	xinfo->xstring->home->xDisplay		= display;
	xinfo->ifocus->owner->xDisplay		= display;
	xinfo->xswitchlng->cur_lang		= xinfo->xswitchlng->get_cur_lang(xinfo->xswitchlng);

	xinfo->xevnt->set_communicator(xinfo->xevnt, xinfo->xobject);
	xinfo->xswitchlng->print_keyboard_list(xinfo->xswitchlng);
 
	// xbinding
	int i;
	for(i = 1; i < DEFAULT_BIND_TABLE_SIZE; i++)
		xinfo->xbtable->bind(xinfo->xbtable, xconfig->btable[i].key, xconfig->btable[i].key_modifier, i);
  
	XSynchronize(xinfo->xobject->xDisplay, TRUE);
	XFlush(xinfo->xobject->xDisplay);
  
	// Function mapping
	xinfo->uninit			= xprogram_uninit; 
	xinfo->update			= xprogram_update;
	xinfo->process_input		= xprogram_process_input;
	xinfo->perform_current_bind	= xprogram_perform_current_bind;
	xinfo->perform_current_mode	= xprogram_perform_current_mode;
	xinfo->change_word		= xprogram_change_word;
	xinfo->change_string		= xprogram_change_string;
	xinfo->set_convert_text		= xprogram_set_convert_text;
	xinfo->get_cur_lang		= xprogram_get_cur_lang;
	xinfo->get_string		= xprogram_get_string;

	return xinfo;
}
