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

#ifndef BTMINIWORKSWIDGET_H
#define BTMINIWORKSWIDGET_H


#include "btminiwidget.h"


class BtMiniWorksWidgetPrivate;

class BtMiniWorksWidget : public BtMiniWidget
{
    Q_OBJECT

public:

    BtMiniWorksWidget(QWidget * parent = 0);
    ~BtMiniWorksWidget();

public slots:
    /** This function works with worksView only. */
    void openModuleSelection();

    /**  This function works with worksView only. */
    void openPlaceSelection();

    void currentIndexChanged(const QModelIndex & index);

    /** */
    void openContext(const QModelIndex &index);

    /** */
    void openMenu(const QModelIndex &index);

    /** */
    void selectedIndexes(const QModelIndex &index);

    /** For pressed module in BtBookshelfModel. */
    void openModuleMenu(const QModelIndex &index);

    /** Handle items for module context */
    void moduleContextClicked(const QModelIndex &index);

    /** */
    void closeContext();
    void closeModuleSelection();
    void closePlaceSelection();

    /** Add module and switch subview with animation. */
    void addWorkLeft();
    void addWorkRight();

private:
    Q_DECLARE_PRIVATE(BtMiniWorksWidget)
    BtMiniWorksWidgetPrivate * const d_ptr;

};

#endif // BTMINIWORKSWIDGET_H
