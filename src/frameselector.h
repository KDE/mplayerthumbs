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

#ifndef FRAMESELECTOR_H
#define FRAMESELECTOR_H
#include <QtCore/QtGlobal>
class PreviewingFile;
class FrameSelector
{
public:
  enum SeekStrategy { FromStart, Random  };

  virtual quint64 framePositionInMilliseconds(PreviewingFile *previewingFile) = 0;
  virtual SeekStrategy seekStrategy() = 0;
  virtual ~FrameSelector();
};

class RandomFrameSelectorPrivate;
class RandomFrameSelector : public FrameSelector
{
  public:
    RandomFrameSelector(uint beginPercentPosition, uint endPercentPosition);
    virtual quint64 framePositionInMilliseconds(PreviewingFile *previewingFile);
    virtual ~RandomFrameSelector();
    virtual SeekStrategy seekStrategy();
    private:
      RandomFrameSelectorPrivate *d;
};

class PlainFrameSelectorPrivate;
class PlainFrameSelector : public FrameSelector
{
  public:
    PlainFrameSelector(quint64 millisecondsToSkip);
    virtual quint64 framePositionInMilliseconds(PreviewingFile *previewingFile);
    virtual ~PlainFrameSelector();
    virtual SeekStrategy seekStrategy();

  private:
    PlainFrameSelectorPrivate *d;
};

#endif // FRAMESELECTOR_H
