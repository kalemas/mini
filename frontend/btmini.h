/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime Mini project. Visit 
* http://sourceforge.net/projects/bibletimemini for more information.
*
* This code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTMINI_H
#define BTMINI_H

#include <QtCore/Qt>

namespace BtMini
{
    enum ItemDataRole
    {
        /** Referenced Sword module or key (QString). */
        ModuleRole = Qt::UserRole + 256,
        PlaceRole,
        PlaceShortRole,
		RepositoryRole,
        /** Preview before thread will calculate item DisplayRole (QString). */
        PreviewRole,
        /** Like preview but then change data via signal. */
        PreviewUpdateRole
    };

    void vibrate(int miliseconds);
    void keepScreenAwake(int seconds);
};

#endif
