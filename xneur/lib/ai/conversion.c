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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "types.h"
#include "log.h"

#include "conversion.h"

static unsigned short UTF8_CODES[] =
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
	// ѓ, Ѓ, ј, Ј, ќ, Ќ, љ, Љ
	0x93D1, 0x83D0, 0x98D1, 0x88D0, 0x9CD1, 0x8CD0, 0x99D1, 0x89D0,
	// њ, Њ, џ, Џ, ѣ, Ѣ, ѳ, Ѳ
	0x9AD1, 0x8AD0, 0x9FD1, 0x8FD0, 0xA3D1, 0xA2D1, 0xB3D1, 0xB2D1
};

static const char *translit[] =
{
	// ё й ц у к е н г ш щ з х
	"yo", "y", "ts", "u", "k", "e", "n", "g", "sh", "sch", "z", "h",
	// ъ ф ы в а п р о л д ж э
	"``", "f", "y`", "v", "a", "p", "r", "o", "l", "d", "zh", "e`",
	// я ч с м и т ь б ю Ё Й Ц
	"ya", "ch", "s", "m", "i", "t", "\'", "b", "yu", "Yo", "Y", "Ts",
	// У К Е Н Г Ш Щ З Х Ъ Ф Ы
	"U", "K", "E", "N", "G", "Sh", "Sch", "Z", "H", "``", "F", "Y`",
	// В А П Р О Л Д Ж Э Я Ч С
	"V", "A", "P", "R", "O", "L", "D", "Zh", "E`", "Ya", "Ch", "S",
	// М И Т Ь Б Ю
	"M", "I", "T", "\'", "B", "Yu",
	// №
	"#",
	// ґ, Ґ, є, Є, і, І, ї, Ї
	"g`", "G`", "ye", "Ye", "i", "I", "yi", "Yi",
	// ў, Ў
	"u`", "U`",
	// ѓ, Ѓ, ј, Ј, ќ, Ќ, љ, Љ
	"g`", "G`", "j", "J", "k`", "K`", "l`", "L`",
	// њ, Њ, џ, Џ, ѣ, Ѣ, ѳ, Ѳ
	"n`", "N`", "dh", "Dh", "ye", "Ye", "fh", "Fh"
};


static const int codes_len = sizeof(UTF8_CODES) / sizeof(UTF8_CODES[0]);

static const char* get_translit(const char *sym)
{
	for (int i = 0; i < codes_len; i++)
	{
		unsigned short usym = *(unsigned short*) sym;
		if (UTF8_CODES[i] == usym)
			return translit[i];
	}
	return NULL;
}

/*static char* get_revert_translit(const char *sym, unsigned int len)
{
	for (int i = 0; i < codes_len; i++)
	{
		if (strlen(translit[i]) != len)
			continue;
		if (strcmp(translit[i], sym) == 0)
		{
			//log_message (ERROR, "%s", (char*)&UTF8_CODES[i]);
			char* tmp = malloc (sizeof(unsigned short) + 1);
			memset(tmp, NULLSYM, sizeof(unsigned short) + 1);
			strncpy(tmp, (char*)UTF8_CODES+i*sizeof(UTF8_CODES[0]), sizeof(unsigned short));
			//tmp[sizeof(UTF8_CODES[0])] = NULLSYM;
			log_message (ERROR, "----%s-----", tmp);
			free(tmp);
			return (char*)UTF8_CODES+i;
		}
	}
	return NULLSYM;
}*/

void convert_text_to_translit(char **work_text)
{
	const char *text = *work_text;
	size_t j = 0, len = strlen(text);

	const size_t TRANSLIT_CHAR_SIZE = sizeof(translit[0])-1;// -1 for \0
	char *trans_text = (char *) malloc((len * TRANSLIT_CHAR_SIZE + 1) * sizeof(char));

	for (size_t i = 0; i < len; ++i)
	{
		if (isascii(text[i]))
		{
			// Revert translit here
			/*for (int sylllen = 3; sylllen > 0; sylllen--)
			{
				if (i + sylllen > len)
					continue;
				char *syll = malloc ((sylllen+ 1) * sizeof(char));
				strncpy(syll, (const char*) text+i, sylllen);
				log_message (ERROR, "%s", syll);
				get_revert_translit(syll, sylllen);
				//log_message (ERROR, "%s - %s", syll, get_revert_translit(syll, sylllen));
				free(syll);
			}*/

			// Without revert translit here
			trans_text[j++] = text[i];

			continue;
		}

		const char *new_symbol = get_translit(&text[i]);

		for(; i < len - 1; i++)
		{
			if (isascii(text[i + 1]))
				break;
			if (get_translit(&text[i + 1]) != NULL)
				break;
		}

		while (*new_symbol != NULL)
		{
			trans_text[j++] = *new_symbol;
			new_symbol++;
		}
	}

	trans_text[j] = '\0';

	free(*work_text);
	*work_text = trans_text;
}
