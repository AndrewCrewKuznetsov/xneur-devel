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
#ifndef KXNEURCONF_H
#define KXNEURCONF_H

#include <kconfigdialog.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include "kxneur.h"
#include "key_convert.h"



class KApplication;
class KXNeurApp;
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
// class QLineEdit;
class QSpinBox;
class KKeyChooser;
class KGlobalAccel;


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

class KXNLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    KXNLineEdit(QWidget * parent, const char * name = 0);
    ~KXNLineEdit();

    // QWidget *par;

public slots:
    void openDlg();
};



class KXNeurPage : public QWidget
{

public:
    KXNeurPage( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KXNeurPage();

    QVBoxLayout *vlayout, *vlayout1, *vlayout2;
    QCheckBox *run_xneur, *autostart, *sw_mode, *force_run, *mouse_mode, *edu_mode, *save_sel_text;
    QComboBox *in_tray, *xneur_mode;
    QGroupBox *group1, *group2;
    QHBox *box2;
};


class XNeurPage : public QWidget
{
    Q_OBJECT
public:
    XNeurPage( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~XNeurPage();

    QVBoxLayout *vlayout, *vlayout2;
    QComboBox *default_group;
    QSpinBox *send_delay;
    QGroupBox *group1, *group2;
    QGridLayout *glayout, *glayout2;
    QGrid *grid;

    QComboBox *lang[MAX_LANGUAGES];
    KXNComboBox *num[MAX_LANGUAGES];
    KXNPushButton *regexp[MAX_LANGUAGES];
    KXNPushButton *dict[MAX_LANGUAGES];

    QCheckBox *remem_win;
    KXNListBox *list;
    QPushButton *addprg, *add2prg, *delprg;
private slots:
    void open_file(char *path);
};


class KeysPage : public QWidget
{

public:
    KeysPage( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KeysPage();

    QVBoxLayout *vlayout;
    // QGrid *grid;

    // QComboBox *mod[MAX_HOTKEYS], *key[MAX_HOTKEYS];
    KKeyChooser *keyChooser;
};


class ProgPage : public QWidget
{

public:
    ProgPage( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ProgPage();

    QVBoxLayout *vlayout;
    QGroupBox *group[3];
    QGridLayout *glayout[3];
    KXNListBox *list[3];
    QPushButton *addprg[3], *add2prg[3], *delprg[3];
};


class SndPage : public QWidget
{

public:
    SndPage( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~SndPage();

    QVBoxLayout *vlayout;
    QCheckBox *enable_snd;
    // QHBox *hbox;
    QGrid *grid;
    KXNLineEdit *edit[14];
    QPushButton *btn[14];

};


class KXNeurConf : public KConfigDialog
{

public:
    KXNeurConf(KXNeurApp *app, QWidget *parent = 0);
    ~KXNeurConf();

    int orig_keys[10];

private:
    KXNeurPage *kxneur_page;
    XNeurPage *xneur_page;
    KeysPage *keys_page;
    ProgPage *prog_page;
    SndPage *snd_page;
    KXNeurApp *knapp;

    KGlobalAccel *keys;
    KeyConvert kc;

    void LoadSettings();
    void SaveSettings();
    int KeyFromXNConf(int action);
    // void ToXNConf(int action, QString s);
    void KeyToXNConf(int a, int c);

protected:
    virtual void slotApply();
    virtual void slotOk();
  // virtual void slotCancel();
};

#endif
