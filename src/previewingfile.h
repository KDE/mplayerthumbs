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

#ifndef PREVIEWINGFILE_H
#define PREVIEWINGFILE_H
#include <QObject>
#include <QFileInfo>
class Thumbnail;
class VideoBackendIFace;
class PreviewingFilePrivate;
class PreviewingFile : public QObject
{
Q_OBJECT
public:
  PreviewingFile(const QString &filePath, uint scalingWidth, uint scalingHeight, QObject *parent = 0);
  ~PreviewingFile();
  bool isBlacklisted(const QStringList &blacklistedExtensions);
  quint64 getMillisecondsLength();
  uint getFPS();
  QString getFilePath() const;
  bool isWide();
  uint getScalingWidth();
  uint getScalingHeight();
  Thumbnail *getPreview(VideoBackendIFace *videoBackend, uint minVariance, uint maxTries, float sequenceIndex);
public slots:
  void setTotalTime(quint64 totalTime);
  void setFPS(uint fps);

private:
  PreviewingFilePrivate *d;
};

#endif // PREVIEWINGFILE_H
