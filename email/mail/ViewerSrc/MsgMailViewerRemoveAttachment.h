/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  E-mail attachment remove operation.
*
*/


#ifndef MSGMAILVIEWERREMOVEATTACMENT_H
#define MSGMAILVIEWERREMOVEATTACMENT_H

//  INCLUDES
#include <msvapi.h> // CMsvOperation

// FORWARD DECLARATIONS
class CImPruneMessage;
class CMsvEntry;
class CMailMessage;

/**
*  E-mail attachment remove operation.
*/
class CMsgMailViewerRemoveAttachmentOp :public CMsvOperation
    {
    public:  // Constructors and destructor
        /**
        * NewLC
        * @param aMsvSession Message server session
        * @param aObserverRequestStatus operation status
        * @param aAttachmentId Attachment to be removed
        * @param aMsgID message that holds the attachment
        */        
        static CMsgMailViewerRemoveAttachmentOp* NewLC(
            CMsvSession& aMsvSession, 
            TRequestStatus& aObserverRequestStatus, 
            const TMsvId aAttachmentId,
            TMsvId aMsgID);
            
        /**
        * NewL
        * @since S60 3.1
        * @param aMailMessage Mail message
        * @param aObserverRequestStatus operation status
        * @param aAttachmentId Attachment to be removed
        */        
        static CMsgMailViewerRemoveAttachmentOp* NewL(
            CMailMessage& aMailMessage, 
            TRequestStatus& aObserverRequestStatus, 
            TMsvAttachmentId aAttachmentId );            

        /**
        * Destructor.
        */
        ~CMsgMailViewerRemoveAttachmentOp();
       
    public: // Functions from base classes
        /// From CMsvOperation
        virtual const TDesC8& ProgressL();        
       
    protected:  // Functions from base classes
        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();
        
        /**
        * From CActive
        */
        TInt RunError( TInt aError );

        
    private:
        /**
        * C++ constructor.
        */
        CMsgMailViewerRemoveAttachmentOp(CMsvSession& aMsvSession, 
            TRequestStatus& aObserverRequestStatus,
            TMsvId aMsgID);

        /**
        * Symbian constructor
        * Sets operation active.
        */
        void ConstructL( const TMsvId aAttachmentId );
        void ConstructL(
            CMailMessage& aMailMessage, 
            TMsvAttachmentId aAttachmentId );
    
    private: // implementation
        
        void UpdateAttachmentStatusL();            
        void UpdateStatusL();
        void DoNextStateL();

   private:    // Data
        /**
        * Own: Entry which has attachment
        */
        CMsvEntry* iMsgEntry;

        /**
        * Own: Mail message which has attachment
        */
        CImPruneMessage* iPruneMsg;

        /**
        * Package buffer
        */
        TPckgBuf<TMsvLocalOperationProgress> iProgress;
        
        /**
        * Own: Mail message id
        */
        TMsvId iMsgID;
        
        /**
        * Own: Internal state of object
        */
        TInt iState;
        
        /**
        * Own: Mail message's entry
        */
        CMsvEntry* iCentry;
        
        /**
        * Own: operation to be handled
        */
        CMsvOperation* iMsvOp;

    };

#endif      // MSGMAILEDITORREMOVEATTACMENT_H
            
// End of File
