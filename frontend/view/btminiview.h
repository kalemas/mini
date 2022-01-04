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

#ifndef BTMINIVIEW_H
#define BTMINIVIEW_H

#include <QAbstractItemView>

class BtMiniLayoutDelegate;
class BtMiniViewPrivate;

/** View for small screens with sliding views. Features: rendering html data,
    threaded data acquisition, kinetic scrolling.
*/
class BtMiniView : public QAbstractItemView
{
    Q_OBJECT
    Q_PROPERTY(bool topShadow READ topShadow WRITE setTopShadow)

public:
    BtMiniView(QWidget *parent = 0);
    virtual ~BtMiniView();

	/** Reimplemented from QWidget */
	QSize sizeHint() const;

    /** Reimplemented from QAbstractItemView. */
    const QModelIndex currentIndex() const;
    QModelIndex       indexAt(const QPoint &point) const;
    QRect             visualRect(const QModelIndex &index) const;
    
    /** Return content under pointer, used for word/strong lookup. Will throw if pointer
        isn't down. */
    QString currentContents() const;
    
    /** */
    const QModelIndexList currentIndexes() const;

    /** */
    int currentLevel() const;

    /** Set mode of view to contain action items. Activating any item in view will 
        cause view to close and currentIndex() will return pressed model index. */
    void setInteractive(bool mode = true);
    
    /** This cause to allocate image of size more that widget size to store render caching.
		Calls of this function (off, then on) are also used to update cache parameters. */
    void setRenderCaching(bool mode);

    /** */
    void setTopShadow(bool enable);
    bool topShadow();

    /** This function does not take ownership of delegate. Also layout delegate can be
        attached automatically if model has a children of BtMiniLayoutDelegate type.
        View should always have such delegate. */
    void setLayoutDelegate(BtMiniLayoutDelegate *layoutDelagate);
    BtMiniLayoutDelegate * layoutDelegate();
    //const BtMiniLayoutDelegate * layoutDelegate() const;
    
    /** Set specified options for given level. */
    void setLevelOptions(int level, int itemsOnLine, QString preText, QString postText);

    /** Set default role for searching indexes. This function change level options. */
    void setSearchRole(int searchRole = Qt::EditRole, int level = -1);

    /** View does not update subviews during sleeping. Scrolling would be also adjusted by
        setting \param[in] scrolling factor. */
    void setSleep(bool sleep, qreal scrolling = 0.0);

    /** */
    void setWebKitEnabled(bool enable);

    /** */
    void setContinuousScrolling(bool enable);

    /** Allow mor than one column in one view. Default is 1. */
    void setColumnsCount(int columns);

    /** Selecting text or items, by default contents of currectIndex are selected. As long
        as selection mode is active two selection markers for start and end of selection
        are displayed. */
    void selectionStart();
    void selectionEnd();
    QString selectedText();
    QModelIndexList selectedIndexes();

public slots:
    /** Reimplemented from QAbstractItemView. */
    void reset();
    void setRootIndex(const QModelIndex &index);

    /** Reimplemented from QAbstractItemView. This function only set current index for
        subview and create \param index item only, clear if index is not exists. */
    void setCurrentIndex(const QModelIndex &index);

    /** Activate and switch next/prev subview with animation. */
    bool slideLeft();
    bool slideRight();

	/** Reimplemented from QAbstractItemView. "Well documented function". */
	void doItemsLayout();

	/** This function will search model for specified \param data with match() and
	    then scroll to the first occurrence. Only active subview is used. Role for 
	    match() can be set using setSearchRole(). */
	void scrollTo(QVariant data);

    /** Reimplemented from QAbstractItemView. This function will activate target subview
        fill it with items and then center view on \param index. Specified index will be
        also current. */
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);


signals:
    /** Emitted when user have long pressed item.*/
    void shortPressed(const QModelIndex &index);
    
    /** Very long press. */
    void longPressed(const QModelIndex &index);

    /** Emitted when user press item or slide view. */
    void currentChanged(const QModelIndex &index);

    /** In interactive views emmited when index was selected. */
    void selected(const QModelIndex &index);

protected slots:
    /** Reimplemented from QAbstractItemView. */
#if QT_VERSION < 0x050000
    void dataChanged(const QModelIndex &from, const QModelIndex &to);
#else
    void dataChanged(const QModelIndex &from, const QModelIndex &to, const QVector<int> & roles = QVector<int> ());
#endif

protected:
    /** Reimplemented from QWidget. */
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void timerEvent(QTimerEvent *e);
    void showEvent(QShowEvent *e);
	void keyPressEvent(QKeyEvent *e);

    /** Reimplemented from QAbstractItemView. */
    int         horizontalOffset() const;
    bool        isIndexHidden(const QModelIndex &index) const;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    void        setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    int         verticalOffset() const;
    bool        viewportEvent(QEvent *e);
    QRegion     visualRegionForSelection(const QItemSelection &selection) const;
    void        rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void        rowsInserted(const QModelIndex &parent, int start, int end);
    
    /** Make or update view. */
    void makeSubView(int level, const QModelIndex &parent, int index);

    /** Activate or create new view if there is no view. */
    void activateSubView(int level);

    /** Scroll contents. Vertical scrolling will slide active subview up/down, 
        horizontal will slide subviews left/right. */
    void scroll(float horizontal, float vertical);
    

private:
    Q_DECLARE_PRIVATE(BtMiniView)
    BtMiniViewPrivate * const d_ptr;

};

#endif
