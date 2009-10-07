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

#include "constants.h"
#include "servicesfactory.h"
#include "previewingfile.h"
#include "videobackendiface.h"
#include "mplayervideobackend.h"
#include "mplayerthumbs.h"
#include "mplayerthumbs-config.h"


#ifdef PHONON_API
	#include "phononbackend.h"
#endif

PreviewingFile* ServicesFactory::previewingFile(const QString& filePath, unsigned int scalingWidth, unsigned int scalingHeight, QObject* parent) {
  return new PreviewingFile(filePath, scalingWidth, scalingHeight, parent);
}

VideoBackendIFace *ServicesFactory::videoBackend(PreviewingFile* previewingFile, MPlayerThumbsCfg* cfg) {
  kDebug(DBG_AREA) << "videopreview: backend: " << cfg->backend() << endl;
  switch(cfg->backend() ) {
#ifdef PHONON_API
    case VideoBackendIFace::Phonon:
      kDebug(DBG_AREA) << "videopreview: Selected phonon backend\n";
      return new PhononBackend(previewingFile, cfg);
	  break;
    case VideoBackendIFace::MPlayer:
#else
    #ifdef __GNUC__
    #warning using mplayer as only backend
    #endif
    default:
#endif
      kDebug(DBG_AREA) << "videopreview: Selected mplayer backend\n";
      return new MPlayerVideoBackend(previewingFile, cfg);
      break;
  }
  // Well, we should never be here...
  return NULL;
}

MPlayerThumbsCfg* ServicesFactory::config() {
  return MPlayerThumbsCfg::self();
}

ServicesFactory::~ServicesFactory() {

}


