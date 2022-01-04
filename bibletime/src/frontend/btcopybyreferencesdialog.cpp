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

#include "btcopybyreferencesdialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "../backend/keys/cswordkey.h"
#include "../backend/models/btmoduletextmodel.h"
#include "../backend/drivers/cswordmoduleinfo.h"
#include "../frontend/display/btmodelviewreaddisplay.h"
#include "../frontend/displaywindow/creadwindow.h"
#include "../util/btconnect.h"
#include "display/modelview/btqmlinterface.h"
#include "keychooser/ckeychooser.h"
#include "messagedialog.h"


BtCopyByReferencesDialog::BtCopyByReferencesDialog(const BtConstModuleList & modules,
                                                   BTHistory * historyPtr,
                                                   CSwordKey * key,
                                                   BtModuleTextModel * model,
                                                   CReadWindow * parent)
        : QDialog(parent), m_modules(modules), m_key(key),
          m_keyChooser1(nullptr), m_keyChooser2(nullptr),
          m_moduleTextModel(model), m_buttons(nullptr),
          m_readWindow(parent) {

    setWindowTitle(tr("Copy by References"));
    setMinimumWidth(400);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    setLayout(vLayout);

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(15);
    gridLayout->setVerticalSpacing(15);
    gridLayout->setContentsMargins(11,11,11,16);
    vLayout->addLayout(gridLayout);

    QLabel* label1 = new QLabel(tr("First"));
    gridLayout->addWidget(label1, 0,0);

    m_keyChooser1 = CKeyChooser::createInstance(modules, historyPtr, key->copy(), this);
    gridLayout->addWidget(m_keyChooser1,0,1);

    QHBoxLayout* hLayout = new QHBoxLayout;
    vLayout->addLayout(hLayout);

    QLabel* label2 = new QLabel(tr("Last"));
    gridLayout->addWidget(label2, 1,0);

    m_keyChooser2 = CKeyChooser::createInstance(modules, historyPtr, key->copy(), this);
    gridLayout->addWidget(m_keyChooser2,1,1);

    m_moduleNameCombo = new QComboBox();
    gridLayout->addWidget(m_moduleNameCombo, 2,1);

    m_sizeToLarge = new QLabel(tr("Copy size is too large."));
    m_sizeToLarge->setVisible(false);
    hLayout->addWidget(m_sizeToLarge);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    message::prepareDialogBox(m_buttons);
    hLayout->addWidget(m_buttons);

    loadSelectionKeys();

    BT_CONNECT(m_keyChooser1, SIGNAL(keyChanged(CSwordKey *)),
               this, SLOT(slotKeyChanged(CSwordKey *)));

    BT_CONNECT(m_keyChooser2, SIGNAL(keyChanged(CSwordKey *)),
               this, SLOT(slotKeyChanged(CSwordKey *)));

    BT_CONNECT(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
    BT_CONNECT(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));

    slotKeyChanged(nullptr);
}

void BtCopyByReferencesDialog::slotKeyChanged(CSwordKey * /* newKey */) {

    QString ref1 = m_keyChooser1->key()->key();
    QString ref2 = m_keyChooser2->key()->key();

    bool toLarge = isCopyToLarge(ref1, ref2);
    m_sizeToLarge->setVisible(toLarge);
    m_buttons->button(QDialogButtonBox::Ok)->setEnabled(!toLarge);
}

bool BtCopyByReferencesDialog::isCopyToLarge(const QString& ref1, const QString& ref2) {
    m_ri = normalizeReferences(ref1, ref2);
    CSwordModuleInfo::ModuleType type = m_modules.at(0)->type();
    if (type == CSwordModuleInfo::Bible ||
            type == CSwordModuleInfo::Commentary) {
        if ((m_ri.index2-m_ri.index1) > 2700)
            return true;
    } else {
        if ( m_ri.index2-m_ri.index1 > 100)
            return true;
    }
    return false;
}

int BtCopyByReferencesDialog::getIndex1() {
    return m_ri.index1;
}

int BtCopyByReferencesDialog::getIndex2() {
    return m_ri.index2;
}

int BtCopyByReferencesDialog::getColumn() {
    return m_moduleNameCombo->currentIndex();
}

QString BtCopyByReferencesDialog::getReference1() {
    return m_ri.r1;
}

QString BtCopyByReferencesDialog::getReference2() {
    return m_ri.r2;
}

RefIndexes BtCopyByReferencesDialog::normalizeReferences(const QString& ref1, const QString& ref2) {
    RefIndexes ri;
    CSwordKey * key = m_key->copy();
    key->setKey(ref1);
    QString x1 = key->key();
    ri.index1 = m_moduleTextModel->keyToIndex(key);
    key->setKey(ref2);
    QString x2 = key->key();
    ri.index2 = m_moduleTextModel->keyToIndex(key);
    ri.r1 = ref1;
    ri.r2 = ref2;
    if (ri.index1 > ri.index2) {
        ri.r1.swap(ri.r2);
        std::swap(ri.index1, ri.index2);
    }
    return ri;
}

void BtCopyByReferencesDialog::loadSelectionKeys() {

    Q_FOREACH( auto m, m_modules ) {
        QString name = m->name();
        m_moduleNameCombo->addItem(name);
    }

    int column = m_readWindow->getSelectedColumn();
    if (column < 0)
        column = 0;

    int first = m_readWindow->getFirstSelectedIndex();
    int last  = m_readWindow->getLastSelectedIndex();
    if (first < 0 || last < 0)
        return; // defaults to top of view.

    CSwordKey* firstKey = m_moduleTextModel->indexToKey(first, 0);
    CSwordKey* lastKey = m_moduleTextModel->indexToKey(last, 0);
    m_keyChooser1->setKey(firstKey);
    m_keyChooser2->setKey(lastKey);

    const CSwordModuleInfo* m = m_modules.at(column);

    QString moduleName = m->name();
    int index = m_moduleNameCombo->findText(moduleName);
    if (index >= 0)
        m_moduleNameCombo->setCurrentIndex(index);
}


