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

#include "btmodelviewreaddisplay.h"

#include <memory>
#include <QDebug>
#include <QDrag>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QMenu>
#include <QString>
#include <QTimer>
#include <QToolBar>
#include "../../backend/keys/cswordkey.h"
#include "../../backend/managers/referencemanager.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/directory.h"
#include "../bibletime.h"
#include "../BtMimeData.h"
#include "../cinfodisplay.h"
#include "../cmdiarea.h"
#include "../displaywindow/cdisplaywindow.h"
#include "../displaywindow/creadwindow.h"
#include "../keychooser/ckeychooser.h"
#include "modelview/btqmlscrollview.h"
#include "modelview/btqmlinterface.h"
#include "modelview/btquickwidget.h"


using namespace InfoDisplay;

BtModelViewReadDisplay::BtModelViewReadDisplay(CReadWindow* readWindow, QWidget* parentWidget)
    : QWidget(parentWidget), CReadDisplay(readWindow), m_magTimerId(0), m_widget(nullptr)

{
    setObjectName("BtModelViewReadDisplay");
    QHBoxLayout* layout = new QHBoxLayout(this);
    setLayout(layout);
    m_widget = new BtQmlScrollView(this, this);
    layout->addWidget(m_widget);
    m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    BT_CONNECT(m_widget->qmlInterface(), SIGNAL(updateReference(const QString&)),
               this, SLOT(slotUpdateReference(const QString&)));
    BT_CONNECT(m_widget->qmlInterface(), SIGNAL(dragOccuring(const QString&,const QString&)),
               this, SLOT(slotDragOccuring(const QString&, const QString&)));
    BT_CONNECT(m_widget, SIGNAL(referenceDropped(const QString&)),
               this, SLOT(slotReferenceDropped(const QString&)));
}

BtModelViewReadDisplay::~BtModelViewReadDisplay() {
}

void BtModelViewReadDisplay::reloadModules() {
    qmlInterface()->textModel()->reloadModules();
}

const QString BtModelViewReadDisplay::text( const CDisplay::TextType format,
                                            const CDisplay::TextPart part) {
    QString text;
    switch (part) {
    case Document: {
        if (format == HTMLText) {
            text = getCurrentSource();
        }
        else {
            CDisplayWindow* window = parentWindow();
            CSwordKey* const key = window->key();
            const CSwordModuleInfo *module = key->module();
            //This is never used for Bibles, so it is not implemented for
            //them.  If it should be, see CReadDisplay::print() for example
            //code.
            BT_ASSERT(module->type() == CSwordModuleInfo::Lexicon ||
                      module->type() == CSwordModuleInfo::Commentary ||
                      module->type() == CSwordModuleInfo::GenericBook);
            if (module->type() == CSwordModuleInfo::Lexicon ||
                    module->type() == CSwordModuleInfo::Commentary ||
                    module->type() == CSwordModuleInfo::GenericBook) {

                FilterOptions filterOptions;
                CSwordBackend::instance()->setFilterOptions(filterOptions);

                text = QString(key->strippedText()).append("\n(")
                        .append(key->key())
                        .append(", ")
                        .append(key->module()->name())
                        .append(")");
            }
        }
        break;
    }

    case AnchorOnly: {
        QString moduleName;
        QString keyName;
        ReferenceManager::Type type;
        ReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

        return keyName;
    }

    case AnchorTextOnly: {
        QString moduleName;
        QString keyName;
        ReferenceManager::Type type;
        ReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

        if (CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName(moduleName)) {
            std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(module));
            key->setKey(keyName);

            return key->strippedText();
        }
        return QString();
    }

    case AnchorWithText: {
        QString moduleName;
        QString keyName;
        ReferenceManager::Type type;
        ReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

        if (CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName(moduleName)) {
            std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(module));
            key->setKey(keyName);

            FilterOptions filterOptions;
            CSwordBackend::instance()->setFilterOptions(filterOptions);

            return QString(key->strippedText()).append("\n(")
                    .append(key->key())
                    .append(", ")
                    .append(key->module()->name())
                    .append(")");
            /*    ("%1\n(%2, %3)")
                    .arg()
                    .arg(key->key())
                    .arg(key->module()->name());*/
        }
        return QString();
    }
    default:
        break;
    }
    return QString();

}

// Puts html text the view
void BtModelViewReadDisplay::setText( const QString& /*newText*/ ) {
}

void BtModelViewReadDisplay::setDisplayFocus() {
    m_widget->quickWidget()->setFocus();
}

void BtModelViewReadDisplay::setDisplayOptions(const DisplayOptions &displayOptions) {
    m_widget->qmlInterface()->textModel()->setDisplayOptions(displayOptions);
}

void BtModelViewReadDisplay::contextMenu(QContextMenuEvent* event) {
    QString activeLink = m_widget->qmlInterface()->getActiveLink();
    QString reference = m_widget->qmlInterface()->getBibleUrlFromLink(activeLink);
    setActiveAnchor(reference);
    QString lemma = m_widget->qmlInterface()->getLemmaFromLink(activeLink);
    setLemma(lemma);

    if (QMenu* popup = installedPopup()) {
        popup->exec(event->globalPos());
    }
}

QString BtModelViewReadDisplay::getCurrentSource( ) {
    return this->currentSource;
}

BtQmlInterface * BtModelViewReadDisplay::qmlInterface() const {
    return m_widget->qmlInterface();
}

void BtModelViewReadDisplay::setModules(const QStringList &modules) {
    m_widget->qmlInterface()->setModules(modules);
}

void BtModelViewReadDisplay::scrollToKey(CSwordKey * key) {
    m_widget->scrollToSwordKey(key);
}

void BtModelViewReadDisplay::scroll(int value) {
    m_widget->quickWidget()->scroll(value);
}

void BtModelViewReadDisplay::setFilterOptions(FilterOptions filterOptions) {
    m_widget->setFilterOptions(filterOptions);
}

void BtModelViewReadDisplay::settingsChanged() {
    m_widget->settingsChanged();
}

void BtModelViewReadDisplay::updateReferenceText() {
    m_widget->quickWidget()->updateReferenceText();
}

void BtModelViewReadDisplay::pageDown() {
    m_widget->pageDown();
}

void BtModelViewReadDisplay::pageUp() {
    m_widget->pageUp();
}

// See if any text is selected
bool BtModelViewReadDisplay::hasSelection() const {
    return false;
}

void BtModelViewReadDisplay::highlightText(const QString& text, bool caseSensitive) {
    m_widget->qmlInterface()->setHighlightWords(text, caseSensitive);
}

void BtModelViewReadDisplay::findText(const QString& text,
                                      bool caseSensitive, bool backward) {
    m_widget->qmlInterface()->findText(text, caseSensitive, backward);
}

// Reimplementation
// Returns the BtModelViewReadDisplayView object
QWidget* BtModelViewReadDisplay::view() {
    return m_widget;
}

// Select all text in the viewer
void BtModelViewReadDisplay::selectAll() {
}

// Scroll to the correct location as specified by the anchor
void BtModelViewReadDisplay::moveToAnchor( const QString& /*anchor*/ ) {
}

void BtModelViewReadDisplay::slotDelayedMoveToAnchor() {
}

// Scroll the view to the correct location specified by anchor
void BtModelViewReadDisplay::slotGoToAnchor(const QString& /*anchor*/) {
}

void BtModelViewReadDisplay::slotUpdateReference(const QString& reference) {
    CDisplayWindow* window = parentWindow();
    auto key = window->key();
    key->setKey(reference);
    window->keyChooser()->updateKey(key);
    QString caption = window->windowCaption();
    window->setWindowTitle(window->windowCaption());
}

void BtModelViewReadDisplay::slotDragOccuring(const QString& moduleName, const QString& keyName) {
    QDrag* drag = new QDrag(this);
    BTMimeData* mimedata = new BTMimeData(moduleName, keyName, QString());
    drag->setMimeData(mimedata);
    //add real Bible text from module/key
    if (CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName(moduleName)) {
        CDisplayWindow* window = parentWindow();
        QToolBar * tb = window->mainToolBar();
        QSize size = tb->iconSize();
        QIcon icon = module->moduleIcon();
        drag->setPixmap(icon.pixmap(size));
        std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(module));
        key->setKey(keyName);
        mimedata->setText(key->strippedText()); // This works across applications!
    }
    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

void BtModelViewReadDisplay::slotReferenceDropped(const QString& reference) {  // TODO - Fix me
    CDisplayWindow* window = parentWindow();
    auto key = window->key();
    key->setKey(reference);
    window->lookupKey(reference);
}

// Save the Lemma (Strongs number) attribute
void BtModelViewReadDisplay::setLemma(const QString& lemma) {
    m_nodeInfo = lemma;
}

// Open the Find text dialog
void BtModelViewReadDisplay::openFindTextDialog() {
    BibleTime* bibleTime = parentWindow()->mdi()->bibleTimeWindow();
    bibleTime->openFindWidget();
}
