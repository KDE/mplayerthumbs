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

#ifndef MPLAYERVIDEOBACKEND_H
#define MPLAYERVIDEOBACKEND_H

#include <videobackendiface.h>

class KRandomSequence;
class MPlayerVideoBackend : public VideoBackendIFace
{
public:
	MPlayerVideoBackend(const QString &filePath,  MPlayerThumbsCfg* cfg);
	virtual ~MPlayerVideoBackend();
  virtual VideoPreview::FileInformation findFileInfo();
  virtual QPixmap getVideoFrame(int flags, VideoPreview::FileInformation fileinfo);
	virtual bool canPreview();
private:
  bool findPlayerBin();
  bool startAndWaitProcess(const QStringList &args);
	void tryUnlink(KTempDir *dir);

  QString playerBin;
  QProcess *mplayerprocess;
  QStringList customargs;
	KTempDir *tmpdir;
	KRandomSequence *rand;

};

#endif // MPLAYERVIDEOBACKEND_H
// kate: indent-mode cstyle; replace-tabs off; tab-width 2; 
