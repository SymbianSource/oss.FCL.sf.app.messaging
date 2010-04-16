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
 * Description: Layout plugin for all the widgets present in
 * conversation view.
 *
 */

#include "conversationviewplugin.h"

// INCLUDES
#include <hbstyle.h>
#include <hbiconitem.h>
#include <hbtextitem.h>
#include <QDir>
#include <hbwidget.h>
#include <hbinstance.h>

#include "debugtraces.h"

// Constants
#define PRIMITIVECOUNT 1;

//---------------------------------------------------------------
// ConversationViewPlugin::primitiveCount
// @see header file
//---------------------------------------------------------------
int ConversationViewPlugin::primitiveCount() const
{
#ifdef _DEBUG_TRACES_
    qDebug() << "ConversationViewPlugin::primitiveCount";
#endif


    return PRIMITIVECOUNT;
}

//---------------------------------------------------------------
// ConversationViewPlugin::createPrimitive
// @see header file
//---------------------------------------------------------------
QGraphicsItem* ConversationViewPlugin::createPrimitive(
    HbStyle::Primitive primitive, QGraphicsItem *parent) const
{
    Q_UNUSED(primitive)
    Q_UNUSED(parent)
    return NULL;
}

//---------------------------------------------------------------
// ConversationViewPlugin::updatePrimitive
// @see header file
//---------------------------------------------------------------
void ConversationViewPlugin::updatePrimitive(QGraphicsItem *item,
    HbStyle::Primitive primitive, const QStyleOption *option) const
{
  Q_UNUSED(item)
  Q_UNUSED(primitive)
  Q_UNUSED(option)
}

//---------------------------------------------------------------
// ConversationViewPlugin::layoutPath
// @see header file
//---------------------------------------------------------------
QString ConversationViewPlugin::layoutPath() const
{
    QString path;

    path = QString(":/");

#ifdef _DEBUG_TRACES_
    qDebug() << "layout plugin resources:" << path;
#endif
    return path;
}

Q_EXPORT_PLUGIN2(conversationviewplugin, ConversationViewPlugin)

// EOF
