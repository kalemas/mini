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
#include <QBoxLayout>
#include <QEventLoop>
#include <QLabel>
#include <QPainter>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QStandardItemModel>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStyleOptionFrame>
#include <QStyleOptionMenuItem>
#include <QtDebug>

#include "view/btminiview.h"
#include "view/btminilayoutdelegate.h"
#include "btminimenu.h"
#include "btminiui.h"


class BtMiniMenuPrivate
{
public:
    BtMiniMenuPrivate()
    {
        _result    = -1;
		_eventLoop = 0;
        _modal     = false;
        _canceled  = false;
        _isPopup   = false;
        _isInput   = false;
    }
    
    ~BtMiniMenuPrivate()
    {
        ;
    }

	void updateGeometry(QWidget *menu)
	{
		menu->adjustSize();
        const QSize s = (BtMiniMenuPrivate::parentWidget()->geometry().size() - menu->frameSize())/2;
		menu->move(QPoint(s.width(), s.height()));
	}

    static QWidget * parentWidget()
    {
        QWidget *w = BtMiniUi::instance()->mainWidget();
        Q_CHECK_PTR(w);
        //return QApplication::topLevelWidgets()[0];
        return w;
    }
    
    QList<QWidget*>  _buttons;
    int              _result;
	QEventLoop      *_eventLoop;
    bool             _modal;
	bool             _canceled;
    bool             _isPopup;
    bool             _isInput;
    QString          _inputPattern;
    int              _inputMin;
    int              _inputMax;

};

// FramelessWindowHint needed for Windows styles
BtMiniMenu::BtMiniMenu() : d_ptr(new BtMiniMenuPrivate)
    , QWidget(BtMiniMenuPrivate::parentWidget(), Qt::FramelessWindowHint)
{
	//setWindowModality(Qt::ApplicationModal);
    //setModal(false);
    //setAutoFillBackground(false);
	//setAttribute(Qt::WA_MouseNoMask);
	//setAttribute(Qt::WA_NoMousePropagation);
    //setAutoFillBackground(true);

	if(parentWidget())
	{
		setMaximumSize(parentWidget()->size());

		QFont f(parentWidget()->font());
		f.setPixelSize(f.pixelSize() * 1.3);
		setFont(f);
	}
	else
		Q_ASSERT(false);

	hide();
}

BtMiniMenu::~BtMiniMenu()
{
	if(d_ptr->_eventLoop)
		d_ptr->_eventLoop->exit();

	delete d_ptr;
}

void BtMiniMenu::show()
{
	d_ptr->updateGeometry(this);

    qApp->installEventFilter(this);

    QWidget::show();
}

void BtMiniMenu::hide()
{
    QWidget::hide();

    qApp->removeEventFilter(this);
}

void BtMiniMenu::exec()
{
	QEventLoop eventLoop;
	d_ptr->_eventLoop = &eventLoop;

	show();

    // FIX on Android, virtual keyboard didnt hide when action key is pressed
    setFocus();

	QPointer<QObject> guard = this;
	
	eventLoop.exec();
	
	if(guard.isNull())
		return;

	d_ptr->_eventLoop = 0;
}

void BtMiniMenu::mouseMoveEvent(QMouseEvent *e)
{
}

void BtMiniMenu::mousePressEvent(QMouseEvent *e)
{
}

void BtMiniMenu::mouseDoubleClickEvent(QMouseEvent *e)
{
}

void BtMiniMenu::mouseReleaseEvent(QMouseEvent *e)
{
}

QSize BtMiniMenu::sizeHint() const
{
    QSize s(BtMiniMenuPrivate::parentWidget()->size());

	// take some place
	if(s.width() * 0.8 > s.height())
		s.setWidth(s.width() * 0.9 - (parentWidget()->fontMetrics().height() * 2));
	else
		s.setHeight(s.height() * 0.9 - (parentWidget()->fontMetrics().height() * 2));

	return QWidget::sizeHint().boundedTo(s);
}

QSize BtMiniMenu::minimumSizeHint() const
{
	return QWidget::minimumSizeHint().boundedTo(parentWidget()->size());
}

QWidget * BtMiniMenu::buttonAt(int id) const
{
    return d_ptr->_buttons[id];
}

void BtMiniMenu::buttonTrigger()
{
    if(d_ptr->_isInput)
    {
        QPushButton *b = qobject_cast<QPushButton*>(sender());
        if(b)
        {
            if(b->text() == " + " && d_ptr->_result < d_ptr->_inputMax)
                d_ptr->_result++;

            if(b->text() == " - " && d_ptr->_result > d_ptr->_inputMin)
                d_ptr->_result--;

            QList<QLabel *> l = sender()->parent()->findChildren<QLabel *>("indicator");

            if(l.size() == 1)
                l[0]->setText(d_ptr->_inputPattern.arg(d_ptr->_result));
        }
    }
    else
    {
        d_ptr->_result = d_ptr->_buttons.indexOf(qobject_cast<QWidget*>(sender()));
        hide();
    }
}

BtMiniMenu * BtMiniMenu::createQuery(QString text, QStringList actions)
{
    BtMiniMenu *dialog = new BtMiniMenu;
    
    QVBoxLayout *v = new QVBoxLayout;

    const int m = dialog->font().pixelSize() / 4;
    v->setSpacing(m);
    dialog->setContentsMargins(m, m, m, m);
    
    // add menu text
    if(!text.isEmpty())
    {
        QLabel *l = new QLabel(text, dialog);
        l->setWordWrap(true);

        QFont f(l->font());
        f.setWeight(QFont::Normal);
        l->setFont(f);

        // FIX QLabel sometimes vertical cut when multiline
        l->setMinimumHeight(f.pixelSize() * 2);

        v->addWidget(l, 0, Qt::AlignCenter);
    }
    
    // add actions
    if(actions.size())
    {
        QLayout *l;
        
        if(!text.isEmpty())
            l = new QHBoxLayout;
        else
            l = v;
        
        foreach(QString string, actions)
        {
            QPushButton *b = new QPushButton(string, dialog);

            QFont f = b->font();
            f.setBold(true);
            b->setFont(f);;

            connect(b, SIGNAL(clicked()), dialog, SLOT(buttonTrigger()));
            dialog->d_ptr->_buttons.append(b);
            l->addWidget(b);
        }
        
        if(!text.isEmpty())
            v->addLayout(l);
    }
    else
        dialog->d_ptr->_isPopup = true;
    
	dialog->setLayout(v);
	
    return dialog;
}

int BtMiniMenu::execQuery(QString text, QStringList actions)
{
    QScopedPointer<BtMiniMenu> dialog(createQuery(text, actions));
    dialog->exec();
    return dialog->d_ptr->_result;
}

int BtMiniMenu::execMenu(QStringList actions)
{
    QScopedPointer<BtMiniMenu> dialog(createQuery(QString(), actions));
    dialog->exec();
    return dialog->d_ptr->_result;
}

int BtMiniMenu::execInput(QString caption, QString pattern, int currentValue, int minValue, int maxValue)
{
    QScopedPointer<BtMiniMenu> dialog(new BtMiniMenu);

    dialog->d_ptr->_isInput = true;
    dialog->d_ptr->_result = currentValue;
    dialog->d_ptr->_inputPattern = pattern;
    dialog->d_ptr->_inputMin = minValue;
    dialog->d_ptr->_inputMax = maxValue;

    QVBoxLayout *vl = new QVBoxLayout;

    const int m = dialog->font().pixelSize() / 4;
    vl->setSpacing(m);
    dialog.data()->setContentsMargins(m, m, m, m);

    // add caption
    QLabel *l = new QLabel(caption, dialog.data());
    l->setWordWrap(true);

    QFont f(l->font());
    f.setWeight(QFont::Normal);
    f.setPixelSize(f.pixelSize() * 0.66);
    l->setFont(f);

    vl->addWidget(l, 0, Qt::AlignCenter);

    // add indicator
    QLabel *l2 = new QLabel(pattern.arg(currentValue), dialog.data());

    l2->setObjectName("indicator");
    l2->setAlignment(Qt::AlignCenter);

    f = l2->font();
    f.setWeight(QFont::Bold);
    f.setPixelSize(f.pixelSize() * 1.2);
    l2->setFont(f);

    vl->addWidget(l2, Qt::AlignCenter);

    // add controls
    QHBoxLayout *hl = new QHBoxLayout;

    f.setWeight(QFont::Bold);

    QPushButton *b1 = new QPushButton(" - ", dialog.data());
    b1->setAutoRepeat(true);
    b1->setFont(f);
    connect(b1, SIGNAL(clicked()), dialog.data(), SLOT(buttonTrigger()));
    hl->addWidget(b1, 0);

    QPushButton *b2 = new QPushButton(tr("Ok"), dialog.data());
    b2->setFont(f);
    connect(b2, SIGNAL(clicked()), dialog.data(), SLOT(hide()));
    hl->addWidget(b2, 0);

    QPushButton *b3 = new QPushButton(" + ", dialog.data());
    b3->setFont(f);
    b3->setAutoRepeat(true);
    connect(b3, SIGNAL(clicked()), dialog.data(), SLOT(buttonTrigger()));
    hl->addWidget(b3, 0);

    vl->addLayout(hl);

    dialog->setLayout(vl);

    dialog->exec();

    if(dialog->wasCanceled())
        return currentValue;
    else
        return dialog->d_ptr->_result;
}

void BtMiniMenu::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    
    // FIX QStyle::PE_PanelMenu on windows is hollow
//#if defined Q_OS_WIN || defined ANDROID || defined __unix__ || defined Q_OS_SYMBIAN
    QStyleOptionButton opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &p, this);
//#else
//    QStyleOptionMenuItem opt;
//    opt.initFrom(this);
//    opt.state = QStyle::State_None;
//    opt.checkType = QStyleOptionMenuItem::NotCheckable;
//    opt.maxIconWidth = 0;
//    opt.tabWidth = 0;
//    style()->drawPrimitive(QStyle::PE_PanelMenu, &opt, &p, this);
//#endif

    if(const int fw = style()->pixelMetric(QStyle::PM_MenuPanelWidth, 0, this))
    {
        QStyleOptionFrame frame;
        frame.rect = rect();
        frame.palette = palette();
        frame.state = QStyle::State_None;
        frame.lineWidth = style()->pixelMetric(QStyle::PM_MenuPanelWidth);
        frame.midLineWidth = 0;
        style()->drawPrimitive(QStyle::PE_FrameMenu, &frame, &p, this);
    }
}

bool BtMiniMenu::event(QEvent *e)
{
	return QWidget::event(e);
}

void BtMiniMenu::hideEvent(QHideEvent *e)
{
	if(d_ptr->_eventLoop)
		d_ptr->_eventLoop->exit();
}

bool BtMiniMenu::eventFilter(QObject *o, QEvent *e)
{
    //qDebug() << "menu event filter" << watched << e;

	switch(e->type())
	{
	case QEvent::Resize:
		if(o == parentWidget())
		{
			setMaximumSize(parentWidget()->size());
			d_ptr->updateGeometry(this);
		}
        break;
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseMove:
        {
            if(d_ptr->_canceled && e->type() == QEvent::MouseButtonRelease)
            {
                cancel();
                return true;
            }

            const QPoint p = mapFromGlobal(static_cast<QMouseEvent*>(e)->globalPos());
            const int w = style()->pixelMetric(QStyle::PM_MenuPanelWidth);
            if(d_ptr->_isPopup || !rect().adjusted(w, w, -w, -w).contains(p))
		    {
                if(e->type() == QEvent::MouseButtonPress && !d_ptr->_modal)
                    d_ptr->_canceled = true; // indicate that window shuld be closed on MouseRelease
			    return true;
            }
        }
		break;
    case QEvent::Close:
        if(children().contains(o))
            hide();
        break;
	}
    return false;
}

void BtMiniMenu::closeMenus()
{
    foreach(BtMiniMenu *m, BtMiniMenuPrivate::parentWidget()->findChildren<BtMiniMenu*>())
        m->cancel();
}

BtMiniMenu * BtMiniMenu::createProgress(QString text)
{
    BtMiniMenu *dialog = new BtMiniMenu;

    QVBoxLayout *vl = new QVBoxLayout;

	const int m = dialog->font().pixelSize() / 4;
	vl->setSpacing(m);
    dialog->setContentsMargins(m, m, m, m);

    QLabel *l = new QLabel(text, dialog);
    l->setWordWrap(true);
    vl->addWidget(l, 0, Qt::AlignCenter);

    QProgressBar *pb = new QProgressBar(dialog);
    pb->setRange(0, 100);
    pb->setValue(0);
	pb->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    vl->addWidget(pb, 0, Qt::AlignCenter);

    QPushButton *b = new QPushButton(tr("Cancel"), dialog);
    QFont f(b->font());
    f.setWeight(QFont::Normal);
    b->setFont(f);
    connect(b, SIGNAL(clicked()), dialog, SLOT(cancel()));
    vl->addWidget(b, 0, Qt::AlignCenter);

    dialog->setLayout(vl);
	dialog->d_ptr->_modal = true;

    return dialog;
}

int BtMiniMenu::execTip(QString text)
{
    BtMiniMenu menu;

    QFont f(menu.font());
    f.setBold(true);
    menu.setFont(f);

    QVBoxLayout *vl = new QVBoxLayout;

    BtMiniView *v = new BtMiniView(&menu);
    f = v->font();
    f.setBold(false);
    f.setPixelSize(f.pixelSize() * 0.66);
    v->setFont(f);

    BtMiniLevelOption lo = v->layoutDelegate()->levelOption();
    lo.scrollBarPolicy = Qt::ScrollBarAlwaysOff;
    v->layoutDelegate()->setLevelOption(lo);

    QStandardItemModel *m = new QStandardItemModel(v);
    QStandardItem i(text);
    m->appendRow(&i);
    v->setModel(m);

    vl->addWidget(v);

    QHBoxLayout *hl = new QHBoxLayout;
    QPushButton *b1 = new QPushButton(tr("Ok"), &menu);
    connect(b1, SIGNAL(clicked()), &menu, SLOT(buttonTrigger()));
    menu.d_ptr->_buttons.append(b1);
    hl->addWidget(b1, b1->text().size() + 8);
    QPushButton *b2 = new QPushButton(tr("Don't show"), &menu);
    connect(b2, SIGNAL(clicked()), &menu, SLOT(buttonTrigger()));
    menu.d_ptr->_buttons.append(b2);
    hl->addWidget(b2, b2->text().size() + 8);

    vl->addLayout(hl);

    menu.setLayout(vl);
    menu.exec();

    return menu.d_ptr->_result;
}

void BtMiniMenu::setValue(int percent)
{
    QProgressBar *pb = findChild<QProgressBar*>();
    if(pb)
    {
        pb->setValue(percent);
        qApp->processEvents();
    }
}

void BtMiniMenu::setText(QString text)
{
    QLabel *l = findChild<QLabel*>();
    if(l)
        l->setText(text);
}

bool BtMiniMenu::wasCanceled()
{
    return d_ptr->_canceled;
}

void BtMiniMenu::cancel()
{
	d_ptr->_canceled = true;
    hide();
    emit canceled();
}
