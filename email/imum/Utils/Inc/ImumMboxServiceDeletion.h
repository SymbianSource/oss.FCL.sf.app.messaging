/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsvOperation encapsulating deletion of email service
*
*/


#ifndef C_IMUMMBOXSERVICEDELETION_H
#define C_IMUMMBOXSERVICEDELETION_H

#include <MsvPrgReporter.h>

class CImumInternalApiImpl;

/**
* MsvOperation encapsulating deletion of email service entries.
*/
NONSHARABLE_CLASS( CImumMBoxServiceDeletion ) :
    public CMsvOperation,
    public MMsvProgressDecoder
    {
    private:
    
        /**
         * 
         * @param aStatus
         * @return CMsvOperation*
         * @since S60 3.2
         */
        CMsvOperation* GetPreCreatedMailDeletionOpL( TRequestStatus& aStatus );
        
        /**
         * Sets the mailbox name into the iProgressString
         * @param aServiceId
         * @since S60 3.2
         */
        void SetProgressStringL( TMsvId aServiceId );
        
    public:
        /**
        *
        * @param aMsvSession
        * @param aObserverRequestStatus
        * @param aReporter
        * @param aServiceId
        * @return CImumMBoxServiceDeletion*
        * @since S60 3.2
        */
        IMPORT_C static CImumMBoxServiceDeletion* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aServiceId );

        /**
        * Destructor
        * @since S60 3.2
        */
        ~CImumMBoxServiceDeletion();

        /**
        * 
        * @since S60 3.2
        */
        virtual const TDesC8& ProgressL();

        /**
        * From MMsvProgressDecoder
        * @since S60 3.2
        */
        virtual TInt DecodeProgress (
            const TDesC8& aProgress,
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans, TBool aInternal );


    protected:
    
        /**
        * From CActive
        * @since S60 3.2
        */
        virtual void RunL();

        /**
        * From CActive
        * @since S60 3.2
        */
        virtual void DoCancel();
        
        /**
        * From CActive
        * @since S60 3.2
        */
		virtual TInt RunError(TInt aError);               

    protected:
    
        /**
        * @param aMsvSession
        * @param aObserverRequestStatus
        * @param aReporter
        * @param aServiceId
        * @since S60 3.2
        */
        CImumMBoxServiceDeletion(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aServiceId );

        /**
        * Second phase Constructor
        * @since S60 3.2
        */
        void ConstructL();

        /**
        * Kick off an entry deletion and go active.
        * @since S60 3.2
        */
        void BeginDeleteOperationL();

        /**
        * Kick off an operation to remove any
        * pre-created messages for the SMTP service and go active.
        * @since S60 3.2
        */
        void RemovePreCreatedMessagesL();

        /**
        *
        * @since S60 3.2
        */
        void DoRunL();

        
        /**
        * Signals observer that we're done
        * @since S60 3.2
        */
        void CompleteObserver();

        
        /**
        * Removes the account from the Central Repository
        * @since S60 3.2
        */
        void BeginRemoveAccountDeleteOpL();
        
        
        /**
        * 
        * @since S60 3.2
        */
        void DoRemoveMailboxL();
        
    private:
    	enum TState
            { EDoPrompts,
              EDeleteService,
              EDeletePreCreatedMsgs,
              EStateComplete };
              
        MMsvProgressReporter& 			iReporter;        
        TState 							iState;
        CMsvOperation* 					iSubOperation;
        TBuf8<KMsvProgressBufferLength> iProgressBuf;
        TMsvId 							iServiceId;
        TMsvId                          iSMTPServiceId;
        HBufC* 							iProgressString;
        TUid                            iMtm;
        TBool                           iCanProceed;                
        CImumInternalApiImpl*           iEmailApi;
    };


#endif //C_IMUMMBOXSERVICEDELETION_H