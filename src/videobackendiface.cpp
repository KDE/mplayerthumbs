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

#include "videobackendiface.h"
#include "previewingfile.h"
#include "mplayerthumbs.h"

VideoBackendIFace::VideoBackendIFace(PreviewingFile *previewingFile,  MPlayerThumbsCfg* cfg) :
  mplayerThumbsConfig(cfg)
{
  this->previewingFile = previewingFile;
}

VideoBackendIFace::~VideoBackendIFace() {

}

bool VideoBackendIFace::cannotPreview() {
  return previewingFile->isBlacklisted( mplayerThumbsConfig->noextensions() ) || playerCannotPreview();
}
