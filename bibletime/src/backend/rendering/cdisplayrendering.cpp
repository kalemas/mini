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

#include "cdisplayrendering.h"

#include <QRegExp>
#include <QString>
#include <QtGlobal>
#include "../config/btconfig.h"
#include "../keys/cswordkey.h"
#include "../keys/cswordversekey.h"
#include "../managers/cdisplaytemplatemgr.h"
#include "../managers/referencemanager.h"


namespace Rendering {

CDisplayRendering::CDisplayRendering(const DisplayOptions &displayOptions,
                                     const FilterOptions &filterOptions)
        : CHTMLExportRendering(true, displayOptions, filterOptions)
{
    // Intentionally empty
}

QString CDisplayRendering::entryLink(const KeyTreeItem & item,
                                     const CSwordKey * key) {
    QString linkText;

    const bool isBible = key->module()->type() == CSwordModuleInfo::Bible;
    const CSwordVerseKey * verseKey = static_cast<const CSwordVerseKey *>(key);

    if (isBible && (verseKey->getVerse() == 0)) {
        return QString(); //Warning: return already here
    }

    switch (item.settings().keyRenderingFace) {

    case KeyTreeItem::Settings::NoKey:
        linkText = QString();
        break; //no key is valid for all modules

    case KeyTreeItem::Settings::ExpandedShort:
        if (isBible) {
            linkText = key->module()->name() + ':' +
                       QString::fromUtf8(verseKey->getShortText());
            break;
        }
        Q_FALLTHROUGH();

    case KeyTreeItem::Settings::CompleteShort:
        if (isBible) {
            linkText = QString::fromUtf8(verseKey->getShortText());
            break;
        }
        Q_FALLTHROUGH();

    case KeyTreeItem::Settings::ExpandedLong:
        if (isBible) {
            linkText = QString("%1 (%2)")
                           .arg(verseKey->key())
                           .arg(verseKey->module()->name());
            break;
        }
        Q_FALLTHROUGH();

    case KeyTreeItem::Settings::CompleteLong:
        if (isBible) {
            linkText = verseKey->key();
            break;
        }
        Q_FALLTHROUGH();

    case KeyTreeItem::Settings::SimpleKey:
        if (isBible) {
            if (item.key() != verseKey->key()) {
                const CSwordVerseKey * baseKey =
                    static_cast<const CSwordVerseKey *>(item.swordKey());

                if (verseKey->getBook() != baseKey->getBook()) {
                    linkText = QString::fromUtf8(verseKey->getShortText());
                } else if (verseKey->getChapter() != baseKey->getChapter()) {
                    linkText = QString("%1:%2")
                                   .arg(verseKey->getChapter())
                                   .arg(verseKey->getVerse());
                } else {
                    linkText = QString::number(verseKey->getVerse());
                }

                if (verseKey->isBoundSet()) {
                    linkText += "-";
                    sword::VerseKey const upper = verseKey->getUpperBound();
                    sword::VerseKey const lower = verseKey->getLowerBound();
                    if (upper.getBook() != lower.getBook()) {
                        linkText += QString::fromUtf8(upper.getShortText());
                    } else if(upper.getChapter() != lower.getChapter()) {
                        linkText += QString("%1:%2").arg(upper.getChapter())
                                                    .arg(lower.getVerse());
                    } else {
                        linkText += QString::number(upper.getVerse());
                    }
                }
            } else {
                linkText = QString::number(verseKey->getVerse());
            }
            break;
        } // else fall through for non-Bible modules
        Q_FALLTHROUGH();

    default: //default behaviour to return the passed key
        linkText = item.key();
        break;

    }


    if (linkText.isEmpty()) {
        return QString("<a name=\"").append(keyToHTMLAnchor(item.key())).append("\"></a>");
    } else {
        return QString("<a name=\"")
            .append(keyToHTMLAnchor(item.key()))
            .append("\" ")
            .append("href=\"")
            .append(ReferenceManager::encodeHyperlink(
                key->module()->name(), key->key(),
                ReferenceManager::typeFromModule(key->module()->type())))
            .append("\">")
            .append(linkText)
            .append("</a>\n");
    }
}

QString CDisplayRendering::keyToHTMLAnchor(const QString& key) {
    // Be careful not to remove non-ASCII characters, this causes problems
    // with many languages.
    return key.trimmed().remove(QRegExp("\\s")).replace(QString(":"), QString("_"));
}

QString CDisplayRendering::finishText(const QString &text, const KeyTree &tree) {
    BtConstModuleList modules = collectModules(tree);

    //marking words is very slow, we have to find a better solution

    /*
     //mark all words by spans

     QString text = oldText;

     QRegExp re("(\\b)(?=\\w)"); //word begin marker
     int pos = text.find(re, 0);

     while (pos != -1) { //word begin found
      //qWarning("found word at %i in %i", pos, text.length());
      int endPos = pos + 1;
      if (!util::tool::inHTMLTag(pos+1, text)) { //the re has a positive look ahead which matches one char before the word start
       //qWarning("matched %s", text.mid(pos+1, 4).latin1());

       //find end of word and put a marker around it
       endPos = text.find(QRegExp("\\b|[,.:]"), pos+1);
       if ((endPos != -1) && !util::tool::inHTMLTag(endPos, text) && (endPos - pos >= 3)) { //reuire wordslonger than 3 chars
        text.insert(endPos, "</span>");
        text.insert(pos, "<span class=\"word\">");

        endPos += 26;
       }
      }
      pos = text.find(re, endPos);
     }
    */

    CDisplayTemplateMgr *tMgr = CDisplayTemplateMgr::instance();

    //BT_ASSERT(modules.count() >= 1);

    CDisplayTemplateMgr::Settings settings;
    settings.modules = modules;
    if (modules.count() == 1) {
        if (auto * const lang = modules.first()->language()) {
            auto const & abbrev = lang->abbrev();
            if (!abbrev.isEmpty())
                settings.langAbbrev = abbrev;
        }
    }

    if (modules.count() == 1)
        settings.textDirection = modules.first()->textDirection();

    return tMgr->fillTemplate(CDisplayTemplateMgr::activeTemplateName(), text, settings);
}
}
