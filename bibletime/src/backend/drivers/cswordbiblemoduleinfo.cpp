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

#include "cswordbiblemoduleinfo.h"

#include <memory>
#include <QFile>
#include "../../util/btassert.h"
#include "../managers/cswordbackend.h"

// Sword includes:
#include <versekey.h>


CSwordBibleModuleInfo::CSwordBibleModuleInfo(sword::SWModule & module,
                                             CSwordBackend & backend,
                                             ModuleType type)
        : CSwordModuleInfo(module, backend, type)
        , m_boundsInitialized(false)
        , m_lowerBound(nullptr)
        , m_upperBound(nullptr)
        , m_bookList(nullptr)
{
    // Intentionally empty
}

void CSwordBibleModuleInfo::initBounds() const {
    /// \todo The fields calculated by this method could be cached to disk.

    BT_ASSERT(!m_boundsInitialized);

    auto & m = module();
    const bool oldStatus = m.isSkipConsecutiveLinks();
    m.setSkipConsecutiveLinks(true);

    sword::VerseKey * key = static_cast<sword::VerseKey *>(m.getKey());
    m_lowerBound.setVersificationSystem(key->getVersificationSystem());
    m_upperBound.setVersificationSystem(key->getVersificationSystem());

    m.setPosition(sword::TOP); // position to first entry
    m_hasOT = (key->getTestament() == 1);
    m_lowerBound.setKey(key->getText());

    m.setPosition(sword::BOTTOM);
    m_hasNT = (key->getTestament() == 2);
    m_upperBound.setKey(key->getText());

    m.setSkipConsecutiveLinks(oldStatus);
    m_boundsInitialized = true;
}


/** Returns the books available in this module */
QStringList *CSwordBibleModuleInfo::books() const {
    {
        CSwordBackend & b = backend();
        if (m_cachedLocale != b.booknameLanguage()) {
            // Reset the booklist because the locale has changed
            m_cachedLocale = b.booknameLanguage();
            delete m_bookList;
            m_bookList = nullptr;
        }
    }

    if (!m_bookList) {
        m_bookList = new QStringList();

        if (!m_boundsInitialized)
            initBounds();

        for (sword::VerseKey key(m_lowerBound);
             !key.popError() && key.compare(m_upperBound) <= 0;
             key.setBook(key.getBook() + 1))
        {
            m_bookList->append( QString::fromUtf8(key.getBookName()) );
        }
    }

    return m_bookList;
}

unsigned int CSwordBibleModuleInfo::chapterCount(const unsigned int book) const {
    int result = 0;

    std::unique_ptr<sword::VerseKey> key(
            static_cast<sword::VerseKey *>(module().createKey()));
    key->setPosition(sword::TOP);

    // works for old and new versions
    key->setBook(book);
    key->setPosition(sword::MAXCHAPTER);
    result = key->getChapter();

    return result;
}

unsigned int CSwordBibleModuleInfo::chapterCount(const QString &book) const {
    return chapterCount(bookNumber(book));
}

/** Returns the number of verses  for the given chapter. */

unsigned int CSwordBibleModuleInfo::verseCount(const unsigned int book,
                                               const unsigned int chapter) const
{
    unsigned int result = 0;

    std::unique_ptr<sword::VerseKey> key(
            static_cast<sword::VerseKey *>(module().createKey()));
    key->setPosition(sword::TOP);

    // works for old and new versions
    key->setBook(book);
    key->setChapter(chapter);
    key->setPosition(sword::MAXVERSE);
    result = key->getVerse();

    return result;
}

unsigned int CSwordBibleModuleInfo::verseCount(const QString &book,
                                               const unsigned int chapter) const
{
    return verseCount(bookNumber(book), chapter);
}

unsigned int CSwordBibleModuleInfo::bookNumber(const QString &book) const {
    unsigned int bookNumber = 0;

    std::unique_ptr<sword::VerseKey> key(
            static_cast<sword::VerseKey *>(module().createKey()));
    key->setPosition(sword::TOP);

    key->setBookName(book.toUtf8().constData());

    bookNumber = ((key->getTestament() > 1) ? key->BMAX[0] : 0) + key->getBook();

    return bookNumber;
}
