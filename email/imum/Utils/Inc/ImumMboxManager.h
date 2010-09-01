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
*       Encapsulates saving and removing email accounts
*
*/


#ifndef __IMUMMAILACCOUNTMANAGER_H__
#define __IMUMMAILACCOUNTMANAGER_H__

// INCLUDES
#include <e32base.h>
#include <msvapi.h>                 // CMsvSession
#include <msvstd.h>
#include "ImumMboxDataExtension.h"
#include "ImumMboxSettingsCtrl.h"
#include <iapprefs.h>
#include <cemailaccounts.h>         // CEmailAccounts
#include <ImumInHealthServices.h>

// CONSTANTS
// MACROS
// DATA TYPES
typedef RArray<TImapAccount> RIMASImap4IdArray;
typedef RArray<TPopAccount> RIMASPop3IdArray;
typedef RArray<TSmtpAccount> RIMASSmtpIdArray;

// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CImumMboxData;
class CEmailAccounts;
class CMsvSession;
class CImumInHealthServicesImpl;
class CImumInternalApiImpl;
class CImumInSettingsData;

/**
* This class encapsulates saving and removing email accounts.
*
*/
class CImumMboxManager : public CBase
    {
    public: // Constructors and Destructor

        /**
        * NewL
        * @since S60 3.0
        * @param aSession, session reference
        * @return self pointer
        */
        static CImumMboxManager* NewL(
            CImumInternalApiImpl& aMailboxApi );

        /**
        * NewLC
        * @since S60 3.0
        * @param CMsvSession&, session reference
        * @return self pointer
        */
        static CImumMboxManager* NewLC(
            CImumInternalApiImpl& aMailboxApi );

        /**
        * Destructor
        * @since S60 3.0
        * Destructor
        */
        virtual ~CImumMboxManager();

    public: // New static functions

        // UTILITIES

        /**
        *
        * @since S60 3.0
        */
        static TInt GetIapPreferencesL(
            const TMsvId aMailboxId,
            CEmailAccounts& aAccounts,
            CImIAPPreferences& aIapPrefs,
            CImumInternalApiImpl& aMailboxApi,
            const TBool aSmtp );

        /**
        *
        * @since S60 3.0
        */
        static TInt SetIapPreferencesL(
            const TMsvId aMailboxId,
            CEmailAccounts& aAccounts,
            const CImIAPPreferences& aIapPrefs,
            CImumInternalApiImpl& aMailboxApi,
            const TBool aSmtp );

        /**
        *
        * @since S60 3.0
        */
        static void ForceAlwaysAskL(
            const TMsvId aMailboxId,
            CEmailAccounts& aAccounts,
            CImIAPPreferences& aIapPrefs,
            CImumInternalApiImpl& aMailboxApi,
            const TBool aSmtp );

    public: // New functions

        // ACCOUNT CREATION

        /**
         * 
         *
         * @since S60 v3.2
         * @param   
         */
        TMsvId CreateEmailAccountL( 
            const CImumInSettingsData& aSettingsData );

        /**
        *
        * @since S60 3.0
        * @return
        */
        void CreateEmailAccount(
            CImumMboxData& aSettings,
            const TBool aPopulateDefaults = EFalse );

        // ACCOUNT SAVING

        /**
         * 
         *
         * @since S60 v3.2
         * @param 
         */
        void SaveEmailSettingsL( 
            const CImumInSettingsData& aSettingsData );
            
        /**
        *
        * @since S60 3.0
        * @return
        */
        TInt SaveEmailSettingsL(
            CImumMboxData& aSettings );

        // ACCOUNT LOADING

        /**
         * 
         *
         * @since S60 v3.2
         * @param 
         * @return 
         * @leave &(leavedesc)s
         */
        CImumInSettingsData* LoadEmailSettingsL( 
            const TMsvId aMailboxId );
            
        /**
        *
        * @since S60 3.0
        */
        void LoadEmailSettingsL(
            const TMsvEntry& aEntry,
            CImumMboxData& aSettings );

        // ACCOUNT DELETING

        /**
        *
        * @since S60 3.0
        */
        void DeleteRelatedSmtpAccountsL(
            const CImumMboxData& aSettings );

        /**
        *
        * @since S60 3.0
        */
        void DeleteRelatedSmtpAccountsL(
            const TUint32 aRelatedAccountId,
            const TBool aIsImap4 );

        /**
        *
        * @since S60 3.0
        */
        void DeleteSmtpAccountsL(
            const TMsvId aRelatedMailboxId,
            const RIMASSmtpIdArray& aSmtpAccountArray );

        /**
        *
        * @since S60 3.0
        */
        void RemoveAccountL(
            const TMsvEntry& aMailboxEntry, TBool aSilent=EFalse );

        /**
        *
        * @since S60 3.0
        */
        void RemoveAccountL(
            const CImumMboxData& aSettings );
            
        /**
        *
        * @since S60 3.0
        */
        void RemoveAccountL(
            const TUint32 aRelatedAccountId,
            const TUid& aMtmId );

        /**
         * Removes the mailbox account
         *
         * @since S60 v3.2
         * @param aMailboxId
         */
        void RemoveAccount( const TMsvId aMailboxId, TBool aSilent=EFalse );

        /**
        *
        * @since S60 3.0
        */
        TInt DeleteAccountSettings(
            const TUint32 aAccountId,
            const TUid& aMtmId );

        /**
        *
        * @since S60 3.2
        */
        TBool CheckAccountDeletionConditionsL( const TMsvId aMailboxId );
        
    private: // Constructors

        /**
        * CImumMboxManager
        * @since S60 3.0
        * C++ constructor
        */
        CImumMboxManager(
            CImumInternalApiImpl& aMailboxApi );

        /**
        * ConstructL
        * @since S60 3.0
        * Symbian OS constructor
        */
        void ConstructL();

    private: // New functions

        /**
        *
        * @since S60 3.0
        * @return
        */
        void CreateEmailAccountL(
            CImumMboxData& aSettings,
            const TBool aPopulateDefaults );

        // MANAGER TOOLS

        /**
        *
        * @since S60 3.0
        */
        void LoadAccountFillIapL(
            CImumMboxData& aSettings );

        /**
        *
        * @since S60 3.0
        */
        void CreateAccountFillIapL(
            CImumMboxData& aSettings );

        /**
        *
        * @since S60 3.0
        */
        void StoreSignatureL(
            const CImumMboxData& aSettings );

        /**
        *
        * @since S60 3.0
        */
        void RestoreSignatureL(
            CImumMboxData& aSettings );

        /**
        *
        * @since S60 3.0
        */
        void StoreAccountIdToEntryL(
            const TMsvId aMailboxId,
            const TUint32 aAccountId );

        /**
        *
        * @since Series60 3.0
        */
        void StoreAccountIdToEntryL( 
            CImumMboxData& aSettings );

        /**
        *
        * @since S60 3.0
        */
        void GetMailboxAccountIdsL(
            const TMsvId aMailboxId,
            TMsvId& aReceivingMailboxId,
            TUint32& aReceivingAccountId,
            TMsvId& aSendingMailboxId,
            TUint32& aSendingAccountId );

        // ACCOUNT CREATION

        /**
        *
        * @since S60 3.0
        */
        TUint32 CreateImap4AccountL(
            CImumMboxData& aSettings,
            const TBool aPopulateDefaults,
            const TBool aReadOnly = EFalse );

        /**
        *
        * @since S60 3.0
        */
        TUint32 CreatePop3AccountL(
            CImumMboxData& aSettings,
            const TBool aPopulateDefaults,
            const TBool aReadOnly = EFalse );

        /**
        *
        * @since S60 3.0
        */
        void CreateExtendedSettingsL(
            const TMsvId aMailboxId,
            const TUint32 aAccountId,
            const TUid& aProtocol,
            CImumMboxData& aSettings,
            const TBool aPopulateDefaults = ETrue );

        /**
        *
        * @since S60 3.0
        */
        void DetermineDefaultAccountL(
            CImumMboxData& aSettings );

        // ACCOUNT DELETION

        /**
        *
        * @since S60 3.0
        */
        TInt DeleteExtendedSettings(
            const TIMAStorerParams& aParams );

        /**
        *
        * @since S60 3.0
        */
        void DeleteBaseSettingsL(
            const TUint32 aRelatedAccountId,
            const TBool aIsImap4 );

        /**
        *
        * @since S60 3.0
        */
        void DeleteBaseSettingsL(
            const CImumMboxData& aSettings );

        // ACCOUNT LOADING

        /**
        *
        * @since S60 3.0
        */
        void LoadPop3SettingsL(
            CImumMboxData& aSettings );

        /**
        *
        * @since S60 3.0
        */
        void LoadImap4SettingsL(
             CImumMboxData& aSettings );

        // ACCOUNT SAVING

        /**
        *
        * @since S60 3.0
        */
        void SavePop3SettingsL(
            CImumMboxData& aSettings );

        /**
        *
        * @since S60 3.0
        */
        void SaveImap4SettingsL(
            CImumMboxData& aSettings );

        /**
         * 
         *
         * @since S60 v3.2
         * @param 
         * @return 
         * @leave &(leavedesc)s
         */
        TBool IsConnectedL( const TMsvId aMailboxId ) const;

        /**
         * 
         *
         * @since S60 v3.2
         * @param 
         * @return 
         * @leave &(leavedesc)s
         */
        TBool IsLockedL( const TMsvId aMailboxId ) const;

        /**
         * 
         *
         * @since S60 v3.2
         */
         TBool HandleMailboxInUseDeleteL(
            MImumInHealthServices::RMailboxIdArray& aMailboxes,
            const TMsvEntry& aSmtpEntry ) const;
     
        /**
         * 
         *
         * @since S60 v3.2
         * @param
         * @return
         * @leave &(leavedesc)s
         */
        TBool AccountDeletionConditionsOkL( const TMsvId aMailboxId );
        
    private: // Data

        // Message Server Session
        CImumInternalApiImpl&           iMailboxApi;
        // An object to handle the storing of the settings
        CImumMboxSettingsCtrl*         iMboxSettingsCtrl;
        // An object to handle account creation
        CEmailAccounts*                 iAccounts;
        // An object to clean up the storage from trash
        CImumInHealthServicesImpl*          iCleaner;
    };

#endif // __IMUMMAILACCOUNTMANAGER_H__
