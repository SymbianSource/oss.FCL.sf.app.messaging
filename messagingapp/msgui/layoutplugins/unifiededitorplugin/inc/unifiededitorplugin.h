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

#ifndef UNIFIEDEDITORPLUGIN_H
#define UNIFIEDEDITORPLUGIN_H

// INCLUDES
#include <QtPlugin>
#include <hbstyleinterface.h>
#include <hbglobal.h>

/**
 * TODO: add comment
 */
class UnifiedEditorPlugin : public QObject, public HbStyleInterface
{
    Q_OBJECT Q_INTERFACES(HbStyleInterface)

public:
    /**
     * Inherited from HbStyleInterface
     * @see HbStyleInterface docs
     */
    int primitiveCount() const;

    /**
     * Inherited from HbStyleInterface
     * @see HbStyleInterface docs
     */
    QGraphicsItem    *createPrimitive( HbStyle::Primitive primitive, QGraphicsItem *parent = 0 ) const;

    /**
     * Inherited from HbStyleInterface
     * @see HbStyleInterface docs
     */
    void updatePrimitive( QGraphicsItem *item, HbStyle::Primitive primitive, const QStyleOption *option ) const;

    /**
     * Inherited from HbStyleInterface
     * @see HbStyleInterface docs
     */
    QString layoutPath() const;

};

#endif // UNIFIEDEDITORPLUGIN_H
