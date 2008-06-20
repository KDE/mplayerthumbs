/***************************************************************************
   Copyright (C) 2006-2008
   by Marco Gulino <marco@kmobiletools.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "mplayerthumbscfg.h"
#include <q3vbox.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <qlabel.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <qtimer.h>
#include <keditlistbox.h>

MPlayerThumbsConfig::MPlayerThumbsConfig(QWidget *parent, const QString &name, MPlayerThumbsCfg *config)
    : KConfigDialog(parent, name, config)
{
    Q3VBox *vbox=new Q3VBox(0);
    vbox->setSpacing( 5);
    new QLabel(i18n("Enter here the path for mplayer executable file."), vbox);
    kcfg_mplayerbin=new KLineEdit( vbox, "kcfg_mplayerbin");
    new QLabel(i18n("Custom arguments for mplayer."), vbox);
    new KLineEdit( vbox, "kcfg_customargs");
    new KEditListBox( i18n("Blacklisted File Extensions"),
            vbox, "kcfg_noextensions", true, KEditListBox::Add | KEditListBox::Remove);
    addPage( vbox, i18n("General"), "mplayer" );
    kDebug() << "config->mplayerbin().isNull()::" << config->mplayerbin().length() << endl;
    if(!config->mplayerbin().length() )
        QTimer::singleShot( 100, this, SLOT(autoFindPath()));
}

MPlayerThumbsConfig::~MPlayerThumbsConfig()
{
}

#include "mplayerthumbscfg.moc"


/*!
    \fn MPlayerThumbsConfig::autoFindPath()
 */
void MPlayerThumbsConfig::autoFindPath()
{
    QString playerPath=KStandardDirs::findExe("mplayer-bin");
    if(playerPath.isNull() ) playerPath=KStandardDirs::findExe("mplayer");
    kDebug() << "Trying to set player path to " << playerPath << endl;
    kcfg_mplayerbin->setText( playerPath );
}
