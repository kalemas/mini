/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BTSTATICTEXT_H
#define BTSTATICTEXT_H

#include <QtCore/qsize.h>
#include <QtCore/qstring.h>
#include <QtCore/qmetatype.h>

#include <QtGui/qtransform.h>
#include <QtGui/qfont.h>
#include <QtGui/qtextoption.h>


class BtStaticTextPrivate;
class BtStaticText
{    
public:
    enum PerformanceHint {
        ModerateCaching,
        AggressiveCaching
    };

    BtStaticText();
    BtStaticText(const QString &text);
    BtStaticText(const BtStaticText &other);
    ~BtStaticText();

    void setText(const QString &text);
    QString text() const;

    void setTextFormat(Qt::TextFormat textFormat);
    Qt::TextFormat textFormat() const;

    void setTextWidth(qreal textWidth);
    qreal textWidth() const;

    void setTextOption(const QTextOption &textOption);
    QTextOption textOption() const;

    QSizeF size() const;

    void prepare(const QTransform &matrix = QTransform(), const QFont &font = QFont());

	QFont defaultFont();

	int hitTest(QPoint coordinates);

	QString plainText();

    void setPerformanceHint(PerformanceHint performanceHint);
    PerformanceHint performanceHint() const;

    BtStaticText &operator=(const BtStaticText &);
    bool operator==(const BtStaticText &) const;
    bool operator!=(const BtStaticText &) const;

private:
    void detach();

    QExplicitlySharedDataPointer<BtStaticTextPrivate> data;
    friend class BtStaticTextPrivate;
};

Q_DECLARE_METATYPE(BtStaticText)

#endif // QSTATICTEXT_H
