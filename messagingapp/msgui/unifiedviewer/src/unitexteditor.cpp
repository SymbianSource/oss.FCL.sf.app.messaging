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
 * Description:Custom widget derived from HbTextEdit which provides rich text
 *              processing
 *
 */

#include "unitexteditor.h"

#include <QFile>
#include <QGraphicsSceneMouseEvent>
#include <HbAbstractEdit>

//---------------------------------------------------------------
// UniTextEditor::UniTextEditor
// @see header file
//---------------------------------------------------------------
UniTextEditor::UniTextEditor(QGraphicsItem * parent) :
    HbTextEdit(parent)
{
    setReadOnly(true);
    setCursorVisibility(Hb::TextCursorHidden);
    setScrollable(false);
    setAttribute(Qt::WA_InputMethodEnabled, false);
}

//---------------------------------------------------------------
// UniTextEditor::~UniTextEditor
// @see header file
//---------------------------------------------------------------
UniTextEditor::~UniTextEditor()
{

    }

//---------------------------------------------------------------
//UniTextEditor :: focusInEvent
// @see header file
//---------------------------------------------------------------
void UniTextEditor::focusInEvent(QFocusEvent *event)
{
    event->ignore();
}

//---------------------------------------------------------------
//UniTextEditor :: focusOutEvent
// @see header file
//---------------------------------------------------------------
void UniTextEditor::focusOutEvent(QFocusEvent *event)
{
    event->ignore();
}

//---------------------------------------------------------------
//UniTextEditor :: mousePressEvent
// @see header file
//---------------------------------------------------------------
void UniTextEditor::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
}

//---------------------------------------------------------------
//UniTextEditor :: mouseMoveEvent
// @see header file
//---------------------------------------------------------------
void UniTextEditor::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
}

//---------------------------------------------------------------
//UniTextEditor :: mouseReleaseEvent
// @see header file
//---------------------------------------------------------------
void UniTextEditor::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
}

// EOF
