/*
   Copyright (C) 2006-2009
   by Marco Gulino <marco.gulino@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2.1 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "videopreview.h"
#include "constants.h"
#include <qpixmap.h>
#include <qimage.h>
#include <QtCore/QVarLengthArray>

#include <kmimetype.h>
#include <qpainter.h>
#include <qdatetime.h>
#include <kdebug.h>
#include <kstandarddirs.h>

#include "videobackendiface.h"
#include "mplayervideobackend.h"
#include "mplayerthumbs.h"
#include "previewingfile.h"
#include "thumbnail.h"
#include "servicesfactory.h"


extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
    {
	kDebug(DBG_AREA) << "videoPreview: new_creator" << endl;

        return new VideoPreview(new ServicesFactory());
    }
}

VideoPreview::VideoPreview(ServicesFactory* servicesFactory) {
  kDebug() << "videoPreview constructor" << endl;
  this->servicesFactory=servicesFactory;
}


VideoPreview::~VideoPreview()
{
}
bool VideoPreview::create(const QString &path, int width, int height, QImage &img)
{
    kDebug(DBG_AREA) << "videopreview svn\n";
    MPlayerThumbsCfg *cfg=servicesFactory->config();
    PreviewingFile *previewingFile = servicesFactory->previewingFile(path, width, height, this);
    VideoBackendIFace *videoBackend = servicesFactory->videoBackend(previewingFile, cfg);

    if( !videoBackend || videoBackend->cannotPreview() || ! videoBackend->readStreamInformation() ) {
      delete cfg;
      delete videoBackend;
      return false;
      kDebug(DBG_AREA) << "cannot preview: " << videoBackend->cannotPreview() << endl;
    }
    
    Thumbnail *thumbnail=previewingFile->getPreview(videoBackend, 40, 4, sequenceIndex());
    if(!thumbnail || ! thumbnail->imageIsValid() ) return false;
    delete videoBackend;
    kDebug(DBG_AREA) << "got valid thumbnail, image variance: " << thumbnail->getVariance() << endl;
    /* From videocreator.cpp - xine_artsplugin
    Copyright (C) 2002 Simon MacMullen
    Copyright (C) 2003 Ewald Snel <ewald@rambo.its.tudelft.nl>
     * */
    QPixmap pix=QPixmap::fromImage(*(thumbnail->getImage()));
    if(cfg->createStrips() ) {
        QPainter painter( &pix );
        QPixmap sprocket;

        kDebug(DBG_AREA) << "videopreview: using strip image sprocket: " << KStandardDirs::locate( "data", "videothumbnail/sprocket-small.png" ) << endl;
        if (pix.height() < 60)
            sprocket = QPixmap(KStandardDirs::locate( "data", "videothumbnail/sprocket-small.png" ));
        else if (pix.height() < 90)
            sprocket = QPixmap(KStandardDirs::locate( "data", "videothumbnail/sprocket-medium.png" ));
        else
            sprocket = QPixmap(KStandardDirs::locate( "data", "videothumbnail/sprocket-large.png" ));

        for (int y = 0; y < pix.height() + sprocket.height(); y += sprocket.height()) {
            painter.drawPixmap( 0, y, sprocket );
        }
}
        // End of xine-artsplugin code
    img = pix.toImage();

    return true;
}

ThumbCreator::Flags VideoPreview::flags() const
{
    return (Flags)(DrawFrame);
}

#include "videopreview.moc"

