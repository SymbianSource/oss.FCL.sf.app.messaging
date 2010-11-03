/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VirtualPhonebook support for importing VCard messages
*
*/



#ifndef C_VCARDVPBUTIL_H
#define C_VCARDVPBUTIL_H

#include <e32base.h>
#include <s32file.h>                // for RFile

//Phonebook2 API includes

#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactStoreListObserver.h>
#include <MVPbkBatchOperationObserver.h>

#include "vcardbiocontrol.hrh"

//  FORWARD DECLARATIONS
class CVPbkVCardEng;
class CVPbkContactManager;
class MVPbkStoreContact;
class CPbk2StoreConfiguration;
class MVPbkContactStoreList;
class RFileReadStream;
class MVPbkBaseContactField;
class MVPbkFieldType;
class CVPbkContactStoreUriArray;

NONSHARABLE_CLASS(CVCardVpbUtil):
                    public CBase,
					public MVPbkContactStoreListObserver,
					public MVPbkSingleContactOperationObserver,
					public MVPbkBatchOperationObserver
	{

public:

	//construction and destruction

		static CVCardVpbUtil* NewL();

		// Destructor
		~CVCardVpbUtil();


	// public API

        /**
         * Imports the VCard to both the phone store (for display) and
         * Default store (for storing)
         *
         * @since S60 3.1
         * @param TVCardBCBusinessCardType aVCardType
         *        The type of imported vcard
         * @param aFileHandle The filehandle for the VCard file
         * @return void
         */
        void ImportVCardL( TVCardBCBusinessCardType aVCardType, RFile aFileHandle );

        /**
         * Commits the VCard to store
         *
         * @since S60 3.1
         * @return ETrue if contact was stored, EFalse if not
         */
        TBool CommitVCardToStoreL();

        /**
         * Returns the imported contact data from the
         * Phone default store
         *
         * @since S60 3.1
         * @return const MVPbkStoreContact contact item
         *         Ownership is not transferred
         */
        MVPbkStoreContact* ContactData() const;

        /**
         * Check for contact item fields if they are empty
         * @return ETrue for empty contact item,
         *         EFalse for not empty
         */
        TBool IsContactItemEmpty();

  		/**

    // from base class MVPbkContactStoreListObserver

    	/**
         * Called when the opening process is complete, ie. all stores have been reported
         * either failed or successful open.
         */
        void OpenComplete();

        /**
         * Called when a contact store is ready to use.
         */
        void StoreReady( MVPbkContactStore& aContactStore );

        /**
         * Called when a contact store becomes unavailable.
         * Client may inspect the reason of the unavailability and decide whether or not
         * it will keep the store opened (ie. listen to the store events).
         * @param aContactStore The store that became unavailable.
         * @param aReason The reason why the store is unavailable.
         *                This is one of the system wide error codes.
         */
        void StoreUnavailable( MVPbkContactStore& aContactStore, TInt aReason );

        /**
         * Called when changes occur in the contact store.
         * @see TVPbkContactStoreEvent
         * @param aContactStore The store the event occurred in.
         * @param aStoreEvent   Event that has occured.
         */
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );


    // from base class MVPbkSingleContactOperationObserver

    	/**
         * Called when operation is completed.
         *
         * @param aOperation the completed operation.
         * @param aContact  the contact returned by the operation.
         *                  Client must take the ownership immediately.
         *
         *                  !!! NOTICE !!!
         *                  If you use Cleanupstack for MVPbkStoreContact
         *                  Use MVPbkStoreContact::PushL or
         *                  CleanupDeletePushL from e32base.h.
         *                  (Do Not Use CleanupStack::PushL(TAny*) because
         *                  then the virtual destructor of the M-class
         *                  won't be called when the object is deleted).
         */
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );

        /**
         * Called if the operation fails.
         *
         * @param aOperation    the failed operation.
         * @param aError        error code of the failure.
         */
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );


    // from base class MVPbkBatchOperationObserver

        void StepComplete( MVPbkContactOperationBase& aOperation,
                TInt aStepSize );

        TBool StepFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aStepSize, TInt aError );

        void OperationComplete( MVPbkContactOperationBase& aOperation );

		/**
		 * Returns formatted contents of aField.
		 * @param aField the field to format
		 * @return formatted content of the field.
		 */
        TPtrC FormatFieldContentL(const MVPbkBaseContactField& aField);

private:

    // private construction

        /**
         * The constructor.
         */
        CVCardVpbUtil();

        /// Second phase constructor.
        void ConstructL();


    // private API

        /**
         * Checking whether the default contact store
         * supports all imported fields
         *
         * @return ETrue if all the fields are supported
         *         EFalse if some fields are not supported
         */
    	TBool DefaultStoreSupportsAllFieldsL();

        /**
         * Implementation to import the contact data
         * to phone internal contact database
         *
         * @param aStream    The stream from which data is imported
         */
    	void VCardToPhoneStoreL( RFileReadStream& aStream );

        /**
         * Implementation to import the contact data
         * to phone default contact database
         * this can be eg. SIM store
         *
         ** @param iStream    The stream from which data is imported
         */
    	void VCardToDefaultStoreL( RFileReadStream& aStream );

        /**
         * Function to start the Active Scheduler
         * and to trigger a leave if asynchronius
         * operation returned with a error code
         */
    	void StartSchedulerAndDecideToLeaveL();

		/**
		 * Checking of the field type.
		 * @param aFieldType The field to be checked
		 * @param aResourceId The resource id that it is checked against
		 * @return TBool Field type matching status
 		 */
    	TBool IsFieldTypeL(
            const MVPbkFieldType& aFieldType,
            TInt aResourceId) const;

        TPtr ReallocBufferL(TInt aNewSize);

        /**
         * Panic notifying functionality
         */
    	static void Panic( TInt aReason );
    	TBool StoreFullL();
    	void ShowNoteL( TInt aResourceId, const TDesC& aString );

private: // data

	    /**
         * last error code from asynchronous operations
         */
		TInt iLastError;
		
		/**
         * Current status to take action to observed event
         */    
        TBool iRespondToObserverEvent;

	    /**
         * The type of imported VCard
         */
		TVCardBCBusinessCardType iVCardType;

        /**
         * VirtualPhonebook contact manager API
         * Own.
         */
    	CVPbkContactManager* iContactManager;

        /**
         * VirtualPhonebook VCard Importing engine
         * Own.
         */
    	CVPbkVCardEng* iVCardEngine;

        /**
         * Default contact store for saved contacts
         * Own.
         */
    	MVPbkContactStore* iDefaultContactStore;

        /**
         * Shown contact store ie. phone memory Agenda database
         * This is for display purposes and is not used to save data
         * Own.
         */
    	MVPbkContactStore* iShowContactStore;

        /**
         * Array of imported contacts to show on screen
         * Own.
         */
    	RPointerArray<MVPbkStoreContact> iContactsToShow;

        /**
         * Array of imported contacts to store on device
         * Own.
         */
    	RPointerArray<MVPbkStoreContact> iContactsToStore;

        /**
         * List of all opened contact stores. Note that they might not be succesfully opened.
         * Own.
         */
    	MVPbkContactStoreList* iStoreList;
        
        /**
    	 * Buffer for handling line strings
    	 * Own.
    	 */
        HBufC* iBuffer;

        /**
         * List of all succesfully opened contact stores.
         * Own.
         */
        CVPbkContactStoreUriArray* iAvailableUris;
	};
#endif //C_VCARDVPBUTIL_H
