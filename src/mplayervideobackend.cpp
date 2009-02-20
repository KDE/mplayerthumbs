/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "mplayervideobackend.h"

#include <kdebug.h>
#include <QDir>
#include <KMD5>
#include <ktempdir.h>
#include <QProcess>
#include "mplayerthumbscfg.h"
#include <kstandarddirs.h>
#include <krandomsequence.h>

MPlayerVideoBackend::MPlayerVideoBackend(const QString& filePath, MPlayerThumbsCfg* cfg) 
  : VideoBackendIFace(filePath, cfg)
{
}


VideoPreview::FileInformation MPlayerVideoBackend::findFileInfo() {
    VideoPreview::FileInformation fileinfo;
    fileinfo.seconds=0;
    fileinfo.fps=0;
    fileinfo.isValid=false;

    tmpdir=new KTempDir();
    if(tmpdir->name().isNull() ) return fileinfo;
    kDebug(DBG_AREA) << "videopreview: using temp directory " << tmpdir->name() << endl;

    rand=new KRandomSequence(QDateTime::currentDateTime().toTime_t());
    mplayerprocess=new QProcess();
    KUrl furl(this->s_filePath);
    kDebug(DBG_AREA) << "videopreview: url=" << furl << "; local:" << furl.isLocalFile() << endl;


    QStringList args;
    args << playerBin << QString("\"" + this->s_filePath + "\"") << "-nocache" << "-identify" << "-vo" << "null" << "-frames" << "0"/* << "-nosound" */<< "-ao" << "null";
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

MPlayerVideoBackend::~MPlayerVideoBackend() {
    delete mplayerprocess;
    delete rand;
    tryUnlink(tmpdir);
    delete tmpdir;
}



QPixmap MPlayerVideoBackend::getVideoFrame(int flags, VideoPreview::FileInformation fileinfo) {
    QStringList args;
    kDebug(DBG_AREA) << "videopreview: using flags " << flags << endl;
#define START ((fileinfo.seconds*15)/100)
#define END ((fileinfo.seconds*70)/100)
    args.clear();
    args << playerBin << "\"" + this->s_filePath + "\"";
    if(fileinfo.towidth>fileinfo.toheight) fileinfo.toheight=-2; else fileinfo.towidth=-2;
//     switch( flags ){
//         case random
//     }
    if( flags & VideoPreview::framerandom )
    {
        kDebug(DBG_AREA) << "videopreview: framerandom\n";
        unsigned long start=(unsigned long)(START+(rand->getDouble() * (END - START) ) );
        args << "-ss" << QString::number( start )
                << "-frames" << "4";
    } else if (flags & VideoPreview::frameend )
    {
        kDebug(DBG_AREA) << "videopreview: frameend\n";
        args << "-ss" << QString::number( fileinfo.seconds - 10 )
                << "-frames" << "4";
    } else if (flags & VideoPreview::framestart)
    {
        kDebug(DBG_AREA) << "videopreview: framestart\n";
        if(!fileinfo.fps) fileinfo.fps=25; // if we've not autodetected a fps rate, let's assume 25fps.. even if it's wrong it shouldn't hurt.
        // If we can't skip to a random frame, let's try playing 10 seconds.
        args << "-frames" << QString::number( fileinfo.fps*10 );
    }
    KMD5 md5builder(this->s_filePath.toLatin1() );
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


void MPlayerVideoBackend::tryUnlink(KTempDir *dir) {
    if(dir) dir->unlink();
}


bool MPlayerVideoBackend::findPlayerBin() {
    playerBin=mplayerThumbsConfig->mplayerbin();
    customargs=mplayerThumbsConfig->customargs().split(" ");
    kDebug(DBG_AREA) << "videopreview: customargs=" << mplayerThumbsConfig->customargs() << " ;;;; " << customargs << endl;
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


bool MPlayerVideoBackend::startAndWaitProcess(const QStringList &args) {
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

bool MPlayerVideoBackend::canPreview() {
  return findPlayerBin();
}


