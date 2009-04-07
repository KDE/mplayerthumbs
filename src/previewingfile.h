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

#ifndef PREVIEWINGFILE_H
#define PREVIEWINGFILE_H
#include <QObject>
#include <QFileInfo>
class PreviewingFile : public QObject
{
public:
  PreviewingFile(const QString &filePath, uint scalingWidth, uint scalingHeight, QObject *parent = 0);
  bool isBlacklisted(const QStringList &blacklistedExtensions);
  void setStreamInformation(uint fps, uint secondsLength);
  uint getSecondsLength();
  uint getFPS();
  QString getFilePath() const;
  bool isWide();
  uint getScalingWidth();
  uint getScalingHeight();
private:
  QFileInfo fileInfo;
  uint fps;
  uint secondsLength;
  uint scalingWidth;
  uint scalingHeight;

};

#endif // PREVIEWINGFILE_H
