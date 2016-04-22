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
#ifndef KXNEURSETTINGS_H
#define KXNEURSETTINGS_H

#include <kconfigskeleton.h>
#include <kdebug.h>

/**
	@author Vadim Likhota <vadim-lvv{a}yandex.ru>
*/
class KXNeurSettings : public KConfigSkeleton
{
public:
    static KXNeurSettings *self();
    ~KXNeurSettings();

    static void setRunXNeur( bool v )
    {
	if (!self()->isImmutable( QString::fromLatin1( "RunXNeur" ) ))
	    self()->run_xneur = v;
    }

    static bool RunXNeur()
    {
	return self()->run_xneur;
    }


    static void setAutostart( bool v )
    {
	if (!self()->isImmutable( QString::fromLatin1( "AutoStart" ) ))
	    self()->autostart = v;
    }

    static bool Autostart()
    {
	return self()->autostart;
    }


    static void setShowInTray( int v )
    {
	if (!self()->isImmutable( QString::fromLatin1( "ShowInTray" ) ))
	    self()->showintray = v;
    }

    static int ShowInTray()
    {
	return self()->showintray;
    }


    static void setSwitcherMode( bool v )
    {
	if (!self()->isImmutable( QString::fromLatin1( "SwitcherMode" ) ))
	    self()->sw_mode = v;
    }

    static bool SwitcherMode()
    {
	return self()->sw_mode;
    }


    static void setForceRun( bool v )
    {
	if (!self()->isImmutable( QString::fromLatin1( "ForceRun" ) ))
	    self()->force_run = v;
    }

    static bool ForceRun()
    {
	return self()->force_run;
    }


    static void writeConfig()
    {
	static_cast<KConfigSkeleton*>(self())->writeConfig();
    }
private:
    KXNeurSettings();

    static KXNeurSettings *mSelf;

    bool run_xneur;
    bool autostart;
    int showintray;
    bool sw_mode;
    bool force_run;
};

#endif
