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
#ifndef KXNEURCONF_H
#define KXNEURCONF_H

#define MAX_LANGUAGES 4

#include <kconfigdialog.h>
#include <qpushbutton.h>

/**
	@author Vadim Likhota <vadim-lvv{a}yandex.ru>
*/

class KApplication;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QHbox;
class QGrid;
class QGroupBox;
class QCheckBox;
class QComboBox;
class KXNComboBox;
class QLabel;
class KXNListBox;



class KXNPushButton : public QPushButton
{
    Q_OBJECT
public:
    KXNPushButton(const QString & text, QWidget * parent, const char * name = 0);
    ~KXNPushButton();

    char * lang;
    char * file;
signals:
    void set_file(char *);
public slots:
    void update(int nn);
    void click1();
};



class KXNeurPage : public QWidget
{
    // Q_OBJECT
public:
    KXNeurPage( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KXNeurPage();

    QVBoxLayout *vlayout;
    QCheckBox *run_xneur, *autostart, *sw_mode;
    QComboBox *in_tray;
};


class XNeurPage : public QWidget
{
    Q_OBJECT
public:
    XNeurPage( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~XNeurPage();

    QVBoxLayout *vlayout, *vlayout2;
    QHBox *hbox;
    QComboBox *xneur_mode;
    QGroupBox *group1, *group2;
    QGridLayout *glayout;

    QComboBox *lang[MAX_LANGUAGES];
    KXNComboBox *num[MAX_LANGUAGES];
    KXNPushButton *regexp[MAX_LANGUAGES];
    KXNPushButton *dict[MAX_LANGUAGES];

    QCheckBox *mouse_mode, *edu_mode, *remem_win, *save_sel_text;
private slots:
    void open_file(char *path);
};


class KeysPage : public QWidget
{
    Q_OBJECT
public:
    KeysPage( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KeysPage();

    QVBoxLayout *vlayout;
    QGrid *grid;

    QComboBox *mod[MAX_HOTKEYS], *key[MAX_HOTKEYS];
};

class ProgPage : public QWidget
{
    Q_OBJECT
public:
    ProgPage( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ProgPage();

    QVBoxLayout *vlayout;
    QGroupBox *group[3];
    QGridLayout *glayout[3];
    KXNListBox *list[3];
    QPushButton *addprg[3], *add2prg[3], *delprg[3];

};


class KXNeurConf : public KConfigDialog
{
    Q_OBJECT
public:
    KXNeurConf(KXNeurApp *app, QWidget *parent = 0);
    ~KXNeurConf();

private:
    void LoadSettings();
    void SaveSettings();

private:
    KXNeurPage *kxneur_page;
    XNeurPage *xneur_page;
    KeysPage *keys_page;
    ProgPage *prog_page;
    KXNeurApp *knapp;

protected:
    virtual void slotApply();
    virtual void slotOk();
  // virtual void slotCancel();
};

#endif
