/***************************************************************************
   Copyright (C) 2006-2009
   by Marco Gulino <marco.gulino@gmail.com>

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

#include "videopreview.h"
#include "constants.h"
#include <qpixmap.h>
#include <qimage.h>
#include <QtCore/QVarLengthArray>

#include <kmimetype.h>
#include <qpainter.h>
#include <qdatetime.h>
#include "videopreview.h"
#include <kdebug.h>
#include <kstandarddirs.h>

#include "videobackendiface.h"
#include "mplayervideobackend.h"
#include "mplayerthumbs.h"
#include "previewingfile.h"
#include "thumbnail.h"

extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
    {
        return new VideoPreview;
    }
}

VideoPreview::VideoPreview()
{
}

VideoPreview::~VideoPreview()
{
}

bool VideoPreview::create(const QString &path, int width, int height, QImage &img)
{
    kDebug(DBG_AREA) << "videopreview svn\n";
    MPlayerThumbsCfg *cfg=MPlayerThumbsCfg::self();
    PreviewingFile *previewingFile = new PreviewingFile(path, width, height, this);
    VideoBackendIFace *videoBackend = new MPlayerVideoBackend(previewingFile, cfg);

    if( videoBackend->cannotPreview() || ! videoBackend->readStreamInformation() ) {
      delete cfg;
      delete videoBackend;
      return false;
    }
    
    Thumbnail *thumbnail=previewingFile->getPreview(videoBackend, 40, 3);
    if(!thumbnail || ! thumbnail->pixmapIsValid() ) return false;

    delete videoBackend;

    /* From videocreator.cpp - xine_artsplugin
    Copyright (C) 2002 Simon MacMullen
    Copyright (C) 2003 Ewald Snel <ewald@rambo.its.tudelft.nl>
     * */
    QPixmap *pix=thumbnail->getPixmap();
    if(cfg->createStrips() ) {
        QPainter painter( pix );
        QPixmap sprocket;

        kDebug(DBG_AREA) << "videopreview: using strip image sprocket: " << KStandardDirs::locate( "data", "videothumbnail/sprocket-small.png" ) << endl;
        if (pix->height() < 60)
            sprocket = QPixmap(KStandardDirs::locate( "data", "videothumbnail/sprocket-small.png" ));
        else if (pix->height() < 90)
            sprocket = QPixmap(KStandardDirs::locate( "data", "videothumbnail/sprocket-medium.png" ));
        else
            sprocket = QPixmap(KStandardDirs::locate( "data", "videothumbnail/sprocket-large.png" ));

        for (int y = 0; y < pix->height() + sprocket.height(); y += sprocket.height()) {
            painter.drawPixmap( 0, y, sprocket );
        }
}
        // End of xine-artsplugin code
    img = pix->toImage();

    return true;
}

ThumbCreator::Flags VideoPreview::flags() const
{
    return (Flags)(DrawFrame);
}

#include "videopreview.moc"

