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
#include "unihighlighter.h"

#include <QtGui>

//---------------------------------------------------------------
//UniHighlighter :: UniHighlighter
// @see header file
//---------------------------------------------------------------
UniHighlighter::UniHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    emailFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp(
        "\\b[a-zA-Z0-9._%-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,4}\\b");
    rule.format = emailFormat;
    highlightingRules.append(rule);

    urlFormat.setAnchor(true);
    urlFormat.setFontUnderline(true);
    rule.pattern
            = QRegExp(
                "\\b((https?|ftp|gopher|telnet|file|notes|ms-help):((//)|(\\\\))+[\\w\\d:#@%/;$()~_?\\+-=\\\\.&]*)\\b");
    rule.format = urlFormat;
    highlightingRules.append(rule);
}

//---------------------------------------------------------------
//UniHighlighter :: highlightBlock
// @see header file
//---------------------------------------------------------------
void UniHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) 
        {
            QRegExp expression(rule.pattern);
            int index = text.indexOf(expression);
            while (index >= 0)
            {
                int length = expression.matchedLength();
                setFormat(index, length, rule.format);
                index = text.indexOf(expression, index + length);
            }
        }
}

// EOF
