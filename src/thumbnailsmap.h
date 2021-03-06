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

#ifndef THUMBNAILSMAP_H
#define THUMBNAILSMAP_H
#include <QHash>
class Thumbnail;
class ThumbnailsMap
{
public:
  bool hasAGoodImageOrSurrenders(uint minVariance, uint maxImages);
  void addThumbnail(Thumbnail *thumbnail);
  Thumbnail* getBestThumbnail();
  uint size();
private:
  uint bestVariance();
  bool hasAGoodImage(uint minVariance);
  QHash<uint, Thumbnail*> thumbnails;

};

#endif // THUMBNAILSMAP_H
