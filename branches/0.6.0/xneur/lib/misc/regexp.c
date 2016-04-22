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

#include <pcre.h>
#include <string.h>

#include "types.h"

int check_regexp_match(const char *str, const char *pattern)
{
	int options = 0;
	const char *error;
	int erroffset;

	pcre *re = pcre_compile((char *)pattern, options, &error, &erroffset, NULL);
	if (!re)
		return FALSE;
	
	int ovector[2];
	int count = pcre_exec(re, NULL, (char *)str, strlen(str), 0, 0, ovector, 2);
	pcre_free(re);
	
	if (count <= 0)
		return FALSE;
	
	if (ovector[0] != 0 || ovector[1] != strlen(str))
		return FALSE;

	return TRUE;			
}
