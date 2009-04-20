/***************************************************************************
   Copyright (C) 2006-2008
   by Marco Gulino <marco@kmobiletools.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
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
#include "videobackendiface.h"
#include <q3vbox.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <qlabel.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <qtimer.h>
#include <keditlistbox.h>
#include <kmessagebox.h>
#include <kio/deletejob.h>
#include <QDir>
const QString MPlayerThumbsConfig::thumbnailsDir( QDir::homePath() + "/.thumbnails/" );


MPlayerThumbsConfig::MPlayerThumbsConfig(QWidget *parent, const QString &name, MPlayerThumbsCfg *config)
    : KConfigDialog(parent, name, config)
{
    dialogUI=new Ui::configDialog();
    mplayerConfigUI=new Ui::mplayerConfig();
    QWidget *dialogWidget=new QWidget();
    QWidget *mplayerConfigUIWidget = new QWidget();
    dialogUI->setupUi(dialogWidget);
    mplayerConfigUI->setupUi(mplayerConfigUIWidget);
    addPage( dialogWidget, i18n("General"), "general" );
    addPage( mplayerConfigUIWidget, i18n("MPlayer Backend"), "mplayer" );
    connect( dialogUI->clean_cache, SIGNAL(clicked() ), this, SLOT(cleanCache() ) );
//     setFaceType(Plain);
    if(!config->mplayerbin().length() )
        QTimer::singleShot( 100, this, SLOT(autoFindPath()));
    dialogUI->kcfg_backend->addItem(i18n("MPlayer"), VideoBackendIFace::MPlayer );
    dialogUI->kcfg_backend->addItem(i18n("Phonon"), VideoBackendIFace::Phonon );
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
    mplayerConfigUI->kcfg_mplayerbin->setPath( playerPath );
}

void MPlayerThumbsConfig::cleanCache() {
    int doClean = KMessageBox::warningContinueCancel(this, i18n("Cleaning the cache will delete all the previously generated thumbnails.\nNote that as there is a single common thumbnail cache, the thumbnails for all other file types will also be deleted.\nDo you really want to clean up the cache?") );
    if (doClean!= KMessageBox::Continue ) return;

    kDebug() << "Cleaning cache from " << thumbnailsDir << endl;
    KIO::del( thumbnailsDir );
}

