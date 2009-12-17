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
* Description:  Defines an API to create and edit mailbox settings
*
*/



#ifndef IMUMMAILACCOUNTSETTINGSSTORER_H
#define IMUMMAILACCOUNTSETTINGSSTORER_H

// INCLUDES
#include <e32base.h>
#include <miutset.h>                    // CImBaseEmailSettings
#include <cemailaccounts.h>             // CEmailAccounts
#include "ImumMboxDataExtension.h"      // CImumMboxDataExtension
#include "ImumInHealthServicesImpl.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CRepository;
class CImumMboxDataExtension;
class CIMASCenRepControl;
class CIMASAccountControl;
class CImumMboxDefaultData;
class CImumInternalApiImpl;
class MImumInMailboxUtilities;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib Imum.lib
*  @since S60 3.0
*/
class TIMAStorerParams
    {
    public: // Constructors

        TIMAStorerParams(
            const TUid& aMtmId,
            const TMsvId aMailboxId );
        TIMAStorerParams(
            const TUint32 aAccountId,
            const TUid& aMtmId );
        TIMAStorerParams(
            const TMsvEntry& aEntry );

    public: // New functions

        TMsvId MailboxId() const;
        TUint32 AccountId() const;
        TUid MtmId() const;

    private: // Data
        TMsvId  iMailboxId;
        TUint32 iAccountId;
        TUid    iMtmId;
    };


/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib Imum.lib
*  @since S60 3.0
*/
class CImumMboxSettingsCtrl : public CBase
    {
    public: // Constructors and destructor

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CImumMboxSettingsCtrl();

        /**
        * Creates the settings storer object
        * @since S60 3.0
        */
        static CImumMboxSettingsCtrl* NewL( 
            CImumInternalApiImpl& aMailboxApi );

        /**
        * Creates the settings storer object
        * @since S60 3.0
        */
        static CImumMboxSettingsCtrl* NewLC( 
            CImumInternalApiImpl& aMailboxApi );

    public: // New functions

        // CREATING NEW ACCOUNT

        /**
        *
        * @since S60 3.0
        */
        TInt CreateSettings(
            const CImumMboxDataExtension& aExtendedMailSettings );

        /**
        *
        * @since S60 3.0
        */
        TInt CreateSettings(
            const TMsvId aMailboxId,
            const TUint32 aAccountId,
            const TUid& aProtocol,
            CImumMboxDataExtension& aExtendedMailSettings,
            const TBool aPopulateDefaults = ETrue );

        /**
        *
        * @since S60 3.0
        */
        void PreInitializeExtendedSettings(
            const TMsvId aMailboxId,
            const TUint32 aAccountId,
            const TUid& aProtocol,
            CImumMboxDataExtension& aExtendedMailSettings ) const;

        /**
        *
        * @since S60 3.0
        */
        TInt PopulateDefaultExtSettings(
            CImumMboxDataExtension& aExtendedMailSettings ) const;

        // DELETING EXISTING ACCOUNT

        /**
        *
        * @since S60 3.0
        */
        TInt DeleteExtendedSettings(
            const TIMAStorerParams& aParams );

        // SAVING CURRENT ACCOUNT

        /**
        * Saves the Extended settings to the Central Repository
        * @since S60 3.0
        * @param aExtendedSettings, Settings object
        * @return KErrNone, if saving has been successful
        */
        TInt SaveSettings(
            const CImumMboxDataExtension& aExtendedMailSettings );

        /**
        *
        * @since S60 3.0
        */
        TInt SaveSettings(
            const TUint32 aAccountId,
            const CImImap4Settings& aImap4Settings );

        /**
        *
        * @since S60 3.0
        */
        TInt SaveSettings(
            const TUint32 aAccountId,
            const CImPop3Settings& aPop3Settings );

        /**
        *
        * @since S60 3.0
        */
        TInt SaveSettings(
            const TUint32 aAccountId,
            const CImSmtpSettings& aSmtpSettings );

        /**
        *
        * @since S60 3.0
        */
        TInt SaveSettings(
            const TMsvId aMailboxId,
            const CImImap4Settings& aImap4Settings );

        /**
        *
        * @since S60 3.0
        */
        TInt SaveSettings(
            const TMsvId aMailboxId,
            const CImPop3Settings& aPop3Settings );

        /**
        *
        * @since S60 3.0
        */
        TInt SaveSettings(
            const TMsvId aMailboxId,
            const CImSmtpSettings& aSmtpSettings );

        /**
        *
        * @since S60 3.0
        */
        TInt SaveSettings(
            const TMsvEntry& aEntry,
            CImBaseEmailSettings& aBaseSettings );

        // LOADING EXISTING ACCOUNT

        /**
        * Loads the Extended settings from the Central Repository
        * @since S60 3.0
        * @param aAccountId, Id of the account to be used
        * @param aExtendedSettings, Settings object
        * @return KErrNone, if loading has been successful
        */
        TInt LoadSettings(
            const TIMAStorerParams& aParams,
            CImumMboxDataExtension& aExtendedMailSettings );

        /**
        *
        * @since S60 3.0
        */
        TInt LoadSettings(
            const TUint32 aAccountId,
            CImImap4Settings& aImap4Settings );

        /**
        *
        * @since S60 3.0
        */
        TInt LoadSettings(
            const TUint32 aAccountId,
            CImPop3Settings& aPop3Settings );

        /**
        *
        * @since S60 3.0
        */
        TInt LoadSettings(
            const TUint32 aAccountId,
            CImSmtpSettings& aSmtpSettings );

        /**
        *
        * @since S60 3.0
        */
        TInt LoadSettings(
            const TMsvId aMailboxId,
            CImImap4Settings& aImap4Settings );

        /**
        *
        * @since S60 3.0
        */
        TInt LoadSettings(
            const TMsvId aMailboxId,
            CImPop3Settings& aPop3Settings );

        /**
        *
        * @since S60 3.0
        */
        TInt LoadSettings(
            const TMsvId aMailboxId,
            CImSmtpSettings& aSmtpSettings );

        /**
        *
        * @since S60 3.0
        */
        TInt LoadSettings(
            const TMsvEntry& aEntry,
            CImBaseEmailSettings& aBaseSettings );

        /**
        *
        * @since S60 3.0
        */
        TUint32 AccountId2ExtensionId(
            const TUint32 aAccountId,
            const TUid& aMtmId ) const;

        /**
        *
        * @since S60 3.0
        */
        TUint32 ExtensionId2AccountId(
            const TUint32 aExtension,
            const TUid& aMtmId ) const;
            
        // DEFAULT DATA RESTORING

        /**
        *
        * @since Series60 3.0
        */
        void RestoreDefaultSettingsDataL( 
            CImumMboxDefaultData& aDefaultData );  
            
        /**
         * 
         *
         * @since S60 v3.2
         * @param 
         * @leave &(leavedesc)s
         */
        void LoadAlwaysOnlineSettingsL(
            const TUint32 aAccountId,
            const TUid& aMtmId,
            TInt64& aDays,
            TInt64& aStartTime,
            TInt64& aStopTime,
            TInt& iActive );

		/**
        * Checks the mailboxname and cut it off if too long
        * @since S60 3.2
        * @param aMailboxEntry
        * @return EIMDNoError, when operation was faultless
        */
        TIMDErrors CheckMailboxNameImap(
        	TMsvEntry& aMailboxEntry ) const;
        
        /**
        * Checks the mailboxname and cut it off if too long
        * @since S60 3.2
        * @param aMailboxEntry
        * @return EIMDNoError, when operation was faultless
        */
        TIMDErrors CheckMailboxNamePop(
        	TMsvEntry& aMailboxEntry ) const;
        
        /**
        * Checks the mailboxname and cut it off if too long
        * @since S60 3.2
        * @param aMailboxEntry
        * @return EIMDNoError, when operation was faultless
        */
        TIMDErrors CheckMailboxNameSmtp(
        	TMsvEntry& aMailboxEntry ) const;
            
        /**
         * Set last update info
         * @since S60 v3.2
         * @param aAccountId, Account ID
         * @param aMtmId, Mtm ID
         * @param aSuccess, True if mailbox update was successfull
         * @param aTime, Update time
         */
        void SetLastUpdateInfoL(TUint32 aAccountId, const TUid& aMtmId,
            TBool aSuccess, const TTime& aTime );

    public: // Functions from base classes

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // Constructors and destructor

        /**
        * Default constructor
        * @since S60 3.0
        */
        CImumMboxSettingsCtrl( 
            CImumInternalApiImpl& aMailboxApi );

        /**
        * ConstructL
        * @since S60 3.0
        */
        void ConstructL();

    private:  // New virtual functions
    private:  // New functions

        // CREATING NEW ACCOUNT

        /**
        *
        * @since S60 3.0
        */
        void CreateSettingsL(
            const CImumMboxDataExtension& aExtendedMailSettings );

        /**
        *
        * @since S60 3.0
        */
        void CreateSettingsL(
            const TMsvId aMailboxId,
            const TUid& aMtmId,
            const CImumMboxDataExtension& aExtendedMailSettings );

        // DELETING EXISTING ACCOUNT

        /**
        *
        * @since S60 3.0
        */
        void DeleteExtendedSettingsL(
            const TMsvId aMailboxId,
            const TUid& aMtmId );

        /**
        *
        * @since S60 3.0
        */
        void DeleteExtendedSettingsL(
            const TUint32 aAccountId,
            const TUid& aMtmId );

        // SAVING CURRENT ACCOUNT

        /**
        * Saves the Extended settings to the Central Repository
        * @since S60 3.0
        * @param aExtendedSettings, Settings object
        * @return KErrNone, if saving has been successful
        */
        void SaveSettingsL(
            const CImumMboxDataExtension& aExtendedMailSettings );

        /**
        *
        * @since S60 3.0
        */
        void SaveSettingsL(
            const TUint32 aAccountId,
            const CImImap4Settings& aImap4Settings );

        /**
        *
        * @since S60 3.0
        */
        void SaveSettingsL(
            const TUint32 aAccountId,
            const CImPop3Settings& aPop3Settings );

        /**
        *
        * @since S60 3.0
        */
        void SaveSettingsL(
            const TUint32 aAccountId,
            const CImSmtpSettings& aSmtpSettings );

        /**
        *
        * @since S60 3.0
        */
        void SaveSettingsL(
            const TMsvId aMailboxId,
            const CImImap4Settings& aImap4Settings );

        /**
        *
        * @since S60 3.0
        */
        void SaveSettingsL(
            const TMsvId aMailboxId,
            const CImPop3Settings& aPop3Settings );

        /**
        *
        * @since S60 3.0
        */
        void SaveSettingsL(
            const TMsvId aMailboxId,
            const CImSmtpSettings& aSmtpSettings );

        /**
        *
        * @since S60 3.0
        */
        void SaveSettingsL(
            const TMsvEntry& aEntry,
            CImBaseEmailSettings& aBaseSettings );

        // LOADING EXISTING ACCOUNT

        /**
        * Loads the Extended settings from the Central Repository
        * @since S60 3.0
        * @param aAccountId, Id of the account to be used
        * @param aExtendedSettings, Settings object
        * @return KErrNone, if loading has been successful
        */
        void LoadSettingsL(
            const TUint32 aAccountId,
            const TUid& aMtmId,
            CImumMboxDataExtension& aExtendedMailSettings );

        /**
        *
        * @since S60 3.0
        */
        void LoadSettingsL(
            const TMsvId aMailboxId,
            const TUid& aMtmId,
            CImumMboxDataExtension& aExtendedMailSettings );

        /**
        *
        * @since S60 3.0
        */
        void LoadSettingsL(
            const TUint32 aAccountId,
            CImImap4Settings& aImap4Settings );

        /**
        *
        * @since S60 3.0
        */
        void LoadSettingsL(
            const TMsvId aMailboxId,
            CImImap4Settings& aImap4Settings );

        /**
        *
        * @since S60 3.0
        */
        void LoadSettingsL(
            const TUint32 aAccountId,
            CImPop3Settings& aPop3Settings );

        /**
        *
        * @since S60 3.0
        */
        void LoadSettingsL(
            const TMsvId aMailboxId,
            CImPop3Settings& aPop3Settings );

        /**
        *
        * @since S60 3.0
        */
        void LoadSettingsL(
            const TUint32 aAccountId,
            CImSmtpSettings& aSmtpSettings );

        /**
        *
        * @since S60 3.0
        */
        void LoadSettingsL(
            const TMsvId aMailboxId,
            CImSmtpSettings& aSmtpSettings );

        /**
        *
        * @since S60 3.0
        */
        void LoadSettingsL(
            const TMsvEntry& aEntry,
            CImBaseEmailSettings& aBaseSettings );

        // SPECIAL STORERS

        /**
        *
        * @since S60 3.0
        */
        void CreateLastUpdateInfoL(
            const TAOInfo& aAoInfo );

        /**
        *
        * @since S60 3.0
        */
        TInt DeleteLastUpdateInfo();

        /**
        *
        * @since S60 3.0
        */
        TInt SaveLastUpdateInfo(
            const TAOInfo& aAoInfo );

        /**
        *
        * @since S60 3.0
        */
        TAOInfo LoadLastUpdateInfo();

        /**
        * Verifies the validity of the extended settings and opens them
        * @since S60 3.0
        */
        void OpenExtendedSettingsL(
            const TIMAStorerParams& aParams,
            CImumMboxDataExtension& aExtendedMailSettings );

        /**
        *
        * @since S60 3.0
        */
        CEmailAccounts* AccountsL();

        /**
        *
        * @since S60 3.0
        */
        void LoadExtendedSettingsStatusFlagsL(
            CImumMboxDataExtension& aExtendedSettings );

        /**
        *
        * @since S60 3.0
        */
        void SaveExtendedSettingsStatusFlagsL(
            const CImumMboxDataExtension& aExtendedSettings );

        /**
        *
        * @since S60 3.0
        */
        void CreateExtendedSettingsStatusFlagsL(
            const CImumMboxDataExtension& aExtendedSettings );
    
        /**
         * Fix possible errors in SMTP mailbox name. Leaves if fixing fails.
         * @param aMailboxEntry Mailbox to fix.
         */
        void FixMailboxNameSmtpL( TMsvEntry& aMailboxEntry ) const;
        
        /**
         * Fix possible errors in IMAP mailbox name. Leaves if fixing fails.
         * @param aMailboxEntry Mailbox to fix.
         */
        void FixMailboxNameImapL( TMsvEntry& aMailboxEntry ) const;
        
        /**
         * Fix possible errors in POP mailbox name. Leaves if fixing fails.
         * @param aMailboxEntry Mailbox to fix.
         */
        void FixMailboxNamePopL( TMsvEntry& aMailboxEntry ) const;
        
    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        // Reference to mailbox api
        CImumInternalApiImpl&   iMailboxApi;
        // Pointer to Utilities 
        MImumInMailboxUtilities* iUtils;
        // Repository for smtp settings
        CRepository*        iCenRepSmtp;
        // Repository for extended mail settings
        CRepository*        iCenRepExtMail;
        // Repository for always online settings
        CRepository*        iCenRepAlwaysOnline;
        // Repository for default settings data
        CRepository*        iCenRepDefaultData;
        // Pointer to accounts handler
        CEmailAccounts*     iAccounts;
        // Class to handle Central Repository operations
        CIMASCenRepControl* iCenRepControl;
        // Class to handle account id shifting
        CIMASAccountControl* iAccountControl;
        // Flag whether cmail variation is on or off
        TBool               iFFEmailVariation;
    };

#endif      // IMUMMAILACCOUNTSETTINGSSTORER_H

// End of File
