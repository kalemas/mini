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


#include "btminiworkswidget.h"

#include <QApplication>
#include <QClipboard>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QStringListModel>
#include <QtDebug>

#include "../../bibletime/src/backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../../bibletime/src/backend/config/btconfig.h"
#include "../../bibletime/src/backend/drivers/cswordbiblemoduleinfo.h"
#include "../../bibletime/src/backend/keys/cswordversekey.h"
#include "../../bibletime/src/backend/managers/cswordbackend.h"
#include "../../bibletime/src/frontend/cinfodisplay.h"

#include "btmini.h"
#include "models/btminimodulenavigationmodel.h"
#include "models/btminimoduletextmodel.h"
#include "models/btminimodulesmodel.h"
#include "view/btminilayoutdelegate.h"
#include "view/btminiview.h"
#include "ui/btminiclippingswidget.h"
#include "ui/btminimenu.h"
#include "ui/btminipanel.h"
#include "ui/btminiui.h"


class BtMiniWorksWidgetPrivate
{
public:
    BtMiniWorksWidgetPrivate()
    {
        ;
    }

    ~BtMiniWorksWidgetPrivate()
    {
        ;
    }

public:
    BtMiniView            *_view;
    QPushButton           *_buttonSwitchLeft;
    QPushButton           *_buttonSwitchRight;
    QPushButton           *_buttonModule;
    QPushButton           *_buttonPlace;
    QPushButton           *_buttonPlusLeft;
    QPushButton           *_buttonPlusRight;
    BtMiniModuleTextModel *_worksModel;

};


BtMiniWorksWidget::BtMiniWorksWidget(QWidget *parent)
    : BtMiniWidget(parent)
    , d_ptr(new BtMiniWorksWidgetPrivate())
{
    Q_D(BtMiniWorksWidget);

    d->_view = new BtMiniView(this);
    d->_view->setTopShadow(true);
    d->_view->setContinuousScrolling(btConfig().value<bool>("mini/miniContinuousScrolling", false));
    d->_view->setWebKitEnabled(btConfig().value<bool>("mini/useWebKit", false));
    changeFontSize(d->_view, btConfig().value<int>("mini/fontTextScale", 140) / 100.0);

    QIcon plusIcon(":/plus.svg");

    // Setup controls
    d->_buttonSwitchLeft  = BtMiniUi::makeButton("", d->_view->style()->standardIcon(QStyle::SP_ArrowLeft));
    d->_buttonSwitchRight = BtMiniUi::makeButton("", d->_view->style()->standardIcon(QStyle::SP_ArrowRight));
    d->_buttonPlusLeft    = BtMiniUi::makeButton("", plusIcon);
    d->_buttonPlusRight   = BtMiniUi::makeButton("", plusIcon);
    d->_buttonModule      = BtMiniUi::makeButton(BtMiniUi::tr("Work"));
    d->_buttonPlace       = BtMiniUi::makeButton(BtMiniUi::tr("Place"));

    QObject::connect(d->_buttonSwitchLeft, SIGNAL(clicked()), d->_view, SLOT(slideLeft()));
    QObject::connect(d->_buttonSwitchRight, SIGNAL(clicked()), d->_view, SLOT(slideRight()));
    QObject::connect(d->_buttonPlusLeft, SIGNAL(clicked()), this, SLOT(addWorkLeft()));
    QObject::connect(d->_buttonPlusRight, SIGNAL(clicked()), this, SLOT(addWorkRight()));
    QObject::connect(d->_buttonModule, SIGNAL(clicked()), this, SLOT(openModuleSelection()));
    QObject::connect(d->_buttonPlace, SIGNAL(clicked()), this, SLOT(openPlaceSelection()));

    BtMiniPanel *p = new BtMiniPanel(BtMiniPanel::Activities() << BtMiniPanel::Search
        << BtMiniPanel::Options
        //<< BtMiniPanel::Installer
        << BtMiniPanel::Settings << BtMiniPanel::Exit, this);
    p->layout()->setContentsMargins(0, 0, 0, 0);
    QFont f (p->font());
    f.setBold(true);
    p->setFont(f);

    // Put into layout
    QVBoxLayout *vl = new QVBoxLayout;

    BtMiniPanel *pn = new BtMiniPanel;
    pn->addWidget(d->_buttonSwitchLeft, Qt::AlignLeft);
    pn->addWidget(d->_buttonPlusLeft, Qt::AlignLeft);
    pn->addWidget(d->_buttonModule, Qt::AlignCenter);
    pn->addWidget(d->_buttonPlace, Qt::AlignCenter);
    pn->addWidget(d->_buttonSwitchRight, Qt::AlignRight);
    pn->addWidget(d->_buttonPlusRight, Qt::AlignRight);

    vl->addWidget(pn);
    vl->addWidget(d->_view);
    vl->addWidget(p);

    setLayout(vl);

    // Retrieve last session
    QStringList modules = btConfig().value<QStringList>("mini/openModules", QStringList() << "");
    QStringList places = btConfig().value<QStringList>("mini/openPlaces", QStringList() << "");
    int openModule = btConfig().value<int>("mini/openModule", 0);

    Q_ASSERT(modules.size() == places.size() && openModule >= 0 && openModule < modules.size());

    QStringList moduleNames;

    foreach(CSwordModuleInfo *m, CSwordBackend::instance()->moduleList())
        moduleNames << m->name();

    for(int i = 0; i < modules.size(); ++i)
    {
        // parallel module
        if(modules[i].contains(','))
        {
            QStringList sl(modules[i].split(','));
            QString rs;
            foreach(QString s, sl)
            {
                if(!moduleNames.contains(s))
                {
                    if(!s.isEmpty())
                        qDebug() << "Remove reference to inexistent module in parallel module" << s;
                }
                else
                    rs.append(rs.isEmpty() ? s : ',' + s);
            }

            modules[i] = rs;

            if(!rs.isEmpty())
                continue;
        }

        if(!moduleNames.contains(modules[i]))
        {
            if(!modules[i].isEmpty())
                qDebug() << "Remove reference to inexistent module" << modules[i];

            modules.erase(modules.begin() + i);
            places.erase(places.begin() + i);

            if(i <= openModule && openModule > 0)
                openModule--;

            --i;
        }
    }

    // create initial session
    if(modules.size() == 0)
    {
        CSwordBibleModuleInfo *m = qobject_cast<CSwordBibleModuleInfo *>(btConfig().getDefaultSwordModuleByType("standardBible"));
        modules.append(m->name());
        if(m->hasNewTestament())
        {
            if(m->hasOldTestament())
            {
                modules.append(m->name());
                places.append("Genesis 1:1");
                openModule = 1;
            }
            places.append("John 1:1");
        }
        else
            places.append("Genesis 1:1");
    }

    Q_ASSERT(modules.size() > 0);

    // Setup model
    d->_worksModel = new BtMiniModuleTextModel(modules, d->_view);

    d->_view->setModel(d->_worksModel);

    connect(d->_view, SIGNAL(currentChanged(const QModelIndex &)), this, SLOT(currentIndexChanged(const QModelIndex &)));
    connect(d->_view, SIGNAL(shortPressed(const QModelIndex &)), this, SLOT(openContext(const QModelIndex &)));
    connect(d->_view, SIGNAL(longPressed(const QModelIndex &)), this, SLOT(openMenu(const QModelIndex &)));
    connect(d->_view, SIGNAL(selected(const QModelIndex &)), this, SLOT(selectedIndexes(const QModelIndex &)));

    // Restore last session
    for(int i = 0; i < modules.size(); ++i)
    {
        QModelIndex index(d->_worksModel->keyIndex(i, places[i]));
        if(i == openModule)
        {
            d->_view->scrollTo(index);
        }
        else if(index.isValid())
        {
            d->_view->setCurrentIndex(index);
        }
    }

    connect(CSwordBackend::instance(), SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
        d->_worksModel, SLOT(modulesReloaded()));
}

BtMiniWorksWidget::~BtMiniWorksWidget()
{
    delete d_ptr;
}


void BtMiniWorksWidget::openModuleSelection()
{
    BtMiniView *works = BtMiniUi::instance()->worksView();
    works->setSleep(true);
    QString cm = works->currentIndex().data(BtMini::ModuleRole).toString();

    QWidget *w = BtMiniUi::instance()->activateNewContextWidget();
    BtMiniView *view = new BtMiniView(w);
    view->setInteractive(true);
    view->setTopShadow(true);

    QPushButton *b = BtMiniUi::instance()->makeButton(tr("Back"), ":/mini-back.svg", ":/mini-back-night.svg");
    QPushButton *i = BtMiniUi::instance()->makeButton(tr("Install"));
    QLabel *lb = new QLabel(tr("Select Text:"));
    lb->setAlignment(Qt::AlignCenter);

    BtMiniPanel *p = new BtMiniPanel;
    p->addWidget(b, Qt::AlignLeft);
    p->addWidget(lb, Qt::AlignCenter);
    p->addWidget(i, Qt::AlignRight);

    QVBoxLayout *l = new QVBoxLayout;
    l->addWidget(p);
    l->addWidget(view);
    w->setLayout(l);

    BtBookshelfTreeModel * m = new BtBookshelfTreeModel(BtBookshelfTreeModel::Grouping(true), view);
    m->setSourceModel(CSwordBackend::instance()->model());

    // if modules more than 4, scrollbar always visible
    if(m->modules().size() > 4)
    {
        BtMiniLevelOption o = view->layoutDelegate()->levelOption();
        o.scrollBarPolicy = Qt::ScrollBarAlwaysOn;
        view->layoutDelegate()->setLevelOption(o);
    }

    QAbstractItemModel *pm = BtMiniModulesModel::wrapWithProxy(m);
    pm->setParent(view);

    view->setModel(pm);

    QModelIndexList list = m->match(m->index(0, 0), BtBookshelfModel::ModuleNameRole,
        cm, 1, Qt::MatchExactly | Qt::MatchRecursive);

    if(list.size() > 0 && list[0].isValid())
        view->scrollTo(list[0]);

    connect(view, SIGNAL(shortPressed(const QModelIndex&)), this, SLOT(openModuleMenu(const QModelIndex&)));
    connect(view, SIGNAL(selected(const QModelIndex &)), this, SLOT(closeModuleSelection()));
    connect(view, SIGNAL(selected(const QModelIndex &)), BtMiniUi::instance(), SLOT(activateWorks()));
    connect(b, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(goBack()));
    //connect(i, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(goBack()));
    connect(i, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(activateInstaller()));
}

void BtMiniWorksWidget::openPlaceSelection()
{
    BtMiniView *works = BtMiniUi::instance()->worksView();
    works->setSleep(true);

    QString cm = works->currentIndex().data(BtMini::ModuleRole).toString().section(',', 0, 0);
    QString cp = works->currentIndex().data(BtMini::PlaceRole).toString();

    QWidget *w = BtMiniUi::instance()->activateNewContextWidget();
    BtMiniView *view = new BtMiniView(w);
    view->setInteractive(true);
    view->setTopShadow(true);
    BtMiniUi::changeFontSize(view, 1.2);

    QPushButton *b = BtMiniUi::instance()->makeButton(tr("Back"), ":/mini-back.svg", ":/mini-back-night.svg");

    BtMiniPanel *p = new BtMiniPanel(w);
    p->addWidget(b, Qt::AlignLeft);

    BtMiniModuleNavigationModel * m = new BtMiniModuleNavigationModel(cm, view);
    view->setModel(m);

    CSwordModuleInfo *mi = CSwordBackend::instance()->findModuleByName(cm);

    if(mi && mi->type() != CSwordModuleInfo::Lexicon)
    {
        QLabel *c = new QLabel;
        c->setAlignment(Qt::AlignCenter);
        m->setIndicator(c);
        connect(view, SIGNAL(currentChanged(QModelIndex)), m, SLOT(updateIndicator(QModelIndex)));
        p->addWidget(c, Qt::AlignCenter);
    }

    QVBoxLayout *l = new QVBoxLayout;
    l->addWidget(p);
    l->addWidget(view);
    w->setLayout(l);


    // setup current place and scroll to proper place
    QModelIndex pi = m->keyToIndex(cp);
    view->setCurrentIndex(pi);
    while(pi.parent() != QModelIndex())
        pi = pi.parent();
    view->scrollTo(pi);

    connect(b, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(goBack()));
    connect(view, SIGNAL(selected(const QModelIndex &)), this, SLOT(closePlaceSelection()));
    connect(view, SIGNAL(selected(const QModelIndex &)), BtMiniUi::instance(), SLOT(activateWorks()));
}

void BtMiniWorksWidget::currentIndexChanged(const QModelIndex &index)
{
    Q_D(BtMiniWorksWidget);

    QString module(index.data(BtMini::ModuleRole).toString());

    QString place(index.data(BtMini::PlaceRole).toString());
    // shorten the text
    if(!place.isEmpty())
    {
        CSwordModuleInfo *info = CSwordBackend::instance()->findModuleByName(module.section(',', 0, 0));

        if(info->type() == CSwordModuleInfo::Bible || info->type() == CSwordModuleInfo::Commentary)
        {
            CSwordVerseKey key(info);
            key.setKey(place); // here would be problem with multithreaded environment
            place = QString::fromLocal8Bit(key.getShortText());
        }
        else if(info->type() == CSwordModuleInfo::GenericBook)
        {
            //place = place.replace("/", " ").replace("Book ", "").replace("Section ", "");
            QStringList sl(place.split("/"));
            if(sl.size() > 1 && sl[sl.size() - 1].size() + sl[sl.size() - 2].size() < 20)
                place = sl[sl.size() - 2] + ", " + sl.last();
            else
                place = sl.last();
        }
    }
    d->_buttonPlace->setEnabled(!place.isEmpty());
    d->_buttonPlace->setText(place);

    d->_buttonModule->setText(module.isEmpty() ? tr("No Module") : module);

    // parent index represents current module in the list
    int i = index.parent().row(), s = index.model()->rowCount();
    d->_buttonSwitchLeft->setVisible(i > 0);
    d->_buttonSwitchRight->setVisible(i < s - 1);
    d->_buttonPlusLeft->setVisible(i <= 0);
    d->_buttonPlusRight->setVisible(i >= s - 1);
}

void BtMiniWorksWidget::openContext(const QModelIndex &index)
{
    Q_D(BtMiniWorksWidget);

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

        Q_ASSERT(modules.size() == m->rowCount());

        for(int i = 0; i < m->rowCount(); ++i)
        {
            if(modules[i] == "[Contents]")
            {
                m->setData(m->index(i, 0), mc, Qt::DisplayRole);
            }
            if(modules[i] == "[Commentary]")
            {
                QString place;

//                foreach(Rendering::InfoData d, list)
//                    if(d.first == Rendering::Key)
//                        place = d.second;

                m->setData(m->index(i, 0), place, BtMini::PlaceRole);
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

void BtMiniWorksWidget::openMenu(const QModelIndex &index)
{
    Q_D(BtMiniWorksWidget);

    BtMiniView *v = qobject_cast<BtMiniView *>(sender());

    Q_CHECK_PTR(v);

    int level = v->currentLevel();

    QString al(tr("Add Left"));
    QString ar(tr("Add Right"));
    QString cl(tr("Clear"));
    QString sl(tr("Select"));

    QStringList actions(QStringList() << al << ar);

    if(d->_worksModel->rowCount() > 1)
        actions << cl;

    actions << sl;

    int r = BtMiniMenu::execMenu(actions);

    if(r < 0) return;

    if(actions[r] == al)
    {
        d->_worksModel->insertRow(level);
        QApplication::processEvents();
        v->slideLeft();
    }
    else if(actions[r] == ar)
    {
        d->_worksModel->insertRow(level + 1);
        v->slideRight();
    }
    else if(actions[r] == cl)
    {
        d->_worksModel->removeRow(level);
    }
    else if(actions[r] == sl)
    {
        v->selectionStart();
    }
    else
        Q_ASSERT(false);
}

void BtMiniWorksWidget::selectedIndexes(const QModelIndex &index)
{
    BtMiniView *v = qobject_cast<BtMiniView *>(sender());
    Q_CHECK_PTR(v);

    switch(BtMiniMenu::execMenu(QStringList() << tr("Copy") << tr("Bookmark") << tr("Cancel")))
    {
    case 0:
        QApplication::clipboard()->setText(v->selectedText());
        break;
    case 1:
        {
            QModelIndexList l(v->selectedIndexes());
            CSwordModuleInfo *m(CSwordBackend::instance()->findModuleByName(
                                l[0].data(BtMini::ModuleRole).toString().section(',', 0, 0)));

            QString k(l[0].data(BtMini::PlaceRole).toString());
            if(l.size() > 1 && l[0] != l[1])
                k.append("-"), k.append(l[1].data(BtMini::PlaceRole).toString());

            BtMiniClippingsWidget::insertBookmark(m, k);
        }
    case 2:
        break;
    }
    v->selectionEnd();
}

void BtMiniWorksWidget::openModuleMenu(const QModelIndex &index)
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

void BtMiniWorksWidget::moduleContextClicked(const QModelIndex &index)
{
    Q_D(BtMiniWorksWidget);

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
        d->_worksModel->setData(wi, wm + ',' + m->name(), BtMini::ModuleRole);
        BtMiniUi::instance()->activateWorks();
    }
}

void BtMiniWorksWidget::closeContext()
{
    BtMiniView *view = BtMiniUi::instance()->currentContextWidget()->findChild<BtMiniView*>();
    Q_CHECK_PTR(view);
    BtMiniModuleTextModel *m = reinterpret_cast<BtMiniModuleTextModel*>(view->model());
    Q_CHECK_PTR(m);

    // Sync context modules to config
    QModelIndexList list = view->currentIndexes();
    QStringList modules;

    for(int i = 0; i < list.size(); ++i)
        modules.append(list[i].data(BtMini::ModuleRole).toString());

    btConfig().setValue<int>("mini/openInfoModule", view->currentLevel());
    btConfig().setValue<QStringList>("mini/openInfoModules", modules);

    BtMiniUi::instance()->goBack();
}

void BtMiniWorksWidget::closeModuleSelection()
{
    Q_D(BtMiniWorksWidget);

    BtMiniView *works = BtMiniUi::instance()->worksView();
    works->setSleep(false);

    BtMiniView *view = reinterpret_cast<BtMiniView*>(sender());
    Q_CHECK_PTR(view);

    QString cm = works->currentIndex().data(BtMini::ModuleRole).toString();
    QString nm = view->currentIndex().data(BtBookshelfModel::ModuleNameRole).toString();

    if(cm != nm)
    {
        //int id = works->currentIndex().parent().row();
        int id = works->currentLevel();

        CSwordVerseKey place(CSwordBackend::instance()->findModuleByName(cm));
        if(cm.size() > 0)
            place.setKey(works->currentIndex().data(BtMini::PlaceRole).toString());

        // Change view current module
        if(!works->model()->setData(works->currentIndex(), nm, BtMini::ModuleRole))
            qDebug() << "BtMiniModuleTextModel::openModuleSelection: failed to change module";
        else
        {
            QModelIndex index(works->model()->index(id, 0).child(0, 0));
            CSwordModuleInfo *mi = CSwordBackend::instance()->findModuleByName(nm);
            CSwordModuleInfo *ci = CSwordBackend::instance()->findModuleByName(cm);

            // Restore module place
            if(ci && mi && (mi->type() == CSwordModuleInfo::Bible || mi->type() == CSwordModuleInfo::Commentary)
                && (ci->type() == CSwordModuleInfo::Bible || ci->type() == CSwordModuleInfo::Commentary))
            {
                place.setModule(mi);
                index = d->_worksModel->keyIndex(id, place.key());
            }

            if(index.isValid())
            {
                for(int i = 0; i < 5 && index.data(BtMini::PlaceRole).toString().size() == 0; ++i)
                    index = index.model()->index(index.row() + 1, 0, index.parent());
                works->scrollTo(index);
            }
        }
    }

    //BtMiniUi::instance()->goBack();
}

void BtMiniWorksWidget::closePlaceSelection()
{
    Q_D(BtMiniWorksWidget);

    d->_view->setSleep(false);

    BtMiniView *view = reinterpret_cast<BtMiniView*>(sender());
    Q_CHECK_PTR(view);

    QString np = view->currentIndex().data(BtMini::PlaceRole).toString();
    d->_view->scrollTo(d->_worksModel->keyIndex(d->_view->currentLevel(), np));
}

void BtMiniWorksWidget::addWorkLeft()
{
    Q_D(BtMiniWorksWidget);

    d->_worksModel->insertRow(d->_view->currentLevel());
    QApplication::processEvents();
    d->_view->slideLeft();
}

void BtMiniWorksWidget::addWorkRight()
{
    Q_D(BtMiniWorksWidget);

    d->_worksModel->insertRow(d->_view->currentLevel() + 1);
    d->_view->slideRight();
}
