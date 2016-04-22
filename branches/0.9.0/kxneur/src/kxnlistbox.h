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
#ifndef KXNLISTBOX_H
#define KXNLISTBOX_H

#include <qlistbox.h>

class KXNListBox : public QListBox
{
    Q_OBJECT
public:
    KXNListBox(QWidget * parent = 0, const char * name = 0, WFlags f = 0);
    ~KXNListBox();

public slots:
   void deleteSelected();
   void getProgList();
   void getPointedProg();
};

#endif
