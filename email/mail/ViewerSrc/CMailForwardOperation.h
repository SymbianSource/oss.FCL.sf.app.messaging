/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles Message forwarding
*
*/



#ifndef CMAILFORWARDOPERATION_H
#define CMAILFORWARDOPERATION_H

//  INCLUDES
#include <e32cmn.h>

#include "cmailoperation.h"
#include "MMessageLoadObserver.h"
// FORWARD DECLARATIONS
class CMsgMailViewerDocument;
class MMsvProgressReporter;


// CLASS DECLARATION

/**
*  Forward operation
*
*  @since S60 3.0
*/
class CMailForwardOperation : public CMailOperation, MMessageLoadObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aReporter UI progress reporter.
        * @param aDocument Mail viewer document.
        * @param aObserverRequestStatus completion status.
        * @parma aSuspend ETrue if operatino should be suspended.
        */
        static CMailForwardOperation* NewL(
        	MMsvProgressReporter& aReporter,
            CMsgMailViewerDocument& aDocument,
            TRequestStatus& aObserverRequestStatus,
            TBool aSetSuspend );
        
        /**
        * Destructor.
        */
        ~CMailForwardOperation();

    private: // Functions from base classes
	    
	    // From CMailOperation
	    void SuspendL( TBool aSetSuspend );
	    TBool CompletedL( TInt aCompletionCode );
	    
	    // From CMsvOperation
	    const TDesC8& ProgressL();
	    
	    // From CActive
	    void DoCancel();
        void RunL();
	    TInt RunError( TInt aError );
	    
	    // From MMessageLoadObserver
	    void MessageLoadingL( TInt aStatus, CMailMessage& aMessage );
	                           
    private:

        /**
        * C++ constructor.
        */
        CMailForwardOperation( 
            CMsgMailViewerDocument& aDocument,
            TRequestStatus& aObserverRequestStatus );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:  // Implementation
        void CompleteObserver( TInt aStatus );
        void ConnectToServiceL();
        void CheckAttachmentsL();
        void LoadAttachmentsL();
        void FetchAttachmentsL( 
            CMsvEntrySelection& aSelection, TInt aMaxSize );
        void FetchAttachmentsL( CMsvEntrySelection& aSelection );
        void ForwardMessageL();
        void UpdateEntryFlagsL();
        TBool NexStateL();
     //   TBool ShowDRMNotesL();    

    private:    // Data
        // Own: operation to complete
        CMsvOperation* iOperation;
        // Own: attachment selection
        CMsvEntrySelection* iSelection;
        // Ref: mail viewer document
        CMsgMailViewerDocument& iDocument;
        // Ref: UI progress reporter
        MMsvProgressReporter* iReporter;
        // Operation internal state
        TInt iOperationState;        
        
    };

#endif      // CMAILFORWARDOPERATION_H   
            
// End of File
