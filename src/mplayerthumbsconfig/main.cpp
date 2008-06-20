/***************************************************************************
 *   Copyright (C) 2006 by Marco Gulino   *
 *   marco@kmobiletools.org   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include "mplayerthumbscfg.h"
#include "mplayerthumbs.h"

static const char description[] =
    I18N_NOOP("MPlayerThumbs Configuration Utility");

static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("MPlayerThumbsConfig", I18N_NOOP("MPlayerThumbsConfig"), version, description,
                     KAboutData::License_GPL, "(C) 2006 Marco Gulino", 0, 0, "marco@kmobiletools.org");
    about.addAuthor( "Marco Gulino", 0, "marco@kmobiletools.org" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;
    MPlayerThumbsConfig *mainWin = 0;

//     if (app.isSessionRestored())
//     {
//         RESTORE(MPlayerThumbsConfig);
//     }
//     else
//     {
        // no session.. just start up normally
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    /// @todo do something with the command line args here

    mainWin = new MPlayerThumbsConfig(0, "configDialog", new MPlayerThumbsCfg() );
    app.setMainWidget( mainWin );
    mainWin->show();

    args->clear();
//     }

    // mainWin has WDestructiveClose flag by default, so it will delete itself.
    return app.exec();
}
