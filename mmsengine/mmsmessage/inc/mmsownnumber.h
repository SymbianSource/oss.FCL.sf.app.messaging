/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   checks if a given number is terminal's own number
*
*/




#ifndef MMSOWNNUMBER_H
#define MMSOWNNUMBER_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

#include <MVPbkContactFindObserver.h>
#include <MVPbkContactStoreListObserver.h>
#include <MVPbkSingleContactOperationObserver.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RFs;

class CVPbkContactManager;
class MVPbkContactStoreList;
class MVPbkContactOperationBase;
class MVPbkStoreContact;

// CLASS DECLARATION

/**
*  Own numer matching
*
*  @lib mmsmessage.lib
*  @since Series 60 3.1
*/

NONSHARABLE_CLASS( CMmsOwnNumber ) : public CActive,
    public MVPbkContactFindObserver,
    public MVPbkContactStoreListObserver,
    public MVPbkSingleContactOperationObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMmsOwnNumber* NewL( RFs* aFs );
        
        /**
        * Destructor.
        */
        virtual ~CMmsOwnNumber();

    public: // New functions
        
        /**
        * Starts searching for the number.
        * @since Series 60 3.1
        * @param aNumberList array of number from which to search own number
        * @param aFs file system pointer
        * @return the index of user's number in the array or error code
        */
        
        TInt StartL( const CDesCArray& aNumberList );

    public: // Functions from base classes

        /**
        * From MVPbkContactFindObserver
        * Called when find is complete. Callee takes ownership of the results.
        * In case of an error during find, the aResults may contain only 
        * partial results of the find.
        * @since Series 60 3.1
        *
        * @param aResults Array of contact links that matched the find.
        *                 Callee must take ownership of this object in
        *                 the end of the function, ie. in case the function
        *                 does not leave.
        */
        void FindCompleteL(MVPbkContactLinkArray* aResults);
        
        /**
        * From MVPbkContactFindObserver
        * Called in case the find fails for some reason.
        * @since Series 60 3.1
        * 
        * @param aError One of the system wide error codes.
        */
        void FindFailed(TInt aError);
        
        /**
        * From MVPbkContactStoreListObserver
        * Called when the opening process is complete, ie. all stores have been reported
        * either failed or successful open.
        */
        void OpenComplete();
        
        /**
        * From MVPbkContactStoreObserver
        * Called when a contact store is ready to use.
        */
        virtual void StoreReady(MVPbkContactStore& aContactStore);

        /**
        * From MVPbkContactStoreObserver
        * Called when a contact store becomes unavailable.
        * Client may inspect the reason of the unavailability and decide whether or not
        * it will keep the store opened (ie. listen to the store events).
        * @param aContactStore The store that became unavailable.
        * @param aReason The reason why the store is unavailable.
        *                This is one of the system wide error codes.
        */
        virtual void StoreUnavailable(MVPbkContactStore& aContactStore, TInt aReason);

        /**
        * From MVPbkContactStoreObserver
        * Called when changes occur in the contact store.
        * @see TVPbkContactStoreEvent
        *
        * @param aStoreEvent Event that has occured.
        */
        virtual void HandleStoreEventL(
                MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent);
                
        /**
        * From MVPbkSingleContactOperationObserver
        * Called when operation is completed.
        *
        * @param aOperation    the completed operation.
        */
        virtual void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact);

        /**
        * From MVPbkSingleContactOperationObserver
        * Called if the operation fails.
        *
        * @param aOperation    the failed operation.
        * @param aError        error code of the failure.
        */
        virtual void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError);

        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CMmsOwnNumber();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RFs* aFs );
        
        /**
        * From CActive: Cancel current operation.
        */
        void DoCancel();
        
        /**
        * From CActive: Active object completion
        */
        void RunL();
        
        /**
        * From CActive: Active object error
        */
        TInt RunError( TInt aError );
        
        /*
        * Search for contact
        */
        void SearchL();
        
        /*
        * if more than one contact found, check for duplicate entries
        */
        void CheckDuplicatesL();
        

    public:     // Data
    
    protected:  // Data

    private:    // Data
        
        enum TMmsOwnNumberStates
            {
            EMmsOwnNumberIdle,
            EMmsOwnNumberOpening,
            EMmsOwnNumberSearching,
            EMmsOwnNumberCheckingDuplicates,
            EMmsOwnNumberClosing,
            EMmsOwnNumberDone
            };
        
      	CActiveSchedulerWait iActiveSchedulerWait;
      	const CDesCArray* iNumberList; // caller's number list
      	TInt iIndex; // index of own number in array
      	CVPbkContactManager* iContactManager;
      	RFs* iFs; // file manager
      	TInt iNumberToMatch; // array index for the number to be matched
      	TInt iDigitsToMatch; // the number of digits to match in a phone number
      	TInt iContactToMatch; // array index of contact for exact matching
      	MVPbkContactStoreList* iContactList; // just a reference, not allocated memory
      	TBool iMatch; // tells if the contact was an exact match
      	TInt iNumberOfStores; // number of stores to search
      	TInt iNumberOfOpenStores; // number of stores we managed to open
      	TInt iState; // state machine state
      	MVPbkContactLinkArray* iResultArray;
      	MVPbkContactOperationBase* iOperation;
      	MVPbkStoreContact* iContact;


    };
    
#endif      // MMSOWNNUMBER_H   
            
// End of File
