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


#ifndef _videopreview_H_
#define _videopreview_H_

#include <qstring.h>

#include <qobject.h>
#include <qpixmap.h>
#include <kio/thumbsequencecreator.h>

class QProcess;
class KTempDir;
class KRandomSequence;
class ServicesFactory;
class QFileInfo;
class MPlayerThumbsCfg;
class VideoPreview : public QObject, public ThumbSequenceCreator
{
Q_OBJECT
    public:
	VideoPreview(ServicesFactory *servicesFactory);
        virtual ~VideoPreview();
        virtual bool create(const QString &path, int width, int height, QImage &img);
        virtual Flags flags() const;

    protected:
        static uint imageVariance(QImage image );
    private:
	ServicesFactory *servicesFactory;
};

#endif
