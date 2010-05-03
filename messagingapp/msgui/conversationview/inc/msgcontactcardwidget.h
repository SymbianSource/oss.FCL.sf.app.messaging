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
#include <HbWidget>

// FORWORD DECLARATIONS
class HbIconItem;
class HbTextItem;
class HbIcon;
class QGraphicsSceneMouseEvent;
class HbGestureSceneFilter;

#include "convergedmessageaddress.h"

/**
 * This class is a custom layout widget for Contact Card layout.
 */
class MsgContactCardWidget : public HbWidget
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
    
    /**
     * for tactile feed back.
     */
    HbFeedback::InstantEffect overrideFeedback(Hb::InstantInteraction interaction) const;
    
    /**
     * To connect/disconnect clicked signal
     */
    void connectSignals(bool yes);
    
protected:
    /**
     * reimplemented from base class.
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    
    /**
     * reimplemented from base class.
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:

    /**
     * Initialization function.
     */
    void init();
    
    /** Helper method to get contact id against phone number.
     * @param value phone number.
     */
    int resolveContactId(const QString& value);
    
    /**
     * Helper method to set back ground.
     */
    void setBackGround(const QString& bg);
    
private slots:
    /**
     * show longpress menu for attachment object
     */
    void handleLongPress(QPointF position);
    
    /**
     * Helper method to initialize gesture.
     */
    void initGesture();
	
    /**
     * Slot for handling valid returns from the framework.
     * Updates the display name in the contact card widget.
     * @param result const QVariant&
     */
    void handleOk(const QVariant& result);
    
    /**
     * Slot for handling errors. Error ids are provided as 
     * 32-bit integers.
     * @param errorCode qint32
     */
    void handleError(int errorCode, const QString& errorMessage);
	    
    /**
     * Called when clicked() signal is emitted
     * Launches phonebook to view an existing contact 
     * or to add a new contact
     */
    void openContactInfo();
    
    /**
     * Launches Dialer Service 
     */
    void call();
    
    /**
     * Adds unknown number to phonebook
     */
    void addToContacts();
    
    /**
     * Called after service request is completed.
     */
    void onServiceRequestCompleted();
    
signals:
   /**
	* Emitted when contact card is short tapped.
	*/
    void clicked();
    

private:
    // Data


    /**
     * To supress short tap if long tap triggered.
     */
    bool mMenuShown;  
	
	/**
     * Contact Number for the conversation
     */
    QString mContactNumber;
	
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
     * gesture filter for long press.
     */    
    HbGestureSceneFilter* mGestureFilter;
	

    
  
};

#endif // MSGCONTACTCARDWIDGET_H
// EOF
