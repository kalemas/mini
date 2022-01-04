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

#include "btminiclippingswidget.h"

#include <QApplication>
#include <QIdentityProxyModel>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>

#include "../../bibletime/src/backend/btbookmarksmodel.h"
#include "../../bibletime/src/backend/config/btconfig.h"
#include "../../bibletime/src/backend/managers/cswordbackend.h"
#include "../../bibletime/src/backend/rendering/ctextrendering.h"
#include "../../bibletime/src/util/bticons.h"

#include "ui/btminimenu.h"
#include "ui/btminipanel.h"
#include "ui/btminiui.h"
#include "view/btminilayoutdelegate.h"
#include "view/btminiview.h"


class BookmarksProxyModel : public QIdentityProxyModel
{
public:
    BookmarksProxyModel(QObject * parent = 0);
    ~BookmarksProxyModel() {;}

    QVariant data(const QModelIndex & index, int role) const;


    DisplayOptions _displayOptions;
    FilterOptions  _filterOptions;
};

class BtMiniClippingsWidgetPrivate
{
public:
    BtMiniClippingsWidgetPrivate()
    {
        ;
    }

    ~BtMiniClippingsWidgetPrivate()
    {
        ;
    }

    static BtBookmarksModel & model()
    {
        /// \note We want model to be destroyed before Sword backend to have all its
        /// facilities working.
        static BtBookmarksModel * model = new BtBookmarksModel(CSwordBackend::instance());
        return *model;
    }

    static QAbstractItemModel & proxyModel()
    {
        static BookmarksProxyModel model;
        return model;
    }


public:
    BtMiniView              *_view;
    QLineEdit               *_captionEdit;
    QLabel                  *_caption;
};


BookmarksProxyModel::BookmarksProxyModel(QObject * parent)
    : QIdentityProxyModel(parent)
{
    _displayOptions = btConfig().getDisplayOptions();
    _filterOptions = btConfig().getFilterOptions();

    setSourceModel(&BtMiniClippingsWidgetPrivate::model());
}

QVariant BookmarksProxyModel::data(const QModelIndex & index, int role) const
{
    switch(role)
    {
    case Qt::DisplayRole:
        {
            QList<CSwordModuleInfo const *> modules;
            modules << BtMiniClippingsWidgetPrivate::model().module(mapToSource(index));
            QString key(BtMiniClippingsWidgetPrivate::model().key(mapToSource(index)));
            QString t = Rendering::CEntryDisplay().textKeyRendering(
                modules,
                key,
                _displayOptions,
                _filterOptions,
                Rendering::CTextRendering::KeyTreeItem::Settings::ExpandedLong
            );
            return t;
        }
    case Qt::DecorationRole:
        return QVariant();
    default:
        return QIdentityProxyModel::data(index, role);
    }
}

BtMiniClippingsWidget::BtMiniClippingsWidget(QWidget *parent)
    : BtMiniWidget(parent)
    , d_ptr(new BtMiniClippingsWidgetPrivate())
{
    Q_D(BtMiniClippingsWidget);

    d->_view = new BtMiniView(this);
    d->_view->setTopShadow(true);
    changeFontSize(d->_view, btConfig().value<int>("mini/fontTextScale", 140) / 100.0);


    BtMiniLayoutDelegate *ld = new BtMiniLayoutDelegate(d->_view);
    ld->setPlainMode(true);

    d->_view->setLayoutDelegate(ld);
    d->_view->setModel(&d->proxyModel());

    QPushButton *bb = BtMiniUi::makeButton(BtMiniUi::tr("Back"), ":/mini-back.svg", ":/mini-back-night.svg");
    d->_captionEdit = new QLineEdit;
    d->_caption     = new QLabel;
    QPushButton *rn = BtMiniUi::makeButton("", BtIcons::instance().icon_pencil);

    d->_captionEdit->setAlignment(Qt::AlignCenter);
    d->_captionEdit->setVisible(false);
    d->_caption->setAlignment(Qt::AlignCenter);

    BtMiniPanel *p = new BtMiniPanel;
    p->addWidget(bb, Qt::AlignLeft);
    p->addWidget(d->_captionEdit, Qt::AlignCenter);
    p->addWidget(d->_caption, Qt::AlignCenter);
    p->addWidget(rn, Qt::AlignRight);

    QVBoxLayout *vl = new QVBoxLayout;
    vl->addWidget(p);
    vl->addWidget(d->_view);

    setLayout(vl);

    connect(bb, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(goBack()));
    connect(d->_view, SIGNAL(currentChanged(QModelIndex)), this, SLOT(currentIndexChanged(QModelIndex)));
    connect(d->_view, SIGNAL(longPressed(QModelIndex)), this, SLOT(openMenu(QModelIndex)));
    connect(rn, SIGNAL(clicked()), d->_caption, SLOT(hide()));
    connect(rn, SIGNAL(clicked()), d->_captionEdit, SLOT(show()));
    connect(d->_captionEdit, SIGNAL(editingFinished()), this, SLOT(renameCurrentList()));

    //currentIndexChanged(QModelIndex());

    QModelIndexList il(d->model().match(d->model().index(0,0),
        Qt::DisplayRole, btConfig().value<QString>("mini/defaultBookmarkFolder")));

    QModelIndex index(il.size() > 0 ? il[0] : d->model().index(0, 0));

    d->_view->scrollTo(index);
    d->_caption->setText(index.data().toString());
}


BtMiniClippingsWidget::~BtMiniClippingsWidget()
{
    delete d_ptr;
}

void BtMiniClippingsWidget::insertBookmark(const CSwordModuleInfo *module, QString key, int where, int list)
{
    // get folder to insert
    if(list == -1)
    {
        QString f(btConfig().value<QString>("mini/defaultBookmarkFolder"));
        if(lists().size() == 0)
            BtMiniClippingsWidgetPrivate::model().addFolder(0, QModelIndex());
        list = qMax(lists().indexOf(f), 0);
    }

    BtMiniClippingsWidgetPrivate::model().addBookmark(where,
        BtMiniClippingsWidgetPrivate::model().index(list, 0), *module, key);
}

QStringList BtMiniClippingsWidget::lists()
{
    QStringList l;
    for(int i = 0; i < BtMiniClippingsWidgetPrivate::model().rowCount(); i++)
        l.append(BtMiniClippingsWidgetPrivate::model().index(i, 0).data().toString());
    return l;
}

void BtMiniClippingsWidget::currentIndexChanged(const QModelIndex &index)
{
    Q_D(BtMiniClippingsWidget);

    const QModelIndex i(d->model().index(d->_view->currentLevel(), 0));
    const QString t(i.data().toString());

    d->_caption->setText(t);
    d->_captionEdit->setText(t);
    d->_caption->setVisible(true);
    d->_captionEdit->setVisible(false);

    btConfig().setValue<QString>("mini/defaultBookmarkFolder", t);
}

void BtMiniClippingsWidget::renameCurrentList()
{
    Q_D(BtMiniClippingsWidget);

    QString name(d->_captionEdit->text());

    QModelIndex i;
    if(d->model().rowCount() == 0)
        i = d->model().addFolder(0, QModelIndex());
    else
        i = d->model().index(d->_view->currentLevel(), 0);

    d->model().setData(i, name);

    d->_caption->setText(name);
    d->_caption->setVisible(true);
    d->_captionEdit->setVisible(false);
}

void BtMiniClippingsWidget::openMenu(const QModelIndex & index)
{
    Q_D(BtMiniClippingsWidget);

    int level = d->_view->currentLevel();

    QString al(tr("Add Left"));
    QString ar(tr("Add Right"));
    QString cl(tr("Clear"));
    QString ri(tr("Remove Item"));

    QStringList actions;
    actions << al << ar;
    if(d->model().rowCount() > 1)
        actions << cl;
    if(index.isValid())
        actions << ri;

    int r = BtMiniMenu::execMenu(actions);

    if(r < 0) return;

    if(actions[r] == al)
    {
        d->model().addFolder(level, QModelIndex());
        QApplication::processEvents();
        d->_view->slideLeft();
    }
    else if(actions[r] == ar)
    {
        d->model().addFolder(level + 1, QModelIndex());
        d->_view->slideRight();
    }
    else if(actions[r] == cl)
    {
        d->model().removeRow(level);
    }
    else if(actions[r] == ri)
    {
        d->model().removeRow(index.row(), d->model().index(level, 0));
    }
    else
        Q_ASSERT(false);
}
