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
#include "kxneursettings.h"

#include <kstaticdeleter.h>

KXNeurSettings *KXNeurSettings::mSelf = 0;
static KStaticDeleter<KXNeurSettings> staticKXNeurSettingsDeleter;


KXNeurSettings::KXNeurSettings()
 : KConfigSkeleton( QString::fromLatin1( "kxneurrc" ) )
{
    mSelf = this;

    setCurrentGroup( QString::fromLatin1( "General" ) );

    KConfigSkeleton::ItemBool  *itemRunXNeur;
    itemRunXNeur = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "RunXNeur" ), run_xneur, true );
    addItem( itemRunXNeur, QString::fromLatin1( "RunXNeur" ) );

    KConfigSkeleton::ItemBool  *itemAutostart;
    itemAutostart = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "AutoStart" ), autostart, true );
    addItem( itemAutostart, QString::fromLatin1( "AutoStart" ) );

    KConfigSkeleton::ItemInt  *itemShowInTray;
    itemShowInTray = new KConfigSkeleton::ItemInt( currentGroup(), QString::fromLatin1( "ShowInTray" ), showintray, 0 );
    // itemShowInTray->setMinValue(0);
    // itemShowInTray->setMinValue(1);
    addItem( itemShowInTray, QString::fromLatin1( "ShowInTray" ) );

    KConfigSkeleton::ItemBool  *itemSwitcherMode;
    itemSwitcherMode = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "SwitcherMode" ), sw_mode, false );
    addItem( itemSwitcherMode, QString::fromLatin1( "SwitcherMode" ) );

}


KXNeurSettings::~KXNeurSettings()
{
    if ( mSelf == this )
	staticKXNeurSettingsDeleter.setObject( mSelf, 0, false );
}

KXNeurSettings *KXNeurSettings::self()
{
  if ( !mSelf ) {
    staticKXNeurSettingsDeleter.setObject( mSelf, new KXNeurSettings() );
    mSelf->readConfig();
  }

  return mSelf;
}


