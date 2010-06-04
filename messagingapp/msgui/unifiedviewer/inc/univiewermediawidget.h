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

#ifndef UNIVIEWERMEDIAWIDGET_H
#define UNIVIEWERMEDIAWIDGET_H

// SYSTEM INCLUDES
#include <HbWidget>

// FORWARD DECLARATIONS
class HbTextItem;
class HbIconItem;
class HbIcon;
class HbPushButton;

/**
 * Widget for displaying inline media objects.
 */
class UniViewerMediaWidget : public HbWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit UniViewerMediaWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    virtual ~UniViewerMediaWidget();

    /**
     * Initialize method
     */
    void init();

    /**
     * Populates the widget contents.
     * @param icon Media icon name.
     * @param name Media file name.
     * @param info Media information.
     * @param bgFrame Background frame.
     */
    void populate(const QString &icon, const QString &name,
                  const QString &info, const QString &bgFrame);

    /**
     * Clears the contents of the widget.
     */
    void clearContent();

signals:

    /**
     * Signal emitted when widget is clicked.
     */
    void clicked();

private:

    /**
     * Media icon item.
     * Own
     */
    HbIconItem *mMediaIcon;

    /**
     * Media name text item.
     * Own
     */
    HbTextItem *mName;

    /**
     * Media information text item.
     * Own
     */
    HbTextItem *mInfo;

    /**
     * Background item of widget.
     * Own
     */
    HbPushButton *mFrameItem;
};

#endif // UNIVIEWERMEDIAWIDGET_H
// EOF
