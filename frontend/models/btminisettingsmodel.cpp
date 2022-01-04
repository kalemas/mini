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

#include <QApplication>
#include <QFontDatabase>
#include <QDesktopServices>
#include <QStyleFactory>
#include <QUrl>

#include <QtDebug>

#include <swversion.h>

#include "../../bibletime/src/backend/config/btconfig.h"
#include "../../bibletime/src/backend/cswordmodulesearch.h"

#include "btmini.h"
#include "view/btminiview.h"
#include "btminisettingsmodel.h"
#include "view/btminilayoutdelegate.h"
#include "ui/btminimenu.h"
#include "ui/btminiui.h"

#define BT_MINI_FORUM_URL "http://sourceforge.net/p/bibletimemini/discussion"

struct Item
{
    enum Type
    {
        None,
        FontSize,
        FontFamily,
        FontFamilies,
        FontTextSize,
        FontTextFamily,
        FontTextFamilies,
        Threads,
        ContinuousScrolling,
        UseWebKit,
        SearchType,
        Style,
        KeepAwake,
        Forum
    };

    Item(Type type, QString text, QVariant data = QVariant())
        : _type(type), _text(text), _data(data) {;}
    ~Item()
    {
        foreach(Item *i, _children)
            delete i;
    }

    int            _type;
    QString        _text;
    QVariant       _data;
    QList<Item*>   _children;
};


class BtMiniSettingsModelPrivate
{
public:

    BtMiniSettingsModelPrivate()
    {
        QString tbs = "<table width=\"100%\" cellpadding=\"5\"><tr><td>";
        QFontDatabase fd;

        _items.append(new Item(Item::None, "<body><font size=\"50%\"><center>"
            + BtMiniSettingsModel::tr("Settings") + "</center></font></body>"));
        _items.append(new Item(Item::FontSize, tbs + BtMiniSettingsModel::tr("Font size:") +
            "</td> <td align=\"right\"><b>%1%</b></td></tr></table>"));
        _items.append(new Item(Item::None, "<b>" + tbs + BtMiniSettingsModel::tr("Font settings:") +
            "</td> <td align=\"right\"> > </td></tr></table></b>"));
        _items.last()->_children.append(new Item(Item::FontTextSize, tbs + BtMiniSettingsModel::tr("Font size for text:") +
            "</td> <td align=\"right\"><b>%1%</b></td></tr></table>"));
        _items.last()->_children.append(new Item(Item::None, "<b>" + tbs + BtMiniSettingsModel::tr("Interface font") +
            "</td> <td align=\"right\"> > </td></tr></table></b>"));
        _items.last()->_children.last()->_children.append(new Item(Item::FontFamilies, "<body><font size=\"+1\" face=\"" +
			QApplication::font().family() + "\">" + QObject::tr("Default") + "</font></body>", QApplication::font().family()));
        foreach(QString f, fd.families())
            _items.last()->_children.last()->_children.append(new Item(Item::FontFamilies,
                "<body><font size=\"+1\" face=\"" + f + "\">" + f + "</font></body>", f));
        _items.last()->_children.append(new Item(Item::None, "<b>" + tbs + BtMiniSettingsModel::tr("Text font") +
            "</td> <td align=\"right\"> > </td></tr></table></b>"));
        _items.last()->_children.last()->_children.append(new Item(Item::FontTextFamilies,
            "<body><font size=\"+1\" face=\"jGaramond\">" + QObject::tr("Default") + "</font></body>", "jGaramond"));
        foreach(QString f, fd.families())
            _items.last()->_children.last()->_children.append(new Item(Item::FontTextFamilies,
                "<body><font size=\"+1\" face=\"" + f + "\">" + f + "</font></body>", f));
        _items.append(new Item(Item::Threads, tbs + BtMiniSettingsModel::tr("Multi-threading:") +
            "</td> <td align=\"right\"><b>%1</b></td></tr></table>"));
        _items.append(new Item(Item::ContinuousScrolling, tbs + BtMiniSettingsModel::tr("Continuous scrolling:") +
            "</td> <td align=\"right\"><b>%1</b></td></tr></table>"));
#ifdef BT_MINI_WEBKIT
        _items.append(new Item(Item::UseWebKit, tbs + BtMiniSettingsModel::tr("Use WebKit:") +
            "</td> <td align=\"right\"><b>%1</b></td></tr></table>"));
#endif
#ifndef BT_NO_LUCENE
        _items.append(new Item(Item::SearchType, tbs + BtMiniSettingsModel::tr("Search type:") +
            "</td> <td align=\"right\"><b>%1</b></td></tr></table>"));
#endif
        _items.append(new Item(Item::Style, tbs + BtMiniSettingsModel::tr("Ui style:") +
            "</td> <td align=\"right\"><b>%1</b></td></tr></table>"));
#ifdef Q_OS_ANDROID
//        _items.append(new Item(Item::KeepAwake, tbs + BtMiniSettingsModel::tr("Keep screen awake:") +
//            "</td> <td align=\"right\"><b>%1</b></td></tr></table>"));
#endif
        _items.append(new Item(Item::None, "<b>" + tbs + BtMiniSettingsModel::tr("Handbook:") +
            "</td> <td align=\"right\"> > </td></tr></table></b>"));
        _items.last()->_children.append(new Item(Item::None, BtMiniSettingsModel::standardData(BtMiniSettingsModel::TipWorks).toString()));
        _items.append(new Item(Item::None, "<b>" + tbs + BtMiniSettingsModel::tr("Changelog") + ":" +
            "</td> <td align=\"right\"> > </td></tr></table></b>"));
        _items.last()->_children.append(new Item(Item::None, BtMiniSettingsModel::standardData(BtMiniSettingsModel::News).toString()));
        _items.append(new Item(Item::None, "<body><font size=\"50%\"><center>" +
            BtMiniSettingsModel::tr("About") + "</center></font><br/>"
            + BtMiniSettingsModel::tr("<b>BibleTime Mini</b> - spend your time with Bible on mobile!") +
            "<br/><table width=\"100%\"><tr><td>" + BtMiniSettingsModel::tr("Current version:") +
            "</td><td align=\"right\"><b>" BT_MINI_VERSION "</b></td></tr></table>"
            "<table width=\"100%\"><tr><td>" + BtMiniSettingsModel::tr("Built on:") + "</td><td align=\"right\">"
            __DATE__ "</td></tr></table><br/><br/>" +
            BtMiniSettingsModel::tr("It is cross-platform open-source Bible study application designed for mobile devices.") +
            "<br/><br/>" +
            BtMiniSettingsModel::tr("Underlying frameworks:") +
            "<table width=\"100%\"><tr><td>BibleTime:</td><td align=\"right\">" BT_VERSION "</td></tr></table>"
            "<table width=\"100%\"><tr><td>Sword project:</td><td align=\"right\">" SWORD_VERSION_STR "</td></tr></table>"
            "<table width=\"100%\"><tr><td>Qt framework:</td><td align=\"right\">" QT_VERSION_STR "</td></tr></table>"
            "</body>"));
        _items.append(new Item(Item::Forum, BtMiniSettingsModel::tr("You could post feedback, report an issue or get help throught forum:") +
            "<br/><a href=\"" BT_MINI_FORUM_URL "\">" BT_MINI_FORUM_URL "</a></body>"));
    }

    ~BtMiniSettingsModelPrivate()
    {
        foreach(Item *i, _items)
            delete i;
    }

public:
    QList<Item*>          _items;
    BtMiniLayoutDelegate *_ld;
};

BtMiniSettingsModel::BtMiniSettingsModel(QObject *parent)
    : QAbstractItemModel(parent) , d_ptr(new BtMiniSettingsModelPrivate())
{
    d_ptr->_ld = new BtMiniLayoutDelegate(this);
    BtMiniLevelOption o = d_ptr->_ld->levelOption();
    o.scrollBarPolicy = Qt::ScrollBarAlwaysOn;
    d_ptr->_ld->setLevelOption(o);


#ifdef QT_DEBUG
    // model sanity check example, lets keep it here
    {
        QModelIndexList l;
        l << QModelIndex();
        for(int i = 0; i < l.size(); i++)
        {
            for(int ii = 0; ii < rowCount(l[i]); ii++)
            {
                QModelIndex iii(index(ii, 0, l[i]));

                //qDebug() << "c:" << iii.parent().row() << iii.parent().data();
                //qDebug() << "p:" << l[i].row() << l[i].data();

                Q_ASSERT(iii.parent() == l[i]);

                l << iii;
            }
        }
    }
#endif
}

BtMiniSettingsModel::~BtMiniSettingsModel()
{
    ;
}

int BtMiniSettingsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int BtMiniSettingsModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const BtMiniSettingsModel);

    if(!parent.isValid())
        return d->_items.size();
    else
        return reinterpret_cast<Item*>(parent.internalPointer())->_children.size();
}

QModelIndex BtMiniSettingsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const BtMiniSettingsModel);

    if(!parent.isValid())
        return createIndex(row, 0, d->_items.at(row));
    else
        return createIndex(row, 0, reinterpret_cast<Item*>(parent.internalPointer())->_children[row]);
}

QModelIndex BtMiniSettingsModel::parent(const QModelIndex &index) const
{
    Q_D(const BtMiniSettingsModel);

    Item *p = reinterpret_cast<Item*>(index.internalPointer());
    if(!p)
        return QModelIndex();

    // we need such stack to get item's parent and parent's index in its parent list
    typedef QPair<const QList<Item *> *, int> StackItem;
    QList<StackItem> stack;
    stack << StackItem(&d->_items, 0);

    while(stack.size() > 0)
    {
        Item * currentChild = stack.last().first->at(stack.last().second);

        // check
        if(p == currentChild)
        {
            if(stack.size() <= 1)
                return QModelIndex();
            else
            {
                int parentIndex = stack[stack.size() - 2].second;
                Item * parentItem = stack[stack.size() - 2].first->at(parentIndex);
                return createIndex(parentIndex, 0, parentItem);
            }
        }

        // move pointer
        if(currentChild->_children.size() > 0)
            stack << StackItem(&currentChild->_children, 0); // deeper
        else
            while(++stack.last().second >= stack.last().first->size()) // next & out
                stack.removeLast();
    }

    qDebug() << "Settings model can't find parent for item" << index;
    Q_ASSERT(false);
    return QModelIndex();

}

QVariant BtMiniSettingsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const BtMiniSettingsModel);

    if (role == Qt::DisplayRole)
    {
        Item *i = reinterpret_cast<Item*>(index.internalPointer());
        switch(i->_type)
        {
        case Item::FontSize:
            return i->_text.arg(btConfig().value<int>("mini/fontScale", 100));
        case Item::FontTextSize:
            return i->_text.arg(btConfig().value<int>("mini/fontTextScale", 140));
#ifdef BT_MINI_WEBKIT
        case Item::UseWebKit:
            return i->_text.arg(btConfig().value<bool>("mini/useWebKit", false) ? tr("on") : tr("off"));
#endif
        case Item::ContinuousScrolling:
            return i->_text.arg(btConfig().value<bool>("mini/miniContinuousScrolling", false) ? tr("on") : tr("off"));
        case Item::Style:
            return i->_text.arg(btConfig().value<QString>("mini/miniStyle", "mini"));
        case Item::KeepAwake:
            {
                const int v = btConfig().value<int>("mini/keepScreenAwake", -1);
                if(v == -1)
                    return i->_text.arg("No");
                if(v == 0)
                    return i->_text.arg("Keep");
                return i->_text.arg(QString(v / 60) + " min");
            }
        case Item::Threads:
            return i->_text.arg(btConfig().value<bool>("mini/threadedTextRetrieving", true) ? tr("on") : tr("off"));
#ifndef BT_NO_LUCENE
        case Item::SearchType:
            switch(btConfig().value<int>("GUI/SearchDialog/searchType", CSwordModuleSearch::AndType))
            {
                case CSwordModuleSearch::AndType:
                    return i->_text.arg(tr("AND"));
                case CSwordModuleSearch::OrType:
                    return i->_text.arg(tr("OR"));
                case CSwordModuleSearch::FullType:
                    return i->_text.arg(tr("Full syntax"));
                default:
                    return i->_text.arg("Unknown");
            }
#endif
        default:
            return i->_text;
        }
    }

    return QVariant();
}

bool BtMiniSettingsModel::hasChildren(const QModelIndex &parent) const
{
    return (rowCount(parent) > 0);
}

Qt::ItemFlags BtMiniSettingsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

QVariant BtMiniSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

QVariant BtMiniSettingsModel::standardData(StandardData data)
{
    switch (data) {
    case TipWorks:
        return QString("<body>" + tr("<h3><center>Usefull tips:</center></h3><br/>") +
            tr("Many views in BibleTime Mini recognizes <b>short pressing</b> by finger and <b>long pressing</b>. "
               "Try to press screen on word you interested in for a second (device should vibrate ones), "
               "information (context) view for that word should open. Same for footnotes and cross-references.<br/>"
               "Try to press Text view for several seconds before device vibrates twice, service menu should open. "
               "Where you will be able to add and remove additional views for parallel (currently unsynchronized) viewing.") + "<br/><br/>" +
            tr("<b>Context view.</b> It is consist of two views: word context, and commentary context. Try to slide left/right "
               "to observe them. Default modules used for this context could be configured in Module selection window "
               "by long pressing.") + "<br/><br/>" +
            tr("<b>Search.</b> BibleTime Mini uses CLucene indexed search, this means that you have to wait for some "
               "time to generate search index database to search in the module (on 1GHz device to index heavy module like KJV, "
               "it takes for 10 minutes and on 600MHz devices same take for about hour). Switch to Find view and type to "
               "input box at the top your request. Try to use wildcards \"*\" (like \"take*\"), keywords (like \"strong:G1203\" "
               "or \"footnote:Eve\"), and combine several words. Note, you should have keyboard with Enter key on your keyboard to "
               "start search, or on some platforms there is way to insert Return Carriage symbol.<br/>Long pressing on item will bring "
			   "verse to reading window.") + "<br/><br/>" +
            tr("<b>Default Modules.</b> There are several types of modules: default Bible is used to render cross-references, "
               "default Commentary, to show in context popup, Greek and Hebrew Strongs Dictionaries to render definitions in "
               "context popup. To change default modules, when you open Module Selection popup, make long press on module you "
               "would like to make default and select corresponding action.") + "<br/><br/>" +
            tr("<b>Parallel Bible display.</b> You would open several Bible texts in one view, with respect of different verse "
               "markups. On any view with Bible opened, open module selection dialog and make long press on the module you would like "
               "to add, then select \"") + tr("Add Parallel") + "\"." + "<br/><br/>" +
            tr("<b>Dark theme.</b> You could change color scheme on dark by changing Ui Style on \"mini-night\". There are also another "
               "interface styles provided by your platform.") + "<br/><br/>" +
            QString("</body>"));
    case tipWorksAddon:
        return QString(tr("Following information you could access again in <b>Settings</b> under the <b>Handbook</b> item.<br/>"));
    case News:
        return QString("<body><b><center>" + tr("News and Updates") + ":</center></b><br/>"
                       "<b>1.1.0</b> - Copy text to clipboard.<br/>"
                       "<b>1.0.4</b> - New menu for module with About information.<br/>"
                       "<b>1.0.0</b> - Remake interface, context windows in fullscreen, font selection, some RtoL languages support.<br/>"
                       "<b>0.9.11</b> - Combined module manager by languages, so you could observe all available modules for particular language.<br/>"
                       "<b>0.9.8</b> - New logic for base font size selection. Tablets should looks better now.<br/>"
                       "<b>0.9.5</b> - Parallel Bible display, make long tap in module selection dialog and press \"Add Parallel\". "
                           "This feature is not finished and may not work as expected.<br/>"
                       "<b>0.9.3</b> - was added support for General Books and search in non Bible modules was fixed."
                       "</body>");
    default:
        return QVariant();
    }
}

void BtMiniSettingsModel::clicked(const QModelIndex &index)
{
    Q_D(const BtMiniSettingsModel);

    if(!index.isValid())
        return;

    Item *i = reinterpret_cast<Item*>(index.internalPointer());
    switch(i->_type)
    {
    case Item::FontTextFamilies:
    case Item::FontFamilies:
        {
            QString f(i->_data.toString());
            if(i->_type == Item::FontFamilies)
                btConfig().setValue("mini/fontFamily", f);
            else
                btConfig().setValue("mini/fontTextFamily", f);
            BtMiniUi::instance()->resetWidgets(true, true, true);
            BtMiniUi::instance()->activateWorks();
        }
        break;
    case Item::FontSize:
        {
            int ov = btConfig().value<int>("mini/fontScale", 100);
            int nv = BtMiniMenu::execInput(tr("Select size:"), "<b>%1%</b>", ov, 1, 1000);

            if(ov != nv)
            {
                btConfig().setValue("mini/fontScale", nv);
                emit dataChanged(index, index);
                BtMiniUi::instance()->resetWidgets(true, true, true);
            }
        }
        break;

    case Item::FontTextSize:
		{
            int ov = btConfig().value<int>("mini/fontTextScale", 140);
			int nv = BtMiniMenu::execInput(tr("Select size:"), "<b>%1%</b>", ov, 1, 1000);

			if(ov != nv)
			{
				btConfig().setValue("mini/fontTextScale", nv);
				emit dataChanged(index, index);
                BtMiniUi::instance()->resetWidgets(false, true, true);
			}
		}
        break;

#ifdef BT_MINI_WEBKIT
    case Item::UseWebKit:
        {
            bool b = !btConfig().value<bool>("mini/useWebKit", false);
            btConfig().setValue("mini/useWebKit", b);
            emit dataChanged(index, index);

            if(BtMiniUi::instance()->worksView())
                BtMiniUi::instance()->worksView()->setWebKitEnabled(b);
            if(BtMiniUi::instance()->searchView())
                BtMiniUi::instance()->searchView()->setWebKitEnabled(b);
        }
        break;
#endif

    case Item::ContinuousScrolling:
        {
            bool b = !btConfig().value<bool>("mini/miniContinuousScrolling", false);
            btConfig().setValue("mini/miniContinuousScrolling", b);
            if(BtMiniUi::instance()->worksView())
                BtMiniUi::instance()->worksView()->setContinuousScrolling(b);
            emit dataChanged(index, index);
        }
        break;

    case Item::Style:
        {
            QStringList ss(QStyleFactory::keys());
            int i = ss.indexOf(btConfig().value<QString>("mini/miniStyle", "mini"));
            QString s;
            if(i >= 0 && i < ss.size() - 1)
                s = ss[i + 1];
            else
                s = ss[0];
            btConfig().setValue("mini/miniStyle", s);
            emit dataChanged(index, index);

            BtMiniUi::instance()->resetWidgets(true, true, true);
        }
        break;

    case Item::KeepAwake:
        {
            int v = btConfig().value<int>("mini/keepScreenAwake", -1);
            if(v == 0)
            {
                v = -1;
            }
            else if(v == -1)
            {
                v = 600;
            }
            else
            {
                v = 0;
            }

            BtMini::keepScreenAwake(v);

            btConfig().setValue("mini/keepScreenAwake", v);
            emit dataChanged(index, index);
        }
        break;

    case Item::Forum:
        if(BtMiniMenu::execQuery(tr("Follow link?"), QStringList() << tr("Yes") << tr("No")) == 0)
            QDesktopServices::openUrl(QUrl(BT_MINI_FORUM_URL));
        break;

    case Item::Threads:
        btConfig().setValue("mini/threadedTextRetrieving", !btConfig().value<int>("mini/threadedTextRetrieving", true));
        emit dataChanged(index, index);

        BtMiniUi::instance()->resetWidgets(false, true, false);
        break;

#ifndef BT_NO_LUCENE
    case Item::SearchType:
        btConfig().setValue("GUI/SearchDialog/searchType", (btConfig().value<int>("GUI/SearchDialog/searchType",
                            CSwordModuleSearch::AndType) + 1) % (CSwordModuleSearch::FullType + 1));
        emit dataChanged(index, index);
		break;
#endif

    default:
        ;
    }
}
