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
 *  Copyright (C) 2006-2013 XNeur Team
 *
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#define XK_PUBLISHING
#include <X11/XKBlib.h>

#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h> 


#ifdef WITH_ASPELL
#  include <aspell.h>
#endif

#include "xnconfig.h"
#include "xnconfig_files.h"

#include "bind_table.h"
#include "event.h"
#include "focus.h"
#include "selection.h"
#include "switchlang.h"
#include "defines.h"
#include "buffer.h"
#include "window.h"
#include "keymap.h"
#include "utils.h"
#include "plugin.h"

#include "types.h"
#include "list_char.h"
#include "log.h"
#include "text.h"
#include "detection.h"
#include "conversion.h"

#include "notify.h"

#include "program.h"

#define KLB_NO_ACTION           0	// Modifier, function etc
#define KLB_ADD_SYM             1	// Alpha
#define KLB_DEL_SYM             2	// Backspace
#define KLB_SPACE               3	// Word end (space etc)
#define KLB_ENTER               4	// Enter
#define KLB_TAB 		        5	// Tab
#define KLB_CLEAR               6	// Home, End etc

#define MANUAL_FLAG_UNSET	0
#define MANUAL_FLAG_SET		1
#define MANUAL_FLAG_NEED_FLUSH	2

#define NO_MODIFIER_MASK	0

#define MIN_PATTERN_LEN		4

static const char *normal_action_names[] =	{
										"Correct/Undo correction", "Transliterate", "Change case", "Preview correction", 
										"Correct last line", 
										"Correct selected text", "Transliterate selected text", "Change case of selected text", "Preview correction of selected text",
	                                    "Correct clipboard text", "Transliterate clipboard text", "Change case of clipboard text", "Preview correction of clipboard text",
										"Switch to layout 1", "Switch to layout 2", "Switch to layout 3", "Switch to layout 4",
		                                "Rotate layouts", "Rotate layouts back", "Expand abbreviations", "Autocompletion confirmation", 
										"Rotate autocompletion", "Block/Unblock keyboard and mouse events", "Insert date"
						};

extern struct _xneur_config *xconfig;

struct _window *main_window;

int last_event_type = 0;

int lock_mask = 0;
int is_lock = 0;

// Private
static void toggle_lock(int mask, int state)
{
	int xkb_opcode, xkb_event, xkb_error;
	int xkb_lmaj = XkbMajorVersion;
	int xkb_lmin = XkbMinorVersion;
	if (XkbLibraryVersion(&xkb_lmaj, &xkb_lmin) && XkbQueryExtension(main_window->display, &xkb_opcode, &xkb_event, &xkb_error, &xkb_lmaj, &xkb_lmin))
	{
		/*int status = */XkbLockModifiers (main_window->display, XkbUseCoreKbd, mask, state);
		//log_message(TRACE, _("Set lock state: %d %d, status: %d"), mask, state, status);
	}
}

static int get_auto_action(struct _program *p, KeySym key, int modifier_mask)
{
	if 	(((key == XK_BackSpace) && (xconfig->troubleshoot_backspace)) || 
		((key == XK_Left) && (xconfig->troubleshoot_left_arrow)) ||
		((key == XK_Right) && (xconfig->troubleshoot_right_arrow)) ||
		((key == XK_Up) && (xconfig->troubleshoot_up_arrow)) ||
		((key == XK_Down) && (xconfig->troubleshoot_down_arrow)) ||
		((key == XK_Delete) && (xconfig->troubleshoot_delete))) 
		p->changed_manual = MANUAL_FLAG_SET;
	
	// Null symbol
	if (key == 0)
		return KLB_NO_ACTION;

	// Cursor keys
	if (IsCursorKey(key))
		return KLB_CLEAR;

	// KeyPad keys
	if (IsKeypadKey(key))
	{
		if (get_key_state(XK_Num_Lock) != 0)
		{
			if (modifier_mask & ControlMask || modifier_mask & Mod1Mask || modifier_mask & ShiftMask || modifier_mask & Mod4Mask)
				return KLB_CLEAR;
			return KLB_ADD_SYM;
		}

		switch (key)
		{
			case XK_KP_Divide:
			case XK_KP_Multiply:
			case XK_KP_Add:
			case XK_KP_Subtract:
				return KLB_ADD_SYM;
		}

		return KLB_CLEAR;
	}

	// Func, Mod, PF, PrivateKeypad keys
	if (IsFunctionKey(key) || IsModifierKey(key) || IsPFKey(key) || IsPrivateKeypadKey(key))
		return KLB_NO_ACTION;

	// MiscFunc keys
	if (IsMiscFunctionKey(key))
	{
		if (key != XK_Insert)
			return KLB_NO_ACTION;

		if (modifier_mask & ControlMask || modifier_mask & Mod1Mask || modifier_mask & ShiftMask || modifier_mask & Mod4Mask)
			return KLB_CLEAR;

		return KLB_NO_ACTION;
	}

	// Del, bkspace, tab, return, alpha & num keys
	switch (key)
	{
		case XK_BackSpace:
			return KLB_DEL_SYM;
		case XK_Pause:
		case XK_Escape:
		case XK_Sys_Req:
		case XK_Delete:
			return KLB_NO_ACTION;
		case XK_Return:
			return KLB_ENTER;
		case XK_Tab:
			return KLB_TAB;
		/*case XK_space:
		case XK_minus:
		case XK_equal:
		case XK_plus:
		case XK_slash:
		case XK_bar:
		case XK_backslash:
		case XK_question:
		case XK_semicolon:
		case XK_comma:
		case XK_period:
		case XK_1:
		case XK_2:
		case XK_3:
		case XK_4:
		case XK_5:
		case XK_6:
		case XK_7:
		case XK_8:
		case XK_9:
		case XK_0:*/
		default:
		{
			for (int i=0; i < xconfig->delimeters_count; i++)
			{
				if (xconfig->delimeters[i] == key)
					return KLB_SPACE;
			}
		}
	}

	/*if (modifier_mask & Mod1Mask || modifier_mask & Mod4Mask)
		return KLB_NO_ACTION;*/

	if (modifier_mask & ControlMask)
		return KLB_CLEAR;

	return KLB_ADD_SYM;
}

static void program_layout_update(struct _program *p)
{
	if (!xconfig->remember_layout)
		return;

	if ((Window) p->last_window == p->focus->owner_window)
		return;

	char *text_to_find	= (char *) malloc(1024 * sizeof(char));
	char *window_layouts	= (char *) malloc(1024 * sizeof(char));

	struct _list_char_data* last_app = NULL;

	char *last_app_name = get_wm_class_name(p->last_window);
	if (last_app_name != NULL)
		last_app = xconfig->layout_remember_apps->find(xconfig->layout_remember_apps, last_app_name, BY_PLAIN);

	if (last_app != NULL)
		sprintf(text_to_find, "%s", last_app_name);
	else
		sprintf(text_to_find, "%d", (int) p->last_window);

	if (last_app_name != NULL)
		free(last_app_name);

	// Remove layout for old window
	for (int lang = 0; lang < xconfig->handle->total_languages; lang++)
	{
		sprintf(window_layouts, "%s %d", text_to_find, lang);

		if (!xconfig->window_layouts->exist(xconfig->window_layouts, window_layouts, BY_PLAIN))
			continue;

		xconfig->window_layouts->rem(xconfig->window_layouts, window_layouts);
	}

	// Save layout for old window
	sprintf(window_layouts, "%s %d", text_to_find, p->last_layout);
	xconfig->window_layouts->add(xconfig->window_layouts, window_layouts);	
	
	struct _list_char_data* curr_app = NULL;

	char *curr_app_name = get_wm_class_name(p->focus->owner_window);
	if (curr_app_name != NULL)
		curr_app = xconfig->layout_remember_apps->find(xconfig->layout_remember_apps, curr_app_name, BY_PLAIN);

	if (curr_app != NULL)
		sprintf(text_to_find, "%s", curr_app_name);
	else
		sprintf(text_to_find, "%d", (int) p->focus->owner_window);

	if (curr_app_name != NULL)
		free(curr_app_name);

	// Restore layout for new window
	for (int lang = 0; lang < xconfig->handle->total_languages; lang++)
	{
		sprintf(window_layouts, "%s %d", text_to_find, lang);
		if (!xconfig->window_layouts->exist(xconfig->window_layouts, window_layouts, BY_PLAIN))
			continue;

		if (text_to_find != NULL)
			free(text_to_find);
		if (window_layouts != NULL)
			free(window_layouts);

		//XkbLockGroup(main_window->display, XkbUseCoreKbd, lang);
		set_keyboard_group(lang);
		log_message(DEBUG, _("Restore layout group to %d"), lang);
		return;
	}

	if (text_to_find != NULL)
		free(text_to_find);
	if (window_layouts != NULL)
		free(window_layouts);

	log_message(DEBUG, _("Store default layout group to %d"), xconfig->default_group);
}

static void program_update(struct _program *p)
{
	p->last_window = p->focus->owner_window;

	int status = p->focus->get_focus_status(p->focus, &p->app_forced_mode, &p->app_focus_mode, &p->app_autocompletion_mode);
	p->event->set_owner_window(p->event, p->focus->owner_window);

	int listen_mode = LISTEN_GRAB_INPUT;
	if (p->app_focus_mode == FOCUS_EXCLUDED)
		listen_mode = LISTEN_DONTGRAB_INPUT;

	p->focus->update_events(p->focus, listen_mode);
	p->focus->update_grab_events(p->focus, LISTEN_GRAB_INPUT);
	
	if (status == FOCUS_UNCHANGED)
		return;

	p->layout_update(p);

	p->buffer->save_and_clear(p->buffer, p->last_window);
	p->correction_buffer->clear(p->correction_buffer);
	p->correction_action = ACTION_NONE;
	
	if (status == FOCUS_NONE)
		return;
	
	// Сброс признака "ручное переключение" после смены фокуса.
	p->changed_manual = MANUAL_FLAG_UNSET;
}

static void program_process_input(struct _program *p)
{
	p->update(p);

	while (1)
	{
		int type = p->event->get_next_event(p->event);

		int curr_layout = get_curr_keyboard_group();
		if ((p->last_layout != curr_layout) && (p->last_window == p->focus->owner_window))
		{
			if (xconfig->troubleshoot_switch)
			{
				//log_message (ERROR, "KBD_SWITCH") ;
				p->changed_manual = MANUAL_FLAG_SET;
			}		
		}
		
		p->last_layout = curr_layout;
		
		switch (type)
		{
			case ClientMessage:
			{
				// Exit from main cycle by message to main window
				//XClientMessageEvent *cme = (XClientMessageEvent *) &(p->event->event);
				//log_message(LOG, _("Exitting from main cycle"));
				//return;
				break;
			}
			case KeyPress:
			{
				if (xconfig->block_events)
				{
					XAllowEvents(main_window->display, AsyncKeyboard, CurrentTime);
				}

				log_message(TRACE, _("Received KeyPress '%s' (event type %d)"), XKeysymToString(p->event->get_cur_keysym(p->event)), type);

				// Save received event
				p->event->default_event = p->event->event;

				// Processing received event
				p->on_key_action(p, type);

				// Resend special key back to window
				if (p->event->default_event.xkey.keycode != 0)
				{
					p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);
					p->event->event = p->event->default_event;
					p->event->send_next_event(p->event);
					p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);
				}
					
				break;
			}
			case KeyRelease:
			{
				if (xconfig->block_events)
				{
					XAllowEvents(main_window->display, AsyncKeyboard, CurrentTime);
				}
				
				log_message(TRACE, _("Received KeyRelease '%s' (event type %d)"), XKeysymToString(p->event->get_cur_keysym(p->event)), type);

				// Save received event
				p->event->default_event = p->event->event;

				// Processing received event
				p->on_key_action(p, type);

				// Resend special key back to window
				if (p->event->default_event.xkey.keycode != 0)
				{		
					p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);
					p->event->event = p->event->default_event;
					p->event->send_next_event(p->event);
					p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);
				}
				break;
			}
			case FocusIn:
			case LeaveNotify:
			case EnterNotify:
			{
				if (type == FocusIn)
				{
					if (p->focus->owner_window != p->event->event.xfocus.window)
						log_message(TRACE, _("Received FocusIn on window %d (event type %d)"), p->event->event.xfocus.window, type);
				}
				//else if (type == LeaveNotify)
				//	log_message(TRACE, _("Received LeaveNotify (event type %d)"), type);
				//else if (type == EnterNotify)
				//	log_message(TRACE, _("Received EnterNotify (event type %d)"), type);
				
				break;
			}
			case FocusOut:
			{
				if (p->focus->owner_window != p->event->event.xfocus.window)
					log_message(TRACE, _("Received FocusOut on window %d (event type %d)"), p->event->event.xfocus.window, type);

				p->update(p);
				
				break;
			}
			case SelectionNotify:
			{
				log_message(TRACE, _("Received SelectionNotify (event type %d)"), type);
				break;
			}
			case SelectionRequest:
			{
				log_message(TRACE, _("Received SelectionRequest (event type %d)"), p->event->event.xselectionrequest.requestor, type);
				break;
			}
			case ButtonPress:
			{
				// Clear buffer only when clicked left button
				if (p->event->event.xbutton.button == Button1)
				{
					p->buffer->save_and_clear(p->buffer, p->focus->owner_window);
					p->correction_buffer->clear(p->correction_buffer);
					p->correction_action = ACTION_NONE;
					log_message(TRACE, _("Received Button%dPress on window %d with subwindow %d (event type %d)"), p->event->event.xbutton.button, p->event->event.xbutton.window, p->event->event.xbutton.subwindow, type);
				}
				
				if (xconfig->block_events)
				{
					XAllowEvents(main_window->display, AsyncPointer, CurrentTime);
					break;
				}
				
				XAllowEvents(main_window->display, ReplayPointer, CurrentTime);

				if ((Window)p->focus->get_focused_window(p->focus) != (Window)p->focus->owner_window)
				{
					p->update(p);
				}
				
				break;
			}
			case ButtonRelease:
			{	
				// Clear buffer only when clicked left button
				if (p->event->event.xbutton.button == Button1)
				{
					p->buffer->save_and_clear(p->buffer, p->focus->owner_window);
					p->correction_buffer->clear(p->correction_buffer);
					p->correction_action = ACTION_NONE;
				}

				log_message(TRACE, _("Received Button%dRelease on window %d with subwindow %d (event type %d)"), p->event->event.xbutton.button, p->event->event.xbutton.window, type);
				
				if (xconfig->block_events)
				{
					XAllowEvents(main_window->display, AsyncPointer, CurrentTime);
					break;
				}

				//p->update(p);
				
				XAllowEvents(main_window->display, ReplayPointer, CurrentTime);
			}
			case PropertyNotify:
			{
				if (XInternAtom(main_window->display, "XKLAVIER_STATE", FALSE) == p->event->event.xproperty.atom)
				{
					log_message(TRACE, _("Received Property Notify (layout switch event) (event type %d)"), type);

					// Flush string
					//p->buffer->clear(p->buffer);
				}
				break;
			}
			case MappingNotify:
			{
				//* В xneur стало валиться слишком много этих сообщений в 13.10, отключено до лучших времен.
				if (last_event_type == MappingNotify)
				{
					break;
				}
				log_message(TRACE, _("Received MappingNotify (event type %d)"), type);

				p->buffer->uninit(p->buffer);
				p->correction_buffer->uninit(p->correction_buffer);
				
				main_window->uninit_keymap(main_window);
				
				xneur_handle_destroy(xconfig->handle);
				xconfig->handle = xneur_handle_create();
				
				main_window->init_keymap(main_window);
				p->buffer = buffer_init(xconfig->handle, main_window->keymap);
				p->buffer->handle = xconfig->handle;
				p->buffer->keymap = main_window->keymap;
				
				p->correction_buffer = buffer_init(xconfig->handle, main_window->keymap);
				p->correction_buffer->handle = xconfig->handle;
				p->correction_buffer->keymap = main_window->keymap;
				p->correction_action = ACTION_NONE;
				
				//log_message (DEBUG, _("Now layouts count %d"), xconfig->handle->total_languages);
				log_message(LOG, _("Keyboard layouts present in system:"));
				for (int lang = 0; lang < xconfig->handle->total_languages; lang++)
				{
					if (xconfig->handle->languages[lang].excluded)
						log_message(LOG, _("   Excluded XKB Group '%s', layout '%s', group '%d'"), xconfig->handle->languages[lang].name, xconfig->handle->languages[lang].dir, lang);
					else
						log_message(LOG, _("   Included XKB Group '%s', layout '%s', group '%d'"), xconfig->handle->languages[lang].name, xconfig->handle->languages[lang].dir, lang);

					char *lang_name = xconfig->handle->languages[lang].name;

					log_message(DEBUG, _("      %s dictionary has %d records"), lang_name, xconfig->handle->languages[lang].dictionary->data_count);
					log_message(DEBUG, _("      %s proto has %d records"), lang_name, xconfig->handle->languages[lang].proto->data_count);
					log_message(DEBUG, _("      %s big proto has %d records"), lang_name, xconfig->handle->languages[lang].big_proto->data_count);
			#ifdef WITH_ASPELL
					if (xconfig->handle->has_spell_checker[lang])
					{
						loKg_message(DEBUG, _("      %s aspell dictionary loaded"), lang_name);
					}
					else
					{
						log_message(DEBUG, _("      %s aspell dictionary not found"), lang_name);
					}
			#endif	
			#ifdef WITH_ENCHANT
					if (xconfig->handle->enchant_dicts[lang])
					{
						log_message(DEBUG, _("      %s enchant wrapper dictionary loaded"), lang_name);
					}
					else
					{
						log_message(DEBUG, _("      %s enchant wrapper dictionary not found"), lang_name);
					}
			#endif	
				}
				log_message(LOG, _("Total %d keyboard layouts detected"), xconfig->handle->total_languages);

				break;
			}
			case MotionNotify:
			{
				log_message(TRACE, _("Received MotionNotify (event type %d)"), type);
				break;
			}
			default:
			{
				int xi_opcode, event, error;
				if (!XQueryExtension(main_window->display, "XInputExtension", &xi_opcode, &event, &error)) 
				{
					log_message(WARNING, _("X Input extension not available."));
					log_message(DEBUG, _("Uncatched event with type %d (see X11/X.h for details)"), type);
					break;
				}
				
				XGenericEventCookie *cookie = &p->event->event.xcookie;

				if (cookie->type != GenericEvent ||
					cookie->extension != xi_opcode ||
					!XGetEventData(main_window->display, cookie))
					break;

				//printf("EVENT TYPE %d\n", cookie->evtype);
				XIDeviceEvent* xi_event = cookie->data;
				switch (xi_event->evtype) {
				case XI_ButtonPress:
				{
					// Clear buffer only when clicked left button
					if (xi_event->detail == 1)
					{
						p->buffer->save_and_clear(p->buffer, p->focus->owner_window);
						p->correction_buffer->clear(p->correction_buffer);
						p->correction_action = ACTION_NONE;
						if ((Window)p->focus->get_focused_window(p->focus) != (Window)p->focus->owner_window)
						{
							p->update(p);
						}
						log_message(TRACE, _("Received XI_ButtonPress (event type %d, subtype %d)"), type, xi_event->evtype);
					}
					break;
				}
				}
				XFreeEventData(main_window->display, cookie);
				break;
			}
		}
		last_event_type = type;
	}
}

static void program_change_lang(struct _program *p, int new_lang)
{
	log_message(DEBUG, _("Changing language from %s to %s"), xconfig->handle->languages[get_curr_keyboard_group()].name, xconfig->handle->languages[new_lang].name);
	p->buffer->set_lang_mask(p->buffer, new_lang);
	//XkbLockGroup(main_window->display, XkbUseCoreKbd, new_lang);
	set_keyboard_group(new_lang);
	p->last_layout = new_lang;
}

static void program_change_incidental_caps(struct _program *p)
{
	log_message(DEBUG, _("Correcting iNCIDENTAL CapsLock"));

	// Change modifier mask
	p->buffer->set_uncaps_mask(p->buffer);

	// Change CAPS if need
	if (!get_key_state(XK_Caps_Lock))
		return;

	toggle_lock(LockMask, 0);
}

static void program_unchange_incidental_caps(struct _program *p)
{
	// Change modifier mask
	p->buffer->set_caps_mask(p->buffer);

	// Change CAPS if need
	if (get_key_state(XK_Caps_Lock))
		return;

	toggle_lock (LockMask, 1);
}

static void program_change_two_capital_letter(struct _program *p)
{
	log_message(DEBUG, _("Correcting two CApital letter"));

	// Change modifier mask
	p->buffer->keycode_modifiers[1] = p->buffer->keycode_modifiers[1] & (~ShiftMask);
}

static void program_unchange_two_capital_letter(struct _program *p)
{
	log_message(DEBUG, _("Correcting two CApital letter"));

	// Change modifier mask
	p->buffer->keycode_modifiers[1] = p->buffer->keycode_modifiers[1] | (ShiftMask);
}

static void program_process_selection_notify(struct _program *p)
{
	char *event_text = NULL;
	if (p->action_mode == ACTION_CHANGE_SELECTED || p->action_mode == ACTION_CHANGECASE_SELECTED || p->action_mode == ACTION_TRANSLIT_SELECTED || p->action_mode == ACTION_PREVIEW_CHANGE_SELECTED)
		event_text = (char *)get_selection_text(SELECTION_PRIMARY);
	else if (p->action_mode == ACTION_CHANGE_CLIPBOARD || p->action_mode == ACTION_CHANGECASE_CLIPBOARD || p->action_mode == ACTION_TRANSLIT_CLIPBOARD || p->action_mode == ACTION_PREVIEW_CHANGE_CLIPBOARD)
		event_text = (char *)get_selection_text(SELECTION_CLIPBOARD);
		
	if (event_text == NULL)
	{
		p->action_mode = ACTION_NONE;
		log_message (DEBUG, _("Received selected text is '%s'"), "NULL");
		return;
	}

	log_message (DEBUG, _("Received selected text '%s'"), event_text);
	
	if (p->action_mode == ACTION_TRANSLIT_SELECTED)
		convert_text_to_translit(&event_text);

	p->buffer->set_content(p->buffer, event_text);
	if (event_text != NULL)
		free(event_text);

	switch (p->action_mode)
	{
		case ACTION_CHANGE_SELECTED:
		{
			p->buffer->rotate_layout(p->buffer);
			
			if (xconfig->rotate_layout_after_convert)
				set_next_keyboard_group(xconfig->handle);
			
			show_notify(NOTIFY_CHANGE_SELECTED, NULL);
			break;
		}
		case ACTION_CHANGE_CLIPBOARD:
		{
			p->buffer->rotate_layout(p->buffer);

			show_notify(NOTIFY_CHANGE_CLIPBOARD, NULL);
			break;
		}
		case ACTION_CHANGECASE_SELECTED:
		{
			p->buffer->change_case(p->buffer);

			show_notify(NOTIFY_CHANGECASE_SELECTED, NULL);
			break;
		}
		case ACTION_CHANGECASE_CLIPBOARD:
		{
			p->buffer->change_case(p->buffer);

			show_notify(NOTIFY_CHANGECASE_CLIPBOARD, NULL);
			break;
		}
		case ACTION_TRANSLIT_SELECTED:
		{
			int lang = main_window->keymap->latin_group;
			p->change_lang(p, lang);

			show_notify(NOTIFY_TRANSLIT_SELECTED, NULL);
			break;
		}
		case ACTION_TRANSLIT_CLIPBOARD:
		{
			int lang = main_window->keymap->latin_group;
			p->change_lang(p, lang);

			show_notify(NOTIFY_TRANSLIT_CLIPBOARD, NULL);
			break;
		}
		case ACTION_PREVIEW_CHANGE_SELECTED:
		{
			p->buffer->rotate_layout(p->buffer);

			char *string = p->buffer->get_utf_string(p->buffer);
			show_notify(NOTIFY_PREVIEW_CHANGE_SELECTED, string);
			if (string)
				free (string);
			break;
		}
		case ACTION_PREVIEW_CHANGE_CLIPBOARD:
		{
			p->buffer->rotate_layout(p->buffer);

			char *string = p->buffer->get_utf_string(p->buffer);
			show_notify(NOTIFY_PREVIEW_CHANGE_CLIPBOARD, string);
			if (string)
				free (string);
			break;
		}
	}

	// Disable receiving events
	p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);
	
	// Selection
	if ((p->action_mode != ACTION_PREVIEW_CHANGE_SELECTED) && (p->action_mode != ACTION_PREVIEW_CHANGE_CLIPBOARD))
		p->change_word(p, CHANGE_SELECTION);

	if (p->action_mode == ACTION_CHANGE_SELECTED || p->action_mode == ACTION_CHANGECASE_SELECTED || p->action_mode == ACTION_TRANSLIT_SELECTED)
	{
		if (xconfig->save_selection_after_convert)
			p->event->send_selection(p->event, p->buffer->cur_pos);
	}

	p->buffer->save_and_clear(p->buffer, p->focus->owner_window);
	
	// Unblock keyboard
	p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);

	p->action_mode = ACTION_NONE;
}


static void program_on_key_action(struct _program *p, int type)
{
	KeySym key = p->event->get_cur_keysym(p->event);
	// Delete language modifier mask
	int modifier_mask = p->event->get_cur_modifiers(p->event);
	if (type == KeyPress)
	{	
		
		p->user_action = get_user_action(key, modifier_mask);
		p->manual_action = get_manual_action(key, modifier_mask);
		// If blocked events then processing stop 
		if ((p->user_action >= 0) || (p->manual_action != ACTION_NONE) || (xconfig->block_events)) 
		{
			is_lock = TRUE;
			lock_mask = modifier_mask;
			p->event->default_event.xkey.keycode = 0;
			return;
		}

		is_lock = FALSE;
		
		p->plugin->key_press(p->plugin, key, modifier_mask);

		int auto_action = get_auto_action(p, key, modifier_mask);

		if ((auto_action != KLB_NO_ACTION) && (auto_action != KLB_CLEAR))
		{
			int lang = get_curr_keyboard_group();
			switch (lang)
			{
				default:
				case 0:
				{
					show_notify(NOTIFY_PRESS_KEY_LAYOUT_0, NULL);
					break;
				}
				case 1:
				{
					show_notify(NOTIFY_PRESS_KEY_LAYOUT_1, NULL);
					break;
				}
				case 2:
				{
					show_notify(NOTIFY_PRESS_KEY_LAYOUT_2, NULL);
					break;
				}
				case 3:
				{
					show_notify(NOTIFY_PRESS_KEY_LAYOUT_3, NULL);
					break;
				}
			}
		}

		p->perform_auto_action(p, auto_action);
	}

	if (type == KeyRelease)
	{	
		// Restore locks state to state before keyboard action.
		if (is_lock)
		{
			//log_message(ERROR, "	Set Caps to %d", (lock_mask & main_window->keymap->capslock_mask)?1:0);
			toggle_lock (main_window->keymap->capslock_mask, (lock_mask & main_window->keymap->capslock_mask)?1:0);
			//log_message(ERROR, "	Set Num to %d", (lock_mask & main_window->keymap->numlock_mask)?1:0);
			toggle_lock (main_window->keymap->numlock_mask, (lock_mask & main_window->keymap->numlock_mask)?1:0);
			//log_message(ERROR, "	Set Scroll to %d", (lock_mask & main_window->keymap->scrolllock_mask)?1:0);
			toggle_lock (main_window->keymap->scrolllock_mask, (lock_mask & main_window->keymap->scrolllock_mask)?1:0);
		}
		
		// If blocked events then processing stop 
		if (xconfig->block_events)
		{
			p->event->default_event.xkey.keycode = 0;
			if (p->manual_action == ACTION_BLOCK_EVENTS)
			{
				p->perform_manual_action(p, p->manual_action);

				p->manual_action = ACTION_NONE;
			}
			return;
		}

		p->plugin->key_release(p->plugin, key, modifier_mask);

		if (p->user_action >= 0)
		{
			log_message(LOG, _("Execute user action \"%s\""), xconfig->actions[p->user_action].name);
			p->perform_user_action(p, p->user_action);
			p->event->default_event.xkey.keycode = 0;
			p->user_action = -1;
			return;
		}

		if (p->manual_action != ACTION_NONE) 
		{
			log_message (LOG, _("Execute manual action \"%s\""), _(normal_action_names[p->manual_action]));
			if (p->perform_manual_action(p, p->manual_action))
			{
				p->manual_action = ACTION_NONE;
				p->event->default_event.xkey.keycode = 0;
				return;
			}

			p->event->default_event.xkey.keycode = 0;
			
			//p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);
			//p->event->send_xkey(p->event, XKeysymToKeycode(main_window->display, key), modifier_mask);
			//p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);
		}	
	}
}

static void program_perform_user_action(struct _program *p, int action)
{
	if (p) {};

	pthread_attr_t action_thread_attr;
	pthread_attr_init(&action_thread_attr);
	pthread_attr_setdetachstate(&action_thread_attr, PTHREAD_CREATE_DETACHED);

	pthread_t action_thread;
	pthread_create(&action_thread, &action_thread_attr,(void *) &system, (void *) xconfig->actions[action].command);

	pthread_attr_destroy(&action_thread_attr);

	show_notify(NOTIFY_EXEC_USER_ACTION, xconfig->actions[action].name);
}

static void program_perform_auto_action(struct _program *p, int action)
{
	if (p->focus->last_focus == FOCUS_EXCLUDED)
		return;
	
	struct _buffer *string = p->buffer;
	switch (action)
	{
		case KLB_NO_ACTION:
		{
			if (xconfig->disable_capslock && get_key_state(XK_Caps_Lock))
				toggle_lock(LockMask, 0);
		
			if (xconfig->flush_buffer_when_press_escape) 
				if (p->event->get_cur_keysym(p->event) == XK_Escape)
				{
					p->buffer->save_and_clear(p->buffer, p->focus->owner_window);
					p->correction_buffer->clear(p->correction_buffer);
				}
			return;
		}
		case KLB_CLEAR:
		{
			p->buffer->save_and_clear(p->buffer, p->focus->owner_window);
			p->correction_buffer->clear(p->correction_buffer);
			return;
		}
		case KLB_DEL_SYM:
		{
			if (p->last_action == ACTION_AUTOCOMPLETION)
			{
				// Block events of keyboard (push to event queue)
				p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);
				
				p->event->send_backspaces(p->event, 1);
				p->last_action = ACTION_NONE;
				
				// Restore events mask
				p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);
			}
			
			string->del_symbol(string);
			p->correction_buffer->del_symbol(p->correction_buffer);
			return;
		}
		case KLB_ENTER:
		case KLB_TAB:	
		case KLB_SPACE:
		case KLB_ADD_SYM:
		{	
			if (((action == KLB_ENTER) || (action == KLB_TAB))&& xconfig->flush_buffer_when_press_enter)
			{
				p->buffer->save_and_clear(p->buffer, p->focus->owner_window);
				p->correction_buffer->clear(p->correction_buffer);
			}
			if (((action == KLB_ENTER && xconfig->troubleshoot_enter) || (action == KLB_TAB && xconfig->troubleshoot_tab)) && 
			     !xconfig->flush_buffer_when_press_enter)
				action = KLB_ADD_SYM;
			
			if (p->changed_manual == MANUAL_FLAG_NEED_FLUSH)
			{
				p->changed_manual = MANUAL_FLAG_UNSET;
				//p->correction_action = CORRECTION_NONE;
			}
			
			char sym = main_window->keymap->get_cur_ascii_char(main_window->keymap, p->event->event);

			if (action == KLB_ADD_SYM)
			{
				// Correct small letter to capital letter after dot
				p->check_capital_letter_after_dot(p);

				// Add symbol to internal bufer
				int modifier_mask = groups[get_curr_keyboard_group()] | p->event->get_cur_modifiers(p->event);
				p->buffer->add_symbol(p->buffer, sym, p->event->event.xkey.keycode, modifier_mask);
				p->correction_buffer->add_symbol(p->correction_buffer, sym, p->event->event.xkey.keycode, modifier_mask);
				// Block events of keyboard (push to event queue)
				p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);

				// Correct space before punctuation
			    p->check_space_before_punctuation(p);

			    // Correct spaces with brackets
			    p->check_space_with_bracket(p);
				
				p->check_brackets_with_symbols(p);

				if (!xconfig->check_lang_on_process)
				{
					p->check_pattern(p);
					
					// Unblock keyboard
					p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);
					return;
				}

				p->check_copyright(p);

				p->check_trademark(p);

				p->check_registered(p);
				
				p->check_ellipsis(p);
				
				// Checking word
				if (p->changed_manual == MANUAL_FLAG_UNSET)
				{
					if (p->check_lang_last_syllable(p))
						p->event->default_event.xkey.keycode = 0;
				}

				p->last_action = ACTION_NONE;
				
				p->check_pattern(p);
				
				// Unblock keyboard
				p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);
				
				return;
			}
			
			// Block events of keyboard (push to event queue)
			p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);

			// Check two capital letter
			p->check_tcl_last_word(p);

			// Check incidental caps
			p->check_caps_last_word(p);

			// Check two minus
			p->check_two_minus(p);
			
			// Checking word
			if (p->changed_manual == MANUAL_FLAG_UNSET)
				p->check_lang_last_word(p);
			
			p->add_word_to_pattern(p, get_curr_keyboard_group());

			if (sym == ' ')
			{
				p->check_two_space(p);
			}

			// Check and correct misprint
			p->check_misprint(p);
			
			// Add symbol to internal bufer
			p->event->event = p->event->default_event;
			int modifier_mask = groups[get_curr_keyboard_group()] | p->event->get_cur_modifiers(p->event);
			p->buffer->add_symbol(p->buffer, sym, p->event->event.xkey.keycode, modifier_mask);
			p->correction_buffer->add_symbol(p->correction_buffer, sym, p->event->event.xkey.keycode, modifier_mask);
			
			//p->check_space_with_punctuation_mark(p);
			
			// Correct space before punctuation
			p->check_space_before_punctuation(p);

			// Correct spaces with brackets
			p->check_space_with_bracket(p);
			
			// Send Event
			p->event->event = p->event->default_event;
			p->event->send_next_event(p->event);
			p->event->default_event.xkey.keycode = 0;
				
			// Unblock keyboard
			p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);
			
			//if (action == KLB_ENTER && xconfig->flush_buffer_when_press_enter)
			//	p->buffer->save_and_clear(p->buffer, p->focus->owner_window);

			p->last_action = ACTION_NONE;

			if (p->changed_manual == MANUAL_FLAG_SET)
				p->changed_manual = MANUAL_FLAG_NEED_FLUSH;

			return;
		}
	}
}

static int program_perform_manual_action(struct _program *p, enum _hotkey_action action)
{
	p->plugin->hotkey_action(p->plugin, action);

	switch (action)
	{
		case ACTION_NONE:
			return FALSE;
		case ACTION_CHANGE_SELECTED:
		case ACTION_TRANSLIT_SELECTED:
		case ACTION_CHANGECASE_SELECTED:
		case ACTION_PREVIEW_CHANGE_SELECTED:
		{
			p->action_mode = action;
			p->process_selection_notify(p);
			p->event->default_event.xkey.keycode = 0;
			return TRUE;
		}
		case ACTION_CHANGE_CLIPBOARD:
		case ACTION_TRANSLIT_CLIPBOARD:
		case ACTION_CHANGECASE_CLIPBOARD:
		case ACTION_PREVIEW_CHANGE_CLIPBOARD:
		{
			p->action_mode = action;
			p->process_selection_notify(p);
			p->event->default_event.xkey.keycode = 0;
			return TRUE;
		}
		case ACTION_CHANGE_STRING:	// User needs to change current string
		{
			int curr_lang = get_curr_keyboard_group();
			int next_lang = curr_lang;
			do
			{
				next_lang++;
				if (next_lang >= xconfig->handle->total_languages)
					next_lang = 0;
			} while (xconfig->handle->languages[next_lang].excluded && (next_lang != curr_lang));
			
			if (next_lang == curr_lang)
				break;

			int action;
			if (next_lang == 0)
				action = CHANGE_STRING_TO_LAYOUT_0;
			else if (next_lang == 1)
				action = CHANGE_STRING_TO_LAYOUT_1;
			else if (next_lang == 2)
				action = CHANGE_STRING_TO_LAYOUT_2;
			else if (next_lang == 3)
				action = CHANGE_STRING_TO_LAYOUT_3;
			else
				break;

			p->change_word(p, action);
			
			show_notify(NOTIFY_CHANGE_STRING, NULL);
			break;
		}
		case ACTION_CHANGE_WORD:	// User needs to cancel last change
		case ACTION_TRANSLIT_WORD:
		case ACTION_CHANGECASE_WORD:
		case ACTION_PREVIEW_CHANGE_WORD:
		{
			p->action_mode = action;

			int curr_lang = get_curr_keyboard_group();
			int next_lang = curr_lang;
			do
			{
				next_lang++;
				if (next_lang >= xconfig->handle->total_languages)
					next_lang = 0;
			} while (xconfig->handle->languages[next_lang].excluded && (next_lang != curr_lang));
			
			if (next_lang == curr_lang)
				break;

			if ((xconfig->educate) && (action == ACTION_CHANGE_WORD) && (p->correction_action == CORRECTION_NONE))
				p->add_word_to_dict(p, next_lang);

			p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);

			int change_action = ACTION_NONE;
			
			if (action == ACTION_CHANGE_WORD)
			{
				if (next_lang == 0)
					change_action = CHANGE_WORD_TO_LAYOUT_0;
				else if (next_lang == 1)
					change_action = CHANGE_WORD_TO_LAYOUT_1;
				else if (next_lang == 2)
					change_action = CHANGE_WORD_TO_LAYOUT_2;
				else
					change_action = CHANGE_WORD_TO_LAYOUT_3;

				switch (p->correction_action)
				{
					case CORRECTION_MISPRINT:
					{
						change_action = CHANGE_MISPRINT; break;
					}
					case CORRECTION_INCIDENTAL_CAPS:
					{
						change_action = CHANGE_INCIDENTAL_CAPS; break;
					}
					case CORRECTION_TWO_CAPITAL_LETTER:
					{
						change_action = CHANGE_TWO_CAPITAL_LETTER; break;
					}
					case CORRECTION_TWO_SPACE:
					{
						change_action = CHANGE_TWO_SPACE; break;
					}
					case CORRECTION_TWO_MINUS:
					{
						change_action = CHANGE_TWO_MINUS; break;
					}
					case CORRECTION_COPYRIGHT:
					{
						change_action = CHANGE_COPYRIGHT; break;
					}
					case CORRECTION_TRADEMARK:
					{
						change_action = CHANGE_TRADEMARK; break;
					}
					case CORRECTION_REGISTERED:
					{
						change_action = CHANGE_REGISTERED; break;
					}
					case CORRECTION_ELLIPSIS:
					{
						change_action = CHANGE_ELLIPSIS; break;
					}
					case CORRECTION_NONE:
					{
						break;
					}
				};
			}	
			
			if (action == ACTION_TRANSLIT_WORD)
				change_action = CHANGE_WORD_TRANSLIT;
			
			if (action == ACTION_CHANGECASE_WORD)
				change_action = CHANGE_WORD_CHANGECASE;
			
			if (action == ACTION_PREVIEW_CHANGE_WORD)
				change_action = CHANGE_WORD_PREVIEW_CHANGE;
			
			p->change_word(p, change_action);

			show_notify(NOTIFY_MANUAL_CHANGE_WORD, NULL);
			p->event->default_event.xkey.keycode = 0;
			
			p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);

			break;
		}
		case ACTION_ENABLE_LAYOUT_0:
		{
			//XkbLockGroup(main_window->display, XkbUseCoreKbd, 0);
			set_keyboard_group(0);
			
			p->event->default_event.xkey.keycode = 0;
			show_notify(NOTIFY_ENABLE_LAYOUT_0, NULL);
			break;
		}
		case ACTION_ENABLE_LAYOUT_1:
		{
			//XkbLockGroup(main_window->display, XkbUseCoreKbd, 1);
			set_keyboard_group(1);
			
			p->event->default_event.xkey.keycode = 0;
			show_notify(NOTIFY_ENABLE_LAYOUT_1, NULL);
			break;
		}
		case ACTION_ENABLE_LAYOUT_2:
		{
			//XkbLockGroup(main_window->display, XkbUseCoreKbd, 2);
			set_keyboard_group(2);
			
			p->event->default_event.xkey.keycode = 0;
			show_notify(NOTIFY_ENABLE_LAYOUT_2, NULL);
			break;
		}
		case ACTION_ENABLE_LAYOUT_3:
		{
			//XkbLockGroup(main_window->display, XkbUseCoreKbd, 3);
			set_keyboard_group(3);
			
			p->event->default_event.xkey.keycode = 0;
			show_notify(NOTIFY_ENABLE_LAYOUT_3, NULL);
			break;
		}
		case ACTION_ROTATE_LAYOUT:
		{
			set_next_keyboard_group(xconfig->handle);
			p->event->default_event.xkey.keycode = 0;
			break;
		}
		case ACTION_ROTATE_LAYOUT_BACK:
		{
			set_prev_keyboard_group(xconfig->handle);
			p->event->default_event.xkey.keycode = 0;
			break;
		}
		case ACTION_AUTOCOMPLETION:
		{
			if (p->last_action == ACTION_AUTOCOMPLETION)
			{	
				//p->check_pattern(p, FALSE);

				// Block events of keyboard (push to event queue)
				p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);
				p->event->send_xkey(p->event, XKeysymToKeycode(main_window->display, XK_Right), p->event->event.xkey.state);
				if (xconfig->add_space_after_autocompletion)
					p->event->send_xkey(p->event, XKeysymToKeycode(main_window->display, XK_space), p->event->event.xkey.state);
				p->last_action = ACTION_NONE;

				p->buffer->save_and_clear(p->buffer, p->focus->owner_window);
				p->correction_buffer->clear(p->correction_buffer);
				  
				// Unblock keyboard
				p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);
				break;
			}
			
			// Block events of keyboard (push to event queue)
			p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);
			
			p->event->send_xkey(p->event, p->event->event.xkey.keycode, p->event->event.xkey.state);
			// Unblock keyboard
			p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);
			
			p->event->event = p->event->default_event;
			char sym = main_window->keymap->get_cur_ascii_char(main_window->keymap, p->event->event);
			int modifier_mask =  p->event->get_cur_modifiers(p->event);
			p->buffer->add_symbol(p->buffer, sym, p->event->event.xkey.keycode, modifier_mask);	

			break;
		}
		case ACTION_ROTATE_AUTOCOMPLETION:
		{
			if (p->last_action == ACTION_AUTOCOMPLETION)
			{	
				p->rotate_pattern(p);

				// Block events of keyboard (push to event queue)
				//p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);

				//p->buffer->save_and_clear(p->buffer, p->focus->owner_window);
				//p->correction_buffer->clear(p->correction_buffer);
				  
				// Unblock keyboard
				//p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);
				break;
			}
			
			// Block events of keyboard (push to event queue)
			p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);
			
			p->event->send_xkey(p->event, p->event->event.xkey.keycode, p->event->event.xkey.state);
			// Unblock keyboard
			p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);
			
			p->event->event = p->event->default_event;
			char sym = main_window->keymap->get_cur_ascii_char(main_window->keymap, p->event->event);
			int modifier_mask =  p->event->get_cur_modifiers(p->event);
			p->buffer->add_symbol(p->buffer, sym, p->event->event.xkey.keycode, modifier_mask);	

			break;
		}
		case ACTION_BLOCK_EVENTS:
		{
			p->buffer->save_and_clear(p->buffer, p->focus->owner_window);
			p->correction_buffer->clear(p->correction_buffer);
			
			p->event->default_event.xkey.keycode = 0;
			xconfig->block_events = !xconfig->block_events;
			if (xconfig->block_events)
			{
				//grab_keyboard(p->focus->owner_window, TRUE);
				show_notify(NOTIFY_BLOCK_EVENTS, NULL);
			}
			else
			{
				//p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);
				show_notify(NOTIFY_UNBLOCK_EVENTS, NULL);
			}
			log_message (DEBUG, _("Now keyboard and mouse block status is %s"), _(xconfig->get_bool_name(xconfig->block_events)));
			
			break;
		}	
		case ACTION_INSERT_DATE:
		{
			time_t curtime = time(NULL);
			struct tm *loctime = localtime(&curtime);
			if (loctime == NULL)
				break;
	
			char *date = malloc(256 * sizeof(char));
			strftime(date, 256, "%x", loctime);
			
			p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);

			// Insert Date 
			log_message(DEBUG, _("Insert date '%s'."), date);

			p->buffer->set_content(p->buffer, date);

			p->change_word(p, CHANGE_INS_DATE);

			p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);

			p->buffer->save_and_clear(p->buffer, p->focus->owner_window);
			p->correction_buffer->clear(p->correction_buffer);
			
			p->event->default_event.xkey.keycode = 0;
			if (date != NULL)
				free (date);
			break;
		}
		case ACTION_REPLACE_ABBREVIATION: // User needs to replace acronym
		{
			//MOVE this code to new function in new module
			char *utf_string = p->buffer->get_utf_string(p->buffer);

			// Check last word to acronym list
			char *word = p->buffer->get_last_word(p->buffer, utf_string);
			if (!word)
			{
				if (utf_string != NULL)
					free(utf_string);
				return FALSE;
			}

			for (int words = 0; words < xconfig->abbreviations->data_count; words++)
			{
				char *string		= strdup(xconfig->abbreviations->data[words].string);
				char *replacement	= strsep(&string, " ");

				if (string == NULL)
				{
					if (replacement != NULL)
						free(replacement);
					continue;
				}

				if (xconfig->abbr_ignore_layout)
				{
					KeyCode *dummy_kc = malloc(strlen(replacement) * sizeof(KeyCode));
					int *dummy_kc_mod = malloc(strlen(replacement) * sizeof(int));
					main_window->keymap->convert_text_to_ascii(main_window->keymap, replacement, dummy_kc, dummy_kc_mod);
					if (dummy_kc != NULL)
						free(dummy_kc);
					if (dummy_kc_mod != NULL)
						free(dummy_kc_mod);
					
					dummy_kc = malloc(strlen(word) * sizeof(KeyCode));
					dummy_kc_mod = malloc(strlen(word) * sizeof(int));
					main_window->keymap->convert_text_to_ascii(main_window->keymap, word, dummy_kc, dummy_kc_mod);

					if (dummy_kc != NULL)
						free(dummy_kc);
					if (dummy_kc_mod != NULL)
						free(dummy_kc_mod);
				}

				if (strcmp(replacement, word) != 0)
				{
					if (replacement != NULL)
						free(replacement);
					continue;
				}

				p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);

				// Replace Abbreviation
				log_message(DEBUG, _("Found Abbreviation '%s' '%s'. Replacing to '%s'."), replacement, word, string);

				int backspaces_count = strlen(p->buffer->get_last_word(p->buffer, p->buffer->content));
				if (p->last_action == ACTION_AUTOCOMPLETION)
					backspaces_count++; 
				p->event->send_backspaces(p->event, backspaces_count);
				p->buffer->set_content(p->buffer, string);

				p->change_word(p, CHANGE_ABBREVIATION);

				p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);

				show_notify(NOTIFY_REPLACE_ABBREVIATION, NULL);
				p->buffer->save_and_clear(p->buffer, p->focus->owner_window);
				p->correction_buffer->clear(p->correction_buffer);
				
				//Incapsulate to p->event->clear_code() or smth else
				p->event->default_event.xkey.keycode = 0;

				if (replacement != NULL)
					free(replacement);
				if (utf_string != NULL)
					free(utf_string);
				return TRUE;
			}

			if (utf_string != NULL)
				free(utf_string);
			return FALSE;
		}
	}

	// When CHANGE_STRING or CHANGE_WORD actions occured
	if ((xconfig->troubleshoot_switch) && (p->buffer->cur_pos > 0))
	{
		if (p->buffer->content[p->buffer->cur_pos-1] != ' ')
			p->changed_manual = MANUAL_FLAG_SET;
	}
	return TRUE;
}

static int program_check_lang_last_word(struct _program *p)
{
	if (xconfig->handle->languages[get_curr_keyboard_group()].excluded)
		return FALSE;
	
	if (p->app_forced_mode == FORCE_MODE_MANUAL)
		return FALSE;

	if (p->app_forced_mode != FORCE_MODE_AUTO && xconfig->manual_mode)
		return FALSE;

	const char *word = p->buffer->get_last_word(p->buffer, p->buffer->content);
	if (!word)
		return FALSE;

	if ((p->buffer->content[p->buffer->cur_pos-1] == ' ') ||
	    (p->buffer->content[p->buffer->cur_pos-1] == 0) || // Nothing word
		(p->buffer->content[p->buffer->cur_pos-1] == 13) || // Enter
	    (p->buffer->content[p->buffer->cur_pos-1] == 11))   // Tab
			return FALSE;
	
	int cur_lang = get_curr_keyboard_group();

	int new_lang = NO_LANGUAGE;
	if (xconfig->check_similar_words)
	{
		new_lang = check_lang_with_similar_words(xconfig->handle, p->buffer, cur_lang);
	}
	else
	{
		new_lang = check_lang(xconfig->handle, p->buffer, cur_lang);
	}
	
	if (new_lang == NO_LANGUAGE)
	{
		log_message(DEBUG, _("No language found to change to"));
		//get_suggest(p, cur_lang); 
		return FALSE;
	}

	if (new_lang == cur_lang)
	{
		//get_suggest(p, cur_lang);
		return FALSE;
	}
	
	int change_action = CHANGE_WORD_TO_LAYOUT_0;
	if (new_lang == 0)
		change_action = CHANGE_WORD_TO_LAYOUT_0;
	else if (new_lang == 1)
		change_action = CHANGE_WORD_TO_LAYOUT_1;
	else if (new_lang == 2)
		change_action = CHANGE_WORD_TO_LAYOUT_2;
	else
		change_action = CHANGE_WORD_TO_LAYOUT_3;

	p->change_word(p, change_action);
	show_notify(NOTIFY_AUTOMATIC_CHANGE_WORD, NULL);

	p->last_layout = new_lang;

	//get_suggest(p, new_lang);
	return TRUE;
}

static int program_check_lang_last_syllable(struct _program *p)
{
	if (xconfig->handle->languages[get_curr_keyboard_group()].excluded)
		return FALSE;
	
	if (p->app_forced_mode == FORCE_MODE_MANUAL)
		return FALSE;

	if (p->app_forced_mode != FORCE_MODE_AUTO && xconfig->manual_mode)
		return FALSE;

	const char *word = p->buffer->get_last_word(p->buffer, p->buffer->content);
	if (!word)
		return FALSE;

	if (strlen(word) < 3)
		return FALSE;

	int cur_lang = get_curr_keyboard_group();
	int new_lang = check_lang(xconfig->handle, p->buffer, cur_lang);

	if (new_lang == NO_LANGUAGE)
	{
		log_message(DEBUG, _("No language found to change to"));
		return FALSE;
	}

	if (new_lang == cur_lang)
		return FALSE;

	int change_action = 0;
	if (new_lang == 0)
		change_action = CHANGE_SYLL_TO_LAYOUT_0;
	else if (new_lang == 1)
		change_action = CHANGE_SYLL_TO_LAYOUT_1;
	else if (new_lang == 2)
		change_action = CHANGE_SYLL_TO_LAYOUT_2;
	else
		change_action = CHANGE_SYLL_TO_LAYOUT_3;

	p->change_word(p, change_action);
	show_notify(NOTIFY_AUTOMATIC_CHANGE_WORD, NULL);

	p->last_layout = new_lang;

	return TRUE;
}

static void program_check_caps_last_word(struct _program *p)
{
	if (!xconfig->correct_incidental_caps)
		return;

	int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

	if (!(p->buffer->keycode_modifiers[offset] & LockMask) || !(p->buffer->keycode_modifiers[offset] & ShiftMask))
		return;

	for (int i = 1; i < p->buffer->cur_pos - offset; i++)
	{
		if ((p->buffer->keycode_modifiers[offset + i] & LockMask) && (p->buffer->keycode_modifiers[offset+i] & ShiftMask))
			return;
		if (!(p->buffer->keycode_modifiers[offset + i] & LockMask))
			return;
	}

	p->correction_buffer->set_content(p->correction_buffer, p->buffer->content);

	if (p->correction_action != CORRECTION_NONE)
		p->correction_action = CORRECTION_NONE;
	
	p->change_word(p, CHANGE_INCIDENTAL_CAPS);
	show_notify(NOTIFY_CORR_INCIDENTAL_CAPS, NULL);

	p->correction_action = CORRECTION_INCIDENTAL_CAPS;
}

static void program_check_tcl_last_word(struct _program *p)
{
	if (!xconfig->correct_two_capital_letter)
		return;

	int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

	if (!isalpha(p->buffer->content[offset]))
		return;
	
	if (p->buffer->cur_pos - offset <= 2)
		return;

	if (!isalpha(p->buffer->content[offset+1]))
		return;
	
	if (isblank(p->buffer->content[offset+2]) || ispunct(p->buffer->content[offset+2]))
		return;

	if (!(p->buffer->keycode_modifiers[offset] & ShiftMask) || !(p->buffer->keycode_modifiers[offset + 1] & ShiftMask))
		return;

	for (int i = 2; i < p->buffer->cur_pos - offset; i++)
	{
		if ((p->buffer->keycode_modifiers[offset + i] & ShiftMask) && (isalpha(p->buffer->content[offset + i])))
			return;
	}

	p->correction_buffer->set_content(p->correction_buffer, p->buffer->content);

	if (p->correction_action != CORRECTION_NONE)
		p->correction_action = CORRECTION_NONE;
	
	p->change_word(p, CHANGE_TWO_CAPITAL_LETTER);
	show_notify(NOTIFY_CORR_TWO_CAPITAL_LETTER, NULL);

	p->correction_action = CORRECTION_TWO_CAPITAL_LETTER;
}

static void program_check_two_space(struct _program *p)
{
	if (!xconfig->correct_two_space_with_comma_and_space)
		return;

	if (p->buffer->cur_pos < 3)
		return;
	
	if (p->buffer->content[p->buffer->cur_pos-1] != ' ')
		return;

	char *word = strdup(p->buffer->get_last_word(p->buffer, p->buffer->i18n_content[get_curr_keyboard_group()].content_unchanged));
	if (word == NULL)
		return;

	int pos = strlen(word);
	trim_word(word, pos);
	pos = strlen(word);
	if (pos < 3)
		return;
	
	pos = pos - 1;
	if (ispunct(word[pos]) || isspace(word[pos]))
	{
		if (word != NULL)
			free(word);
		return;
	}

	log_message (DEBUG, _("Find two space, correction with a comma and a space..."));

	if (word != NULL)
		free(word);

	p->correction_buffer->set_content(p->correction_buffer, p->buffer->content);

	if (p->correction_action != CORRECTION_NONE)
		p->correction_action = CORRECTION_NONE;
	
	p->change_word(p, CHANGE_TWO_SPACE);
	show_notify(NOTIFY_CORR_TWO_SPACE, NULL);

	p->correction_action = CORRECTION_TWO_SPACE;
}		

static void program_check_two_minus(struct _program *p)
{
	if (!xconfig->correct_two_minus_with_dash)
		return;

	if (p->buffer->cur_pos < 2)
		return;

	if ((p->buffer->content[p->buffer->cur_pos-1] != '-') || (p->buffer->content[p->buffer->cur_pos-2] != '-')) 
		return;

	log_message (DEBUG, _("Find two minus, correction with a dash..."));
	
	p->correction_buffer->set_content(p->correction_buffer, p->buffer->content);

	if (p->correction_action != CORRECTION_NONE)
		p->correction_action = CORRECTION_NONE;

	p->change_word(p, CHANGE_TWO_MINUS);	
	show_notify(NOTIFY_CORR_TWO_MINUS, NULL);
	p->correction_action = CORRECTION_TWO_MINUS;
}		

static void program_check_copyright(struct _program *p)
{
	if (!xconfig->correct_c_with_copyright)
		return;

	if (p->buffer->cur_pos < 3)
		return;
	
	if ((p->buffer->content[p->buffer->cur_pos-1] != ')') || 
		(p->buffer->content[p->buffer->cur_pos-2] != 'c') ||
		(p->buffer->content[p->buffer->cur_pos-3] != '(')) 
		return;
	
	log_message (DEBUG, _("Find (c), correction with a copyright sign..."));
	
	p->correction_buffer->set_content(p->correction_buffer, p->buffer->content);

	if (p->correction_action != CORRECTION_NONE)
		p->correction_action = CORRECTION_NONE;

	p->change_word(p, CHANGE_COPYRIGHT);	
	show_notify(NOTIFY_CORR_COPYRIGHT, NULL);
	p->correction_action = CORRECTION_COPYRIGHT;	
}		

static void program_check_registered(struct _program *p)
{
	if (!xconfig->correct_r_with_registered)
		return;

	if (p->buffer->cur_pos < 3)
		return;
	
	if ((p->buffer->content[p->buffer->cur_pos-1] != ')') || 
		(p->buffer->content[p->buffer->cur_pos-2] != 'r') ||
		(p->buffer->content[p->buffer->cur_pos-3] != '(')) 
		return;
		
	log_message (DEBUG, _("Find (r), correction with a registered sign..."));
	
	p->correction_buffer->set_content(p->correction_buffer, p->buffer->content);

	if (p->correction_action != CORRECTION_NONE)
		p->correction_action = CORRECTION_NONE;

	p->change_word(p, CHANGE_REGISTERED);
	show_notify(NOTIFY_CORR_REGISTERED, NULL);

	p->correction_action = CORRECTION_REGISTERED;
}		

static void program_check_trademark(struct _program *p)
{
	if (!xconfig->correct_tm_with_trademark)
		return;

	if (p->buffer->cur_pos < 4)
		return;
	
	if ((p->buffer->content[p->buffer->cur_pos-1] != ')') || 
		(p->buffer->content[p->buffer->cur_pos-2] != 'm') ||
		(p->buffer->content[p->buffer->cur_pos-3] != 't') ||
		(p->buffer->content[p->buffer->cur_pos-4] != '(')) 
		return;
	
	log_message (DEBUG, _("Find (tm), correction with a trademark sign..."));
	
	p->correction_buffer->set_content(p->correction_buffer, p->buffer->content);

	if (p->correction_action != CORRECTION_NONE)
		p->correction_action = CORRECTION_NONE;

	p->change_word(p, CHANGE_TRADEMARK);
	show_notify(NOTIFY_CORR_TRADEMARK, NULL);

	p->correction_action = CORRECTION_TRADEMARK;
}		

static void program_check_ellipsis(struct _program *p)
{
	if (!xconfig->correct_three_points_with_ellipsis)
		return;

	if (p->buffer->cur_pos < 3)
		return;
	
	char *text = p->buffer->get_utf_string(p->buffer);
	if (text == NULL)
		return;
	
	int text_len = strlen(text);

	if ((text[text_len-1] != '.') || 
		(text[text_len-2] != '.') ||
		(text[text_len-3] != '.')) 
	{
		if (text != NULL)
			free(text);
		return;
	}
	
	log_message (DEBUG, _("Find three points, correction with a ellipsis sign..."));

	p->correction_buffer->set_content(p->correction_buffer, p->buffer->content);

	if (p->correction_action != CORRECTION_NONE)
		p->correction_action = CORRECTION_NONE;

	p->change_word(p, CHANGE_ELLIPSIS);
	show_notify(NOTIFY_CORR_ELLIPSIS, NULL);

	//p->correction_buffer->set_content(p->correction_buffer, p->buffer->content);
	
	p->correction_action = CORRECTION_ELLIPSIS;
	if (text != NULL)
		free(text);
}

static void program_check_space_before_punctuation(struct _program *p)
{
	if (!xconfig->correct_space_with_punctuation)
		return;
	
	char *text = p->buffer->get_utf_string(p->buffer);
	if (text == NULL)
		return;

	if (p->buffer->cur_pos < 3)
	{
		if (text != NULL)
			free(text);
		return;
	}
	
	int text_len = strlen(text);
	if (text[text_len - 1] != '.' && text[text_len - 1] != ',' && text[text_len - 1] != '!' && 
	    text[text_len - 1] != '?' && text[text_len - 1] != ';' && text[text_len - 1] != ':')
	{
		if (text != NULL)
			free(text);
		return;
	}
	
	if (text[text_len - 2] != ' ')
	{
		if (text != NULL)
			free(text);
		return;
	}
	
	log_message(DEBUG, _("Find spaces before punctuation, correction..."));

	p->buffer->del_symbol(p->buffer);
	p->correction_buffer->del_symbol(p->correction_buffer);
	while (p->buffer->content[p->buffer->cur_pos-1] == ' ')
	{
		p->event->send_backspaces(p->event, 1);
		p->buffer->del_symbol(p->buffer);
		p->correction_buffer->del_symbol(p->correction_buffer);
	}

	p->event->event = p->event->default_event;
	char sym = main_window->keymap->get_cur_ascii_char(main_window->keymap, p->event->event);
	int modifier_mask = groups[get_curr_keyboard_group()] | p->event->get_cur_modifiers(p->event);
	p->buffer->add_symbol(p->buffer, sym, p->event->event.xkey.keycode, modifier_mask);

	if (text != NULL)
		free(text);
}

static void program_check_space_with_bracket(struct _program *p)
{
	if (!xconfig->correct_space_with_punctuation)
		return;
	
	char *text = p->buffer->get_utf_string(p->buffer);
	if (text == NULL)
		return;

	if (p->buffer->cur_pos < 3)
	{
		if (text != NULL)
			free(text);
		return;
	}
	
	int text_len = strlen(text);
	if (text[text_len - 1] != '(' && text[text_len - 1] != ')')
	{
		if (text != NULL)
			free(text);
		return;
	}
	
	if (((text[text_len - 1] == '(') && (text[text_len - 2] == ' ' || text[text_len - 2] == ':' || text[text_len - 2] == ';' || text[text_len - 2] == '-' || text[text_len - 2] == '\r' || text[text_len - 2] == '\n' || text[text_len - 2] == '\t' || isdigit(text[text_len - 2]))) ||
	    ((text[text_len - 1] == ')' && text[text_len - 2] != ' ' )))
	{
		if (text != NULL)
			free(text);
		return;
	}
	
	if (text[text_len - 1] == '(')
	{
		log_message(DEBUG, _("Find no space before left bracket, correction..."));
		
		p->buffer->del_symbol(p->buffer);
		p->correction_buffer->del_symbol(p->correction_buffer);
		p->event->event = p->event->default_event;
		p->event->event.xkey.keycode = XKeysymToKeycode(main_window->display, XK_space);
		p->event->send_next_event(p->event);
		int modifier_mask = groups[get_curr_keyboard_group()];
		p->buffer->add_symbol(p->buffer, ' ', p->event->event.xkey.keycode, modifier_mask);

		p->event->event = p->event->default_event;
		char sym = main_window->keymap->get_cur_ascii_char(main_window->keymap, p->event->event);
		modifier_mask |=  p->event->get_cur_modifiers(p->event);
		p->buffer->add_symbol(p->buffer, sym, p->event->event.xkey.keycode, modifier_mask);	
	}

	if (text[text_len - 1] == ')')
	{
		log_message(DEBUG, _("Find spaces before right bracket, correction..."));

		p->buffer->del_symbol(p->buffer);
		p->correction_buffer->del_symbol(p->correction_buffer);
		while (p->buffer->content[p->buffer->cur_pos - 1] == ' ')
		{
			p->event->send_backspaces(p->event, 1);
			p->buffer->del_symbol(p->buffer);
			p->correction_buffer->del_symbol(p->correction_buffer);
		}
		p->event->event = p->event->default_event;
		char sym = main_window->keymap->get_cur_ascii_char(main_window->keymap, p->event->event);
		int modifier_mask = groups[get_curr_keyboard_group()] | p->event->get_cur_modifiers(p->event);
		p->buffer->add_symbol(p->buffer, sym, p->event->event.xkey.keycode, modifier_mask);	
	}

	if (text != NULL)
		free(text);
}

static void program_check_brackets_with_symbols(struct _program *p)
{
	if (!xconfig->correct_space_with_punctuation)
		return;

	char *text = strdup(p->buffer->content);
	if (text == NULL)
		return;
	
	int text_len = strlen(text);
	if (text_len < 2)
	{
		if (text != NULL)
			free(text);
		return;
	}
	
	if (text[text_len-2] == ')')
	{
		char sym = main_window->keymap->get_cur_ascii_char(main_window->keymap, p->event->default_event);
		//log_message(ERROR, "%c %d", text[text_len], text_len);
		if (ispunct(sym))
		{
			if (text != NULL)
				free(text);
			return;
		}
		log_message(DEBUG, _("Find no spaces after right bracket, correction..."));
		
		p->buffer->del_symbol(p->buffer);
		p->correction_buffer->del_symbol(p->correction_buffer);
		p->event->event = p->event->default_event;
		p->event->event.xkey.keycode = XKeysymToKeycode(main_window->display, XK_space);
		p->event->send_next_event(p->event);
		int modifier_mask = groups[get_curr_keyboard_group()];
		p->buffer->add_symbol(p->buffer, ' ', p->event->event.xkey.keycode, modifier_mask);

		p->event->event = p->event->default_event;
		sym = main_window->keymap->get_cur_ascii_char(main_window->keymap, p->event->event);
		modifier_mask = groups[get_curr_keyboard_group()] | p->event->get_cur_modifiers(p->event);
		p->buffer->add_symbol(p->buffer, sym, p->event->event.xkey.keycode, modifier_mask);	
	}

	if (text[text_len - 2] != ' ')
	{
		if (text != NULL)
			free(text);
		return;
	}
	
	int space_count = 0;
	int pos = text_len - 2;
	while ((pos >= 0) && (text[pos] == ' '))
	{
		space_count++;
		pos--;
	} 
	
	if (pos < 0 || text[pos] != '(')
	{
		if (text != NULL)
			free(text);
	    return;
	}
	    
	log_message(DEBUG, _("Find spaces after left bracket, correction..."));

	p->buffer->del_symbol(p->buffer);
	p->correction_buffer->del_symbol(p->correction_buffer);
	for (int i = 0; i < space_count; i++)
	{
		p->event->send_backspaces(p->event, 1);
		p->buffer->del_symbol(p->buffer);
		p->correction_buffer->del_symbol(p->correction_buffer);
	}
	p->event->event = p->event->default_event;
	char sym = main_window->keymap->get_cur_ascii_char(main_window->keymap, p->event->event);
	int modifier_mask = groups[get_curr_keyboard_group()] | p->event->get_cur_modifiers(p->event);
	p->buffer->add_symbol(p->buffer, sym, p->event->event.xkey.keycode, modifier_mask);	

	if (text != NULL)
		free(text);
}

static void program_check_capital_letter_after_dot(struct _program *p)
{
	if (!xconfig->correct_capital_letter_after_dot)
		return;

	if (p->buffer->cur_pos < 3)
		return;

	if (p->event->event.xkey.state & ShiftMask)
		return;

	char *symbol = main_window->keymap->keycode_to_symbol(main_window->keymap, p->event->event.xkey.keycode, get_curr_keyboard_group(), p->event->event.xkey.state);
	if (symbol == NULL)
		return;

	switch (symbol[0])
	{
		case '`':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '0':
		case '-':
		case '=':
		case '[':
		case ']':
		case '\\':
		case '/':
		case ';':
		case '\'':
		case ',':
		case '.':
		{
			if (symbol != NULL)
				free (symbol);
			return;
		}
	}
	if (symbol != NULL)
		free (symbol);
	
	char *text = strdup(p->buffer->content);
	if (text == NULL)
		return;

	int offset = strlen(text) - 1;
	if ((text[offset] != ' ') && (text[offset] != 13) && (text[offset] != 9))
	{
		if (text != NULL)
			free(text);
		return;
	}
	if (text != NULL)
		free (text);

	text = p->buffer->get_utf_string_on_kbd_group(p->buffer, get_curr_keyboard_group());

	for (offset = strlen(text) - 2; offset > 1; offset--)
	{
		if ((text[offset] != ' ') && (text[offset] != 13) && (text[offset] != 9))
			break;
	}

	if ((text[offset] == '.') || (text[offset] == '!') || (text[offset] == '?'))
	{
		log_message(DEBUG, _("Find small letter after dot, correction..."));
		p->event->event.xkey.state = p->event->event.xkey.state | ShiftMask;
		p->event->default_event.xkey.state = p->event->default_event.xkey.state | ShiftMask;
	}
	if (text != NULL)
		free(text);
}

static void program_check_pattern(struct _program *p)
{
	if (!xconfig->autocompletion)
		return;

	if (p->app_autocompletion_mode == AUTOCOMPLETION_EXCLUDED)
		return;
	
	char *tmp = p->buffer->get_last_word(p->buffer, p->buffer->content);
	if (tmp == NULL)
		return;
	
	if (strlen(tmp) < MIN_PATTERN_LEN - 1)
		return;

	int lang = get_curr_keyboard_group();
	tmp = p->buffer->get_last_word(p->buffer, p->buffer->i18n_content[lang].content);

	char *word = strdup(tmp);

	int len = trim_word(word, strlen(tmp));
	if (len == 0)
	{
		if (word != NULL)
			free (word);
		return;
	}

	/*
	if (selection) 
	{
	struct _list_char* list_alike = xconfig->handle->languages[lang].pattern->alike(xconfig->handle->languages[lang].pattern, word);
	if (list_alike != NULL)
	{
		for (int i = 0; i < list_alike->data_count; i++)
		{
			log_message (ERROR, "%s", list_alike->data[i].string);
		}

		
		list_alike->uninit(list_alike);
		list_alike  =  NULL;
	}	
	if (list_alike != NULL)
	  list_alike->uninit(list_alike);

		
	}*/
	
	struct _list_char_data *pattern_data = xconfig->handle->languages[lang].pattern->find_alike(xconfig->handle->languages[lang].pattern, word);
	if (pattern_data == NULL)
	{
		p->last_action = ACTION_NONE;
		if (word != NULL)
			free (word);
		return;
	}
	
	log_message (DEBUG, _("Recognition word '%s' from text '%s' (layout %d), autocompletation..."), pattern_data->string, word, get_curr_keyboard_group());
	
	p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);

	struct _buffer *tmp_buffer = buffer_init(xconfig->handle, main_window->keymap);
	
	tmp_buffer->set_content(tmp_buffer, pattern_data->string + strlen(word)*sizeof(char));

	if (tmp_buffer->cur_pos == 0)
	{
		tmp_buffer->uninit(tmp_buffer);
		p->last_action = ACTION_NONE;
		if (word != NULL)
			free (word);
		return;
	}

	p->event->event = p->event->default_event;
	p->event->send_next_event(p->event);
	
	p->event->send_string(p->event, tmp_buffer);
	p->event->send_selection(p->event, tmp_buffer->cur_pos);

	p->event->default_event.xkey.keycode = 0;
	
	tmp_buffer->uninit(tmp_buffer);

	p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);

	p->last_action = ACTION_AUTOCOMPLETION;
	p->last_pattern_id = 0;

	if (word != NULL)
		free (word);
}

static void program_rotate_pattern(struct _program *p)
{
	if (!xconfig->autocompletion)
		return;

	if (p->app_autocompletion_mode == AUTOCOMPLETION_EXCLUDED)
		return;

	if (p->last_action != ACTION_AUTOCOMPLETION)
		return;
	
	char *tmp = p->buffer->get_last_word(p->buffer, p->buffer->content);
	if (tmp == NULL)
		return;
	
	if (strlen(tmp) < MIN_PATTERN_LEN - 1)
		return;

	int lang = get_curr_keyboard_group();
	tmp = p->buffer->get_last_word(p->buffer, p->buffer->i18n_content[lang].content);

	char *word = strdup(tmp);

	int len = trim_word(word, strlen(tmp));
	if (len == 0)
	{
		if (word != NULL)
			free (word);
		return;
	}

	struct _list_char* list_alike = xconfig->handle->languages[lang].pattern->alike(xconfig->handle->languages[lang].pattern, word);
	if (list_alike == NULL)
	{
		if (word != NULL)
			free (word);
		return;
	}
	
	/*for (int i = 0; i < list_alike->data_count; i++)
	{
		log_message (ERROR, "%s", list_alike->data[i].string);
	}*/

	if (list_alike->data_count == 1)
	{
		list_alike->uninit(list_alike);
		list_alike  =  NULL;
		if (word != NULL)
			free (word);
		return;
	}

	p->last_pattern_id++;
	if (p->last_pattern_id == list_alike->data_count)
		p->last_pattern_id = 0;
	
    log_message (DEBUG, _("Next autocompletion word '%s' from text '%s' (layout %d), rotate autocompletation..."), list_alike->data[p->last_pattern_id].string, word, get_curr_keyboard_group());
	
	p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);

	struct _buffer *tmp_buffer = buffer_init(xconfig->handle, main_window->keymap);
	
	tmp_buffer->set_content(tmp_buffer, list_alike->data[p->last_pattern_id].string + strlen(word)*sizeof(char));

	if (tmp_buffer->cur_pos == 0)
	{
		list_alike->uninit(list_alike);
		tmp_buffer->uninit(tmp_buffer);
		p->last_action = ACTION_NONE;
		if (word != NULL)
			free (word);
		return;
	}

	p->event->event = p->event->default_event;
	p->event->send_next_event(p->event);
	
	p->event->send_string(p->event, tmp_buffer);
	p->event->send_selection(p->event, tmp_buffer->cur_pos);

	p->event->default_event.xkey.keycode = 0;
	
	tmp_buffer->uninit(tmp_buffer);

	p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);

	p->last_action = ACTION_AUTOCOMPLETION;
	if (word != NULL)
		free (word);
		
	list_alike->uninit(list_alike);
	list_alike  =  NULL;
}

static void program_check_misprint(struct _program *p)
{
	if (!xconfig->correct_misprint)
		return;

	int lang = get_curr_keyboard_group ();
	if (xconfig->handle->languages[lang].disable_auto_detection || xconfig->handle->languages[lang].excluded)
		return;

	char *word = strdup(p->buffer->get_last_word(p->buffer, p->buffer->i18n_content[lang].content_unchanged));
	
	del_final_numeric_char(word);

	if (word == NULL)
		return;

	if ((strlen(word) > 250) || (strlen(word) < 4))
	{
		if (word != NULL)
			free(word);
		return;
	}
	
	if (xconfig->handle->languages[lang].dictionary->exist(xconfig->handle->languages[lang].dictionary, word, BY_REGEXP))
	{
		if (word != NULL)
			free(word);
		return;
	}
	
	int min_levenshtein = 3;
	char *possible_word = NULL;

	// check leader puctuation char
	unsigned int offset = 0;
	
	for (offset = 0; offset < strlen(word); offset++)
	{
		if (!ispunct(word[offset]) && (!isdigit(word[offset])))
			break;
	}
	
#ifdef WITH_ENCHANT 
	size_t count = 0;

	if (!xconfig->handle->has_enchant_checker[lang])
	{
		if (word != NULL)
			free(word);
		return;
	}

	if (strlen(word+offset) <= 0)
	{
		if (word != NULL)
			free(word);
		return;
	}
	
	if (!enchant_dict_check(xconfig->handle->enchant_dicts[lang], word+offset, strlen(word+offset)))
	{
		if (word != NULL)
			free(word);
		return;
	}

	char **suggs = enchant_dict_suggest (xconfig->handle->enchant_dicts[lang], word+offset, strlen(word+offset), &count); 
	if (count > 0)
	{

		for (unsigned int i = 0; i < count; i++)
		{
			int tmp_levenshtein = levenshtein(word+offset, suggs[i]);
			if (tmp_levenshtein < min_levenshtein)
				min_levenshtein = tmp_levenshtein;
		}	

		if (min_levenshtein < 3) 
		{
			for (unsigned int i = 0; i < count; i++)
			{
				int tmp_levenshtein = levenshtein(word+offset, suggs[i]);
				if (tmp_levenshtein == min_levenshtein)
				{
					if (xconfig->handle->languages[lang].pattern->exist(xconfig->handle->languages[lang].pattern, suggs[i], BY_PLAIN))	
					{
						possible_word = strdup(suggs[i]);
						break;
					}
				}
			}
			if (possible_word == NULL)
				possible_word = strdup(suggs[0]);
		}
	}

	enchant_dict_free_string_list(xconfig->handle->enchant_dicts[lang], suggs);
#endif

#ifdef WITH_ASPELL
	if (!xconfig->handle->has_spell_checker[lang])
	{
		if (word != NULL)
			free(word);
		return;
	}
	if (aspell_speller_check(xconfig->handle->spell_checkers[lang], word+offset, strlen(word+offset)))
	{
		if (word != NULL)
			free(word);
		return;
	}
	const AspellWordList *suggestions = aspell_speller_suggest (xconfig->handle->spell_checkers[lang], (const char *) word+offset, strlen(word+offset));
	if (! suggestions)
	{
		if (word != NULL)
			free(word);
		return;
	}

	AspellStringEnumeration *elements = aspell_word_list_elements(suggestions);
	const char *sugg_word;
	while ((sugg_word = aspell_string_enumeration_next (elements)) != NULL)
	{		
		int tmp_levenshtein = levenshtein(word+offset, sugg_word);
		if (tmp_levenshtein < min_levenshtein)
			min_levenshtein = tmp_levenshtein;
	}
	delete_aspell_string_enumeration (elements);

	// Check word in user's pattern  
	elements = aspell_word_list_elements(suggestions);
	if (min_levenshtein < 3) 
	{
		char *first_sugg = NULL;
		while ((sugg_word = aspell_string_enumeration_next (elements)) != NULL)
		{	
			int tmp_levenshtein = levenshtein(word+offset, sugg_word);
			if (tmp_levenshtein == min_levenshtein)
			{
				if (first_sugg == NULL)
					first_sugg = strdup(sugg_word);
				if (xconfig->handle->languages[lang].pattern->exist(xconfig->handle->languages[lang].pattern, sugg_word, BY_PLAIN))
				{
					possible_word = strdup(sugg_word);
					break;
				}
			}
		}
		// If user's pattern is clear then get first word 
		if (possible_word == NULL)
			possible_word = first_sugg;
		else
			if (first_sugg != NULL)
				free(first_sugg);
	}	


	delete_aspell_string_enumeration (elements);
#endif

	if (possible_word != NULL)	
	{
		if (p->correction_action != CORRECTION_NONE)
			p->correction_action = CORRECTION_NONE;
		
		p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);

		log_message (DEBUG, _("Found a misprint , correction '%s' to '%s'..."), word+offset, possible_word);
;
		p->correction_buffer->set_content(p->correction_buffer, p->buffer->content);
		
		int backspaces_count = p->buffer->cur_pos - p->buffer->get_last_word_offset (p->buffer, p->buffer->content, p->buffer->cur_pos) - offset;
		p->event->send_backspaces(p->event, backspaces_count);

		if (p->last_action == ACTION_AUTOCOMPLETION)
			p->event->send_backspaces(p->event, 1);
		for (int i = 0; i < (backspaces_count); i++)
			p->buffer->del_symbol(p->buffer);
		
		int new_offset = p->buffer->cur_pos;
		int possible_word_len = strlen(possible_word);
		char *new_content = malloc((p->buffer->cur_pos + possible_word_len + backspaces_count + 1) * sizeof(char));
		memset(new_content, 0, (p->buffer->cur_pos + possible_word_len + backspaces_count + 1) * sizeof(char));
		//char *new_content = malloc(1024 * sizeof(char));
		//memset(new_content, 0, 1024 * sizeof(char));
		new_content = strcat(new_content, p->buffer->content);
		new_content = strcat(new_content, possible_word);
		// после исправления опечатки, добавляем запятые и прочее, идущее после слова >>>
		int finish_offset = 0;
		for (int i = strlen(p->correction_buffer->i18n_content[lang].content_unchanged) - 1; i >= 0 ; i--)
		{
			finish_offset++;
			if (  (!ispunct(p->correction_buffer->i18n_content[lang].content_unchanged[i])) && 
			      (!isdigit(p->correction_buffer->i18n_content[lang].content_unchanged[i])) && 
			      (!isspace(p->correction_buffer->i18n_content[lang].content_unchanged[i])))
			{
				break;
			}
		}
		new_content = strcat(new_content, p->correction_buffer->i18n_content[lang].content_unchanged + strlen(p->correction_buffer->i18n_content[lang].content_unchanged) - finish_offset + 1);
		// <<<
		
		p->buffer->set_content(p->buffer, new_content);
		if (new_content != NULL)
			free(new_content);

		p->buffer->set_offset(p->buffer, new_offset);
		p->send_string_silent(p, 0);	
		p->buffer->unset_offset(p->buffer, new_offset);
		
		p->focus->update_events(p->focus, LISTEN_GRAB_INPUT);

		int notify_text_len = strlen(_("Correction '%s' to '%s'")) + strlen(word+offset) + 1 + possible_word_len;
		char *notify_text = (char *) malloc(notify_text_len * sizeof(char));
		snprintf(notify_text , notify_text_len, _("Correction '%s' to '%s'"), word+offset, possible_word);			
		show_notify(NOTIFY_CORR_MISPRINT, notify_text);
		if (notify_text != NULL)
			free(notify_text);

		p->correction_action = CORRECTION_MISPRINT;
		//p->buffer->save_and_clear(p->buffer, p->focus->owner_window);

		if (possible_word != NULL)
			free(possible_word);
	}
	if (word != NULL)
		free(word);
}

static void program_send_string_silent(struct _program *p, int send_backspaces)
{
	if (p->buffer->cur_pos == 0)
	{
		log_message(DEBUG, _("No string to change"));
		return;
	}

	log_message(DEBUG, _("Processing string '%s'"), p->buffer->content);

	// Work-arround
	if ((xconfig->compatibility_with_completion) && (p->last_action != ACTION_AUTOCOMPLETION))
	{
		p->event->send_xkey(p->event, XKeysymToKeycode(main_window->display, XK_bar), 1);
		p->event->send_backspaces(p->event, 1);
		
	} // end workarround
	
	p->event->send_backspaces(p->event, send_backspaces);		// Delete old string
	p->event->send_string(p->event, p->buffer);		// Send new string
}

static void program_change_word(struct _program *p, enum _change_action action)
{
	switch (action)
	{
		case CHANGE_INCIDENTAL_CAPS:
		{
			if (p->correction_action == CORRECTION_NONE) 
			{
				int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

				// Shift fields to point to begin of word
				p->buffer->set_offset(p->buffer, offset);

				p->change_incidental_caps(p);

				p->send_string_silent(p, p->buffer->cur_pos);

				// Revert fields back
				p->buffer->unset_offset(p->buffer, offset);
			}
			else if (p->correction_action == CORRECTION_INCIDENTAL_CAPS)
			{
				p->buffer->set_content(p->buffer, p->correction_buffer->get_last_word(p->correction_buffer, p->correction_buffer->content));
				p->buffer->set_lang_mask(p->buffer, get_curr_keyboard_group ());
				
				int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

				// Shift fields to point to begin of word
				p->buffer->set_offset(p->buffer, offset);

				p->unchange_incidental_caps(p);

				p->send_string_silent(p, p->buffer->cur_pos);

				// Revert fields back
				p->buffer->unset_offset(p->buffer, offset);

				p->correction_buffer->clear(p->correction_buffer);
				p->correction_action = CORRECTION_NONE;
			}
			
			break;
		}
		case CHANGE_TWO_CAPITAL_LETTER:
		{
			if (p->correction_action == CORRECTION_NONE) 
			{
				int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

				// Shift fields to point to begin of word
				p->buffer->set_offset(p->buffer, offset);

				p->change_two_capital_letter(p);

				p->send_string_silent(p, p->buffer->cur_pos);

				// Revert fields back
				p->buffer->unset_offset(p->buffer, offset);
			}
			else if (p->correction_action == CORRECTION_TWO_CAPITAL_LETTER)
			{
				p->buffer->set_content(p->buffer, p->correction_buffer->get_last_word(p->correction_buffer, p->correction_buffer->content));
				p->buffer->set_lang_mask(p->buffer, get_curr_keyboard_group ());
				
				int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

				// Shift fields to point to begin of word
				p->buffer->set_offset(p->buffer, offset);

				p->unchange_two_capital_letter(p);

				p->send_string_silent(p, p->buffer->cur_pos);

				// Revert fields back
				p->buffer->unset_offset(p->buffer, offset);

				p->correction_buffer->clear(p->correction_buffer);
				p->correction_action = CORRECTION_NONE;
			}
			break;
		}
		case CHANGE_TWO_SPACE:
		{
			if (p->correction_action == CORRECTION_NONE) 
			{
				p->event->send_backspaces(p->event, 1);
				p->buffer->del_symbol(p->buffer);

				KeyCode kc = 0;
				int modifier = 0;
				size_t sym_size = strlen(",");
				int lang = get_curr_keyboard_group ();
				p->buffer->keymap->get_ascii(p->buffer->keymap, ",", &lang, &kc, &modifier, &sym_size);
				p->event->send_xkey(p->event, kc, modifier);
				p->buffer->add_symbol(p->buffer, ',', kc, modifier);
			}
			else if (p->correction_action == CORRECTION_TWO_SPACE) 
			{
				p->event->send_backspaces(p->event, 2);
				p->buffer->del_symbol(p->buffer);
				p->buffer->del_symbol(p->buffer);

				p->event->send_spaces (p->event, 2);
				KeyCode kc = 0;
				int modifier = 0;
				size_t sym_size = strlen(",");
				int lang = get_curr_keyboard_group ();
				p->buffer->keymap->get_ascii(p->buffer->keymap, " ", &lang, &kc, &modifier, &sym_size);
				p->buffer->add_symbol(p->buffer, ' ', kc, modifier);
				p->buffer->add_symbol(p->buffer, ' ', kc, modifier);
				
				p->correction_buffer->clear(p->correction_buffer);
				p->correction_action = CORRECTION_NONE;
			}
			break;
		}
		case CHANGE_TWO_MINUS:
		{
			if (p->correction_action == CORRECTION_NONE) 
			{
				p->event->send_backspaces(p->event, 2);
	
				int key_code = main_window->keymap->max_keycode;
				KeySym keysyms_bckp[main_window->keymap->keysyms_per_keycode];
				KeySym *keymap = main_window->keymap->keymap + (key_code - main_window->keymap->min_keycode) * main_window->keymap->keysyms_per_keycode;
				for (int i = 0; i < main_window->keymap->keysyms_per_keycode; i++)
				{
					keysyms_bckp[i]= keymap[i];
				}

				KeySym keysyms[main_window->keymap->keysyms_per_keycode];
				
				for (int i = 0; i < main_window->keymap->keysyms_per_keycode; i++)
				{
					keysyms[i]= XK_endash;//XK_trademark;//XK_copyright;//
					if (xconfig->correct_dash_with_emdash) 
						keysyms[i]= XK_emdash;
				}
				XChangeKeyboardMapping(main_window->display, key_code, 
								        main_window->keymap->keysyms_per_keycode, keysyms, 1);
				XFlush(main_window->display);
				XSync(main_window->display, TRUE);
				p->event->send_xkey(p->event, key_code, 0);
				usleep(100000);
	
				XChangeKeyboardMapping(main_window->display, key_code, 
			   		                    main_window->keymap->keysyms_per_keycode, keysyms_bckp, 1);
				XFlush(main_window->display);
				XSync(main_window->display, TRUE);
	
				p->buffer->clear(p->buffer);
			}
			else if (p->correction_action == CORRECTION_TWO_MINUS) 
			{
				p->event->send_backspaces(p->event, 2);
				
				p->buffer->set_content(p->buffer, p->correction_buffer->get_last_word(p->correction_buffer, p->correction_buffer->content));
				p->buffer->set_lang_mask(p->buffer, get_curr_keyboard_group ());
				
				KeyCode kc = 0;
				int modifier = 0;
				size_t sym_size = strlen("-");
				int lang = get_curr_keyboard_group ();
				p->buffer->keymap->get_ascii(p->buffer->keymap, "-", &lang, &kc, &modifier, &sym_size);
				p->event->send_xkey(p->event, kc, modifier);
				p->event->send_xkey(p->event, kc, modifier);
				
				p->event->send_spaces(p->event, 1);

				kc = 0;
				modifier = 0;
				sym_size = strlen(" ");
				p->buffer->keymap->get_ascii(p->buffer->keymap, " ", &lang, &kc, &modifier, &sym_size);
				
				p->correction_buffer->clear(p->correction_buffer);
				p->correction_action = CORRECTION_NONE;
			}
			break;
		}
		case CHANGE_COPYRIGHT:
		{
			if (p->correction_action == CORRECTION_NONE) 
			{
				p->event->send_backspaces(p->event, 2);
	
				int key_code = main_window->keymap->max_keycode;
				KeySym keysyms_bckp[main_window->keymap->keysyms_per_keycode];
				KeySym *keymap = main_window->keymap->keymap + (key_code - main_window->keymap->min_keycode) * main_window->keymap->keysyms_per_keycode;
				for (int i = 0; i < main_window->keymap->keysyms_per_keycode; i++)
				{
					keysyms_bckp[i]= keymap[i];
				}

				KeySym keysyms[main_window->keymap->keysyms_per_keycode];
				
				for (int i = 0; i < main_window->keymap->keysyms_per_keycode; i++)
				{
					keysyms[i]= XK_copyright;//
				}
				XChangeKeyboardMapping(main_window->display, key_code, 
								        main_window->keymap->keysyms_per_keycode, keysyms, 1);
				XFlush(main_window->display);
				XSync(main_window->display, TRUE);
				p->event->send_xkey(p->event, key_code, 0);
				usleep(100000);
	
				XChangeKeyboardMapping(main_window->display, key_code, 
			   		                    main_window->keymap->keysyms_per_keycode, keysyms_bckp, 1);
				XFlush(main_window->display);
				XSync(main_window->display, TRUE);
	
				p->buffer->clear(p->buffer);
				p->event->default_event.xkey.keycode = 0;
			}
			else if (p->correction_action == CORRECTION_COPYRIGHT) 
			{
				p->event->send_spaces(p->event, 2);
				
				p->buffer->set_content(p->buffer, p->correction_buffer->get_last_word(p->correction_buffer, p->correction_buffer->content));
				p->buffer->set_lang_mask(p->buffer, get_curr_keyboard_group ());

				int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

				// Shift fields to point to begin of word
				p->buffer->set_offset(p->buffer, offset);

				p->send_string_silent(p, p->buffer->cur_pos);

				// Revert fields back
				p->buffer->unset_offset(p->buffer, offset);
				
				p->correction_buffer->clear(p->correction_buffer);
				p->correction_action = CORRECTION_NONE;
			}
			break;
		}	
		case CHANGE_TRADEMARK:
		{
			if (p->correction_action == CORRECTION_NONE) 
			{
				p->event->send_backspaces(p->event, 3);
	
				int key_code = main_window->keymap->max_keycode;
				KeySym keysyms_bckp[main_window->keymap->keysyms_per_keycode];
				KeySym *keymap = main_window->keymap->keymap + (key_code - main_window->keymap->min_keycode) * main_window->keymap->keysyms_per_keycode;
				for (int i = 0; i < main_window->keymap->keysyms_per_keycode; i++)
				{
					keysyms_bckp[i]= keymap[i];
				}

				KeySym keysyms[main_window->keymap->keysyms_per_keycode];
				
				for (int i = 0; i < main_window->keymap->keysyms_per_keycode; i++)
				{
					keysyms[i]= XK_trademark;//
				}
				XChangeKeyboardMapping(main_window->display, key_code, 
								        main_window->keymap->keysyms_per_keycode, keysyms, 1);
				XFlush(main_window->display);
				XSync(main_window->display, TRUE);
				p->event->send_xkey(p->event, key_code, 0);
				usleep(100000);
	
				XChangeKeyboardMapping(main_window->display, key_code, 
			   		                    main_window->keymap->keysyms_per_keycode, keysyms_bckp, 1);
				XFlush(main_window->display);
				XSync(main_window->display, TRUE);
	
				p->buffer->clear(p->buffer);
				p->event->default_event.xkey.keycode = 0;
			}
			else if (p->correction_action == CORRECTION_TRADEMARK) 
			{
				p->event->send_spaces(p->event, 3);
				
				p->buffer->set_content(p->buffer, p->correction_buffer->get_last_word(p->correction_buffer, p->correction_buffer->content));
				p->buffer->set_lang_mask(p->buffer, get_curr_keyboard_group ());

				int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

				// Shift fields to point to begin of word
				p->buffer->set_offset(p->buffer, offset);

				p->send_string_silent(p, p->buffer->cur_pos);

				// Revert fields back
				p->buffer->unset_offset(p->buffer, offset);
				
				p->correction_buffer->clear(p->correction_buffer);
				p->correction_action = CORRECTION_NONE;
			}
			break;
		}	
		case CHANGE_REGISTERED:
		{
			if (p->correction_action == CORRECTION_NONE) 
			{
				p->event->send_backspaces(p->event, 2);
	
				int key_code = main_window->keymap->max_keycode;
				KeySym keysyms_bckp[main_window->keymap->keysyms_per_keycode];
				KeySym *keymap = main_window->keymap->keymap + (key_code - main_window->keymap->min_keycode) * main_window->keymap->keysyms_per_keycode;
				for (int i = 0; i < main_window->keymap->keysyms_per_keycode; i++)
				{
					keysyms_bckp[i]= keymap[i];
				}

				KeySym keysyms[main_window->keymap->keysyms_per_keycode];
				
				for (int i = 0; i < main_window->keymap->keysyms_per_keycode; i++)
				{
					keysyms[i]= XK_registered;//
				}
				XChangeKeyboardMapping(main_window->display, key_code, 
								        main_window->keymap->keysyms_per_keycode, keysyms, 1);
				XFlush(main_window->display);
				XSync(main_window->display, TRUE);
				p->event->send_xkey(p->event, key_code, 0);
				usleep(100000);
	
				XChangeKeyboardMapping(main_window->display, key_code, 
			   		                    main_window->keymap->keysyms_per_keycode, keysyms_bckp, 1);
				XFlush(main_window->display);
				XSync(main_window->display, TRUE);
	
				p->buffer->clear(p->buffer);
				p->event->default_event.xkey.keycode = 0;
			}
			else if (p->correction_action == CORRECTION_REGISTERED) 
			{
				p->event->send_spaces(p->event, 2);

				p->buffer->set_content(p->buffer, p->correction_buffer->get_last_word(p->correction_buffer, p->correction_buffer->content));
				p->buffer->set_lang_mask(p->buffer, get_curr_keyboard_group ());

				int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

				// Shift fields to point to begin of word
				p->buffer->set_offset(p->buffer, offset);

				p->send_string_silent(p, p->buffer->cur_pos);

				// Revert fields back
				p->buffer->unset_offset(p->buffer, offset);
				
				p->correction_buffer->clear(p->correction_buffer);
				p->correction_action = CORRECTION_NONE;
			}
			break;
		}
		case CHANGE_ELLIPSIS:
		{
			if (p->correction_action == CORRECTION_NONE) 
			{
				p->event->send_backspaces(p->event, 2);
	
				int key_code = main_window->keymap->max_keycode;
				KeySym keysyms_bckp[main_window->keymap->keysyms_per_keycode];
				KeySym *keymap = main_window->keymap->keymap + (key_code - main_window->keymap->min_keycode) * main_window->keymap->keysyms_per_keycode;
				for (int i = 0; i < main_window->keymap->keysyms_per_keycode; i++)
				{
					keysyms_bckp[i]= keymap[i];
				}

				KeySym keysyms[main_window->keymap->keysyms_per_keycode];
				
				for (int i = 0; i < main_window->keymap->keysyms_per_keycode; i++)
				{
					keysyms[i]= XK_ellipsis;//
				}
				XChangeKeyboardMapping(main_window->display, key_code, 
								        main_window->keymap->keysyms_per_keycode, keysyms, 1);
				XFlush(main_window->display);
				XSync(main_window->display, TRUE);
				p->event->send_xkey(p->event, key_code, 0);
				usleep(100000);
	
				XChangeKeyboardMapping(main_window->display, key_code, 
			   		                    main_window->keymap->keysyms_per_keycode, keysyms_bckp, 1);
				XFlush(main_window->display);
				XSync(main_window->display, TRUE);
	
				p->buffer->clear(p->buffer);
				p->correction_buffer->clear(p->buffer);
				p->event->default_event.xkey.keycode = 0;
			}
			else if (p->correction_action == CORRECTION_ELLIPSIS) 
			{
				p->event->send_spaces(p->event, 2);
				
				p->buffer->set_content(p->buffer, p->correction_buffer->get_last_word(p->correction_buffer, p->correction_buffer->content));
				p->buffer->set_lang_mask(p->buffer, get_curr_keyboard_group ());

				int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

				// Shift fields to point to begin of word
				p->buffer->set_offset(p->buffer, offset);

				p->send_string_silent(p, p->buffer->cur_pos);

				// Revert fields back
				p->buffer->unset_offset(p->buffer, offset);
				
				p->correction_buffer->clear(p->correction_buffer);
				p->correction_action = CORRECTION_NONE;
			}
			break;
		}
		case CHANGE_WORD_TO_LAYOUT_0:
		{
			int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

			// Shift fields to point to begin of word
			p->buffer->set_offset(p->buffer, offset);

			p->change_lang(p, 0);

			int len = p->buffer->cur_pos;
			if (p->last_action == ACTION_AUTOCOMPLETION)
				len = p->buffer->cur_pos + 1;
			p->send_string_silent(p, len);

			p->last_action = ACTION_NONE;
			
			// Revert fields back
			p->buffer->unset_offset(p->buffer, offset);
			break;
		}
		case CHANGE_WORD_TO_LAYOUT_1:
		{
			int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

			// Shift fields to point to begin of word
			p->buffer->set_offset(p->buffer, offset);

			p->change_lang(p, 1);

			int len = p->buffer->cur_pos;
			if (p->last_action == ACTION_AUTOCOMPLETION)
				len = p->buffer->cur_pos + 1;
			p->send_string_silent(p, len);

			p->last_action = ACTION_NONE;
			
			// Revert fields back
			p->buffer->unset_offset(p->buffer, offset);
			break;
		}
		case CHANGE_WORD_TO_LAYOUT_2:
		{
			int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

			// Shift fields to point to begin of word
			p->buffer->set_offset(p->buffer, offset);

			p->change_lang(p, 2);

			int len = p->buffer->cur_pos;
			if (p->last_action == ACTION_AUTOCOMPLETION)
				len = p->buffer->cur_pos + 1;
			p->send_string_silent(p, len);

			p->last_action = ACTION_NONE;
			
			// Revert fields back
			p->buffer->unset_offset(p->buffer, offset);
			break;
		}
		case CHANGE_WORD_TO_LAYOUT_3:
		{
			int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

			// Shift fields to point to begin of word
			p->buffer->set_offset(p->buffer, offset);

			p->change_lang(p, 3);

			int len = p->buffer->cur_pos;
			if (p->last_action == ACTION_AUTOCOMPLETION)
				len = p->buffer->cur_pos + 1;
			p->send_string_silent(p, len);

			p->last_action = ACTION_NONE;
			
			// Revert fields back
			p->buffer->unset_offset(p->buffer, offset);
			break;
		}
		case CHANGE_WORD_TRANSLIT:
		{
			int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);
			p->buffer->set_offset(p->buffer, offset);
			int curr_lang = get_curr_keyboard_group();
			char *text = strdup(p->buffer->get_last_word(p->buffer, p->buffer->i18n_content[curr_lang].content));
			p->buffer->unset_offset(p->buffer, offset);

			convert_text_to_translit(&text);
			p->buffer->set_content(p->buffer, text);

			if (text != NULL)
				free(text);

			int len = p->buffer->cur_pos;
			if (p->last_action == ACTION_AUTOCOMPLETION)
				len = p->buffer->cur_pos + 1;
			p->send_string_silent(p, len);

			p->last_action = ACTION_NONE;

			show_notify(NOTIFY_MANUAL_TRANSLIT_WORD, NULL);
			break;
		}
		case CHANGE_WORD_CHANGECASE:
		{
			int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

			// Shift fields to point to begin of word
			p->buffer->set_offset(p->buffer, offset);

			p->buffer->change_case(p->buffer);

			int len = p->buffer->cur_pos;
			if (p->last_action == ACTION_AUTOCOMPLETION) 
				len = p->buffer->cur_pos + 1;
			p->send_string_silent(p, len);

			p->last_action = ACTION_NONE;     
			
			// Revert fields back
			p->buffer->unset_offset(p->buffer, offset);

			show_notify(NOTIFY_MANUAL_CHANGECASE_WORD, NULL);
			break;
		}
		case CHANGE_WORD_PREVIEW_CHANGE:
		{
			int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

			// Shift fields to point to begin of word
			p->buffer->set_offset(p->buffer, offset);
			
			p->buffer->rotate_layout(p->buffer);

			char *string = p->buffer->get_utf_string(p->buffer);
			show_notify(NOTIFY_MANUAL_PREVIEW_CHANGE_WORD, string);
			if (string != NULL)
				free(string);
			p->buffer->unset_offset(p->buffer, offset);
			
			break;
		}
		case CHANGE_SYLL_TO_LAYOUT_0:
		{
			int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

			// Shift fields to point to begin of word
			p->buffer->set_offset(p->buffer, offset);

			p->change_lang(p, 0);

			int len = p->buffer->cur_pos - 1;
			if (p->last_action == ACTION_AUTOCOMPLETION)
				len = p->buffer->cur_pos;
			p->send_string_silent(p, len);

			// Revert fields back
			p->buffer->unset_offset(p->buffer, offset);
			break;
		}
		case CHANGE_SYLL_TO_LAYOUT_1:
		{
			int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

			// Shift fields to point to begin of word
			p->buffer->set_offset(p->buffer, offset);

			p->change_lang(p, 1);

			int len = p->buffer->cur_pos - 1;
			if (p->last_action == ACTION_AUTOCOMPLETION)
				len = p->buffer->cur_pos;
			p->send_string_silent(p, len);

			// Revert fields back
			p->buffer->unset_offset(p->buffer, offset);
			break;
		}
		case CHANGE_SYLL_TO_LAYOUT_2:
		{
			int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

			// Shift fields to point to begin of word
			p->buffer->set_offset(p->buffer, offset);

			p->change_lang(p, 2);

			int len = p->buffer->cur_pos - 1;
			if (p->last_action == ACTION_AUTOCOMPLETION)
				len = p->buffer->cur_pos;
			p->send_string_silent(p, len);

			// Revert fields back
			p->buffer->unset_offset(p->buffer, offset);
			break;
		}
		case CHANGE_SYLL_TO_LAYOUT_3:
		{
			int offset = p->buffer->get_last_word_offset(p->buffer, p->buffer->content, p->buffer->cur_pos);

			// Shift fields to point to begin of word
			p->buffer->set_offset(p->buffer, offset);

			p->change_lang(p, 3);

			int len = p->buffer->cur_pos - 1;
			if (p->last_action == ACTION_AUTOCOMPLETION)
				len = p->buffer->cur_pos;
			p->send_string_silent(p, len);

			// Revert fields back
			p->buffer->unset_offset(p->buffer, offset);
			break;
		}
		case CHANGE_SELECTION:
		{
			p->send_string_silent(p, 0);
			break;
		}
		case CHANGE_STRING_TO_LAYOUT_0:
		{
			p->change_lang(p, 0);
			p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);	// Disable receiving events

			p->send_string_silent(p, p->buffer->cur_pos);
			break;
		}
		case CHANGE_STRING_TO_LAYOUT_1:
		{
			p->change_lang(p, 1);
			p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);	// Disable receiving events

			p->send_string_silent(p, p->buffer->cur_pos);
			break;
		}
		case CHANGE_STRING_TO_LAYOUT_2:
		{
			p->change_lang(p, 2);
			p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);	// Disable receiving events

			p->send_string_silent(p, p->buffer->cur_pos);
			break;
		}
		case CHANGE_STRING_TO_LAYOUT_3:
		{
			p->change_lang(p, 3);
			p->focus->update_events(p->focus, LISTEN_DONTGRAB_INPUT);	// Disable receiving events

			p->send_string_silent(p, p->buffer->cur_pos);
			break;
		}
		case CHANGE_ABBREVIATION:
		case CHANGE_INS_DATE:
		{
			p->send_string_silent(p, 0);
			break;
		}
		case CHANGE_MISPRINT:
		{
			if (p->correction_action == CORRECTION_MISPRINT) 
			{
				int backspaces_count = p->buffer->cur_pos - p->correction_buffer->get_last_word_offset(p->correction_buffer, p->correction_buffer->content, p->correction_buffer->cur_pos);
				int offset = p->buffer->cur_pos - p->correction_buffer->cur_pos;
				
				p->buffer->set_content(p->buffer, p->correction_buffer->content);
				p->buffer->set_lang_mask(p->buffer, get_curr_keyboard_group ());

				int cur_pos = p->buffer->cur_pos - backspaces_count + offset;
				
				p->buffer->set_offset(p->buffer, cur_pos);
				p->send_string_silent(p, backspaces_count);
				p->buffer->unset_offset(p->buffer, cur_pos);

				p->correction_buffer->clear(p->correction_buffer);

				if (xconfig->educate)
					p->add_word_to_dict(p, get_curr_keyboard_group());
				
				p->correction_action = CORRECTION_NONE;
			}
			break;
		}
	}

	p->plugin->change_action(p->plugin, action);
	p->correction_action = CORRECTION_NONE;
}

static void program_add_word_to_dict(struct _program *p, int new_lang)
{
	char *tmp = p->buffer->get_last_word(p->buffer, p->buffer->content);
	if (tmp == NULL)
		return;

	// Remove word from all temp dictionary (excl. new language)
	for (int lang = 0; lang < xconfig->handle->total_languages; lang++)
	{
		if (lang != new_lang)
		{
			tmp = p->buffer->get_last_word(p->buffer, p->buffer->i18n_content[lang].content);

			char *curr_word = strdup(tmp);
	
			// Del final spaces
			int len = trim_word(curr_word, strlen(tmp));
			if (len == 0)
			{
				if (curr_word != NULL)
					free(curr_word);
				continue;
			}

			del_final_numeric_char(curr_word);
			if (strlen(curr_word) == 0)
			{
				if (curr_word != NULL)
					free(curr_word);
				continue;
			}

			unsigned int offset = 0;
			for (offset = 0; offset < strlen(curr_word); offset++)
			{
				if (!ispunct(curr_word[offset]) && (!isdigit(curr_word[offset])))
					break;
			}
			
			struct _list_char *curr_temp_dictionary = xconfig->handle->languages[lang].temp_dictionary;
			if (curr_temp_dictionary->exist(curr_temp_dictionary, curr_word+offset, BY_REGEXP))
			{
				char *word_to_dict = malloc((strlen(curr_word+offset) + 7) * sizeof(char));
				sprintf(word_to_dict, "%s%s%s", "(?i)^", curr_word+offset, "$");
				curr_temp_dictionary->rem(curr_temp_dictionary, word_to_dict);
				if (word_to_dict != NULL)
					free(word_to_dict);
			}
			if (curr_word != NULL)
				free(curr_word);
		}
	}
	
	struct _list_char *new_temp_dictionary = xconfig->handle->languages[new_lang].temp_dictionary;

	tmp = p->buffer->get_last_word(p->buffer, p->buffer->i18n_content[new_lang].content);

	char *new_word = strdup(tmp);

	int new_len = trim_word(new_word, strlen(tmp));
	if (new_len == 0)
	{
		if (new_word != NULL)
			free(new_word);
		return;
	}

	del_final_numeric_char(new_word);
	if (strlen(new_word) == 0)
	{
		if (new_word != NULL)
			free(new_word);
		return;
	}

	unsigned int offset = 0;
	for (offset = 0; offset < strlen(new_word); offset++)
	{
		if (!ispunct(new_word[offset]) && (!isdigit(new_word[offset])))
			break;
	}

	if (!new_temp_dictionary->exist(new_temp_dictionary, new_word+offset, BY_REGEXP))
	{
		char *word_to_dict = malloc((strlen(new_word+offset) + 7) * sizeof(char));
		sprintf(word_to_dict, "%s%s%s", "(?i)^", new_word+offset, "$");
		if (strcmp(word_to_dict, "(?i)^.$") != 0)
		{
			new_temp_dictionary->add(new_temp_dictionary, word_to_dict);
		}
		if (word_to_dict != NULL)
			free(word_to_dict);
		if (new_word != NULL)
			free(new_word);
		return;
	}

	for (int lang = 0; lang < xconfig->handle->total_languages; lang++)
	{
		if (lang != new_lang)
		{
			tmp = p->buffer->get_last_word(p->buffer, p->buffer->i18n_content[lang].content);

			char *curr_word = strdup(tmp);
	
			// Del final spaces
			int len = trim_word(curr_word, strlen(tmp));
			if (len == 0)
			{
				if (curr_word != NULL)
					free(curr_word);
				continue;
			}

			del_final_numeric_char(curr_word);
			if (strlen(curr_word) == 0)
			{
				if (curr_word != NULL)
					free(curr_word);
				continue;
			}

			unsigned int offset = 0;
			for (offset = 0; offset < strlen(curr_word); offset++)
			{
				if (!ispunct(curr_word[offset]) && (!isdigit(curr_word[offset])))
					break;
			}
			
			struct _list_char *curr_dictionary = xconfig->handle->languages[lang].dictionary;
			if (curr_dictionary->exist(curr_dictionary, curr_word+offset, BY_REGEXP))
			{
				log_message(DEBUG, _("Remove word '%s' from %s dictionary"), curr_word+offset, xconfig->handle->languages[lang].name);
				char *word_to_dict = malloc((strlen(curr_word+offset) + 7) * sizeof(char));
				sprintf(word_to_dict, "%s%s%s", "(?i)^", curr_word+offset, "$");
				curr_dictionary->rem(curr_dictionary, word_to_dict);
				xconfig->save_dict(xconfig, lang);
				if (word_to_dict != NULL)
					free(word_to_dict);
			}
			if (curr_word != NULL)
				free(curr_word);
		}
	}
	
	struct _list_char *new_dictionary = xconfig->handle->languages[new_lang].dictionary;
	if (!new_dictionary->exist(new_dictionary, new_word+offset, BY_REGEXP))
	{		
		char *word_to_dict = malloc((strlen(new_word+offset) + 7) * sizeof(char));
		sprintf(word_to_dict, "%s%s%s", "(?i)^", new_word+offset, "$");
		if (strcmp(word_to_dict, "(?i)^.$") != 0)
		{
			log_message(DEBUG, _("Add word '%s' in %s dictionary"), new_word+offset, xconfig->handle->languages[new_lang].name);
			new_dictionary->add(new_dictionary, word_to_dict);
		}
		xconfig->save_dict(xconfig, new_lang);
		if (word_to_dict != NULL)
			free(word_to_dict);
	}

	p->add_word_to_pattern(p, new_lang);

	if (new_word != NULL)
		free(new_word);
}

static void program_add_word_to_pattern(struct _program *p, int new_lang)
{
	// Enable saving pattern always
	//if (!xconfig->autocompletion)
	//	return;

	char *tmp = p->buffer->get_last_word(p->buffer, p->buffer->content);
	if (tmp == NULL)
		return;

	if (strlen(tmp) < MIN_PATTERN_LEN)
		return;
	
	tmp = p->buffer->get_last_word(p->buffer, p->buffer->i18n_content[new_lang].content);

	char *new_word = strdup(tmp);

	int len = trim_word(new_word, strlen(tmp));
	if (len == 0)
	{
		if (new_word != NULL)
			free(new_word);
		return;
	}

	del_final_numeric_char(new_word);
	if (strlen(new_word) == 0)
	{
		if (new_word != NULL)
			free(new_word);
		return;
	}

	unsigned int offset = 0;
	for (offset = 0; offset < strlen(new_word); offset++)
	{
		if (!ispunct(new_word[offset]) && (!isdigit(new_word[offset])))
			break;
	}
	
	for (int i = 0; i < xconfig->handle->total_languages; i++)
	{
		if (i == new_lang)
			continue;
		
		tmp = p->buffer->get_last_word(p->buffer, p->buffer->i18n_content[i].content);
		char *old_word = strdup(tmp);

		len = trim_word(old_word, strlen(tmp));
		if (len == 0)
		{
			if (old_word != NULL)
				free (old_word);
			continue;
		}
		unsigned int offset = 0;
		for (offset = 0; offset < strlen(old_word); offset++)
		{
			if (!ispunct(old_word[offset]) && (!isdigit(old_word[offset])))
				break;
		}
		
		struct _list_char *old_pattern = xconfig->handle->languages[i].pattern;
		if (old_pattern->exist(old_pattern, old_word+offset, BY_PLAIN))
		{
			log_message(DEBUG, _("Remove word '%s' from %s pattern"), old_word+offset, xconfig->handle->languages[i].name);
			old_pattern->rem(old_pattern, old_word+offset);
			xconfig->save_pattern(xconfig, i);
		}
		if (old_word != NULL)
			free (old_word);
	}

#ifdef WITH_ASPELL
	if (xconfig->handle->has_spell_checker[new_lang])
	{
		if (!aspell_speller_check(xconfig->handle->spell_checkers[new_lang], new_word+offset, strlen(new_word)))
		{
			if (new_word != NULL)
				free(new_word);
			return;
		}
	}
#endif

#ifdef WITH_ENCHANT
	if (xconfig->handle->has_enchant_checker[new_lang])
	{
		if (strlen(new_word+offset) <= 0)
		{
			if (new_word != NULL)
				free(new_word);
			return;
		}
		
		if (enchant_dict_check(xconfig->handle->enchant_dicts[new_lang], new_word+offset, strlen(new_word+offset)))
		{
			if (new_word != NULL)
				free(new_word);
			return;
		}
	}
#endif
	
	struct _list_char *new_pattern = xconfig->handle->languages[new_lang].pattern;
	if (!new_pattern->exist(new_pattern, new_word+offset, BY_PLAIN))
	{
		log_message(DEBUG, _("Add word '%s' in %s pattern"), new_word+offset, xconfig->handle->languages[new_lang].name);
		new_pattern->add(new_pattern, new_word+offset);
		xconfig->save_pattern(xconfig, new_lang);
	}

	if (new_word != NULL)
		free(new_word);
}

static void program_uninit(struct _program *p)
{
	p->focus->uninit(p->focus);
	p->event->uninit(p->event);
	p->buffer->uninit(p->buffer);
	p->correction_buffer->uninit(p->correction_buffer);
	p->plugin->uninit(p->plugin);

	main_window->uninit(main_window);

	if (p != NULL)
		free(p);

	log_message(DEBUG, _("Program is freed"));
}

struct _program* program_init(void)
{
	struct _program *p = (struct _program*) malloc(sizeof(struct _program));
	bzero(p, sizeof(struct _program));

	main_window = window_init(xconfig->handle);

	if (!main_window->create(main_window) || !main_window->init_keymap(main_window))
	{
		if (p != NULL)
			free(p);
		return NULL;
	}
	
	p->event			= event_init();			// X Event processor
	p->focus			= focus_init();			// X Input Focus and Pointer processor
	p->buffer			= buffer_init(xconfig->handle, main_window->keymap);	// Input string buffer
	
	p->plugin			= plugin_init();
	for (int i=0; i<xconfig->plugins->data_count; i++)
	{
		p->plugin->add(p->plugin, xconfig->plugins->data[i].string);
	}
	
	p->user_action = -1;
	p->manual_action = ACTION_NONE;

	p->correction_buffer = buffer_init(xconfig->handle, main_window->keymap);
	p->correction_action = CORRECTION_NONE;
	
	// Function mapping
	p->uninit			= program_uninit;
	p->layout_update		= program_layout_update;
	p->update			= program_update;
	p->on_key_action		= program_on_key_action;
	p->process_input		= program_process_input;
	p->perform_auto_action		= program_perform_auto_action;
	p->perform_manual_action	= program_perform_manual_action;
	p->perform_user_action		= program_perform_user_action;
	p->check_lang_last_word		= program_check_lang_last_word;
	p->check_lang_last_syllable	= program_check_lang_last_syllable;
	p->check_caps_last_word		= program_check_caps_last_word;
	p->check_tcl_last_word		= program_check_tcl_last_word;
	p->check_space_before_punctuation	= program_check_space_before_punctuation;
	p->check_space_with_bracket	= program_check_space_with_bracket;
	p->check_brackets_with_symbols = program_check_brackets_with_symbols;
	p->check_capital_letter_after_dot = program_check_capital_letter_after_dot;
	p->check_two_space = program_check_two_space;
	p->check_two_minus = program_check_two_minus;
	p->check_copyright = program_check_copyright;
	p->check_trademark = program_check_trademark;
	p->check_registered = program_check_registered;
	p->check_ellipsis = program_check_ellipsis;
	p->check_pattern	= program_check_pattern;
	p->rotate_pattern	= program_rotate_pattern;
	p->check_misprint	= program_check_misprint;
	p->change_word			= program_change_word;
	p->add_word_to_dict		= program_add_word_to_dict;
	p->add_word_to_pattern		= program_add_word_to_pattern;
	p->process_selection_notify	= program_process_selection_notify;
	p->change_lang			= program_change_lang;
	p->change_incidental_caps	= program_change_incidental_caps;
	p->unchange_incidental_caps	= program_unchange_incidental_caps;
	p->change_two_capital_letter	= program_change_two_capital_letter;
	p->unchange_two_capital_letter	= program_unchange_two_capital_letter;
	p->send_string_silent		= program_send_string_silent;

	return p;
}
