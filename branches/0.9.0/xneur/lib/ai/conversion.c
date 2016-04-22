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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "types.h"

#include "conversion.h"

static unsigned short rus[] =
{
	// ё й ц у к е н г ш щ з х
	0x91D1, 0xB9D0, 0x86D1, 0x83D1, 0xBAD0, 0xB5D0, 0xBDD0, 0xB3D0, 0x88D1, 0x89D1, 0xB7D0, 0x85D1,
	// ъ ф ы в а п р о л д ж э
	0x8AD1, 0x84D1, 0x8BD1, 0xB2D0, 0xB0D0, 0xBFD0, 0x80D1, 0xBED0, 0xBBD0, 0xB4D0, 0xB6D0, 0x8DD1,
	// я ч с м и т ь б ю Ё Й Ц
	0x8FD1, 0x87D1, 0x81D1, 0xBCD0, 0xB8D0, 0x82D1, 0x8CD1, 0xB1D0, 0x8ED1, 0x81D0, 0x99D0, 0xA6D0,
	// У К Е Н Г Ш Щ З Х Ъ Ф Ы
	0xA3D0, 0x9AD0, 0x95D0, 0x9DD0, 0x93D0, 0xA8D0, 0xA9D0, 0x97D0, 0xA5D0, 0xAAD0, 0xA4D0, 0xABD0,
	// В А П Р О Л Д Ж Э Я Ч С
	0x92D0, 0x90D0, 0x9FD0, 0xA0D0, 0x9ED0, 0x9BD0, 0x94D0, 0x96D0, 0xADD0, 0xAFD0, 0xA7D0, 0xA1D0,
	// М И Т Ь Б Ю
	0x9CD0, 0x98D0, 0xA2D0, 0xACD0, 0x91D0, 0xAED0,
	// №
	0x84E2,
	// ґ, Ґ, є, Є, і, І, ї, Ї
	0x91D2, 0x90D2, 0x94D1, 0x84D0, 0x96D1, 0x86D0, 0x97D1, 0x87D0,
	// ў, Ў
	0x9ED1, 0x8ED0,
	// ?, e, e, c, a, u, °, ?, ?, µ, §
	0xb2c2, 0xa9c3, 0xa8c3, 0xa7c3, 0xa0c3, 0xb9c3, 0xb0c2, 0xa8c2, 0xa3c2, 0xb5c2, 0xa7c2,
	//u, u, o, o, e, e, j, i, i,
	0xbcc3, 0xbbc3, 0xb6c3, 0xb4c3, 0xabc3, 0xaac3, 0xb5c4, 0xafc3, 0xaec3,
	//U, U, O, O, E, E, J, I, I, A,
	0x9cc3, 0x9bc3, 0x96c3, 0x94c3, 0x8bc3, 0x8ac3, 0xb4c4, 0x8fc3, 0x8ec3, 0x80c3
};

static const char *translit[] =
{
	// ё й ц у к е н г ш щ з х
	"yo\0", "y\0", "ts\0", "u\0", "k\0", "e\0", "n\0", "g\0", "sh\0", "sch\0", "z\0", "h\0",
	// ъ ф ы в а п р о л д ж э
	"`\0", "f\0", "y\0", "v\0", "a\0", "p\0", "r\0", "o\0", "l\0", "d\0", "zh\0", "e\0",
	// я ч с м и т ь б ю Ё Й Ц
	"ya\0", "ch\0", "s\0", "m\0", "i\0", "t\0", "\'\0", "b\0", "yu\0", "Yo\0", "Y\0", "Ts\0",
	// У К Е Н Г Ш Щ З Х Ъ Ф Ы
	"U\0", "K\0", "E\0", "N\0", "G\0", "Sh\0", "Sch\0", "Z\0", "H\0", "`\0", "F\0", "Y\0",
	// В А П Р О Л Д Ж Э Я Ч С
	"V\0", "A\0", "P\0", "R\0", "O\0", "L\0", "D\0", "Zh\0", "E\0", "Ya\0", "Ch\0", "S\0",
	// М И Т Ь Б Ю
	"M\0", "I\0", "T\0", "\'\0", "B\0", "Yu\0",
	// №
	"#\0",
	// ґ, Ґ, є, Є, і, І, ї, Ї
	"?\0", "?\0", "e\0", "E\0", "i\0", "I\0", "i\0", "I\0",
	// ў, Ў
	"?\0", "?\0",
	// ?, e, e, c, a, u, °, ?, ?, µ, §
	"?\0", "?\0", "?\0", "?\0", "?\0", "?\0", "?\0", "?\0", "?\0", "?\0", "?\0",
	//u, u, o, o, e, e, j, i, i,
	"?\0", "?\0", "?\0", "?\0", "?\0", "?\0", "?\0", "?\0", "?\0",
	//U, U, O, O, E, E, J, I, I, A,
	"?\0", "?\0", "?\0", "?\0", "?\0", "?\0", "?\0", "?\0", "?\0", "?\0"
};

static const int rus_len = sizeof(rus) / sizeof(rus[0]);

static const char* get_translit(const char *sym)
{
	for (int i = 0; i < rus_len; i++)
	{
		unsigned short usym = *(unsigned short*) sym;
		if (rus[i] == usym)
			return translit[i];
	}
	return NULLSYM;
}

void convert_text_to_translit(char **work_text)
{
	char *text = *work_text;
	int j = 0, len = strlen(text);

	char *trans_text = (char *) malloc((len * 3 + 1) * sizeof(char));

	for (int i = 0; i < len; i++)
	{
		if (isascii(text[i]))
		{
			trans_text[j++] = text[i];
			continue;
		}

		const char *new_symbol = get_translit(&text[i]);

		for(; i < len - 1; i++)
		{
			if (isascii(text[i + 1]))
				break;
			if (get_translit(&text[i + 1]) != NULLSYM)
				break;
		}

		while (*new_symbol != NULLSYM)
		{
			trans_text[j++] = *new_symbol;
			new_symbol++;
		}
	}

	trans_text[j] = NULLSYM;

	free(*work_text);
	*work_text = trans_text; 
}
