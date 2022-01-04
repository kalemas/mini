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

#ifndef BTMINISTYLE_H
#define BTMINISTYLE_H

#include <QAbstractItemView>
#include <QApplication>
#include <QCommonStyle>
#include <QLayout>
#include <QLineEdit>
#include <QStyleOption>
#include <QStylePlugin>
#include <QPainter>
#include <QPushButton>
#include <QtDebug>
#include <QtCore/qmath.h>
#include <qdrawutil.h>


class BtMiniStyle : public QCommonStyle
{
    Q_OBJECT

public:
    BtMiniStyle(bool night = false)
    {
        Q_INIT_RESOURCE(btministyle);

        _night = night;
        _menuFrame = 0;
        _elideEnabled = false;

        if(_night)
        {
            _palette.setColor(QPalette::Text, QColor(215, 215, 215));
            _palette.setColor(QPalette::Base, QColor(0, 0, 0));
            _palette.setColor(QPalette::Link, QColor("#62a4db"));
            _palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
        }
        else
        {
            _palette.setColor(QPalette::Text, QColor(0, 0, 0));
            _palette.setColor(QPalette::Base, QColor(255, 255, 255));
            _palette.setColor(QPalette::Link, QColor(127, 196, 255));
            _palette.setColor(QPalette::HighlightedText, QColor("#000000"));
        }

        _palette.setColor(QPalette::Window, _palette.color(QPalette::Base));
        _palette.setColor(QPalette::Button, _palette.color(QPalette::Base));
        _palette.setColor(QPalette::WindowText, _palette.color(QPalette::Text));
        _palette.setColor(QPalette::ButtonText, _palette.color(QPalette::Text));
        _palette.setColor(QPalette::Highlight, _palette.color(QPalette::Link));
    }

    ~BtMiniStyle()
    {
        if(_menuFrame)
            delete _menuFrame;
    }

    int baseSize()
    {
        return QApplication::topLevelWidgets().at(0)->font().pixelSize();
    }

    void drawPrimitive(PrimitiveElement element, const QStyleOption *opt, QPainter *p, const QWidget *widget = 0) const
    {
        switch(element)
        {
        case PE_FrameMenu:
            if(const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame*>(opt))
            {
                if(!_menuFrame)
                    _menuFrame = new QPixmap(_night ? ":/style-mini/menu-night-frame.png"
                                                    : ":/style-mini/menu-frame.png");

                if(!_menuFrame)
                    break;

                const int n = QApplication::topLevelWidgets()[0]->font().pixelSize();
                const int m = _menuFrame->width() / 3;
                //qDrawBorderPixmap(p, frame->rect, QMargins(m, m, m, m), *_menuFrame);
                //p->drawPixmap(0, 0, _menuFrame->width(), _menuFrame->height(), *_menuFrame);

                p->setBrush(_palette.base());
                p->setPen(Qt::NoPen);
                p->drawRect(frame->rect.adjusted(n, n, -n, -n));

                p->drawPixmap(0, 0, n, n, *_menuFrame, 0, 0, m , m);
                p->drawPixmap(n, 0, frame->rect.width() - n - n, n, *_menuFrame, m, 0, m, m);
                p->drawPixmap(frame->rect.width() - n, 0, n, n, *_menuFrame, _menuFrame->width() - m, 0, m , m);
                p->drawPixmap(0, n, n, frame->rect.height() - n - n, *_menuFrame, 0, m, m, m);
                p->drawPixmap(0, frame->rect.height() - n, n, n, *_menuFrame, 0, _menuFrame->height() - m, m , m);
                p->drawPixmap(n, frame->rect.height() - n, frame->rect.width() - n - n, n, *_menuFrame, m,
                              _menuFrame->height() - m, m, m);
                p->drawPixmap(frame->rect.width() - n, frame->rect.height() - n, n, n, *_menuFrame,
                              _menuFrame->width() - m, _menuFrame->height() - m, m , m);
                p->drawPixmap(frame->rect.width() - n, n, n, frame->rect.height() - n - n, *_menuFrame,
                              _menuFrame->width() - m, m, m, m);
            }
            return;
        case PE_PanelButtonCommand:
            if(opt->state & QStyle::State_Sunken)
            {
                p->setPen(Qt::NoPen);
                p->setBrush(_palette.highlight());
                p->drawRect(opt->rect);
            }
            return;
        case PE_FrameFocusRect:
        case PE_PanelMenu:
        case PE_Frame:
            if(qobject_cast<const QLineEdit*>(widget))
            {
                QPen n(_palette.highlight().color(), qCeil(widget->font().pixelSize() / 9.0));
                n.setJoinStyle(Qt::MiterJoin);
                p->setPen(n);
                p->drawPolyline(QVector<QPoint>() << QPoint(opt->rect.x(), opt->rect.y() + (opt->rect.height() * 0.75))
                                << opt->rect.bottomLeft() - QPoint(0, (opt->rect.height() * 0.15))
                                << opt->rect.bottomRight() - QPoint(0, (opt->rect.height() * 0.15))
                                << QPoint(opt->rect.right(), opt->rect.y() + (opt->rect.height() * 0.75)));
            }
            return;
        case PE_CustomBase + 1:
            {
                const int h = _night ? opt->fontMetrics.height() : opt->fontMetrics.height() * 0.66 ;
                const int m = _night ? 200 : 100;

                QRect r(opt->rect.adjusted(0, 0, 0, h - opt->rect.height()));

                QLinearGradient g(r.topLeft(), r.bottomLeft());

                g.setColorAt(0.0, QColor(0, 0, 0, m));
                g.setColorAt(1.0, QColor(0, 0, 0, 0));

                p->setPen(Qt::NoPen);
                p->setBrush(g);
                p->drawRect(r);
            }
            return;
        }
        QCommonStyle::drawPrimitive(element, opt, p, widget);
    }

    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p,
        const QWidget *widget = 0) const
    {
        switch(element)
        {
        case CE_ScrollBarSlider:
            {
                const int s = opt->rect.width() / (_night ? 3.6 : 3);
                p->setBrush(_night ? QColor(0, 0, 0) : QColor(255, 255, 255));
                p->drawRect(opt->rect.adjusted(s, (opt->rect.width() - s - s) / 2, -s, (opt->rect.width() - s - s) / -2));
                p->drawEllipse(opt->rect.adjusted(s, 0, -s, (opt->rect.width() - s * 2) - opt->rect.height()));
                p->drawEllipse(opt->rect.adjusted(s, opt->rect.height() - (opt->rect.width() - s * 2), -s, 0));
                return;
            }
        case CE_PushButton:
            _elideEnabled = true;
            QCommonStyle::drawControl(element, opt, p, widget);
            _elideEnabled = false;
            return;
        }
        QCommonStyle::drawControl(element, opt, p, widget);
    }

    void drawItemText(QPainter *painter, const QRect &rectangle, int alignment, const QPalette &palette, bool enabled, const QString &text, QPalette::ColorRole textRole) const
    {
        Q_UNUSED(enabled);

        if(text.isEmpty())
            return;
        QPen savedPen;
        if(textRole != QPalette::NoRole)
        {
            savedPen = painter->pen();
            painter->setPen(QPen(palette.brush(textRole), savedPen.widthF()));
        }
//        if(!enabled)
//        {
//            if(proxy()->styleHint(SH_DitherDisabledText))
//            {
//                QRect br;
//                painter->drawText(rect, alignment, text, &br);
//                painter->fillRect(br, QBrush(painter->background().color(), Qt::Dense5Pattern));
//                return;
//            } else if (proxy()->styleHint(SH_EtchDisabledText))
//            {
//                QPen pen = painter->pen();
//                painter->setPen(pal.light().color());
//                painter->drawText(rect.adjusted(1, 1, 1, 1), alignment, text);
//                painter->setPen(pen);
//            }
//        }
//        if(fm.width(text) > rectangle.width())
//        {
//            QChar e(2026);
//            int ew = fm.width(e);
//            for(int c = 0;; ++c)
//            {
//                int w = fm.width(text.left(c));
//                if(w <= rectangle.width() && w + ew >= rectangle.width())
//                {
//                    painter->drawText(rectangle, alignment, text.left(c) + e);
//                    return;
//                }
//            }
////            painter->drawText(rectangle, alignment, text.left(c) + e);
////            alignment = (alignment | Qt::AlignHorizontal_Mask) ^ (Qt::AlignHorizontal_Mask ^ Qt::AlignRight);
//        }
//        else

//        QTextLayout textLayout(text);
//        textLayout.setFont(painter->font());
//        int widthUsed = 0;
//        int height = 0;
//        int lineCount = 0;
//        textLayout.beginLayout();

//        while (++lineCount < 10) {
//            QTextLine line = textLayout.createLine();
//            if (!line.isValid())
//                break;
//            line.setLineWidth(rectangle.width());
//            line.setPosition(QPointF(0, height));
//            height += line.height();
//            if(height > rectangle.height())
//                break;
//            widthUsed += line.naturalTextWidth();
//            qDebug() << widthUsed << line.position();
//        }
//        textLayout.endLayout();

//        //widthUsed += rectangle.width();
//        qDebug() << text << rectangle << widthUsed;

//        painter->drawText(rectangle, alignment, painter->fontMetrics().elidedText(text, Qt::ElideRight, widthUsed));


        // do not work with multiline text
        if(_elideEnabled && !text.contains('\n'))
            painter->drawText(rectangle, alignment, QFontMetrics(painter->font()).elidedText(text, Qt::ElideRight, rectangle.width()));
        else
            painter->drawText(rectangle, alignment, text);
        if (textRole != QPalette::NoRole)
            painter->setPen(savedPen);
    }

    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *option,
        QPainter *p, const QWidget *widget = 0 ) const
    {
        switch(cc)
        {
        case CC_ScrollBar:
            {
                if(const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(option))
                {
                    p->setPen(Qt::NoPen);
                    //p->setBrush(_palette.color(QPalette::Link)); // "#2c6799"
                    p->setBrush(_palette.highlight().color()); // "#2c6799"
                    p->drawRect(scrollbar->rect);

                    if(scrollbar->subControls & SC_ScrollBarSlider)
                    {
                        QStyleOptionSlider to = *scrollbar;

                        to.rect = subControlRect(cc, &to, SC_ScrollBarSlider, widget);
                        if(to.rect.isValid())
                        {
                            if(!(scrollbar->activeSubControls& SC_ScrollBarSlider))
                                to.state &= ~(State_Sunken | State_MouseOver);

                            drawControl(CE_ScrollBarSlider, &to, p, widget);
                        }
                    }

                    QCommonStyle::drawComplexControl(cc, option, p, widget);

                    // top shadow
                    if(widget && widget->parentWidget() && widget->parentWidget()->parentWidget() &&
                       widget->parentWidget()->parentWidget()->property("topShadow").toBool())
                        drawPrimitive((PrimitiveElement)(PE_CustomBase + 1), option, p, widget);
                }
            }
            return;
        }

        QCommonStyle::drawComplexControl(cc, option, p, widget);
    }

    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
    {
        switch(metric)
        {
        case PM_LayoutLeftMargin:
        case PM_LayoutTopMargin:
        case PM_LayoutRightMargin:
        case PM_LayoutBottomMargin:
        case PM_LayoutVerticalSpacing:
        case PM_ButtonShiftHorizontal:
        case PM_ButtonShiftVertical:
            return 0;
        case PM_LayoutHorizontalSpacing:
            if(widget && !qstrcmp(widget->metaObject()->className(), "BtMiniPanel"))
                return widget->font().pixelSize() / 3;
            return 0;
        case PM_DefaultFrameWidth:
            if(!qobject_cast<const QLineEdit*>(widget))
                return 0;
            break;
        //case PM_ButtonMargin:
        //    return 50;
        case PM_MenuPanelWidth:
                return QApplication::topLevelWidgets()[0]->font().pixelSize();
        case PM_MaximumDragDistance:
            return -1;
        case PM_SliderThickness:
        case PM_ScrollBarExtent:
            {
                static int v = -2;

                if(v == -2)
                {
                    const QWidget *w = widget;
                    while(w->parentWidget())
                        w = w->parentWidget();
                    v = w->font().pixelSize() * 1.9;
                }

                return v;
            }
        }
        return QCommonStyle::pixelMetric(metric, option, widget);
    }

    QPalette standardPalette() const
    {
        return _palette;
    }


	void polish(QWidget *widget)
    {
        // palettes
        widget->setPalette(widget->parentWidget() == 0 ? _palette : widget->parentWidget()->palette());

        if(QLineEdit *le = qobject_cast<QLineEdit*>(widget))
        {
            const int cm = le->font().pixelSize() / 7.0;
            le->setContentsMargins(cm, cm, cm, cm);
        }

        if(QString(widget->metaObject()->className()) == "BtMiniMenu")
        {
            QMargins m = widget->contentsMargins();
            const int n = QApplication::topLevelWidgets()[0]->font().pixelSize();
            widget->setContentsMargins(m.left() + n, m.top() + n, m.right() + n, m.bottom() + n);
            widget->setAttribute(Qt::WA_TranslucentBackground);
        }

        if(QString(widget->metaObject()->className()) == "BtMiniView")
        {
            if(QAbstractItemView *l = qobject_cast<QAbstractItemView*>(widget))
            {
                l->viewport()->setAutoFillBackground(false);
                l->setAutoFillBackground(false);
            }
        }

        // bottom panel widget
        if(QString(widget->metaObject()->className()) == "BtMiniPanel")
        {
            //if(widget->parentWidget()->layout()->indexOf(widget) > 0)
            //{
                //widget->setAutoFillBackground(true);

                //QPalette p = widget->palette();
                //if(_night)
                //{
                //    p.setColor(QPalette::Window, QColor(0, 0, 0));
                //    p.setColor(QPalette::Button, QColor(0, 0, 0));
                //}
                //else
                //{
                //    p.setColor(QPalette::Window, QColor::fromHsl(0, 0, 128));
                //    p.setColor(QPalette::ButtonText, QColor::fromHsl(0, 0, 255));
                //}

                //foreach(QWidget *w, widget->findChildren<QWidget*>() << widget)
                //    w->setPalette(p);
            //}

            widget->setMinimumHeight(baseSize() * 2.6);
        }

#ifdef Q_WS_WINCE
        // FIX on wince some why font size is not correct if not set explicitly
        {
            QFont f(widget->font());
            f.setPixelSize(f.pixelSize());
            widget->setFont(f);
        }
#endif
    }

    void unpolish(QWidget *widget)
    {
        widget->setPalette(QPalette());

        if(QLineEdit *le = qobject_cast<QLineEdit*>(widget))
            le->setContentsMargins(QMargins());
    }

    int styleHint(StyleHint sh, const QStyleOption *option, const QWidget *widget,
        QStyleHintReturn *hret) const
    {
        switch(sh)
        {
        case SH_ScrollBar_ContextMenu:
            return false;
        //case SH_ScrollBar_Transient:
        //    return true;
        case SH_ScrollBar_LeftClickAbsolutePosition:
            return true;
        }

        return QCommonStyle::styleHint(sh, option, widget, hret);
    }

    SubControl hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *option,
        const QPoint &point, const QWidget *widget) const
    {
        switch(cc)
        {
        case CT_ScrollBar:
            return SC_ScrollBarGroove;
        }

        return QCommonStyle::hitTestComplexControl(cc, option, point, widget);
    }

    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget = 0) const
    {
        switch(element)
        {
        case SE_ProgressBarContents:
        case SE_ProgressBarGroove:
        case SE_ProgressBarLabel:
            return option->rect;
        case SE_ShapedFrameContents:
        case SE_FrameLayoutItem:
            if(!qobject_cast<const QLineEdit*>(widget))
                return option->rect;
            break;
        }

        return QCommonStyle::subElementRect(element, option, widget);
    }

    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *option,
        SubControl sc, const QWidget *widget) const
    {
        switch(cc)
        {
        case CC_ScrollBar:
            {
                if(const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(option))
                {
                    switch(sc)
                    {
                    case SC_ScrollBarSlider:
                        if(scrollbar->orientation == Qt::Vertical)
                        {
                            const float vh = scrollbar->rect.height();
                            const float hh = scrollbar->maximum - scrollbar->minimum;
                            
                            // limit slider height
                            int sh = (int)(vh * (vh / (hh + vh)));
                            sh = qMax(scrollbar->rect.width(), sh);
                            //sh = qMin(scrollbar->rect.height() / 3 * 2, sh);

                            if(_night)
                                sh += (scrollbar->rect.height() - sh) * 0.33;
                            
                            const float vv = scrollbar->sliderPosition;
                            const float sp = qRound((vh - sh) * (vv / hh));

                            if(sh >= scrollbar->rect.height())
                                return QRect();

                            return scrollbar->rect.adjusted(0, sp,
                                    0, sp + sh - scrollbar->rect.height());
                        }
                    case SC_ScrollBarGroove:
                        return scrollbar->rect;
                    case SC_ScrollBarAddPage:
                    case SC_ScrollBarSubPage:
                        {
                            QRect slider = subControlRect(CC_ScrollBar, option, SC_ScrollBarSlider, widget);
                            if(sc == SC_ScrollBarSubPage)
                                return scrollbar->rect.adjusted(0, 0, 0,
                                    -scrollbar->rect.height() + slider.top());
                            else
                                return scrollbar->rect.adjusted(0, slider.bottom(), 0, 0);
                        }
                    default:
                        return QRect(scrollbar->rect.topLeft(), QSize());
                    }
                }
            }
            break;
        }

        return QCommonStyle::subControlRect(cc, option, sc, widget);
    }

    QSize sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &csz, const QWidget *widget) const
    {
        switch(ct)
        {
        case CT_PushButton:
            {
                QSize s = QCommonStyle::sizeFromContents(ct, opt, csz, widget);
                return QSize(s.width() * 1.5, s.height() * 2);
            }
        }
        return QCommonStyle::sizeFromContents(ct, opt, csz, widget);
    }

#if QT_VERSION < 0x050000
public slots:
    QIcon standardIconImplementation(StandardPixmap icon, const QStyleOption *option = 0, const QWidget *widget = 0) const
#else
	QIcon standardIcon(StandardPixmap icon, const QStyleOption *option = 0, const QWidget *widget = 0) const
#endif
    {
        switch(icon)
        {
        case SP_ArrowLeft:
            {
                QIcon i(":/style-mini/arrow-left.svg");
                i.addFile(":/style-mini/arrow-left-inactive.svg", QSize(), QIcon::Disabled);
                return i;
            }
        case SP_ArrowRight:
            {
                QIcon i(":/style-mini/arrow-right.svg");
                i.addFile(":/style-mini/arrow-right-inactive.svg", QSize(), QIcon::Disabled);
                return i;
            }
        default:
            return standardIcon(icon, option, widget);
        }
    }

private:
    Q_DISABLE_COPY(BtMiniStyle)

    mutable bool     _elideEnabled;
    mutable QPixmap *_menuFrame;
    bool             _night;
    QPalette         _palette;
};


class BtMiniStylePlugin : public QStylePlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface")
#endif

public:
    BtMiniStylePlugin() {;}
    ~BtMiniStylePlugin() {;}

    QStyle * create(const QString &key)
    {
        if (key.toLower() == "mini")
            return new BtMiniStyle();
        if (key.toLower() == "mini-night")
            return new BtMiniStyle(true);
        return 0;
    }

    QStringList	keys() const
    {
        return QStringList() << "mini" << "mini-night";
    }
};

#endif
