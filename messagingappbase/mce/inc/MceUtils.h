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
*     Mixed class to offer different account functions.
*     Implemented by CMceUi.
*
*/



#ifndef MCEUTILS_H
#define MCEUTILS_H

//  INCLUDES

#include <msvstd.h>     // TMsvId
#include <msvstd.hrh>   // KMsvNullIndexEntryIdValue
#include <msvreg.h>     // THumanReadableName

#include <bldvariant.hrh>
#include <ImumInHealthServices.h>       // MImumInHealthServices
#include <PhCltTypes.h>

// DATA TYPES
enum TMceListType
    {
    EMceListTypeMainView,
    EMceListTypeMessage
    };

// FORWARD DECLARATIONS
class CUidNameArray;
class CMsvSession;
class CImumInternalApi;

// CLASS DECLARATION

/**
*  Mixed class to offer different account functions.
*  Implemented by CMceUi
*/
class MMceAccountManager
    {
    public:
        /**
        * Create new account.
        * @param aType type of the account.
        * @param aOldServiceId id of the service where old settings are copied (this is for 'create from existing')
        * @return ETrue if account created, EFalse otherwise
        */
        virtual TBool CreateNewAccountL(TUid aType, TMsvId aOldServiceId )=0;

        /**
        * Edit account.
        * @param aId id of the account to be edited.
        */
        virtual void EditAccountL(TMsvId aId)=0;

        /**
        * Returns array of accounts of desired type.
        * Callers responsibility to destroy array!
        * @param aType type of the account.
        */
        virtual CUidNameArray* MtmAccountsL( TUid aType )=0;

        /**
        * Can create create new account of aMtm.
        * @param aMtm Mtm uid.
        * @return EFalse if new accounts cannot be created for mtm aMtm.
        */
        virtual TBool CanCreateNewAccountL(TUid aMtm) = 0;

        /**
        * Delete account.
        * @param aId id of the account to be deleted.
        */
        virtual void DeleteAccountL(TMsvId aId) = 0;

        /**
         * Returns session used.
         * @return reference to object of CMsvSession class.
         */
        virtual CMsvSession& Session() = 0;

        /**
         * Returns ETrue if aMtm is installed in the device.
         * @param aMtm: Mtm uid to be checked.
         * @return EFalse if aMtm is not installed, ETrue if it is installed.
         */
        virtual TBool IsPresent( TUid aMtm ) const = 0;

        /**
         * Returns human readable name of mtm.
         * @param aMtm: Mtm uid
         * @return human readable name of mtm.
         */
         virtual THumanReadableName MtmName( TUid aMtm ) const = 0;

    };

/**
* Helper class
* Includes static functions.
*/
class MceUtils
    {
    public:
        /**
        * Returns ETrue if given entry is one of the default folders:
        * Inbox, Outbox, Sent items, Draft, Documents, Templates.
        * @param TMsvId of the entry
        * @return ETrue / EFalse
        */
        static TBool IsEntryFixed( TMsvId aId );

        /**
        * Replaces tab and enter characters by spaces using AknTextUtils::ReplaceCharacters
        */
        static void ReplaceCharacters( TDes& aText );

        /**
        * Gets ir attachment file name and path for the aEntryId.
        * @param aSession: session object
        * @param aEntryId: entry to be checked
        * @param aFileName: filename buffer to be filled
        * @param aFileHandle: handle to the file (aFileName)
        * @param aSize: size of the file (aFileName)
        */
		static void GetIrFilePathL( CMsvSession& aSession, TMsvId aEntryId, TFileName& aFileName, RFile& aFileHandle, TInt& aSize );

        /**
        * Returns ETrue if 'Memory in use' option is enabled in mce/settings               
        */
        static TBool MemoryInUseOptionL( );        
                                                    
        /**
         * Gets the array of valid mailboxes.
         *
         * @since S60 v3.2
         * @param aEmailApi: Ptr to the email API
         * @param aMailboxIdArray: list of valid mailboxes
         * @param aGetImap4: ETrue if the imap mailboxes are to be included
         * @param aGetPop3: ETrue if the pop3 mailboxes are to be included
         * @param aGetSyncML: ETrue if the syncml mailboxes are to be included
         * @param aGetOther: ETrue if the Other (3rd party) mailboxes are to be included
         * @return KErrNone if the list of healthy mailboxes list is found
         */
        static TInt GetHealthyMailboxListL(
            const MImumInHealthServices& aHealthServices,
            MImumInHealthServices::RMailboxIdArray& aMailboxIdArray,
            TBool aGetImap4,
            TBool aGetPop3,
            TBool aGetSyncML,
            TBool aGetOther );           
            
        /**
        * Returns ETrue if phonenumber in entry is valid
        * @since 5.0
        * @param aEntryId of the entry
        * @param aSession: Session to message server
        * @param aPhoneNumber: Phone number
        * @param aEntryDetails: If this is not NULL, 
        * then KMceVisibleTextLength from the 
        * TMsvEntry.iDetails is copied to aEntryDetails buffer
        * @Returns ETrue if phonenumber is valid
        */
        static TBool ValidPhoneNumberL( 
            TMsvId aEntryId,
            CMsvSession& aSession,
            TPhCltTelephoneNumber& aPhoneNumber, 
            TDes* aEntryDetails );

        /**
        * Returns ETrue if phonenumber in entry is valid
        * @since 5.0
        * @param aEntryId of the entry
        * @param aSession: Session to message server
        * @Returns ETrue if phonenumber is valid
        */
        static TBool ValidPhoneNumberL( 
            TMsvId aEntryId,
            CMsvSession& aSession );

        /**
        * Gets phone number from SMS entry
        * @since 5.0
        * @param aEntryId of the entry
        * @param aSession: Session to message server
        * @param aPhoneNumber: Phone number
        */
        static void GetSmsNumberL( 
            TMsvId aEntryId, 
            CMsvSession& aSession,
            TPhCltTelephoneNumber& aNumber );

        /**
        * Gets phone number from MMS entry
        * @since 5.0
        * @param aEntryId of the entry
        * @param aSession: Session to message server
        * @param aPhoneNumber: Phone number
        */
        static void GetMmsNumberL( 
            TMsvEntry& aEntry, 
            CMsvSession& aSession,
            TPhCltTelephoneNumber& aNumber );
            
            
        /**
        * Copies text buffer from aSourceString to 
        * aTargetSting and also adds EEllipsis if needed and 
        * performs AknTextUtils::ReplaceCharacters to copied buffer.
        * @since 5.0
        * @param aTargetString
        * @param aSourceString
        * @param aDescriptionLength
        * @return: none
        */
        static void StripAndAppendStringL(
            TDes& aTargetString, 
            const TDesC& aSourceString, 
            TInt aDescriptionLength );
            
        /**
        * Reads description length value from muiu cenrep file:
        * KCRUidMuiuSettings / KMuiuDescriptionLength
        * If some errors, returns KMceVisibleTextLength.
        * @since 5.0
        * @return iDescription length value. 
        */            
        static TInt DescriptionLength();

        /**
        * Reads description length value from muiu cenrep file:
        * KCRUidMuiuSettings / KMuiuDescriptionLength
        * If some errors, returns KMceVisibleTextLength.
        * @since 5.0
        * @return iDescription length value. 
        * @leave KErrNoMemory
        */            
        static TInt DescriptionLengthL();

        /**
        * Returns ETrue if MTM is build in type 
        * @since 5.0
        * @param aMtmUid
        * @return ETrue/EFalse
        */    
        static TBool IsMtmBuiltIn( TUid aMtmUid );
        
        /**
         * Reads KCRUidNumberGrouping / KNumberGrouping
         * from the cenrep and returns ETrue if number 
         * grouping is enabled. 
         * If reading fails then it returns EFalse
         * @since 5.0
         */
        static TBool NumberGroupingEnabled();

        /**
         * Reads KCRUidNumberGrouping / KNumberGrouping
         * from the cenrep and returns ETrue if number 
         * grouping is enabled. 
         * If reading fails then it returns EFalse
         * @since 5.0
         * @leave if CRepository::NewL leaves.
         */
        static TBool NumberGroupingEnabledL();
            

    };
/**
* Helper class for flags.
*/
class TMceFlags
    {
    public:
        /**
        * constructor
        */
        TMceFlags();

        /**
        * Sets flag on
        * @param aFlag: flag to be set
        */
        inline void SetMceFlag( TUint32 aFlag );

        /**
        * Clear flag on
        * @param aFlag: flag to be cleared
        */
        inline void ClearMceFlag( TUint32 aFlag );

        /**
        * Query for the flag
        * @param aFlag: flag to be checked
        */
        inline TBool MceFlag( TUint32 aFlag ) const;

    private:
        TUint32 iFlags;

    };

#include "MceUtils.inl"

#endif

// End of file
