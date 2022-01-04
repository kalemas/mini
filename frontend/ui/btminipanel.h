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

#ifndef BTMINIPANEL_H
#define BTMINIPANEL_H

#include <QWidget>

class BtMiniPanelPrivate;

class BtMiniPanel : public QWidget
{
    Q_OBJECT

public:
    enum Activity
    {
        None = 0,
        Search,
        Installer,
        Exit,
        Options,
        Settings,
        Close
    };

    typedef QVector<Activity> Activities;

    BtMiniPanel(Activities activities, QWidget *parent=0);
    BtMiniPanel(QWidget *parent=0);
    ~BtMiniPanel();

    /** Setup panel in layout style. */
    void addWidget(QWidget *widget, Qt::Alignment anchor);

protected slots:
    /** Handle interaction with activities's controls. */
    void controlActivated();

protected:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *e);
    bool event(QEvent * e);

private:
    Q_DECLARE_PRIVATE(BtMiniPanel)
    BtMiniPanelPrivate * const d_ptr;

};

#endif
