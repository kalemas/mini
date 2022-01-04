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

#ifndef BT_DISPLAY_VIEW_INTERFACE_H
#define BT_DISPLAY_VIEW_INTERFACE_H

#include <memory>
#include <QFont>
#include <QList>
#include <QObject>
#include <QMap>
#include <QString>
#include <QTimer>
#include "../../../backend/rendering/ctextrendering.h"
#include "bttextfilter.h"


class CSwordKey;
class CSwordModuleInfo;

class QTimer;
class RoleItemModel;

struct RefIndexes {
    QString r1;
    QString r2;
    int index1;
    int index2;
};

/**
 * /brief This class provides communications between QML and c++.
 *
 * It is instantiated by usage within the QML files. It provides
 * properties and functions written in c++ and usable by QML.
 */

class BtQmlInterface : public QObject {

    Q_OBJECT

    enum Format {
        HTML,
        Text
    };
    Q_PROPERTY(QString      activeLink              READ getActiveLink  NOTIFY activeLinkChanged    WRITE setActiveLink)
    Q_PROPERTY(QColor       backgroundColor         READ getBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor       backgroundHighlightColor READ getBackgroundHighlightColor NOTIFY backgroundHighlightColorChanged)
    Q_PROPERTY(int          backgroundHighlightColorIndex READ getBackgroundHighlightColorIndex NOTIFY backgroundHighlightColorIndexChanged)
    Q_PROPERTY(int          contextMenuIndex        READ getContextMenuIndex NOTIFY contextMenuIndexChanged WRITE setContextMenuIndex)
    Q_PROPERTY(int          contextMenuColumn       READ getContextMenuColumn NOTIFY contextMenuColumnChanged WRITE setContextMenuColumn)
    Q_PROPERTY(int          currentModelIndex       READ getCurrentModelIndex NOTIFY currentModelIndexChanged)
    Q_PROPERTY(QFont        font0                   READ getFont0   NOTIFY fontChanged)
    Q_PROPERTY(QFont        font1                   READ getFont1   NOTIFY fontChanged)
    Q_PROPERTY(QFont        font2                   READ getFont2   NOTIFY fontChanged)
    Q_PROPERTY(QFont        font3                   READ getFont3   NOTIFY fontChanged)
    Q_PROPERTY(QFont        font4                   READ getFont4   NOTIFY fontChanged)
    Q_PROPERTY(QFont        font5                   READ getFont5   NOTIFY fontChanged)
    Q_PROPERTY(QFont        font6                   READ getFont6   NOTIFY fontChanged)
    Q_PROPERTY(QFont        font7                   READ getFont7   NOTIFY fontChanged)
    Q_PROPERTY(QFont        font8                   READ getFont8   NOTIFY fontChanged)
    Q_PROPERTY(QFont        font9                   READ getFont9   NOTIFY fontChanged)
    Q_PROPERTY(QColor       foregroundColor         READ getForegroundColor NOTIFY foregroundColorChanged)
    Q_PROPERTY(QString      highlightWords          READ getHighlightWords NOTIFY highlightWordsChanged)
    Q_PROPERTY(int          numModules              READ getNumModules NOTIFY numModulesChanged)
    Q_PROPERTY(bool         pageDown                READ getPageDown   NOTIFY pageDownChanged)
    Q_PROPERTY(bool         pageUp                  READ getPageUp     NOTIFY pageUpChanged)
    Q_PROPERTY(double       pixelsPerMM             READ getPixelsPerMM NOTIFY pixelsPerMMChanged)
    Q_PROPERTY(QVariant     textModel               READ getTextModel NOTIFY textModelChanged)

public:
    Q_INVOKABLE void cancelMagTimer();
    Q_INVOKABLE void changeReference(int i);
    Q_INVOKABLE void dragHandler(int index,const QString& link);
    Q_INVOKABLE QString getRawText(int row, int column);
    Q_INVOKABLE QStringList getModuleNames() const;
    Q_INVOKABLE bool moduleIsWritable(int column);
    Q_INVOKABLE void openContextMenu(int x, int y,int width);
    Q_INVOKABLE void openEditor(int row, int column);
    Q_INVOKABLE void referenceChosen();
    Q_INVOKABLE void setMagReferenceByUrl(const QString& url);
    Q_INVOKABLE void setRawText(int row, int column, const QString& text);
    Q_INVOKABLE void clearSelectedText();
    Q_INVOKABLE bool hasSelectedText();
    Q_INVOKABLE void saveSelectedText(int index, const QString& text);
    Q_INVOKABLE void setBoundsMovement();
    Q_INVOKABLE void setKeyFromLink(const QString& link);
    Q_INVOKABLE bool shiftKeyDown();
    Q_INVOKABLE int indexToVerse(int index);

    BtQmlInterface(QObject *parent = nullptr);
    ~BtQmlInterface();

    int countHighlightsInItem(int index);
    void findText(const QString& text, bool caseSensitive, bool backward);
    void getNextMatchingItem(int index);
    void getPreviousMatchingItem(int index);


    QString getActiveLink() const;
    QColor getBackgroundColor() const;
    QColor getBackgroundHighlightColor() const;
    QColor getForegroundColor() const;
    int getBackgroundHighlightColorIndex() const;
    void changeColorTheme();
    void copyRange(int index1, int index2);
    void copyVerseRange(const QString& ref1, const QString& ref2, const CSwordModuleInfo * module);
    QString getBibleUrlFromLink(const QString& url);
    int getContextMenuIndex() const;
    int getContextMenuColumn() const;
    int getCurrentModelIndex() const;
    QFont getFont0() const;
    QFont getFont1() const;
    QFont getFont2() const;
    QFont getFont3() const;
    QFont getFont4() const;
    QFont getFont5() const;
    QFont getFont6() const;
    QFont getFont7() const;
    QFont getFont8() const;
    QFont getFont9() const;
    QString getHighlightWords() const;
    CSwordKey* getKey() const;
    CSwordKey* getMouseClickedKey() const;
    QString getLemmaFromLink(const QString& url);
    int getNumModules() const;
    bool getPageDown() const;
    bool getPageUp() const;
    double getPixelsPerMM() const;
    QString getSelectedText();
    QVariant getTextModel();
    bool isBibleOrCommentary();
    BtModuleTextModel * textModel();
    void pageDown();
    void pageUp();
    void referenceChoosen();
    void scrollToSwordKey(CSwordKey * key);
    void setActiveLink(const QString& link);
    void setContextMenuIndex(int index);
    void setContextMenuColumn(int index);
    void setFilterOptions(FilterOptions filterOptions);
    void setHighlightWords(const QString& words, bool caseSensitivy);
    void setKey(CSwordKey* key);
    void setModules(const QStringList &modules);
    void settingsChanged();

signals:
    void activeLinkChanged();
    void backgroundColorChanged();
    void backgroundHighlightColorChanged();
    void backgroundHighlightColorIndexChanged();
    void contextMenuIndexChanged();
    void contextMenuColumnChanged();
    void contextMenu(int x, int y, int moduleNum);
    void currentModelIndexChanged();
    void fontChanged();
    void foregroundColorChanged();
    void highlightWordsChanged();
    void numModulesChanged();
    void pageDownChanged();
    void pageUpChanged();
    void pixelsPerMMChanged();
    void positionItemOnScreen(int index);
    void referenceChange();
    void newBibleReference(const QString& reference);
    void textChanged();
    void textModelChanged();
    void updateReference(const QString& reference);
    void dragOccuring(const QString& moduleName, const QString& keyName);

private slots:
    void timeoutEvent();
    void slotSetHighlightWords();

private:
    void configModuleByType(const QString& type, const QStringList& availableModuleNames);
    bool copyKey(CSwordKey const * const key, Format const format, bool const addText);
    QString decodeLemma(const QString& value);
    QString decodeMorph(const QString& value);
    QFont font(int column) const;
    void getFontsFromSettings();
    QString getReferenceFromUrl(const QString& url);
    const CSwordModuleInfo* module() const;
    std::unique_ptr<Rendering::CTextRendering> newRenderer(Format const format, bool const addText);
    RefIndexes normalizeReferences(const QString& ref1, const QString& ref2);
    QString stripHtml(const QString& html);

    bool m_firstHref;
    QTimer* m_linkTimer;
    BtModuleTextModel* m_moduleTextModel;
    CSwordKey* m_swordKey;

    QList<QFont> m_fonts;
    int m_backgroundHighlightColorIndex;
    bool m_caseSensitive;
    QString m_highlightWords;
    QStringList m_moduleNames;
    BtTextFilter m_textFilter;
    QString m_timeoutUrl;
    int m_contextMenuIndex;
    int m_contextMenuColumn;
    QString m_activeLink;
    FindState m_findState;
    QTimer m_timer;
    QMap<int, QString> m_selectedText;
};

#endif
