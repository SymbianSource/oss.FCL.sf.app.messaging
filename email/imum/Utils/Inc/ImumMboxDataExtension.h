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



#ifndef CImumMboxDataExtension_H
#define CImumMboxDataExtension_H

// INCLUDES
#include <e32base.h>
#include <e32cmn.h>                               // TBuf
#include <msvstd.h>
#include <muiuflags.h>
//#include "MuiuDynamicSettingItemBase.h"         // TMuiuSettingsText

// CONSTANTS

enum TMailAoStates
    {
    EMailAoAlways = 0,
    EMailAoHomeOnly,
    EMailAoOff
    };

enum TMailEmnStates
    {
    EMailEmnAutomatic = 0,
    EMailEmnHomeOnly,
    EMailEmnOff
    };

enum TMailAoUpdateModes
    {
    EMailAoHeadersOnly = 0,
    EMailAoBody,
    EMailAoBodyAndAttas
    };

// Flags to handle status of settings
enum TIMASMailSettingsStatusFlags
    {
    EIMASStatusTempRcvPassword = 0,
    EIMASStatusTempSndPassword,
    EIMASStatusLastFlag
    };
    
// Mail deletion setting mode
enum TIMASMailDeletionMode
    {
    EIMASMailDeletionAlwaysAsk = 0,
    EIMASMailDeletionPhone,
    EIMASMailDeletionServer
    };

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
* Utility structure TAOInfo.
* Contains info was last update successful ( TBool )
* and the date and time of last successful update ( TTime )
*/
struct TAOInfo
    {
    TBool iLastUpdateFailed;
    TTime iLastSuccessfulUpdate;
    TBool iUpdateSuccessfulWithCurSettings;
    };

/**
*  Settings extension to current mailbox settings
*
*  @lib Imum.lib
*  @since S60 3.0
*/
class CImumMboxDataExtension : public CBase
    {
    public: // Constructors and destructor

        /**
        * Creates mailbox extension object
        * @since S60 3.0
        */
        static CImumMboxDataExtension* NewLC();

        /**
        * Creates mailbox extension object
        * @since S60 3.0
        */
        static CImumMboxDataExtension* NewL();

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CImumMboxDataExtension();

    public: // New functions

        /**
        * Verifies the validy of the metadata in the settings
        * @since S60 3.0
        * @return ETrue if the settings are valid
        * @return EFalse if the settings are invalid
        */
        TBool IsSettingsValid() const;

        // COMMON

        /**
        * Retrieves the status flags of the settings
        * @since S60 3.0
        * @return Status flags
        */
        void SetStatusFlags( const TMuiuFlags& aStatusFlags );

        /**
        * Retrieves the status flags of the settings
        * @since S60 3.0
        * @return Status flags
        */
        const TMuiuFlags& StatusFlags() const;

        /**
        * Makes identical copy of settings
        * @since S60 3.0
        * @param aExtension, Setting to be copied
        */
        void CopyL(
            const CImumMboxDataExtension& aExtension );

        /**
        * Sets the id of the mailbox the setting is associated with
        * This is not exported
        * @since S60 3.0
        * @param aMailboxId, Mailbox, who owns the settings
        */
        void SetMailboxId(
            const TMsvId aMailboxId );

        /**
        * Returns the id of the owner of the setting
        * @since S60 3.0
        * @return Mailbox id
        */
        TMsvId MailboxId() const;

        /**
        * Set the account id of the base settings
        * This is not exported
        * @since S60 3.0
        * @param aAccountId, Sets value of iAccountId
        */
        void SetAccountId( const TUint32 aAccountId );

        /**
        * Get the account id of the base settings
        * @since S60 3.0
        * @return value of iAccountId
        */
        TUint32 AccountId() const;

        /**
        * Set the account id of the extended settings
        * This is not exported
        * @since S60 3.0
        * @param aExtensionId, Sets value of iExtensionId
        */
        void SetExtensionId( const TUint32 aExtensionId );

        /**
        * Get the account id of the extended settings
        * @since S60 3.0
        * @return value of iExtensionId
        */
        TUint32 ExtensionId() const;

        /**
        * Set method for member iProtocol
        * This is not exported
        * @since S60 3.0
        * @param aProtocol, Sets value of iProtocol
        */
        void SetProtocol( const TUid& aProtocol );

        /**
        * Get function for member iProtocol
        * @since S60 3.0
        * @return value of iProtocol
        */
        TUid Protocol() const;

        /**
        * Set method for member iEmailAddress
        * @since S60 3.0
        * @param aEmailAddress, Sets value of iEmailAddress
        */
        void SetEmailAddress( const TDesC& aEmailAddress );

        /**
        * Get function for member iEmailAddress
        * @since S60 3.0
        * @return value of iEmailAddress
        */
        const TDesC* EmailAddress() const;

        // EXTENDED MAIL SETTINGS

        /**
        * Sets the state of email notification -flag
        * @since S60 3.0
        */
        void SetEmailNotificationState(
            const TMailEmnStates aEmnState );

        /**
        * Fetches the state of email notification -flag
        * @since S60 3.0
        * @return ETrue or EFalse, according to flag state
        */
        TMailEmnStates EmailNotificationState() const;

        /**
        * A flag to
        * @since S60 3.0
        * @param aNewMailIndicators, Sets value of iNewMailIndicators
        */
        void SetNewMailIndicators(
            const TInt aNewState );

        /**
        *
        * @since S60 3.0
        * @return value of EMailNewIndicators
        */
        TBool NewMailIndicators() const;

        /**
        * Sets the state of hide messages -flag
        * @since S60 3.0
        * @param aNewState, The new state of the flag
        */
        void SetHideMsgs(
            const TBool aNewState );

        /**
        * Fetches the state of hide messages -flag
        * @since S60 3.0
        * @return ETrue or EFalse, according to flag state
        */
        TBool HideMsgs() const;

        /**
        * Sets the value of open html mail -setting
        * @since S60 3.0
        * @param aOpenHtmlMail, New value of the setting
        */
        void SetOpenHtmlMail(
            const TInt aOpenHtmlMail );

        /**
        * Fetches the state of open html mail -setting
        * @since S60 3.0
        * @return Setting value
        */
        TInt OpenHtmlMail() const;

        // ALWAYS ONLINE

        /**
        * Get function for member iVersion
        * @since S60 3.0
        * @return value of iVersion
        */
        TInt Version() const;

        /**
        * Set method for member iAlwaysOnlineState
        * @since S60 3.0
        * @param aAlwaysOnlineState, Sets value of iAlwaysOnlineState
        */
        void SetAlwaysOnlineState(
            const TMailAoStates aAlwaysOnlineState );

        /**
        * Get function for member iAlwaysOnlineState
        * @since S60 3.0
        * @return value of iAlwaysOnlineState
        */
        TMailAoStates AlwaysOnlineState() const;

        /**
        * Set method for member iSelectedWeekDays
        * @since S60 3.0
        * @param aSelectedWeekDays, Sets value of iSelectedWeekDays
        */
        void SetSelectedWeekDays(
            const TUint aSelectedWeekDays );

        /**
        * Get function for member iSelectedWeekDays
        * @since S60 3.0
        * @return value of iSelectedWeekDays
        */
        TUint SelectedWeekDays() const;

        /**
        * Set method for member iSelectedTimeStart
        * @since S60 3.0
        * @param aSelectedTimeStart, Sets value of iSelectedTimeStart
        */
        void SetSelectedTimeStart(
            const TTime aSelectedTimeStart );

        /**
        * Get function for member iSelectedTimeStart
        * @since S60 3.0
        * @return value of iSelectedTimeStart
        */
        TTime SelectedTimeStart() const;

        /**
        * Set method for member iSelectedTimeStop
        * @since S60 3.0
        * @param aSelectedTimeStop, Sets value of iSelectedTimeStop
        */
        void SetSelectedTimeStop(
            const TTime aSelectedTimeStop );

        /**
        * Get function for member iSelectedTimeStop
        * @since S60 3.0
        * @return value of iSelectedTimeStop
        */
        TTime SelectedTimeStop() const;

        /**
        * Set method for member iInboxRefreshTime
        * @since S60 3.0
        * @param aInboxRefreshTime, Sets value of iInboxRefreshTime
        */
        void SetInboxRefreshTime(
            const TInt aInboxRefreshTime );

        /**
        * Get function for member iInboxRefreshTime
        * @since S60 3.0
        * @return value of iInboxRefreshTime
        */
        TInt InboxRefreshTime() const;

        /**
        * Set method for member iUpdateMode
        * @since S60 3.0
        * @param aUpdateMode, Sets value of iUpdateMode
        */
        void SetUpdateMode(
            const TMailAoUpdateModes aUpdateMode );

        /**
        * Get function for member iUpdateMode
        * @since S60 3.0
        * @return value of iUpdateMode
        */
        TMailAoUpdateModes UpdateMode() const;

        /**
        * Set method for member iLastUpdateInfo
        * @since S60 3.0
        * @param aLastUpdateInfo, Sets value of iLastUpdateInfo
        */
        void SetLastUpdateInfo( const TAOInfo& aLastUpdateInfo );

        /**
        * Get function for member iLastUpdateInfo
        * @since S60 3.0
        * @return value of iLastUpdateInfo
        */
        TAOInfo LastUpdateInfo() const;
        
        /**
        * Set mail deletion setting mode
        * @since S60 3.2
        * @param aMailDeletion, Sets value of iMailDeletion
        */
        void SetMailDeletion( const TIMASMailDeletionMode aMailDeletion );
        
        /**
        * Get mail deletion setting mode
        * @since S60 3.2
        * @return value of iMailDeletion
        */
        TIMASMailDeletionMode MailDeletion() const;
        
        /**
        * Sets the predefinition status
        * @since S60 v3.2
        * @param aIsPredefined, Predefinition state
        */
        void SetIsPredefined( const TBool& aIsPredefined );
        
        /**
        * Gets the predefinition status
        * @since S60 v3.2
        * @return Predefinition status
        */
        TBool IsPredefined() const;
        
    public: // Functions from base classes

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // Constructors and destructor

        /**
        * ConstructL
        * @since S60 3.0
        */
        void ConstructL();

        /**
        * Default constructor
        * @since S60 3.0
        */
        CImumMboxDataExtension();

    private:  // New virtual functions
    private:  // New functions
    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        enum TMailExtensionFlags
            {
            EMailNewIndicators = 0,
            EMailNewNotifications,
            EMailHideMsgsOverLimit,
            EMailShowFieldCC,
            EMailShowFieldBCC,
            EMailShowFieldSubject,
            EMailIsPredefined,
            EMailLastFlag
            };

        // Common: Status flags of settings
        TMuiuFlags          iStatusFlags;
        // Common: Id of the mailbox this settings belongs
        TInt                iMailboxId;
        // Common: Account id of the base settings
        TUint32             iAccountId;
        // Common: Account id of the extended settings
        TUint32             iExtensionId;
        // Common: Protocol of the base settings
        TUid                iProtocol;
        // Extended: The setting flags
        TMuiuFlags          iFlags;
        // Extended:
        TInt                iOpenHtmlMail;
        // AO:
        TInt                iVersion;
        // AO:
        TMailAoStates       iAlwaysOnlineState;
        // AO: Selected weekdays: 0-6 = Monday-Sunday
        TUint               iSelectedWeekDays;
        // AO: Connection starting time
        TTime               iSelectedTimeStart;
        // AO: Connection ending time
        TTime               iSelectedTimeStop;
        // AO: Inbox refresh interval in minutes
        TInt                iInboxRefreshTime;
        // AO:
        TMailAoUpdateModes  iUpdateMode;
        // AO:
        TAOInfo             iLastUpdateInfo;
        // AO:
        TBuf<1600>* iEmailAddress; // Temporary
        // EMN:
        TMailEmnStates      iEmnState;
        // Common: mail deletion behaviour
        TIMASMailDeletionMode iMailDeletion;

    };

#endif      // CImumMboxDataExtension_H

// End of File
