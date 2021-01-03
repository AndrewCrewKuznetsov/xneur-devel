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
 *  Copyright (C) 2006-2016 XNeur Team
 *
 */

#ifndef _TEXT_H_
#define _TEXT_H_

#include <stddef.h>

int   is_upper_non_alpha_cyr(char symbol);
//int   get_last_word_offset(const char *string, int string_len);
//char* get_last_word(char *string);
int   trim_word(char *word, int len);
void  lower_word_inplace(char *word);
char* lower_word(const char *word, int len);
char* str_replace(const char *source, const char *search, const char *replace);
char* real_sym_to_escaped_sym(const char *source);
char* escaped_sym_to_real_sym(const char *source);
void  del_final_numeric_char(char *word);
size_t levenshtein(const char *s, const char *t);

#endif /* _TEXT_H_ */
