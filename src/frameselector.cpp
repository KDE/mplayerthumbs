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

#include "frameselector.h"
#include "previewingfile.h"
#include <krandomsequence.h>
#include <QtCore/QDateTime>
FrameSelector::~FrameSelector() {

}


class RandomFrameSelectorPrivate {
  public:
    uint beginPercentPosition;
    uint endPercentPosition;
    KRandomSequence *rand;

};

RandomFrameSelector::RandomFrameSelector(unsigned int beginPercentPosition, unsigned int endPercentPosition) {
  d=new RandomFrameSelectorPrivate();
  d->beginPercentPosition=beginPercentPosition;
  d->endPercentPosition=endPercentPosition;
  d->rand=new KRandomSequence(QDateTime::currentDateTime().toTime_t());

}

RandomFrameSelector::~RandomFrameSelector() {
  delete d->rand;
  delete d;
}

FrameSelector::SeekStrategy RandomFrameSelector::seekStrategy() {
  return FrameSelector::Random;
}

quint64 RandomFrameSelector::framePositionInMilliseconds(PreviewingFile* previewingFile) {
  quint64 start = (previewingFile->getMillisecondsLength()* d->beginPercentPosition)/100;
  quint64 end = (previewingFile->getMillisecondsLength()*d->endPercentPosition)/100;
  return (qint64)(start+(d->rand->getDouble() * (end - start) ) );
}




class PlainFrameSelectorPrivate {
  public:
    quint64 millisecondsToSkip;
};

PlainFrameSelector::PlainFrameSelector(quint64 millisecondsToSkip) {
  d=new PlainFrameSelectorPrivate();
  d->millisecondsToSkip=millisecondsToSkip;
}

PlainFrameSelector::~PlainFrameSelector() {
  delete d;
}

FrameSelector::SeekStrategy PlainFrameSelector::seekStrategy() {
  return FrameSelector::FromStart;
}


quint64 PlainFrameSelector::framePositionInMilliseconds(PreviewingFile* previewingFile) {
  quint64 fileLength=previewingFile->getMillisecondsLength();
  return (d->millisecondsToSkip>fileLength)?(fileLength):(d->millisecondsToSkip);
}
