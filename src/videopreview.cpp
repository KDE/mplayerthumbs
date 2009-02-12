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

#include <kstandarddirs.h>
#include <kmimetype.h>
#include <QDir>
#include <qpainter.h>
#include <krandomsequence.h>
#include <qdatetime.h>
#include <qregexp.h>
#include "videopreview.h"
#include <QProcess>
#include <kdebug.h>
#include <ktempdir.h>
#include <kurl.h>
#include <math.h>
#include <QFileInfo>
#include <kcodecs.h>

#include "mplayerthumbs.h"

#include <unistd.h>

#define DBG_AREA

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
    delete tmpdir;
    delete rand;
    delete mplayerprocess;
}

bool VideoPreview::startAndWaitProcess(const QStringList &args) {
    kDebug(DBG_AREA) << "videopreview: starting process with args: " << args << endl;
    mplayerprocess->start( args.join(" ") );
    if(! mplayerprocess->waitForStarted() ) {
        kDebug(DBG_AREA) << "videopreview: PROCESS NOT STARTED!!! exiting\n";
        return false;
    }
    if(! mplayerprocess->waitForFinished() ) {
        kDebug(DBG_AREA) << "videopreview: PROCESS DIDN'T FINISH!! exiting\n";
        mplayerprocess->close();
        return false;
    }
  kDebug() << "videopreview: process started and ended correctly\n";
  return true;
}

bool VideoPreview::create(const QString &path, int width, int height, QImage &img)
{
    MPlayerThumbsCfg *cfg=MPlayerThumbsCfg::self();
    QFileInfo fi(path);
    if(hasBlacklistedExtension(&fi, cfg) || ! findPlayerBin(cfg) ) {
      delete cfg;
      return false;
    }

    FileInformation fileinfo=findFileInfo(path);
    if(! fileinfo.isValid) return false;
    fileinfo.towidth=width;
    fileinfo.toheight=height;
    QPixmap pix;
    int flags=0;
    flags=framerandom;
    #define LASTTRY 3
    for(int i=0; i<=LASTTRY; i++)
    {
        kDebug(DBG_AREA) << "videopreview: try " << i << endl;
flags=((i<LASTTRY) ? flags : framestart );
        pix=getFrame(path, flags );
        if(!pix.isNull()) {
            uint variance=imageVariance(pix.toImage()/*.bits(),( (width+ 7) & ~0x7), width, height, 1 */);
            kDebug(DBG_AREA) << "videopreview: " << QFileInfo(path).fileName() << " frame variance: " << variance << "; " << 
                    ((variance<=40 && ( i!=LASTTRY-1))? "!!!DROPPING!!!" : "GOOD :-)" ) << endl;
            if(variance>40 || i==LASTTRY-1 ) break;
        }
    }
    if(pix.isNull() )
    {
        tryUnlink(tmpdir);
        return false;
    }
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

    tryUnlink(tmpdir);
    return true;
}

void VideoPreview::tryUnlink(KTempDir *dir) {
    if(dir) dir->unlink();
}

QPixmap VideoPreview::getFrame(const QString &path, int flags)
{
    QStringList args;
    kDebug(DBG_AREA) << "videopreview: using flags " << flags << endl;
#define START ((fileinfo.seconds*15)/100)
#define END ((fileinfo.seconds*70)/100)
    args.clear();
    args << playerBin << "\"" + path + "\"";
    if(fileinfo.towidth>fileinfo.toheight) fileinfo.toheight=-2; else fileinfo.towidth=-2;
//     switch( flags ){
//         case random
//     }
    if( flags & framerandom )
    {
        kDebug(DBG_AREA) << "videopreview: framerandom\n";
        unsigned long start=(unsigned long)(START+(rand->getDouble() * (END - START) ) );
        args << "-ss" << QString::number( start )
                << "-frames" << "4";
    } else if (flags & frameend )
    {
        kDebug(DBG_AREA) << "videopreview: frameend\n";
        args << "-ss" << QString::number( fileinfo.seconds - 10 )
                << "-frames" << "4";
    } else if (flags & framestart)
    {
        kDebug(DBG_AREA) << "videopreview: framestart\n";
        if(!fileinfo.fps) fileinfo.fps=25; // if we've not autodetected a fps rate, let's assume 25fps.. even if it's wrong it shouldn't hurt.
        // If we can't skip to a random frame, let's try playing 10 seconds.
        args << "-frames" << QString::number( fileinfo.fps*10 );
    }
    KMD5 md5builder(path.toLatin1() );
    QString md5file=md5builder.hexDigest().data();
    QString tmpDirPath = tmpdir->name() + md5file + QDir::separator();
    args << "-nocache" << "-idx" /*@TODO check if it's too slow..*/ << "-ao" << "null"/*"-nosound" << */<< "-speed" << "99"  /*<< "-sstep" << "5"*/
            << "-vo" << QString("jpeg:outdir=%1").arg(tmpDirPath ) << "-vf" << QString("scale=%1:%2").arg(fileinfo.towidth).arg(fileinfo.toheight);
    args+=customargs;

    if (! startAndWaitProcess(args) ) return NULL;

    kDebug(DBG_AREA) << "videopreview: temp dir '" << tmpDirPath << "'\n";

    if (QDir(tmpDirPath ).entryList( QStringList("*.jpg") ).isEmpty() ) return false;

    QString lastframe=QDir(tmpDirPath ).entryList( QStringList("*.jpg") ).last();
    kDebug(DBG_AREA) << "videopreview: LastFrame==" << lastframe << endl;
    QPixmap retpix(tmpDirPath.append( lastframe ));
    return retpix;
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

bool VideoPreview::findPlayerBin( MPlayerThumbsCfg* cfg) {
    playerBin=cfg->mplayerbin();
    customargs=cfg->customargs().split(" ");
    kDebug(DBG_AREA) << "videopreview: customargs=" << cfg->customargs() << " ;;;; " << customargs << endl;
    if(playerBin.length()) kDebug(DBG_AREA) << "videopreview: found playerbin from config: " << playerBin << endl;
    else
    {
        playerBin=KStandardDirs::findExe("mplayer-bin");
        if(!playerBin.length()) playerBin=KStandardDirs::findExe("mplayer");
        if(!playerBin.length())
        {
            kDebug(DBG_AREA) << "videopreview: mplayer not found, exiting. Run mplayerthumbsconfig to setup mplayer path manually.\n";
            return false;
        }
        kDebug(DBG_AREA) << "videopreview: found playerbin from path: " << playerBin << endl;
    }
    return true;
}

VideoPreview::FileInformation VideoPreview::findFileInfo(QString filePath) {
    FileInformation fileinfo;
    fileinfo.seconds=0;
    fileinfo.fps=0;
    fileinfo.isValid=false;
    tmpdir=new KTempDir();
    if(tmpdir->name().isNull() ) return fileinfo;
    kDebug(DBG_AREA) << "videopreview: using temp directory " << tmpdir->name() << endl;

    rand=new KRandomSequence(QDateTime::currentDateTime().toTime_t());
    mplayerprocess=new QProcess();
    KUrl furl(filePath);
    kDebug(DBG_AREA) << "videopreview: url=" << furl << "; local:" << furl.isLocalFile() << endl;


    QStringList args;
    args << playerBin << QString("\"" + filePath + "\"") << "-nocache" << "-identify" << "-vo" << "null" << "-frames" << "0"/* << "-nosound" */<< "-ao" << "null";
    args+= customargs;

    kDebug(DBG_AREA) << "videopreview: starting process: --_" << " " << args.join(" ") << "_--\n";
    if (! startAndWaitProcess(args) ) return fileinfo;

    QString information=QString(mplayerprocess->readAllStandardOutput() );
    QRegExp findInfos("ID_VIDEO_FPS=([\\d]*).*ID_LENGTH=([\\d]*).*");
    if(findInfos.indexIn( information) == -1 )
    {
        kDebug(DBG_AREA) << "videopreview: No information found, exiting\n";
        return fileinfo;
    }
    fileinfo.seconds =findInfos.cap(2).toInt();
    fileinfo.fps=findInfos.cap(1).toInt();
    
    kDebug(DBG_AREA) << "videopreview: find length=" << fileinfo.seconds << ", fps=" << fileinfo.fps << endl;
    fileinfo.isValid=true;
    return fileinfo;
}



#include "videopreview.moc"

