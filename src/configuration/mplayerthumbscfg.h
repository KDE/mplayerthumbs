/***************************************************************************
   Copyright (C) 2006-2009
   by Marco Gulino <marco.gulino@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
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

#ifndef MPLAYERTHUMBSCONFIG_H
#define MPLAYERTHUMBSCONFIG_H
#include <kconfigdialog.h>
#include "mplayerthumbs.h"
#include "ui_configDialog.h"
#include "ui_mplayerConfig.h"


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
	Ui::mplayerConfig *mplayerConfigUI;

public slots:
    void autoFindPath();
    void cleanCache();
};

#endif
