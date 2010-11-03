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
* Description:  ImumMboxDataExtension.cpp
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <muiuflags.h>

#include "EmailUtils.H"
#include "ImumMboxDataExtension.h"
#include "MuiuDynamicSettingItemBase.h"
#include "IMASAccountControl.h"
#include "ImumUtilsLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::CImumMboxDataExtension()
// ----------------------------------------------------------------------------
//
CImumMboxDataExtension::CImumMboxDataExtension()
    :
    iMailboxId( KErrNotFound ),
    iAccountId( 0 ),
    iExtensionId( 0 ),
    iProtocol( TUid::Uid( 0 ) ),
    iOpenHtmlMail( KErrNotFound ),
    iVersion( KErrNotFound ),
    iAlwaysOnlineState( EMailAoOff ),
    iSelectedWeekDays( 0 ),
    iSelectedTimeStart( 0 ),
    iSelectedTimeStop( 0 ),
    iInboxRefreshTime( KErrNotFound ),
    iUpdateMode( EMailAoHeadersOnly ),
    iEmailAddress( NULL ),
    iEmnState( EMailEmnOff ),
    iMailDeletion( EIMASMailDeletionAlwaysAsk )
    {
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::~CImumMboxDataExtension()
// ----------------------------------------------------------------------------
//
CImumMboxDataExtension::~CImumMboxDataExtension()
    {
    delete iEmailAddress;
    iEmailAddress = NULL;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::NewL()
// ----------------------------------------------------------------------------
//
CImumMboxDataExtension* CImumMboxDataExtension::NewL()
    {
    CImumMboxDataExtension* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::NewLC()
// ----------------------------------------------------------------------------
//
CImumMboxDataExtension* CImumMboxDataExtension::NewLC()
    {
    CImumMboxDataExtension* self =
        new ( ELeave ) CImumMboxDataExtension();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::ConstructL()
    {
    iEmailAddress = new ( ELeave ) TMuiuSettingsText();
    iLastUpdateInfo.iLastUpdateFailed = EFalse;
    iLastUpdateInfo.iLastSuccessfulUpdate =  Time::NullTTime();
    iLastUpdateInfo.iUpdateSuccessfulWithCurSettings = EFalse;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::CopyL()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::CopyL(
    const CImumMboxDataExtension& aExtension )
    {
    iMailboxId = aExtension.MailboxId();
    iAccountId = aExtension.AccountId();
    iExtensionId = aExtension.ExtensionId();
    iProtocol = aExtension.Protocol();
    iOpenHtmlMail = aExtension.OpenHtmlMail();
    iVersion = aExtension.Version();
    iAlwaysOnlineState = aExtension.AlwaysOnlineState();
    iSelectedWeekDays = aExtension.SelectedWeekDays();
    iSelectedTimeStart = aExtension.SelectedTimeStart();
    iSelectedTimeStop = aExtension.SelectedTimeStop();
    iInboxRefreshTime = aExtension.InboxRefreshTime();
    iUpdateMode = aExtension.UpdateMode();
    iLastUpdateInfo = aExtension.LastUpdateInfo();
    iEmailAddress->Copy( *aExtension.EmailAddress() );
    iEmnState = aExtension.EmailNotificationState();
    iMailDeletion = aExtension.MailDeletion();

    // Get flags
    for ( TInt flag = 0; flag < EMailLastFlag; flag++ )
        {
        iFlags.ChangeFlag( flag, iFlags.Flag( flag ) );
        }

    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::IsSettingsValid()
// ----------------------------------------------------------------------------
//
TBool CImumMboxDataExtension::IsSettingsValid() const
    {
    // Verify that the account id is set, extension id is set, mailbox id
    // is set and the protocol is set to either imap4 or pop3

    // Make sure the mailbox id is valid
    if ( iMailboxId < 0 )
        {
        return EFalse;
        }

    // Make sure that the account id is between 1 <= id <= 255
    if ( !iAccountId || iAccountId > KIMASMaxAccounts )
        {
        return EFalse;
        }

    // Check the extension id's for each protocol
    IMASAccountControl accountControl;
    TUint32 extensionId = accountControl.ExtendedAccountIdToAccountId(
        iExtensionId, iProtocol );

    // Make sure that the extension id is between 1 <= id <= 255
    if ( !extensionId || extensionId > KIMASMaxAccounts )
        {
        return EFalse;
        }

    // Settings are valid
    return ETrue;
    }

/******************************************************************************

    Extended mail settings

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetStatusFlags()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetStatusFlags(
    const TMuiuFlags& aStatusFlags )
    {
    iStatusFlags = aStatusFlags;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::StatusFlags()
// ----------------------------------------------------------------------------
//
const TMuiuFlags& CImumMboxDataExtension::StatusFlags() const
    {
    return iStatusFlags;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetEmailAddress()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetEmailAddress(
    const TDesC& aEmailAddress )
    {
    iEmailAddress->Copy( aEmailAddress );
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::EmailAddress()
// ----------------------------------------------------------------------------
//
const TDesC* CImumMboxDataExtension::EmailAddress() const
    {
    return iEmailAddress;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetEmailNotificationState()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetEmailNotificationState(
    const TMailEmnStates aEmnState )
    {
    iEmnState = aEmnState;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::EmailNotificationState()
// ----------------------------------------------------------------------------
//
TMailEmnStates CImumMboxDataExtension::EmailNotificationState() const
    {
    return iEmnState;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetNewMailIndicators()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetNewMailIndicators(
    const TInt aNewState )
    {
    iFlags.ChangeFlag( EMailNewIndicators, aNewState );
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::NewMailIndicators()
// ----------------------------------------------------------------------------
//
TBool CImumMboxDataExtension::NewMailIndicators() const
    {
    return iFlags.Flag( EMailNewIndicators );
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetHideMsgs()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetHideMsgs(
    const TBool aNewState )
    {
    iFlags.ChangeFlag( EMailHideMsgsOverLimit, aNewState );
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::HideMsgs()
// ----------------------------------------------------------------------------
//
TBool CImumMboxDataExtension::HideMsgs() const
    {
    return iFlags.Flag( EMailHideMsgsOverLimit );
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetOpenHtmlMail()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetOpenHtmlMail(
    const TInt aOpenHtmlMail )
    {
    iOpenHtmlMail = aOpenHtmlMail;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::OpenHtmlMail()
// ----------------------------------------------------------------------------
//
TInt CImumMboxDataExtension::OpenHtmlMail() const
    {
    return iOpenHtmlMail;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetOpenHtmlMail()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetMailboxId(
    const TMsvId aMailboxId )
    {
    iMailboxId = aMailboxId;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::OpenHtmlMail()
// ----------------------------------------------------------------------------
//
TMsvId CImumMboxDataExtension::MailboxId() const
    {
    return iMailboxId;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetAccountId()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetAccountId(
    const TUint32 aAccountId )
    {
    iAccountId = aAccountId;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::AccountId()
// ----------------------------------------------------------------------------
//
TUint32 CImumMboxDataExtension::AccountId() const
    {
    return iAccountId;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetExtensionId()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetExtensionId(
    const TUint32 aExtensionId )
    {
    iExtensionId = aExtensionId;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::ExtensionId()
// ----------------------------------------------------------------------------
//
TUint32 CImumMboxDataExtension::ExtensionId() const
    {
    return iExtensionId;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetProtocol()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetProtocol(
    const TUid& aProtocol )
    {
    iProtocol = aProtocol;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::Protocol()
// ----------------------------------------------------------------------------
//
TUid CImumMboxDataExtension::Protocol() const
    {
    return iProtocol;
    }

/******************************************************************************

    Always Online settings

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::Version()
// ----------------------------------------------------------------------------
//
TInt CImumMboxDataExtension::Version() const
    {
    IMUM_CONTEXT( CImumMboxDataExtension::Version, 0, KLogData );

    return iVersion;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetAlwaysOnlineState()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetAlwaysOnlineState(
    const TMailAoStates aAlwaysOnlineState )
    {
    IMUM_CONTEXT( CImumMboxDataExtension::SetAlwaysOnlineState, 0, KLogData );

    iAlwaysOnlineState = aAlwaysOnlineState;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::AlwaysOnlineState()
// ----------------------------------------------------------------------------
//
TMailAoStates CImumMboxDataExtension::AlwaysOnlineState() const
    {
    return iAlwaysOnlineState;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetSelectedWeekDays()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetSelectedWeekDays(
    const TUint aSelectedWeekDays )
    {
    iSelectedWeekDays = aSelectedWeekDays;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SelectedWeekDays()
// ----------------------------------------------------------------------------
//
TUint CImumMboxDataExtension::SelectedWeekDays() const
    {
    return iSelectedWeekDays;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetSelectedTimeStart()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetSelectedTimeStart(
    const TTime aSelectedTimeStart )
    {
    iSelectedTimeStart = aSelectedTimeStart;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SelectedTimeStart()
// ----------------------------------------------------------------------------
//
TTime CImumMboxDataExtension::SelectedTimeStart() const
    {
    return iSelectedTimeStart;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetSelectedTimeStop()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetSelectedTimeStop(
    const TTime aSelectedTimeStop )
    {
    iSelectedTimeStop = aSelectedTimeStop;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SelectedTimeStop()
// ----------------------------------------------------------------------------
//
TTime CImumMboxDataExtension::SelectedTimeStop() const
    {
    return iSelectedTimeStop;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetInboxRefreshTime()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetInboxRefreshTime(
    const TInt aInboxRefreshTime )
    {
    iInboxRefreshTime = aInboxRefreshTime;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::InboxRefreshTime()
// ----------------------------------------------------------------------------
//
TInt CImumMboxDataExtension::InboxRefreshTime() const
    {
    return iInboxRefreshTime;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetUpdateMode()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetUpdateMode(
    const TMailAoUpdateModes aUpdateMode )
    {
    iUpdateMode = aUpdateMode;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::UpdateMode()
// ----------------------------------------------------------------------------
//
TMailAoUpdateModes CImumMboxDataExtension::UpdateMode() const
    {
    return iUpdateMode;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetLastUpdateInfo()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetLastUpdateInfo(
    const TAOInfo& aLastUpdateInfo )
    {
    iLastUpdateInfo = aLastUpdateInfo;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::LastUpdateInfo()
// ----------------------------------------------------------------------------
//
TAOInfo CImumMboxDataExtension::LastUpdateInfo() const
    {
    return iLastUpdateInfo;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetMailDeletion()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetMailDeletion(
    const TIMASMailDeletionMode aMailDeletion )
    {
    iMailDeletion = aMailDeletion;
    }
// ----------------------------------------------------------------------------
// CImumMboxDataExtension::MailDeletion()
// ----------------------------------------------------------------------------
//
TIMASMailDeletionMode CImumMboxDataExtension::MailDeletion() const
    {
    return iMailDeletion;
    }

// ----------------------------------------------------------------------------
// CImumMboxDataExtension::SetIsPredefined()
// ----------------------------------------------------------------------------
//
void CImumMboxDataExtension::SetIsPredefined(
    const TBool& aIsPredefined )
    {
    iFlags.ChangeFlag( EMailIsPredefined, aIsPredefined );
    }
// ----------------------------------------------------------------------------
// CImumMboxDataExtension::IsPredefined()
// ----------------------------------------------------------------------------
//
TBool CImumMboxDataExtension::IsPredefined() const
    {
    return iFlags.Flag( EMailIsPredefined );
    }

//  End of File
