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

#include "phononbackend.h"
#include <QtCore/QtGlobal>
#include <phonon/phononnamespace.h>
#include <phonon/mediaobject.h>
#include "previewingfile.h"
#include <phonon/videowidget.h>
#include "thumbnail.h"
#include <kdebug.h>

using namespace Phonon;

// TODO: it seems that phonon MediaObject is not sync.
// How to handle stream information?

PhononBackend::PhononBackend(PreviewingFile* previewingFile, MPlayerThumbsCfg* cfg) 
  : VideoBackendIFace(previewingFile, cfg)
{
  mediaObject=new MediaObject(previewingFile);
  mediaObject->setCurrentSource(previewingFile->getFilePath());
  QObject::connect(mediaObject, SIGNAL(totalTimeChanged (qint64)), previewingFile, SLOT(setTotalTime(quint64 )));
  videoWidget=new VideoWidget();
  videoWidget->hide();
  createPath(mediaObject, videoWidget);
}

PhononBackend::~PhononBackend() {
  delete videoWidget;
}


Thumbnail* PhononBackend::preview(FrameSelector *frameSelector)
{
  mediaObject->pause();
  mediaObject->seek(frameSelector->framePositionInMilliseconds(previewingFile));
  QImage *image=new QImage(videoWidget->snapshot());
  kDebug() << "videopreview: phonon snapshot at ms " << mediaObject->currentTime() << " of " << mediaObject->totalTime() << ", snapshot valid: " << ! image->isNull() << endl;
  return new Thumbnail(image, previewingFile);
}

bool PhononBackend::readStreamInformation()
{
  // FPS unneeded, probably
  previewingFile->setTotalTime(mediaObject->totalTime());
  return mediaObject->hasVideo();
}

bool PhononBackend::playerCannotPreview()
{
#ifdef QT_NO_PHONON_VIDEO
  return true;
#else
  return false;
#endif
}

