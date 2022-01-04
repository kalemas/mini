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
#include <QAbstractButton>
#include <QBoxLayout>
#include <QClipboard>
#include <QLabel>
#include <QMutex>
#include <QMutexLocker>
#include <QPushButton>
#include <QStringList>
#include <QStringListModel>
#include <QThread>
#include <QtDebug>

#include "../../bibletime/src/backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../../bibletime/src/backend/config/btconfig.h"
#include "../../bibletime/src/backend/cswordmodulesearch.h"
#include "../../bibletime/src/backend/keys/cswordldkey.h"
#include "../../bibletime/src/backend/keys/cswordtreekey.h"
#include "../../bibletime/src/backend/drivers/cswordbiblemoduleinfo.h"
#include "../../bibletime/src/backend/drivers/cswordlexiconmoduleinfo.h"
#include "../../bibletime/src/backend/drivers/cswordbookmoduleinfo.h"
#include "../../bibletime/src/backend/managers/cswordbackend.h"
#include "../../bibletime/src/backend/rendering/btinforendering.h"
#include "../../bibletime/src/backend/rendering/cdisplayrendering.h"
#include "../../bibletime/src/backend/rendering/centrydisplay.h"
#include "../../bibletime/src/backend/rendering/ctextrendering.h"
#include "../../bibletime/src/backend/btglobal.h"
#include "../../bibletime/src/frontend/cinfodisplay.h"

#include "btmini.h"
#include "btminimoduletextmodel.h"
#include "models/btminimodulenavigationmodel.h"
#include "view/btminilayoutdelegate.h"
#include "view/btminiview.h"
#include "ui/btminimenu.h"
#include "ui/btminipanel.h"
#include "ui/btminiui.h"


QMutex BtMiniSwordMutex;

class BtMiniModuleTextModelPrivate
{
public:
    BtMiniModuleTextModelPrivate()
    {
		_isSearch = false;
        _singleModule = false;
    }
    
    ~BtMiniModuleTextModelPrivate()
    {
        ;
    }

	struct List
	{
		List()
		{
			init();
		}

		List(QString &name)
		{
			init();

			_name = name;

			// TODO view now not supports row without items at all
			_maxEntries = 1;

            _displayOptions = btConfig().getDisplayOptions();
            _filterOptions = btConfig().getFilterOptions();

			setModule(_name);
		}

		void init()
		{
			_name = "";
			_module = 0;
			_maxEntries = 0;
			_firstEntry = 0;
			_hasScope = false;
			_hasContents = false;
		}

		/** */
		void setModule(QString module)
		{
			if(module == "[Commentary]")
                setModule(btConfig().getDefaultSwordModuleByType("standardCommentary"));
            else if(module.contains(','))
                setModule(CSwordBackend::instance()->findModuleByName(module.section(',', 0, 0)));
			else
				setModule(CSwordBackend::instance()->findModuleByName(module));
		}

		void setModule(CSwordModuleInfo *module)
		{
			_module = module;

			if(!_module)
				return;

			if(_module->type() == CSwordModuleInfo::Bible ||
				_module->type() == CSwordModuleInfo::Commentary)
			{
				CSwordBibleModuleInfo *bm = qobject_cast<CSwordBibleModuleInfo*>(_module);

				_firstEntry = bm->lowerBound().getIndex();
				_maxEntries = bm->upperBound().getIndex() - _firstEntry + 1;

                _displayOptions.verseNumbers = true;
                _simpleVerseNumber = true;
			}

            if(_module->type() == CSwordModuleInfo::Lexicon)
			{
				CSwordLexiconModuleInfo *lm = qobject_cast<CSwordLexiconModuleInfo*>(_module);
				_maxEntries = lm->entries().size();
			}

            if(_module->type() == CSwordModuleInfo::GenericBook)
            {
                CSwordBookModuleInfo *bm = qobject_cast<CSwordBookModuleInfo*>(_module);

                sword::TreeKeyIdx tk(*bm->tree());
                tk.root();
                tk.firstChild();

                Q_ASSERT(tk.getOffset() == 4);

                tk.setPosition(sword::BOTTOM);

                _maxEntries = tk.getOffset() / 4;
            }
		}

		/** Set list contents to specified text. */
        void setContents(QString contents)
		{
			_hasContents = true;
			_contents = contents;

			_maxEntries = 1;
			_firstEntry = 0;
		}

		/** For sword module lists set scope of verses. Scope will not be set if \list is empty. */
        void setScope(sword::ListKey list)
		{
			_hasScope = list.Count() > 0;
			_scopeMap.clear();

            if(!_module)
                return;

            if(_module->type() == CSwordModuleInfo::Lexicon)
            {
                CSwordLexiconModuleInfo *mi = qobject_cast<CSwordLexiconModuleInfo*>(_module);

                for(int i = 0; i < list.Count(); ++i)
                {
                    CSwordLDKey k(list.GetElement(i), mi);
                    _scopeMap.append(mi->entries().indexOf(k.key()));
                }
            }
            else
            {
                for(int i = 0; i < list.Count(); ++i)
                    _scopeMap.append(list.GetElement(i)->getIndex());
            }

            Q_ASSERT(!_scopeMap.contains(-1));
        }

		QString             _name;

		CSwordModuleInfo   *_module;

		long                _maxEntries;
		long                _firstEntry;

        DisplayOptions      _displayOptions;
        FilterOptions       _filterOptions;
        bool                _simpleVerseNumber;
        bool                _introdutions;

		bool                _hasScope;
		QVector<int>        _scopeMap;

		bool                _hasContents;
		QVariant            _contents;
	};

    CSwordVerseKey indexToVerseKey(const QModelIndex &index) const
    {

        Q_ASSERT(indexDepth(index) == 2);

		const List *l = indexList(index);

        QMutexLocker locker(&BtMiniSwordMutex);
        
		CSwordVerseKey key(l->_module);

		key.setIntros(true);
		key.setIndex(l->_hasScope ? l->_scopeMap[index.row()] : index.row() + l->_firstEntry);
		
        return key;
    }

    /** Parents count. */
    inline int indexDepth(const QModelIndex &index) const
    {
        return index.isValid() ? (static_cast<unsigned int>(index.internalId()) <= _lists.size() ? 1 : 2) :
                                 _singleModule ? 1 : 0;
    }

    /** List for index. */
    List * indexList(const QModelIndex &index) const
    {
        if(indexDepth(index) == 1)
            return &_lists[index.internalId()];
        else
        {
            List *l = reinterpret_cast<List*>(index.internalId());
#ifdef QT_DEBUG
			for(int i = 0; i < _lists.size(); ++i)
				if(&_lists[i] == l)
					break;
				else if(i == _lists.size() - 1)
					Q_ASSERT(false);
#endif
            return l;
        }
    }

    /** Index of list data for model index. */
    int indexListId(const QModelIndex &index) const
    {
        if(!index.isValid())
            return -1;

        const List *l = indexList(index);
        for(int i = 0; i < _lists.size(); ++i)
            if(&_lists[i] == l)
                return i;
        
		qDebug() << l->_module->name();
        Q_ASSERT(false);
        return -1;
    }

    /** */
    void insertModule(int i, QString module)
    {
		_lists.insert(i, List());

		// insert new option
		if(_lists.size() > _ld->levelOptionsCount())
		{
            QVector<BtMiniLevelOption> os;
			for(int ii = 0; ii < _ld->levelOptionsCount(); ++ii)
				os.append(_ld->levelOption(ii));

			os.insert(i, _ld->levelOption(i));

			for(int ii = 0; ii < os.size(); ++ii)
				_ld->setLevelOption(ii, os[ii]);
		}

		setupModule(i, module);
    }

    /** */
    void eraseModule(int i)
    {
        _ld->eraseLevelOption(i);
        _lists.erase(_lists.begin() + i);
    }

	/** */
	void setupModule(int i, QString module)
	{
		_lists[i] = List(module);

		BtMiniLevelOption o;

		_isSearch = false;

		if(module.isEmpty())
		{
			;
		}
		else if(module == "[Search]")
		{
			o.scrollBarPolicy = Qt::ScrollBarAsNeeded;
			o.limitItems      = true;
			o.perCycle        = 1;
			o.scrollPerItem   = true;
			o.allowStaticText = false;

			_isSearch         = true;

            _lists[i]._simpleVerseNumber = false;
            _lists[i]._introdutions = false;
		}
		else if(module == "[Contents]")
		{
			o.scrollBarPolicy = Qt::ScrollBarAlwaysOn;
			o.limitItems      = false;
			o.perCycle        = 0;
		}
		else if(module == "[Commentary]")
		{
			o.scrollBarPolicy = Qt::ScrollBarAlwaysOn;
		}
		else
		{
            Q_CHECK_PTR(_lists[i]._module);

			o.scrollBarPolicy = Qt::ScrollBarAlwaysOff;
			o.limitItems      = true;
			o.perCycle        = 1;

			if(_lists[i]._module->type() == CSwordModuleInfo::Lexicon)
				;
            else if(btConfig().value<bool>("mini/threadedTextRetrieving"))
			{
				o.perCycle    = 3;
				o.useThread   = true;
			}
		}
		
		Q_ASSERT(i < _ld->levelOptionsCount());
		_ld->setLevelOption(i, o);
	}

	/** */
	static void searchProgress(char percent, void *data)
	{
		typedef QPair<BtMiniMenu*, CSwordModuleInfo*> Pair;
		
		Pair *p = reinterpret_cast<Pair*>(data);

		if(!p)
			return;

		BtMiniMenu *dialog = p->first;
		CSwordModuleInfo *m = p->second;

		if(dialog && m)
		{
			if(dialog->wasCanceled())
                m->module().terminateSearch = true;
			dialog->setValue(percent);
		}
	}

    mutable QList<List>         _lists;

    BtMiniLayoutDelegate       *_ld;

    QPointer<QWidget>           _leftIndicator;
    QPointer<QAbstractButton>   _moduleIndicator;
    QPointer<QAbstractButton>   _placeIndicator;
    QPointer<QWidget>           _rightIndicator;

	QString                     _searchText;
	bool                        _isSearch;

    bool                        _singleModule;

	mutable QMutex              _mutex;
};

BtMiniModuleTextModel::BtMiniModuleTextModel(QStringList modules, QObject *parent)
    : QAbstractItemModel(parent), d_ptr(new BtMiniModuleTextModelPrivate)
{
    Q_D(BtMiniModuleTextModel);

    d->_ld = new BtMiniLayoutDelegate(this);
    d->_ld->setPlainMode(true);

    for(int i = 0; i < modules.size(); ++i)
        d->insertModule(i, modules[i]);
}

BtMiniModuleTextModel::BtMiniModuleTextModel(QObject *parent)
    : QAbstractItemModel(parent), d_ptr(new BtMiniModuleTextModelPrivate)
{
    Q_D(BtMiniModuleTextModel);

    d->_ld = new BtMiniLayoutDelegate(this);
    d->_ld->setPlainMode(true);
    d->_singleModule = true;
}

BtMiniModuleTextModel::~BtMiniModuleTextModel()
{
    Q_D(BtMiniModuleTextModel);
    
    delete d;
}

int BtMiniModuleTextModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int BtMiniModuleTextModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const BtMiniModuleTextModel);
    
    switch(d->indexDepth(parent))
    {
    case 0:
        return d->_lists.size();
    case 1:
		{
            const BtMiniModuleTextModelPrivate::List *l = d->indexList(parent);

			if(l->_hasScope)
				return l->_scopeMap.size();
            if(l->_hasContents)
                return 1;
			return l->_maxEntries;
		}
    }

    return 0;
}

QModelIndex BtMiniModuleTextModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const BtMiniModuleTextModel);

    switch(d->indexDepth(parent))
    {
    case 0:
        return createIndex(row, column, row);
    case 1:
        return createIndex(row, column, (void*)&d->_lists[parent.internalId()]);
    }

    return QModelIndex();
}

QModelIndex BtMiniModuleTextModel::parent(const QModelIndex &index) const
{
    Q_D(const BtMiniModuleTextModel);

    switch(d->indexDepth(index))
    {
    case 2:
        {
            const int i = d->indexListId(index);
            return createIndex(i, 0, i);
        }
    default:
        return QModelIndex();
    }
}

QVariant BtMiniModuleTextModel::data(const QModelIndex &index, int role) const
{
    Q_D(const BtMiniModuleTextModel);

    switch(role)
	{
    case BtMini::PreviewUpdateRole:
	case BtMini::PreviewRole:
		{
            const BtMiniModuleTextModelPrivate::List *list = d->indexList(index);

            // put to thread processing
            if(role == BtMini::PreviewUpdateRole)
            {
                ;
            }


            if(list->_module->type() == CSwordModuleInfo::GenericBook)
            {
                CSwordBookModuleInfo *b = qobject_cast<CSwordBookModuleInfo*>(list->_module);
                CSwordTreeKey k(b->tree(), b);

                k.setIndex(index.row() * 4);

                return QString("<word-breaks/><font color='#aaaaaa'><center>%1</center></font><font size='1'><br>"
                               "&nbsp;</br></font>").arg(k.key().replace('/', ' '));
            }
            else
            {

                // there was issue with VerseKey::freshtext at simoultaneous call
                static CSwordVerseKey vk(list->_module);
                vk.setModule(list->_module);
                vk.setIndex(index.row() + list->_firstEntry);
                int v = vk.getVerse();

                if(v == 0)
                    return QString();

                QString r("<font color='#aaaaaa'>");

                if(v == 1)
                    r += QString("<center><b><font size='+1'>%1 %2</font></b></center>"
                    "<font size='1'><br>&nbsp;</br></font>").arg(vk.book()).arg(vk.getChapter());

                if(v > 0)
                    r += QString("<center>%1</center></font><font size='1'><br>&nbsp;</br>").arg(v);

                return r + "</font>";
            }
		}

	case Qt::DisplayRole:
        {
			switch(d->indexDepth(index))
			{
			case 1:
                {
                    const BtMiniModuleTextModelPrivate::List & l = d->_lists[index.internalId()];
                    return l._module == 0 ? "" : l._module->name();
                }
			case 2:
				{
					QString r;
                    const BtMiniModuleTextModelPrivate::List *l = d->indexList(index);

					if(l->_module && (l->_module->type() == CSwordModuleInfo::Bible ||
						l->_module->type() == CSwordModuleInfo::Commentary))
					{
						CSwordVerseKey key(d->indexToVerseKey(index));
						const int v = key.getVerse();

                        QList<const CSwordModuleInfo*> modules;
                        modules << l->_module;

                        // parallel display
                        if(l->_name.contains(','))
                            foreach(QString s, l->_name.split(',').mid(1))
                                if(CSwordModuleInfo *m = CSwordBackend::instance()->findModuleByName(s)) modules << m;

                        if(!d->_isSearch && v == 1)
                            r += "<center><b><font size='+2' face=\"" + btConfig().getDefaultFont().family() + "\">"
                                    + key.book() + " " + QString::number(key.getChapter()) + "</font></b></center>";

                        if(v != 0)
                            r += Rendering::CEntryDisplay().textKeyRendering(modules, key.key(),
                                l->_displayOptions, l->_filterOptions,
                                l->_simpleVerseNumber ? Rendering::CTextRendering::KeyTreeItem::Settings::SimpleKey :
                                                        Rendering::CTextRendering::KeyTreeItem::Settings::CompleteShort
//                                , l->_introdutions
                                );

                        if(!d->_isSearch && v == key.getVerseMax())
                            r += "<font size='1'><br>&nbsp;</font>";
					}
					else if(l->_module && l->_module->type() == CSwordModuleInfo::Lexicon)
					{
						CSwordLexiconModuleInfo *lm = qobject_cast<CSwordLexiconModuleInfo*>(l->_module);

						Rendering::CEntryDisplay ed;
                        r += ed.textKeyRendering(QList<const CSwordModuleInfo*>() << lm, lm->entries()[l->_hasScope ?
                            l->_scopeMap[index.row()] : index.row()],
                            l->_displayOptions, l->_filterOptions,
                            l->_simpleVerseNumber ? Rendering::CTextRendering::KeyTreeItem::Settings::SimpleKey :
                                                    Rendering::CTextRendering::KeyTreeItem::Settings::CompleteShort
//                            , l->_introdutions
                            );
					}
                    else if(l->_module && l->_module->type() == CSwordModuleInfo::GenericBook)
                    {
                        CSwordBookModuleInfo *b = reinterpret_cast<CSwordBookModuleInfo*>(l->_module);
                        CSwordTreeKey key(b->tree(), b);
                        key.setIndex(l->_hasScope ? l->_scopeMap[index.row()] : index.row() * 4);

						Rendering::CEntryDisplay ed;
                        r += ed.textKeyRendering(QList<const CSwordModuleInfo*>() << l->_module,
                            key.key(), l->_displayOptions, l->_filterOptions,
                            l->_simpleVerseNumber ? Rendering::CTextRendering::KeyTreeItem::Settings::SimpleKey :
                                                    Rendering::CTextRendering::KeyTreeItem::Settings::CompleteShort
//                            , l->_introdutions
                            );
                    }

                    if(d->_isSearch && !d->_searchText.isEmpty())
                        r = CSwordModuleSearch::highlightSearchedText(r, d->_searchText);

					if(l->_hasContents)
						r += l->_contents.toString();

					return r;
				}
			}
        }

    case BtMini::PlaceShortRole:
    case BtMini::PlaceRole:
		{
			const BtMiniModuleTextModelPrivate::List *l = d->indexList(index);

			if(l->_module)
			{
                if(l->_module->type() == CSwordModuleInfo::GenericBook)
                {
                    CSwordBookModuleInfo *b = qobject_cast<CSwordBookModuleInfo*>(l->_module);
                    CSwordTreeKey key(b->tree(), b);

                    key.setIndex(l->_hasScope ? l->_scopeMap[index.row()] : index.row() * 4);
                    return key.key();
                }
                else if(l->_module->type() == CSwordModuleInfo::Lexicon)
				{
					CSwordLexiconModuleInfo *lm = qobject_cast<CSwordLexiconModuleInfo*>(l->_module);
                    return lm->entries()[l->_hasScope ? l->_scopeMap[index.row()] : index.row()];
                }
                else if(d->indexDepth(index) == 2)
                {
                    if(role == BtMini::PlaceShortRole)
                        return d->indexToVerseKey(index).getShortText();
                    else
                        return d->indexToVerseKey(index).key();
                }
			}
		}
        break;
        
    case BtMini::ModuleRole:
        return d->indexList(index)->_name;
//        switch(d->indexDepth(index))
//        {
//        case 1:
//            return d->_lists[index.internalId()]._module->name();
//        case 2:
//            if(d->indexList(index)->_module)
//                return d->indexList(index)->_module->name();
//            break;
//        }
        break;
    }
    
    return QVariant();
}

bool BtMiniModuleTextModel::hasChildren(const QModelIndex &parent) const
{
    return (rowCount(parent) > 0);
}

Qt::ItemFlags BtMiniModuleTextModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

QVariant BtMiniModuleTextModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section == 0)
        return "Module";
    return QVariant();
}

QModelIndex BtMiniModuleTextModel::keyIndex(int i, QString keyName) const
{
    Q_D(const BtMiniModuleTextModel);

    if(keyName.isEmpty())
        return QModelIndex();

	Q_ASSERT(!d->_lists[i]._hasScope);

	const BtMiniModuleTextModelPrivate::List *l = &d->_lists[i];

    if(!l->_module)
        return QModelIndex();

	if(l->_module->type() == CSwordModuleInfo::Lexicon)
	{
		CSwordLexiconModuleInfo *li = qobject_cast<CSwordLexiconModuleInfo*>(l->_module);
        int i = li->entries().indexOf(keyName);
		if(i >= 0)
            return createIndex(i, 0, (void*)l);
	}
    else if(l->_module->type() == CSwordModuleInfo::GenericBook)
    {
        CSwordBookModuleInfo *m = qobject_cast<CSwordBookModuleInfo*>(l->_module);
        CSwordTreeKey k(m->tree(), m);
        k.setKey(keyName);

        CSwordTreeKey p(k);
        p.root();

        if(p != k)
            return createIndex(k.getIndex() / 4, 0, (void*)l);
    }
    else
    {
        BtMiniSwordMutex.lock();

        CSwordVerseKey verse(l->_module);
        verse.setKey(keyName);
        const int r = verse.getIndex() - l->_firstEntry;

        BtMiniSwordMutex.unlock();

        if(r >= 0 && r < l->_maxEntries)
            return createIndex(r, 0, (void*)l);
    }

    return QModelIndex();
}

int BtMiniModuleTextModel::keyIndex(QString key) const
{
    Q_D(const BtMiniModuleTextModel);

    Q_ASSERT(d->_singleModule);

    return keyIndex(0, key).row();
}

QString BtMiniModuleTextModel::getModule() const
{
    Q_D(const BtMiniModuleTextModel);
    Q_ASSERT(d->_singleModule);

    if(d->_lists.size() == 1)
        return d->_lists[0]._name;
    return QString();
}

void BtMiniModuleTextModel::setModule(QString module)
{
    Q_D(BtMiniModuleTextModel);
    Q_ASSERT(d->_singleModule);

    if(d->_lists.size() == 1)
        d->setupModule(0, module);
    else
        d->insertModule(0, module);
}


void BtMiniModuleTextModel::openContext(const QModelIndex &index)
{
    Q_D(BtMiniModuleTextModel);

    BtMiniView *v = qobject_cast<BtMiniView *>(sender());
    Q_CHECK_PTR(v);

    QString contents = v->currentContents();
    const QString place = v->currentIndex().data(BtMini::PlaceRole).toString();

    if(!place.isEmpty())
    {
        if(contents[0] == '<')
            contents.insert(contents.indexOf(' ') + 1, "key=\"" + place + "\" ");
        else
            contents = "<verse key=\"" + place + "\">" + contents + "</verse>";
    }

    if(!contents.isEmpty())
    {
        // Construct model from current user settings with given info text
        // Info is string with text and outer opening and closing tag (<tag attr="value">data</tag>)
        QString mc = contents.left(contents.indexOf('>') - 1).mid(contents.indexOf(' ') + 1
            ).replace("\" ", "||").replace("=\"", "=");

        using namespace InfoDisplay;

        Rendering::ListInfoData list = Rendering::detectInfo(mc);
        mc = Rendering::formatInfo(list);

        if(mc.isEmpty())
            return;

        if(!(list.size() == 1 && list[0].first == Rendering::Footnote))
            mc = "<center><small>" + contents.left(contents.indexOf('<', 1)).mid(
                contents.indexOf('>') + 1) + "</small></center>" + mc;

        QStringList modules;
//        foreach(Rendering::InfoData d, list)
//        {
//            if(d.first == Rendering::Key && btConfig().getDefaultSwordModuleByType("standardCommentary") != 0)
//                modules.append("[Commentary]");
//            else if(!modules.contains("[Contents]"))
//                modules.prepend("[Contents]");
//        }

        BtMiniModuleTextModel *m = new BtMiniModuleTextModel(modules);

        for(int i = 0; i < modules.size(); ++i)
        {
            BtMiniModuleTextModelPrivate::List *l = &m->d_func()->_lists[i];

            if(modules[i] == "[Contents]")
            {
                l->setContents(mc);
            }
            if(modules[i] == "[Commentary]")
            {
                QString place;

//                foreach(Rendering::InfoData d, list)
//                    if(d.first == Rendering::Key)
//                        place = d.second;

                if(!place.isEmpty())
                {
                    CSwordVerseKey key(l->_module);
                    l->setScope(key.parseVerseList((const char*)place.toUtf8()));
                }

                if(l->_scopeMap.size() == 0)
                {
                    l->_hasScope = false;
                    l->setContents(QString());
                }
            }
        }

        if(m)
        {
            QWidget *w = BtMiniUi::instance()->activateNewContextWidget();
            BtMiniView *view = new BtMiniView(w);
            view->setWebKitEnabled(btConfig().value<bool>("mini/useWebKit", false));
            view->setTopShadow(true);

            QFont f(view->font());
            f.setPixelSize(f.pixelSize() * btConfig().value<int>("mini/fontTextScale", 140) / 100);
            f.setWeight(QFont::Normal);
            view->setFont(f);

            QPushButton *b = BtMiniUi::instance()->makeButton(QObject::tr("Back"), ":/mini-back.svg", ":/mini-back-night.svg");

            BtMiniPanel *p = new BtMiniPanel;
            p->addWidget(b, Qt::AlignLeft);

            QVBoxLayout *l = new QVBoxLayout;
            l->addWidget(p);
            l->addWidget(view);
            w->setLayout(l);

            view->setModel(m);
            view->scrollTo(m->index(0, 0));

            connect(view, SIGNAL(longPressed(const QModelIndex&)), m, SLOT(openMenu(const QModelIndex&)));
            connect(view, SIGNAL(shortPressed(const QModelIndex&)), m, SLOT(openContext(const QModelIndex&)));
            connect(b, SIGNAL(clicked()), m, SLOT(closeContext()), Qt::QueuedConnection);
        }
    }
}

void BtMiniModuleTextModel::openMenu(const QModelIndex &index)
{
    Q_D(BtMiniModuleTextModel);

    BtMiniView *v = qobject_cast<BtMiniView *>(sender());

    Q_CHECK_PTR(v);

    int level = v->currentLevel();

    QStringList actions;
    actions << tr("Add Left") << tr("Add Right");

    if(d->_lists.size() > 1)
        actions << tr("Clear");

    actions << tr("Select");

    int r = BtMiniMenu::execMenu(actions);

    if(r < 0) return;

    if(actions[r] == tr("Add Left"))
    {
        beginInsertRows(QModelIndex(), level, level);
        d->insertModule(level, "");
        endInsertRows();
    }
    if(actions[r] == tr("Add Right"))
    {
        beginInsertRows(QModelIndex(), level + 1, level + 1);
        d->insertModule(level + 1, "");
        endInsertRows();
    }
    if(actions[r] == tr("Clear"))
    {
        beginRemoveRows(QModelIndex(), level, level);
        d->eraseModule(level);
        endRemoveRows();
    }
    if(actions[r] == tr("Select"))
    {
        v->selectionStart();
    }
}

void BtMiniModuleTextModel::selectedIndexes(const QModelIndex &index)
{
    BtMiniView *v = qobject_cast<BtMiniView *>(sender());
    Q_CHECK_PTR(v);

    switch(BtMiniMenu::execMenu(QStringList() << tr("Copy") << tr("Cancel")))
    {
    case 0:
        QApplication::clipboard()->setText(v->selectedText());
        v->selectionEnd();
        break;
    case 1:
        v->selectionEnd();
        break;
    }
}


void BtMiniModuleTextModel::updateIndicators(const QModelIndex &index)
{
    Q_D(BtMiniModuleTextModel);

    if(d->_placeIndicator)
	{
		QString place(index.data(BtMini::PlaceRole).toString());

        // Shorten text
        if(!place.isEmpty())
        {
            if(d->indexList(index)->_module->type() == CSwordModuleInfo::Bible ||
                d->indexList(index)->_module->type() == CSwordModuleInfo::Commentary)
            {
                CSwordVerseKey key(d->indexToVerseKey(index));
                QMutexLocker locker(&BtMiniSwordMutex);

                place = QString::fromLocal8Bit(key.getShortText());
            }
            else if(d->indexList(index)->_module->type() == CSwordModuleInfo::GenericBook)
            {
                //place = place.replace("/", " ").replace("Book ", "").replace("Section ", "");
                QStringList sl(place.split("/"));
                if(sl.size() > 1 && sl[sl.size() - 1].size() + sl[sl.size() - 2].size() < 20)
                    place = sl[sl.size() - 2] + ", " + sl.last();
                else
                    place = sl.last();
            }
        }

        d->_placeIndicator->setEnabled(!place.isEmpty());
        d->_placeIndicator->setText(place);
	}

    if(d->_moduleIndicator)
    {
        //Q_ASSERT(index.model() == this);
        //QString module(d->indexList(index)->_name);
        QString module(index.data(BtMini::ModuleRole).toString());
        d->_moduleIndicator->setText(module.isEmpty() ? tr("No Module") : module);
    }

    if(d->_leftIndicator)
        d->_leftIndicator->setEnabled(d->indexListId(index) > 0);

    if(d->_rightIndicator)
        d->_rightIndicator->setEnabled(d->indexListId(index) < d->_lists.size() - 1);
}

bool BtMiniModuleTextModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(BtMiniModuleTextModel);

    switch(role)
    {
    case BtMini::ModuleRole:
        {
            QModelIndex i(d->indexDepth(index) == 2 ? index.parent() : index);
            const int l = d->indexListId(i);
            d->setupModule(l, value.toString());
            emit dataChanged(i, i);
        }
        return true;
    case BtMini::PlaceRole:
        {
            QModelIndex i(d->indexDepth(index) == 2 ? index.parent() : index);
            BtMiniModuleTextModelPrivate::List *l = d->indexList(i);

            QString place(value.toString());
            CSwordVerseKey key(l->_module);
            l->setScope(key.parseVerseList((const char*)place.toUtf8()));

            // module have no such entries
            if(l->_scopeMap.size() == 0 && !place.isEmpty())
            {
                l->_hasScope = false;
                l->setContents(QString());
            }

            emit dataChanged(i, i);
        }
        return true;
    case Qt::DisplayRole:
        {
            QModelIndex i(d->indexDepth(index) == 2 ? index.parent() : index);
            BtMiniModuleTextModelPrivate::List *l = d->indexList(i);

            l->setContents(value.toString());

            emit dataChanged(i, i);
        }
        return true;
    default:
        return false;
    }
}

bool BtMiniModuleTextModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_D(BtMiniModuleTextModel);

    Q_ASSERT(!parent.isValid());

    beginInsertRows(parent, row, row + count - 1);
    for(; count > 0; row++, count--)
        d->insertModule(row, "");
    endInsertRows();

	return true;
}

bool BtMiniModuleTextModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_D(BtMiniModuleTextModel);

    Q_ASSERT(!parent.isValid());

    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for(; count > 0; count--)
        d->eraseModule(row);
    endRemoveRows();

	return true;
}

QHash<int, QByteArray> BtMiniModuleTextModel::roleNames() const
{
    QHash<int, QByteArray> rn(QAbstractItemModel::roleNames());
    rn[BtMini::PlaceRole] = "place";
    rn[BtMini::PlaceShortRole] = "placeShort";
    rn[BtMini::PreviewRole] = "preview";
    rn[BtMini::PreviewUpdateRole] = "previewUpdate";
    return rn;
}

void BtMiniModuleTextModel::setSearchText(const QString &text)
{
	Q_D(BtMiniModuleTextModel);

	d->_searchText = text;
}

void BtMiniModuleTextModel::startSearch()
{
	Q_D(BtMiniModuleTextModel);

    BtMiniView *works = BtMiniUi::instance()->worksView();

    QString cm = works->currentIndex().data(BtMini::ModuleRole).toString();
    CSwordModuleInfo *m = CSwordBackend::instance()->findModuleByName(cm.section(',', 0, 0));

    Q_CHECK_PTR(m);

    //qDebug() << "Start search" << cm << d->_searchText;

	sword::ListKey results;
	sword::ListKey scope;
    QString searchText = d->_searchText.replace(tr("strong:", "Strongs search keyword"), "strong:")
            .replace(tr("footnote:", "Footnote search keyword"), "footnote:")
            .replace(tr("heading:", "Heading search keyword"), "heading:")
            .replace(tr("morph:", "Morph search keyword"), "morph:");

    // test on empty keywords
    if (searchText.replace(QRegExp("heading:|footnote:|morph:|strong:"), "").simplified().isEmpty())
        return;
	
	if(CSwordBibleModuleInfo *bm = qobject_cast<CSwordBibleModuleInfo*>(m))
	{
		sword::VerseKey key(bm->lowerBound());
		key.setLowerBound(bm->lowerBound());
		key.setUpperBound(bm->upperBound());
		scope = key;
	}

	if(d->_searchText.isEmpty())
		;
    else if(btConfig().value<bool>("mini/searchUsingSword"))
	{
		QScopedPointer<BtMiniMenu> dialog(BtMiniMenu::createProgress(tr("Search ...")));
		dialog->show();

        m->module().createSearchFramework();

        QPair<BtMiniMenu*, CSwordModuleInfo*> p(dialog.data(), m);
        results = m->module().search(searchText.toUtf8(),
            0, 0, &scope, 0, &BtMiniModuleTextModelPrivate::searchProgress, &p);

        if(dialog->wasCanceled() || m->module().terminateSearch)
			results = sword::ListKey();
	}
	else
	{
        bool ok = true;

		// build index if ascent
		if(!m->hasIndex())
		{
			if(BtMiniMenu::execQuery(tr("Build index for module?"), QStringList() << tr("Yes") << tr("No")) != 0)
				ok = false;
			else
			{
				QScopedPointer<BtMiniMenu> dialog(BtMiniMenu::createProgress(tr("Indexing...")));

				QObject::connect(dialog.data(), SIGNAL(canceled()), m, SLOT(cancelIndexing()));
				QObject::connect(m, SIGNAL(indexingProgress(int)), dialog.data(), SLOT(setValue(int)));

                dialog->show();

                try {
                    m->buildIndex();
                } catch (...) {
                    BtMiniMenu::execQuery(tr("Indexing aborted") + "\n" +
                                         tr("An internal error occurred while building "
                                            "the index."));
                    ok = false;
                }

                if(!ok || dialog->wasCanceled())
				{
					if (m->hasIndex())
						m->deleteIndex();
					ok = false;
				}
			}
		}

		if(ok)
		{
            CSwordModuleSearch searcher;

            searcher.setSearchedText(searchText);
			searcher.setModules(QList<const CSwordModuleInfo*>() << m);
			searcher.setSearchScope(scope);

			searcher.startSearch();

            results = searcher.results()[m];
		}
	}

	Q_ASSERT(d->_lists.size() == 1 && d->_lists[0]._name == "[Search]");

    if(results.getCount() > 0)
        d->_lists[0]._module = m;
    else
        d->_lists[0]._module = 0;

	d->_lists[0].setScope(results);

	QModelIndex i = index(0, 0);
	emit dataChanged(i, i);
}

void BtMiniModuleTextModel::openModuleMenu(const QModelIndex &index)
{
    CSwordModuleInfo *m = static_cast<CSwordModuleInfo*>(
        index.data(BtBookshelfModel::ModulePointerRole).value<void*>());

    if(!m) return;

    QModelIndex wi = BtMiniUi::instance()->worksView()->currentIndex();
    QString wm(wi.data(BtMini::ModuleRole).toString());
    CSwordModuleInfo *md = CSwordBackend::instance()->findModuleByName(wm.section(',', 0, 0));

    QWidget *w = BtMiniUi::instance()->activateNewContextWidget();
    BtMiniView *view = new BtMiniView(w);
    view->setTopShadow(true);

    BtMiniPanel *p = new BtMiniPanel(w);
    QPushButton *b = BtMiniUi::instance()->makeButton(tr("Back"), ":/mini-back.svg", ":/mini-back-night.svg");
    QLabel *c = new QLabel(m->name());
    c->setAlignment(Qt::AlignCenter);
    p->addWidget(b, Qt::AlignLeft);
    p->addWidget(c, Qt::AlignCenter);

    /// \todo add / remove from context
    /// \todo remove module
    QStringList items;
    items << "<h2><center>" + tr("Set default ") + "<br/>" + m->categoryName(m->category()) + "</center></h2>";
    if(!wm.isEmpty()) items << "<h2><center>" + tr("Add Parallel") + "</center></h2>";
    items << "<hr>" + m->aboutText();
    QStringListModel* model = new QStringListModel(items, view);
    model->setObjectName(m->name());
    view->setModel(model);

    QVBoxLayout *l = new QVBoxLayout;
    l->addWidget(p);
    l->addWidget(view);
    w->setLayout(l);

    connect(b, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(goBack()));
    connect(view, SIGNAL(clicked(const QModelIndex &)), this, SLOT(moduleContextClicked(const QModelIndex &)));
}

void BtMiniModuleTextModel::moduleContextClicked(const QModelIndex &index)
{
    CSwordModuleInfo *m = CSwordBackend::instance()->findModuleByName(index.model()->objectName());

    if(index.row() == 0)
    {
        switch(m->category())
        {
        case CSwordModuleInfo::Bibles:
            btConfig().setDefaultSwordModuleByType("standardBible", m);
            break;
        case CSwordModuleInfo::Commentaries:
            btConfig().setDefaultSwordModuleByType("standardCommentary", m);
            break;
        case CSwordModuleInfo::Lexicons:
            btConfig().setDefaultSwordModuleByType("standardLexicon", m);
            if(m->has(CSwordModuleInfo::HebrewDef))
                btConfig().setDefaultSwordModuleByType("standardHebrewStrongsLexicon", m);
            if(m->has(CSwordModuleInfo::GreekDef))
                btConfig().setDefaultSwordModuleByType("standardGreekStrongsLexicon", m);
            if(m->has(CSwordModuleInfo::HebrewParse))
               btConfig().setDefaultSwordModuleByType("standardHebrewMorphLexicon", m);
            if(m->has(CSwordModuleInfo::GreekParse))
                btConfig().setDefaultSwordModuleByType("standardGreekMorphLexicon", m);
            break;
        case CSwordModuleInfo::DailyDevotional:
            btConfig().setDefaultSwordModuleByType("standardDailyDevotional", m);
            break;
        }
        BtMiniUi::instance()->activateWorks();
    }
    else if(index.model()->rowCount() < 3)
        ;
    else if(index.row() == 1)
    {
        QModelIndex wi = BtMiniUi::instance()->worksView()->currentIndex();
        QString wm(wi.data(BtMini::ModuleRole).toString());
        setData(wi, wm + ',' + m->name(), BtMini::ModuleRole);
        BtMiniUi::instance()->activateWorks();
    }
}

void BtMiniModuleTextModel::modulesReloaded()
{
	Q_D(BtMiniModuleTextModel);

	for(int i = 0; i < d->_lists.size(); ++i)
        d->_lists[i].setModule(d->_lists[i]._name);
}

void BtMiniModuleTextModel::closeContext()
{
    BtMiniView *view = BtMiniUi::instance()->currentContextWidget()->findChild<BtMiniView*>();
    Q_CHECK_PTR(view);
    BtMiniModuleTextModel *m = reinterpret_cast<BtMiniModuleTextModel*>(view->model());
    Q_CHECK_PTR(m);

    // Sync context modules to config
    QModelIndexList list = view->currentIndexes();
    QStringList modules;

    for(int i = 0; i < list.size(); ++i)
        modules.append(m->d_func()->_lists[i]._name);

    btConfig().setValue<int>("mini/openInfoModule", view->currentLevel());
    btConfig().setValue<QStringList>("mini/openInfoModules", modules);

    BtMiniUi::instance()->goBack();
}

void BtMiniModuleTextModel::closeModuleSelection()
{
    BtMiniView *works = BtMiniUi::instance()->worksView();
    works->setSleep(false);

    BtMiniView *view = reinterpret_cast<BtMiniView*>(sender());
    Q_CHECK_PTR(view);

    QString cm = works->currentIndex().data(BtMini::ModuleRole).toString();
    QString nm = view->currentIndex().data(BtBookshelfModel::ModuleNameRole).toString();

    if(cm != nm)
    {
        CSwordVerseKey place(CSwordBackend::instance()->findModuleByName(cm));
        if(cm.size() > 0)
            place.setKey(works->currentIndex().data(BtMini::PlaceRole).toString());

        // Change view current module
        if(!works->model()->setData(works->currentIndex(), nm, BtMini::ModuleRole))
            qDebug() << "BtMiniModuleTextModel::openModuleSelection: failed to change module";
        else
        {
            QModelIndex index(createIndex(0, 0, (void*)&d_func()->_lists[works->currentLevel()]));
            CSwordModuleInfo *mi = CSwordBackend::instance()->findModuleByName(nm);
            CSwordModuleInfo *ci = CSwordBackend::instance()->findModuleByName(cm);

            // Restore module place
            if(ci && mi && (mi->type() == CSwordModuleInfo::Bible || mi->type() == CSwordModuleInfo::Commentary)
                && (ci->type() == CSwordModuleInfo::Bible || ci->type() == CSwordModuleInfo::Commentary))
            {
                place.setModule(mi);
                index = keyIndex(works->currentLevel(), place.key());
            }

            if(index.isValid())
            {
                for(int i = 0; i < 5 && index.data(BtMini::PlaceRole).toString().size() == 0; ++i)
                    index = index.model()->index(index.row() + 1, 0, index.parent());
                works->scrollTo(index);
            }
        }
    }

    BtMiniUi::instance()->goBack();
}

void BtMiniModuleTextModel::closePlaceSelection()
{
    BtMiniView *works = BtMiniUi::instance()->worksView();
    works->setSleep(false);

    BtMiniView *view = reinterpret_cast<BtMiniView*>(sender());
    Q_CHECK_PTR(view);

    QString np = view->currentIndex().data(BtMini::PlaceRole).toString();
    works->scrollTo(keyIndex(works->currentLevel(), np));

    BtMiniUi::instance()->goBack();
}

QModelIndexList BtMiniModuleTextModel::match(const QModelIndex &start, int role, const QVariant &value, int hits,
                      Qt::MatchFlags flags) const
{
    if(role == BtMini::PlaceRole)
        return QModelIndexList() << keyIndex(d_ptr->indexListId(start), value.toString());

    return QAbstractItemModel::match(start, role, value, hits, flags);
}

