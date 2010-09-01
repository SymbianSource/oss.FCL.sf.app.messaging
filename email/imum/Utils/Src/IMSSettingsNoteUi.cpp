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
* Description:  Contains functions to show notes in UI
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <ImumUtils.rsg>
#include <StringLoader.h>       // StringLoader
#include <aknnotewrappers.h>    // CAknConfirmationNote
#include <aknclearer.h>         // CAknLocalScreenClearer

#include "ImumInternalApiImpl.h"
#include "ImumInSettingsData.h"
#include "ImumInMailboxUtilitiesImpl.h"
#include "IMSSettingsNoteUi.h"
#include "EmailUtils.H"
#include "IMASPageIds.hrh"
#include "ImumPanic.h"
#include "IMSSettingsBaseUi.h"
#include "ImumUtilsLogging.h"
#include "EmailFeatureUtils.h"
#include "muiuflagger.h"

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
// CIMSSettingsNoteUi::CIMSSettingsNoteUi()
// ----------------------------------------------------------------------------
//
CIMSSettingsNoteUi::CIMSSettingsNoteUi(
    CImumInternalApiImpl& aMailboxApi,
    CIMSSettingsBaseUI& aDialog )
    :
    iDialog( aDialog ),
    iMailboxApi( aMailboxApi )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::CIMSSettingsNoteUi, 0, KLogUi );
    
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::~CIMSSettingsNoteUi()
// ----------------------------------------------------------------------------
//
CIMSSettingsNoteUi::~CIMSSettingsNoteUi()
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::~CIMSSettingsNoteUi, 0, KLogUi );
    
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ConstructL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsNoteUi::ConstructL()
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ConstructL, 0, KLogUi );
    
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::NewL()
// ----------------------------------------------------------------------------
//
CIMSSettingsNoteUi* CIMSSettingsNoteUi::NewL(
    CImumInternalApiImpl& aMailboxApi,
    CIMSSettingsBaseUI& aDialog )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::NewL, 0, utils, KLogUi );
    
    CIMSSettingsNoteUi* self = NewLC( aMailboxApi, aDialog );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::NewLC()
// ----------------------------------------------------------------------------
//
CIMSSettingsNoteUi* CIMSSettingsNoteUi::NewLC(
    CImumInternalApiImpl& aMailboxApi,
    CIMSSettingsBaseUI& aDialog )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::NewLC, 0, utils, KLogUi );
    
    CIMSSettingsNoteUi* self =
        new ( ELeave ) CIMSSettingsNoteUi( aMailboxApi, aDialog );
    CleanupStack::PushL( self );

    return self;
    }

/******************************************************************************

    STATIC FUNCTIONS

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::MakeStringLC()
// ----------------------------------------------------------------------------
//
HBufC* CIMSSettingsNoteUi::MakeStringLC( const TUint aTextResource )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::MakeStringLC, 0, utils, KLogUi );
    
    HBufC* prompt = StringLoader::LoadL( aTextResource );
    CleanupStack::PushL( prompt );

    return prompt;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::MakeString()
// ----------------------------------------------------------------------------
//
void CIMSSettingsNoteUi::MakeString(
    TDes& aDest,
    TInt aResourceId,
    TInt aValue )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::MakeString, 0, utils, KLogUi );

    // To minimize stack usage, use callers buffer to make the string instead
    // of using StringLoader::Format() and temporary buffer.

    // Read format string from resource
    aDest.Zero();
    StringLoader::Load( aDest, aResourceId );

    // Convert integer to text representation
    TBuf<sizeof(TInt) * 4> subs; // CSI: 47 # For conversion.
    subs.AppendNum( aValue );

    // Replace tag with supplied int
    _LIT(KTag, "%N");
    MakeString( aDest, KTag, subs );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::MakeString()
// ----------------------------------------------------------------------------
//
void CIMSSettingsNoteUi::MakeStringL(
    TDes& aDest,
    TInt aResourceId,
    const TDesC& aSubs )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::MakeString, 0, utils, KLogUi );

    HBufC* statusText = StringLoader::LoadL(
        aResourceId,
        aSubs,CCoeEnv::Static() );
    aDest.Copy(statusText->Des());
    delete statusText;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::MakeString()
// ----------------------------------------------------------------------------
//
void CIMSSettingsNoteUi::MakeString(
    TDes& aDest,
    const TDesC& aTag,
    const TDesC& aSubs )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::MakeString, 0, utils, KLogUi );

    // To minimize stack usage, use callers buffer to make the string instead
    // of using StringLoader::Format() and temporary buffer.

    // Replace tag with supplied text
    TInt pos = aDest.Find( aTag );
    if ( pos != KErrNotFound)
        {
        aDest.Delete( pos, aTag.Length() );
        TInt room = aDest.MaxLength() - aDest.Length();
        TInt insertLen = aSubs.Length() <= room ? aSubs.Length():room;
        const TPtrC replacement( aSubs.Ptr(),  insertLen );
        aDest.Insert( pos, replacement );
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowQuery
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CIMSSettingsNoteUi::ShowQueryL(
    const TUint aTextResource,
    const TInt  aDialogResource,
    const TBool& aClearBackground,
    const CAknQueryDialog::TTone aTone )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::ShowQueryL, 0, utils, KLogUi );
    
    // Get text to be shown
    HBufC* prompt = MakeStringLC( aTextResource );

    // Create dialog and execute the dialog
    TInt result = CIMSSettingsNoteUi::ShowQueryL(
        prompt->Des(), aDialogResource, aClearBackground, aTone );

    CleanupStack::PopAndDestroy( prompt );

    return result;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowQuery
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CIMSSettingsNoteUi::ShowQueryL(
    const TDesC& aText,
    const TInt  aDialogResource,
    const TBool& aClearBackground,
    const CAknQueryDialog::TTone aTone )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::ShowQueryL, 0, utils, KLogUi );
    
    CAknLocalScreenClearer* localScreenClearer = NULL;
    if ( aClearBackground )
        {
        localScreenClearer = CAknLocalScreenClearer::NewL( EFalse );
        CleanupStack::PushL( localScreenClearer );
        CEikonEnv::Static()->EikAppUi()->AddToStackL( 
        	localScreenClearer, ECoeStackPriorityDefault, 
        	ECoeStackFlagRefusesAllKeys|ECoeStackFlagRefusesFocus );
        CleanupStack::PushL( TCleanupItem( &ScreenClearerCleanupOperation, 
        	localScreenClearer ) );
        }     
    
    // Create dialog and execute the dialog
    CAknQueryDialog* dlg = CAknQueryDialog::NewL( aTone );
    TInt button = dlg->ExecuteLD( aDialogResource, aText );    
    
    if ( aClearBackground )
        {        
        CleanupStack::PopAndDestroy(); // CSI: 47 # TCleanupItem
        CleanupStack::PopAndDestroy( localScreenClearer );
        localScreenClearer = NULL;
        }
            
    return button;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ScreenClearerCleanupOperation
// ----------------------------------------------------------------------------
//
void CIMSSettingsNoteUi::ScreenClearerCleanupOperation( TAny* aScreenClearer )
	{
	CEikonEnv::Static()->EikAppUi()->RemoveFromStack( 
		static_cast<CAknLocalScreenClearer*>( aScreenClearer ) );
	}
    
// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowNoteL
// ----------------------------------------------------------------------------
//
EXPORT_C void CIMSSettingsNoteUi::ShowNoteL(
    const TUint      aResource,
    const TIMSNotes& aNoteType,
    const TBool      aNoteWaiting )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::ShowNoteL, 0, utils, KLogUi );
    
    // Get text to be shown
    HBufC* prompt = MakeStringLC( aResource );

    // Show the note
    CIMSSettingsNoteUi::ShowNoteL( *prompt, aNoteType, aNoteWaiting );

    CleanupStack::PopAndDestroy( prompt );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowNoteL
// ----------------------------------------------------------------------------
//
EXPORT_C void CIMSSettingsNoteUi::ShowNoteL(
    const TDesC&      aText,
    const TIMSNotes& aNoteType,
    const TBool      aNoteWaiting )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::ShowNoteL, 0, utils, KLogUi );

    CAknResourceNoteDialog* note = NewNoteL( aNoteType, aNoteWaiting );

    // Show the note
    note->ExecuteLD( aText );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowNoteL
// ----------------------------------------------------------------------------
//
EXPORT_C void CIMSSettingsNoteUi::ShowNoteL(
    TUint aResource,
    TIMSNotes aNoteType,
    TBool aNoteWaiting,
    TInt aTimeOut )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::ShowNoteL, 0, utils, KLogUi );
    
    // Get text to be shown
    HBufC* prompt = MakeStringLC( aResource );

    CAknResourceNoteDialog* note = NewNoteL( aNoteType, aNoteWaiting );
    note->SetTimeout( static_cast<CAknNoteDialog::TTimeout>( aTimeOut ) );                

    // Show the note
    note->ExecuteLD( *prompt );

    CleanupStack::PopAndDestroy( prompt );
    }

/******************************************************************************

    Error note ui

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowDialog()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::ShowDialog(
    const CMuiuSettingBase& aBaseItem,
    const TIMSErrors aError,
    const TDesC& aNewText )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowDialog, 0, KLogUi );
    
    TMuiuPageEventResult result = EMuiuPageEventResultApproved;
    TRAP_IGNORE( result = SelectDialogL( aBaseItem, aError, aNewText ) );
    /*TRAPD( err,
    // Show error note based on the id
    switch ( aBaseItem.iItemId.iUid )
        {
        case EIMAWProtocol:
            result = ShowProtocolErrorNoteL( aError );
            return result;

        case EIMASOutgoingEmailAddress:
        case EIMAWEmailAddress:
        case EIMASUserReplyTo:
            result = ShowEmailAddressErrorNoteL( aError );
            return result;

        case EIMAWReceivingServer:
        case EIMAWSendingServer:
        case EIMASIncomingMailServer:
        case EIMASOutgoingMailServer:
            result = ShowServerErrorNoteL( aError );
            return result;

        case EIMAWIap:
            result = ShowIapErrorNoteL( aError );
            return result;

        case EIMAWMailboxName:
            result = ShowWizMailboxNameErrorNoteL( aError );
            return result;

        case EIMASIncomingMailboxName:
            result = ShowSetMailboxNameErrorNoteL( aError, aNewText );
            return result;

        case EIMASIncomingProtocol:
            result = ShowProtocolReadOnlyErrorNoteL( aError );
            return result;

        case EIMASMailNotifications:
        case EIMASAORolling:
            result = ShowAlwaysOnlineErrorNoteL( aError );
            return result;

        case EIMASIncomingUserPwd:
        case EIMASIncomingUserName:
            result = ShowAlwaysOnlineRemovalQueryL( aError );
            return result;

        case EIMASLimitInbox:
        case EIMASLimitFolders:
            result = ShowFetchLimitErrorNoteL( aError );
            return result;

        default:
            break;
        }
    );*/

    return result;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::SelectDialogL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::SelectDialogL( 
    const CMuiuSettingBase& aBaseItem,
    const TIMSErrors aError,
    const TDesC& aNewText )
    {
    // Show error note based on the id
    switch ( aBaseItem.iItemId.iUid )
        {
        case EIMAWProtocol:
            return ShowProtocolErrorNoteL( aError );

        case EIMASOutgoingEmailAddress:
        case EIMAWEmailAddress:
        case EIMASUserReplyTo:
            return ShowEmailAddressErrorNoteL( aError );

        case EIMAWReceivingServer:
        case EIMAWSendingServer:
        case EIMASIncomingMailServer:
        case EIMASOutgoingMailServer:
            return ShowServerErrorNoteL( aError );

        case EIMAWIap:
            return ShowIapErrorNoteL( aError );

        case EIMAWMailboxName:
            return ShowWizMailboxNameErrorNoteL( aError );

        case EIMASIncomingMailboxName:
            return ShowSetMailboxNameErrorNoteL( aError, aNewText );

        case EIMASIncomingProtocol:
            return ShowProtocolReadOnlyErrorNoteL( aError );

        case EIMASMailNotifications:
        case EIMASAORolling:
            return ShowAlwaysOnlineErrorNoteL( aError );

        case EIMASIncomingUserPwd:
        case EIMASIncomingUserName:
            return ShowAlwaysOnlineRemovalQueryL( aError );

        case EIMASLimitInbox:
        case EIMASLimitFolders:
            return ShowFetchLimitErrorNoteL( aError );

        default:
            break;
        }

    return EMuiuPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowProtocolErrorNoteL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::ShowProtocolErrorNoteL(
    const TIMSErrors aError )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowProtocolErrorNoteL, 0, KLogUi );
    
    switch ( aError )
        {
        case EIMSItemEmpty:
            CIMSSettingsNoteUi::ShowQueryL(
                R_IMUM_WIZARD_INSERT_PROTOCOL, 
                R_EMAIL_INFORMATION_QUERY, 
                ETrue );
            return EMuiuPageEventResultDisapproved;

        default:
            break;
        }

    return EMuiuPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowEmailAddressErrorNoteL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::ShowEmailAddressErrorNoteL(
    const TIMSErrors aError )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowEmailAddressErrorNoteL, 0, KLogUi );
    
    // Show notes
    switch ( aError )
        {
        // Incorrect
        case EIMSItemInvalid:
            CIMSSettingsNoteUi::ShowNoteL(
                R_IMUM_EDIT_OWN_EMAIL_ADDRESS_ERROR_TEXT, 
                EIMSInformationNote, ETrue );
            return EMuiuPageEventResultDisapproved;

        // Not typed
        case EIMSItemEmpty:
            CIMSSettingsNoteUi::ShowQueryL(
                R_IMUM_WIZARD_INSERT_EMAIL, 
                R_EMAIL_INFORMATION_QUERY,
                ETrue );
            return EMuiuPageEventResultDisapproved;

        default:
            break;
        }

    return EMuiuPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowServerErrorNoteL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::ShowServerErrorNoteL(
    const TIMSErrors aError )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowServerErrorNoteL, 0, KLogUi );
    
    switch ( aError )
        {
        case EIMSNoError:
            return EMuiuPageEventResultApproved;

        default:
            break;
        }

    CIMSSettingsNoteUi::ShowQueryL(
        R_IMUM_WIZARD_INSERT_SERVER_ADDRESS, 
        R_EMAIL_INFORMATION_QUERY,
        ETrue );
    return EMuiuPageEventResultDisapproved;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowIapErrorNoteL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::ShowIapErrorNoteL(
    const TIMSErrors aError )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowIapErrorNoteL, 0, KLogUi );
    
    switch ( aError )
        {
        case EIMSNoError:
            return EMuiuPageEventResultApproved;

        default:
            break;
        }

    return EMuiuPageEventResultDisapproved;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowWizMailboxNameErrorNoteL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::ShowWizMailboxNameErrorNoteL(
    const TIMSErrors aError )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowWizMailboxNameErrorNoteL, 0, KLogUi );
    
    switch ( aError )
        {
        case EIMSMailboxNameEmpty:
            CIMSSettingsNoteUi::ShowQueryL(
                R_MBXS_SETTINGS_MAIL_NEWBOX_NAME_QRY, 
                R_EMAIL_INFORMATION_QUERY,
                ETrue );
            return EMuiuPageEventResultDisapproved;

        case EIMSMailboxNameExists:
            CIMSSettingsNoteUi::ShowQueryL(
                R_MBXS_SETTINGS_MAIL_NEWBOX_RENAME_QRY, 
                R_EMAIL_INFORMATION_QUERY,
                ETrue );
            return EMuiuPageEventResultDisapproved;

        default:
            break;
        }

    return EMuiuPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowMailboxAlreadyExistsErrorNoteL()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsNoteUi::ShowMailboxAlreadyExistsErrorNoteL(
    const TDesC& aMailboxName )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowMailboxAlreadyExistsErrorNoteL, 0, KLogUi );
    
    TMuiuSettingsText query;
    MakeStringL( query, R_IMUM_SETTINGS_RENAME_QUERY, aMailboxName );

    return ShowQueryL( query, R_EMAIL_CONFIRMATION_QUERY, ETrue );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowSetMailboxNameErrorNoteL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::ShowSetMailboxNameErrorNoteL(
    const TIMSErrors aError,
    const TDesC& aMailboxName )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowSetMailboxNameErrorNoteL, 0, KLogUi );
    
    switch ( aError )
        {
        case EIMSMailboxNameEmpty:
            CIMSSettingsNoteUi::ShowQueryL(
                R_MBXS_SETTINGS_MAIL_NEWBOX_NAME_QRY, 
                R_EMAIL_INFORMATION_QUERY,
                ETrue );
            return EMuiuPageEventResultDisapproved;

        case EIMSMailboxNameExists:
            return ShowMailboxAlreadyExistsErrorNoteL( aMailboxName ) ?
                EMuiuPageEventResultDisapproved : EMuiuPageEventResultCancel;

        default:
            break;
        }

    return EMuiuPageEventResultApproved;

    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowProtocolReadOnlyErrorNoteL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::ShowProtocolReadOnlyErrorNoteL(
    const TIMSErrors aError )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowProtocolReadOnlyErrorNoteL, 0, KLogUi );
    
    switch ( aError )
        {
        case EIMSItemInvalid:
            CIMSSettingsNoteUi::ShowNoteL( 
                R_IMUM_EDIT_PROTOCOL_ERROR_TEXT, EIMSErrorNote, EFalse );
            return EMuiuPageEventResultCancel;

        default:
            break;
        }

    return EMuiuPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowAlwaysOnlineErrorNoteL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::ShowAlwaysOnlineErrorNoteL(
    const TIMSErrors aError )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowAlwaysOnlineErrorNoteL, 0, KLogUi );
    
    switch ( aError )
        {
        // Access point is set to always ask
        case EIMSAoAlwaysAsk:
            CIMSSettingsNoteUi::ShowQueryL(
                R_MCE_SETTINGS_ALWAYS_DEFINE_AP, 
                R_EMAIL_INFORMATION_QUERY,
                ETrue );
            return EMuiuPageEventResultCancel;

        // Access point prompts password
        case EIMSAoIAPPassword:
            CIMSSettingsNoteUi::ShowQueryL(
                R_MCE_SETTINGS_ALWAYS_PROMPTACC, 
                R_EMAIL_INFORMATION_QUERY,
                ETrue );
            return EMuiuPageEventResultCancel;

        // No username or password
        case EIMSAoNoLogin:
            CIMSSettingsNoteUi::ShowQueryL(
                R_IMUM_SETTINGS_ALWAYS_PROMPTMAIL_SRVR_PSWD,
                R_EMAIL_INFORMATION_QUERY,
                ETrue );
            return EMuiuPageEventResultCancel;

        // Always online is on
        case EIMSOmaEmnAoOn:
            CIMSSettingsNoteUi::ShowQueryL(
                R_MBXS_SETTINGS_AOL_OFF_QRY, 
                R_EMAIL_INFORMATION_QUERY,
                ETrue );
            return EMuiuPageEventResultCancel;

        // OMA emn is on
        case EIMSAoOmaEmnOn:
            CIMSSettingsNoteUi::ShowQueryL(
                R_MBXS_SETTINGS_OMA_OFF_QRY, 
                R_EMAIL_INFORMATION_QUERY,
                ETrue );
            return EMuiuPageEventResultCancel;

        // Too many always online mailboxes
        case EIMSAoManyAlwaysOnline:
            CIMSSettingsNoteUi::ShowQueryL(
                R_MCE_SETTINGS_ALWAYS_MAXLIMIT, R_EMAIL_INFORMATION_QUERY );
            return EMuiuPageEventResultCancel;

        // If fetch limit is something else than all, query user
        case EIMSAoFetchNotAccepted:
            if ( iMailboxApi.Flags().LF( EmailFeatureAlwaysonlineHeaders ) )
                {
                return CIMSSettingsNoteUi::ShowQueryL(
                    R_MCE_SETTINGS_MAIL_ALL_MAILS_FET2,
                    R_EMAIL_CONFIRMATION_QUERY,
                    ETrue ) ?
                        EMuiuPageEventResultApproved : EMuiuPageEventResultCancel;
                }
            break;
        // Access Point type is CSD
        case EIMSAoIapTypeCSD:
            return ShowAoCsdIapError();

        // Leave occurred
        case EIMSItemInvalid:
            return EMuiuPageEventResultCancel;

        default:
            break;
        }

    return EMuiuPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowAoCsdIapError()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::ShowAoCsdIapError()
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowAoCsdIapError, 0, KLogUi );
    
    TMsvEntry mailbox;
     // Get the mailbox
    TRAP_IGNORE( mailbox = iMailboxApi.MailboxUtilitiesL().GetMailboxEntryL(
        iDialog.CurrentMailboxId(), 
        MImumInMailboxUtilities::ERequestReceiving ) );

    TMuiuSettingsText infonote;
    TRAP_IGNORE( MakeStringL( infonote, R_MCE_AUTO_CONN_INCORRECT_AP,
                 mailbox.iDetails ) );

    // Show the text to user
    TRAP_IGNORE( CIMSSettingsNoteUi::ShowQueryL(
        infonote, R_EMAIL_INFORMATION_QUERY, ETrue ) );
 
    return EMuiuPageEventResultCancel;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowFetchLimitErrorNoteL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::ShowFetchLimitErrorNoteL(
    const TIMSErrors aError )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowFetchLimitErrorNoteL, 0, KLogUi );
    
    switch ( aError )
        {
        case EIMSItemInvalid:
            if ( iMailboxApi.Flags().LF( EmailFeatureAlwaysonlineHeaders ) )
                {
                CIMSSettingsNoteUi::ShowQueryL(
                    R_MCE_SETTINGS_MAIL_ALL_MAILS_FET, 
                    R_EMAIL_INFORMATION_QUERY,
                    ETrue );
                return EMuiuPageEventResultDisapproved;
                }
            break;
        default:
            break;
        }

    return EMuiuPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowAlwaysOnlineRemovalQueryL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsNoteUi::ShowAlwaysOnlineRemovalQueryL(
    const TIMSErrors aError )
    {
    IMUM_CONTEXT( CIMSSettingsNoteUi::ShowAlwaysOnlineRemovalQueryL, 0, KLogUi );
    
    switch ( aError )
        {
        // Indicate to the user, that it's unwise to remove login information
        case EIMSAoNoLogin:
            return CIMSSettingsNoteUi::ShowQueryL(
                R_MCE_ALWAYS_CHANGEPROMPT, 
                R_EMAIL_CONFIRMATION_QUERY,
                ETrue ) ?
                EMuiuPageEventResultApproved : EMuiuPageEventResultCancel;

        default:
            break;
        }

    return EMuiuPageEventResultApproved;
    }

// -----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowPopupLC
// -----------------------------------------------------------------------------
//
CAknInfoPopupNoteController* CIMSSettingsNoteUi::ShowPopupLC(
    const TInt aResource,
    const TInt aAfterMs,
    const TInt aPopupVisibleTimeMs )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::ShowPopupLC, 0, utils, KLogUi );
    
    CAknInfoPopupNoteController* noteDialog =
        CAknInfoPopupNoteController::NewL();
    CleanupStack::PushL( noteDialog );
    noteDialog->SetTimePopupInView( aPopupVisibleTimeMs );
    HBufC* prompt = StringLoader::LoadL( aResource );
    CleanupStack::PushL( prompt );
    noteDialog->SetTextL( prompt->Des() );
    CleanupStack::PopAndDestroy( prompt );
    noteDialog->SetTimeDelayBeforeShow( aAfterMs );
    noteDialog->ShowInfoPopupNote();
    return noteDialog;
    }


// -----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowPopupL
// -----------------------------------------------------------------------------
//
CAknInfoPopupNoteController* CIMSSettingsNoteUi::ShowPopupL(
    const TInt aResource,
    const TInt aAfterMilliSeconds,
    const TInt aPopupVisibleTime )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::ShowPopupL, 0, utils, KLogUi );
    
    CAknInfoPopupNoteController* noteDialog = ShowPopupLC(
        aResource, aAfterMilliSeconds, aPopupVisibleTime );
    CleanupStack::Pop( noteDialog );
    return noteDialog;
    }

// -----------------------------------------------------------------------------
// CIMSSettingsNoteUi::NewNoteL
// -----------------------------------------------------------------------------
//
CAknResourceNoteDialog* CIMSSettingsNoteUi::NewNoteL( TIMSNotes aNoteType, TBool aNoteWaiting )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsNoteUi::NewNoteL, 0, utils, KLogUi );
    
    switch( aNoteType )
        {
        // Confirmation note
        case EIMSConfirmationNote:
            return new ( ELeave ) CAknConfirmationNote( aNoteWaiting );
        // Information note
        case EIMSInformationNote:
            return new ( ELeave ) CAknInformationNote( aNoteWaiting );
        // Error note
        case EIMSErrorNote:
            return new ( ELeave ) CAknErrorNote( aNoteWaiting );
        // Warning note
        case EIMSWarningNote:
            return new ( ELeave ) CAknWarningNote( aNoteWaiting );
        // Wrong note given, leave
        default:
            User::Leave( KErrUnknown );
            return NULL;
        }
    }
