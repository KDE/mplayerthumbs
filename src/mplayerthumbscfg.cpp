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
#include <QVBoxLayout>


MPlayerThumbsConfig::MPlayerThumbsConfig(QWidget *parent, const QString &name, MPlayerThumbsCfg *config)
    : KConfigDialog(parent, name, config)
{
    dialogUI=new Ui::configDialog();
    QWidget *dialogWidget=new QWidget();
    dialogUI->setupUi(dialogWidget);
    addPage( dialogWidget, i18n("General"), "mplayer" );
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
    dialogUI->kcfg_mplayerbin->setPath( playerPath );
}