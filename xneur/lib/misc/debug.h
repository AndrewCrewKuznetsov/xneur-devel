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
 *  Copyright (C) 2006-2012 XNeur Team
 *
 */

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#ifdef WITH_DEBUG

#ifndef _DEBUG_H_
#define _DEBUG_H_

#undef malloc
#undef calloc
#undef free
#undef strdup
#undef realloc

#define malloc(len)		xndebug_malloc((len), __FILE__, __LINE__)
#define calloc(n, size)		xndebug_calloc((n), (size), __FILE__, __LINE__)
#define free(mem)		xndebug_free((mem), __FILE__, __LINE__)
#define strdup(str)		xndebug_strdup((str), __FILE__, __LINE__)
#define realloc(mem, len)	xndebug_realloc((mem), (len), __FILE__, __LINE__)

void  xndebug_init(void);
void* xndebug_malloc(int len, char *file, int line);
char* xndebug_calloc(int n, int size, char *file, int line);
void  xndebug_free(void *mem, char *file, int line);
char* xndebug_strdup(const char *str, char *file, int line);
void* xndebug_realloc(void *mem, int len, char *file, int line);
void  xndebug_uninit(void);

#endif /* _DEBUG_H_ */

#endif /* WITH_DEBUG */
