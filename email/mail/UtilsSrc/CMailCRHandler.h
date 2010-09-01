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
* Description:  Central repository handler for mail
*
*/



#ifndef CMAILCRHANDLER_H
#define CMAILCRHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <cenrepnotifyhandler.h>
#include <s32std.h>
#include "MailVariant.hrh"

// FORWARD DECLARATIONS
class CRepository;
class CMsgMailDocument;

// CLASS DECLARATION

/**
*  Central repository handler for mail
*  Sets and observes central repository keys for mail
*
*  @lib msgeditorutils.lib
*  @since Series 60 3.0
*/
class CMailCRHandler :	public CBase,
						public MCenRepNotifyHandlerCallback
    {
    public:  // Constructors and destructor

    	enum ERepository
			{
			EMailRepository = 0,
			ECommonUiRepository,
			EMuiuVariationKeys,
			ERepositoryOutOfBounds
			};


        /**
        * Two-phased constructor.
        * @param aModel key observer.
        */
        IMPORT_C static CMailCRHandler* NewL(CMsgMailDocument *aModel);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMailCRHandler();

    public: // New functions

		/**
		* Fetches the integer from CR with given keys
		* @param aUid Uid to be used
		* @param aKey Which value is to be fetched
		* @param aValue Variable to store fetched value
		* @return KErrNone, when no errors happened
		*/
		IMPORT_C TInt GetCRInt(
			const TUid&     aUid,
			const TUint32   aKey,
			TInt&           aValue ) const;

        /**
        * Sets the integer to CR with given keys
        * @param aUid Uid to be used
        * @param aKey Which value is to be fetched
        * @param aValue Variable that contains value to be stored
        * @return KErrNone, when no errors happened
        */
        IMPORT_C TInt SetCRInt(
            const TUid&     aUid,
            const TUint32   aKey,
            const TInt      aValue ) const;

		/**
		* Fetches the real from CR with given keys
		* @param aUid Uid to be used
		* @param aKey Which value is to be fetched
		* @param aValue Variable to store fetched value
		* @return KErrNone, when no errors happened
		*/
    	IMPORT_C TInt GetCRReal(
	  		const TUid&     aUid,
	  		const TUint32   aKey,
	  		TReal&          aValue ) const;

		/**
        * Sets the real to CR with given keys
        * @param aUid Uid to be used
        * @param aKey Which value is to be fetched
        * @param aValue Variable that contains value to be stored
        * @return KErrNone, when no errors happened
        */
		IMPORT_C TInt SetCRReal(
	       	const TUid&     aUid,
	       	const TUint32   aKey,
    	   	const TReal&    aValue ) const;

		/**
		* Fetches the binary from CR with given keys
		* @param aUid Uid to be used
		* @param aKey Which value is to be fetched
		* @param aValue Variable to store fetched value
		* @return KErrNone, when no errors happened
		*/
       	IMPORT_C TInt GetCRBinary(
	    	const TUid&     aUid,
	    	const TUint32   aKey,
    		TDes8&          aValue ) const;

		/**
        * Sets the integer to CR with given keys
        * @param aUid Uid to be used
        * @param aKey Which value is to be fetched
        * @param aValue Variable that contains value to be stored
        * @return KErrNone, when no errors happened
        */
		IMPORT_C TInt SetCRBinary(
    		const TUid&     aUid,
    		const TUint32   aKey,
    		const TDesC8&   aValue ) const;

		/**
		* Fetches the string from CR with given keys
		* @param aUid Uid to be used
		* @param aKey Which value is to be fetched
		* @param aValue Variable to store fetched value
		* @return KErrNone, when no errors happened
		*/
    	IMPORT_C TInt GetCRString(
		    const TUid&     aUid,
		    const TUint32   aKey,
    		TDes16&         aValue ) const;

		/**
        * Sets the integer to CR with given keys
        * @param aUid Uid to be used
        * @param aKey Which value is to be fetched
        * @param aValue Variable that contains value to be stored
        * @return KErrNone, when no errors happened
        */
    	IMPORT_C TInt SetCRString(
			const TUid&     aUid,
		    const TUint32   aKey,
    		const TDesC16&  aValue ) const;

		/**
        * Sets the notifyhandler on specific key
        * @param aObserver observer
        * @param aRepository repository to be watched. See ERepository
        * @param aKeyType keytype to be watched 
        * @param aKeyValue which uid is watched
        */
		IMPORT_C void SetNotifyHandlerL(
			MCenRepNotifyHandlerCallback* aObserver,
			CMailCRHandler::ERepository aRepository, 
			CCenRepNotifyHandler::TCenRepKeyType aKeyType, 
			TUint32 aKeyValue );

		/**
		* Fetches all parameters from central repository and initializes model
        */
		IMPORT_C void UpdateModelFromCentralRepositoryL();
				
		/**
		* inline
		* MailAdditionalHeaders
		* @return ETrue if Additional Bcc header field is supported.
		*/
        inline TBool MailAdditionalHeaders() const;	
		
		/**
		* inline
		* MailAttachmentField
		* @return ETrue if Attachment field is supported.
		*/
        inline TBool MailAttachmentField() const;
            		
		/**
		* inline
		* MailEncodingSetting
		* @return ETrue if EncodingSetting is supported.
		*/
        inline TBool MailEncodingSetting() const;
        
		/**
		* inline
		* MailCopyMoveSupport
		* @return ETrue if MailCopyMove is supported.
		*/
        inline TBool MailCopyMoveSupport() const; 
        
		/**
		* inline
		* MailInsertToolBar
		* @return ETrue if Insert toolbar is supported.
		*/
        inline TBool MailInsertToolBar() const;
                    
    public: // Functions from base classes

        /**
        * From MCentRepNotifyHandlerCallback.
	    */
        void HandleNotifyInt(
			TUint32 aId, TInt aNewValue );

        /**
		* From MCentRepNotifyHandlerCallback.
	    */
        void HandleNotifyReal(
			TUint32 aId, TReal aNewValue);

        /**
		* From MCentRepNotifyHandlerCallback.
	    */
        void HandleNotifyString(
			TUint32 aId, const TDesC16& aNewValue);

        /**
		* From MCentRepNotifyHandlerCallback.
	    */
        void HandleNotifyBinary(
			TUint32 aId, const TDesC8& aNewValue);

        /**
		*   From MCentRepNotifyHandlerCallback.
		*/
		void HandleNotifyError(
			TUint32 aId, TInt aError, CCenRepNotifyHandler* aHandler );

		/**
        *   From MCentRepNotifyHandlerCallback.
        */
        void HandleNotifyGeneric( TUint32 aId );

		/**
        * Through this method the Property subscribers notify of 
        * Property changes.
        * @param aCategory The category of the property to be observed
		* @param aKey The subkey of the property to be observed
		*/
        void HandlePropertyChangedL( const TUid& aCategory, TInt aKey );



    private: // private functions

    	/**
		* Gets the CR client according to Uid to be used
		* @param aUid Uid to be used
		* @param aClient Variable to client shall be stored
		* @return KErrNone, when succesful
		*/
		TInt GetClientForUid(
			const TUid&		aUid,
			CRepository*&  	aClient ) const;

    private: //private constructors

        /**
        * C++ default constructor.
        */
        CMailCRHandler(CMsgMailDocument* aModel);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

    // a pointer to the model (document)
	CMsgMailDocument* iModel;

	// A CR session & notifier for mail
    RPointerArray<CRepository> iSessions;
    RPointerArray<CCenRepNotifyHandler> iSettingsNotifyHandlers;
    // Mail UI local variation flags
    TInt iMailUiLocalVariationFlags;
    // Muiu variation flags
    TInt iMuiuVariationFlags;
    // MMS variation flags
    TInt iMmsVariationFags;    
    };

// Inline functions
#include "CMailCRHandler.inl"

#endif      // CMAILCRHANDLER_H

// End of File
