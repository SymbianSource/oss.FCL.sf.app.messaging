/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MMsgMailOpObserver
*
*/



#ifndef CMSGMAILBASEOP_H
#define CMSGMAILBASEOP_H

//  INCLUDES
#include <e32base.h>
#include <AknWaitDialog.h>

// DATA TYPES

// FORWARD DECLARATIONS
class CMsgMailEditorDocument;
class CMsgMailBaseOp;

// CLASS DECLARATION

/**
* Observer interface for operations.
*/
NONSHARABLE_CLASS( MMsgMailOpObserver )
    {    
    public:
        /**
        * Method for observing operation completion. Intentionally
        * non-leaving since any problem in observer should not affect the
        * operation.
        * @param aOp operation instance reference
        * @param aResult result code
        */
        virtual void HandleOpCompleted( const CMsgMailBaseOp& aOp,
                                        TInt aResult ) = 0;    
    };

/**
* Base class for state machine -based long-running operations.
* Operations are divided to steps by the implementing subclass, and
* these steps are performed as individual CActive requests. The requests
* may either be truly asynchronous, or self-completing synchronous steps.
*
* Due to existing tight coupling with MsgMailEditorAppUi and UI
* in general the subclass may need to delegate some tasks to the UI via
* MMsgMailOpDelegate interface.
*
* Two alternative observation mechanisms for operations are available:
* either via MMsgMailOpObserver interface or using TRequestStatus completion.
*
* The base class does not cancel the operation in it's destructor
* since subclass destructors are called before base class destructor
* and that would lead to access violations. Subclasses must call Cancel()
* by themselves in their destructors if they want to cancel in destruction.
*/
NONSHARABLE_CLASS( CMsgMailBaseOp ) : public CActive, 
                                      public MProgressDialogCallback 
    {
    protected:
    
        // subclasses add their own states
        enum TState
            {
            EIdleState, // initial and final state of operation
            EFirstFreeState // a marker for subclasses, not a real state
            };
    
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        ~CMsgMailBaseOp();

    public: // API
            
        /**
        * Method for the class user to start the entire operation. Possibly
        * already ongoing operation is cancelled.
        * @param aObserver observer which is notified when operation completes
        */
        void StartOp( MMsgMailOpObserver& aObserver );
        
        /**
        * Method for the class user to start the entire operation. Possibly
        * already ongoing operation is cancelled.
        * @param aStatus request status, completed along with the operation
        */
        void StartOp( TRequestStatus& aStatus );

    protected: // From CActive

        // From CActive
        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );
    
    protected: // From MProgressDialogCallback
        /**
         * Callback method is called when a dialog is dismissed.
         * @param aButtonId Cancel button id.
         */
        void DialogDismissedL( TInt aButtonId );
        
    protected:  // Implementation
    
        /**
        * Template method for implementing state switching logic.
        */
        virtual void SetNextState() = 0;
        
        /**
        * Template method for handling action for latest state transition.
        */
        virtual void HandleStateActionL() = 0;
        
        /**
        * Template method for handling operation cancellation. This is called
        * from DoCancel to allow subclass to cancel any outstanding request.
        */
        virtual void HandleOperationCancel() = 0;
        
        /**
        * Optional template method for handling an error occurred during
        * current state action. This method is called from RunError().
        * Return value determines whether the entire operation is continued
        * (ETrue returned) or stopped (EFalse returned). The base class
        * implementation always returns EFalse.
        * @param aError error code
        * @retval ETrue if error was handled and operation can continue
        */
        virtual TBool HandleStateActionError( TInt aError );
        
        /**
        * Helper method for completing non-asynchronous steps and
        * continuing with next iteration. If action is completed with an
        * error code, then RunError will get executed.
        * @param aError completion code, by default KErrNone
        */
        void CompleteStateAction( TInt aError = KErrNone );
        
        /**
        * Helper method for starting a wait note.
        * @param aTextResourceId wait note text
        * @param aDelayOff wait note delay on/off
        * @param aResourceId wait dialog resource
        */
        void StartWaitNoteL( TInt aTextResourceId,
                             TBool aDelayOff,
                             TInt aResouceID );
        
        /**
        * Helper method for stopping a wait note. If not called, then
        * wait note will be closed when operation is stopped.
        */
        void StopWaitNote();

    private:  // Implementation
        
        /**
        * Resets state machine, closes and waits possibly open wait note.
        * Calls NotifyObservers if wait note is not visible, otherwise the
        * operation handling continues in DialogDismissedL.
        * @param aError error code
        */
        void CompleteOperation( TInt aError );

        /**
        * Notifies observer with the given error code
        * @param aError error code
        */
        void NotifyObservers( TInt aError );
        
    protected:
    
        /**
        * Constructor.
        * @param aDocument Editor's document reference
        */
        CMsgMailBaseOp( CMsgMailEditorDocument& aDocument );                     

    protected:    // Data
    
        // Wait dialog, own
        CAknWaitDialog* iWaitDialog;
    
        // Editor's document, not own
        CMsgMailEditorDocument& iDocument;   
        
        // Operation observer, not own
        MMsgMailOpObserver* iObserver;
        
        // Operation obeserver's request status, not own
        TRequestStatus* iExtRequestStatus;

        // Current state
        TInt iState;
        
        // Pending operation result (waiting wait dialog).
        TInt iWaitingError;
    };

#endif      // CMSGMAILBASEOP_H

// End of File
