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

#ifndef VIDEOBACKENDIFACE_H
#define VIDEOBACKENDIFACE_H

#include <QObject>
#include <QPixmap>
#include "frameselector.h"

class PreviewingFile;
class MPlayerThumbsCfg;
class Thumbnail;
class VideoBackendIFace
{
public:
  VideoBackendIFace(PreviewingFile *previewingFile, MPlayerThumbsCfg* cfg);
  bool cannotPreview();
  enum Backend { MPlayer=0, Phonon=1 };

  virtual Thumbnail *preview(FrameSelector *frameSelector) = 0;
  virtual bool readStreamInformation() = 0;
  virtual ~VideoBackendIFace();
protected:
  PreviewingFile *previewingFile;
  MPlayerThumbsCfg* mplayerThumbsConfig;
  virtual bool playerCannotPreview() = 0;
};

#endif // VIDEOBACKENDIFACE_H
