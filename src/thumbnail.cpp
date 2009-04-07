/*
   Copyright (C) 2006-2009
   by Marco Gulino <marco.gulino@gmail.com>

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

#include "thumbnail.h"
#include "constants.h"
#include <QPixmap>
#include <kdebug.h>
#include <QImage>
#include <QVarLengthArray>

Thumbnail::Thumbnail(QPixmap* pixmap, QObject* parent) :
  QObject(parent)
{
  this->variance=0;
  this->pixmap=pixmap;
  if(pixmapIsValid() ) calculateVariance();
}

Thumbnail::~Thumbnail() {
  delete pixmap;
}

bool Thumbnail::pixmapIsValid() {
  return pixmap && ! pixmap->isNull();
}


unsigned int Thumbnail::getVariance() {
  return variance;
}

QPixmap* Thumbnail::getPixmap() {
  return pixmap;
}


void Thumbnail::calculateVariance() {
    QImage image = pixmap->toImage();
    uint delta=0;
    uint avg=0;
    uint bytes=image.numBytes();
    uint STEPS=bytes/2;
    QVarLengthArray<uchar> pivot(STEPS);
    kDebug(DBG_AREA) << "Using " << STEPS << " steps\n";
    uchar *bits=image.bits();
    // First pass: get pivots and taking average
    for( uint i=0; i<STEPS ; i++ ){
        pivot[i]=bits[i*(bytes/STEPS)];
        avg+=pivot[i];
    }
    avg=avg/STEPS;
    // Second Step: calculate delta (average?)
    for (uint i=0; i<STEPS; i++)
    {
        int curdelta=abs(int(avg-pivot[i]));
        delta+=curdelta;
    }
    variance= delta/STEPS;
}


#include "thumbnail.moc"