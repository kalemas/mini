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
#include "btqmlinterface.h"

#include <QApplication>
#include <QClipboard>
#include <QQuickItem>
#include <QScreen>
#include <QTimer>
#include "../../../backend/config/btconfig.h"
#include "../../../backend/drivers/cswordbookmoduleinfo.h"
#include "../../../backend/drivers/cswordlexiconmoduleinfo.h"
#include "../../../backend/drivers/cswordmoduleinfo.h"
#include "../../../backend/keys/cswordkey.h"
#include "../../../backend/managers/colormanager.h"
#include "../../../backend/managers/cdisplaytemplatemgr.h"
#include "../../../backend/managers/cswordbackend.h"
#include "../../../backend/models/btmoduletextmodel.h"
#include "../../../backend/rendering/btinforendering.h"
#include "../../../backend/rendering/chtmlexportrendering.h"
#include "../../../backend/rendering/cplaintextexportrendering.h"
#include "../../../backend/rendering/btinforendering.h"
#include "../../../util/btconnect.h"
#include "../../bibletime.h"
#include "../../cinfodisplay.h"
#include "../../edittextwizard/btedittextwizard.h"


BtQmlInterface::BtQmlInterface(QObject* parent)
    : QObject(parent),
      m_firstHref(false),
      m_linkTimer(new QTimer(this)),
      m_moduleTextModel(new BtModuleTextModel(this)),
      m_swordKey(nullptr),
      m_backgroundHighlightColorIndex(-1),
      m_caseSensitive(false) {

    m_moduleTextModel->setTextFilter(&m_textFilter);
    m_textFilter.setShowReferences(true);
    m_linkTimer->setSingleShot(true);
    m_findState.enabled = false;

    BT_CONNECT(m_linkTimer, SIGNAL(timeout()), this, SLOT(timeoutEvent()));
}

BtQmlInterface::~BtQmlInterface() {
}

static bool moduleIsBook(const CSwordModuleInfo* module) {
    CSwordModuleInfo::ModuleType moduleType = module->type();
    if (moduleType == CSwordModuleInfo::GenericBook)
        return true;
    return false;
}

static bool moduleIsLexicon(const CSwordModuleInfo* module) {
    CSwordModuleInfo::ModuleType moduleType = module->type();
    if (moduleType == CSwordModuleInfo::Lexicon)
        return true;
    return false;
}

static bool moduleIsBibleOrCommentary(const CSwordModuleInfo* module) {
    CSwordModuleInfo::ModuleType moduleType = module->type();
    if (moduleType == CSwordModuleInfo::Bible ||
            moduleType == CSwordModuleInfo::Commentary)
        return true;
    return false;
}

bool BtQmlInterface::isBibleOrCommentary() {
    return moduleIsBibleOrCommentary(module());
}

void BtQmlInterface::setFilterOptions(FilterOptions filterOptions) {
    m_moduleTextModel->setFilterOptions(filterOptions);
}

int BtQmlInterface::getContextMenuIndex() const {
    return m_contextMenuIndex;
}

void BtQmlInterface::setContextMenuIndex(int index) {
    m_contextMenuIndex = index;
    emit contextMenuIndexChanged();
}

int BtQmlInterface::getContextMenuColumn() const {
    return m_contextMenuColumn;
}

void BtQmlInterface::setContextMenuColumn(int index) {
    m_contextMenuColumn = index;
    emit contextMenuColumnChanged();
}

QString BtQmlInterface::getActiveLink() const {
    return m_activeLink;
}

void BtQmlInterface::setActiveLink(const QString& link) {
    m_activeLink = link;
    emit activeLinkChanged();
}

QColor BtQmlInterface::getBackgroundColor() const {
    return ColorManager::instance().getBackgroundColor();
}

QColor BtQmlInterface::getBackgroundHighlightColor() const {
    return ColorManager::instance().getBackgroundHighlightColor();
}

int BtQmlInterface::getBackgroundHighlightColorIndex() const {
    return m_backgroundHighlightColorIndex;
}

QColor BtQmlInterface::getForegroundColor() const {
    return ColorManager::instance().getForegroundColor();
}

int BtQmlInterface::getCurrentModelIndex() const {
    if (m_swordKey == nullptr)
        return 0;
    if (moduleIsBibleOrCommentary(module())) {
        CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(m_swordKey);
        int index = m_moduleTextModel->verseKeyToIndex(*verseKey);
        return index;
    }
    else if (moduleIsBook(module())) {
        const CSwordBookModuleInfo *m = qobject_cast<const CSwordBookModuleInfo*>(module());
        CSwordTreeKey key(m->tree(), m);
        QString keyName = m_swordKey->key();
        key.setKey(keyName);
        CSwordTreeKey p(key);
        p.root();
        if(p != key)
            return static_cast<int>(key.getIndex())/4;
    }
    else if (moduleIsLexicon(module())){
        const CSwordLexiconModuleInfo *li =
                qobject_cast<const CSwordLexiconModuleInfo*>(m_swordKey->module());
        int index = li->entries().indexOf(m_swordKey->key());
        return index;
    }
    return 0;
}

QStringList BtQmlInterface::getModuleNames() const {
    return m_moduleNames;
}

int BtQmlInterface::getNumModules() const {
    return m_moduleNames.count();
}

bool BtQmlInterface::getPageDown() const {
    return true;
}

bool BtQmlInterface::getPageUp() const {
    return true;
}

double BtQmlInterface::getPixelsPerMM() const {
    QScreen* screen = QGuiApplication::screens().at(0);
    double millimeterPerInch = 25.4;
    return screen->physicalDotsPerInchX() / millimeterPerInch;
}

QString BtQmlInterface::stripHtml(const QString& html) {
    QString t = html;
    //since t is a complete HTML page at the moment, strip away headers and footers of a HTML page
    QRegExp re("(?:<html.*>.+<body.*>)", Qt::CaseInsensitive); //remove headers, case insensitive
    re.setMinimal(true);
    t.replace(re, "");
    t.replace(QRegExp("</body></html>", Qt::CaseInsensitive), "");//remove footer
    return t;
}

QString BtQmlInterface::getRawText(int row, int column) {
    BT_ASSERT(column >= 0 && column <= m_moduleNames.count());
    CSwordVerseKey key = m_moduleTextModel->indexToVerseKey(row);
    QString moduleName = m_moduleNames.at(column);
    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    CSwordVerseKey mKey(module);
    mKey.setKey(key.key());
    QString rawText = mKey.rawText();
    return stripHtml(rawText);
}

void BtQmlInterface::openEditor(int row, int column) {
    BtEditTextWizard wiz;
    QString keyName = m_moduleTextModel->indexToKeyName(row);
    wiz.setTitle(tr("Edit") + " " + keyName);
    wiz.setText(getRawText(row,column));
    QFont font = m_fonts.at(column);
    wiz.setFont(font);
    int rtn = wiz.exec();
    if (rtn == 0)
        return;
    setRawText(row, column, wiz.text());
    return;
}

int BtQmlInterface::indexToVerse(int index) {
    return m_moduleTextModel->indexToVerse(index);
}

void BtQmlInterface::openContextMenu(int x, int y, int width) {
    double xRatio = x / static_cast<double>(width);
    int column = int(xRatio * m_moduleNames.count());
    if (column < 0 || column >= m_moduleNames.count())
        return;
    emit contextMenu(x, y, column);
}

QString BtQmlInterface::getLemmaFromLink(const QString& url) {
    QString reference;

    QRegExp rx("sword://lemmamorph/([a-s]+)=([GH][0-9]+)");
    rx.setMinimal(false);
    int pos1 = rx.indexIn(url);
    if (pos1 > -1) {
        reference = rx.cap(2);
    }
    return reference;
}

QString BtQmlInterface::getBibleUrlFromLink(const QString& url) {
    QString reference;

    QRegExp rx("(sword://Bible/.*)\\|\\|(.*)=(.*)");
    rx.setMinimal(false);
    int pos1 = rx.indexIn(url);
    if (pos1 > -1) {
        reference = rx.cap(1);
    }
    return reference;
}

QString BtQmlInterface::getReferenceFromUrl(const QString& url) {
    QString reference;


    QRegExp rx("sword://(bible|lexicon)/(.*)/(.*)(\\|\\|)", Qt::CaseInsensitive);
    rx.setMinimal(false);
    int pos1 = rx.indexIn(url);
    if (pos1 > -1) {
        reference = "href=sword://" + rx.cap(1) + "/" + rx.cap(2) + "/" + rx.cap(3);
    } else {
        QRegExp rx0("sword://(bible|lexicon)/(.*)/(.*)", Qt::CaseInsensitive);
        rx0.setMinimal(false);
        int pos1 = rx0.indexIn(url);
        if (pos1 > -1) {
            reference = "href=sword://" + rx0.cap(1) + "/" + rx0.cap(2) + "/" + rx0.cap(3);

        } else {
            QRegExp rx1("sword://footnote/(.*)=(.*)", Qt::CaseInsensitive);
            rx1.setMinimal(false);
            int pos1 = rx1.indexIn(url);
            if (pos1 > -1) {
                reference = "note=" + rx1.cap(1);

            } else {
                QRegExp rx2("sword://lemmamorph/(.*)=(.*)/(.*)", Qt::CaseInsensitive);
                rx2.setMinimal(false);
                int pos1 = rx2.indexIn(url);
                if (pos1 > -1) {
                    reference = rx2.cap(1) + "=" + rx2.cap(2);
                }
            }
        }
    }
    return reference;
}

void BtQmlInterface::setRawText(int row, int column, const QString& text) {
    QModelIndex index = m_moduleTextModel->index(row, 0);
    int role = ModuleEntry::Text0Role + column;
    m_moduleTextModel->setData(index, text, role);
}

void BtQmlInterface::cancelMagTimer() {
    m_linkTimer->stop();
}

void BtQmlInterface::setMagReferenceByUrl(const QString& url) {
    if (url.isEmpty())
        return;
    m_timeoutUrl = url;
    m_linkTimer->start(400);
}

void BtQmlInterface::timeoutEvent() {
    QString link = getReferenceFromUrl(m_timeoutUrl);
    Rendering::ListInfoData infoList(Rendering::detectInfo(link));
    if (!(infoList.isEmpty()))
        BibleTime::instance()->infoDisplay()->setInfo(infoList);
}

void BtQmlInterface::settingsChanged() {
    getFontsFromSettings();
    changeColorTheme();
    emit textChanged();
}

void BtQmlInterface::pageDown() {
    emit pageDownChanged();
}

void BtQmlInterface::pageUp() {
    emit pageUpChanged();
}

void BtQmlInterface::getFontsFromSettings() {
    m_fonts.clear();
    for(int i=0; i<m_moduleNames.count(); ++i) {
        QString moduleName = m_moduleNames.at(i);
        QFont font;
        CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(moduleName);
        if (m != nullptr) {

            const CLanguageMgr::Language* lang = m->language();
            if (lang != nullptr) {

                BtConfig::FontSettingsPair fontPair = btConfig().getFontForLanguage(*lang);
                if (fontPair.first) {
                    font = fontPair.second;
                } else {
                    font = btConfig().getDefaultFont();
                }
            }
        }
        m_fonts.append(font);
        emit fontChanged();
    }
}

/** Sets the new sword key. */
void BtQmlInterface::setKey( CSwordKey* key ) {
    BT_ASSERT(key);
    m_swordKey = key;
}

void BtQmlInterface::setKeyFromLink(const QString& link) {
    QRegExp rx("sword://Bible/(.*)/(.*)\\|\\|(.*)=(.*)");
    rx.setMinimal(false);
    int pos1 = rx.indexIn(link);
    QString keyName;
    if (pos1 > -1) {
        keyName = rx.cap(2);
        Q_EMIT newBibleReference(keyName);
    }
}

void BtQmlInterface::scrollToSwordKey(CSwordKey * key) {
    m_backgroundHighlightColorIndex = m_moduleTextModel->keyToIndex(key);

    /* Convert from sword index to ListView index */
    m_backgroundHighlightColorIndex = m_backgroundHighlightColorIndex - m_moduleTextModel->getFirstEntryIndex();

    emit backgroundHighlightColorIndexChanged();
    m_swordKey = key;
    emit currentModelIndexChanged();
}

void BtQmlInterface::setModules(const QStringList &modules) {
    m_moduleNames = modules;
    m_moduleTextModel->setModules(modules);
    getFontsFromSettings();
    emit numModulesChanged();
}

void BtQmlInterface::referenceChosen() {
    emit referenceChange();
    emit currentModelIndexChanged();
}

void BtQmlInterface::changeReference(int i) {
    QString reference = m_moduleTextModel->indexToKeyName(i);
    emit updateReference(reference);
}

void BtQmlInterface::dragHandler(int index, const QString& activeLink) {
    QString moduleName;
    QString keyName;

    QRegExp rx("sword://Bible/(.*)/(.*)\\|\\|(.*)=(.*)");
    rx.setMinimal(false);
    int pos1 = rx.indexIn(activeLink);

    if (pos1 > -1) {
        moduleName = rx.cap(1);
        keyName = rx.cap(2);
    } else {
        moduleName = m_moduleNames.at(0);
        keyName = m_moduleTextModel->indexToKeyName(index);
    }

    emit dragOccuring(moduleName, keyName);
}

const CSwordModuleInfo* BtQmlInterface::module() const {
    if (!m_swordKey)
        return nullptr;
    const CSwordModuleInfo* module = m_swordKey->module();
    return module;
}

CSwordKey* BtQmlInterface::getKey() const {
    return m_swordKey;
}

CSwordKey* BtQmlInterface::getMouseClickedKey() const {
    return m_moduleTextModel->indexToKey(m_contextMenuIndex, m_contextMenuColumn);
}

QFont BtQmlInterface::font(int column) const {
    if (column >= 0 && column < m_fonts.count())
        return m_fonts.at(column);
    return QApplication::font();
}

QFont BtQmlInterface::getFont0() const { return font(0); }
QFont BtQmlInterface::getFont1() const { return font(1); }
QFont BtQmlInterface::getFont2() const { return font(2); }
QFont BtQmlInterface::getFont3() const { return font(3); }
QFont BtQmlInterface::getFont4() const { return font(4); }
QFont BtQmlInterface::getFont5() const { return font(5); }
QFont BtQmlInterface::getFont6() const { return font(6); }
QFont BtQmlInterface::getFont7() const { return font(7); }
QFont BtQmlInterface::getFont8() const { return font(8); }
QFont BtQmlInterface::getFont9() const { return font(9); }

QString BtQmlInterface::getSelectedText() {

    QString text;
    QMap<int, QString>::const_iterator i = m_selectedText.constBegin();
    while (i != m_selectedText.constEnd()) {
        text.append(i.value());
        text.append('\n');
        ++i;
    }
    return text;
}

QVariant BtQmlInterface::getTextModel() {
    QVariant var;
    var.setValue(m_moduleTextModel);
    return var;
}

BtModuleTextModel * BtQmlInterface::textModel() {
    return m_moduleTextModel;
}

bool BtQmlInterface::moduleIsWritable(int column) {
    if (column >= m_moduleNames.count())
        return false;
    QString moduleName = m_moduleNames.at(column);
    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    return module->isWritable();
}

void BtQmlInterface::configModuleByType(const QString& type, const QStringList& availableModuleNames) {
    CSwordModuleInfo* module = btConfig().getDefaultSwordModuleByType(type);
    if (!module && availableModuleNames.count() > 0) {
        QString moduleName = availableModuleNames.at(0);
        module = CSwordBackend::instance()->findModuleByName(moduleName);
        btConfig().setDefaultSwordModuleByType(type, module);
    }
}

RefIndexes BtQmlInterface::normalizeReferences(const QString& ref1, const QString& ref2) {
    RefIndexes ri;
    CSwordKey * key = m_swordKey->copy();
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

void BtQmlInterface::changeColorTheme() {
    emit backgroundHighlightColorChanged();
    emit backgroundColorChanged();
    emit foregroundColorChanged();
}

void BtQmlInterface::copyRange(int index1, int index2) {
    QString text;
    CSwordKey * key = m_swordKey->copy();

    for (int i=index1; i<=index2; ++i) {
        QString keyName = m_moduleTextModel->indexToKeyName(i);
        key->setKey(keyName);
        text += keyName + "\n" + key->strippedText() + "\n\n";
    }
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
    delete key;
}

void BtQmlInterface::copyVerseRange(const QString& ref1, const QString& ref2, const CSwordModuleInfo * module) {
    CSwordVerseKey dummy(module);
    CSwordVerseKey vk(module);
    dummy.setKey(ref1);
    vk.setLowerBound(dummy);
    dummy.setKey(ref2);
    vk.setUpperBound(dummy);
    copyKey(&vk, BtQmlInterface::Text, true);
}


bool BtQmlInterface::copyKey(CSwordKey const * const key,
                             Format const format,
                             bool const addText)
{
    if (!key || !key->module())
        return false;

    QString text;
    BtConstModuleList modules;
    modules.append(key->module());

    auto const render = newRenderer(format, addText);
    CSwordVerseKey const * const vk =
            dynamic_cast<CSwordVerseKey const *>(key);
    if (vk && vk->isBoundSet()) {
        text = render->renderKeyRange(
                    QString::fromUtf8(vk->getLowerBound()),
                    QString::fromUtf8(vk->getUpperBound()),
                    modules
                    );
    } else {
        text = render->renderSingleKey(key->key(), modules);
    }

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
    return true;
}

std::unique_ptr<Rendering::CTextRendering> BtQmlInterface::newRenderer(Format const format,
                                                                       bool const addText)
{
    DisplayOptions displayOptions;
    displayOptions.lineBreaks = true;
    displayOptions.verseNumbers = true;

    FilterOptions filterOptions;
    filterOptions.footnotes = 0;
    filterOptions.greekAccents = 1;
    filterOptions.headings = 1;
    filterOptions.hebrewCantillation = 1;
    filterOptions.hebrewPoints = 1;
    filterOptions.lemmas = 0;
    filterOptions.morphSegmentation = 1;
    filterOptions.morphTags = 0;
    filterOptions.redLetterWords = 1;
    filterOptions.scriptureReferences = 0;
    filterOptions.strongNumbers = 0;
    filterOptions.textualVariants = 0;

    using R = std::unique_ptr<Rendering::CTextRendering>;
    BT_ASSERT((format == Text) || (format == HTML));
    if (format == HTML)
        return R{new Rendering::CHTMLExportRendering(addText,
                                                     displayOptions,
                                                     filterOptions)};
    return R{new Rendering::CPlainTextExportRendering(addText,
                                                      displayOptions,
                                                      filterOptions)};
}

QString BtQmlInterface::getHighlightWords() const {
    return m_highlightWords;
}

void BtQmlInterface::setHighlightWords(const QString& words, bool caseSensitive) {
    m_highlightWords = words;
    m_caseSensitive = caseSensitive;
    QTimer::singleShot(900, this, &BtQmlInterface::slotSetHighlightWords);
}

void BtQmlInterface::slotSetHighlightWords() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_moduleTextModel->setHighlightWords(m_highlightWords, m_caseSensitive);
    m_findState.enabled = false;
    m_moduleTextModel->setFindState(m_findState);
    emit highlightWordsChanged();
    QApplication::restoreOverrideCursor();
}

void BtQmlInterface::findText(const QString& /*text*/,
                              bool /*caseSensitive*/, bool backward) {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (! m_findState.enabled) {  // initialize it
        m_findState.enabled = true;
        m_findState.index = getCurrentModelIndex();
        m_findState.subIndex = 0;
    }

    if (backward)
        getPreviousMatchingItem(m_findState.index);
    else
        getNextMatchingItem(m_findState.index);

    m_moduleTextModel->setFindState(m_findState);
    emit highlightWordsChanged();
    emit positionItemOnScreen(m_findState.index);
    QApplication::restoreOverrideCursor();
}

int BtQmlInterface::countHighlightsInItem(int index) {
    QModelIndex mIndex = m_moduleTextModel->index(index);
    QString text = m_moduleTextModel->data(mIndex, ModuleEntry::Text1Role).toString();
    int num = text.count("\"highlightwords");
    return num;
}

void BtQmlInterface::getNextMatchingItem(int startIndex) {
    int num = countHighlightsInItem(startIndex);
    if (num > m_findState.subIndex) { // Found within startIndex item
        m_findState.index = startIndex;
        ++m_findState.subIndex;
        return;
    }

    if (startIndex >= m_moduleTextModel->rowCount())
        return;

    int index = startIndex+1;
    for (int i = 0; i < 1000; ++i) {
        int num = countHighlightsInItem(index);
        if (num > 0 ) {
            m_findState.index = index;
            m_findState.subIndex = 1;
            return;
        }
        ++index;
    }
    return;
}

void BtQmlInterface::getPreviousMatchingItem(int startIndex) {
    int num = countHighlightsInItem(startIndex);
    if (num > 0 && m_findState.subIndex == 0) {
        // Found within startIndex item
        m_findState.index = startIndex;
        m_findState.subIndex = 1;
        return;
    }

    if (startIndex <= 0)
        return;

    int index = startIndex;
    if (m_findState.subIndex == 0)
        --index;
    for (int i = 0; i < 1000; ++i) {
        int num = countHighlightsInItem(index);
        if (num > 0 ) {
            m_findState.index = index;
            if (m_findState.subIndex == 0)
                m_findState.subIndex = num;
            else
                --m_findState.subIndex;
            if (m_findState.subIndex != 0)
                return;
        }
        --index;
    }
    return;
}

bool BtQmlInterface::hasSelectedText() {
    return ! m_selectedText.isEmpty();
}

void BtQmlInterface::clearSelectedText() {
    m_selectedText.clear();
}

void BtQmlInterface::saveSelectedText(int index, const QString& text) {
    m_selectedText.insert(index, text);
}

bool BtQmlInterface::shiftKeyDown() {
    return QGuiApplication::keyboardModifiers() & Qt::ShiftModifier;
}

void BtQmlInterface::setBoundsMovement() {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    QQuickItem* displayListView = parent()->findChild<QQuickItem*>("DisplayListView");
    if (displayListView != nullptr) {
        int StopAtBounds = 0;
        displayListView->setProperty("boundsMovement", StopAtBounds);
    }
#endif
}
