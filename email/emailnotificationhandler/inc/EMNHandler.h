/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: An EMN handler WAP Push Plugin.
*		
*
*/


#ifndef EMNHANDLER_H
#define EMNHANDLER_H

enum TEMNErrors
    {
    EEMNNoError = 0,
    EEMNMissingEMNElement,
    EEMNMissingMailboxAttribute,
    EEMNMissingTimestampAttribute,
    EEMNInvalidTimestampAttribute,
    EEMNInvalidYear,
    EEMNInvalidMonth,
    EEMNInvalidDay,
    EEMNInvalidHour,
    EEMNInvalidMinute,
    };

#include <e32base.h>
#include <push/cpushhandlerbase.h>
#include <AlwaysOnlineManagerClient.h>
#include "EMNXMLContentHandler.h"

/** 
* The CEMNHandler is a WAP Push Plugin. It registers for a email notification 
* push message. It is invoked to do its work in when HandleMessage() API is 
* called by the Push Watcher framework.
* 
* As a WAP Push Plugin, it must destroy itself when it is complete. The Watcher
* does not hold a pointer to it, it is invoked and responsible for destroying 
* itself.
*   
*/ 
class CEMNHandler : public CPushHandlerBase
	{
    public:	// Methods

        /**
        * Creates object from CEMNHandler
        * @since S60 v3.1
        * @return, Constructed object
        */
    	static CEMNHandler* NewL();
    	
        /**
        * Destructor
        * @since S60 v3.1
        */
     	virtual ~CEMNHandler();

    public:	// Methods from CPushHandlerBase

        /**
        * Called by the Wap Push Framework to handle the message.
        * @since S60 v3.1
        * @param aPushMsg The push message delivered from the framework containing
    	*                 the Push Message.  Ownership is taken over.
        * @param aStatus  when this function is complete, this status is returned 
    	*                 with acompletion code.
        */
    	void HandleMessageL( CPushMessage* aPushMsg, TRequestStatus& aStatus );

        /**
        * Called by the Wap Push Framework to handle the message.
        * @since S60 v3.1
        * @param aPushMsg The push message delivered from the framework containing
    	*                 the Push Message.  Ownership is taken over.
        */
    	void HandleMessageL( CPushMessage* aPushMsg );

        /**
        * Called by the Wap Push Framework to cancel outstanding events.
        * @since S60 v3.1
        */
    	void CancelHandleMessage();

        /**
        * Reserved for ECOM for future expansion
        * @since S60 v3.1
        */
    	virtual void CPushHandlerBase_Reserved1();

        /**
        * Reserved for ECOM for future expansion
        * @since S60 v3.1
        */
    	virtual void CPushHandlerBase_Reserved2();

    private:	// Methods from CActive

        /**
        * Cancels the outstanding request.
        * @since S60 v3.1
        */
    	void DoCancel();

        /**
        * Called when object is active
        * @since S60 v3.1
        */
    	void RunL();


    private:	// Methods

        /**
        * Default constructor for class CEMNHandler
        * @since S60 v3.1
        * @return, Constructed object
        */
    	CEMNHandler();

        /**
        * Symbian 2-phase constructor
        * @since S60 v3.1
        */
    	void ConstructL();


        /**
        * Completes itself.
        * @since S60 v3.1
        */
    	void IdleComplete();

        /**
        * Handle the suicide.
        * @since S60 v3.1
        */
        void Done( TInt aError );

        /**
        * Handles the parsing of received EMN message.
        * @since S60 v3.1
        */
    	void ParsePushMsgL();
    	
        /**
        * Relays parsed message to always online client.
        * @since S60 v3.1
        */
    	void ProcessPushMsgL();

    private:	// Attributes
    	typedef enum TState { EParsing, EProcessing, EDone };  
    	TState iState;

    	// Push Message passed in
    	CPushMessage*	iMessage;

        // The body of the pushed message as HBufC8 descriptor
        HBufC8* iBody;
            
    	// Flag to indicate if this was called asynchonously
    	TBool			iAsyncHandling;

        // EMN element containing mailbox and timestamp
        TEMNElement     iElement;

        // Used to relay parsed message to always online client.
        RAlwaysOnlineClientSession  iAOClient;

	};

#endif	// EMNHANDLER_H
