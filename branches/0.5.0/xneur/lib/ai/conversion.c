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

#include <X11/keysym.h>

#include <string.h>
#include <ctype.h>

#include "types.h"
#include "utils.h"
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
	0x91D2, 0x90D2, 0x94D1, 0x84D0, 0x96D1, 0x86D0, 0x97D1, 0x87D1,
	// ў, Ў
	0x9ED1, 0x8ED0
};

static const char eng[] =
{
	'`', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',
	']', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
	'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '~', 'Q', 'W',
	'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', 'Z', 'X', 'C',
	'V', 'B', 'N', 'M', '<', '>', 
	'#', 
	'\\', '|', '\'', '"', 's', 'S', ']', '}',
	'o', 'O'
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
	"?\0", "?\0"
};

static const int eng_len = sizeof(eng) / sizeof(eng[0]);

static const KeySym ksym[] =
{
	// Shift Numeric 14
	XK_asciitilde, XK_exclam, XK_at, XK_numbersign, XK_dollar,
	XK_percent, XK_asciicircum, XK_ampersand, XK_asterisk, XK_parenleft, 
	XK_parenright, XK_underscore, XK_plus, XK_bar,

	// Numeric 14  
	XK_grave, XK_1, XK_2, XK_3, XK_4, 
	XK_5, XK_6, XK_7, XK_8, XK_9, 
	XK_0, XK_minus, XK_equal, XK_backslash,  

	// Shift Character 33 	  
	XK_Q, XK_W, XK_E, XK_R, 
	XK_T, XK_Y, XK_U, XK_I, 
	XK_O, XK_P, XK_braceleft, XK_braceright,
	XK_A, XK_S, XK_D, XK_F,
	XK_G, XK_H, XK_J, XK_K,
	XK_L, XK_colon, XK_quotedbl, XK_Z,
	XK_X, XK_C, XK_V, XK_B,
	XK_N, XK_M, XK_less, XK_greater, 
	XK_question,	  

	// Character 33
	XK_q, XK_w, XK_e, XK_r, 
	XK_t, XK_y, XK_u, XK_i, 
	XK_o, XK_p, XK_bracketleft, XK_bracketright,
	XK_a, XK_s, XK_d, XK_f,
	XK_g, XK_h, XK_j, XK_k,
	XK_l, XK_semicolon, XK_apostrophe, XK_z,
	XK_x, XK_c, XK_v, XK_b,
	XK_n, XK_m, XK_comma, XK_period, 
	XK_slash,	

	// Special Character 3
	XK_space, XK_Tab, XK_Return,
	
	// KeyPad Character 16
	XK_KP_Divide, XK_KP_Multiply, XK_KP_Add, XK_KP_Subtract,
	XK_KP_Home, XK_KP_Up, XK_KP_Page_Up,
	XK_KP_Left, XK_KP_Begin, XK_KP_Right,
	XK_KP_End, XK_KP_Down, XK_KP_Page_Down,
	XK_KP_Insert, XK_KP_Delete, XK_KP_Enter
};
  
static const char sym[] =
{ 
	// Shift Numeric 14
	'~', '!', '@', '#', '$',
	'%', '^', '&', '*', '(',
	')', '_', '+', '|',

	// Numeric 14
	'`', '1', '2', '3', '4',
	'5', '6', '7', '8', '9',
	'0', '-', '=', '\\',

	// Shift Character 33
	'Q', 'W', 'E', 'R',
	'T', 'Y', 'U', 'I',
	'O', 'P', '{', '}',
	'A', 'S', 'D', 'F',
	'G', 'H', 'J', 'K',
	'L', ':', '\"', 'Z',
	'X', 'C', 'V', 'B',
	'N', 'M', '<', '>',
	'?',

	// Character 33
	'q', 'w', 'e', 'r',
	't', 'y', 'u', 'i',
	'o', 'p', '[', ']',
	'a', 's', 'd', 'f',
	'g', 'h', 'j', 'k',
	'l', ';', '\'', 'z',
	'x', 'c', 'v', 'b',
	'n', 'm', ',', '.',
	'/', 

	// Special Character 3
	' ', '	', 13,
	
	// KeyPad Character 16
	'/', '*', '+', '-',
	'7', '8', '9',
	'4', '5', '6',
	'1', '2', '3',
	'0', '.', 13
};

static const int ksym_len = sizeof(ksym) / sizeof(ksym[0]);
static const int sym_len = sizeof(sym) / sizeof(sym[0]);

char keysym_to_char(KeySym ks)
{
	int i;
	for (i = 0; i < ksym_len; i++)
	{
		if (ksym[i] == ks)
			return sym[i];
	}
	return NULLSYM;
}

KeySym char_to_keysym(char ch)
{
	int i;
	for (i = 0; i < sym_len; i++)
	{
		if (sym[i] == ch)
			return ksym[i];
	}
	return XK_Return;
}

static char get_ascii(const char *sym)
{
	int i;
	for (i = 0; i < eng_len; i++)
	{
		unsigned short usym = *(unsigned short*) sym;
		if (rus[i] == usym)
			return eng[i];
	}
	return NULLSYM;
}

void convert_text_to_ascii(char *text)
{
	int i, j, len = strlen(text);
	for(i = 0, j = 0; i < len; i++, j++)
	{
		if (isascii(text[i]))
		{
			text[j] = text[i];
			continue;
		}

		char new_symbol = get_ascii(&text[i]);
			
		for(; i < len - 1; i++)
		{
			if (isascii(text[i + 1]))
				break;
			if (get_ascii(&text[i + 1]) != NULLSYM)
				break;
		}

		text[j] = new_symbol;
	}

	text[j] = NULLSYM;
}

static const char* get_translit(const char *sym)
{
	int i;
	for (i = 0; i < eng_len; i++)
	{
		unsigned short usym = *(unsigned short*) sym;
		if (rus[i] == usym)
			return translit[i];
	}
	return NULLSYM;
}

char* convert_text_to_translit(const char *text)
{
	int i, j, len = strlen(text);

	char *trans_text = (char *)xnmalloc((len * 3 + 1) * sizeof(char));

	for(i = 0, j = 0; i < len; i++)
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
	return trans_text;
}
