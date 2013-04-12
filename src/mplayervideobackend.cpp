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
#include <kstandarddirs.h>
#include "previewingfile.h"
#include "thumbnail.h"
#include <kurl.h>
#include "mplayerthumbs.h"

ArgsCalculator::ArgsCalculator(PreviewingFile* previewingFile)
{
  this->previewingFile=previewingFile;
}

class RandomArgsCalculator : public ArgsCalculator {
  public:
    RandomArgsCalculator(PreviewingFile *previewingFile) : ArgsCalculator(previewingFile) {}
    virtual QStringList args(FrameSelector *frameSelector) {
      kDebug(DBG_AREA) << "videopreview: framerandom\n";
      return QStringList() << QLatin1String( "-ss" ) << QString::number( frameSelector->framePositionInMilliseconds(previewingFile) / 1000 ) << QLatin1String( "-frames" ) << QLatin1String( "4" );
    }
};


class FromStartArgsCalculator : public ArgsCalculator {
  public:
    FromStartArgsCalculator(PreviewingFile *previewingFile) : ArgsCalculator(previewingFile) {}
    virtual QStringList args(FrameSelector *frameSelector) {
        kDebug(DBG_AREA) << "videopreview: framestart\n";
	int fps=previewingFile->getFPS();
        if(!fps) fps=25; // if we've not autodetected a fps rate, let's assume 25fps.. even if it's wrong it shouldn't hurt.
        // If we can't skip to a random frame, let's try playing N seconds.
        return QStringList() << QLatin1String( "-frames" ) << QString::number( fps * frameSelector->framePositionInMilliseconds(previewingFile) / 1000 );
    }
};


MPlayerVideoBackend::MPlayerVideoBackend(PreviewingFile *previewingfile, MPlayerThumbsCfg* cfg)
  : VideoBackendIFace(previewingfile, cfg)
{
  seekArguments.insert(FrameSelector::FromStart, new FromStartArgsCalculator(previewingfile) );
  seekArguments.insert(FrameSelector::Random, new RandomArgsCalculator(previewingfile));
}


bool MPlayerVideoBackend::readStreamInformation() {
    tmpdir=new KTempDir();
    if(tmpdir->name().isNull() ) return false;
    kDebug(DBG_AREA) << "videopreview: using temp directory " << tmpdir->name() << endl;

    mplayerprocess=new QProcess();
    KUrl furl( previewingFile->getFilePath() );
    kDebug(DBG_AREA) << "videopreview: url=" << furl << "; local:" << furl.isLocalFile() << endl;


    QStringList args;
    args << playerBin << QLatin1String("\"" ) + previewingFile->getFilePath() + QLatin1String( "\"") << QLatin1String( "-nocache" ) << QLatin1String( "-identify" ) << QLatin1String( "-vo" ) << QLatin1String( "null" ) << QLatin1String( "-frames" ) << QLatin1String( "0" )/* << "-nosound" */<< QLatin1String( "-ao" ) << QLatin1String( "null" );
    args+= customargs;

    kDebug(DBG_AREA) << "videopreview: starting process: --_" << QLatin1String( " " ) << args.join(QLatin1String( " " )) << "_--\n";
    if (! startAndWaitProcess(args) ) return false;

    QString information=QString::fromLatin1(mplayerprocess->readAllStandardOutput() );
    QRegExp findInfos( QLatin1String( "ID_VIDEO_FPS=([\\d]*).*ID_LENGTH=([\\d]*).*" ));
    if(findInfos.indexIn( information) == -1 )
    {
        kDebug(DBG_AREA) << "videopreview: No information found, exiting\n";
        return false;
    }
    previewingFile->setTotalTime(findInfos.cap(2).toInt() * 1000);
    previewingFile->setFPS(findInfos.cap(1).toInt());

    kDebug(DBG_AREA) << "videopreview: find length=" << previewingFile->getMillisecondsLength() << " ms, fps=" << previewingFile->getFPS() << endl;
    return true;
}


MPlayerVideoBackend::~MPlayerVideoBackend() {
  foreach(ArgsCalculator *argsCalculator, seekArguments) delete argsCalculator;
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
    args << playerBin << QLatin1String( "\"" ) + previewingFile->getFilePath() + QLatin1String( "\"" );
    if(previewingFile->isWide()) scalingHeight=-2; else scalingWidth=-2;

    args << seekArguments[frameSelector->seekStrategy()]->args(frameSelector);

    KMD5 md5builder(previewingFile->getFilePath().toLatin1() );
    QString md5file=QLatin1String( md5builder.hexDigest().data() );
    QString tmpDirPath = tmpdir->name() + md5file + QDir::separator();
    args << QLatin1String( "-nocache" )
    // << "-idx" /*@TODO check if it's too slow..*/ Update: probably yes...
    << QLatin1String( "-ao" ) << QLatin1String( "null" )/*"-nosound" << */<< QLatin1String( "-speed" ) << QLatin1String( "99" )  /*<< "-sstep" << "5"*/
         << QLatin1String( "-vo" ) << QString::fromLatin1("jpeg:outdir=%1").arg(tmpDirPath )
//  << "-vf" << QString("scale=%1:%2").arg(scalingWidth).arg(scalingHeight) // This is probably just a waste of resources, as KDE API suggests to not scale the image.
    ;
    args+=customargs;

    if (! startAndWaitProcess(args) ) return NULL;

    kDebug(DBG_AREA) << "videopreview: temp dir '" << tmpDirPath << "'\n";

    if (QDir(tmpDirPath ).entryList( QStringList(QLatin1String( "*.jpg" )) ).isEmpty() ) return NULL;

    QString lastframe=QDir(tmpDirPath ).entryList( QStringList(QLatin1String( "*.jpg" )) ).last();
    kDebug(DBG_AREA) << "videopreview: LastFrame==" << lastframe << endl;
    QImage *retImage = new QImage(tmpDirPath.append( lastframe ));
    return new Thumbnail(retImage, previewingFile);
}


void MPlayerVideoBackend::tryUnlink(KTempDir *dir) {
    if(dir) dir->unlink();
}


bool MPlayerVideoBackend::findPlayerBin() {
    playerBin=mplayerThumbsConfig->mplayerbin();
    customargs=mplayerThumbsConfig->customargs().split(QLatin1Char( ' ' ));
    kDebug(DBG_AREA) << "videopreview: customargs=" << mplayerThumbsConfig->customargs() << " ;;;; " << customargs << endl;
    if(playerBin.length()) kDebug(DBG_AREA) << "videopreview: found playerbin from config: " << playerBin << endl;
    else
    {
        playerBin=KStandardDirs::findExe(QLatin1String( "mplayer-bin" ));
        if(!playerBin.length()) playerBin=KStandardDirs::findExe(QLatin1String( "mplayer" ));
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
    mplayerprocess->start( args.join(QLatin1String( " " )) );
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


