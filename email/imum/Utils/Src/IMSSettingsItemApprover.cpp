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
* Description:  IMSSettingsItemApprover.cpp
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <msvstd.hrh>                   // KUidMsvServiceEntryValue
#include <MuiuMsvUiServiceUtilities.h>  // MsvUiServiceUtilities
#include <StringLoader.h>               // StringLoader
#include <ImumUtils.rsg>
#include "MuiuDynamicSettingItemBase.h"      // CMuiuSettingBase
#include "MuiuDynamicSettingItemEditValue.h" // CMuiuSettingEditValue
#include "MuiuDynamicSettingItemEditText.h"  // CMuiuSettingEditText
#include "MuiuDynamicSettingItemLink.h"      // CMuiuSettingLink
#include "MuiuDynamicSettingItemExtLink.h"   // CMuiuSettingExtLink

#include "IMSSettingsItemApprover.h"
#include "ImumInMailboxUtilitiesImpl.h"
#include "ImumInternalApi.h"
#include "ImumInSettingsData.h"
#include "IMSSettingsDialog.h"
#include "EmailUtils.H"
#include "IMASPageIds.hrh"
#include "EmailFeatureUtils.h"
#include "Imas.hrh"
#include "ImumUtilsLogging.h"
#include "ImumPanic.h"


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
// CIMSSettingsItemApprover::CIMSSettingsItemApprover()
// ----------------------------------------------------------------------------
CIMSSettingsItemApprover::CIMSSettingsItemApprover(
    CImumInternalApi& aMailboxApi,
    CMsvCommDbUtilities& aCommsDb,
    CIMSSettingsNoteUi& aNoteUi,
    CIMSSettingsBaseUI& aDialog )
    :
    iMailboxApi( aMailboxApi ),
    iCommsDbUtils( aCommsDb ),
    iNoteUi( aNoteUi ),
    iDialog( aDialog ),
    iTempText( NULL )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::CIMSSettingsItemApprover, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::ConstructL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsItemApprover::ConstructL()
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::ConstructL, 0, KLogUi );

    iTempText = new ( ELeave ) TMuiuSettingsText();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::~CIMSSettingsItemApprover()
// ----------------------------------------------------------------------------
//
CIMSSettingsItemApprover::~CIMSSettingsItemApprover()
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::~CIMSSettingsItemApprover, 0, KLogUi );

    delete iTempText;
    iTempText = NULL;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::NewL()
// ----------------------------------------------------------------------------
//
CIMSSettingsItemApprover* CIMSSettingsItemApprover::NewL(
    CImumInternalApi& aMailboxApi,
    CMsvCommDbUtilities& aCommsDb,
    CIMSSettingsNoteUi& aNoteUi,
    CIMSSettingsBaseUI& aDialog )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsItemApprover::NewL, 0, utils, KLogUi );

    CIMSSettingsItemApprover* self = NewLC(
        aMailboxApi, aCommsDb, aNoteUi, aDialog );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::NewLC()
// ----------------------------------------------------------------------------
//
CIMSSettingsItemApprover* CIMSSettingsItemApprover::NewLC(
    CImumInternalApi& aMailboxApi,
    CMsvCommDbUtilities& aCommsDb,
    CIMSSettingsNoteUi& aNoteUi,
    CIMSSettingsBaseUI& aDialog )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsItemApprover::NewLC, 0, utils, KLogUi );

    CIMSSettingsItemApprover* self =
        new ( ELeave ) CIMSSettingsItemApprover(
            aMailboxApi, aCommsDb, aNoteUi, aDialog );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

/******************************************************************************

    Common item validating

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::ValidateType()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::ValidateType(
    const CMuiuSettingBase& aBaseItem,
    TDes& aNewText,
    TInt& aNewValue )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::ValidateType, 0, KLogUi );
    IMUM_IN();

    // Handle the item, based on the type
    switch ( aBaseItem.Type() )
        {
        // Text editor
        case EMuiuDynSetItemEditText:
        	IMUM_OUT();
            return ValidateText( aBaseItem, aNewText );

        // Number editor
        case EMuiuDynSetCheckBoxArray:
        case EMuiuDynSetItemEditValue:
        	IMUM_OUT();
            return ValidateValue( aBaseItem, aNewValue );

        case EMuiuDynSetRadioButtonArray:
        	IMUM_OUT();
            return ValidateRadioButtons( aNewValue );

        // Type is not recognized or not supported
        default:
            break;
        }

    IMUM_OUT();
    return EIMSNoError;
    }

/******************************************************************************

    Special item validating

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::EvaluateAccountName()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::EvaluateAccountName(
    const CMuiuSettingBase& aBaseItem,
    TDes& aNewText )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::EvaluateAccountName, 0, KLogUi );

    // First make basic test
    TIMSErrors result = ValidateText( aBaseItem, aNewText );

    // Setting has been written, try to check
    if ( result == EIMSNoError )
        {
        // Do not allow leaving, as it might interrupt initialization phase
        TRAPD( error,
            MsvEmailMtmUiUtils::CheckAccountNameL(
                iMailboxApi,
                aNewText,
                iDialog.CurrentMailboxId(),
                aBaseItem.iItemFlags.Flag( EIMSApproverGenerate ) ) );
        result = ( error == KErrNone ) ? EIMSNoError : EIMSMailboxNameExists;
        }
    else
        {
        result = EIMSMailboxNameEmpty;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::EvaluateServerAddress()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::EvaluateServerAddress(
    const CMuiuSettingBase& aBaseItem,
    const TDesC& aNewText )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::EvaluateServerAddress, 0, KLogUi );

    TIMSErrors result = ValidateText( aBaseItem, aNewText );

    // Check that dot isn't in start of the server address
    if ( result == EIMSNoError )
        {
        TBool ok = EFalse;
        TRAPD( error, ok = MsvUiServiceUtilities::IsValidDomainL( aNewText ) );

        result = ( error == KErrNone ) && ok ? EIMSNoError : EIMSItemInvalid;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::EvaluateEmailAddress()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::EvaluateEmailAddress(
    const CMuiuSettingBase& aBaseItem,
    const TDesC& aNewText )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::EvaluateEmailAddress, 0, KLogUi );

    // First make basic test
    TIMSErrors result = ValidateText( aBaseItem, aNewText );

    // If text check was successful and the address is given, validate the
    // email address
    if ( result == EIMSNoError && aNewText.Length() )
        {
        // Trap any errors to prevent leaving
        TBool ok = EFalse;
        TRAPD( error, ok =
            MsvUiServiceUtilities::IsValidEmailAddressL( aNewText ) );

        result = ( ok && error == KErrNone ) ? EIMSNoError : EIMSItemInvalid;
        }

    return result;
    }

/******************************************************************************

    Brand new validating code

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::EvaluateSettingItems()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::EvaluateSettingItems(
    TIMSApproverEvent& aEvent,
    const CMuiuSettingBase& aBaseItem,
    TDes& aNewText,
    TInt& aNewValue )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::EvaluateSettingItems, 0, KLogUi );

    // Incoming settings
    switch ( aBaseItem.iItemId.iUid )
        {
        // No validating
        case EIMASUserFolderSubscription:
        	break;

        // Special validating
        case EIMASUserUnixPath:
        case EIMASUserOwnName:
        	return EvaluateFreeText( aNewText );

        case EIMASIncomingMailboxName:
            return EvaluateAccountName( aBaseItem, aNewText );

        case EIMASIncomingMailServer:
        case EIMASOutgoingMailServer:
            return EvaluateServerAddress( aBaseItem, aNewText );

        case EIMASOutgoingEmailAddress:
        case EIMASUserReplyTo:
            return EvaluateEmailAddress( aBaseItem, aNewText );

        // When clearing the username and password, it must be checked
        // that the always online and mail message notification is off.
        // Otherwise user must be reminded of always online or mail message
        // notification requiring username and password in connection.
        case EIMASIncomingUserPwd:
        case EIMASIncomingUserName:
            return EvaluateUsernameOrPassword( aEvent, aNewText );

        // If the OMA EMN is attempted to turn on, check that
        // current state of settings allows it
        case EIMASMailNotifications:
            return CheckItemOnlineSettingsCheckTurnOn(
                aEvent, aNewValue, ETrue );

        // If the Always Online is attempted to turn on, check that
        // current state of settings allows it
        case EIMASAORolling:
            return CheckItemOnlineSettingsCheckTurnOn(
                aEvent, aNewValue, EFalse );

        case EIMASAODays:
            break;

        case EIMASAOHours:
            break;

        default:
            break;
        }

    return ValidateType( aBaseItem, aNewText, aNewValue );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::EvaluateWizardItems()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::EvaluateWizardItems(
    TIMSApproverEvent& /* aEvent */,
    const CMuiuSettingBase& aBaseItem,
    TDes& aNewText,
    TInt& aNewValue )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::EvaluateWizardItems, 0, KLogUi );

    switch ( aBaseItem.iItemId.iUid )
        {
        case EIMAWEmailAddress:
            return EvaluateEmailAddress( aBaseItem, aNewText );

        case EIMAWSendingServer:
        case EIMAWReceivingServer:
            return EvaluateServerAddress( aBaseItem, aNewText );

        case EIMAWMailboxName:
            return EvaluateAccountName( aBaseItem, aNewText );

        case EIMAWProtocol:
        case EIMAWIap:
        default:
            break;
        }

    return ValidateType( aBaseItem, aNewText, aNewValue );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::EvaluateItem()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsItemApprover::EvaluateItem(
    TIMSApproverEvent& aEvent,
    const CMuiuSettingBase& aBaseItem,
    TDes& aNewText,
    TInt& aNewValue )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::EvaluateItem, 0, KLogUi );

    TIMSErrors error = EIMSNoError;

    if ( iDialog.Flag( EImumSettingsWizard ) )
        {
        error = EvaluateWizardItems( aEvent, aBaseItem, aNewText, aNewValue );
        }
    else
        {
        error = EvaluateSettingItems( aEvent, aBaseItem, aNewText, aNewValue );
        }

    return iNoteUi.ShowDialog( aBaseItem, error, aNewText );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::EvaluateText()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsItemApprover::EvaluateText(
    TIMSApproverEvent& aEvent,
    const CMuiuSettingBase& aBaseItem,
    TDes& aNewText )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::EvaluateText, 0, KLogUi );

    aEvent = EIMSApproverNoEvent;
    TInt nothing = 0;
    return EvaluateItem( aEvent, aBaseItem, aNewText, nothing );
    }

 // ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::EvaluateValue()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsItemApprover::EvaluateValue(
    TIMSApproverEvent& aEvent,
    const CMuiuSettingBase& aBaseItem,
    TInt& aNewValue )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::EvaluateValue, 0, KLogUi );

    aEvent = EIMSApproverNoEvent;
    TMuiuSettingsText nothing;
    return EvaluateItem( aEvent, aBaseItem, nothing, aNewValue );
    }

/******************************************************************************

    Common code

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::IsItemFilled()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsItemApprover::IsItemFilled(
    const CMuiuSettingBase& aBaseItem,
    const TInt aLength )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::IsItemFilled, 0, KLogUi );

    // At first, check if the item can be left empty
    TBool ok =
        !( aBaseItem.iItemFlags.Flag( EMuiuDynItemMustFill ) ||
           aBaseItem.iItemFlags.Flag( EMuiuDynItemForceMustFill ) );

    // The text field is considered filled, when:
    // A: Item has NO must fill -flags on
    // B: Item has must fill -flags on and length is higher than 0
    return ok || ( !ok && aLength > 0 );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::ValidateRadioButton()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::ValidateRadioButtons(
    const TInt aNewValue )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::ValidateRadioButtons, 0, KLogUi );
    IMUM_IN();
    IMUM_OUT();

    // Make sure item is checked
    return ( aNewValue != KErrNotFound ) ? EIMSNoError : EIMSItemEmpty;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::ValidateValue()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::ValidateValue(
    const CMuiuSettingBase& aBaseItem,
    TInt& aNewValue )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::ValidateValue, 0, KLogUi );
    IMUM_IN();
    IMUM_OUT();

    // This check makes sure, that the provided value is larger than 0
    return IsItemFilled( aBaseItem, aNewValue ) ?
        EIMSNoError : EIMSItemInvalid;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::ValidateTextL()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::ValidateText(
    const CMuiuSettingBase& aBaseItem,
    const TDesC& aNewText )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::ValidateText, 0, KLogUi );
    IMUM_IN();
    IMUM_OUT();

    TBool result = IsItemFilled( aBaseItem, aNewText.Length() );

    if( result )
    	{
    	// Checks if the name of the mailbox contains only
    	// white space characters.
    	TInt length = aNewText.Length();

		// we should not test empty string
		// result was that the item is filled, but it may
		// still be allowed to be empty
    	if( !length )
    		{
    		return EIMSNoError;
    		}

    	TUint currentAscii;

    	for( TInt i = 0 ; i < length ; i++ )
    		{
    		currentAscii = aNewText[i];
    		TChar tempChar( currentAscii );

    		if( !tempChar.IsSpace() )
    			{
    			// If the character isn't a white space character
    			// then EIMSNoError will be returned.
    			return EIMSNoError;
    			}
    		}
    		return EIMSItemEmpty;
    	}
    else
    	{
    	return EIMSItemEmpty;
    	}
    }

/******************************************************************************

    Individual item code

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::EvaluateUsernameOrPassword()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::EvaluateUsernameOrPassword(
    TIMSApproverEvent& aEvent,
    TDes& aNewText )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::EvaluateUsernameOrPassword, 0, KLogUi );

    TBool ok = aNewText.Length() > 0;
    TBool isAoOn = iDialog.Flag( EDialogAlwaysOnlineOn );
    TBool isEmnOn = iDialog.Flag( EDialogOmaEmnOn );

    MsvEmailMtmUiUtils::StripCharacters( aNewText );

    // If the field is deleted and always online or oma emn is on
    if ( !ok && ( isAoOn || isEmnOn ) )
        {
        // Prepare the correct event
        aEvent = isAoOn ?
            EIMSApproverRemoveAlwaysOnline : EIMSApproverRemoveOmaEmn;
        return EIMSAoNoLogin;
        }

    return EIMSNoError;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::EvaluateFreeText()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::EvaluateFreeText( TDes& aNewText )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::EvaluateFreeText, 0, KLogUi );
    IMUM_IN();

    MsvEmailMtmUiUtils::StripCharacters( aNewText );

    IMUM_OUT();
    return EIMSNoError;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::CheckItemAOCheckTurnOn()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::CheckItemOnlineSettingsCheckTurnOn(
    TIMSApproverEvent& aEvent,
    const TInt aNewValue,
    TBool aIsEmn )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::CheckItemOnlineSettingsCheckTurnOn, 0, KLogUi );

    TIMSErrors result = EIMSNoError;

    // If Always Online or OMA EMN state is changed from off to on, check that
    // the current state of settings allows it
    TBool ao = ( aNewValue != EMailAoOff &&
        !iDialog.Flag( EDialogAlwaysOnlineOn ) && !aIsEmn );

    TBool emn = ( aNewValue != EMailEmnOff &&
        !iDialog.Flag( EDialogOmaEmnOn ) && aIsEmn );

    if ( ao || emn )
        {
        TRAPD( error,
            result = CheckItemOnlineSettingsCheckTurnOnL( aEvent, aIsEmn ) );

        // If leave occurred, don't accept the setting
        if ( error != KErrNone )
            {
            // Panic on debug to spot any errors
            __ASSERT_DEBUG( EFalse, User::Panic(
                KIMSSettingsApproverPanic, error ) );

            aEvent = EIMSApproverNoEvent;
            result = EIMSItemInvalid;
            }
        }
    // Always online or OMA EMN is turned off, or the on state is changed in
    // case of always online, simply continue
    else
        {
        result = EIMSNoError;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::CheckItemOnlineSettingsCheckTurnOnL()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::CheckItemOnlineSettingsCheckTurnOnL(
    TIMSApproverEvent& aEvent,
    TBool aIsEmn )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::CheckItemOnlineSettingsCheckTurnOnL, 0, KLogUi );

    // Make packet Data check
    TIMSErrors result = CheckItemAoConnectionTypeL();

    // Number of access point mailboxes has to be less than 2
    if ( result == EIMSNoError && !aIsEmn )
        {
        result = CheckItemAoCountOkL();
        }

    // Always ask not acceptable, require valid access point
    // IAP user name and password check is not allowed
    if ( result == EIMSNoError )
        {
        result = CheckItemAoAccessPointOkL();
        }

    // Oma EMN not allowed for the AO mailbox
    // AO not allowed for the Oma EMN mailbox
    if ( result == EIMSNoError )
        {
        result = CheckItemAoOmaEmnOffL( aIsEmn );
        }

    // Check for incoming username and password
    if ( result == EIMSNoError )
        {
        result = CheckItemAoUsernameAndPasswordL();
        }

    // Query the user, if the header limit needs to be ignored
    if ( result == EIMSNoError && !aIsEmn )
        {
        result = CheckItemAOFixFetchL();
        }

    aEvent = EIMSApproverNoEvent;

    return result;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::CheckItemAoCountOkL()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::CheckItemAoCountOkL()
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::CheckItemAoCountOkL, 0, KLogUi );

    // Number of always online mailboxes has to be less than 2
    TBool on = MsvEmailMtmUiUtils::CheckActiveAOLimitsL(
        iMailboxApi, iDialog.CurrentMailboxId() );

    // Show error note in case of any errors
    if ( !on )
        {
        return EIMSAoManyAlwaysOnline;
        }

    return EIMSNoError;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::CheckItemAoAccessPointOkL()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::CheckItemAoAccessPointOkL()
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::CheckItemAoAccessPointOkL, 0, KLogUi );
    TIMSErrors result = EIMSNoError;

    return result;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::CheckItemAoOmaEmnOffL()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::CheckItemAoOmaEmnOffL( TBool aIsEmn )
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::CheckItemAoOmaEmnOffL, 0, KLogUi );

    TIMSErrors result = EIMSNoError;

    // AO is tried to turn on
    if ( !aIsEmn )
        {
        CMuiuSettingsLinkExtended* omaEmnNotification = NULL;
        User::LeaveIfError( iDialog.FindItem(
            omaEmnNotification, TUid::Uid( EIMASMailNotifications ) ) );

        // Show error note in case of any errors
        if ( omaEmnNotification->Value() != EMailEmnOff )
            {
            result = EIMSAoOmaEmnOn;
            }
        }
    // OMA EMN is tried to turn on
    else
        {
        CMuiuSettingsLinkExtended* ao = NULL;
        User::LeaveIfError( iDialog.FindItem(
            ao, TUid::Uid( EIMASAORolling ) ) );

        // Show error note in case of any errors
        if ( ao->Value() != EMailAoOff )
            {
            result = EIMSOmaEmnAoOn;
            }
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::CheckItemAoUsernameAndPasswordL()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::CheckItemAoUsernameAndPasswordL()
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::CheckItemAoUsernameAndPasswordL, 0, KLogUi );

    // Initialize incoming settings array before accessing username and
    // password
    CMuiuSettingsLinkExtended* username = NULL;
    CMuiuSettingsLinkExtended* password = NULL;
    User::LeaveIfError( iDialog.FindItem(
        username, TUid::Uid( EIMASIncomingUserName ) ) );
    User::LeaveIfError( iDialog.FindItem(
        password, TUid::Uid( EIMASIncomingUserPwd ) ) );

    TBool on = password->Text() && password->Text()->Length() > 0 &&
        username->Text() && username->Text()->Length() > 0;

    // Show error note in case of any errors
    if ( !on )
        {
        return EIMSAoNoLogin;
        }

    return EIMSNoError;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::CheckItemAoConnectionTypeL()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::CheckItemAoConnectionTypeL()
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::CheckItemAoConnectionTypeL, 0, KLogUi );

    if ( !iDialog.Flags().LF( EMailFeatureAlwaysOnlineCSD ) )
        {
        // Initialize incoming settings array before accessing
        // incoming accesspoints
        CIMSSettingsAccessPointItem* iapItem = NULL;
        User::LeaveIfError( iDialog.FindItem(
            iapItem, TUid::Uid( EIMASIncomingIap ) ) );

        if ( iCommsDbUtils.IsApBearerCircuitSwitchedL( iapItem->iIap.iId ) )
            {
            return EIMSAoIapTypeCSD;
            }
        }

    return EIMSNoError;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsItemApprover::CheckItemAOFixFetchL()
// ----------------------------------------------------------------------------
//
TIMSErrors CIMSSettingsItemApprover::CheckItemAOFixFetchL()
    {
    IMUM_CONTEXT( CIMSSettingsItemApprover::CheckItemAOFixFetchL, 0, KLogUi );

    // Turning on the always online requires of fetching all the mails,
    // without header limits. This means the header limit is ignored, which
    // has to be queried.
    TIMSErrors result = EIMSNoError;

    // Initalize fetch headers array
    CMuiuSettingsLinkExtended* inboxLimit = NULL;
    CMuiuSettingsLinkExtended* folderLimit = NULL;
    User::LeaveIfError( iDialog.FindItem(
        inboxLimit, TUid::Uid( EIMASLimitInbox ) ) );
    User::LeaveIfError( iDialog.FindItem(
        folderLimit, TUid::Uid( EIMASLimitFolders ) ) );

    // Check if the value in either of the settings is something else
    // than all
    TBool inboxOk = !inboxLimit->Value();
    TBool folderOk = !folderLimit->Value();
    TBool hasFolders = iMailboxApi.MailboxUtilitiesL().HasSubscribedFoldersL(
        iDialog.CurrentMailboxId() );

    // If inbox retrieval limit is not all, or user has subscribed folders
    // and limit is not all
    if ( !inboxOk || ( hasFolders && !folderOk ) )
        {
        // Show the note
        result = EIMSAoFetchNotAccepted;
        }

    inboxLimit = NULL;
    folderLimit = NULL;

    return result;
    }


//  End of File
