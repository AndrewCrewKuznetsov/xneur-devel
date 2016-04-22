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
 *  Copyright (C) 2006-2008 XNeur Team
 *
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "xcursor.h"

#include "log.h"
#include "types.h"

#ifdef WITH_XPM

#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <X11/XKBlib.h>

#include <unistd.h>
#include <stdio.h>

#include "xnconfig_files.h"

#include "xwindow.h"

extern struct _xneur_config *xconfig;
extern struct _xwindow *main_window;

static void unmap_window(XWindowAttributes w_attributes)
{
	if (w_attributes.map_state != IsUnmapped)
		XUnmapWindow(main_window->display, main_window->flag_window);
	XFlush(main_window->display);
}

static int ReadPixmapFromFile(char *FileName, Pixmap *phPm, Pixmap *phMask, XpmAttributes *pAttrs, Display *pDisplay, Window hWnd, Colormap hColorMap)
{
	FILE *pFile = fopen(FileName, "r");
	if (!pFile)
		return -1;

	fseek(pFile, 0, SEEK_END);
	long sizeFile = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char *buffer = (char *) malloc(sizeFile);
	if (!buffer)
	{
		fclose(pFile);
		return -1;
	}

	fread(buffer, 1, sizeFile, pFile);
	fclose(pFile);

	// Check Signature
	if (strncmp(buffer, "/* XPM */", 9))
	{
		free(buffer);
		return -1;
	}

	long N = 0;
	for (long i = 0; i < sizeFile; i++)
		N += (buffer[i] == '"' ? 1 : 0);
	N /= 2;

	char **xpm = (char **) malloc(N * sizeof(char *));
	for (long k = 0, i = 0; i < sizeFile; i++)
	{
		if (buffer[i] != '"')
			continue;

		xpm[k++] = buffer + ++i;
		while (buffer[i] != '"')
			i++;
		buffer[i] = '\0';
	}

	pAttrs->valuemask	= XpmColormap | XpmCloseness;
	pAttrs->colormap	= hColorMap;
	pAttrs->closeness	= 65535;
	XpmCreatePixmapFromData(pDisplay, hWnd, xpm, phPm, phMask, pAttrs);

	free(xpm);
	free(buffer);
	return 0;
}

static GC create_gc(Display* display, Window win, int reverse_video)
{
	XGCValues values;			// Initial values for the GC
	unsigned long valuemask = 0;		// Which values in 'values' to check when creating the GC

	GC gc = XCreateGC(display, win, valuemask, &values);
	if (gc < 0)
	{
		fprintf(stderr, "XCreateGC: \n");
		return NULL;
	}

	int screen_num = DefaultScreen(display);

	// Allocate foreground and background colors for this GC
	if (reverse_video)
	{
		XSetForeground(display, gc, WhitePixel(display, screen_num));
		XSetBackground(display, gc, BlackPixel(display, screen_num));
	}
	else
	{
		XSetForeground(display, gc, BlackPixel(display, screen_num));
		XSetBackground(display, gc, WhitePixel(display, screen_num));
	}

	int line_width	= 2;			// Line width for the GC
	int line_style	= LineSolid;		// Style for lines drawing
	int cap_style	= CapButt;		// Style of the line's edje
	int join_style	= JoinBevel;		// Joined lines

	// Define the style of lines that will be drawn using this GC
	XSetLineAttributes(display, gc, (unsigned int) line_width, line_style, cap_style, join_style);

	// Define the fill style for the GC to be 'solid filling'
	XSetFillStyle(display, gc, FillSolid);

	return gc;
}

void xcursor_show_flag(struct _xcursor *p, int x, int y)
{
	XWindowAttributes w_attributes;
	XGetWindowAttributes(main_window->display, main_window->flag_window, &w_attributes);

	XkbStateRec xkbState;
	XkbGetState(main_window->display, XkbUseCoreKbd, &xkbState);

	// if for layout not defined xpm file then unmap window and stop procedure
	if (xconfig->flags[xkbState.group].file[0] == NULLSYM)
	{
		unmap_window(w_attributes);
		return;
	}

	// if current layout not equal last layout then load new pixmap
	if (p->last_layuot != xkbState.group)
	{
		p->last_layuot = xkbState.group;

		XFreePixmap(main_window->display, p->bitmap);
		XFreePixmap(main_window->display, p->bitmap_mask);

		p->bitmap = 0;
		p->bitmap_mask = 0;

		char *path = get_file_path_name(PIXMAPDIR, xconfig->flags[xkbState.group].file);
		if (path == NULL)
		{
			unmap_window(w_attributes);
			return;
		}

		ReadPixmapFromFile(path, &p->bitmap, &p->bitmap_mask, &p->Attrs, main_window->display, main_window->flag_window, XDefaultColormap(main_window->display, DefaultScreen(main_window->display)));
		free(path);

		if (p->bitmap == 0)
		{
			unmap_window(w_attributes);
			return;
		}
	}

	// For set pixmap to window, need map window
	if (w_attributes.map_state == IsUnmapped)
		XMapWindow(main_window->display, main_window->flag_window);

	XSetClipMask(main_window->display, p->gc, p->bitmap_mask);
	XSetClipOrigin(main_window->display, p->gc, 0, 0);
	XCopyArea(main_window->display, p->bitmap, main_window->flag_window, p->gc, 0, 0, p->Attrs.width, p->Attrs.height, 0, 0);

	XRaiseWindow(main_window->display, main_window->flag_window);
	XMoveResizeWindow(main_window->display, main_window->flag_window, x, y, p->Attrs.width, p->Attrs.height);

	XFlush(main_window->display);
}

void xcursor_hide_flag(struct _xcursor *p)
{
	XWindowAttributes w_attributes;
	XGetWindowAttributes(main_window->display, main_window->flag_window, &w_attributes);

	unmap_window(w_attributes);
}

void xcursor_uninit(struct _xcursor *p)
{
	Display *dpy = XOpenDisplay(NULL);

	XFreePixmap(dpy, p->bitmap);
	XFreePixmap(dpy, p->bitmap_mask);

	XFreeGC(dpy, p->gc);
	XCloseDisplay(dpy);

	free(p);

	log_message(DEBUG, "Current cursor is freed");
}

struct _xcursor* xcursor_init(void)
{
	struct _xcursor *p = (struct _xcursor *) malloc(sizeof(struct _xcursor));
	bzero(p, sizeof(struct _xcursor));

	int dummy;
	p->gc = create_gc(main_window->display, main_window->flag_window, dummy);
	if (p->gc == NULL)
	{
		free(p);
		return NULL;
	}

	XSync(main_window->display, False);

	p->last_layuot = -1;

	// Functions mapping
	p->show_flag	= xcursor_show_flag;
	p->hide_flag	= xcursor_hide_flag;
	p->uninit	= xcursor_uninit;

	return p;
}


#else /* WITH_XPM */

void xcursor_load_pixmaps(struct _xcursor *p)
{
	return;
}

void xcursor_show_flag(struct _xcursor *p, int x, int y)
{
	return;
}

void xcursor_hide_flag(struct _xcursor *p)
{
	return;
}

void xcursor_uninit(struct _xcursor *p)
{
	free(p);

	log_message(DEBUG, "Current cursor is freed");
}

struct _xcursor* xcursor_init(void)
{
	struct _xcursor *p = (struct _xcursor *) malloc(sizeof(struct _xcursor));
	bzero(p, sizeof(struct _xcursor));

	// Functions mapping
	p->show_flag	= xcursor_show_flag;
	p->hide_flag	= xcursor_hide_flag;
	p->uninit	= xcursor_uninit;

	return p;
}

#endif /* WITH_XPM */
