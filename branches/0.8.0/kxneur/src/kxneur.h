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

#ifndef _KXNEUR_H_
#define _KXNEUR_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define XN_END


#include <kuniqueapplication.h>
#include <qptrvector.h>
#include <qvaluevector.h>
extern "C" {
#include <xneur/xnconfig.h>
}

#define SHOW_ICON 0
#define SHOW_LANG 1
#define SHOW_ALL -1

class KXNKeyboard;
class KXNeurTray;
class KAction;
class KConfig;

class XNLang;
typedef QPtrVector<XNLang> XNLangVector;

class KXNeurApp : public KUniqueApplication
// class KXNeurApp : public KApplication
{
    Q_OBJECT
public:
    KXNeurApp();
    ~KXNeurApp();

    bool xneur_start();
    bool xneur_stop();
    bool xnconf_reload();

public:
    struct _xneur_config *xnconf;
    int cur_lang;
    int prev_lang;
    int cnt_langs;

public slots:
    void slotUpdateMode();
    void slotUpdateRun();
    void slotPref();

private:
    int xneur_pid;
    KXNeurTray *trayicon;
    KXNKeyboard *xnkb;
    XNLangVector langs;
    KConfig *all_langs;

    bool x11EventFilter(XEvent *e);
    void xnconf_gets();
private slots:
    void slotExit();
    void groupChange(int);
    void setNextLang();
    void setMenuLang(int);
    void refreshLang();
};


/* class for save lang groups */
class XNLang
{
public:
    XNLang(const QString &n);
    ~XNLang();

    QString name;
    QPixmap pic;
    QString lg;
    int supp_lg;
    int menuid;
};

#endif // _KXNEUR_H_
