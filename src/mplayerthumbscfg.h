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

#ifndef _MPLAYERTHUMBSCONFIG_
#define _MPLAYERTHUMBSCONFIG_
#include <kconfigdialog.h>
#include "mplayerthumbs.h"
#include "ui_configDialog.h"


class KLineEdit;
class MPlayerThumbsConfig : public KConfigDialog
{
    Q_OBJECT
    public:
        MPlayerThumbsConfig(QWidget *parent, const QString &name, MPlayerThumbsCfg *config);
        ~MPlayerThumbsConfig();
        static const QString thumbnailsDir;
    private:
        KLineEdit *kcfg_mplayerbin;
        Ui::configDialog *dialogUI;

public slots:
    void autoFindPath();
    void cleanCache();
};

#endif
