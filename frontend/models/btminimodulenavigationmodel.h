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

#ifndef BTMINIMODULENAVIGATIONMODEL_H
#define BTMINIMODULENAVIGATIONMODEL_H

#include <QAbstractItemModel>

class BtMiniModuleNavigationModelPrivate;

/** Provide tree for module map.
*/
class BtMiniModuleNavigationModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    BtMiniModuleNavigationModel(QString module, QObject *parent = 0);
    ~BtMiniModuleNavigationModel();
    
    /** Reimplemented from QAbstractItemModel. */
    int           columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant      data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QModelIndex   index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex   parent(const QModelIndex &index) const;
    int           rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool          hasChildren(const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant      headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
    /** Change reference model module. */
    void setModule(QString &module);

    /** Convert given Sword key to model index. */
    QModelIndex keyToIndex(QString key) const;

    /** Convert given model index to Sword key. */
    QString indexToKey(const QModelIndex &index) const;

    /** */
    void setIndicator(QWidget *w);

public slots:
    /** */
    void updateIndicator(QModelIndex index);

protected:

private:
    Q_DECLARE_PRIVATE(BtMiniModuleNavigationModel)
    BtMiniModuleNavigationModelPrivate * const d_ptr;

};

#endif
