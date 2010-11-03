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
* Description:  
*     Copies/Moves message store from source drive to target drive
*
*/



#if !defined(__MCESETTINGSMOVEMSGS_H__)
#define __MCESETTINGSMOVEMSGS_H__

//  INCLUDES
#include <e32base.h>
#include <msvstd.h>

// CONSTANTS

const TInt KMsgTransHndlrPriority=0;
const TInt KMsgTransTimerPriority=0;
const TInt KCopyBufSize=0x1000;

// FORWARD DECLARATIONS
class CMceMessageTransHandler;
class CMsvSession;
class CMsvOperation;


// CLASS DECLARATION

class MMceMsgTransHandlerObserver
    {
public:

    virtual void CopyCompleteL(TInt aErr) = 0;

    /**
    * Called when message store transfer is complete
    */
    virtual void TransferCompleteL(TInt aErr)=0;

    /**
    * Called when message store transfer cannot be cancelled
    */
    virtual void DisableCancelL()=0;
    };


//
// CMceMessageTransHandler - active object used to transfer the message store
//
class CEkmsDocument;
class CMceMessageTransHandler : public CActive
    {
    public:
        ~CMceMessageTransHandler();
        static CMceMessageTransHandler* NewL( MMceMsgTransHandlerObserver& aOwner, 
                                                                        CMsvSession* aSession, 
                                                                        TInt aTargetDrive, 
                                                                        TInt aSourceDrive, 
                                                                        TBool aDeleteCopiedStore );

        /**
        * Sets operation
        * Must only be called once during the lifetime of a CMsvSingleOpWatcher
        * object.
        */
		void SetOperation( CMsvOperation* aOperation );

        /**
        * Returns reference to the operation
        */
        IMPORT_C CMsvOperation& Operation() const;

        /**
        * Display progress in main thread
        */
        TInt ProgressL() const;
        
        /**
        * Called by the framework if RunL() leaves
        */
        TInt RunError( TInt );

    protected:
        /**
        * Cancels transfer thread
        */
        virtual void DoCancel();

        /**
        * Calls TransferCompleteL and sets message drive on the message server
        */
        virtual void RunL();

    private:

        /**
        * C++ constructor
        */
		CMceMessageTransHandler( MMceMsgTransHandlerObserver& aOwner, 
		                                            CMsvSession* aSession,
		                                            TInt aTargetDrive, 
		                                            TInt aSourceDrive, 
		                                            TBool aDeleteCopiedStore );

        /**
        * Create transfer thread and delete thread
        */
        void ConstructL();        

        TInt ProgressErrorL() const;
        
        enum TTransHandlerState
                {
                EStateCopying,
                EStateDeleting,
                };
                
    private:
        MMceMsgTransHandlerObserver& iOwner;
        CMsvSession*            iSession; // not owed by this class.
        CMsvOperation*			iOperation;
		TDriveUnit				iTargetDrive; // The target drive for the copied message store
		TDriveUnit              iSourceDrive; // The source drive of the message store
		TBool                   iDeleteCopiedStore; // True if the contents of the original store is to be deleted
        TTransHandlerState      iState;
    };

#endif // __MCESETTINGSMOVEMSGS_H__

// End of file
