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
 *  Copyright (C) 2006-2010 XNeur Team
 *
 */
 

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "xkb.h"

int isXkbLayoutSymbol(char *symbol)
{
	if ((strcmp(symbol, "group") == 0) || 
	    (strcmp(symbol, "inet") == 0) ||
	    (strcmp(symbol, "pc") == 0))
	{
		return 0;
	}
	return 1;
}
void XkbSymbolParse(const char *symbols, struct _xkb_name *symbolList)
{
    int inSymbol = 0;
	
    char *curSymbol = (char *) malloc (sizeof(char));
    char *curVariant = (char *) malloc (sizeof(char));
	curSymbol[0] = '\0';
	curVariant[0] = '\0';
	
	int curLayout = 0;
	
	//printf("%s", symbols);
	// A sample line:
	// pc+fi(dvorak)+fi:2+ru:3+inet(evdev)+group(menu_toggle)
    
	for (size_t i = 0; i < strlen(symbols); i++) 
	{
		char ch = symbols[i];
		if (ch == '+' || ch == '_') 
		{
			if (inSymbol) 
			{
				if (isXkbLayoutSymbol(curSymbol)) 
				{
					symbolList[curLayout].symbol = (char *)strdup(curSymbol);
					symbolList[curLayout].variant = (char *)strdup(curVariant);
					curLayout++;
                }
				curSymbol = (char *) realloc (curSymbol, sizeof(char));
				curVariant = (char *) realloc (curVariant, sizeof(char));
				curSymbol[0] = '\0';
				curVariant[0] = '\0';
			} 
			else 
			{
				inSymbol = 1;
			}
		} 
		else if (inSymbol && (isalpha(ch) || ch == '_')) 
		{
			int len = strlen(curSymbol);
			curSymbol = (char *) realloc (curSymbol, sizeof(char)*(len+2));
			curSymbol[len] = ch;
			curSymbol[len+1] = '\0';
		} 
		else if (inSymbol && ch == '(') 
		{
			while (++i < strlen(symbols)) 
			{
				ch = symbols[i];
				if (ch == ')')
				{
					break;
				}
				else
				{
					int len = strlen(curVariant);
					curVariant = (char *) realloc (curVariant, sizeof(char)*(len+2));
					curVariant[len] = ch;
					curVariant[len+1] = '\0';
				}
			}
		} 
		else 
		{
			if (inSymbol) 
			{
				if (isXkbLayoutSymbol(curSymbol)) 
				{
					symbolList[curLayout].symbol = (char *)strdup(curSymbol);
					symbolList[curLayout].variant = (char *)strdup(curVariant);
					curLayout++;
				}
				curSymbol = (char *) realloc (curSymbol, sizeof(char));
				curVariant = (char *) realloc (curVariant, sizeof(char));
				curSymbol[0] = '\0';
				curVariant[0] = '\0';
				inSymbol = 0;
			}
		}
	}
	if (curSymbol != NULL)
		free(curSymbol);
	if (curVariant != NULL)
		free(curVariant);
}

char *get_active_kbd_symbol(Display *dpy)
{
	if (dpy == NULL)
		return NULL;

	XkbDescRec desc[1];
	memset(desc, 0, sizeof(desc));
    desc->device_spec = XkbUseCoreKbd;
    XkbGetControls(dpy, XkbGroupsWrapMask, desc);
	XkbGetNames(dpy, XkbGroupNamesMask, desc);
	XkbGetNames(dpy, XkbSymbolsNameMask, desc);
	
	struct _xkb_name *xkb_names = (struct _xkb_name *) malloc(sizeof(struct _xkb_name) * desc->ctrls->num_groups);
	
	XkbSymbolParse(XGetAtomName(dpy, desc->names->symbols), xkb_names);
	//printf("gxneur active group symbol: %s, variant: %s\n",xkb_names[get_active_kbd_group(dpy)].symbol,xkb_names[get_active_kbd_group(dpy)].variant);
	char *symbol = strdup(xkb_names[get_active_kbd_group(dpy)].symbol);
	
	for (int i = 0; i < desc->ctrls->num_groups; i++)
	{
		free(xkb_names[i].symbol);
		free(xkb_names[i].variant);
	}
	XkbFreeControls(desc, XkbGroupsWrapMask, True);
    XkbFreeNames(desc, XkbSymbolsNameMask, True);
	XkbFreeNames(desc, XkbGroupNamesMask, True);
	return symbol;
}

int get_active_kbd_group(Display *dpy)
{
	if (dpy == NULL)
		return -1;
	XkbStateRec xkbState;
	XkbGetState(dpy, XkbUseCoreKbd, &xkbState);

	return xkbState.group;
}

int get_kbd_group_count(Display *dpy) 
{
	if (dpy == NULL)
		return -1;

    XkbDescRec desc[1];
    int gc;
    memset(desc, 0, sizeof(desc));
    desc->device_spec = XkbUseCoreKbd;
    XkbGetControls(dpy, XkbGroupsWrapMask, desc);
    XkbGetNames(dpy, XkbGroupNamesMask, desc);
	XkbGetNames(dpy, XkbSymbolsNameMask, desc);
	gc = desc->ctrls->num_groups;
    XkbFreeControls(desc, XkbGroupsWrapMask, True);
    XkbFreeNames(desc, XkbGroupNamesMask, True);

    return gc;
}

int set_next_kbd_group(Display *dpy)
{
	if (dpy == NULL)
		return -1;

	int active_layout_group = get_active_kbd_group(dpy);
	
	int new_layout_group = active_layout_group + 1;
	if (new_layout_group == get_kbd_group_count(dpy))
		new_layout_group = 0;

	XkbLockGroup(dpy, XkbUseCoreKbd, new_layout_group);

	return 1;
}
