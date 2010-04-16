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
 * Description: Custom layout widget for Contact Card layout.
 *
 */

#ifndef MSGCONTACTCARDWIDGET_H
#define MSGCONTACTCARDWIDGET_H

// SYSTEM INCLUDES
#include <HbAbstractButton>

// FORWORD DECLARATIONS
class HbIconItem;
class HbTextItem;
class HbIcon;
class HbPushButton;
class QGraphicsSceneMouseEvent;

#include "convergedmessageaddress.h"

/**
 * This class is a custom layout widget for Contact Card layout.
 */
class MsgContactCardWidget : public HbAbstractButton
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit MsgContactCardWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    virtual ~MsgContactCardWidget();

public:

    /**
     * Sets the avatar
     * @param avatar Avatar to be set.
     */
    void setAvatar(const HbIcon &avatar);

    /**
     * Sets the presence icon
     * @param presenceIcon Presence icon to set.
     */
    void setPresenceIcon(const HbIcon &presenceIcon);

    /**
     * Sets the address to be displayed.
     * @param address Address string to be set.
     */
    void setAddress(const QString &address);

    /**
     * Returns the address displayed in the Contact card.
     */
    ConvergedMessageAddressList address();

    /**
     * Refreshes all the Contact card fields.
     */
    void updateContents();
    

    /**
     * Clears  all the Contact card fields.
     */
    void clearContent();

signals:

    /**
     * Signal emitted when widget is clicked.
     */
    void clicked();

private:

    /**
     * Initialization function.
     */
    void init();

private:
    // Data

    /**
     * Address string.
     */
    QString mAddress;

    /**
     * Avatar icon item.
     * Own.
     */
    HbIconItem *mAvatarIconItem;

    /**
     * Presence icon item.
     * Own.
     */
    HbIconItem *mPresenceIconItem;

    /**
     * Address text item.
     * Own.
     */
    HbTextItem *mAddressTextItem;

    /**
     * Background frame item.
     * Own.
     */
    HbPushButton *mBackgroundItem;
};

#endif // MSGCONTACTCARDWIDGET_H
// EOF
