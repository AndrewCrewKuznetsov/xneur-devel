/***************************************************************************
 *   Copyright (C) 2007 by Vadim Likhota   *
 *   vadim-lvv{a}yandex.ru   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <qstringlist.h>
#include <kinputdialog.h>
#include <qregexp.h>
#include <klocale.h>
#include "kxnlistbox.h"


KXNListBox::KXNListBox(QWidget * parent, const char * name, WFlags f)
 : QListBox(parent, name, f)
{
}


KXNListBox::~KXNListBox()
{
}

void KXNListBox::deleteSelected()
{
    removeItem(currentItem());
}

void KXNListBox::getProgList()
{
    size_t n = 0;
    ssize_t m;
    char *buf;
    FILE *fl;
    // char *cmd = (char *)"perl -e 'open F,\"xwininfo -root -children |\";my %l;while(<F>){if(/\\(\\\"(\\w+)\\\"/){$l{$1}=0}}close F;for(sort keys %l){print\"$_\\n\"}'"; // for get program
    char *cmd = (char *)"perl -e 'open F,\"xwininfo -root -children |\";my %l;while(<F>){if(/\\\"(\\w+)\\\"\\)/){$l{$1}=0}}close F;for(sort keys %l){print\"$_\\n\"}'"; // for get window name
    bool ok;
    QStringList prgs;
    if ( (fl = popen(cmd, "r")) != NULL ) {
	while ( (m = getline(&buf, &n, fl)) != -1 )
	    prgs << QString::fromLatin1(buf, m-1);
	pclose(fl);
	if ( buf )
	    free(buf);

	QString text = KInputDialog::getItem(NULL, i18n("current programs:"), prgs, 0, false, &ok);
	if ( ok )
	    insertItem(text);
    }
    else
	printf("can not open pipe with perl string\n");
}

void KXNListBox::getPointedProg()
{
    size_t n = 0;
    ssize_t m;
    char *buf;
    FILE *fl;
    if ( (fl = popen("xprop WM_CLASS", "r")) != NULL ) {
	while ( (m = getline(&buf, &n, fl)) != -1 ) {
	    QString ss(buf);
	    QRegExp re( "\"");
	    // insertItem(ss.section(re, 1, 1)); // for get program
	    insertItem(ss.section(re, 3, 3)); // for get window name
	}
	pclose(fl);
    }
    else
	printf("can not open pipe with xprop\n");
}


#include  "kxnlistbox.moc"



