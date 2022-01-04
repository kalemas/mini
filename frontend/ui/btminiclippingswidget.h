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

#ifndef BTMINICLIPPINGSWIDGET_H
#define BTMINICLIPPINGSWIDGET_H


#include "btminiwidget.h"


class BtMiniClippingsWidgetPrivate;
class CSwordModuleInfo;

class BtMiniClippingsWidget : public BtMiniWidget
{
    Q_OBJECT

public:

    BtMiniClippingsWidget(QWidget * parent = 0);
    ~BtMiniClippingsWidget();

public:
    /**
     * \param list if default - item will be added to the last viewed bookmarks folder.
     */
    static void insertBookmark(const CSwordModuleInfo * module, QString key, int where = -1, int list = -1);

    static QStringList lists();

    //void insertList(int where, QString & name = QString());

public slots:


private slots:
    /** */
    void currentIndexChanged(const QModelIndex & index);

    void renameCurrentList();

    void openMenu(const QModelIndex &index);

private:
    Q_DECLARE_PRIVATE(BtMiniClippingsWidget)
    BtMiniClippingsWidgetPrivate * const d_ptr;

};

#endif // BTMINICLIPPINGSWIDGET_H
