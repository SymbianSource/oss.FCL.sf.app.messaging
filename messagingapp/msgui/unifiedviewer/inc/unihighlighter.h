/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: Enables parsing and change font parameters for email/url
 *
 */

#ifndef UNI_HIGHLIGHTER_H
#define UNI_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

/**
 * Enables parsing and change font parameters for email/url
 */
class UniHighlighter : public QSyntaxHighlighter
{
Q_OBJECT

public:
    /*
     * Constructor
     */
    UniHighlighter(QTextDocument *parent = 0);

protected:
    /*
     * highlightBlock
     * @see QSyntaxHighlighter
     */
    void highlightBlock(const QString &text);

private:

    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat emailFormat;
    QTextCharFormat urlFormat;
};

#endif //UNI_HIGHLIGHTER_H
// EOF
