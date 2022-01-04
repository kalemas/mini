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

#ifndef BTMINISETTINGSMODEL_H
#define BTMINISETTINGSMODEL_H

#include <QAbstractItemModel>

class BtMiniSettingsModelPrivate;

/** 
*/
class BtMiniSettingsModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    BtMiniSettingsModel(QObject *parent = 0);
    ~BtMiniSettingsModel();
    
    /** Reimplemented from QAbstractItemModel. */
    int           columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant      data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QModelIndex   index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex   parent(const QModelIndex &index) const;
    int           rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool          hasChildren(const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant      headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    enum StandardData
    {
        TipWorks = 0,
        tipWorksAddon,
        News
    };

    static QVariant standardData(StandardData data);

public slots:
    void clicked(const QModelIndex &index);

protected:

private:
    Q_DECLARE_PRIVATE(BtMiniSettingsModel)
    BtMiniSettingsModelPrivate * const d_ptr;

};

#endif
