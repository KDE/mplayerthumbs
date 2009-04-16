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

#include "previewingfile.h"
#include "constants.h"
#include <kdebug.h>
#include <QStringList>
#include "thumbnail.h"
#include "thumbnailsmap.h"
#include "videobackendiface.h"
#include "frameselector.h"

class PreviewingFilePrivate {
  public:
    QFileInfo fileInfo;
    uint fps;
    uint millisecondsLength;
    uint scalingWidth;
    uint scalingHeight;
};
PreviewingFile::PreviewingFile(const QString& filePath, uint scalingWidth, uint scalingHeight, QObject* parent)
  : QObject(parent)
{
  d=new PreviewingFilePrivate();
  d->fileInfo=filePath;
  d->scalingHeight=scalingHeight;
  d->scalingWidth=scalingWidth;
}

PreviewingFile::~PreviewingFile() {
  delete d;
}



bool PreviewingFile::isBlacklisted(const QStringList& blacklistedExtensions) {
    QString extension=d->fileInfo.suffix().trimmed();
    kDebug(DBG_AREA) << "videopreview: file extension=\"" << extension << "\"\n";
    if( extension.length() && !blacklistedExtensions.filter(extension, Qt::CaseInsensitive).isEmpty() )
    {
        kDebug(DBG_AREA) << "videopreview: matched extension " << extension.prepend('.') << "; exiting.\n";
        return true;
    }
    return false;
}

Thumbnail* PreviewingFile::getPreview(VideoBackendIFace* videoBackend, uint minVariance, unsigned int maxTries) {
  kDebug() << "getPreview with minVariance: " << minVariance << " and max tries: " << maxTries << endl;
  ThumbnailsMap thumbnailsMap;
  RandomFrameSelector randomFrameSelector(25, 75);
  PlainFrameSelector plainFrameSelector(10000);
  FrameSelector *frameSelector = &randomFrameSelector;
  
  while(! thumbnailsMap.hasAGoodImageOrSurrenders(minVariance, maxTries)) {
    Thumbnail *currentFrame=videoBackend->preview(frameSelector);
    thumbnailsMap.addThumbnail( currentFrame );
    kDebug() << "try " << thumbnailsMap.size() << ", image variance: " << currentFrame->getVariance() << endl;;
    if(thumbnailsMap.size()>=maxTries-1) frameSelector=&plainFrameSelector;
  }
  return thumbnailsMap.getBestThumbnail();
}


unsigned int PreviewingFile::getFPS() {
  return d->fps;
}

quint64 PreviewingFile::getMillisecondsLength() {
  return d->millisecondsLength;
}

QString PreviewingFile::getFilePath() const {
  return d->fileInfo.absoluteFilePath();
}

bool PreviewingFile::isWide() {
  return d->scalingWidth>d->scalingHeight;
}

unsigned int PreviewingFile::getScalingHeight() {
  return d->scalingHeight;
}

unsigned int PreviewingFile::getScalingWidth() {
  return d->scalingWidth;
}



void PreviewingFile::setTotalTime(quint64 totalTime) {
  d->millisecondsLength=totalTime;
}

void PreviewingFile::setFPS(unsigned int fps) {
  d->fps=fps;
}


#include "previewingfile.moc"




