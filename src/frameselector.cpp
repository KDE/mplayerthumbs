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

#include "frameselector.h"
#include "previewingfile.h"

class RandomFrameSelectorPrivate {
  public:
    uint beginPercentPosition;
    uint endPercentPosition;
};

RandomFrameSelector::RandomFrameSelector(unsigned int beginPercentPosition, unsigned int endPercentPosition) {
  d=new RandomFrameSelectorPrivate();
  d->beginPercentPosition=beginPercentPosition;
  d->endPercentPosition=endPercentPosition;
}

RandomFrameSelector::~RandomFrameSelector() {
  delete d;
}

qint64 RandomFrameSelector::framePositionInMilliseconds(PreviewingFile* previewingFile) {

}




class PlainFrameSelectorPrivate {
  public:
    qint64 millisecondsToSkip;
};

PlainFrameSelector::PlainFrameSelector(qint64 millisecondsToSkip) {
  d=new PlainFrameSelectorPrivate();
  d->millisecondsToSkip=millisecondsToSkip;
}

PlainFrameSelector::~PlainFrameSelector() {
  delete d;
}


qint64 PlainFrameSelector::framePositionInMilliseconds(PreviewingFile* previewingFile) {
  qint64 fileLength=previewingFile->getSecondsLength()*1000;
  return (d->millisecondsToSkip>fileLength)?(fileLength):(d->millisecondsToSkip);
}
