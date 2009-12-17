/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     Postcard application's interface to contacts (Phonebook2 and
*     virtual phonebook)
*
*/



#ifndef __POSTCARDCONTACT_H__
#define __POSTCARDCONTACT_H__

#include <e32base.h>
#include <AiwCommon.h>
#include <MVPbkFieldType.h>

#include "Postcard.hrh"
#include "PostcardPanic.h"

class CAiwServiceHandler;
class CAiwGenericParamList;
class CVPbkContactStoreUriArray;
class CContactMatcher;
class MVPbkStoreContact;

/**
 * Postcard contact
 */
class CPostcardContact : public CBase, 
                         public MAiwNotifyCallback, 
                         public MAknInputBlockCancelHandler
    {
    public:  // constants

    enum TLocation // address location
        {
        ELocationPreferred,
        ELocationHome,
        ELocationWork
        };
    static const TInt KNumLocations = ELocationWork + 1;

    enum TAddressField // address field
        {
        EAddressFieldStreet,
        EAddressFieldExtendedAddress,
        EAddressFieldPostalCode,
        EAddressFieldLocality,
        EAddressFieldRegion,
        EAddressFieldCountry
        };
    static const TInt KNumAddressFields = EAddressFieldCountry + 1;

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @return pointer to created CPostcardContact
        */
        static CPostcardContact* NewL( RFs& aFs );

        /**
         * Destructor.
         */
        ~CPostcardContact();

    public:

        /**
        * Checks if the contact refers to a valid contact (contact was found)
        */
        inline TBool IsValid() const;

        /**
        * Checks if the contact includes any of Extended Info, Street, City,
        * Zip, Region or Country fields with location aLocation.
        * @param aLocation Location Preferred/Home/Work to check
        * @return ETrue if the contact has address information for the location
        */
	    TBool HasLocationL(TLocation aLocation) const;

        /**
        * Creates a contact string of aContactItem's address field with
        * location aAddressLocation 
        * (in practise "<street><extended><zip><city><region><country>")
        * @param aBuffer Contains the contact string on return
        * @param aAddressLocation Location Preferred/Home/Work to get
        */
        void GetContactStringL(TDes& aBuffer, TLocation aAddressLocation) const;

        /**
        * Get address field from a contact
        * @param aLocation Location Preferred/Home/Work to get
        * @param aAddressField Address field to get
        * @return Address field text or "empty" descriptor
        */
        HBufC* GetAddressFieldLC(TLocation aLocation,
            TAddressField aAddressField) const;

        /**
        * Get name from a contact. Uses Phonebook2 name formatter.
        * @return Contact name text or "empty" descriptor
        */
        HBufC* GetNameLC() const;

        /**
        * Fetch contact from using Phonebook2 UI services
        */
        void FetchContactL();

        /**
        * Fetch contact from a contact link in a file
        * @param aFile File containing a contact link packed into a descriptor
        */
        void FetchContactL(RFile& aFile);

        /**
        * Convert from TPostcardControls to TAddressField
        * @param aControlId Control ID to convert
        * @return Address field
        */
        static inline TAddressField ControlIdToAddrField(TInt aControlId);

    public: // Functions from base classes

        /**
         * from MAiwNotifyCallback
         * Handles notifications caused by an asynchronous AIW commands
         */
        TInt HandleNotifyL(TInt aCmdId, TInt aEventId,
            CAiwGenericParamList& aEventParamList,
            const CAiwGenericParamList& aInParamList);

        /**
        * from MAknInputBlockCancelHandler
        * Handles cancellation of AIW call to phonebook.
        */
        void AknInputBlockCancel();

    private:	// new functions

        /**
         * Handles asynchronous AIW command completed event notification
         * @param aEventParamList Event parameters
         */
        void HandleAiwEventCompletedL(CAiwGenericParamList& aEventParamList);

        /**
        * Convert location to Virtual Phonebook field type parameter
        * @param aLocation Location to convert
        * @return Field type parameter
        */
        static inline TVPbkFieldTypeParameter LocToFieldTypeParam(
            TLocation aLocation);

        /**
        * Convert address field to Virtual Phonebook subfield type
        * @param aAddressField Address field to convert
        * @return Subfield type
        */
        static inline TVPbkSubFieldType AddrFieldToSubFieldType(
            TAddressField aAddressField);

        /**
        * Initializes AIW single item contact fetch parameter list
        * @param aParamList Parameter list to initialize with parameters
        */
        void InitAiwContactFetchParamL(
            CAiwGenericParamList& aParamList) const;

        /**
        * Read a field from a contact
        * @param aFieldType Virtual Phonebook field type
        * @param aSubFieldType Virtual Phonebook subfield type
        * @param aFieldTypeParameter Virtual Phonebook field type parameter
        * @return Contact field text or "empty" descriptor
        */
        HBufC* GetContactFieldLC( TVPbkFieldTypeName aFieldType,
            TVPbkSubFieldType aSubFieldType,
            TVPbkFieldTypeParameter aFieldTypeParameter) const;

    private:  // constructors
	    CPostcardContact( RFs& aFs );

        /**
        * EPOC constructor
        */
        void ConstructL();

    private:    // data
        // Table to convert from TLocation to virtual phonebook field
        // type parameter
        static const TVPbkFieldTypeParameter
            iLocToFieldTypeParam[KNumLocations];

        // Table to convert from TAddressField to virtual phonebook subfield
        // type
        static const TVPbkSubFieldType
            iAddrFieldToSubFieldType[KNumAddressFields];

        // Table to convert from TPostcardControls to TAddressField
        static const TAddressField
            iControlIdToAddrField[KNumAddressFields];

        CAiwServiceHandler*         iAiwServiceHandler; // for contact fetch
        CVPbkContactStoreUriArray*  iUriArray;          // opened contact stores
        CContactMatcher*            iContactMatcher;    // contact matcher
        MVPbkStoreContact*          iStoreContact;      // contact selected by user
        CActiveSchedulerWait        iActSchedWait;      // to hide the asynchronity
        TBool                       iShutdown;
        
        RFs&                        iFs;
     };

#include "PostcardContact.inl"

#endif // __POSTCARDCONTACT_H__

// End of File
