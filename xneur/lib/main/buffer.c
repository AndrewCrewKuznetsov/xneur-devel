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

#include <X11/XKBlib.h>

#include <pthread.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>

#include "xnconfig.h"
#include "xnconfig_files.h"

#include "event.h"
#include "window.h"
#include "keymap.h"
#include "utils.h"

#include "types.h"
#include "text.h"
#include "conversion.h"
#include "log.h"
#include "mail.h"
#include "archiver.h"

#include "buffer.h"

#define INIT_STRING_LENGTH 64

static const int keyboard_groups[]	= {0x00000000, 0x00002000, 0x00004000, 0x00006000};

extern struct _xneur_config *xconfig;

Window last_log_window = 0;
time_t last_log_time = 0;

// Private
static void set_new_size(struct _buffer *p, int new_size)
{
	p->cur_size		= new_size;
	p->content		= (char *) realloc(p->content, p->cur_size * sizeof(char));
	p->keycode		= (KeyCode *) realloc(p->keycode, p->cur_size * sizeof(KeyCode));
	p->keycode_modifiers	= (int *) realloc(p->keycode_modifiers, p->cur_size * sizeof(int));
}

static void buffer_set_lang_mask(struct _buffer *p, int lang)
{
	int keycode_mod		= get_keycode_mod(lang);
	int languages_mask	= get_languages_mask();

	// Set new language bit
	for (int i = 0; i < p->cur_pos; i++)
	{
		p->keycode_modifiers[i] = p->keycode_modifiers[i] & (~languages_mask);
		p->keycode_modifiers[i] = p->keycode_modifiers[i] | keycode_mod;
	}
}

static void buffer_set_uncaps_mask(struct _buffer *p)
{
	// Set uncaps bit
	for (int i = 0; i < p->cur_pos; i++)
		p->keycode_modifiers[i] = p->keycode_modifiers[i] & (~LockMask);
}

static void buffer_set_caps_mask(struct _buffer *p)
{
	// Set uncaps bit
	for (int i = 0; i < p->cur_pos; i++)
		p->keycode_modifiers[i] = p->keycode_modifiers[i] | (LockMask);
}

static void buffer_mail_and_archive(char *file_path_name)
{
	time_t curtime = time(NULL);
	struct tm *loctime = localtime(&curtime);
	if (loctime == NULL)
		return;
	
	char *date = malloc(256 * sizeof(char));
	char *time = malloc(256 * sizeof(char));
	strftime(date, 256, "%x", loctime);
	strftime(time, 256, "%X", loctime);
	
	int len = strlen(file_path_name) + strlen(date) + strlen(time) + 4;
	char *arch_file_path_name = malloc(len * sizeof (char));
	snprintf(arch_file_path_name, len, "%s %s %s", file_path_name, date, time);
		
	if (rename(file_path_name, arch_file_path_name) == 0)
	{
		// Compress the file
		char *gz_arch_file_path_name = malloc(len+3 * sizeof (char));
		snprintf(gz_arch_file_path_name, len+3, "%s%s", arch_file_path_name, ".gz");
	
		FILE *source = fopen(arch_file_path_name, "r");
		FILE *dest = fopen(gz_arch_file_path_name, "w");
		if ((source != NULL) && (dest != NULL))
			file_compress (source, dest);
		if (source != NULL)
			fclose (source);
		if (dest != NULL)
			fclose (dest);

		// Remove uncompressed file
		remove(arch_file_path_name);

		log_message(DEBUG, _("Compressed old log file to %s"), gz_arch_file_path_name);

		// Send to e-mail
		send_mail_with_attach(gz_arch_file_path_name, xconfig->host_keyboard_log, xconfig->port_keyboard_log, xconfig->mail_keyboard_log);
		log_message(DEBUG, _("Sended log to e-mail %s via %s:%d host"), xconfig->mail_keyboard_log, xconfig->host_keyboard_log, xconfig->port_keyboard_log);

		if (gz_arch_file_path_name != NULL) 
			free(gz_arch_file_path_name);
	}
	else
		log_message (ERROR, _("Error rename file \"%s\" to \"%s\""), file_path_name, arch_file_path_name);

	if (arch_file_path_name != NULL)
		free(arch_file_path_name);
	if (file_path_name != NULL)
		free(file_path_name);
	if (time != NULL)
		free(time);
	if (date != NULL)
		free(date);
}

static void buffer_save(struct _buffer *p, char *file_name, Window window)
{
#ifdef WITH_KEYLOGGER
	if (!xconfig->save_keyboard_log || p->cur_pos == 0 || file_name == NULL)
#endif		
		return;

	if (strlen (p->content) < 4)
		return;
	
	int save = FALSE;
	for (int i = 0; i < p->cur_pos; i++)
		if (isgraph (p->content[i]))
		{
			save = TRUE;
			break;
		}
	if (!save)
		return;
	
	char *file_path_name = get_home_file_path_name(NULL, file_name);

	time_t curtime = time(NULL);
	struct tm *loctime = localtime(&curtime);
	if (loctime == NULL)
		return;
	
	char *buffer = malloc(256 * sizeof(char));
	
	// Check file size
	struct stat sb;

	if (stat(file_path_name, &sb) == 0 && sb.st_size > xconfig->size_keyboard_log)
	{
		pthread_attr_t mail_and_archive_thread_attr;
		pthread_attr_init(&mail_and_archive_thread_attr);
		pthread_attr_setdetachstate(&mail_and_archive_thread_attr, PTHREAD_CREATE_DETACHED);

		pthread_t mail_and_archive_thread;
		char *thread_file = strdup (file_path_name);
		pthread_create(&mail_and_archive_thread, &mail_and_archive_thread_attr, (void*) &buffer_mail_and_archive, thread_file);

		pthread_attr_destroy(&mail_and_archive_thread_attr);
	}
	//

	// Check existing log file
	FILE *stream = fopen(file_path_name, "r");
	if (stream == NULL) // File not exist
	{
		stream = fopen(file_path_name, "a");
		if (stream == NULL)
		{
			free(file_path_name);
			free(buffer);
			return;
		}
		fprintf(stream, "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"><title>X Neural Switcher Log</title></head><body>\n");
		fprintf(stream, "<ul></body></html>\n");
	}
	fclose (stream);
	
	stream = fopen(file_path_name, "r+");
	free(file_path_name);
	if (stream == NULL)
	{
		free(buffer);
		return;
	}

	fseek(stream, -15, SEEK_END);

	bzero(buffer, 256 * sizeof(char));
	strftime(buffer, 256, "%x", loctime);

	if (window != last_log_window)
	{
		last_log_window = window;
		last_log_time = 0;
		char *app_name = get_wm_class_name(window);
		fprintf(stream, "</ul>\n<br><font color=\"#FF0000\"><b>%s <font size=\"2\">[%s]</font></font></b><br><ul>\n", app_name, buffer);
		if (app_name != NULL)
			free(app_name);
	}

	if (difftime(curtime, last_log_time) > 300)
	{
		last_log_time = curtime;
		bzero(buffer, 256 * sizeof(char));
		strftime(buffer, 256, "%X", loctime);
		fprintf(stream, "</ul><ul>\n<font color=\"#0000FF\" size=\"2\">(%s): </font>", buffer);
	}
	free(buffer);
	
	for (int i = 0; i < p->cur_pos; i++)
	{
		if (p->keycode[i] == 36)			// Return
		{
			fprintf(stream, "<br>\n");
			continue;
		}
		if (p->keycode[i] == 23)			// Tab
		{
			fprintf(stream, "&nbsp;&nbsp;&nbsp;&nbsp;\t");
			continue;
		}

		char *symbol = p->keymap->keycode_to_symbol(p->keymap, p->keycode[i], -1, p->keycode_modifiers[i]);
		if (symbol == NULL)
		{
			fprintf(stream, "<?>");
			continue;
		}

		if (symbol[0] == ' ')
			fprintf(stream, "&nbsp;");
		else	
			fprintf(stream, "%s", symbol);
		
		free(symbol);
	}

	fprintf(stream, "\n</body></html>\n");
	fclose(stream);
}

static void buffer_clear(struct _buffer *p)
{
	for (int i = 0; i < p->cur_pos; i++)
	{
		p->keycode[i] = 0;
		p->keycode_modifiers[i] = 0;
	}

	p->cur_pos = 0;
	p->content[0] = NULLSYM;

	for (int i=0; i<p->handle->total_languages; i++)
	{
		p->i18n_content[i].content = realloc(p->i18n_content[i].content, sizeof(char));
		p->i18n_content[i].content[0] = NULLSYM;
		p->i18n_content[i].content_unchanged = realloc(p->i18n_content[i].content_unchanged, sizeof(char));
		p->i18n_content[i].content_unchanged[0] = NULLSYM;
	}
}

static int buffer_is_space_last(struct _buffer *p)
{
	if (p->cur_pos <= 0)
		return FALSE;

	if (isspace(p->content[p->cur_pos - 1]))
		return TRUE;

	return FALSE;
}

static void buffer_set_i18n_content(struct _buffer *p)
{
	// i18n_content
	/*for (int i = 0; i < p->handle->total_languages; i++)
	{
		p->i18n_content[i].content = (char *) realloc(p->i18n_content[i].content, 1);
		p->i18n_content[i].content[0] = NULLSYM;
		p->i18n_content[i].symbol_len = (int *) realloc(p->i18n_content[i].symbol_len, 1);
		p->i18n_content[i].content_unchanged = (char *) realloc(p->i18n_content[i].content_unchanged, 1);
		p->i18n_content[i].content_unchanged[0] = NULLSYM;
		p->i18n_content[i].symbol_len_unchanged = (int *) realloc(p->i18n_content[i].symbol_len_unchanged, 1);
	}*/
	
	int languages_mask = get_languages_mask();
	for (int k = 0; k < p->cur_size-1; k++)
	{
		int modifier = p->keycode_modifiers[k] & (~languages_mask);

		for (int i = 0; i < p->handle->total_languages; i++)
		{
		
			char *symbol = p->keymap->keycode_to_symbol(p->keymap, p->keycode[k], i, modifier & (~ShiftMask));
			if (symbol == NULL)
				continue;
			
			char *symbol_unchanged = p->keymap->keycode_to_symbol(p->keymap, p->keycode[k], i, modifier);
			if (symbol_unchanged == NULL)
				continue;
			
			p->i18n_content[i].content = (char *) realloc(p->i18n_content[i].content, (strlen(p->i18n_content[i].content) + strlen(symbol) + 1) * sizeof(char));
			p->i18n_content[i].content = strcat(p->i18n_content[i].content, symbol);

			p->i18n_content[i].content_unchanged = (char *) realloc(p->i18n_content[i].content_unchanged, (strlen(p->i18n_content[i].content_unchanged) + strlen(symbol_unchanged) + 1) * sizeof(char));
			p->i18n_content[i].content_unchanged = strcat(p->i18n_content[i].content_unchanged, symbol_unchanged);

			p->i18n_content[i].symbol_len = (int *) realloc(p->i18n_content[i].symbol_len, (k + 1) * sizeof(int));
			p->i18n_content[i].symbol_len[k] = strlen(symbol);

			p->i18n_content[i].symbol_len_unchanged = (int *) realloc(p->i18n_content[i].symbol_len_unchanged, (k + 1) * sizeof(int));
			p->i18n_content[i].symbol_len_unchanged[k] = strlen(symbol_unchanged);

			free(symbol);
			free(symbol_unchanged);
		}
	}
}

static void buffer_set_content(struct _buffer *p, const char *new_content)
{
	p->clear(p);
	char *content = NULL;
	content = strdup(new_content);
	p->cur_pos = strlen(content);
	if (p->cur_pos >= p->cur_size)
		set_new_size(p, p->cur_pos + 1);

	if (p->content == NULL || p->keycode == NULL || p->keycode_modifiers == NULL)
	{
		if (content != NULL)
			free(content);
		return;
	}
	
	p->content[p->cur_pos] = NULLSYM;
	if (!p->cur_pos)
	{
		if (p->content != NULL)
			free(content);
		return;
	}

	memcpy(p->content, content, p->cur_pos);
	free(content);

	p->keymap->convert_text_to_ascii(p->keymap, p->content, p->keycode, p->keycode_modifiers);

	p->cur_pos = strlen(p->content);
	set_new_size(p, p->cur_pos + 1);

	if (p->content == NULL || p->keycode == NULL || p->keycode_modifiers == NULL)
	{
		for (int i = 0; i < p->handle->total_languages; i++)
		{
			free(p->i18n_content[i].content);
			free(p->i18n_content[i].symbol_len);
			free(p->i18n_content[i].content_unchanged);
			free(p->i18n_content[i].symbol_len_unchanged);
		}
		return;
	}
	
	buffer_set_i18n_content(p);
}

static void buffer_change_case(struct _buffer *p)
{
	char *symbol = (char *) malloc((256 + 1) * sizeof(char));
	
	Display *display = XOpenDisplay(NULL);
	XEvent event;
	event.type		= KeyPress;
	event.xkey.type		= KeyPress;
	event.xkey.root		= RootWindow(display, DefaultScreen(display));
	event.xkey.subwindow	= None;
	event.xkey.same_screen	= True;
	event.xkey.display	= display;
	event.xkey.state	= 0;
	event.xkey.keycode	= XKeysymToKeycode(display, XK_space);
	event.xkey.time		= CurrentTime;
	
	for (int i = 0; i < p->cur_pos; i++)
	{
		event.xkey.keycode	= p->keycode[i];
		event.xkey.state	= p->keycode_modifiers[i];

		int nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, NULL, NULL);
		if (nbytes <= 0)
			continue;

		symbol[nbytes] = NULLSYM;	

		if (ispunct(symbol[0]) || isdigit(symbol[0]))
			continue;

		if (p->keycode_modifiers[i] & ShiftMask)
			p->keycode_modifiers[i] = (p->keycode_modifiers[i] & ~ShiftMask);
		else
			p->keycode_modifiers[i] = (p->keycode_modifiers[i] | ShiftMask);
	}
	
	free(symbol);
	XCloseDisplay(display);
}

static void buffer_rotate_layout(struct _buffer *p)
{
	int languages_mask = get_languages_mask();

	for (int i = 0; i < p->cur_pos; i++)
	{
		// Get current lang. modifier
		int km = p->keycode_modifiers[i] & (~languages_mask);
		for (int lang = 0; lang < p->handle->total_languages; lang++)
		{
			if (p->keycode_modifiers[i] == (get_keycode_mod(lang) | km))
			{
				lang++;
				if (lang == p->handle->total_languages)
					lang = 0;
				int keycode_mod	= get_keycode_mod(lang);
				p->keycode_modifiers[i] = p->keycode_modifiers[i] & (~languages_mask);
				p->keycode_modifiers[i] = p->keycode_modifiers[i] | keycode_mod;
				break;
			}
		}
	}
}

static void buffer_add_symbol(struct _buffer *p, char sym, KeyCode keycode, int modifier)
{
	if (p->cur_pos == p->cur_size - 1)
		set_new_size(p, p->cur_size * 2);

	if (p->content == NULL || p->keycode == NULL || p->keycode_modifiers == NULL)
		return;

	p->content[p->cur_pos] = sym;
	p->keycode[p->cur_pos] = keycode;
	p->keycode_modifiers[p->cur_pos] = modifier;

	// i18n_content
	int languages_mask = get_languages_mask();
	modifier = modifier & (~languages_mask);

	for (int i = 0; i < p->handle->total_languages; i++)
	{
		char *symbol = p->keymap->keycode_to_symbol(p->keymap, keycode, i, modifier & (~ShiftMask));
		if (symbol == NULL)
			continue;

		char *symbol_unchanged = p->keymap->keycode_to_symbol(p->keymap, keycode, i, modifier);
		if (symbol_unchanged == NULL)
			continue;

		//log_message (ERROR, _("'%c' - '%c' - '%c'"), sym, symbol, symbol_unchanged);
		p->i18n_content[i].content = (char *) realloc(p->i18n_content[i].content, (strlen(p->i18n_content[i].content) + strlen(symbol) + 1) * sizeof(char));
		p->i18n_content[i].content = strcat(p->i18n_content[i].content, symbol);

		p->i18n_content[i].content_unchanged = (char *) realloc(p->i18n_content[i].content_unchanged, (strlen(p->i18n_content[i].content_unchanged) + strlen(symbol_unchanged) + 1) * sizeof(char));
		p->i18n_content[i].content_unchanged = strcat(p->i18n_content[i].content_unchanged, symbol_unchanged);

		p->i18n_content[i].symbol_len = (int *) realloc(p->i18n_content[i].symbol_len, (p->cur_pos + 1) * sizeof(int));
		p->i18n_content[i].symbol_len[p->cur_pos] = strlen(symbol);

		p->i18n_content[i].symbol_len_unchanged = (int *) realloc(p->i18n_content[i].symbol_len_unchanged, (p->cur_pos + 1) * sizeof(int));
		p->i18n_content[i].symbol_len_unchanged[p->cur_pos] = strlen(symbol_unchanged);

		free(symbol);
		free(symbol_unchanged);
	}

	p->cur_pos++;
	p->content[p->cur_pos] = NULLSYM;
}

static void buffer_del_symbol(struct _buffer *p)
{
	if (p->cur_pos == 0)
		return;

	p->cur_pos--;
	p->content[p->cur_pos] = NULLSYM;

	for (int i = 0; i < p->handle->total_languages; i++)
	{
		p->i18n_content[i].content[strlen(p->i18n_content[i].content) - p->i18n_content[i].symbol_len[p->cur_pos]] = NULLSYM;
		p->i18n_content[i].content_unchanged[strlen(p->i18n_content[i].content_unchanged) - p->i18n_content[i].symbol_len_unchanged[p->cur_pos]] = NULLSYM;
	}
}

static char *buffer_get_utf_string(struct _buffer *p)
{
	char *symbol = (char *) malloc((256 + 1) * sizeof(char));

	char *utf_string = (char *) malloc(1 * sizeof(char));
	utf_string[0] = NULLSYM;

	Display *display = XOpenDisplay(NULL);
	XEvent event;
	event.type		= KeyPress;
	event.xkey.type		= KeyPress;
	event.xkey.root		= RootWindow(display, DefaultScreen(display));
	event.xkey.subwindow	= None;
	event.xkey.same_screen	= True;
	event.xkey.display	= display;
	event.xkey.state	= 0;
	event.xkey.keycode	= XKeysymToKeycode(display, XK_space);
	event.xkey.time		= CurrentTime;
	
	for (int i = 0; i < p->cur_pos; i++)
	{
		event.xkey.keycode	= p->keycode[i];
		event.xkey.state	= p->keycode_modifiers[i];

		int nbytes = XLookupString((XKeyEvent *) &event, symbol, 256, NULL, NULL);
		if (nbytes <= 0)
			continue;

		symbol[nbytes] = NULLSYM;

		char *tmp = realloc(utf_string, strlen(utf_string) * sizeof(char) + nbytes + 1);
		if (tmp != NULL)
		{
			utf_string = tmp;
			strcat(utf_string, symbol);
			//free(tmp);
		}
	}

	free(symbol);
	XCloseDisplay(display);
	
	return utf_string;
}

static char *buffer_get_utf_string_on_kbd_group(struct _buffer *p, int group)
{
	char *utf_string = (char *) malloc(1 * sizeof(char));
	utf_string[0] = NULLSYM;
	
	for (int i = 0; i < p->cur_pos; i++)
	{
		int state = p->keycode_modifiers[i];
		for (int j = 0; j < p->handle->total_languages; j++)
		{
			state = state & (~keyboard_groups[j]);
		}
		char *symbol = p->keymap->keycode_to_symbol(p->keymap, p->keycode[i], group, state);
		if (symbol)
		{
			char *tmp = realloc(utf_string, strlen(utf_string) * sizeof(char) + strlen(symbol) + 1);
			if (tmp != NULL)
			{
				utf_string = tmp;
				strcat(utf_string, symbol);	
			}
			free(symbol);
		}
	}
	
	return utf_string;
}		

static void buffer_save_and_clear(struct _buffer *p, Window window)
{
	p->save(p, LOG_NAME, window);
	p->clear(p);
}

static void buffer_set_offset(struct _buffer *p, int offset)
{
	// Shift fields to point to begin of word
	p->content		+= offset;
	p->keycode		+= offset;
	p->keycode_modifiers	+= offset;
	p->cur_pos		-= offset;
}

static void buffer_unset_offset(struct _buffer *p, int offset)
{
	// Revert fields back
	p->content		-= offset;
	p->keycode		-= offset;
	p->keycode_modifiers	-= offset;
	p->cur_pos		+= offset;
}

static void buffer_uninit(struct _buffer *p)
{
	free(p->keycode_modifiers);
	free(p->keycode);
	free(p->content);

	for (int i = 0; i < p->handle->total_languages; i++)
	{
		free(p->i18n_content[i].content);
		free(p->i18n_content[i].symbol_len);
		free(p->i18n_content[i].content_unchanged);
		free(p->i18n_content[i].symbol_len_unchanged);
	}

	free(p->i18n_content);
	free(p);

	log_message(DEBUG, _("String is freed"));
}

struct _buffer* buffer_init(struct _xneur_handle *handle, struct _keymap *keymap)
{
	struct _buffer *p = (struct _buffer *) malloc(sizeof(struct _buffer));
	bzero(p, sizeof(struct _buffer));

	p->handle = handle;

	p->keymap = keymap;
	
	p->cur_size		= INIT_STRING_LENGTH;

	p->content		= (char *) malloc(p->cur_size * sizeof(char));
	p->keycode		= (KeyCode *) malloc(p->cur_size * sizeof(KeyCode));
	p->keycode_modifiers	= (int *) malloc(p->cur_size * sizeof(int));

	bzero(p->content, p->cur_size * sizeof(char));
	bzero(p->keycode, p->cur_size * sizeof(KeyCode));
	bzero(p->keycode_modifiers, p->cur_size * sizeof(int));

	p->i18n_content = (struct _buffer_content *) malloc((handle->total_languages) * sizeof(struct _buffer_content));
	for (int i=0; i<p->handle->total_languages; i++)
	{
		p->i18n_content[i].content = malloc(sizeof(char));
		p->i18n_content[i].content[0] = NULLSYM;
		p->i18n_content[i].symbol_len = malloc(sizeof(int));
		p->i18n_content[i].content_unchanged = malloc(sizeof(char));
		p->i18n_content[i].content_unchanged[0] = NULLSYM;
		p->i18n_content[i].symbol_len_unchanged = malloc(sizeof(int));
	}

	// Functions mapping
	p->clear		= buffer_clear;
	p->save			= buffer_save;
	p->save_and_clear	= buffer_save_and_clear;
	p->is_space_last	= buffer_is_space_last;
	p->set_lang_mask	= buffer_set_lang_mask;
	p->set_uncaps_mask	= buffer_set_uncaps_mask;
	p->set_caps_mask	= buffer_set_caps_mask;
	p->set_content		= buffer_set_content;
	p->change_case		= buffer_change_case;
	p->rotate_layout	= buffer_rotate_layout;
	p->add_symbol		= buffer_add_symbol;
	p->del_symbol		= buffer_del_symbol;
	p->get_utf_string	= buffer_get_utf_string;
	p->get_utf_string_on_kbd_group	= buffer_get_utf_string_on_kbd_group;
	p->set_offset		= buffer_set_offset;
	p->unset_offset		= buffer_unset_offset;
	p->uninit		= buffer_uninit;

	return p;
}
