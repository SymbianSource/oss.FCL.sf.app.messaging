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
 * Description: Operation which performs either copy or deletion of entries from the sim.
 *
 */

#ifndef __SIMMESSAGEDELETECOPYOPERATION_H__
#define __SIMMESSAGEDELETECOPYOPERATION_H__


//  INCLUDES
#include <msvstd.h>  // TMsvId

// FORWARD DECLARATIONS
class CSmsMtmUi;
class CMsvSession;
class CCoeEnv;
class CAknProgressDialog;

// CLASS DECLARATION

/**
* CSimDeleteCopyOperation
* Operation which performs either copy or delete entries from the sim
* Gets array of messages to be copied or deleted and then performs
* copy or delete operation one by one and updates progress dialog
* after every copy or delete operation.
* Does not take ownership of progress note, it should be created
* and deleted by other components.
*/
NONSHARABLE_CLASS( CSimDeleteCopyOperation ) : public CMsvOperation
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor, copying operation
        * @param aMsvSession: reference to object of CMsvSession
        * @param aSmsMtmUi: reference to sms mtm ui object
        * @param aObserverRequestStatus: TRequest of the observing object
        * @param aSelection: pointer to the array of messages to be deleted or copied. 
        *    Note! Copies array so does not take ownership for the array
        * @param aDestination: if of the folder where messages are copied
        * @param aProgressDialog: reference to progress dialog, which is updated
        * @param aCoeEnv: pointer to CCoeEnv for StringLoader
        */
        static CSimDeleteCopyOperation* NewL(
            CMsvSession& aMsvSession, 
            CSmsMtmUi& aSmsMtmUi,
            TRequestStatus& aObserverRequestStatus, 
            CMsvEntrySelection* aSelection, 
            TMsvId aDestinationId,
            CAknProgressDialog& aProgressDialog, 
            CCoeEnv* aCoeEnv );

        /**
        * Constructor, deleting operation
        * @param aMsvSession: reference to object of CMsvSession
        * @param aSmsMtmUi: reference to sms mtm ui object
        * @param aObserverRequestStatus: TRequest of the observing object
        * @param aSelection: pointer to the array of messages to be deleted or copied. 
        *    Note! Copies array so does not take ownership for the array
        * @param aProgressDialog: reference to progress dialog, which is updated
        * @param aCoeEnv: pointer to CCoeEnv for StringLoader
        */
        static CSimDeleteCopyOperation* NewL(
            CMsvSession& aMsvSession, 
            CSmsMtmUi& aSmsMtmUi,
            TRequestStatus& aObserverRequestStatus, 
            CMsvEntrySelection* aSelection, 
            CAknProgressDialog& aProgressDialog, 
            CCoeEnv* aCoeEnv );

        /**
        * Destructor.
        */
        virtual ~CSimDeleteCopyOperation();

    public: // New functions
        
        /**
        * Sets operation to be cancelled after next copy or delete
        * operation is completed.
        */
        void CancelAfterCurrentCompleted();

    public: // Functions from base classes

        /**
        * from CMsvOperation
        */
        virtual const TDesC8& ProgressL();

    public: // New functions

        /** 
        * Starts next copy or move operation, traps MakeNextDeleteCopyL
        */
        void DeleteCopyNext();

        /** 
        * Starts next copy or move operation
        */
        void MakeNextDeleteCopyL();

    private:  // Functions from base classes

        /** 
        * From CActive
        */
        void DoCancel();
        /** 
        * From CActive
        */
        void RunL();
        
    private:

        /**
        * Constructor.
        */
        CSimDeleteCopyOperation(
            CMsvSession& aMsvSession,
            CSmsMtmUi& aSmsMtmUi,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aDestinationId,
            CAknProgressDialog& aProgressDialog,
            TBool aCopying,
            CCoeEnv* aCoeEnv );

        /**
        * Constructor.
        */
        void ConstructL(
            CMsvEntrySelection* aSelection );


    private:    // Data
        CMsvOperation*      iOperation;
        CMsvEntrySelection* iEntrySelection;
        TMsvId              iDestinationId;
        TBool               iCopying;
        TInt                iCurrentEntryIndex;
        TInt                iEntryCount;
        TBool               iCanceling;
        CSmsMtmUi&          iSmsMtmUi;
        CAknProgressDialog& iProgressDialog;
        CCoeEnv*            iCoeEnv;
        TBuf8<1>            iBlank;
        TMsvId              iSmsServiceId;
        HBufC*              iNoteString;
    };

#endif      
            
// End of File
