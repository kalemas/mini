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

#ifndef BTMINILAYOUTDELEGATE_H
#define BTMINILAYOUTDELEGATE_H

#include <QAbstractItemModel>
#include <QVector>

#include "btmini.h"

/** Options for subviews. There are always options for at least
    zero (default) level.
*/
struct BtMiniLevelOption
{
    BtMiniLevelOption();
    
    QString preText;
    QString postText;

    int     perLine;

    /** Layout full model into the view (if 0) or use batched layout. */
    int     perCycle;
    
    /** Remove items that could be unnecessary (faraway from viewport). */
    bool    limitItems;
    
	Qt::ScrollBarPolicy scrollBarPolicy;
    bool    scrollPerItem;

    bool    useThread;
    int     searchRole;

	bool    allowStaticText;
};

/** Class that controls layout of subviews and items in view.
    This class doesn't know anything of view or model, isn't it good?
*/
class BtMiniLayoutDelegate : public QObject
{
    Q_OBJECT

public:
    BtMiniLayoutDelegate(QObject *parent = 0);
    virtual ~BtMiniLayoutDelegate();
    
    /** Get/set options for given layout level. Levels are usually represents
        subviews. */
    const BtMiniLevelOption & levelOption(int level = 0) const;
    void setLevelOption(int level, BtMiniLevelOption &option);
    void setLevelOption(BtMiniLevelOption &option);
    void eraseLevelOption(int level);
    inline int levelOptionsCount() const { return _options.size(); }
    
    /** Plain or Tree layout mode.
        Plain layout should have first row of items that will not be displayed, view
        will have subview for each of that items, contain their children. */
    inline bool plainMode() const { return _plain; }
    inline void setPlainMode(bool mode) { _plain = mode; }


protected:
    QVector<BtMiniLevelOption> _options;
    bool                        _plain;

};

#endif
