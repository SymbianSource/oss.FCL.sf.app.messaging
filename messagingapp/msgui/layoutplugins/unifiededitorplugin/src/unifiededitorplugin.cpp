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
 * Description:
 *
 */

// INCLUDES
#include <hbstyle.h>
#include "debugtraces.h"

// USER INCLUDES
#include "unifiededitorplugin.h"

//---------------------------------------------------------------
// UnifiedEditorPlugin::primitiveCount
// @see header file
//---------------------------------------------------------------
int UnifiedEditorPlugin::primitiveCount() const
{
    return 1;
}

//---------------------------------------------------------------
// UnifiedEditorPlugin::createPrimitive
// @see header file
//---------------------------------------------------------------
QGraphicsItem* UnifiedEditorPlugin::createPrimitive(
    HbStyle::Primitive primitive, QGraphicsItem *parent) const
{
    Q_UNUSED(primitive);
    Q_UNUSED(parent);
    return NULL;
}

//---------------------------------------------------------------
// UnifiedEditorPlugin::updatePrimitive
// @see header file
//---------------------------------------------------------------
void UnifiedEditorPlugin::updatePrimitive(QGraphicsItem *item,
    HbStyle::Primitive primitive, const QStyleOption *option) const
{
    Q_UNUSED(item);
    Q_UNUSED(primitive);
    Q_UNUSED(option);
}

//---------------------------------------------------------------
// UnifiedEditorPlugin::layoutPath
// @see header file
//---------------------------------------------------------------
QString UnifiedEditorPlugin::layoutPath() const
{
#ifdef _DEBUG_TRACES_
    qDebug() << "UnifiedEditorPlugin::layoutPath";
#endif


    QString path = QString(":/");

#ifdef _DEBUG_TRACES_
    qDebug() << "layout plugin resources:" << path;
#endif

    return path;
}

Q_EXPORT_PLUGIN2(UnifiedEditorPlugin, UnifiedEditorPlugin)
