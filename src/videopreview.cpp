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

#include "videopreview.h"

#include <QFile>
#include <qpixmap.h>
#include <qimage.h>
#include <QtCore/QVarLengthArray>

#include <kmimetype.h>
#include <QDir>
#include <qpainter.h>
#include <qdatetime.h>
#include <qregexp.h>
#include "videopreview.h"
#include <kdebug.h>
#include <kurl.h>
#include <math.h>
#include <QFileInfo>
#include <kcodecs.h>
#include <kstandarddirs.h>

#include "videobackendiface.h"
#include "mplayervideobackend.h"
#include "mplayerthumbs.h"

#include <unistd.h>

//#include "config.h"
extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
    {
        return new VideoPreview;
    }
}

VideoPreview::VideoPreview()
    : m_data(0),
      m_dataSize(0)
{
}

VideoPreview::~VideoPreview()
{
    delete [] m_data;
}

bool VideoPreview::create(const QString &path, int width, int height, QImage &img)
{
    kDebug(DBG_AREA) << "videopreview svn\n";
    MPlayerThumbsCfg *cfg=MPlayerThumbsCfg::self();
    VideoBackendIFace *videoBackend = new MPlayerVideoBackend(path, cfg);

    QFileInfo fi(path);
    if(hasBlacklistedExtension(&fi, cfg) || ! videoBackend->canPreview() ) {
      delete cfg;
      delete videoBackend;
      return false;
    }

    FileInformation fileinfo=videoBackend->findFileInfo();

    if(! fileinfo.isValid) return false;
    fileinfo.towidth=width;
    fileinfo.toheight=height;
    QPixmap pix;
    int flags=framerandom;
    #define LASTTRY 3
    for(int i=0; i<=LASTTRY; i++)
    {
        kDebug(DBG_AREA) << "videopreview: try " << i << endl;
	flags=((i<LASTTRY) ? flags : framestart );
        pix=videoBackend->getVideoFrame(flags, fileinfo);
        if(!pix.isNull()) {
            uint variance=imageVariance(pix.toImage()/*.bits(),( (width+ 7) & ~0x7), width, height, 1 */);
            kDebug(DBG_AREA) << "videopreview: " << QFileInfo(path).fileName() << " frame variance: " << variance << "; " << 
                    ((variance<=40 && ( i!=LASTTRY-1))? "!!!DROPPING!!!" : "GOOD :-)" ) << endl;
            if(variance>40 || i==LASTTRY-1 ) break;
        }
    }

  delete videoBackend;

    if(pix.isNull() )
        return false;
    /** From videocreator.cpp - xine_artsplugin
    Copyright (C) 2002 Simon MacMullen
    Copyright (C) 2003 Ewald Snel <ewald@rambo.its.tudelft.nl>
     * */
//     QPixmap pix( createThumbnail( &frame, width, height ) );
//#ifdef STRIPS_SUPPORT
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
//#endif
    img = pix.toImage();

    return true;
}

ThumbCreator::Flags VideoPreview::flags() const
{
    return (Flags)(DrawFrame);
}


uint VideoPreview::imageVariance(QImage image )
{
    uint delta=0;
    uint avg=0;
    uint bytes=image.numBytes();
    uint STEPS=bytes/2;
    QVarLengthArray<uchar> pivot(STEPS);
    kDebug(DBG_AREA) << "Using " << STEPS << " steps\n";
    uchar *bits=image.bits();
    // First pass: get pivots and taking average
    for( uint i=0; i<STEPS ; i++ ){
        pivot[i]=bits[i*(bytes/STEPS)];
        avg+=pivot[i];
    }
    avg=avg/STEPS;
    // Second Step: calculate delta (average?)
    for (uint i=0; i<STEPS; i++)
    {
        int curdelta=abs(int(avg-pivot[i]));
        delta+=curdelta;
    }
    return delta/STEPS;
}

bool VideoPreview::hasBlacklistedExtension(QFileInfo* fileInfo, MPlayerThumbsCfg *cfg) {
    QString extension=fileInfo->suffix().trimmed();
    kDebug(DBG_AREA) << "videopreview: file extension=\"" << extension << "\"\n";
    if( extension.length() && !cfg->noextensions().filter(extension, Qt::CaseInsensitive).isEmpty() )
    {
        kDebug(DBG_AREA) << "videopreview: matched extension " << extension.prepend('.') << "; exiting.\n";
        return true;
    }
    return false;
}


#include "videopreview.moc"

