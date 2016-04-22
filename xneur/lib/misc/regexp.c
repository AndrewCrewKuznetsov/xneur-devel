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
#  include "config.h"
#endif

#include <pcre.h>

#include <string.h>
#include <stdio.h>

#include "types.h"
#include "log.h"

int check_regexp_match(const char *str, const char *pattern)
{
	int options = PCRE_UTF8;
	const char *error;
	int erroffset;

	const unsigned char *tables = pcre_maketables();
	pcre *re = pcre_compile(pattern, options, &error, &erroffset, tables);
	if (!re)
	{
		log_message(ERROR, _("Can't compile regular expression '%s'"), pattern);
		return FALSE;
	}

	int str_len = strlen(str);

	int ovector[50];
	int count = pcre_exec(re, NULL, str, str_len, 0, 0, ovector, 50);
	if (count <= 0 && count != PCRE_ERROR_NOMATCH)
	{
		log_message(ERROR, _("Can't exec regular expression '%s', eror code %d"), pattern, count);
		pcre_free(re);
		pcre_free((void*)tables);
		return FALSE;
	}

	pcre_free(re);
	pcre_free((void*)tables);
	
	if (count == PCRE_ERROR_NOMATCH)
		return FALSE;
	
	const char *pcre_string = NULL;
	if(pcre_get_substring(str, ovector, count, 0, &pcre_string) < 0)
		return FALSE;

	//log_message(TRACE, _("Match word '%s' and PERL pattern '%s'"), str, pattern);
	
	pcre_free_substring(pcre_string);
		
	return TRUE;
}
