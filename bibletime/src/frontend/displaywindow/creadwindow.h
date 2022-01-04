/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef CREADWINDOW_H
#define CREADWINDOW_H

#include "cdisplaywindow.h"

#include "../display/cdisplay.h"
#include "../display/creaddisplay.h"


class BtActionCollection;
class QResizeEvent;

/** \brief The base class for all read-only display windows. */
class CReadWindow: public CDisplayWindow {

    Q_OBJECT

public: /* Methods: */

    CReadWindow(QList<CSwordModuleInfo *> modules, CMDIArea * parent);

    virtual CSwordModuleInfo::ModuleType moduleType() const = 0;

    int getSelectedColumn() const;

    int getFirstSelectedIndex() const;

    int getLastSelectedIndex() const;

    CSwordKey* getMouseClickedKey() const;

protected: /* Methods: */

    void setDisplayWidget(CDisplay * newDisplay) override;

    void resizeEvent(QResizeEvent * e) override;

    void pageDown() override;

    void pageUp() override;

    void copySelectedText() override;

    void copyByReferences() override;

    bool hasSelectedText() override;

protected slots:

    void lookupSwordKey(CSwordKey *) override;

    /**
      Catches the signal when the KHTMLPart has finished the layout (anchors are
      not ready before that).
    */
    virtual void slotMoveToAnchor();

    /** Updates the status of the popup menu entries. */
    virtual void copyDisplayedText();

    /** Opens the search dialog with the strong info of the last clicked word.*/
    void openSearchStrongsDialog();

    void colorThemeChangedSlot();

private: /* Fields: */

    CReadDisplay * m_readDisplayWidget;

};

#endif /* CREADWINDOW_H */
