/*
   Copyright (C) 2006-2009
   by Marco Gulino <marco.gulino@gmail.com>

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

#include "previewingfile.h"
#include "constants.h"
#include <kdebug.h>
#include <QStringList>
#include "thumbnail.h"
#include "thumbnailsmap.h"
#include "videobackendiface.h"
PreviewingFile::PreviewingFile(const QString& filePath, uint scalingWidth, uint scalingHeight, QObject* parent)
  : QObject(parent), fileInfo(filePath)
{
  this->scalingHeight=scalingHeight;
  this->scalingWidth=scalingWidth;
}


bool PreviewingFile::isBlacklisted(const QStringList& blacklistedExtensions) {
    QString extension=fileInfo.suffix().trimmed();
    kDebug(DBG_AREA) << "videopreview: file extension=\"" << extension << "\"\n";
    if( extension.length() && !blacklistedExtensions.filter(extension, Qt::CaseInsensitive).isEmpty() )
    {
        kDebug(DBG_AREA) << "videopreview: matched extension " << extension.prepend('.') << "; exiting.\n";
        return true;
    }
    return false;
}

void PreviewingFile::setStreamInformation(unsigned int fps, unsigned int secondsLength) {
  this->fps=fps;
  this->secondsLength=secondsLength;
}

Thumbnail* PreviewingFile::getPreview(VideoBackendIFace* videoBackend, uint minVariance, unsigned int maxTries) {
  kDebug() << "getPreview with minVariance: " << minVariance << endl;
  ThumbnailsMap thumbnailsMap;
  Preview::frameflags flags;
  while(thumbnailsMap.hasAGoodImageOrSurrenders(minVariance, maxTries)) {
    flags=(thumbnailsMap.size()>=maxTries) ? Preview::framestart : Preview::framerandom;
    thumbnailsMap.addThumbnail(videoBackend->preview(flags) );
    kDebug() << "try " << thumbnailsMap.size();
  }
  return thumbnailsMap.getBestThumbnail();
}


unsigned int PreviewingFile::getFPS() {
  return this->fps;
}

unsigned int PreviewingFile::getSecondsLength() {
  return this->secondsLength;
}

QString PreviewingFile::getFilePath() const {
  return fileInfo.absoluteFilePath();
}

bool PreviewingFile::isWide() {
  return scalingWidth>scalingHeight;
}

unsigned int PreviewingFile::getScalingHeight() {
  return this->scalingHeight;
}

unsigned int PreviewingFile::getScalingWidth() {
  return this->scalingWidth;
}

#include "previewingfile.moc"




