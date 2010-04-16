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
 * conversationlist view.
 *
 */

// INCLUDES
#include <hbstyle.h>
#include <hbiconitem.h>
#include <hbtextitem.h>
#include <QDir>
#include "debugtraces.h"
#include <hbwidget.h>
#include <hbinstance.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>

// USER INCLUDES
#include "msglistviewitemplugin.h"

// Frames
#define NEW_ITEM_FRAME ":/newitem/qtg_fr_list_new_item"
// Constants
#define PRIMITIVECOUNT 6;

//---------------------------------------------------------------
// MsgListViewItemPlugin::primitiveCount
// @see header file
//---------------------------------------------------------------
int MsgListViewItemPlugin::primitiveCount() const
{
    return PRIMITIVECOUNT;
}

//---------------------------------------------------------------
// MsgListViewItemPlugin::createPrimitive
// @see header file
//---------------------------------------------------------------
QGraphicsItem* MsgListViewItemPlugin::createPrimitive(
    HbStyle::Primitive primitive, QGraphicsItem *parent) const
{
    Q_UNUSED(primitive);
    Q_UNUSED(parent);   
}



//---------------------------------------------------------------
// MsgListViewItemPlugin::updatePrimitive
// @see header file
//---------------------------------------------------------------
void MsgListViewItemPlugin::updatePrimitive(QGraphicsItem *item,
    HbStyle::Primitive primitive, const QStyleOption *option) const
{

    Q_UNUSED(item);

    Q_UNUSED(primitive);
    Q_UNUSED(option);
}


	


//---------------------------------------------------------------
// MsgListViewItemPlugin::layoutPath
// @see header file
//---------------------------------------------------------------
QString MsgListViewItemPlugin::layoutPath() const
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgListViewItemPlugin::layoutPath";
#endif

    QString path;

    path = QString(":/");

#ifdef _DEBUG_TRACES_
    qDebug() << "layout plugin resources:" << path;
#endif

    return path;
}

Q_EXPORT_PLUGIN2(msglistviewitemplugin, MsgListViewItemPlugin)
