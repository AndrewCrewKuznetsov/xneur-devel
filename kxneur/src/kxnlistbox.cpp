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
#include <stdlib.h>
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
#ifndef Q_OS_FREEBSD
    size_t n = 0;
    ssize_t m;
    char *buf;
#else
    char buf[1024];
    int n;
#endif
    FILE *fl;
    // char *cmd = (char *)"perl -e 'open F,\"xwininfo -root -children |\";my %l;while(<F>){if(/\\(\\\"(\\w+)\\\"/){$l{$1}=0}}close F;for(sort keys %l){print\"$_\\n\"}'"; // for get program
    char *cmd = (char *)"perl -e 'open F,\"xwininfo -root -children |\";my %l;while(<F>){if(/\\\"(\\w+)\\\"\\)/){$l{$1}=0}}close F;for(sort keys %l){print\"$_\\n\"}'"; // for get window name
    bool ok;
    QStringList prgs;
    if ( (fl = popen(cmd, "r")) != NULL ) {
#ifndef Q_OS_FREEBSD
	while ( (m = getline(&buf, &n, fl)) != -1 )
#else
	while ( fgets(buf, n, fl) ) {
#endif
	    prgs << QString::fromLatin1(buf, m-1);
	pclose(fl);
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
#ifndef Q_OS_FREEBSD
    size_t n = 0;
    ssize_t m;
    char *buf;
#else
    char buf[1024];
    int n;
#endif
    FILE *fl;
    if ( (fl = popen("xprop WM_CLASS", "r")) != NULL ) {
#ifndef Q_OS_FREEBSD
	while ( (m = getline(&buf, &n, fl)) != -1 ) {
#else
	while ( fgets(buf, n, fl) ) {
#endif
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


// #include  "kxnlistbox.moc"



