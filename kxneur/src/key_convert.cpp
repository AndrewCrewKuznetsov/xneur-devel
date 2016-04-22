/**********************************************************************************
 *  KXNeur (KDE X Neural Switcher) is XNeur front-end for KDE ( http://xneur.ru ).*
 *  Copyright (C) 2007-2008  Vadim Likhota <vadim-lvv@yandex.ru>                  *
 *                                                                                *
 *  This program is free software; you can redistribute it and/or modify          *
 *  it under the terms of the GNU General Public License as published by          *
 *  the Free Software Foundation; either version 2 of the License, or             *
 *  (at your option) any later version.                                           *
 *                                                                                *
 *  This program is distributed in the hope that it will be useful,               *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
 *  GNU General Public License for more details.                                  *
 *                                                                                *
 *  You should have received a copy of the GNU General Public License             *
 *  along with this program; if not, write to the Free Software                   *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA    *
 **********************************************************************************/

#include "key_convert.h"
#include <qnamespace.h>


KeyConvert::KeyConvert()
{
    keys["Escape"] = Qt::Key_Escape;
    codes[Qt::Key_Escape] = "Escape";

    keys["BackSpace"] = Qt::Key_Backspace;
    codes[Qt::Key_Backspace] = "BackSpace";

    keys["Insert"] = Qt::Key_Insert;
    codes[Qt::Key_Insert] = "Insert";

    keys["Break"] = Qt::Key_Pause;
    codes[Qt::Key_Pause] = "Break";
    keys["Pause"] = Qt::Key_Pause;

    keys["Print"] = Qt::Key_Print;
    codes[Qt::Key_Print] = "Print";

    keys["SysReq"] = Qt::Key_SysReq;
    codes[Qt::Key_SysReq] = "SysReq";

    keys["Scroll_Lock"] = Qt::Key_ScrollLock;
    codes[Qt::Key_ScrollLock] = "Scroll_Lock";

    keys["F1"] = Qt::Key_F1;
    codes[Qt::Key_F1] = "F1";

    keys["F2"] = Qt::Key_F2;
    codes[Qt::Key_F2] = "F2";

    keys["F3"] = Qt::Key_F3;
    codes[Qt::Key_F3] = "F3";

    keys["F4"] = Qt::Key_F4;
    codes[Qt::Key_F4] = "F4";

    keys["F5"] = Qt::Key_F5;
    codes[Qt::Key_F5] = "F5";

    keys["F6"] = Qt::Key_F6;
    codes[Qt::Key_F6] = "F6";

    keys["F7"] = Qt::Key_F7;
    codes[Qt::Key_F7] = "F7";

    keys["F8"] = Qt::Key_F8;
    codes[Qt::Key_F8] = "F8";

    keys["F9"] = Qt::Key_F9;
    codes[Qt::Key_F9] = "F9";

    keys["F10"] = Qt::Key_F10;
    codes[Qt::Key_F10] = "F10";

    keys["F11"] = Qt::Key_F11;
    codes[Qt::Key_F11] = "F11";

    keys["F12"] = Qt::Key_F12;
    codes[Qt::Key_F12] = "F12";

    keys["0"] = Qt::Key_0;
    codes[Qt::Key_0] = "0";

    keys["1"] = Qt::Key_1;
    codes[Qt::Key_1] = "1";

    keys["2"] = Qt::Key_2;
    codes[Qt::Key_2] = "2";

    keys["3"] = Qt::Key_3;
    codes[Qt::Key_3] = "3";

    keys["4"] = Qt::Key_4;
    codes[Qt::Key_4] = "4";

    keys["5"] = Qt::Key_5;
    codes[Qt::Key_5] = "5";

    keys["6"] = Qt::Key_6;
    codes[Qt::Key_6] = "6";

    keys["7"] = Qt::Key_7;
    codes[Qt::Key_7] = "7";

    keys["8"] = Qt::Key_8;
    codes[Qt::Key_8] = "8";

    keys["9"] = Qt::Key_9;
    codes[Qt::Key_0] = "9";

    keys["a"] = Qt::Key_A;
    codes[Qt::Key_A] = "a";

    keys["b"] = Qt::Key_B;
    codes[Qt::Key_B] = "b";

    keys["c"] = Qt::Key_C;
    codes[Qt::Key_C] = "c";

    keys["d"] = Qt::Key_D;
    codes[Qt::Key_D] = "d";

    keys["e"] = Qt::Key_E;
    codes[Qt::Key_E] = "e";

    keys["f"] = Qt::Key_F;
    codes[Qt::Key_F] = "f";

    keys["g"] = Qt::Key_G;
    codes[Qt::Key_G] = "g";

    keys["h"] = Qt::Key_H;
    codes[Qt::Key_H] = "h";

    keys["i"] = Qt::Key_I;
    codes[Qt::Key_I] = "i";

    keys["j"] = Qt::Key_J;
    codes[Qt::Key_J] = "j";

    keys["k"] = Qt::Key_K;
    codes[Qt::Key_K] = "k";

    keys["l"] = Qt::Key_L;
    codes[Qt::Key_L] = "l";

    keys["m"] = Qt::Key_M;
    codes[Qt::Key_M] = "m";

    keys["n"] = Qt::Key_N;
    codes[Qt::Key_N] = "n";

    keys["o"] = Qt::Key_O;
    codes[Qt::Key_O] = "o";

    keys["p"] = Qt::Key_P;
    codes[Qt::Key_P] = "p";

    keys["q"] = Qt::Key_Q;
    codes[Qt::Key_Q] = "q";

    keys["r"] = Qt::Key_R;
    codes[Qt::Key_R] = "r";

    keys["s"] = Qt::Key_S;
    codes[Qt::Key_S] = "s";

    keys["t"] = Qt::Key_T;
    codes[Qt::Key_T] = "t";

    keys["u"] = Qt::Key_U;
    codes[Qt::Key_U] = "u";

    keys["v"] = Qt::Key_V;
    codes[Qt::Key_V] = "v";

    keys["w"] = Qt::Key_W;
    codes[Qt::Key_W] = "w";

    keys["x"] = Qt::Key_X;
    codes[Qt::Key_X] = "x";

    keys["y"] = Qt::Key_Y;
    codes[Qt::Key_Y] = "y";

    keys["z"] = Qt::Key_Z;
    codes[Qt::Key_Z] = "z";

    keys["Tab"] = Qt::Key_Tab;
    codes[Qt::Key_Tab] = "Tab"; 

    keys["Return"] = Qt::Key_Return;
    codes[Qt::Key_Return] = "Return"; 

    keys["Space"] = Qt::Key_Return;
    codes[Qt::Key_Return] = "Space"; 

    keys["BackSpace"] = Qt::Key_BackSpace;
    codes[Qt::Key_BackSpace] = "BackSpace"; 

    keys["Escape"] = Qt::Key_Escape;
    codes[Qt::Key_Escape] = "Escape"; 
};

KeyConvert::~KeyConvert()
{
};

