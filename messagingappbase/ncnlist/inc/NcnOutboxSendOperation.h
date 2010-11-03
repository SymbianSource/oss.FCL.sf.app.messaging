/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*       When the phone mode is changed from offline to online mode, this class
*       sends the messages in outbox, that are in suspended or failed state
*       because of the offline mode. When phone is changed from online to
*       offline mode, the currently running sending operation will be
*       cancelled.
*
*/



#ifndef __NCNOUTBOXSENDOPERATION_H__
#define __NCNOUTBOXSENDOPERATION_H__

//  INCLUDES
#include <msvapi.h>
#include <MuiuMsvSingleOpWatcher.h>

// CONSTANTS
_LIT8( KNcnBlankBuffer, "" );

// FORWARD DECLARATIONS
class CMsvEntrySelection;

// CLASS DECLARATION

/**
*  Sends messages from Outbox when user selects "Start" in Outbox.
*/
class CNcnOutboxSendOperation : public CMsvOperation
    {
    public: // Constructors and destructor

        /**
        *   A two-phased constructor.
        */
        static CNcnOutboxSendOperation* NewL(
            CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus);

        /**
        *   A destructor.
        */
        virtual ~CNcnOutboxSendOperation();

        /**
        *   Returns the current operation.
        */
        virtual const TDesC8& ProgressL();

    private:

        /**
        *   A C++ constructor
        */
        CNcnOutboxSendOperation(
            CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus);

        /**
        *   Creates the sending operation session.
        */
        void ConstructL();

    private:    // Functions from base classes

        /**
        *   Cancels sending operation.
        */
        void DoCancel();

        /**
        *   Keeps the message sending operaion active until all
        *   messages are sent.
        */
        void RunL();

        /**
        *  Handles the runtime error events
        */
        TInt RunError( TInt aError );

    private:    // New functions

        /**
        *   Starts sending operation for  next selection.
        */
        void StartSendingL();

        /**
        *   Sets the selected messages to sending state.
        */
        void SendWaitingSMSMessages();

        /**
        *   Sets the selected messages to sending state.
        */
        void SendWaitingSMSMessagesL();

        /**
        *   Creates selection of spesified type of messages in outbox
        */
        void CreateSelectionL(
            const TUid& aUidForSel,
            const TMsvId& aServiceId,
            CMsvEntrySelection*& aSelection );

        /**
        *   Checks if MTM is available and creates selection
        */
        TBool CheckAndCreateSelectionL();

        /**
        * Completes operation and cancels notify
        */
        void CompleteObserver( TInt aStatus );

        /**
        * Deletes the selection
        */
        void RemoveSelection();

        /**
        * Checks if there is need to continue sending operation
        */
        TBool IsSendingNeeded( TUid& aMtm ) const;

        /**
        * Removes the sending flag
        */
        void RemoveSendingFlag( const TUid& aMtm );
        
        /**
        * Complete own request
        */
        void CompleteSelf( TInt aValue );


    private:    // Data
        enum
        {
            ENcnSupportsSmsSending      = 0x01,
            ENcnSendSms                 = 0x10,
        };

        // Selection queue
        CArrayPtrFlat<CMsvEntrySelection> iSelections;
        // Services selection
        CMsvEntrySelection      iServices;
        // Pointer to Msv Operation
        CMsvOperation*          iOperation;
        // Entry pointer
        CMsvEntry*              iEntry;
        // Supported message types
        TUint                   iSupportedMsgs;
    };


#endif // __NCNOUTBOXSENDOPERATION_H__

// End of file
