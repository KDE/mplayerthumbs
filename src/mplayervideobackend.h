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

#ifndef MPLAYERVIDEOBACKEND_H
#define MPLAYERVIDEOBACKEND_H
#include <QtCore/QMap>
#include <videobackendiface.h>
class KTempDir;
class QProcess;
class ArgsCalculator {
	public:
		ArgsCalculator(PreviewingFile* previewingFile);
		virtual QStringList args(FrameSelector *frameSelector) = 0;
		virtual ~ArgsCalculator() {};
	protected:
		PreviewingFile *previewingFile;
};
class MPlayerVideoBackend : public VideoBackendIFace
{
public:
	MPlayerVideoBackend(PreviewingFile *previewingFile,  MPlayerThumbsCfg* cfg);
	virtual ~MPlayerVideoBackend();
  virtual Thumbnail* preview(FrameSelector *frameSelector);
	virtual bool playerCannotPreview();
	virtual bool readStreamInformation();

private:
  bool findPlayerBin();
  bool startAndWaitProcess(const QStringList &args);
	void tryUnlink(KTempDir *dir);

  QString playerBin;
  QProcess *mplayerprocess;
  QStringList customargs;
	KTempDir *tmpdir;
	QMap<FrameSelector::SeekStrategy, ArgsCalculator*> seekArguments;
};

#endif // MPLAYERVIDEOBACKEND_H
// kate: indent-mode cstyle; replace-tabs off; tab-width 2; 
