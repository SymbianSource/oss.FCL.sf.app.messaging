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
* Description:  ImumInHealthServicesImpl.h
*
*/


#ifndef CIMUMINHEALTHSERVICESIMPL_H
#define CIMUMINHEALTHSERVICESIMPL_H

// INCLUDES
#include <e32base.h>
#include "ImumInHealthServices.h"


// CONSTANTS
enum TIMDErrors
    {
    EIMDNoError = 0,                        // 0
    EIMDUnknownMailbox,
    EIMDNoMemory,
    EIMDNotMailMtmId,
    EIMDGetEntryFailure,
    EIMDImap4AccountFailure,
    EIMDImap4AccountIdNotMatching,
    EIMDImap4RelatedEntryIdNotMatching,
    EIMDPop3AccountFailure,
    EIMDPop3AccountIdNotMatching,
    EIMDPop3RelatedEntryIdNotMatching,      // 10
    EIMDSmtpAccountFailure,
    EIMDSmtpAccountIdNotMatching,
    EIMDSmtpRelatedEntryIdNotMatching,
    EIMDSyncMlSettingsLoadFailure,          // Deprecated
    EIMDSyncMlMailboxIdNotMatching,         // Deprecated
    EIMDSyncMlMtmIdNotMatching,             // Deprecated
    EIMDStoringFailureWithIMEI,
    EIMDMismatchingIMEI,
    EIMDMissingIMEI,
    EIMDMailboxArrayCreationFailure,        // 20
    EIMDFlagCombinationNotSupported
    };

// Maximum length of IMEI code
const TInt KIMAMaxPhoneIdLength = 50;

// Mail for Exchange MTM Uid
const TInt KImumMtmMailForExchangeUidValue  = 0x1020695b;
const TUid KImumMtmMailForExchangeUid       = { KImumMtmMailForExchangeUidValue };

// MACROS
// FORWARD DECLARATIONS
class CImumInternalApiImpl;
class MImumInMailboxUtilities;
class CImumMboxManager;
class CImumMboxSettingsCtrl;
class CEmailAccounts;
class CImumMboxDataExtension;

// DATA TYPES

/**
*
*  @lib
*  @since S60 3.0
*/
class CImumInHealthServicesImpl : public CBase, public MImumInHealthServices
    {
    public:  // Constructors and destructor

        /**
        * Creates object from CImumInHealthServicesImpl and leaves it to cleanup stack
        * @since S60 3.0
        * @return, Constructed object
        */
        static CImumInHealthServicesImpl* NewLC(
            CImumInternalApiImpl& aMailboxApi );

        /**
        * Create object from CImumInHealthServicesImpl
        * @since S60 3.0
        * @return, Constructed object
        */
        static CImumInHealthServicesImpl* NewL(
            CImumInternalApiImpl& aMailboxApi );

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CImumInHealthServicesImpl();

    public: // New functions

        /**
        *
        * @since S60 3.0
        */
        TIMDErrors StoreIMEIToMailbox( const TMsvId aMailboxId ) const;

    public: // Functions from base classes

        /*
        * From MImumInHealthServices. Returns array of fully operating mailboxes
        * @since S60 3.0
        * @param aMailboxArray Return parameter for array of mailboxes
        * @param aFlags Flags indicates the protocols to be included in array
        * @param aClearArray By default, the array is cleared before use
        * @return KErrNone, when array has been constructed successfully
        */
        TInt GetMailboxList(
            RMailboxIdArray& aHealthyMailboxArray,
            const TInt64 aFlags = EFlagSetHealthyReceiving,
            const TBool aClearArray = ETrue ) const;

        TInt GetHealthyMailboxList(
            RMailboxIdArray& aHealthyMailboxArray,
            const TInt64 aFlags = EFlagSetHealthyReceiving,
            const TBool aClearArray = ETrue ) const;

        /**
        * From MImumInHealthServices. Veriefies the mailbox condition
        * @since S60 3.0
        * @param aMailboxId The Id of the mailbox for the check
        * @return ETrue/EFalse according to condition
        */
        TBool IsMailboxHealthy(
            const TMsvId aMailboxId ) const;

        /**
        * Searches and destroys unhealthy mailboxes
        * @since S60 3.0
        */
        void CleanupUnhealthyMailboxes() const;

    protected:  // Constructors

        /**
        *
        * @since S60 3.0
        */
        CImumInHealthServicesImpl(
            CImumInternalApiImpl& aMailboxApi );

        /**
        *
        * @since S60 3.0
        */
        void ConstructL();

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions

        /**
        *
        * @since S60 3.0
        */
        TIMDErrors StoreIMEIToEntry( const TMsvId aMailboxId ) const;

        /**
        *
        * @since S60 3.0
        */
        void StoreIMEIToEntryL( const TMsvId aMailboxId ) const;

        /**
        *
        * @since S60 3.0
        */
        TIMDErrors GetIMEIFromTheEntry(
            const TMsvId aEntryId,
            TDes& aIMEI ) const;

        /**
        *
        * @since S60 3.0
        */
        TIMDErrors GetIMEIFromTheEntry(
            const TMsvEntry& aEntry,
            TDes& aIMEI ) const;

        /**
        *
        * @since S60 3.0
        */
        void GetIMEIFromThePhoneL();

        /**
        *
        * @since S60 3.0
        */
        TIMDErrors VerifyMailboxHealth( const TMsvId aMailboxId ) const;

        /**
        *
        * @since S60 3.0
        */
        TIMDErrors VerifyMailboxHealth(
            const TMsvEntry& aPrimaryEntry,
            const TMsvEntry* aSecondaryEntry ) const;

        /**
        *
        * @since S60 3.0
        */
        TIMDErrors MatchEntryWithSettings(
            const TMsvEntry& aMailbox ) const;

        /**
        *
        * @since S60 3.0
        */
        TIMDErrors VerifyImap4EntryWithSettings(
            const TMsvEntry& aMailbox ) const;

        /**
        *
        * @since S60 3.0
        */
        TIMDErrors VerifyPop3EntryWithSettings(
            const TMsvEntry& aMailbox ) const;

        /**
        *
        * @since S60 3.0
        */
        TIMDErrors VerifySmtpEntryWithSettings(
            const TMsvEntry& aMailbox ) const;

        /**
        *
        * @since S60 3.0
        */
        TIMDErrors VerifyEntryIMEI( const TMsvEntry& aMailbox ) const;

        /**
        *
        * @since S60 3.0
        */
        void AppendToArray(
            const CMsvEntrySelection& aSelection,
            RMailboxIdArray& aMailboxArray,
            const TBool aAppendHealthy ) const;

        /**
        *
        * @since S60 3.0
        */
        void DoGetMailboxList(
            RMailboxIdArray& aMailboxArray,
            const TMuiuFlags& aFlags ) const;

        /**
        *
        * @since S60 3.0
        */
        void CreateMailboxListL(
            CMsvEntrySelection*& aPopSel,
            CMsvEntrySelection*& aImapSel ) const;

        /**
        *
        * @since S60 3.0
        */
        void DoCleanup( const RMailboxIdArray& aMailboxArray ) const;


        /**
        * Check if the mailbox type matches required mailbox
        * @since S60 3.1
        * @param aEntry, suspected mailbox entry
        * @param aFlags, controller flags (see TIMDFlags)
        * @return ETrue, when correct mailbox is in question
        * @return EFalse, when incorrect mailbox is in question
        */
        TBool IsRequiredMailbox(
            const TMsvEntry& aEntry,
            const TMuiuFlags& aFlags ) const;

        /**
        * Takes the provided flags to use, or uses default
        * @since S60 3.0
        * @param aFlags Flags to be used
        * @return Flags to be used for controlling
        */
        TMuiuFlags PrepareFlags( const TInt64 aFlags ) const;

        /**
        *
        * @since S60 3.0
        */
        TIMDErrors GetMailboxEntry(
            const TMsvId aMailboxId,
            TMsvEntry& aMailboxEntry ) const;

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        enum TFlags
            {
            EFlagImap4 = 0,
            EFlagPop3,
            EFlagSmtp,
            EFlagSyncMl,    // Deprecated
            EFlagHealthy,
            EFlagOtherService,
            };

        // Message server session
        CImumInternalApiImpl&       iInternalApi;
        // Pointer to extended mail settings
        CImumMboxDataExtension*  iExtendedSettings;
        // IMEI code of the phone
        TBuf<KIMAMaxPhoneIdLength>  iIMEI;
        // RootEntry
        CMsvEntry*                  iRoot;
        // Shortcut to utilities
        MImumInMailboxUtilities*    iUtils;
    };

#endif //  CIMUMINHEALTHSERVICESIMPL_H


