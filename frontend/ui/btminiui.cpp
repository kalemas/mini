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
#include <QDesktopWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QStackedWidget>
#include <QtCore/qmath.h>
#include <QtDebug>

#include "../../bibletime/src/backend/managers/cswordbackend.h"
#include "../../bibletime/src/backend/config/btconfig.h"
#include "../../bibletime/src/util/bticons.h"
#include "../../bibletime/src/util/cresmgr.h"

#include "btmini.h"
#include "btminimenu.h"
#include "btminipanel.h"
#include "btminiui.h"
#include "models/btminimodulesmodel.h"
#include "models/btminimoduletextmodel.h"
#include "models/btminisettingsmodel.h"
#include "view/btminilayoutdelegate.h"
#include "view/btminiview.h"
#include "ui/btminiclippingswidget.h"
#include "ui/btminiworkswidget.h"

#if QT_VERSION >= 0x050000
#include <QAbstractNativeEventFilter>
#endif


class BtMiniUiPrivate
{
public:
    enum WidgetFlag
    {
        Main = 1 << 0,
        Works = 1 << 1,
        Rest = 1 << 2,

        Search = 1 << 3,
        Installer = 1 << 4,
        Settings = 1 << 5,
        Context = 1 << 6,
        Clippings = 1 << 7,

        Last = 1 << 8
    };

public:
    BtMiniUiPrivate(BtMiniUi *parent) : q_ptr(parent)
        , _mainWidget(0)
        , _worksWidget(0)
        , _searchWidget(0)
        , _installerWidget(0)
        , _settingsWidget(0)
        , _clippingsWidget(0)
        , _installModel(0)
        , _haveBible(false) {;}
    ~BtMiniUiPrivate()
    {
        ;
    }


    class BtMiniMainWidget : public QStackedWidget
    {
    public:
        BtMiniMainWidget() : QStackedWidget()
        {
            setAttribute(Qt::WA_DeleteOnClose);
            setFrameStyle(QFrame::NoFrame);
            _saveTimer = startTimer(5*60*1000);

            setWindowTitle("BibleTime Mini");
            setWindowIcon(BtIcons::instance().icon_bibletime);

#if QT_VERSION >= 0x050200
            QObject::connect(QApplication::instance(), SIGNAL(applicationStateChanged(Qt::ApplicationState)),
                    BtMiniUi::instance(), SLOT(applicationStateChanged()));
#endif

            const int v = btConfig().value<int>("mini/keepScreenAwake", -1);
            if(v >= 0)
                BtMini::keepScreenAwake(v);
        }

        ~BtMiniMainWidget()
        {
            saveConfig();
        }

        /** Save opened session. */
        static void saveConfig()
        {
            if(BtMiniUi::instance()->d_func()->_worksWidget)
            {
                QModelIndexList list = BtMiniUi::instance()->worksView()->currentIndexes();
                QModelIndex index = BtMiniUi::instance()->worksView()->currentIndex();
                QStringList modules, places;

                for(int i = 0; i < list.size(); ++i)
                {
                    if(list[i] == index)
                        btConfig().setValue<int>("mini/openModule", i);

                    modules.append(list[i].data(BtMini::ModuleRole).toString());
                    places.append(list[i].data(BtMini::PlaceRole).toString());
                }

                btConfig().setValue<QStringList>("mini/openModules", modules);
                btConfig().setValue<QStringList>("mini/openPlaces", places);

                btConfig().sync();
            }
        }

        /** */
        enum ScreenOrientation {
            ScreenOrientationLockPortrait,
            ScreenOrientationLockLandscape,
            ScreenOrientationAuto
        };
        void setOrientation(ScreenOrientation orientation)
        {
#if QT_VERSION < 0x050000
#ifdef Q_OS_SYMBIAN
                // If the version of Qt on the device is < 4.7.2, that attribute won't work
                if (orientation != ScreenOrientationAuto) {
                    const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
                    if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
                        qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
                        return;
                    }
                }
#endif // Q_OS_SYMBIAN

            Qt::WidgetAttribute attribute;
            switch (orientation) {
#if QT_VERSION < 0x040702
            // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
            case ScreenOrientationLockPortrait:
                attribute = static_cast<Qt::WidgetAttribute>(128);
                break;
            case ScreenOrientationLockLandscape:
                attribute = static_cast<Qt::WidgetAttribute>(129);
                break;
            default:
            case ScreenOrientationAuto:
                attribute = static_cast<Qt::WidgetAttribute>(130);
                break;
#else
            case ScreenOrientationLockPortrait:
                attribute = Qt::WA_LockPortraitOrientation;
                break;
            case ScreenOrientationLockLandscape:
                attribute = Qt::WA_LockLandscapeOrientation;
                break;
            case ScreenOrientationAuto:
            default:
                attribute = Qt::WA_AutoOrientation;
                break;
#endif
            };
            setAttribute(attribute, true);
#else
            Q_UNUSED(orientation);
    #endif
        }

        void showNormal()
        {
#if defined  Q_OS_WIN32 || (defined Q_OS_LINUX && !defined Q_OS_ANDROID) || defined Q_OS_OSX
            resize(480, 640);
            //show();
            //raise();
            QStackedWidget::showNormal();
#else
            setOrientation(BtMiniMainWidget::ScreenOrientationAuto);

#if defined(Q_OS_WINCE)
            resize(QApplication::desktop()->size());
            show();
            showFullScreen();
#elif defined (Q_OS_ANDROID)
            showMaximized();
#elif defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR) || defined(Q_WS_MAEMO_5) || defined(Q_WS_X11)
            showFullScreen();
#else
            show();
#endif
#endif
            _fullscreen = false;
        }

        void showFullScreen()
        {
            QStackedWidget::showFullScreen();

            _fullscreen = true;
        }

    protected:
        QSize sizeHint() const
        {
            return QWidget::sizeHint().boundedTo(QApplication::desktop()->screenGeometry().size());
        }

        QSize minimumSizeHint() const
        {
            return QWidget::minimumSizeHint().boundedTo(QApplication::desktop()->screenGeometry().size());
        }

        void timerEvent(QTimerEvent *e)
        {
            if(e->timerId() == _saveTimer)
                saveConfig();
            else
                QStackedWidget::timerEvent(e);
        }

//        void resizeEvent(QResizeEvent *e)
//        {
//            qDebug() << "Resize main widget" << e->oldSize() << "to" << e->size() << "desktop" << QApplication::desktop()->screenGeometry();
//            QStackedWidget::resizeEvent(e);
//        }

        void keyReleaseEvent(QKeyEvent *e)
        {
            if(e->key() == Qt::Key_Back || (e->modifiers() & Qt::AltModifier
                && (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Left)))
            {
                if(!BtMiniUi::instance()->goBack())
                {
                    qDebug() << "close main widget:" << BtMiniUi::instance()->d_func()->_widgetStack;
                    close();
                }
                e->accept();
                return;
            }

            QStackedWidget::keyReleaseEvent(e);
        }

        bool eventFilter(QObject *o, QEvent *e)
        {
            if(e->type() == QEvent::MouseButtonDblClick)
            {
                Q_ASSERT(o == BtMiniUi::instance()->worksView()->viewport());

                if(_fullscreen)
                    showNormal();
                else
                    showFullScreen();
            }
            return false;
        }

    private:
        int  _saveTimer;
        bool _fullscreen;
    };

    class BtMiniWidget : public QWidget
    {
    public:
        BtMiniWidget(QWidget *parent=0) : QWidget(parent) {;}
        ~BtMiniWidget() {;}

    protected:
        QSize sizeHint() const
        {
            return QWidget::sizeHint().boundedTo(parentWidget()->size());
        }

        QSize minimumSizeHint() const
        {
            return QWidget::minimumSizeHint().boundedTo(parentWidget()->size());
        }

//        void resizeEvent(QResizeEvent *e)
//        {
//            //qDebug() << "Resize widget" << e->oldSize() << "to" << e->size() << "desktop";
//            QWidget::resizeEvent(e);
//        }
    };


    void createMainWidget()
    {
        Q_ASSERT(_mainWidget == 0);

        // TODO qml frontend

        _mainWidget = new BtMiniMainWidget;
        _mainWidget->showNormal();

        /*  device          resolution          logical dpi         physical dpi
            htc hd2         800x480             138                 254
                factor 30 is ok
            htc diamond                         192
            desktop         1920x1200           96                  72
                factor 26 is ok
            nexus 10        1454                200                 321
                factor 72 is ok
            nexus 7         800                 133                 214
                factor 45 is ok
            wexler 10is     1368x720            72                  120
                factor 31 is big
        */

#if defined  Q_OS_WIN32 || (defined Q_OS_LINUX && !defined Q_OS_ANDROID)
        qreal sf = QApplication::desktop()->logicalDpiY() / 4.2;
#else
        qreal dpi = (QApplication::desktop()->physicalDpiX()/2.0 + QApplication::desktop()->physicalDpiY()/2.0);
        qreal screenSize = qSqrt(qPow(QApplication::desktop()->screenGeometry().width(), 2) +
                                 qPow(QApplication::desktop()->screenGeometry().height(), 2)) / dpi;

        // average of fixed physical (3mm) font size and screen resolution dependent size
        qreal sf = (dpi / (32.0 / screenSize) + (dpi / 8.4)) / 2.0;
#endif
        _sizeFactor = qMax((qreal)10.0, qMin((qreal)200.0, sf));

        qDebug() << "Surface:" << _mainWidget->size() << "  Screen:" << QApplication::desktop()->screenGeometry() <<
                    "  Dpi lx ly px py:" << _mainWidget->logicalDpiX() << _mainWidget->logicalDpiY() <<
                    _mainWidget->physicalDpiX() << _mainWidget->physicalDpiY() << "  Size factor:" << _sizeFactor;

        updateMainWidget();
    }

    void createWorksWidget()
    {
        Q_Q(BtMiniUi);
        Q_ASSERT(_worksWidget == 0);
        Q_CHECK_PTR(_mainWidget);

        _worksWidget = new BtMiniWorksWidget(_mainWidget);
        _mainWidget->addWidget(_worksWidget);

        // this is for double click handling to switch fullscreen
        BtMiniUi::instance()->worksView()->viewport()->installEventFilter(_mainWidget);
    }

    void createSearchWidget()
    {
        Q_Q(BtMiniUi);
        Q_ASSERT(_searchWidget == 0);
        Q_CHECK_PTR(_mainWidget);

        _searchWidget = new BtMiniWidget(_mainWidget);
        _mainWidget->addWidget(_searchWidget);

        BtMiniView *v = new BtMiniView(_searchWidget);
        v->setTopShadow(true);
        v->setWebKitEnabled(btConfig().value<bool>("mini/useWebKit", false));
        BtMiniUi::changeFontSize(v, btConfig().value<int>("mini/fontTextScale", 140) / 100.0);

        QLineEdit *le = new QLineEdit(_searchWidget);
        le->setAlignment(Qt::AlignCenter);
        le->setPlaceholderText(BtMiniUi::tr("search string"));
        BtMiniUi::changeFontSize(le, 0.95);
        le->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

        QPushButton *pb = q->makeButton(BtMiniUi::tr("Back"), ":/mini-back.svg", ":/mini-back-night.svg");
        QPushButton *eb = q->makeButton("", BtIcons::instance().icon_find);

        BtMiniPanel *p = new BtMiniPanel;
        p->addWidget(pb, Qt::AlignLeft);
        p->addWidget(le, Qt::AlignCenter);
        p->addWidget(eb, Qt::AlignRight);

        // Put into layout
        QVBoxLayout *vl = new QVBoxLayout;
        vl->addWidget(p);
        vl->addWidget(v);

        _searchWidget->setLayout(vl);

        // Setup model
        BtMiniModuleTextModel *m = new BtMiniModuleTextModel(QStringList() << "[Search]", v);
        v->setModel(m);

        QObject::connect(le, SIGNAL(textChanged(const QString &)), m, SLOT(setSearchText(const QString &)));
        QObject::connect(le, SIGNAL(returnPressed()), m, SLOT(startSearch()));
        QObject::connect(eb, SIGNAL(clicked()), m, SLOT(startSearch()));
        QObject::connect(v, SIGNAL(shortPressed(const QModelIndex &)), m, SLOT(openContext(const QModelIndex &)));
        QObject::connect(v, SIGNAL(longPressed(const QModelIndex &)), _worksWidget->findChild<BtMiniView*>(), SLOT(scrollTo(const QModelIndex &)));
        QObject::connect(v, SIGNAL(longPressed(const QModelIndex &)), BtMiniUi::instance(), SLOT(activateWorks()));
        QObject::connect(pb, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(goBack()));
    }

    void createInstallerWidget()
    {
        Q_Q(BtMiniUi);
        Q_ASSERT(_installerWidget == 0);
        Q_CHECK_PTR(_mainWidget);

        _installerWidget = new BtMiniWidget(_mainWidget);
        _mainWidget->addWidget(_installerWidget);

        BtMiniView *v = new BtMiniView(_installerWidget);
        v->setTopShadow(true);

        BtMiniPanel *p = new BtMiniPanel;

        QPushButton *bb = 0;
        if(_worksWidget)
            bb = q->makeButton(BtMiniUi::tr("Back"), ":/mini-back.svg", ":/mini-back-night.svg");
#ifdef BT_MINI_EXIT_BUTTON
        else
            bb = q->makeButton(BtMiniUi::tr("Exit"));
#endif
        if(bb) bb->setObjectName("back");

        //QPushButton *rb = new QPushButton(BtMiniUi::tr("Refresh"));
        QPushButton *rb = q->makeButton("", BtIcons::instance().icon_refresh);

        QLabel *lb = new QLabel(BtMiniModulesModel::tr("Updating") + "...", _installerWidget);
        lb->setAlignment(Qt::AlignCenter);
        lb->setObjectName("label");

        p->addWidget(lb, Qt::AlignCenter);
        if(bb) p->addWidget(bb, Qt::AlignLeft);
        p->addWidget(rb, Qt::AlignRight);

        QVBoxLayout *vl = new QVBoxLayout;
        vl->addWidget(p);
        vl->addWidget(v);

        _installerWidget->setLayout(vl);

        // setup model
        if(!_installModel)
            _installModel = new BtMiniModulesModel(true, _mainWidget);

        _installModel->setIndicator(lb);

        QObject::connect(v, SIGNAL(currentChanged(QModelIndex)), _installModel, SLOT(updateIndicators(QModelIndex)));
        v->disconnect(SIGNAL(clicked(const QModelIndex &)));
        QObject::connect(v, SIGNAL(clicked(const QModelIndex &)), _installModel, SLOT(installerQuery(const QModelIndex &)));

        v->setModel(_installModel);
        _installModel->updateIndicators();

        if(_worksWidget)
            QObject::connect(bb, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(goBack()));
        else if(bb)
            QObject::connect(bb, SIGNAL(clicked()), BtMiniUi::instance()->mainWidget(), SLOT(close()));
        QObject::connect(rb, SIGNAL(clicked()), _installModel, SLOT(backgroundDownload()));
    }

    void createSettingsWidget()
    {
        Q_Q(BtMiniUi);
        Q_ASSERT(_settingsWidget == 0);
        Q_CHECK_PTR(_mainWidget);

        _settingsWidget = new BtMiniWidget(_mainWidget);
        _mainWidget->addWidget(_settingsWidget);

        BtMiniView *v = new BtMiniView(_settingsWidget);
        v->setTopShadow(true);

        BtMiniSettingsModel *m = new BtMiniSettingsModel(v);
        v->setModel(m);

        QPushButton *bb = q->makeButton(BtMiniUi::tr("Apply"), ":/mini-back.svg", ":/mini-back-night.svg");

        QLabel *lb = new QLabel(BtMiniUi::tr("Settings"));
        lb->setAlignment(Qt::AlignCenter);

        BtMiniPanel *p = new BtMiniPanel;
        p->addWidget(lb, Qt::AlignCenter);
        p->addWidget(bb, Qt::AlignLeft);

        QVBoxLayout *vl = new QVBoxLayout;
        vl->addWidget(p);
        vl->addWidget(v);

        _settingsWidget->setLayout(vl);

        QObject::connect(v, SIGNAL(clicked(const QModelIndex &)), m, SLOT(clicked(const QModelIndex &)));
        QObject::connect(bb, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(goBack()));
    }

    void createClippingsWidget()
    {
        Q_Q(BtMiniUi);
        Q_ASSERT(_clippingsWidget == 0);
        Q_CHECK_PTR(_mainWidget);

        _clippingsWidget = new BtMiniClippingsWidget(_mainWidget);
        _mainWidget->addWidget(_clippingsWidget);
    }

    void updateMainWidget()
    {
        Q_CHECK_PTR(_mainWidget);

        QFont f = _mainWidget->font();
        f.setPixelSize(_sizeFactor * btConfig().value<int>("mini/fontScale", 100) / 100.0);
        f.setFamily(btConfig().value<QString>("mini/fontFamily", QApplication::font().family()));
        _mainWidget->setFont(f);

        QString s = btConfig().value<QString>("mini/miniStyle", "mini");
        if(s != _mainWidget->style()->objectName())
            QApplication::setStyle(s);

//        QFont cf(f);
//        cf.setFamily(btConfig().value<QString>("mini/fontTextFamily", "jGaramond"));
//        btConfig().setDefaultFont(cf);
    }

    /** Return basic icon size for BtMini interface. */
    QSize getIconSize(QIcon i)
    {
        int h = _mainWidget->font().pixelSize();
        if(h < 0)
        {
            QFontInfo i(_mainWidget->font());
            h = i.pixelSize();
        }
        h *= 1.6;
        QSize s(i.actualSize(QSize(h, h)));
        return QSize(h / s.height() * s.width(), h);
    }

    /** Do work to switch from current widget type. */
    void switchTo(BtMiniUiPrivate::WidgetFlag widget)
    {
        // sleep works
        if(_worksWidget && _mainWidget->currentWidget() == _worksWidget)
            BtMiniUi::instance()->worksView()->setSleep(true);

        if(_resetFlag & Main)
            updateMainWidget();

        if(_resetFlag & Works && _worksWidget)
        {
            _mainWidget->saveConfig();

            _worksWidget->deleteLater();
            _worksWidget = 0;
        }
        if(_resetFlag & Rest && _installerWidget)
        {
            _installerWidget->deleteLater();
            _installerWidget = 0;
        }
        if(_resetFlag & Rest && _settingsWidget)
        {
            _settingsWidget->deleteLater();
            _settingsWidget = 0;
        }
        if(_resetFlag & Rest && _searchWidget)
        {
            _searchWidget->deleteLater();
            _searchWidget = 0;
        }
        if(_resetFlag & Rest && _clippingsWidget)
        {
            _clippingsWidget->deleteLater();
            _clippingsWidget = 0;
        }

        _resetFlag = 0;

        // remove all context windows
        if(widget != BtMiniUiPrivate::Context)
        {
            _widgetStack.removeAll(BtMiniUiPrivate::Context);
            for(int i = 0; i < _contextWidgets.size(); ++i)
                _contextWidgets.takeLast()->deleteLater();
        }

        if(widget == BtMiniUiPrivate::Works || widget == BtMiniUiPrivate::Installer ||
           widget == BtMiniUiPrivate::Search  || widget == BtMiniUiPrivate::Settings ||
           widget == BtMiniUiPrivate::Clippings)
        {
            if(!_widgetStack.contains(widget))
                _widgetStack.append(widget);
        }
    }

    void setupDefaultModules()
    {
        // setup default modules if not set
#define SETUP_STANDARD_MODULE(a, b, c, d) \
    if(btConfig().getDefaultSwordModuleByType(a) == 0) { \
        Q_FOREACH(CSwordModuleInfo *m, QList<CSwordModuleInfo*>() << CSwordBackend::instance()->findModuleByName(c) << \
            CSwordBackend::instance()->findModuleByName(b) << CSwordBackend::instance()->moduleList()) \
            if(m && d) { btConfig().setDefaultSwordModuleByType(a, m); break; } }

        SETUP_STANDARD_MODULE("standardBible", "KJV", BtMiniUi::tr("KJV", "default Bible module"), m->type() == CSwordModuleInfo::Bible);
        SETUP_STANDARD_MODULE("standardCommentary", "MHC", BtMiniUi::tr("MHC", "default commentary module"), m->type() == CSwordModuleInfo::Commentary);
        SETUP_STANDARD_MODULE("standardLexicon", "ISBE", BtMiniUi::tr("ISBE", "default lexicon module"), m->type() == CSwordModuleInfo::Lexicon && \
            !m->has(CSwordModuleInfo::HebrewDef) && !m->has(CSwordModuleInfo::GreekDef) && \
            !m->has(CSwordModuleInfo::HebrewParse) && !m->has(CSwordModuleInfo::GreekParse));
        SETUP_STANDARD_MODULE("standardHebrewStrongsLexicon", "StrongsHebrew", BtMiniUi::tr("StrongsHebrew", "default Hebrew strongs lexicon"), \
            m->type() == CSwordModuleInfo::Lexicon && m->has(CSwordModuleInfo::HebrewDef));
        SETUP_STANDARD_MODULE("standardGreekStrongsLexicon", "StrongsGreek", BtMiniUi::tr("StrongsGreek", "default Greek morph lexicon"), \
            m->type() == CSwordModuleInfo::Lexicon && m->has(CSwordModuleInfo::GreekDef));
        SETUP_STANDARD_MODULE("standardHebrewMorphLexicon", "StrongsHebrew", BtMiniUi::tr("StrongsHebrew", "default Hebrew strongs lexicon"), \
            m->type() == CSwordModuleInfo::Lexicon && m->has(CSwordModuleInfo::HebrewParse));
        SETUP_STANDARD_MODULE("standardGreekMorphLexicon", "StrongsGreek", BtMiniUi::tr("StrongsGreek", "default Greek morph lexicon"), \
                              m->type() == CSwordModuleInfo::Lexicon && m->has(CSwordModuleInfo::GreekParse));
    }


    BtMiniMainWidget            *_mainWidget;
    QWidget                     *_worksWidget;
    QWidget                     *_searchWidget;
    QWidget                     *_installerWidget;
    QWidget                     *_settingsWidget;
    QWidget                     *_clippingsWidget;
    QList<QWidget*>              _contextWidgets;

    BtMiniModulesModel          *_installModel;

    bool                         _haveBible;

    qreal                        _sizeFactor;

    QFlags<WidgetFlag>           _resetFlag;
    QList<WidgetFlag>            _widgetStack;


    Q_DECLARE_PUBLIC(BtMiniUi);
    BtMiniUi * const             q_ptr;
};

BtMiniUi::BtMiniUi(QObject *parent)
    : QObject(parent)
    , d_ptr(new BtMiniUiPrivate(this))
{
    ;
}

BtMiniUi::~BtMiniUi()
{
    delete d_ptr;
}

BtMiniUi* BtMiniUi::instance()
{
    // for iOS this required to be in implementation
    static BtMiniUi ui;
    return &ui;
}

void BtMiniUi::show()
{
    Q_D(BtMiniUi);

	// need to do queued connection otherwise install dialogs wontn't close
	qRegisterMetaType<CSwordBackend::SetupChangedReason>("CSwordBackend::SetupChangedReason");
    connect(CSwordBackend::instance(), SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
		this, SLOT(modulesReloaded()), Qt::QueuedConnection);

    foreach(CSwordModuleInfo *m, CSwordBackend::instance()->moduleList())
        if(m->type() == CSwordModuleInfo::Bible)
        {
            d->_haveBible = true;
            break;
        }

    d->createMainWidget();

    if(!d->_haveBible)
        activateInstaller();
    else
    {
        d->setupDefaultModules();
        activateWorks();
    }
}

QWidget *BtMiniUi::mainWidget()
{
    Q_D(BtMiniUi);
    return d->_mainWidget;
}

QWidget *BtMiniUi::worksWidget()
{
    Q_D(BtMiniUi);
    return d->_worksWidget;
}

BtMiniView *BtMiniUi::worksView()
{
    Q_D(BtMiniUi);

    if(!d->_worksWidget)
        return 0;
    return d->_worksWidget->findChild<BtMiniView*>();
}

BtMiniView *BtMiniUi::searchView()
{
    Q_D(BtMiniUi);

    if(!d->_searchWidget)
        return 0;
    return d->_searchWidget->findChild<BtMiniView*>();
}

void BtMiniUi::resetWidgets(bool main, bool works, bool rest)
{
    Q_D(BtMiniUi);

    d->_resetFlag = 0;
    if(main)
        d->_resetFlag |= BtMiniUiPrivate::Main;
    if(works)
        d->_resetFlag |= BtMiniUiPrivate::Works;
    if(rest)
        d->_resetFlag |= BtMiniUiPrivate::Rest;
}

QWidget* BtMiniUi::activateNewContextWidget()
{
    Q_D(BtMiniUi);

    d->switchTo(BtMiniUiPrivate::Context);

    d->_widgetStack.append(BtMiniUiPrivate::Context);
    d->_contextWidgets.append(new QWidget(d->_mainWidget));
    d->_mainWidget->addWidget(d->_contextWidgets.last());
    d->_mainWidget->setCurrentWidget(d->_contextWidgets.last());

    return d->_contextWidgets.last();
}

QWidget *BtMiniUi::currentContextWidget()
{
    Q_D(BtMiniUi);

    Q_ASSERT(d->_widgetStack.last() == BtMiniUiPrivate::Context);
    return d->_contextWidgets.last();
}

QPushButton* BtMiniUi::makeButton(QString text, QString icon, QString invertedIcon)
{
    QPushButton *b = new QPushButton(text);
    b->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    if(!icon.isEmpty())
    {
        if(!invertedIcon.isEmpty() && BtMiniUi::instance()->mainWidget()->style()->standardPalette().background().color().lightnessF() < 0.2)
            b->setIcon(QIcon(invertedIcon));
        else
            b->setIcon(QIcon(icon));

        b->setIconSize(BtMiniUi::getIconSize(b->icon()));
    }
    return b;
}

QPushButton *BtMiniUi::makeButton(QString text, QIcon icon)
{
    QPushButton *b = new QPushButton(text);
    b->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    if(!icon.isNull())
    {
        b->setIcon(icon);
        b->setIconSize(BtMiniUi::getIconSize(b->icon()));
    }
    return b;
}

QSize BtMiniUi::getIconSize(QIcon icon)
{
    QWidget *m = BtMiniUi::instance()->mainWidget();
    Q_CHECK_PTR(m);
    int h = m->font().pixelSize();
    if(h < 0)
    {
        QFontInfo i(m->font());
        h = i.pixelSize();
    }
    h *= 1.6;
    QSize s(icon.actualSize(QSize(h, h)));
    return QSize(h / s.height() * s.width(), h);
}

void BtMiniUi::changeFontSize(QWidget *w, qreal factor)
{
    QFont f(w->font());
    f.setPixelSize(f.pixelSize() * factor);
    w->setFont(f);
}

bool BtMiniUi::goBack()
{
    Q_D(BtMiniUi);

    // check if we could switch previus list in current view
    BtMiniView * v = d->_mainWidget->currentWidget()->findChild<BtMiniView*>();
    if(v && !v->layoutDelegate()->plainMode() && v->slideLeft())
        return true;

    if(d->_widgetStack.size() <= 1)
        return false;

    if(d->_widgetStack.last() == BtMiniUiPrivate::Context)
        d->_contextWidgets.takeLast()->deleteLater();

    d->_widgetStack.removeLast();

    switch(d->_widgetStack.last())
    {
        case BtMiniUiPrivate::Works: activateWorks(); break;
        case BtMiniUiPrivate::Installer: activateInstaller(); break;
        case BtMiniUiPrivate::Settings: activateSettings(); break;
        case BtMiniUiPrivate::Search: activateSearch(); break;
        case BtMiniUiPrivate::Clippings: activateClippings(); break;
        case BtMiniUiPrivate::Context:
            d->switchTo(BtMiniUiPrivate::Context);
            d->_mainWidget->setCurrentWidget(d->_contextWidgets.last());
            break;
        default:
            Q_ASSERT(false);
    }
    return true;
}

void BtMiniUi::applicationStateChanged()
{
    Q_D(BtMiniUi);

#if QT_VERSION >= 0x050200
    // on android we get this function called simultaneously
    static QMutex m;
    QMutexLocker ml(&m);

    if(QApplication::applicationState() == Qt::ApplicationSuspended)
    {
        d->_mainWidget->saveConfig();
    }
    if(QApplication::applicationState() == Qt::ApplicationHidden ||
       QApplication::applicationState() == Qt::ApplicationInactive)
    {
        if(d->_widgetStack.last() == BtMiniUiPrivate::Works)
            worksView()->setSleep(true, 1.0);
    }
    if(QApplication::applicationState() == Qt::ApplicationActive)
    {
        if(d->_widgetStack.last() == BtMiniUiPrivate::Works)
            worksView()->setSleep(false);
    }
#endif
}

void BtMiniUi::activateWorks()
{
    Q_D(BtMiniUi);

    d->switchTo(BtMiniUiPrivate::Works);

    if(!d->_worksWidget)
        d->createWorksWidget();

    d->_mainWidget->setCurrentWidget(d->_worksWidget);
    worksView()->setSleep(false);

    // tips
    static bool shown = false;
    if(!shown && btConfig().value<bool>("mini/showTipAtStartup", true))
    {
        if(BtMiniMenu::execTip(BtMiniSettingsModel::standardData(BtMiniSettingsModel::tipWorksAddon).toString() +
                            BtMiniSettingsModel::standardData(BtMiniSettingsModel::TipWorks).toString()) == 1)
            btConfig().setValue<bool>("mini/showTipAtStartup", false);
        shown = true;
    }
}

void BtMiniUi::activateInstaller()
{
    Q_D(BtMiniUi);

    d->switchTo(BtMiniUiPrivate::Installer);

    if(!d->_installerWidget)
        d->createInstallerWidget();

    d->_mainWidget->setCurrentWidget(d->_installerWidget);
}

void BtMiniUi::activateSearch()
{
    Q_D(BtMiniUi);

    d->switchTo(BtMiniUiPrivate::Search);

    if(!d->_searchWidget)
        d->createSearchWidget();

    d->_mainWidget->setCurrentWidget(d->_searchWidget);
}

void BtMiniUi::activateSettings()
{
    Q_D(BtMiniUi);

    d->switchTo(BtMiniUiPrivate::Settings);

    if(!d->_settingsWidget)
        d->createSettingsWidget();

    d->_mainWidget->setCurrentWidget(d->_settingsWidget);

    // always open on first page
    QModelIndex i = d->_settingsWidget->findChild<BtMiniView*>()->currentIndex();
    if(i.parent().isValid())
    {
        while(i.parent().isValid()) i = i.parent();
        d->_settingsWidget->findChild<BtMiniView*>()->scrollTo(i);
    }
}

void BtMiniUi::activateClippings()
{
    Q_D(BtMiniUi);

    d->switchTo(BtMiniUiPrivate::Clippings);

    if(!d->_clippingsWidget)
        d->createClippingsWidget();

    d->_mainWidget->setCurrentWidget(d->_clippingsWidget);
}

void BtMiniUi::modulesReloaded()
{
    Q_D(BtMiniUi);

    if(!d->_haveBible)
    {
        bool haveBible = false;
        foreach(CSwordModuleInfo *m, CSwordBackend::instance()->moduleList())
            if(m->type() == CSwordModuleInfo::Bible)
            {
                haveBible = true;
                break;
            }

        // case when the first Bible was installed runtime
        if(haveBible)
        {
            d->_haveBible = true;

            // HACK clear widget set and start all again
            resetWidgets(false, false, true);
            Q_ASSERT(d->_widgetStack.size() == 1);
            d->_widgetStack.clear();

            activateWorks();
        }
    }

    d->setupDefaultModules();
}

void BtMiniUi::openWorksMenu()
{
    switch(BtMiniMenu::execMenu(QStringList() << "Install" << "Clippings"))
    {
    case 0:
        activateInstaller();
        break;
    case 1:
        activateClippings();
        break;
    }
}

