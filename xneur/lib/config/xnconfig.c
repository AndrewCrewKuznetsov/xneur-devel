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
#include <X11/keysym.h>
#include <X11/XKBlib.h>

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "xnconfig_files.h"

#include "types.h"
#include "list_char.h"
#include "text.h"
#include "log.h"

#include "xnconfig.h"

#define LIBRARY_VERSION_MAJOR		19
#define LIBRARY_VERSION_MINOR		0
#define OPTIONS_DELIMETER		" "

#define USR_CMD_START 	"<cmd>"
#define USR_CMD_END 	"</cmd>"

static const char *log_levels[] =	{"None", "Error", "Warning", "Log", "Debug", "Trace"};
static const char *bool_names[] =	{"No", "Yes"};
static const char *status_names[] =	{"Disable", "Enable"};
static const char *modifier_names[] =	{"Shift", "Control", "Alt", "Super"};

static const char *option_names[] = 	{
						"ManualMode", "ExcludeApp", "AddBind", "LogLevel", "ExcludeLanguage", "Autocompletion",
						"DisableCapsLock", "CheckOnProcess", "SetAutoApp", "SetManualApp", "AutocompletionExcludeApp",
						"EducationMode", "Version", "LayoutRememberMode", "SaveSelectionMode",
						"DefaultXkbGroup", "AddSound", "PlaySounds", "SendDelay", "LayoutRememberModeForApp",
						"LogSave", "ReplaceAbbreviation",
						"ReplaceAbbreviationIgnoreLayout", "CorrectIncidentalCaps", "CorrectTwoCapitalLetter",
						"FlushBufferWhenPressEnter", "TroubleshootEnter", "AddAction",
						"ShowOSD", "AddOSD", "FontOSD", "ShowPopup", "AddPopup", 
	                    "CorrectSpaceWithPunctuation", "AddSpaceAfterAutocompletion", "LoadModule",
						"LogSize", "LogMail", "LogHostIP", "SoundVolumePercent",
						"TroubleshootBackspace", "TroubleshootLeftArrow", "TroubleshootRightArrow",
						"TroubleshootUpArrow", "TroubleshootDownArrow", "TroubleshootDelete", 
						"TroubleshootSwitch", "TroubleshootFullScreen",
						"SetDontSendKeyReleaseApp", "LogPort", "RotateLayoutAfterChangeSelectedMode", "CorrectCapitalLetterAfterDot",
						"FlushBufferWhenPressEscape", "CompatibilityWithCompletion", "TrackingInput", "TrackingMouse",
						"PopupExpireTimeout", "CorrectTwoSpaceWithCommaAndSpace","CorrectTwoMinusWithDash",
						"CorrectCWithCopyright", "CorrectTMWithTrademark", "CorrectRWithRegistered",
						"CorrectDashWithEmDash","CorrectThreePointsWithEllipsis", "CorrectMisprint", "CheckSimilarWords",
						"TroubleshootTab", "DelaySendingEventApp", "Delimeter" 
					};
static const char *action_names[] =	{
						"ChangeWord", "TranslitWord", "ChangecaseWord", "PreviewChangeWord",
						"ChangeString", 
						"ChangeSelected", "TranslitSelected", "ChangecaseSelected", "PreviewChangeSelected",
						"ChangeClipboard", "TranslitClipboard", "ChangecaseClipboard", "PreviewChangeClipboard",
						"EnableLayout1", "EnableLayout2", "EnableLayout3", "EnableLayout4",
						"RotateLayout","RotateLayoutBack","ReplaceAbbreviation", "AutocompletionConfirmation",
						"RotateAutocompletion", "BlockKeyboardAndMouseEvents", "InsertDate"
					};
static const char *notify_names[] =	{
						"XneurStart", "XneurReload", "XneurStop",
						"PressKeyLayout1", "PressKeyLayout2", "PressKeyLayout3", "PressKeyLayout4",
						"EnableLayout1", "EnableLayout2", "EnableLayout3", "EnableLayout4",
						"AutomaticChangeWord", "ManualChangeWord", "ManualTranslitWord", "ManualChangecaseWord", "ManualPreviewChangeWord",
						"ChangeString",
						"ChangeSelected", "TranslitSelected", "ChangecaseSelected", "PreviewChangeSelected",
						"ChangeClipboard", "TranslitClipboard", "ChangecaseClipboard",  "PreviewChangeClipboard",
						"ReplaceAbbreviation", "CorrectIncidentalCaps", "CorrectTwoCapitalLetter", 
						"CorrectTwoSpaceWithCommaAndSpace", "CorrectTwoMinusWithDash", "CorrectCWithCopyright", 
						"CorrectTMWithTrademark", "CorrectRWithRegistered", "CorrectThreePointsWithEllipsis",
						"CorrectMisprint",
						"ExecuteUserAction","BlockKeyboardAndMouseEvents", "UnblockKeyboardAndMouseEvents"
					};

pid_t getsid(pid_t pid);

static char* get_word(char **string)
{
	return strsep(string, OPTIONS_DELIMETER);
}

#define get_option_index(options, option) \
	get_option_index_size(options, option, sizeof(options) / sizeof(options[0]));

static int get_option_index_size(const char *options[], char *option, int options_count)
{
	for (int i = 0; i < options_count; i++)
	{
		if (strcmp(option, options[i]) == 0)
			return i;
	}
	return -1;
}

static void xneur_config_get_library_version(int *major_version, int *minor_version)
{
	*major_version = LIBRARY_VERSION_MAJOR;
	*minor_version = LIBRARY_VERSION_MINOR;
}

static const char* xneur_config_get_bool_name(int option)
{
	return bool_names[option];
}

static int xneur_config_get_status(char *str)
{
	if (strcmp(str, status_names[0]) == 0)
		return FALSE;

	if (strcmp(str, status_names[1]) == 0)
		return TRUE;

	return -1;
}

static void parse_line(struct _xneur_config *p, char *line)
{
	if (line[0] == '#')
		return;

	char *option = get_word(&line);

	int index = get_option_index(option_names, option);
	if (index == -1)
	{
		log_message(WARNING, _("Unrecognized option \"%s\" detected"), option);
		return;
	}

	if (line == NULL)
	{
		log_message(WARNING, _("Param mismatch for option %s"), option);
		return;
	}
	
	char *full_string = strdup(line);
	char *param = get_word(&line);

	if (param == NULL)
	{
		if (full_string != NULL)
			free(full_string);
		log_message(WARNING, _("Param mismatch for option %s"), option);
		return;
	}

	switch (index)
	{
		case 0: // Get Default Mode (Auto/Manual)
		{
			int manual = get_option_index(bool_names, param);
			if (manual == -1)
			{
				log_message(WARNING, _("Invalid value for manual mode specified"));
				break;
			}

			p->manual_mode = manual;
			break;
		}
		case 1: // Get Applications Names
		{
			p->excluded_apps->add(p->excluded_apps, full_string);
			break;
		}
		case 2: // Get Keyboard Binds
		{
			int action = get_option_index(action_names, param);
			if (action == -1)
			{
				log_message(WARNING, _("Invalid value for action name specified"));
				break;
			}

			p->hotkeys[action].key = NULL;
			while (TRUE)
			{
				char *modifier = get_word(&line);
				if (modifier == NULL)
					break;

				if (modifier[0] == '\0')
					continue;

				int index = get_option_index(modifier_names, modifier);
				if (index == -1)
					p->hotkeys[action].key = strdup(modifier);
				else
					p->hotkeys[action].modifiers |= (1 << index);
			}

			break;
		}
		case 3: // Get Log Level
		{
			int index = get_option_index(log_levels, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for log level specified"));
				break;
			}

			p->log_level = index;
			log_set_level(p->log_level);
			break;
		}
		case 4: // Add Language
		{
			for (int lang = 0; lang < p->handle->total_languages; lang++)
			{
				if (strcmp(p->handle->languages[lang].dir, param) == 0)
					p->handle->languages[lang].excluded = TRUE;
			}

			break;
		}
		case 5: // Pattern Mining and Recognition
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for pattern minig and recognition mode specified"));
				break;
			}

			p->autocompletion = index;
			break;
		}
		case 6: // Disable CapsLock use
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for disable CapsLock using mode specified"));
				break;
			}

			p->disable_capslock = index;
			break;
		}
		case 7: // Check lang on input process
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for check language on input process mode specified"));
				break;
			}

			p->check_lang_on_process = index;
			break;
		}
		case 8: // Get Auto Processing Applications
		{
			p->auto_apps->add(p->auto_apps, full_string);
			break;
		}
		case 9: // Get Manual Processing Applications
		{
			p->manual_apps->add(p->manual_apps, full_string);
			break;
		}
		case 10: // Autocompletion Exclude App
		{
			p->autocompletion_excluded_apps->add(p->autocompletion_excluded_apps, full_string);
			break;
		}
		case 11: // Get Education Mode
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for education mode specified"));
				break;
			}

			p->educate = index;
			break;
		}
		case 12: // Get config version
		{
			p->version = strdup(param);
			break;
		}
		case 13: // Get Layout Remember Mode
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for remember layout mode specified"));
				break;
			}

			p->remember_layout = index;
			break;
		}
		case 14: // Get Save Selection Mode
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for save selection mode specified"));
				break;
			}

			p->save_selection_after_convert = index;
			break;
		}
		case 15: // Get Initial Xkb Group for all new windows
		{
			p->default_group = atoi(get_word(&param));
			break;
		}
		case 16: // Sounds
		{
			int sound = get_option_index(notify_names, param);
			if (sound == -1)
			{
				log_message(WARNING, _("Invalid value for sound action name specified"));
				break;
			}

			if (line == NULL)
				break;
			
			char *param1 = get_word(&line);
			char *param2 = get_word(&line);

			if (strlen(param1) != 0)
			{
				p->sounds[sound].enabled = xneur_config_get_status(param1);

				if (p->sounds[sound].enabled == -1)
				{
					p->sounds[sound].enabled = TRUE;
					p->sounds[sound].file = get_file_path_name(SOUNDDIR, param1);
				}
				else if (strlen(param2) != 0)
					p->sounds[sound].file = get_file_path_name(SOUNDDIR, param2);
			}
			if (p->sounds[sound].file == NULL)
				p->sounds[sound].enabled = FALSE;

			break;
		}
		case 17: // Play Sound
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for play sounds mode specified"));
				break;
			}

			p->play_sounds = index;
			break;
		}
		case 18: // Backevent Delay
		{
			p->send_delay = atoi(param);
			if (p->send_delay < 0 || p->send_delay > 50)
			{
				log_message(WARNING, _("Send delay must be between 0 and 50"));
				p->send_delay = 0;
			}
			break;
		}
		case 19: // layout remember for each application
		{
			p->layout_remember_apps->add(p->layout_remember_apps, full_string);
			break;
		}
		case 20: // Save Keyboard Log
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for save keyboard log mode specified"));
				break;
			}

			p->save_keyboard_log = index;
			break;
		}
		case 21: // Get Words for Replacing
		{
			char *replaced_string = escaped_sym_to_real_sym(full_string);
			if (replaced_string != NULL)
			{
				p->abbreviations->add(p->abbreviations, replaced_string);
				free(replaced_string);
			}
			break;
		}
		case 22: // Ignore keyboard layout for abbreviations
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for ignore keyboard layout for abbreviations mode specified"));
				break;
			}

			p->abbr_ignore_layout = index;
			break;
		}
		case 23: // Change iNCIDENTAL CapsLock Mode
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for change iNCIDENTAL CapsLock mode specified"));
				break;
			}

			p->correct_incidental_caps = index;
			break;
		}
		case 24: // Change two CApital letter Mode
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for change two CApital letter mode specified"));
				break;
			}

			p->correct_two_capital_letter = index;
			break;
		}
		case 25: // Flush internal buffer when pressed Enter Mode
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for flush internal buffer when pressed Enter mode specified"));
				break;
			}

			p->flush_buffer_when_press_enter = index;
			break;
		}
		case 26: // Don't process word when pressed Enter Mode
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for don't processing word when pressed Enter mode specified"));
				break;
			}

			p->troubleshoot_enter = index;
			break;
		}
		case 27: // User actions
		{
			void *tmp = realloc(p->actions, (p->actions_count + 1) * sizeof(struct _xneur_action));
			if (tmp == NULL)
				break;
			p->actions = (struct _xneur_action *)tmp;
			bzero(&p->actions[p->actions_count], sizeof(struct _xneur_action));

			while (TRUE)
			{
				if (param == NULL)
					break;

				if (param[0] == '\0')
					continue;

				int index = get_option_index(modifier_names, param);
				if (index == -1)
				{
					p->actions[p->actions_count].hotkey.key = strdup(param);
					if (line != NULL)
					{
						char *cmd = strstr(line, USR_CMD_START);
						if (cmd == NULL)
						{
							p->actions[p->actions_count].name = NULL;
							p->actions[p->actions_count].command = strdup(line);
							break;
						}
						int len = strlen(line) - strlen(cmd);
						p->actions[p->actions_count].name = strdup(line);
						p->actions[p->actions_count].name[len - 1] = NULLSYM;

						p->actions[p->actions_count].command = strdup(cmd + strlen(USR_CMD_START)*sizeof(char));
						cmd = strstr(cmd + strlen(USR_CMD_START)*sizeof(char), USR_CMD_END);
						if (cmd == NULL)
						{
							if (p->actions[p->actions_count].command != NULL)
								free(p->actions[p->actions_count].command);
							p->actions[p->actions_count].command = NULL;
							break;
						}
						len = strlen(p->actions[p->actions_count].command) - strlen(cmd);
						p->actions[p->actions_count].command[len] = NULLSYM;

						//log_message (ERROR, "\"%s\" \"%s\"", p->actions[p->actions_count].name, p->actions[p->actions_count].command);
						
					}
					break;
				}

				p->actions[p->actions_count].hotkey.modifiers |= (1 << index);

				param = get_word(&line);
			}

			p->actions_count++;
			break;
		}
		case 28: // Show OSD
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for show OSD mode specified"));
				break;
			}

			p->show_osd = index;
			break;
		}
		case 29: // OSDs
		{
			int osd = get_option_index(notify_names, param);
			if (osd == -1)
			{
				log_message(WARNING, _("Invalid value for OSD action name specified"));
				break;
			}

			if (line == NULL)
			{
				p->osds[osd].file = NULL;
				p->osds[osd].enabled = FALSE;
				break;
			}
			
			char *tmp = strdup(line);
			if (tmp == NULL) 
				break;
			char *param1 = get_word(&line);

			if (strlen(param1) != 0)
			{
				p->osds[osd].enabled = xneur_config_get_status(param1);

				if (p->osds[osd].enabled == -1)
				{
					p->osds[osd].enabled = TRUE;
					if (strlen(tmp) != 0)
						p->osds[osd].file = strdup(tmp);
				}
				else if (strlen(line) != 0)
					p->osds[osd].file = strdup(line);
			}
			if (p->osds[osd].file == NULL)
				p->osds[osd].enabled = FALSE;

			if (tmp != NULL)
				free(tmp);

			break;
		}
		case 30: // Get Initial Xkb Group for all new windows
		{
			p->osd_font = strdup(param);
			break;
		}
		case 31: // Show popup
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for show popup message mode specified"));
				break;
			}

			p->show_popup = index;
			break;
		}
		case 32: // Popups
		{
			int popup = get_option_index(notify_names, param);
			if (popup == -1)
			{
				log_message(WARNING, _("Invalid value for popup message action name specified"));
				break;
			}

			if (line == NULL)
			{
				p->popups[popup].file = NULL;
				p->popups[popup].enabled = FALSE;
				break;
			}
			
			char *tmp = strdup(line);
			if (tmp == NULL) 
				break;
			char *param1 = get_word(&line);

			if (strlen(param1) != 0)
			{
				p->popups[popup].enabled = xneur_config_get_status(param1);

				if (p->popups[popup].enabled == -1)
				{
					p->popups[popup].enabled = TRUE;
					if (strlen(tmp) != 0)
						p->popups[popup].file = strdup(tmp);
				}
				else if (strlen(line) != 0)
					p->popups[popup].file = strdup(line);
			}
			if (p->popups[popup].file == NULL)
				p->popups[popup].enabled = FALSE;

			if (tmp != NULL)
				free(tmp);
			
			break;
		}
		case 33:
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
				break;

			p->correct_space_with_punctuation = index;
			break;
		}
		case 34:
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
				break;

			p->add_space_after_autocompletion = index;
			break;
		}
		case 35: // Add plugin 
		{
			p->plugins->add(p->plugins, full_string);
			break;
		}
		case 36: // Log Size
		{
			if (param != NULL) 
			  p->size_keyboard_log = atoi (param);
			break;
		}
		case 37: // Log E-Mail 
		{
			p->mail_keyboard_log = strdup (param);
			break;
		}
		case 38: // Log host ip
		{
			p->host_keyboard_log = strdup (param);
			break;
		}
		case 39: // Get volume percentage
		{
			p->volume_percent = atoi(get_word(&param));
			if ((p->volume_percent < 0) || (p->volume_percent > 100))
				p->volume_percent = 30;
			break;
		}
		case 40: // Get troubleshooting mode for backspace
		{
			int status = get_option_index(bool_names, param);
			if (status == -1)
			{
				log_message(WARNING, _("Invalid value for troubleshooting mode for backspace specified"));
				break;
			}

			p->troubleshoot_backspace = status;
			break;
		}
		case 41: // Get troubleshooting mode for left arrow
		{
			int status = get_option_index(bool_names, param);
			if (status == -1)
			{
				log_message(WARNING, _("Invalid value for troubleshooting mode for left arrow specified"));
				break;
			}

			p->troubleshoot_left_arrow = status;
			break;
		}
		case 42: // Get troubleshooting mode for right arrow
		{
			int status = get_option_index(bool_names, param);
			if (status == -1)
			{
				log_message(WARNING, _("Invalid value for troubleshooting mode for right arrow specified"));
				break;
			}

			p->troubleshoot_right_arrow = status;
			break;
		}
		case 43: // Get troubleshooting mode for up arrow
		{
			int status = get_option_index(bool_names, param);
			if (status == -1)
			{
				log_message(WARNING, _("Invalid value for troubleshooting mode for up arrow specified"));
				break;
			}

			p->troubleshoot_up_arrow = status;
			break;
		}
		case 44: // Get troubleshooting mode for down arrow
		{
			int status = get_option_index(bool_names, param);
			if (status == -1)
			{
				log_message(WARNING, _("Invalid value for troubleshooting mode for down arrow specified"));
				break;
			}

			p->troubleshoot_down_arrow = status;
			break;
		}
		case 45: // Get troubleshooting mode for delete
		{
			int status = get_option_index(bool_names, param);
			if (status == -1)
			{
				log_message(WARNING, _("Invalid value for troubleshooting mode for delete specified"));
				break;
			}

			p->troubleshoot_delete = status;
			break;
		}
		case 46: // Get troubleshooting mode for keyboard switch
		{
			int status = get_option_index(bool_names, param);
			if (status == -1)
			{
				log_message(WARNING, _("Invalid value for troubleshooting mode for keyboard switch specified"));
				break;
			}

			p->troubleshoot_switch = status;
			break;
		}
		case 47: // Get troubleshooting mode for full screen apps
		{
			int status = get_option_index(bool_names, param);
			if (status == -1)
			{
				log_message(WARNING, _("Invalid value for troubleshooting mode for full screen apps specified"));
				break;
			}

			p->troubleshoot_full_screen = status;
			break;
		}
		case 48: // Get Applications That Event Will Not Be Sent KeyRelease
		{
			p->dont_send_key_release_apps->add(p->dont_send_key_release_apps, full_string);
			break;
		}
		case 49: // Port on host to send logfile
		{
			p->port_keyboard_log = atoi(get_word(&param));
			if ((p->port_keyboard_log < 1) || (p->port_keyboard_log > 32000))
				p->port_keyboard_log = 25;
			break;
		}
		case 50: // Rotate layout after convert selected text
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for rotate layout after correction selected text mode specified"));
				break;
			}

			p->rotate_layout_after_convert = index;
			break;
		}
		case 51: // Correct small letter to capital letter after dot
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for change small letter to capital letter after dot mode specified"));
				break;
			}

			p->correct_capital_letter_after_dot = index;
			break;
		}
		case 52: // Flush internal buffer when pressed Escape Mode
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for flush internal buffer when pressed Escape mode specified"));
				break;
			}

			p->flush_buffer_when_press_escape = index;
			break;
		}
		case 53: // Compatibility with the completion
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for compatibility with the completion mode specified"));
				break;
			}

			p->compatibility_with_completion = index;
			break;
		}
		case 54: // Tracking input
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for tracking input mode specified"));
				break;
			}

			p->tracking_input = index;
			break;
		}
		case 55: // Tracking mouse
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for tracking mouse mode specified"));
				break;
			}

			p->tracking_mouse = index;
			break;
		}
		case 56: // PopupExpireTimeout
		{
			p->popup_expire_timeout = atoi(param);
			if (p->popup_expire_timeout < 0 || p->popup_expire_timeout > 30000)
			{
				log_message(WARNING, _("Popup expire timeout must be between 0 and 30000"));
				p->popup_expire_timeout = 0;
			}
			break;
		}
		case 57: // CorrectTwoSpaceWithCommaAndSpace
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for correct two space with a comma and a space mode specified"));
				break;
			}

			p->correct_two_space_with_comma_and_space = index;
			break;
		}
		case 58: // CorrectTwoMinusWithDash
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for correct two minus with a dash mode specified"));
				break;
			}

			p->correct_two_minus_with_dash = index;
			break;
		}
		case 59: // CorrectCWithCopyright
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for correct (c) with a copyright sign mode specified"));
				break;
			}

			p->correct_c_with_copyright = index;
			break;
		}
		case 60: // CorrectTMWithTrademark
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for correct (tm) with a trademark sign mode specified"));
				break;
			}

			p->correct_tm_with_trademark = index;
			break;
		}
		case 61: // CorrectRWithRegistered
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for correct (r) with a registered sign mode specified"));
				break;
			}

			p->correct_r_with_registered = index;
			break;
		}
		case 62:
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
				break;

			p->correct_dash_with_emdash = index;
			break;
		}
		case 63:
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
				break;

			p->correct_three_points_with_ellipsis = index;
			break;
		}
		case 64:
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
				break;

			p->correct_misprint = index;
			break;
		}
		case 65:
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
				break;

			p->check_similar_words = index;
			break;
		}
		case 66: // Don't process word when pressed Tab Mode
		{
			int index = get_option_index(bool_names, param);
			if (index == -1)
			{
				log_message(WARNING, _("Invalid value for don't processing word when pressed Tab mode specified"));
				break;
			}

			p->troubleshoot_tab = index;
			break;
		}
		case 67: // Get Applications That Event Will Be Delayed Sent events
		{
			p->delay_send_key_apps->add(p->delay_send_key_apps, full_string);
			break;
		}
		case 68: // Get Delimeters
		{
			KeySym symbol = XStringToKeysym(full_string);
			if ((symbol != NoSymbol) &&
			    (symbol != XK_BackSpace) &&
			    (symbol != XK_Pause) &&
			    (symbol != XK_Escape) &&
			    (symbol != XK_Sys_Req) &&
			    (symbol != XK_Delete) &&
			    (symbol != XK_Return) &&
			    (symbol != XK_Tab)) 
			{
				int is_double = FALSE;
				for (int i = 0; i < p->delimeters_count; i++)
				{
					if (p->delimeters[i] == symbol)
						is_double = TRUE;
				}
				if (!is_double) 
				{
					void *tmp = realloc(p->delimeters, sizeof(KeySym) * (p->delimeters_count + 1));
					if (tmp == NULL) 
						break;
					p->delimeters = (KeySym *)tmp;
					tmp = realloc(p->delimeters_string, sizeof(char) * (p->delimeters_count + 2));
					if (tmp == NULL) 
						break;
					p->delimeters_string = (char *)tmp;

					p->delimeters[p->delimeters_count] = symbol;
					p->delimeters_count++;
				}
			}
			break;
		}
	}
	if (full_string != NULL)
		free(full_string);
}

static int parse_config_file(struct _xneur_config *p, const char *dir_name, const char *file_name)
{
	struct _list_char *list = load_list(dir_name, file_name, FALSE);
	if (list == NULL)
	{
		log_message(ERROR, _("Can't find config file %s"), file_name);
		return FALSE;
	}

	for (int i = 0; i < list->data_count; i++)
	{	
		parse_line(p, list->data[i].string);
	}
	
	list->uninit(list);
	return TRUE;
}

static void free_structures(struct _xneur_config *p)
{
	p->window_layouts->uninit(p->window_layouts);
	p->manual_apps->uninit(p->manual_apps);
	p->auto_apps->uninit(p->auto_apps);
	p->layout_remember_apps->uninit(p->layout_remember_apps);
	p->excluded_apps->uninit(p->excluded_apps);
	p->autocompletion_excluded_apps->uninit(p->autocompletion_excluded_apps);
	p->dont_send_key_release_apps->uninit(p->dont_send_key_release_apps);
	p->delay_send_key_apps->uninit(p->delay_send_key_apps);	
	p->abbreviations->uninit(p->abbreviations);
	
	p->plugins->uninit(p->plugins);
	
	for (int hotkey = 0; hotkey < MAX_HOTKEYS; hotkey++)
	{
		if (p->hotkeys[hotkey].key != NULL)
			free(p->hotkeys[hotkey].key);
	}

	for (int notify = 0; notify < MAX_NOTIFIES; notify++)
	{
		if (p->sounds[notify].file != NULL)
			free(p->sounds[notify].file);

		if (p->osds[notify].file != NULL)
			free(p->osds[notify].file);

		if (p->popups[notify].file != NULL)
			free(p->popups[notify].file);
	}

	if (p->actions != NULL)
	{
		for (int action = 0; action < p->actions_count; action++)
		{
			if (p->actions[action].hotkey.key != NULL)
				free(p->actions[action].hotkey.key);
			if (p->actions[action].name != NULL)
				free(p->actions[action].name);
			if (p->actions[action].command != NULL)
				free(p->actions[action].command);
		}
	}

	bzero(p->hotkeys, MAX_HOTKEYS * sizeof(struct _xneur_hotkey));
	bzero(p->sounds, MAX_NOTIFIES * sizeof(struct _xneur_notify));
	bzero(p->osds, MAX_NOTIFIES * sizeof(struct _xneur_notify));
	bzero(p->popups, MAX_NOTIFIES * sizeof(struct _xneur_notify));

	p->actions_count = 0;

	if (p->version != NULL)
		free(p->version);

	if (p->osd_font != NULL)
		free(p->osd_font);

	if (p->actions != NULL)
		free(p->actions);
}

static void xneur_config_reload(struct _xneur_config *p)
{
	pid_t process_id = p->get_pid(p);
	if (process_id <= 0)
		return;

	kill(process_id, SIGHUP);
}

static pid_t xneur_config_set_pid(struct _xneur_config *p, pid_t process_id)
{
	// Set lock file to ~/.xneur/.cache/lock
	char *lock_file_path_name = get_home_file_path_name(CACHEDIR, LOCK_NAME);
	if (lock_file_path_name == NULL)
		return -1;
	
	if (process_id == 0)
	{
	    if (remove(lock_file_path_name) == -1)
		{
			free(lock_file_path_name);
			return -1;
		}
		free(lock_file_path_name);
		p->pid = process_id;
		return process_id;
	}     
	
	log_message(LOG, _("Saving lock file to %s (pid %d)"), lock_file_path_name, process_id);

	FILE *stream = fopen(lock_file_path_name, "w");
	if (stream == NULL)
	{
		log_message(ERROR, _("Can't create lock file %s"), lock_file_path_name);
		free(lock_file_path_name);
		return -1;
	}

	free(lock_file_path_name);
	
	fprintf(stream, "%d", process_id);
	fclose (stream);
	p->pid = process_id;
	return process_id;
}

static int xneur_config_kill(struct _xneur_config *p)
{
	pid_t process_id = p->get_pid(p);
	if (process_id <= 0)
		return FALSE;

	if (kill(process_id, SIGTERM) == -1)
		return FALSE;

	xneur_config_set_pid(p, 0);

	return TRUE;
}

static int xneur_config_get_pid(struct _xneur_config *p)
{
	// Get lock file from ~/.xneur/.cache/lock
	char *config_file_path_name = get_home_file_path_name(CACHEDIR, LOCK_NAME);
	if (config_file_path_name == NULL)
		return -1;
	
	log_message(LOG, _("Get lock file %s"), config_file_path_name);
	
	char *pid_str = get_file_content(config_file_path_name);
	free(config_file_path_name);
	if (pid_str == NULL)
		return -1;

	pid_t process_id = atoi(pid_str);
	free(pid_str);

	if (getsid(process_id) == -1)
		return -1;

	char *ps_command = (char *) malloc(1024 * sizeof(char));
	if (ps_command == NULL)
		return -1;
	snprintf(ps_command, 1024, "ps -p %d | grep xneur", process_id);
	FILE *fp = popen(ps_command, "r");
	if (ps_command != NULL)
		free (ps_command);
	if (fp != NULL)
	{
		char buffer[1024];
		if (fgets(buffer, 1024, fp) != NULL)
		{
			p->pid = process_id;
			pclose(fp);
			return process_id;
		}
		pclose(fp);
	}
	
	return -1;
}

static int xneur_config_load(struct _xneur_config *p)
{
	if (!parse_config_file(p, NULL, CONFIG_NAME))
		return FALSE;

	if (p->handle->total_languages == 0)
	{
		log_message(ERROR, _("No languages specified in config file"));
		return FALSE;
	}
	return TRUE;
}

static void xneur_config_clear(struct _xneur_config *p)
{
	free_structures(p);

	p->window_layouts		= list_char_init();
	p->manual_apps			= list_char_init();
	p->auto_apps			= list_char_init();
	p->layout_remember_apps		= list_char_init();
	p->dont_send_key_release_apps	= list_char_init();
	p->delay_send_key_apps	= list_char_init();
	
	p->excluded_apps		= list_char_init();
	p->autocompletion_excluded_apps	= list_char_init();
	p->abbreviations		= list_char_init();
	p->plugins				= list_char_init();
	
	p->version	= NULL;
	p->osd_font	= NULL;
	p->actions	= NULL;
}

static int xneur_config_save(struct _xneur_config *p)
{
 	char *config_file_path_name = get_home_file_path_name(NULL, CONFIG_NAME);
	if (config_file_path_name == NULL)
		return FALSE;
	
	log_message(LOG, _("Saving main config to %s"), config_file_path_name);

	FILE *stream = fopen(config_file_path_name, "w");
	if (stream == NULL)
	{
		log_message(ERROR, _("Can't create file %s"), config_file_path_name);
		if (config_file_path_name != NULL)
			free(config_file_path_name);
		return FALSE;
	}

	if (config_file_path_name != NULL)
		free(config_file_path_name);

	fprintf(stream, "# It's a X Neural Switcher configuration file by XNeur\n# All values writted XNeur\n\n");
	
	fprintf(stream, "# Config version\nVersion %s\n\n", VERSION);
	fprintf(stream, "# Work in manual mode\nManualMode %s\n\n", p->get_bool_name(p->manual_mode));

	fprintf(stream, "# Level of messages program will write to output\n");
	fprintf(stream, "#LogLevel Error\n");
	fprintf(stream, "#LogLevel Warning\n");
	fprintf(stream, "#LogLevel Log\n");
	fprintf(stream, "#LogLevel Debug\n");
	fprintf(stream, "#LogLevel Trace\n");
	fprintf(stream, "LogLevel %s\n\n", p->get_log_level_name(p));

	fprintf(stream, "# Word Delimeters\n");
	fprintf(stream, "#Delimeter space\n");
	for (int i = 0; i < p->delimeters_count; i++)
	{
		fprintf(stream, "Delimeter %s\n", XKeysymToString(p->delimeters[i]));
	}
	fprintf(stream, "\n");
	
	fprintf(stream, "# Define unused languages\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#ExcludeLanguage de\n");

	for (int lang = 0; lang < p->handle->total_languages; lang++)
	{
		if (p->handle->languages[lang].excluded)
			fprintf(stream, "ExcludeLanguage %s\n", p->handle->languages[lang].dir);
	}
	fprintf(stream, "\n");

	fprintf(stream, "# Define initial keyboard layout for all new applications\n");
	fprintf(stream, "DefaultXkbGroup %d\n\n", p->default_group);

	fprintf(stream, "# Add Applications names to exclude it from procces with xneur\n");
	fprintf(stream, "# Xneur will not process the input for this applications\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#ExcludeApp Gaim\n");
	for (int i = 0; i < p->excluded_apps->data_count; i++)
		fprintf(stream, "ExcludeApp %s\n", p->excluded_apps->data[i].string);
	fprintf(stream, "\n");

	fprintf(stream, "# Use this parameter to force set work mode in current application to Auto.\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#SetAutoApp Gedit\n");
	for (int i = 0; i < p->auto_apps->data_count; i++)
		fprintf(stream, "SetAutoApp %s\n", p->auto_apps->data[i].string);
	fprintf(stream, "\n");

	fprintf(stream, "# Use this parameter to force set work mode in current application to Manual.\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#SetManualApp Anjuta\n");
	for (int i = 0; i < p->manual_apps->data_count; i++)
		fprintf(stream, "SetManualApp %s\n", p->manual_apps->data[i].string);
	fprintf(stream, "\n");

	fprintf(stream, "# Binds hotkeys for some actions\n");
	for (int action = 0; action < MAX_HOTKEYS; action++)
	{
		fprintf(stream, "AddBind %s ", action_names[action]);

		const int total_modifiers = sizeof(modifier_names) / sizeof(modifier_names[0]);
		for (int i = 0; i < total_modifiers; i++)
		{
			if (p->hotkeys[action].modifiers & (1 << i))
				fprintf(stream, "%s ", modifier_names[i]);
		}

		if (p->hotkeys[action].key != NULL)
			fprintf(stream, "%s", p->hotkeys[action].key);
		fprintf(stream, "\n");
	}
	fprintf(stream, "\n");

	fprintf(stream, "# This option add user action when pressed key bind\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#AddAction Control Alt f Firefox Browser <cmd>firefox</cmd>\n");
	for (int action = 0; action < p->actions_count; action++)
	{
		fprintf(stream, "AddAction ");

		const int total_modifiers = sizeof(modifier_names) / sizeof(modifier_names[0]);
		for (int i = 0; i < total_modifiers; i++)
		{
			if (p->actions[action].hotkey.modifiers & (1 << i))
				fprintf(stream, "%s ", modifier_names[i]);
		}
		if (p->actions[action].name == NULL)
			fprintf(stream, "%s %s\n", p->actions[action].hotkey.key, p->actions[action].command);
		else
			fprintf(stream, "%s %s %s%s%s\n", p->actions[action].hotkey.key, p->actions[action].name, USR_CMD_START, p->actions[action].command, USR_CMD_END);
	}
	fprintf(stream, "\n");

	fprintf(stream, "# Word Replacing\n# Ignore keyboard layout for abbreviations list\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#ReplaceAbbreviationIgnoreLayout No\n");
	fprintf(stream, "ReplaceAbbreviationIgnoreLayout %s\n\n", p->get_bool_name(p->abbr_ignore_layout));

	fprintf(stream, "# Abbreviations list\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#ReplaceAbbreviation xneur X Neural Switcher\n");
	for (int words = 0; words < p->abbreviations->data_count; words++)
		//fprintf(stream, "ReplaceAbbreviation %s\n", real_sym_to_escaped_sym(p->abbreviations->data[words].string));
	{
		char *str = real_sym_to_escaped_sym(p->abbreviations->data[words].string);
		if (str == NULL)
			continue;
		fprintf(stream, "ReplaceAbbreviation %s\n", str);
		free(str);
	}
	fprintf(stream, "\n");

	fprintf(stream, "# This option enable or disable sound playing\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#PlaySounds No\n");
	fprintf(stream, "PlaySounds %s\n\n", p->get_bool_name(p->play_sounds));

	fprintf(stream, "# This option defined sound playing volume percent\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#SoundVolumePercent 10\n");
	fprintf(stream, "SoundVolumePercent %d\n\n", p->volume_percent);
	    
	fprintf(stream, "# Binds sounds for some actions\n");
	for (int sound = 0; sound < MAX_NOTIFIES; sound++)
	{
		if (p->sounds[sound].file == NULL)
			fprintf(stream, "AddSound %s \n", notify_names[sound]);
		else
			fprintf(stream, "AddSound %s %s %s\n", notify_names[sound], status_names[p->sounds[sound].enabled], p->sounds[sound].file);
	}
	fprintf(stream, "\n");

	fprintf(stream, "# This option enable or disable self education of xneur\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#EducationMode No\n");
	fprintf(stream, "EducationMode %s\n\n", p->get_bool_name(p->educate));

	fprintf(stream, "# This option enable or disable checking similar words in heuristics\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CheckSimilarWords No\n");
	fprintf(stream, "CheckSimilarWords %s\n\n", p->get_bool_name(p->check_similar_words));
	
	fprintf(stream, "# This option enable or disable layout remember for each window\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#LayoutRememberMode No\n");
	fprintf(stream, "LayoutRememberMode %s\n\n", p->get_bool_name(p->remember_layout));

	fprintf(stream, "# Use this parameter to force enable layout remember for each application, not window.\n");
	fprintf(stream, "# Option \"LayoutRememberMode\" must be enabled.\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#LayoutRememberModeForApp Gaim\n");
	for (int i = 0; i < p->layout_remember_apps->data_count; i++)
		fprintf(stream, "LayoutRememberModeForApp %s\n", p->layout_remember_apps->data[i].string);
	fprintf(stream, "\n");

	fprintf(stream, "# This option enable or disable saving selection text\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#SaveSelectionMode No\n");
	fprintf(stream, "SaveSelectionMode %s\n\n", p->get_bool_name(p->save_selection_after_convert));

	fprintf(stream, "# This option enable or disable rotating layout after convert selected text\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#RotateLayoutAfterChangeSelectedMode No\n");
	fprintf(stream, "RotateLayoutAfterChangeSelectedMode %s\n\n", p->get_bool_name(p->rotate_layout_after_convert));

	fprintf(stream, "# Add Applications names to include it to delay sending process\n");
	fprintf(stream, "# Xneur will be delay KeyPress and KeyRelease events for this applications\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#DelaySendingEventApp Firefox\n");
	for (int i = 0; i < p->delay_send_key_apps->data_count; i++)
		fprintf(stream, "DelaySendingEventApp %s\n", p->delay_send_key_apps->data[i].string);
	fprintf(stream, "\n");
	
	fprintf(stream, "# This option define delay before sendind events to application (in milliseconds between 0 to 50).\n");
	fprintf(stream, "SendDelay %d\n\n", p->send_delay);
	
	fprintf(stream, "# This option enable or disable logging keyboard\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#LogSave No\n");
	fprintf(stream, "LogSave %s\n\n", p->get_bool_name(p->save_keyboard_log));

	fprintf(stream, "# This option set max size of log file (bytes).\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#LogSize 1048576\n");
	fprintf(stream, "LogSize %d\n\n", p->size_keyboard_log);

	fprintf(stream, "# This option define e-mail for send log file, if it's size greater then max size.\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#LogMail your.mail@your.server.com\n");
	if (p->mail_keyboard_log != NULL)
		fprintf(stream, "LogMail %s\n\n", p->mail_keyboard_log);

	fprintf(stream, "# This option define host to send e-mail without login and password.\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#LogHostIP 127.0.0.1\n");
	fprintf(stream, "#LogHostIP mail.example.com\n");
	if (p->host_keyboard_log != NULL)
		fprintf(stream, "LogHostIP %s\n\n", p->host_keyboard_log);

	fprintf(stream, "# This option define port to send e-mail without login and password.\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#LogPort 25\n");
	fprintf(stream, "LogPort %d\n\n", p->port_keyboard_log);
	
	fprintf(stream, "# This option enable or disable correction of iNCIDENTAL CapsLock\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CorrectIncidentalCaps Yes\n");
	fprintf(stream, "CorrectIncidentalCaps %s\n\n", p->get_bool_name(p->correct_incidental_caps));

	fprintf(stream, "# This option enable or disable correction of two CApital letter\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CorrectTwoCapitalLetter Yes\n");
	fprintf(stream, "CorrectTwoCapitalLetter %s\n\n", p->get_bool_name(p->correct_two_capital_letter));

	fprintf(stream, "# This option enable or disable correction of small letter to capital letter after dot\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CorrectCapitalLetterAfterDot Yes\n");
	fprintf(stream, "CorrectCapitalLetterAfterDot %s\n\n", p->get_bool_name(p->correct_capital_letter_after_dot));

	fprintf(stream, "# This option enable or disable correction of two space with a comma and a space\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CorrectTwoSpaceWithCommaAndSpace Yes\n");
	fprintf(stream, "CorrectTwoSpaceWithCommaAndSpace %s\n\n", p->get_bool_name(p->correct_two_space_with_comma_and_space));

	fprintf(stream, "# This option enable or disable correction of two minus with a dash\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CorrectTwoMinusWithDash Yes\n");
	fprintf(stream, "CorrectTwoMinusWithDash %s\n\n", p->get_bool_name(p->correct_two_minus_with_dash));

	fprintf(stream, "# This option enable or disable correction of two minus with a emdash\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CorrectDashWithEmDash Yes\n");
	fprintf(stream, "CorrectDashWithEmDash %s\n\n", p->get_bool_name(p->correct_dash_with_emdash));

	fprintf(stream, "# This option enable or disable correction of (c) with a copyright sign\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CorrectCWithCopyright Yes\n");
	fprintf(stream, "CorrectCWithCopyright %s\n\n", p->get_bool_name(p->correct_c_with_copyright));

	fprintf(stream, "# This option enable or disable correction of (tm) with a trademark sign\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CorrectTMWithTrademark Yes\n");
	fprintf(stream, "CorrectTMWithTrademark %s\n\n", p->get_bool_name(p->correct_tm_with_trademark));

	fprintf(stream, "# This option enable or disable correction of (r) with a registered sign\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CorrectRWithRegistered Yes\n");
	fprintf(stream, "CorrectRWithRegistered %s\n\n", p->get_bool_name(p->correct_r_with_registered));

	fprintf(stream, "# This option enable or disable correction of three points with a ellipsis sign\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CorrectThreePointsWithEllipsis Yes\n");
	fprintf(stream, "CorrectThreePointsWithEllipsis %s\n\n", p->get_bool_name(p->correct_three_points_with_ellipsis));

	fprintf(stream, "# This option enable or disable correction of mispints\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CorrectMisprint Yes\n");
	fprintf(stream, "CorrectMisprint %s\n\n", p->get_bool_name(p->correct_misprint));
	
	fprintf(stream, "# This option enable or disable flushing internal buffer when pressed Escape\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#FlushBufferWhenPressEscape Yes\n");
	fprintf(stream, "FlushBufferWhenPressEscape %s\n\n", p->get_bool_name(p->flush_buffer_when_press_escape));
	
	fprintf(stream, "# This option enable or disable flushing internal buffer when pressed Enter or Tab\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#FlushBufferWhenPressEnter Yes\n");
	fprintf(stream, "FlushBufferWhenPressEnter %s\n\n", p->get_bool_name(p->flush_buffer_when_press_enter));

	fprintf(stream, "# This option disable or enable show OSD\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#ShowOSD Yes\n");
	fprintf(stream, "ShowOSD %s\n\n", p->get_bool_name(p->show_osd));

	fprintf(stream, "# This option set font for OSD\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#FontOSD -*-*-*-*-*-*-32-*-*-*-*-*-*-u\n");
	fprintf(stream, "FontOSD %s\n\n", p->osd_font);

	fprintf(stream, "# Binds OSDs for some actions\n");
	for (int notify = 0; notify < MAX_NOTIFIES; notify++)
	{
		if (p->osds[notify].file == NULL)
			fprintf(stream, "AddOSD %s\n", notify_names[notify]);
		else
			fprintf(stream, "AddOSD %s %s %s\n", notify_names[notify], status_names[p->osds[notify].enabled], p->osds[notify].file);
	}

	fprintf(stream, "\n# This option disable or enable show popup messages\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#ShowPopup Yes\n");
	fprintf(stream, "ShowPopup %s\n\n", p->get_bool_name(p->show_popup));

	fprintf(stream, "# This option defines popup expiration interval in milliseconds\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#PopupExpireTimeout 1000\n");
	fprintf(stream, "PopupExpireTimeout %d\n\n", p->popup_expire_timeout);

	fprintf(stream, "# Binds popup messages for some actions\n");
	for (int notify = 0; notify < MAX_NOTIFIES; notify++)
	{
		if (p->popups[notify].file == NULL)
			fprintf(stream, "AddPopup %s\n", notify_names[notify]);
		else
			fprintf(stream, "AddPopup %s %s %s\n", notify_names[notify], status_names[p->popups[notify].enabled], p->popups[notify].file);
	}

	fprintf(stream, "\n# This option disable or enable checking language on input process\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CheckOnProcess Yes\n");
	fprintf(stream, "CheckOnProcess %s\n", p->get_bool_name(p->check_lang_on_process));

	fprintf(stream, "\n# This option disable or enable CapsLock use\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#DisableCapsLock Yes\n");
	fprintf(stream, "DisableCapsLock %s\n", p->get_bool_name(p->disable_capslock));

	fprintf(stream, "\n# This option disable or enable correction spaces befor punctuation\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#CorrectSpaceWithPunctuation No\n");
	fprintf(stream, "CorrectSpaceWithPunctuation %s\n", p->get_bool_name(p->correct_space_with_punctuation));

	fprintf(stream, "\n# This option disable or enable pattern mining and recognition (autocompletion)\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#Autocompletion No\n");
	fprintf(stream, "Autocompletion %s\n", p->get_bool_name(p->autocompletion));

	fprintf(stream, "\n# This option disable or enable adding space after autocompletion\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#AddSpaceAfterAutocompletion No\n");
	fprintf(stream, "AddSpaceAfterAutocompletion %s\n", p->get_bool_name(p->add_space_after_autocompletion));
	fprintf(stream, "\n");

	fprintf(stream, "# Add Applications names to exclude it from autocompletion process\n");
	fprintf(stream, "# Xneur will not process the autocompletion for this applications\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#AutocompletionExcludeApp Gnome-terminal\n");
	for (int i = 0; i < p->autocompletion_excluded_apps->data_count; i++)
		fprintf(stream, "AutocompletionExcludeApp %s\n", p->autocompletion_excluded_apps->data[i].string);
	fprintf(stream, "\n");

	fprintf(stream, "# Use next options for troubleshoot on autoswitching\n");
	fprintf(stream, "# Disable autoswitching if pressed backspace\nTroubleshootBackspace %s\n", p->get_bool_name(p->troubleshoot_backspace));
	fprintf(stream, "# Disable autoswitching if pressed left arrow\nTroubleshootLeftArrow %s\n", p->get_bool_name(p->troubleshoot_left_arrow));
	fprintf(stream, "# Disable autoswitching if pressed right arrow\nTroubleshootRightArrow %s\n", p->get_bool_name(p->troubleshoot_right_arrow));
	fprintf(stream, "# Disable autoswitching if pressed up arrow\nTroubleshootUpArrow %s\n", p->get_bool_name(p->troubleshoot_up_arrow));
	fprintf(stream, "# Disable autoswitching if pressed down arrow\nTroubleshootDownArrow %s\n", p->get_bool_name(p->troubleshoot_down_arrow));
	fprintf(stream, "# Disable autoswitching if pressed delete\nTroubleshootDelete %s\n", p->get_bool_name(p->troubleshoot_delete));
	fprintf(stream, "# Disable autoswitching if pressed enter\nTroubleshootEnter %s\n", p->get_bool_name(p->troubleshoot_enter));
	fprintf(stream, "# Disable autoswitching if pressed tab\nTroubleshootTab %s\n", p->get_bool_name(p->troubleshoot_tab));
	fprintf(stream, "# Disable autoswitching if layout switched\nTroubleshootSwitch %s\n", p->get_bool_name(p->troubleshoot_switch));
	fprintf(stream, "# Disable autoswitching for full screen apps\nTroubleshootFullScreen %s\n\n", p->get_bool_name(p->troubleshoot_full_screen));

	fprintf(stream, "# Add Applications names to exclude it from sending KeyRelease process\n");
	fprintf(stream, "# Xneur will not send KeyRelease events for this applications\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#SetDontSendKeyReleaseApp libreoffice-writer\n");
	for (int i = 0; i < p->dont_send_key_release_apps->data_count; i++)
		fprintf(stream, "SetDontSendKeyReleaseApp %s\n", p->dont_send_key_release_apps->data[i].string);
	fprintf(stream, "\n");
	
	fprintf(stream, "# Work-arround for compatibility with the completion\nCompatibilityWithCompletion %s\n\n", p->get_bool_name(p->compatibility_with_completion));

	fprintf(stream, "# Disabling this option will add any application to the list of excluded applications.\nTrackingInput %s\n\n", p->get_bool_name(p->tracking_input));

	fprintf(stream, "# Disabling this option will disable mouse tracking.\nTrackingMouse %s\n\n", p->get_bool_name(p->tracking_mouse));

	fprintf(stream, "# Modules list\n");
	fprintf(stream, "# Example:\n");
	fprintf(stream, "#LoadModule libxntest.so\n");
	for (int plugins = 0; plugins < p->plugins->data_count; plugins++)
		fprintf(stream, "LoadModule %s\n", p->plugins->data[plugins].string);
	fprintf(stream, "\n");
	        
	fprintf(stream, "# That's all\n");

	fclose(stream);

	return TRUE;
}

static int xneur_config_replace(struct _xneur_config *p)
{
	char *config_file_path_name = get_file_path_name(NULL, CONFIG_NAME);
	if (config_file_path_name == NULL)
		return FALSE;
	char *config_backup_file_path_name = get_file_path_name(NULL, CONFIG_BCK_NAME);
	if (config_backup_file_path_name == NULL)
	{
		free(config_file_path_name);
		return FALSE;
	}
	
	log_message(LOG, _("Moving config file from %s to %s"), config_file_path_name, config_backup_file_path_name);
	
	if (remove(config_backup_file_path_name) == -1)
	{
		log_message(ERROR, _("Can't move file!"), config_backup_file_path_name);
		free(config_file_path_name);
		free(config_backup_file_path_name);
		return FALSE;
	}

	if (rename(config_file_path_name, config_backup_file_path_name) != 0)
	{
		log_message(ERROR, _("Can't move file!"), config_backup_file_path_name);
		free(config_file_path_name);
		free(config_backup_file_path_name);
		return FALSE;
	}

	free(config_file_path_name);
	free(config_backup_file_path_name);

	return p->load(p);
}

static void xneur_config_save_dict(struct _xneur_config *p, int lang)
{
	if (!p->educate)
		return;

	log_message(LOG, _("Saving %s dictionary"), p->handle->languages[lang].name);

	int path_len = strlen(LANGUAGEDIR) + strlen(p->handle->languages[lang].dir) + 2;
	char *lang_dir = (char *) malloc(path_len * sizeof(char));
	snprintf(lang_dir, path_len, "%s/%s", LANGUAGEDIR, p->handle->languages[lang].dir);
	save_list(p->handle->languages[lang].dictionary, lang_dir, DICT_NAME);
	if (lang_dir != NULL)
		free (lang_dir);
}

static void xneur_config_save_pattern(struct _xneur_config *p, int lang)
{
	if (!p->educate)
		return;

	log_message(LOG, _("Saving %s pattern"), p->handle->languages[lang].name);

	int path_len = strlen(LANGUAGEDIR) + strlen(p->handle->languages[lang].dir) + 2;
	char *lang_dir = (char *) malloc(path_len * sizeof(char));
	snprintf(lang_dir, path_len, "%s/%s", LANGUAGEDIR, p->handle->languages[lang].dir);
	save_list(p->handle->languages[lang].pattern, lang_dir, PATTERN_NAME);
	if (lang_dir != NULL)
		free(lang_dir);
}

static char* xneur_config_get_lang_dir(struct _xneur_config *p, int lang)
{
	if (lang < 0 || lang >= p->handle->total_languages)
		return NULL;

	int path_len = strlen(LANGUAGEDIR) + strlen(p->handle->languages[lang].dir) + 2;
	char *path_file = (char *) malloc(path_len * sizeof(char));
	snprintf(path_file, path_len, "%s/%s", LANGUAGEDIR, p->handle->languages[lang].dir);

	return path_file;
}

static const char* xneur_config_get_log_level_name(struct _xneur_config *p)
{
	return log_levels[p->log_level];
}

static void xneur_config_uninit(struct _xneur_config *p)
{
	if (p == NULL)
		return;
	
	free_structures(p);

	free(p->hotkeys);
	free(p->sounds);
	free(p->osds);
	free(p->popups);

	if (p->delimeters != NULL)
		free(p->delimeters);
	if (p->delimeters_string != NULL)
		free(p->delimeters_string);
	p->delimeters_count = 0;

	if (p->mail_keyboard_log != NULL) 
		free(p->mail_keyboard_log);
	if (p->host_keyboard_log != NULL) 
		free(p->host_keyboard_log);

	xneur_handle_destroy(p->handle);

	free(p);
}

struct _xneur_config* xneur_config_init(void)
{
	struct _xneur_config *p = (struct _xneur_config *) malloc(sizeof(struct _xneur_config));
	bzero(p, sizeof(struct _xneur_config));

	p->pid = -1;
	
	p->handle = xneur_handle_create();

	p->delimeters = (KeySym *) malloc(sizeof(KeySym));
	p->delimeters_string = (char *) malloc(sizeof(char));
	p->delimeters[p->delimeters_count] = XK_space;
	p->delimeters_string[p->delimeters_count] = '\0';
	p->delimeters_count++;

	p->hotkeys = (struct _xneur_hotkey *) malloc(MAX_HOTKEYS * sizeof(struct _xneur_hotkey));
	bzero(p->hotkeys, MAX_HOTKEYS * sizeof(struct _xneur_hotkey));

	p->sounds = (struct _xneur_notify *) malloc(MAX_NOTIFIES * sizeof(struct _xneur_notify));
	bzero(p->sounds, MAX_NOTIFIES * sizeof(struct _xneur_notify));

	p->osds = (struct _xneur_notify *) malloc(MAX_NOTIFIES * sizeof(struct _xneur_notify));
	bzero(p->osds, MAX_NOTIFIES * sizeof(struct _xneur_notify));

	p->popups = (struct _xneur_notify *) malloc(MAX_NOTIFIES * sizeof(struct _xneur_notify));
	bzero(p->popups, MAX_NOTIFIES * sizeof(struct _xneur_notify));

	p->mail_keyboard_log = NULL;
	p->host_keyboard_log = NULL;
	p->port_keyboard_log = 25;
	
	p->log_level			= LOG;
	p->excluded_apps		= list_char_init();
	p->auto_apps			= list_char_init();
	p->manual_apps			= list_char_init();
	p->layout_remember_apps		= list_char_init();
	p->dont_send_key_release_apps = list_char_init();
	p->delay_send_key_apps	= list_char_init();
	
	p->window_layouts		= list_char_init();
	p->abbreviations		= list_char_init();
	p->autocompletion_excluded_apps	= list_char_init();
	p->plugins		= list_char_init();

	p->check_similar_words = TRUE;
	
	p->troubleshoot_backspace = FALSE; 
	p->troubleshoot_left_arrow = FALSE; 
	p->troubleshoot_right_arrow = FALSE;
	p->troubleshoot_up_arrow = FALSE;
	p->troubleshoot_down_arrow = FALSE;
	p->troubleshoot_delete = FALSE;
	p->troubleshoot_switch = TRUE;
	p->troubleshoot_full_screen = TRUE;
	
	p->tracking_input = TRUE;
	p->tracking_mouse = TRUE;
	
	p->popup_expire_timeout = 1000;

	// Function mapping
	p->get_home_dict_path		= get_home_file_path_name;
	p->get_global_dict_path		= get_file_path_name;

	p->get_library_version		= xneur_config_get_library_version;
	p->get_bool_name		= xneur_config_get_bool_name;

	p->load				= xneur_config_load;
	p->clear			= xneur_config_clear;
	p->save				= xneur_config_save;
	p->replace			= xneur_config_replace;
	p->reload			= xneur_config_reload;
	p->kill				= xneur_config_kill;
	p->save_dict			= xneur_config_save_dict;
	p->save_pattern			= xneur_config_save_pattern;
	p->set_pid			= xneur_config_set_pid;
	p->get_pid			= xneur_config_get_pid;
	p->get_lang_dir		= xneur_config_get_lang_dir;
	p->get_log_level_name		= xneur_config_get_log_level_name;

	p->uninit			= xneur_config_uninit;

	return p;
}

