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

#ifndef SERVICESFACTORY_H
#define SERVICESFACTORY_H
#include <QString>
#include <QObject>
class PreviewingFile;
class VideoBackendIFace;
class MPlayerThumbsCfg;

class ServicesFactory
{
public:
  virtual PreviewingFile *previewingFile(const QString &filePath, unsigned int scalingWidth, unsigned int scalingHeight, QObject *parent = 0);
  virtual VideoBackendIFace *videoBackend(PreviewingFile *previewingFile, MPlayerThumbsCfg* cfg);
  virtual MPlayerThumbsCfg *config();

  virtual ~ServicesFactory();
};

#endif // SERVICESFACTORY_H
