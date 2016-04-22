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

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include "kxneur.h"
#include "kxnkeyboard.h"
// #include <stdlib.h>

static const char description[] =
    I18N_NOOP("KXNeur (KDE X Neural Switcher) is XNeur front-end for KDE ( http://xneur.ru ).\nThis version work with XNeur v.0.9.0 only");

static const char version[] = "0.9.0";

static KCmdLineOptions options[] =
{
//    { "+[URL]", i18n( "Document to open" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("kxneur", I18N_NOOP("keyboard autoswitcher"), version, description,
		     KAboutData::License_GPL, "(C) 2007-2008 Vadim Likhota", 0, "http://xneur.ru",
		     "xneur@lists.net.ru");
    about.addAuthor( "Vadim Likhota", 0, "vadim-lvv@yandex.ru" );
    about.addCredit("XNeur Team", I18N_NOOP("Authors X Neural Switcher, including Andrew Crew Kuznetsov and Yankin Nickolay Valerevich"), "http://xneur.ru/");
    about.addCredit("Leonid Zeitlin", I18N_NOOP("Author kkbswitch, "), "lz@europe.com");
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
#ifdef XN_END
    if (!KUniqueApplication::start()) {
        // fprintf(stderr, "KXNeur is already running!\n");
        QApplication::exit(0);
    }
#endif
    KXNeurApp app;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    args->clear();

    if (KXNKeyboard::self()->xkbAvailable())
	return app.exec();
    return -1;
}
