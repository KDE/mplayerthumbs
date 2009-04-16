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

#include "mplayervideobackend.h"
#include "constants.h"
#include <kdebug.h>
#include <QDir>
#include <KMD5>
#include <ktempdir.h>
#include <QProcess>
#include "mplayerthumbscfg.h"
#include <kstandarddirs.h>
#include "previewingfile.h"
#include "thumbnail.h"

MPlayerVideoBackend::MPlayerVideoBackend(PreviewingFile *previewingfile, MPlayerThumbsCfg* cfg) 
  : VideoBackendIFace(previewingfile, cfg)
{
}


bool MPlayerVideoBackend::readStreamInformation() {
    tmpdir=new KTempDir();
    if(tmpdir->name().isNull() ) return false;
    kDebug(DBG_AREA) << "videopreview: using temp directory " << tmpdir->name() << endl;

    mplayerprocess=new QProcess();
    KUrl furl( previewingFile->getFilePath() );
    kDebug(DBG_AREA) << "videopreview: url=" << furl << "; local:" << furl.isLocalFile() << endl;


    QStringList args;
    args << playerBin << QString("\"" + previewingFile->getFilePath() + "\"") << "-nocache" << "-identify" << "-vo" << "null" << "-frames" << "0"/* << "-nosound" */<< "-ao" << "null";
    args+= customargs;

    kDebug(DBG_AREA) << "videopreview: starting process: --_" << " " << args.join(" ") << "_--\n";
    if (! startAndWaitProcess(args) ) return false;

    QString information=QString(mplayerprocess->readAllStandardOutput() );
    QRegExp findInfos("ID_VIDEO_FPS=([\\d]*).*ID_LENGTH=([\\d]*).*");
    if(findInfos.indexIn( information) == -1 )
    {
        kDebug(DBG_AREA) << "videopreview: No information found, exiting\n";
        return false;
    }
    previewingFile->setStreamInformation(findInfos.cap(1).toInt(), findInfos.cap(2).toInt() * 1000);
    
    kDebug(DBG_AREA) << "videopreview: find length=" << previewingFile->getMillisecondsLength() << " ms, fps=" << previewingFile->getFPS() << endl;
    return true;
}


MPlayerVideoBackend::~MPlayerVideoBackend() {
    delete mplayerprocess;
    tryUnlink(tmpdir);
    delete tmpdir;
}



Thumbnail *MPlayerVideoBackend::preview(FrameSelector *frameSelector) {
    QStringList args;
    kDebug(DBG_AREA) << "videopreview: using seek strategy " << frameSelector->seekStrategy() << endl;
    args.clear();
    int scalingWidth=previewingFile->getScalingWidth();
    int scalingHeight=previewingFile->getScalingHeight();
    int fps=previewingFile->getFPS();
    args << playerBin << "\"" + previewingFile->getFilePath() + "\"";
    if(previewingFile->isWide()) scalingHeight=-2; else scalingWidth=-2;

    if( frameSelector->seekStrategy() & FrameSelector::Random )
    {
        kDebug(DBG_AREA) << "videopreview: framerandom\n";
        args << "-ss" << QString::number( frameSelector->framePositionInMilliseconds(previewingFile) / 1000 ) << "-frames" << "4";
    } else if (frameSelector->seekStrategy() & FrameSelector::FromStart)
    {
        kDebug(DBG_AREA) << "videopreview: framestart\n";
        if(!fps) fps=25; // if we've not autodetected a fps rate, let's assume 25fps.. even if it's wrong it shouldn't hurt.
        // If we can't skip to a random frame, let's try playing 10 seconds.
        args << "-frames" << QString::number( fps* frameSelector->framePositionInMilliseconds(previewingFile) / 1000 );
    }
    KMD5 md5builder(previewingFile->getFilePath().toLatin1() );
    QString md5file=md5builder.hexDigest().data();
    QString tmpDirPath = tmpdir->name() + md5file + QDir::separator();
    args << "-nocache" << "-idx" /*@TODO check if it's too slow..*/ << "-ao" << "null"/*"-nosound" << */<< "-speed" << "99"  /*<< "-sstep" << "5"*/
            << "-vo" << QString("jpeg:outdir=%1").arg(tmpDirPath ) << "-vf" << QString("scale=%1:%2").arg(scalingWidth).arg(scalingHeight);
    args+=customargs;

    if (! startAndWaitProcess(args) ) return NULL;

    kDebug(DBG_AREA) << "videopreview: temp dir '" << tmpDirPath << "'\n";

    if (QDir(tmpDirPath ).entryList( QStringList("*.jpg") ).isEmpty() ) return false;

    QString lastframe=QDir(tmpDirPath ).entryList( QStringList("*.jpg") ).last();
    kDebug(DBG_AREA) << "videopreview: LastFrame==" << lastframe << endl;
    QPixmap *retpix = new QPixmap(tmpDirPath.append( lastframe ));
    return new Thumbnail(retpix, previewingFile);
}


void MPlayerVideoBackend::tryUnlink(KTempDir *dir) {
    if(dir) dir->unlink();
}


bool MPlayerVideoBackend::findPlayerBin() {
    playerBin=mplayerThumbsConfig->mplayerbin();
    customargs=mplayerThumbsConfig->customargs().split(' ');
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

bool MPlayerVideoBackend::playerCannotPreview() {
  return ! findPlayerBin();
}


