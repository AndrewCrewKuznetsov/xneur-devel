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

#ifndef _TEXT_H_
#define _TEXT_H_

int   is_upper_non_alpha_cyr(char symbol);
int   get_last_word_offset(const char *string, int string_len);
char* get_last_word(char *string);
int   trim_word(char *word, int len);
void  lower_word_inplace(char *word);
char* lower_word(const char *word, int len);

#endif /* _TEXT_H_ */
