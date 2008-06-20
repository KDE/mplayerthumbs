/***************************************************************************
   Copyright (C) 2006-2008
   by Marco Gulino <marco@kmobiletools.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
 ***************************************************************************/


#ifndef _videopreview_H_
#define _videopreview_H_

#include <qstring.h>
#include <q3cstring.h>

#include <qpixmap.h>
#include <kio/thumbcreator.h>

class QProcess;
class Q3CString;
class KTempDir;
class KRandomSequence;
class KPixmapSplitter;
#include <qobject.h>

class VideoPreview : public QObject, public ThumbCreator
{
Q_OBJECT
    public:
        VideoPreview();
        virtual ~VideoPreview();
        virtual bool create(const QString &path, int width, int height, QImage &img);
        virtual Flags flags() const;
    protected:
        QPixmap getFrame(const QString &path, int flags);
        static uint imageVariance(QImage image );
    private:
        KPixmapSplitter *m_splitter;
        char *m_data;
        int m_dataSize;
        QPixmap m_pixmap;
        QProcess *mplayerprocess;
        QStringList customargs;
        KTempDir *tmpdir;
        KRandomSequence *rand;
        QString playerBin;
        bool startAndWaitProcess(const QStringList &args);
        enum frameflags { framerandom=0x1, framestart=0x2, frameend=0x4 };
        struct { int towidth; int toheight; int fps; int seconds; } fileinfo;
};

#endif
