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

#ifndef BTMINIMODULETEXTMODEL_H
#define BTMINIMODULETEXTMODEL_H

#include <QAbstractItemModel>

class BtMiniModuleTextModelPrivate;

/** Standard model for module contents display.
    This model have own layout delegate.
*/
class BtMiniModuleTextModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(QString module READ getModule WRITE setModule)

public:
    BtMiniModuleTextModel(QStringList modules, QObject *parent = 0);
    explicit BtMiniModuleTextModel(QObject *parent = 0);
    ~BtMiniModuleTextModel();
    
    /** Reimplemented from QAbstractItemModel. */
    int             columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant        data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags   flags(const QModelIndex &index) const;
    bool            hasChildren(const QModelIndex &parent) const;
    QVariant        headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex     index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndexList match(const QModelIndex &start, int role, const QVariant &value, int hits = 1,
                          Qt::MatchFlags flags = Qt::MatchFlags( Qt::MatchStartsWith | Qt::MatchWrap )) const;
    QModelIndex     parent(const QModelIndex &index) const;
    QHash<int, QByteArray> roleNames() const;
    int             rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * @param role BtMini::PlaceRole will set scope for index's list, if value is empty
     *             scope will be removed (if was set)
     *             BtMini::ModuleRole will set/change module
     *             roles above would be set for parent index representing module and child,
     *             representing verse
     *             Qt::DisplayRole will set text data for item in non-module/content lists
     */
    bool            setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool            insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
    bool            removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
    /** */
    QString indexModule(const QModelIndex &index) const;
    
    /** */
    QString indexKey(const QModelIndex &index) const;

    /** */
    QModelIndex keyIndex(int i, QString key) const;
    Q_INVOKABLE int keyIndex(QString key) const;

    /** for qml */
    QString getModule() const;
    void setModule(QString module);


public slots:
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
    void updateIndicators(const QModelIndex &index);

	/** Set text to search. */
	void setSearchText(const QString &text);

	/** Perform search and fill model with results. Model should be initialized
		with only one "[Search]" item. Search module will be taken from current
		workWidget module. */
	void startSearch();

	/** Called after module installation. */
	void modulesReloaded();

    /** */
    void closeContext();
    void closeModuleSelection();
    void closePlaceSelection();

protected:

private:
    Q_DECLARE_PRIVATE(BtMiniModuleTextModel)
    BtMiniModuleTextModelPrivate * const d_ptr;

};

#endif
