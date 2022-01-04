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

#include "../../bibletime/src/backend/config/btconfig.h"

#include "btminilayoutdelegate.h"

BtMiniLevelOption::BtMiniLevelOption()
{
    perLine         = 1;
    scrollBarPolicy = Qt::ScrollBarAsNeeded;
    perCycle        = 0;
    limitItems      = false;
    useThread       = false;
	scrollPerItem   = false;
	searchRole      = BtMini::PlaceRole;
	allowStaticText = true;
}

BtMiniLayoutDelegate::BtMiniLayoutDelegate(QObject *parent) : QObject(parent)
{
    _options.resize(1);
    _plain = false;
}

BtMiniLayoutDelegate::~BtMiniLayoutDelegate()
{
    ;
}

void BtMiniLayoutDelegate::setLevelOption(BtMiniLevelOption &option)
{
    _options.resize(1);
    _options[0] = option;
}

void BtMiniLayoutDelegate::setLevelOption(const int level, BtMiniLevelOption &option)
{
    Q_ASSERT(_options.size() > 0);

    while(_options.size() <= level)
        _options.append(_options[_options.size() - 1]);

    _options[level] = option;
}

const BtMiniLevelOption & BtMiniLayoutDelegate::levelOption(const int level) const
{
    Q_CHECK_PTR(this);

    return _options[qMin(level, _options.size() - 1)];
}

void BtMiniLayoutDelegate::eraseLevelOption(const int level)
{
    if(level < _options.size())
        _options.erase(_options.begin() + level);
}
