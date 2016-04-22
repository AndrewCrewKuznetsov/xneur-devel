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

#ifndef _KXNEUR_H_
#define _KXNEUR_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <kuniqueapplication.h>
#include <qptrvector.h>
#include <qvaluevector.h>
extern "C" {
#include <xneur/xnconfig.h>
}

#define XNEUR_NEEDED_MAJOR_VERSION      3
#define MAX_LANGUAGES 4
#define SHOW_ICON 0
#define SHOW_LANG 1
#define SHOW_ALL -1

#define XN_END

class KXNKeyboard;
class KXNeurTray;
class KAction;
class KConfig;

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

typedef QPtrVector<XNLang> XNLangVector;

// moc not conceive #define
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


#endif // _KXNEUR_H_
