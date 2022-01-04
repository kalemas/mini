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


#ifndef BTMINIWIDGET_H
#define BTMINIWIDGET_H


#include <QWidget>


class BtMiniWidget : public QWidget
{
public:
    BtMiniWidget(QWidget *parent=0) : QWidget(parent) {;}
    virtual ~BtMiniWidget() {;}

protected:
    QSize sizeHint() const
    {
        return QWidget::sizeHint().boundedTo(parentWidget()->size());
    }

    QSize minimumSizeHint() const
    {
        return QWidget::minimumSizeHint().boundedTo(parentWidget()->size());
    }

    /** use of this function solves a problem (?) on some (?) platform */
    static void changeFontSize(QWidget *w, qreal factor)
    {
        QFont f(w->font());
        f.setPixelSize(f.pixelSize() * factor);
        w->setFont(f);
    }


//        void resizeEvent(QResizeEvent *e)
//        {
//            //qDebug() << "Resize widget" << e->oldSize() << "to" << e->size() << "desktop";
//            QWidget::resizeEvent(e);
//        }
};

#endif // BTMINIWIDGET_H
