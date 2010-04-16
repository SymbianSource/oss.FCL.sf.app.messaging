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

#include "unifiedviewerplugin.h"

// INCLUDES
#include <HbStyle>
#include <QtDebug>

//---------------------------------------------------------------
// UnifiedViewerPlugin::primitiveCount
// @see header file
//---------------------------------------------------------------
int UnifiedViewerPlugin::primitiveCount() const
{
    // Return non zero count.
    return 1;
}

//---------------------------------------------------------------
// UnifiedViewerPlugin::createPrimitive
// @see header file
//---------------------------------------------------------------
QGraphicsItem* UnifiedViewerPlugin::createPrimitive(
    HbStyle::Primitive primitive, QGraphicsItem *parent) const
{
    Q_UNUSED(primitive);
    Q_UNUSED(parent);
    return NULL;
}

//---------------------------------------------------------------
// UnifiedViewerPlugin::updatePrimitive
// @see header file
//---------------------------------------------------------------
void UnifiedViewerPlugin::updatePrimitive(QGraphicsItem *item,
    HbStyle::Primitive primitive, const QStyleOption *option) const
{
    Q_UNUSED(item);
    Q_UNUSED(primitive);
    Q_UNUSED(option);
}

//---------------------------------------------------------------
// UnifiedViewerPlugin::layoutPath
// @see header file
//---------------------------------------------------------------
QString UnifiedViewerPlugin::layoutPath() const
{
    qDebug() << "UnifiedViewerPlugin::layoutPath";

    QString path = QString(":/");

    qDebug() << "layout plugin resources:" << path;
    return path;
}

Q_EXPORT_PLUGIN2(UnifiedViewerPlugin, UnifiedViewerPlugin)

// EOF
