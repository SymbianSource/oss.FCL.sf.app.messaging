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
 * Description: Custom widget derived from HbTextEdit which provides rich text
 *              processing
 *
 */
#ifndef UNI_TEXT_EDITOR_H
#define UNI_TEXT_EDITOR_H

#include <hbwidget.h>
#include <hbtextedit.h>
#include <qobject.h>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextOption>
#include <QString>

#include "unidatamodelplugininterface.h"

class QGraphicsItem;

/**
 * UniTextEditor provides support for rich text processing
 */
class UniTextEditor : public HbTextEdit
{
Q_OBJECT
public:
    /**
     * Constructor
     */
    UniTextEditor(QGraphicsItem * parent = 0);

    /**
     * Destructor
     */
    ~UniTextEditor();

public:

    /**
     * called when focus is gained.
     */
    void focusInEvent(QFocusEvent *event);

    /**
     * called when focus is lost.
     */
    void focusOutEvent(QFocusEvent *event);

    /**
     * called when mouse is pressed.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    /**
     * called when mouse is moved.
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    /**
     * called when mouse is released.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    };

#endif // UNI_TEXT_EDITOR_H
// EOF
