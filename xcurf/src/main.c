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
 *  Copyright (C) 2006-2009 XNeur Team
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <X11/XKBlib.h>
#include <X11/Xutil.h>

#include <Imlib2.h>

#define TRUE 1
#define FALSE 0

#define MWM_HINTS_DECORATIONS   (1L << 1)
#define PROP_MWM_HINTS_ELEMENTS 5

#define HOME_CONF_DIR		".xcurf"
#define DEFAULT_MAX_PATH	4096
#define DIR_SEPARATOR		"/"

typedef struct {
	int flags;
	int functions;
	int decorations;
	int input_mode;
	int status;
} MWMHints;

Display *display;

Window parent_window;
Window flag_window;

int last_xkb_group = -1;

Visual  *vis;
Colormap cm;
int depth;

Imlib_Image image, buffer;
Imlib_Updates updates, current_update;
int w = 0, h = 0;

char *pixmaps[4];

typedef void (*sg_handler)(int);
void signal_trap(int sig, sg_handler handler)
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handler;

	if (sigaction(sig, &sa, NULL) == -1)
	{
		printf("Can't trap signal\n");
		exit(1);
	}
}

static void xcf_exit (int status)
{
	if (status) {};

	printf("\nCaught SIGTERM/SIGINT, terminating\n");
	exit(0);
}

void set_event_mask(Window window, int event_mask)
{
	if (window == flag_window)
		return;

	XSelectInput(display, window, event_mask);
}

static void set_mask_to_window(Window current_window, int mask)
{
	if (current_window == None)
		return;

	set_event_mask(current_window, mask);

	unsigned int children_count;
	Window root, parent;
	Window *children;

	int is_same_screen = XQueryTree(display, current_window, &root, &parent, &children, &children_count);
	if (!is_same_screen)
		return;

	unsigned int i;
	for (i = 0; i < children_count; i++)
		set_mask_to_window(children[i], mask);

	XFree(children);
}

static void update_focus (void)
{
	Window new_window;
	while (TRUE)
	{
		// Wait for new window was focused.
		usleep(500000);

		int revert_to;
		XGetInputFocus(display, &new_window, &revert_to);

		// Catch not empty and not system window
		if (new_window != None && new_window > 1000)
			break;

		printf("New window empty\n");
		usleep(100000);
	}

	if (parent_window == new_window)
		return;
	printf("Focused window %d\n", (int) new_window);

	// Up to heighted window
	parent_window = new_window;
	while (TRUE)
	{
		unsigned int children_count;
		Window root, parent;
		Window *children;

		int is_same_screen = XQueryTree(display, parent_window, &root, &parent, &children, &children_count);
		if (children != NULL)
			XFree(children);
		if (!is_same_screen || parent == None || parent == root)
			break;

		parent_window = parent;
	}

	set_mask_to_window(parent_window, PointerMotionMask | PropertyChangeMask | FocusChangeMask | KeyReleaseMask);
}

static void cursor_hide(XWindowAttributes w_attributes)
{
	if (w_attributes.map_state != IsUnmapped)
		XUnmapWindow(display, flag_window);
	XFlush(display);
}

void cursor_show(void)
{
	XWindowAttributes w_attributes;
	XGetWindowAttributes(display, flag_window, &w_attributes);

	// For set pixmap to window, need map window
	if (w_attributes.map_state == IsUnmapped)
		XMapWindow(display, flag_window);

	XkbStateRec xkbState;
	XkbGetState(display, XkbUseCoreKbd, &xkbState);

	// if for layout not defined xpm file then unmap window and stop procedure
	if (pixmaps[xkbState.group] == NULL)
	{
		cursor_hide(w_attributes);
		return;
	}

	// if current layout not equal last layout then load new pixmap
	if (last_xkb_group != xkbState.group)
	{
		printf("Group %d\n", xkbState.group);
		last_xkb_group = xkbState.group;

		if (pixmaps[xkbState.group] == NULL)
		{
			cursor_hide(w_attributes);
			return;
		}

		image = imlib_load_image(pixmaps[xkbState.group]);

		if (image == NULL)
		{
			cursor_hide(w_attributes);
			return;
		}

		imlib_context_set_image(image);
		w = imlib_image_get_width();
		h = imlib_image_get_height();
		XResizeWindow(display, flag_window, w, h);

	}

	imlib_render_image_on_drawable_at_size(0, 0, w, h);
	XRaiseWindow(display, flag_window);

	int root_x, root_y, win_x, win_y;
	Window root, child;
	unsigned int dummyU;
	XQueryPointer(display, parent_window, &root, &child, &root_x, &root_y, &win_x, &win_y, &dummyU);

	XMoveWindow(display, flag_window, root_x + 10, root_y + 10);

	XFlush(display);
}

static int get_max_path_len(void)
{
	int max_path_len = pathconf(PACKAGE_SYSCONFDIR_DIR, _PC_PATH_MAX);
	if (max_path_len <= 0)
		return DEFAULT_MAX_PATH;
	return max_path_len;
}

char* get_file_path_name(const char *dir_name, const char *file_name)
{
	#define SEARCH_IN(DIRECTORY) \
	if (dir_name == NULL)\
		snprintf(path_file, max_path_len, "%s/%s", DIRECTORY, file_name);\
	else\
		snprintf(path_file, max_path_len, "%s/%s/%s", DIRECTORY, dir_name, file_name);\
	stream = fopen(path_file, "r");\
	if (stream != NULL)\
	{\
		fclose(stream);\
		return path_file;\
	}

	int max_path_len = get_max_path_len();

	char *path_file = (char *) malloc((max_path_len + 1) * sizeof(char));

	// Search by only full path
	strncpy(path_file, file_name, max_path_len);

	FILE *stream = fopen(path_file, "r");
	if (stream != NULL)
	{
		fclose(stream);
		return path_file;
	}

	// Search conf in ~/.xneur
	if (dir_name == NULL)
		snprintf(path_file, max_path_len, "%s/%s/%s", getenv("HOME"), HOME_CONF_DIR, file_name);
	else
		snprintf(path_file, max_path_len, "%s/%s/%s/%s", getenv("HOME"), HOME_CONF_DIR, dir_name, file_name);

	stream = fopen(path_file, "r");
	if (stream != NULL)
	{
		fclose(stream);
		return path_file;
	}

	SEARCH_IN(PACKAGE_SYSCONFDIR_DIR);
	SEARCH_IN("/etc/xneur");
	SEARCH_IN(PACKAGE_SHAREDIR_DIR);

	return NULL;
}

int main(int argc, char *argv[])
{
	// Set hook to terminate
	signal_trap(SIGTERM, xcf_exit);
	signal_trap(SIGINT, xcf_exit);

	// Init pixmaps
	int i;
	for (i=0; i<4; i++)
		pixmaps[i] = NULL;

	XkbDescRec *kbd_desc_ptr = XkbAllocKeyboard();
	if (kbd_desc_ptr == NULL)
		return (1);

	display = XOpenDisplay(NULL);
	XkbGetNames(display, XkbAllNamesMask, kbd_desc_ptr);

	if (kbd_desc_ptr->names == NULL)
	{
		XCloseDisplay(display);
		return (1);
	}

	int groups_count = 0;
	for (; groups_count < XkbNumKbdGroups; groups_count++)
	{
		if (kbd_desc_ptr->names->groups[groups_count] == None)
			break;
	}

	if (groups_count == 0)
	{
		XCloseDisplay(display);
		return (1);
	}

	Atom symbols_atom = kbd_desc_ptr->names->symbols;
	char *symbols	= XGetAtomName(display, symbols_atom);
	char *tmp_symbols = symbols;
	strsep(&tmp_symbols, "+");

	int group = 0;
	for (group = 0; group < groups_count; group++)
	{
		Atom group_atom = kbd_desc_ptr->names->groups[group];

		if (group_atom == None)
			continue;

		char *short_name = strsep(&tmp_symbols, "+");
		short_name[2] = '\0';

		pixmaps[group] = malloc((2 /*language code*/ + 4 /*.png*/ +1 /*\0*/) * sizeof(char));
		sprintf(pixmaps[group], "%s.png", short_name);
		pixmaps[group] = get_file_path_name("pixmaps", pixmaps[group]);
		printf("%s\n", pixmaps[group]);
	}

	free(symbols);

	// Open display
	display = XOpenDisplay(NULL);
	if (!display)
	{
		printf("Can't connect to XServer\n");
		return (1);
	}

	// Init main window
	Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, 0, 0);
	if (!window)
	{
		printf("Can't create program window\n");
		XCloseDisplay(display);
		return (1);
	}

	// Create flag window
	XSetWindowAttributes attrs;
	attrs.override_redirect = True;

	flag_window = XCreateWindow(display, DefaultRootWindow(display), 0, 0, 1, 1,0, CopyFromParent, CopyFromParent, CopyFromParent, CWOverrideRedirect, &attrs);
	if (!flag_window)
	{
		printf("Can't create flag window\n");
		XCloseDisplay(display);
		return (1);
	}

	// Set no border mode to flag window
	MWMHints mwmhints;
	memset(&mwmhints, 0, sizeof(mwmhints));
	mwmhints.flags		= MWM_HINTS_DECORATIONS;
	mwmhints.decorations	= 0;

	Atom motif_prop = XInternAtom(display, "_MOTIF_WM_HINTS", False);

	XChangeProperty(display, flag_window, motif_prop, motif_prop, 32, PropModeReplace, (unsigned char *) &mwmhints, PROP_MWM_HINTS_ELEMENTS);

	XWMHints wmhints;
	memset(&wmhints, 0, sizeof(wmhints));
	wmhints.flags = InputHint;
	wmhints.input = 0;

	Atom win_prop = XInternAtom(display, "_WIN_HINTS", False);

	XChangeProperty(display, flag_window, win_prop, win_prop, 32, PropModeReplace, (unsigned char *) &mwmhints, sizeof (XWMHints) / 4);

	printf("Main window with id %d created\n", (int) window);

	// Init image environment
	vis   = DefaultVisual(display, DefaultScreen(display));
	cm    = DefaultColormap(display, DefaultScreen(display));

	imlib_set_cache_size(2048 * 1024);

	imlib_context_set_display(display);
	imlib_context_set_visual(vis);
	imlib_context_set_colormap(cm);
	imlib_context_set_drawable(flag_window);

	imlib_context_set_dither(1);

	//
	XSynchronize(display, TRUE);
	XFlush(display);

	update_focus();
	XEvent e;
	int k = 0;
	while (TRUE)
	{
		XNextEvent(display, &e);
		switch (e.type)
		{
			//case LeaveNotify:
			//case EnterNotify:
			case FocusIn:
			case FocusOut:
			{
				update_focus();
				cursor_show();
				break;
			}
			case MotionNotify:
			{
				k++;

				printf("Motion %d\n",k);
				cursor_show();
				break;
			}
			case KeyRelease:
			{

				cursor_show();
				break;
			}
		}
	}

	return (0);
}
