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
* Description:  IMSSettingsDialog.cpp
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <avkon.hrh>
#include <muiuflagger.h>
#include <ImumUtils.rsg>
#include <miutset.h>                // CImBaseEmailSettings
#include <smtpset.h>                // CImSmtpSettings
#include <pop3set.h>                // CImPop3Settings
#include <imapset.h>                // CImImap4Settings
#include <iapprefs.h>               // CImIAPPreferences
#include <AknQueryDialog.h>         // CAknMultiLineDataQueryDialog
#include <charconv.h>               // CCnvCharacterSetConverter
#include <aknradiobuttonsettingpage.h> // CAknRadioButtonSettingPage
#include <muiu_internal.rsg>
#include <AknUtils.h>				//AknTextUtils
#include "ComDbUtl.h"

#include <cmapplicationsettingsui.h>

#include "ImumInternalApiImpl.h"
#include "ImumMboxSymbianDataConverter.h"
#include "ImumMboxInternalDataConverter.h"
#include "EmailFeatureUtils.h"
#include "Imas.hrh"
#include "ImumInSettingsData.h"
#include "IMSSettingsDialog.h"
#include "IMSSettingsNoteUi.h"
#include "IMSFolderSubscription.h"
#include "ImumUtilsLogging.h"
#include <csxhelp/mbxs.hlp.hrh>
#include "ImumPanic.h"
#include "ImumInSettingsData.h"
#include "ImumConstants.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TInt KIMASEveryDayMask = 0x7f;
const TInt KIMSDefaultSizeKB = 1;
const TInt KIMSMaxNumValue = 999;

enum TIMSRetrieveLimit
    {
    EIMSLimitFetchAll = 0,
    EIMSLimitUserDefined
    };

// MACROS
#define DFILLBUF( a, b ) buffer.Copy( a.Left( b ) )
#define DITEM( a ) ( *array )[a].iItem

// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
typedef CCnvCharacterSetConverter::SCharacterSet CIMACharConvItem;
typedef CArrayFix<CIMACharConvItem> CIMACharConvList;

// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

/******************************************************************************

    Constructors & Destructor

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::CIMSSettingsDialog()
// ----------------------------------------------------------------------------
CIMSSettingsDialog::CIMSSettingsDialog(
    CIMSSettingsUi& aDialog,
    CImumInternalApiImpl& aMailboxApi,
    CEikFormattedCellListBox& aListBox,
    CAknTitlePane& aTitlePane,
    CMuiuFlags& aFlags )
    :
    CIMSSettingsBaseUI(
        aDialog, aMailboxApi, aListBox,
        aTitlePane, aFlags ),
    iInitArray( NULL ),
    iInitSetFlag( EDialogLastFlag ),
    iInitCurrentIndex( KErrNotFound ),
    iInitResource( 0 )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::CIMSSettingsDialog, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::ConstructL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::ConstructL(
    CEikButtonGroupContainer& aSoftkeys,
    CImumInSettingsData& aMailboxSettings )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::ConstructL, 0, KLogUi );

    iIdle = CIdle::NewL( CActive::EPriorityHigh );

    // Add the complete menu tree to settings
    BaseUiConstructL( aSoftkeys, R_IMAS_MAIN_MENU, ETrue );

    // Clear the flags
    for ( TInt flag = EDialogLastFlag; --flag >= 0; )
        {
        Flag( flag );
        }

    // Create default data set
    iAccountSettings =
        CImumMboxSymbianDataConverter::ConvertToSymbianMboxDataLC(
            iMailboxApi, aMailboxSettings );
    CleanupStack::Pop( iAccountSettings );

    // Set always online flag
    ChangeFlag( EDialogAlwaysOnlineOn,
        iAccountSettings->iExtendedSettings->AlwaysOnlineState() !=
            EMailAoOff );

    // Set mail message notification flag
    ChangeFlag( EDialogOmaEmnOn,
        iAccountSettings->iExtendedSettings->EmailNotificationState() !=
            EMailEmnOff );

    // Prepare the init arrays
    iInitTreeArray =
        new ( ELeave ) CMuiuDynInitItemIdArray( KMuiuDynArrayGranularity );
    iInitIdArray =
        new ( ELeave ) CMuiuDynInitItemIdArray( KMuiuDynArrayGranularity );
    iInitValueArray =
        new ( ELeave ) CMuiuDynInitItemValueArray(
            KMuiuDynArrayGranularity );
    iInitTextArray =
        new ( ELeave ) CMuiuDynInitItemTextArray(
            KMuiuDynArrayGranularity );
    iInitInfoArray =
        new ( ELeave ) CMuiuDynInitInfoArray( KMuiuDynArrayGranularity );

    // The main menu is about to open, so initialize it
    InitializeArrayMainMenu();

    // Set the titlepane text
    SetTitlePaneTextL( iAccountSettings->iName, ETrue );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::~CIMSSettingsDialog()
// ----------------------------------------------------------------------------
//
CIMSSettingsDialog::~CIMSSettingsDialog()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::~CIMSSettingsDialog, 0, KLogUi );

    delete iItemApprover;
    iItemApprover = NULL;
    delete iInitArray;
    iInitArray = NULL;
    delete iTempText;
    iTempText = NULL;
    delete iInitTreeArray;
    iInitTreeArray = NULL;
    delete iInitIdArray;
    iInitIdArray = NULL;
    delete iInitValueArray;
    iInitValueArray = NULL;
    delete iInitInfoArray;
    iInitInfoArray = NULL;

    if ( iInitTextArray )
        {
        iInitTextArray->ResetAndDestroy();
        }

    delete iInitTextArray;
    iInitTextArray = NULL;
    delete iAccountSettings;
    iAccountSettings = NULL;
    delete iIdle;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::NewL()
// ----------------------------------------------------------------------------
//
CIMSSettingsDialog* CIMSSettingsDialog::NewL(
    CIMSSettingsUi& aDialog,
    CImumInternalApiImpl& aMailboxApi,
    CEikFormattedCellListBox& aListBox,
    CAknTitlePane& aTitlePane,
    CEikButtonGroupContainer& aButtons,
    CMuiuFlags& aFlags,
    CImumInSettingsData& aMailboxSettings )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsDialog::NewL, 0, utils, KLogUi );

    CIMSSettingsDialog* self = NewLC(
        aDialog, aMailboxApi, aListBox,
        aTitlePane, aButtons, aFlags, aMailboxSettings );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::NewLC()
// ----------------------------------------------------------------------------
//
CIMSSettingsDialog* CIMSSettingsDialog::NewLC(
    CIMSSettingsUi& aDialog,
    CImumInternalApiImpl& aMailboxApi,
    CEikFormattedCellListBox& aListBox,
    CAknTitlePane& aTitlePane,
    CEikButtonGroupContainer& aButtons,
    CMuiuFlags& aFlags,
    CImumInSettingsData& aMailboxSettings )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsDialog::NewLC, 0, utils, KLogUi );

    CIMSSettingsDialog* self =
        new ( ELeave ) CIMSSettingsDialog(
            aDialog, aMailboxApi, aListBox,
            aTitlePane, aFlags );
    CleanupStack::PushL( self );
    self->ConstructL( aButtons, aMailboxSettings );

    return self;
    }

/******************************************************************************

    Event and keypress handling

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::OkToExitL()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsDialog::OkToExitL( const TInt /* aButtonId */ )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::OkToExitL, 0, KLogUi );

    // Show query for exit
    SetEmailSettingsState();

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::KeyPressSoftkey()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsDialog::KeyPressSoftkey(
    const TInt aButton )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::KeyPressSoftkey, 0, KLogUi );

    if ( aButton == EAknSoftkeyBack )
        {
        if ( Flag( EDialogMainMenu ) )
            {
            SetFlag( EImumSettingShouldClose );
            SetPreviousTitlePaneText();
            }
        else
            {
            // Go backwards to previous setting page
            // Panic if any error
            TRAPD( error, HandleStackBackwardL() );

            if ( error != KErrNone )
                {
                __ASSERT_DEBUG( EFalse,
                    User::Panic( KIMSSettingsDialogPanic, error ) );
                }
            //Not crucial, should not leave
            TRAP_IGNORE( UpdateMskL() );
            }
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::KeyPressOKButtonL()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsDialog::KeyPressOKButtonL( const TInt aButton )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::KeyPressOKButtonL, 0, KLogUi );

    // Get the currently selected item
    CMuiuSettingBase* base = CurrentItem();
    __ASSERT_DEBUG( base != NULL,
        User::Panic( KIMSSettingsDialogPanic, KErrNotFound ) );

    // When Options->Change is selected, then the menu should
    // be opened, no matter what
    base->iItemFlags.ChangeFlag( EMuiuDynItemOneTimeForceViewOpen,
        aButton == EAknSoftkeySelect );

    TMuiuPageResult result = OpenSettingPageL( *base );

    // Continue opening the page, until the setting is approved
    if ( result == EMuiuPageResultOk )
        {
        SetStoreFlags( ETrue, GetStorerFlag( *base ) );
        }
    // For read only items, show special error note
    else if ( result == EMuiuPageResultReadOnly )
        {
        TMuiuSettingsText empty;
        iNoteUi->ShowDialog( *base, EIMSItemInvalid, empty );
        }
    else if ( result == EMuiuPageResultPageLocked )
        {
        CIMSSettingsNoteUi::ShowNoteL(
            R_QTN_SELEC_PROTECTED_SETTING,
            EIMSInformationNote );
        }
    else
        {
        // other cases can be skipped
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::EventSubArrayChangeL
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::EventSubArrayChangeL(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::EventSubArrayChangeL, 0, KLogUi );

    // Set the flag to indicate main menu is active/inactive
    ChangeFlag( EDialogMainMenu,
        aBaseItem.iItemResourceId == R_IMAS_MAIN_MENU );

    // In case new item is provided, do settings open
    HandleSubMenuOpenL( aBaseItem );

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::EventItemEditStartsL
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsDialog::EventItemEditStartsL(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::EventItemEditStartsL, 0, KLogUi );

    TMuiuPageEventResult result = EMuiuPageEventResultDisapproved;

    switch ( aBaseItem.iItemId.iUid )
        {
        case EIMASIncomingIap:
        case EIMASOutgoingIap:
            LaunchIapPageL( aBaseItem );
            break;

        case EIMASUserFolderSubscription:
            SubscribeFoldersL();
            break;

        case EIMASIncomingPort:
        case EIMASOutgoingPort:
			// Set port setting dialog resource
	    	SetSettingPageResource(
	    	    EIPRNumber,
	    	    R_IMAS_SETTINGS_DIALOG_FIVE_DIGITS );
	    	result = EMuiuPageEventResultApproved;
			break;

        case EIMASIncomingMailboxName:
        case EIMASIncomingMailServer:
        case EIMASIncomingUserName:
        case EIMASOutgoingEmailAddress:
        case EIMASOutgoingMailServer:
        case EIMASOutgoingUserName:
        case EIMASUserReplyTo:
            // Set text dialog to lower case (abc) when needed
    	    SetSettingPageResource( EIPRText, R_IMAS_SETTINGS_DIALOG_TEXT );
            result = EMuiuPageEventResultApproved;
	        break;

        default:
            result = EMuiuPageEventResultApproved;
            break;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::EventItemEditEndsL
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::EventItemEditEndsL(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::EventItemEditEndsL, 0, KLogUi );

    switch ( aBaseItem.iItemId.iUid )
        {
        // After the editing of the access point item has finished,
        // update the setting item
        case EIMASIncomingIap:
        case EIMASOutgoingIap:
            UpdateAccessPointItem( aBaseItem );
            UpdateAOIntervalsL();
            break;

        case EIMASDownloadRetrievedParts:
            IncludePartialFetchString( aBaseItem );
            break;

        case EIMASLimitInbox:
        case EIMASLimitFolders:
            UpdateItemText( aBaseItem.iItemId );
            break;

        case EIMASIncomingPort:
        case EIMASOutgoingPort:
            UpdateItemText( aBaseItem.iItemId );
            break;

        // Make sure that port setting item is updated, if user has selected
        // default, otherwise it will be overwritten when exiting settings.
        case EIMASIncomingSecurity:
            CheckPort( *GetItem( ToUid( EIMASIncomingPort ) ), ETrue );
            break;

        // Make sure that port setting item is updated, if user has selected
        // default, otherwise it will be overwritten when exiting settings.
        case EIMASOutgoingSecurity:
            CheckPort( *GetItem( ToUid( EIMASOutgoingPort ) ), EFalse );
            break;

        // After editing the always online setting, the hidden items in the
        // array has to be revealed for editing; or they need to be hidden
        // if the Always Online is turned off
        case EIMASAORolling:
            EventUpdateAlwaysOnline( aBaseItem.Value() );
            break;

        // After the Always Online days setting item is closed, check the
        // settings and update the setting text according to new value
        case EIMASAODays:
            EventUpdateAoDays( aBaseItem );
            break;

        case EIMASMailNotifications:
            EventUpdateMailNotifications( aBaseItem.Value() );
            break;

        // if user have selected deletion phone only settings
        // we should tell user that email headers will stay on phone
        case EIMASUserMailDeletion:
            if ( aBaseItem.Value() == EIMASMailDeletionPhone )
                {
                CIMSSettingsNoteUi::ShowNoteL(
                    R_IMUM_HEADER_WILL_REMAIN_PHONE,
                    EIMSInformationNote, ETrue );
                }
            break;
        case EIMASIncomingMailboxName:
            SetTitlePaneTextL( *aBaseItem.Text(), EFalse );

            break;
        default:
            break;
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::EventItemEvaluateText()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsDialog::EventItemEvaluateText(
    const CMuiuSettingBase& aBaseItem,
    TDes& aNewText )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::EventItemEvaluateText, 0, KLogUi );

    // Evaluate the item on fly
    TIMSApproverEvent event;
    TMuiuPageEventResult result =
        iItemApprover->EvaluateText( event, aBaseItem, aNewText );

    if ( result == EMuiuPageEventResultApproved )
        {
        // Handle possible events
        EventHandleApproverEvent( event );
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::EventItemEvaluateRadioButton()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsDialog::EventItemEvaluateRadioButton(
    const CMuiuSettingBase& aBaseItem,
    TInt& aNewValue )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::EventItemEvaluateRadioButton, 0, KLogUi );

    // Evaluate the item on fly
    TIMSApproverEvent event;
    TMuiuPageEventResult result =
        iItemApprover->EvaluateValue( event, aBaseItem, aNewValue );

    if ( result == EMuiuPageEventResultApproved )
        {
        // Handle possible events
        EventHandleApproverEvent( event );
        }

    return result;
    }


// ----------------------------------------------------------------------------
// CIMSSettingsDialog::IncludePartialFetchString()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::IncludePartialFetchString(
    CMuiuSettingBase& aRetrievedParts )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::IncludePartialFetchString, 0, KLogUi );

    TInt index = KErrNotFound;
    TUid id;
    CMuiuSettingsLinkExtended* retrievedParts =
        static_cast<CMuiuSettingsLinkExtended*>( &aRetrievedParts );

    // Check if partial fetch is selected
    if ( FindCheckedRadiobutton( *retrievedParts, id, index ) == KErrNone &&
         id.iUid == EIMASBtnRetrieveLimit )
        {
        CMuiuSettingBase* subItem = SettingFindEditor( *retrievedParts, id );

        // Finally, set the text accoding to item
        TMuiuSettingsText settingText;
        CIMSSettingsNoteUi::MakeString( settingText,
            R_IMAS_SETTINGS_MAIL_FETCH_LESS_VALUE, subItem->Value() );

        retrievedParts->iItemSettingText->Copy( settingText );
        }

    Refresh();
    }

/******************************************************************************

    Array initialization

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::GetUninitializedItem()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CIMSSettingsDialog::GetUninitializedItem(
    const TUid& aId )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::GetUninitializedItem, 0, KLogUi );

    // At first, find the item and get its resource id, then
    // use the resource id to initialize the actual array.
    CMuiuSettingBase* base = GetItem( aId );

    if ( base )
        {
        TRAP_IGNORE( InitializeArrayL( base->iItemResourceId ) );
        }

    // After item initialization is done, the item can be returned
    return base;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::HandleSubMenuOpenL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::HandleSubMenuOpenL(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::HandleSubMenuOpenL, 0, KLogUi );

    InitializeArrayL( aBaseItem.iItemResourceId );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeArray()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeArrayL( const TInt aResource )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeArrayL, 0, KLogUi );

    iInitResource = aResource;

    switch ( iInitResource )
        {
        case R_IMAS_MAIN_MENU:
            InitializeArrayMainMenu();
            break;

        case R_IMAS_INCOMING_SETTINGS:
            InitializeArrayIncomingL();
            break;

        case R_IMAS_OUTGOING_SETTINGS:
            InitializeArrayOutgoingL();
            break;

        case R_IMAS_USER_PREFERENCES:
            InitializeArrayUserPrefL();
            break;

        case R_IMAS_RETRIEVAL_LIMIT:
            InitializeArrayRetLimitL();
            break;

        case R_IMAS_DOWNLOAD_SETTINGS:
            InitializeArrayDownloadL();
            break;

        case R_IMAS_ALWAYSONLINE_SETTINGS:
            InitializeArrayAlwaysOnlineL();
            break;

        case R_IMAS_MAILBOX_MENU:
        default:
            // Buttons are not needed to be initialized separately
            break;
        }
    }


// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeNewResourceL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeNewResourceL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeNewResourceL, 0, KLogUi );

    // Delete the previous item before acquiring the new one
    delete iInitArray;
    iInitArray = NULL;

    iInitArray = GetResourceLC( iInitResource, EFalse );
    CleanupStack::Pop( iInitArray );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeItem()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeItem(
    CMuiuDynFinderItemArray& aArray,
    const TInt aIndex,
    const TInt aValue,
    const TMuiuSettingsText* aText )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeItem, 0, KLogUi );

    // Try initilizing the item
    if ( aIndex >= 0 && aIndex < aArray.Count() )
        {
        TInt error = InitAnyItem(
            *aArray.At( aIndex ).iItem, aValue, aText );

        // Set Initialize flags
        SetInitializeFlags( error );
        }
    else
        {
        __ASSERT_DEBUG( EFalse, User::Panic(
            KIMSSettingsDialogPanic, KErrUnknown ) );
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeItem()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeItem(
    CMuiuSettingBase& aBaseItem,
    const TInt aValue,
    const TMuiuSettingsText* aText )
    {
    // Try initilizing the item
    TInt error = InitAnyItem( aBaseItem, aValue, aText );

    // If initializing has failed, it means the item needs to be saved again
    SetInitializeFlags( error );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeNext()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeNext(
    const TUid& aItemId,
    const TMuiuSettingsText& aText )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeNext, 0, KLogUi );

    // Fetch next item
    CMuiuSettingBase* base = NextItemSearch( aItemId );

    // Validate the item
    TInt error = KErrNone;

    // Finish item
    error = InitAnyItem( *base, KErrNotFound, &aText );

    // If initializing has failed, it means the item needs to be saved again
    SetInitializeFlags( error );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeNext()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeNext(
    const TUid& aItemId,
    const TInt aValue,
    const TMuiuSettingsText& aText )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeNext, 0, KLogUi );

    // Validate the item
    TInt error = InitAnyItem(
        *NextItemSearch( aItemId ), aValue, &aText );

    // If initializing has failed, it means the item needs to be saved again
    SetInitializeFlags( error );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeNext()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeNext(
    const TUid& aItemId,
    const TPtrC8& aText )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeNext, 0, KLogUi );

    TMuiuSettingsText buffer;
    buffer.Copy( aText );

    // Set next item
    InitializeNext( aItemId, buffer );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeNext()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeNext(
    const TUid& aItemId,
    const TInt aValue )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeNext, 0, KLogUi );

    // Fetch next item
    CMuiuSettingBase* base = NextItemSearch( aItemId );

    // Validate the item
    TInt error = InitAnyItem( *base, aValue, NULL );

    // If initializing has failed, it means the item needs to be saved again
    SetInitializeFlags( error );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeResetArrays()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeResetArrays()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeResetArrays, 0, KLogUi );

    iInitTreeArray->Reset();
    iInitIdArray->Reset();
    iInitValueArray->Reset();
    iInitInfoArray->Reset();
    iInitTextArray->ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeArrayMainMenu()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeArrayMainMenu()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeArrayMainMenu, 0, KLogUi );

    if ( Flag( EDialogMainMenuInitialized ) )
        {
        return;
        }

    // Set flags
    SetFlag( EDialogMainMenuInitialized );
    SetFlag( EDialogMainMenu );

    // Hide items that can be hidden
    HideItemsPermanently();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeArrayIncomingL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeArrayIncomingL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeArrayIncomingL, 0, KLogUi );

    // Check if the item is already initialized
    if ( Flag( EDialogIncomingInitialized ) )
        {
        return;
        }

    // Set flag
    SetFlag( EDialogIncomingInitialized );

    // The settings for imap4 and pop3 differs
    if ( iAccountSettings->iIsImap4 )
        {
        InitializeArrayIncomingImap4L();
        }
    else
        {
        InitializeArrayIncomingPop3L();
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeArrayIncomingImap4L()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeArrayIncomingImap4L()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeArrayIncomingImap4L, 0, KLogUi );

    // Prepare for initialize
    InitializeNewResourceL();
    iInitSetFlag = EDialogIncomingChanged;
    iInitCurrentIndex = KErrNotFound;

    // User name
    InitializeUsername( ToUid( EIMASIncomingUserName ),
        iAccountSettings->iImap4Settings->LoginName() );

    // User password
    InitializePassword( ToUid( EIMASIncomingUserPwd ),
        iAccountSettings->iImap4Settings->Password(),
        EIMASStatusTempRcvPassword );

    // Used mail server
    InitializeNext( ToUid( EIMASIncomingMailServer ),
        iAccountSettings->iImap4Settings->ServerAddress() );

    // Access point in use
    TInt error = InitializeIap( ToUid( EIMASIncomingIap ) );
    SetFlags( error, EDialogIncomingIapNotOk );

    // Mailbox name
    InitializeNext( ToUid( EIMASIncomingMailboxName ),
        iAccountSettings->iName );

    // Mailbox type (Read only)
    InitializeNext( ToUid( EIMASIncomingProtocol ),
        iAccountSettings->iIsImap4 );

    // Security & Port
    InitializeSecuritySettings( ToUid( EIMASIncomingSecurity ) );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeArrayIncomingPop3L()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeArrayIncomingPop3L()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeArrayIncomingPop3L, 0, KLogUi );

    // Prepare for initialize
    InitializeNewResourceL();
    iInitSetFlag = EDialogIncomingChanged;
    iInitCurrentIndex = KErrNotFound;

    // User name
    InitializeUsername( ToUid( EIMASIncomingUserName ),
        iAccountSettings->iPop3Settings->LoginName() );

    // User password
    InitializePassword( ToUid( EIMASIncomingUserPwd ),
        iAccountSettings->iPop3Settings->Password(),
        EIMASStatusTempRcvPassword );

    // Used mail server
    InitializeNext( ToUid( EIMASIncomingMailServer ),
        iAccountSettings->iPop3Settings->ServerAddress() );

    // Access point in use
    TInt error = InitializeIap( ToUid( EIMASIncomingIap ) );
    SetFlags( error, EDialogIncomingIapNotOk );

    // Mailbox name
    InitializeNext( ToUid( EIMASIncomingMailboxName ),
        iAccountSettings->iName );

    // Mailbox type (Read only)
    InitializeNext( ToUid( EIMASIncomingProtocol ),
        iAccountSettings->iIsImap4 );

    // Security & Port
    InitializeSecuritySettings( ToUid( EIMASIncomingSecurity ) );

    // APOP secure login
    InitializeNext( ToUid( EIMASIncomingAPop ),
        iAccountSettings->iPop3Settings->Apop() ?
            EIMASOn : EIMASOff );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeArrayOutgoingL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeArrayOutgoingL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeArrayOutgoingL, 0, KLogUi );

    // Check if the item is already initialized
    if ( Flag( EDialogOutgoingInitialized ) )
        {
        return;
        }

    // Set flag
    SetFlag( EDialogOutgoingInitialized );

    // initialize
    InitializeNewResourceL();
    iInitSetFlag = EDialogOutgoingChanged;
    iInitCurrentIndex = KErrNotFound;

    // Email address
    InitializeNext( ToUid( EIMASOutgoingEmailAddress ),
        iAccountSettings->iSmtpSettings->EmailAddress() );

    // User name
    InitializeUsername( ToUid( EIMASOutgoingUserName ),
        iAccountSettings->iSmtpSettings->LoginName() );

    // User password
    InitializePassword( ToUid( EIMASOutgoingUserPwd ),
        iAccountSettings->iSmtpSettings->Password(),
        EIMASStatusTempSndPassword );

    // Used mail server
    InitializeNext( ToUid( EIMASOutgoingMailServer ),
        iAccountSettings->iSmtpSettings->ServerAddress() );

    // Access point in use
    TInt error = InitializeIap( ToUid( EIMASOutgoingIap ) );
    SetFlags( error, EDialogOutgoingIapNotOk );

    // Security & Port
    InitializeSecuritySettings( ToUid( EIMASOutgoingSecurity ) );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeArrayUserPrefL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeArrayUserPrefL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeArrayUserPrefL, 0, KLogUi );

    // Check if the item is already initialized
    if ( Flag( EDialogUserPrefInitialized ) )
        {
        return;
        }

    // Set flag
    SetFlag( EDialogUserPrefInitialized );

    // Prepare for initialize
    InitializeNewResourceL();
    iInitSetFlag = EDialogUserPrefChanged;
    iInitCurrentIndex = KErrNotFound;

    // Own Name
    InitializeNext( ToUid( EIMASUserOwnName ),
        iAccountSettings->iSmtpSettings->EmailAlias() );

    // Send Message
    InitializeNext( ToUid( EIMASUserSendMsg ),
        iAccountSettings->iSmtpSettings->SendMessageOption() ==
            ESendMessageImmediately ? EImmediately : EOnNextConnection );

    // Send copy to self
    InitializeNext( ToUid( EIMASUserSendCopy ),
        iAccountSettings->iSmtpSettings->SendCopyToSelf() == ESendNoCopy );

    // Include Signature
    InitializeSignature();

    // Show HTML mails
    InitializeNext( ToUid( EIMASUserShowHtml ),
        iAccountSettings->iExtendedSettings->OpenHtmlMail() ?
             EIMASYes : EIMASNo );

    // New mail indicators
    InitializeNext( ToUid( EIMASUserNewMailInd ),
        iAccountSettings->iExtendedSettings->NewMailIndicators() ?
            EIMASYes : EIMASNo );

    // Mail Deletion setting
    InitializeNext( ToUid ( EIMASUserMailDeletion ),
        iAccountSettings->iExtendedSettings->MailDeletion() ) ;

    // Reply-to Address
    InitializeNext( ToUid( EIMASUserReplyTo ),
        iAccountSettings->iSmtpSettings->ReplyToAddress() );

    // Default encoding
    InitializeNext( ToUid( EIMASUserEncoding ),
        iAccountSettings->iSmtpSettings->DefaultMsgCharSet().iUid );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeRetrieveLimitL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeRetrieveLimitL(
    const TInt aLimit,
    const TInt aId )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeRetrieveLimitL, 0, KLogUi );

    // Reset before initializing
    InitializeResetArrays();

    // Add id's to tree
    iInitTreeArray->AppendL( ToUid( EIMASMainDownloadCtrl ) );
    iInitTreeArray->AppendL( ToUid( EIMASDownloadRetrievalLimit ) );
    iInitTreeArray->AppendL( ToUid( EIMASBtnRetrieveLimit ) );
    iInitIdArray->AppendL( ToUid( aId ) );
    iInitIdArray->AppendL( ToUid( EIMASBtnFetchUserDefined ) );
    iInitValueArray->AppendL( aLimit == KErrNotFound ?
        EIMSLimitFetchAll : EIMSLimitUserDefined );
    iInitValueArray->AppendL( aLimit == KErrNotFound ?
        KImumMboxDefaultInboxUpdateLimit : aLimit );

    InitAnyMultiItem( NULL, *iInitIdArray, iInitValueArray, iInitTextArray );
    }


// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeArrayRetLimitL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeArrayRetLimitL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeArrayRetLimitL, 0, KLogUi );

    // Check if the item is already initialized
    if ( Flag( EDialogRetLimitInitialized ) )
        {
        if ( Flag( EDialogDownloadChanged ) )
            {
            // This should really be done after subscribe dialog is closed
            CheckSubscribedFolderStateL();
            }
        return;
        }

    // Set flag
    SetFlag( EDialogRetLimitInitialized );

    // Prepare for initialize
    InitializeNewResourceL();
    iInitSetFlag = EDialogRetLimitChanged;
    iInitCurrentIndex = KErrNotFound;

    // Individual handling for each protocol
    if ( iAccountSettings->iIsImap4 )
        {
        // Inbox limit
        InitializeRetrieveLimitL(
            iAccountSettings->iImap4Settings->InboxSynchronisationLimit(),
            EIMASLimitInbox );

        // Subscribed folders limit
        InitializeRetrieveLimitL(
            iAccountSettings->iImap4Settings->MailboxSynchronisationLimit(),
            EIMASLimitFolders );

        CheckSubscribedFolderStateL();
        }
    else
        {
        // Inbox limit
        InitializeRetrieveLimitL(
            iAccountSettings->iPop3Settings->InboxSynchronisationLimit(),
            EIMASLimitInbox );
        }

    // When always online is set, these settings can only be read
    UpdateFetchLimitSetting();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeArrayDownloadL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeArrayDownloadL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeArrayDownloadL, 0, KLogUi );

    // Check if the item is already initialized
    if ( Flag( EDialogDownloadInitialized ) )
        {
        return;
        }

    // Set flag
    SetFlag( EDialogDownloadInitialized );

    // Prepare for initialize
    InitializeNewResourceL();
    iInitSetFlag = EDialogDownloadChanged;
    iInitCurrentIndex = KErrNotFound;

    // Retrieved parts
    InitializeRetrievedParts();

    // Retrieval limit
    ++iInitCurrentIndex;

    // Imap4 folder path
    InitializeFolderPath();

    // Folder subscriptions
    ++iInitCurrentIndex;

    // Del. msgs. over limit
    InitializeNext( ToUid( EIMASDownloadMsgsOverLimit ),
        iAccountSettings->iExtendedSettings->HideMsgs() ?
            EIMASYes : EIMASNo );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeArrayAlwaysOnlineL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeArrayAlwaysOnlineL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeArrayAlwaysOnlineL, 0, KLogUi );

    // Check if the item is already initialized
    if ( Flag( EDialogAlOnlineInitialized ) )
        {
        return;
        }

    // Set flag
    SetFlag( EDialogAlOnlineInitialized );

    // EDialogCsdAccessPoint flag needed when showing AO intervals.
    UpdateCSDFlagL();

    // Prepare for initialize
    iInitResource = R_IMAS_ALWAYSONLINE_SETTINGS;
    InitializeNewResourceL();
    iInitSetFlag = EDialogAlOnlineChanged;
    iInitCurrentIndex = KErrNotFound;

    // Mail message notifications
    InitializeNext( ToUid( EIMASMailNotifications ),
        iAccountSettings->iExtendedSettings->EmailNotificationState() );

    // Automatic Retrieval
    InitializeNext( ToUid( EIMASAORolling ),
        iAccountSettings->iExtendedSettings->AlwaysOnlineState() );

    // Retrieval Days
    InitializeAoDays();

    // Retrieval hours
    InitializeAoHoursL();

    // Retrieval interval
    InitializeAoInterval();

    InitializeAlwaysOnlineState();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeAlwaysOnlineState()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeAlwaysOnlineState()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeAlwaysOnlineState, 0, KLogUi );

    TBool hide = !Flag( EDialogAlwaysOnlineOn );

    SetHideItem( hide, ToUid( EIMASAODays ) );
    SetHideItem( hide, ToUid( EIMASAOHours ) );
    SetHideItem( hide, ToUid( EIMASAOInterval ), ETrue );
    UpdateFetchLimitSetting();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeUsername()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeUsername(
    const TUid& aItemId,
    const TPtrC8& aUsername )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeUsername, 0, KLogUi );

    TMuiuSettingsText login;
    login.Copy( aUsername );

    InitializeNext( aItemId, login );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializePassword()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializePassword(
    const TUid& aItemId,
    const TPtrC8& aPassword,
    const TInt aFlag )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializePassword, 0, KLogUi );

    TMuiuSettingsText login;
    login.Copy( aPassword );
    TMuiuFlags flags = iAccountSettings->iExtendedSettings->StatusFlags();

    if ( flags.Flag( aFlag ) )
        {
        login.Zero();
        SetInitializeFlags( KErrArgument );
        }

    InitializeNext( aItemId, login );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeSignature()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::InitializeSignature()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeSignature, 0, KLogUi );

    TRAPD( error, InitializeSignatureL() );

    return error;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeSignatureL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeSignatureL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeSignatureL, 0, KLogUi );

    // Reset the arrays
    InitializeResetArrays();

    // Determine the button
    TInt button = iAccountSettings->iSmtpSettings->AddSignatureToEmail() ?
        EIMASYes : EIMASNo;

    // Fetch next item
    CMuiuSettingBase* base = NextItemSearch(
        ToUid( EIMASUserIncludeSignature ) );

    // Add the id to array and the value
    iInitInfoArray->AppendL( EMuiuDynInitInfoValue );
    iInitInfoArray->AppendL( EMuiuDynInitInfoText );
    iInitIdArray->AppendL( base->iItemId );
    iInitIdArray->AppendL( ToUid( EIMASBtnSignatureYes ) );
    iInitValueArray->AppendL( button );

    TMuiuSettingsText text;
    iAccountSettings->iSignature->iRichText->Extract(
        text, 0, KImasImailSignatureLength );
    iInitTextArray->AppendL( &text );

    InitAnyMultiItem(
        NULL, *iInitIdArray, iInitValueArray, iInitTextArray, iInitInfoArray );
    iInitTextArray->Reset();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeIap()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::InitializeIap( const TUid& aIapItemId )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeIap, 0, KLogUi );

    CIMSSettingsAccessPointItem* apItem =
        static_cast<CIMSSettingsAccessPointItem*>(
            NextItemSearch( aIapItemId ) );

    if ( aIapItemId.iUid == EIMASIncomingIap )
        {
        apItem->iIap.iId = iAccountSettings->iIncomingIap;
        }
    else
        {
        apItem->iIap.iId = iAccountSettings->iOutgoingIap;
        }

    TRAPD( err, iMailboxApi.CommDbUtilsL().
    		    InitItemAccessPointL( *apItem, EFalse ) );

    // Initialize the items and prepare the setting item
    SetInitializeFlags( err );

    return err;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeRetrievedParts()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::InitializeRetrievedParts()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeRetrievedParts, 0, KLogUi );

    TRAPD( error, InitializeRetrievedPartsL() );

    return error;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeRetrievedPartsL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeRetrievedPartsL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeRetrievedPartsL, 0, KLogUi );

    // Reset the arrays
    InitializeResetArrays();

    // Fetch next item
    CMuiuSettingsLinkExtended* radioArray =
        static_cast<CMuiuSettingsLinkExtended*>(
            NextItemSearch( ToUid( EIMASDownloadRetrievedParts ) ) );

    // Add id's to tree
    TInt button = 0;
    TInt sizeFilter = KIMSDefaultSizeKB;
    iInitTreeArray->AppendL( ToUid( EIMASMainDownloadCtrl ) );
    iInitIdArray->AppendL( ToUid( EIMASDownloadRetrievedParts ) );
    iInitIdArray->AppendL( ToUid( EIMASBtnRetrieveLimit ) );

    // Handle each protocol individually
    if ( iAccountSettings->iIsImap4 )
        {
        // Get the settings
        TImImap4PartialMailOptions partial =
            iAccountSettings->iImap4Settings->PartialMailOptions();
        TImap4GetMailOptions options =
            iAccountSettings->iImap4Settings->GetMailOptions();

        // Define button from setting
        // Setting can be only header, keep it so
        button = 0;
        }
    else
        {
        sizeFilter = iAccountSettings->iPop3Settings->PopulationLimit();

        // Check the headers only button
        if ( sizeFilter == EIMASMapHeadersOnly )
            {
            button = GetSubItemIndex(
                *radioArray, ToUid( EIMASBtnRetrieveHeader ), EFalse );
            }
        // Body and attachment
        else if ( sizeFilter == EIMASMapFullBody )
            {
            button = GetSubItemIndex(
                *radioArray, ToUid( EIMASBtnRetrieveBodyAtt ), EFalse );
            }
        // User defined
        else
            {
            button = GetSubItemIndex(
                *radioArray, ToUid( EIMASBtnRetrieveLimit ), EFalse );
            }
        }

    // Initialize the items and prepare the setting item
    sizeFilter = sizeFilter < 0 ? KIMSDefaultSizeKB : sizeFilter;
    sizeFilter = sizeFilter > KIMSMaxNumValue ?
        KIMSDefaultSizeKB : sizeFilter;

    iInitValueArray->AppendL( button );
    iInitValueArray->AppendL( sizeFilter );
    InitAnyMultiItem( iInitTreeArray, *iInitIdArray, iInitValueArray );

    IncludePartialFetchString( *radioArray );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeAoHoursL()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::InitializeAoHoursL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeAoHoursL, 0, KLogUi );

    TInt error = KErrNone;

    // Fetch next item and its subitems
    CMuiuSettingsLinkExtended* radioArray =
        static_cast<CMuiuSettingsLinkExtended*>(
            NextItemSearch( ToUid( EIMASAOHours ) ) );
    CMuiuSettingsEditValue* sub1 = NULL;
    CMuiuSettingsEditValue* sub2 = NULL;
    FindMultilineEditorL( ToUid( EIMASEditAoHoursEditor ), sub1, sub2 );
    __ASSERT_DEBUG( sub1 && sub2, User::Panic(
        KIMSSettingsDialogPanic, KErrNotFound ) );

    sub1->SetValue(
        iAccountSettings->iExtendedSettings->SelectedTimeStart().Int64() );
    sub2->SetValue(
        iAccountSettings->iExtendedSettings->SelectedTimeStop().Int64() );

    // Update the status
    if ( sub1->Value() == sub2->Value() )
        {
        radioArray->SetValue( EIMASAoHoursAll );
        CheckRadioButton( *radioArray, EIMASAoHoursAll );

        TRAP( error,
            UpdateItemDefaultSettingTextL( *radioArray ) );
        }
    else
        {
        radioArray->SetValue( EIMASAoHoursUserDefined );
        CheckRadioButton( *radioArray, EIMASAoHoursUserDefined );

        TRAP( error,
            UpdateItemCustomSettingTextL( *radioArray, *sub1, *sub2 ) );
        }

    radioArray = NULL;
    sub1 = NULL;
    sub2 = NULL;

    return error;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeSecuritySettings()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeSecuritySettings(
    const TUid& aSecurityItemId )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeSecuritySettings, 0, KLogUi );

    // Security
    TIMASMailSecurity security = InitializeSecurity( aSecurityItemId );
    InitializeNext( aSecurityItemId, security );

    // Port
    InitializePort( security, aSecurityItemId.iUid == EIMASIncomingSecurity );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeSecurity()
// ----------------------------------------------------------------------------
//
TIMASMailSecurity CIMSSettingsDialog::InitializeSecurity(
    const TUid& aSecurityItemId )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeSecurity, 0, KLogUi );

    CImBaseEmailSettings* base = NULL;

    // Get the mailbox type
    if ( aSecurityItemId.iUid == EIMASIncomingSecurity )
        {
        if ( iAccountSettings->iIsImap4 )
            {
            base = iAccountSettings->iImap4Settings;
            }
        else
            {
            base = iAccountSettings->iPop3Settings;
            }
        }
    else
        {
        base = iAccountSettings->iSmtpSettings;
        }

    TIMASMailSecurity security( EImumSecurityOff );

    // Security: TLS
    if( base->SecureSockets() )
        {
        security = EImumSecurityOn;
        }
    // Security: MS
    else if( base->SSLWrapper() )
        {
        security = EImumSecurityMs;
        }
    // Security Off
    else
        {
        security = EImumSecurityOff;
        }

    return security;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializePort()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::InitializePort(
    const TIMASMailSecurity aSecurity,
    const TBool aIncoming )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializePort, 0, KLogUi );

    TRAPD( error, InitializePortL( aSecurity, aIncoming ) );

    return error;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializePortL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializePortL(
    const TIMASMailSecurity aSecurity,
    const TBool aIncoming  )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializePortL, 0, KLogUi );

    // Get next item in the array
    InitializeResetArrays();

    // Add id's to list
    TInt button = KErrNotFound;
    TInt port = 0;

    TUint32 defaultPort = MsvEmailMtmUiUtils::GetDefaultSecurityPort(
        aSecurity, aIncoming, iAccountSettings->iIsImap4 );

    // Make the first check between incoming and outgoing protocol
    if ( aIncoming )
        {
        // Add id's to tree
        iInitTreeArray->AppendL( ToUid( EIMASMailboxIncoming ) );
        iInitIdArray->AppendL( ToUid( EIMASIncomingPort ) );
        iInitIdArray->AppendL( ToUid( EIMASBtnPortUserDefined ) );

        // Another check, this time between receiving protocol
        if ( iAccountSettings->iIsImap4 )
            {
            port = iAccountSettings->iImap4Settings->Port();
            }
        else
            {
            port = iAccountSettings->iPop3Settings->Port();
            }
        }
    else
        {
        // Add id's to tree and to id array
        iInitTreeArray->AppendL( ToUid( EIMASMailboxOutgoing ) );
        iInitIdArray->AppendL( ToUid( EIMASOutgoingPort ) );
        iInitIdArray->AppendL( ToUid( EIMASBtnPortUserDefined ) );

        port = iAccountSettings->iSmtpSettings->Port();
        }

    button = ( port == defaultPort ) ? EIMASPortDefault : EIMASPortUserDefined;
    iInitValueArray->AppendL( button );
    iInitValueArray->AppendL( port );
    InitAnyMultiItem( iInitTreeArray, *iInitIdArray, iInitValueArray );

    UpdateItemText( ToUid( EIMASIncomingPort ) );
    UpdateItemText( ToUid( EIMASOutgoingPort ) );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeFolderPath()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::InitializeFolderPath()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeFolderPath, 0, KLogUi );

    TInt error = KErrNone;

    // Folder path is only for IMAP4
    if ( iAccountSettings->iIsImap4 )
        {
        TMuiuSettingsText buffer;
        buffer.Copy( iAccountSettings->iImap4Settings->FolderPath() );

        // In case path is not set, show default value
        if ( !buffer.Length() )
            {
            HBufC* text = NULL;
            error = SafeStringLoad( R_IMUM_SETTINGS_UNIX_PATH, text );

            if ( error == KErrNone )
                {
                buffer.Copy( *text );
                }

            delete text;
            text = NULL;
            }

        InitializeNext( ToUid( EIMASUserUnixPath ), buffer );
        }

    return error;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeAoDays()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::InitializeAoDays()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeAoDays, 0, KLogUi );

    // Fetch the days from settings
    TInt error = KErrNone;
    TInt selectedDays =
        iAccountSettings->iExtendedSettings->SelectedWeekDays();

    // Make sure that all the checkbox are checked,
    // if none of the buttons are checked
    if ( !selectedDays )
        {
        selectedDays = KIMASEveryDayMask;
        }

    HBufC* text = NULL;
    error = SafeStringLoad( ( ( selectedDays == KIMASEveryDayMask ) ?
        R_IMAS_SETTINGS_DIALOG_DAY_LIST_ALL :
        R_IMAS_SETTINGS_DIALOG_DAY_LIST_USERDEFINED ), text );

    // Initialize item
    InitializeNext( ToUid( EIMASAODays ), selectedDays, *text );

    delete text;
    text = NULL;

    return error;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeAoIntervalButtons()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeAoIntervalButtons(
    TInt& aInterval )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeAoIntervalButtons, 0, KLogUi );

    TBool csd = Flag( EDialogCsdAccessPoint );

    TBool hide5and15minutes(EFalse);
    TBool hide60minutes(EFalse);

    //5 and 15 minute intervals hidden for CSD
    SetHideItem( csd, ToUid( EIMASBtnAo5Min ) );
    SetHideItem( csd, ToUid( EIMASBtnAo15Min ) );
    hide5and15minutes = csd;

    SetStoreFlags( ETrue, EIMASAOInterval );

    // interval is 0 only when account is first created
    // if interval is one of the hidden values it is changed to default.
    if ( aInterval == 0 || ( hide5and15minutes & (
         ( aInterval == KIMASAoInterval5Min ) ||
         ( aInterval == KIMASAoInterval15Min ) ) ) ||
         ( hide60minutes & (aInterval == KIMASAoInterval1Hour ) ) )
        {
        //with default connection feature, we use 1 hour
        //default for both imap and pop.
        aInterval = KIMASAoInterval1Hour;
        }
    }


// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeAoIntervalEditor()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeAoIntervalEditor(
    TInt& aInterval,
    HBufC*& aText )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeAoIntervalEditor, 0, KLogUi );

    TInt intervalResource = 0;
    InitializeAoIntervalButtons( aInterval );

    switch ( aInterval )
        {
        case KIMASAoInterval5Min:
            aInterval = EIMASAoInterval5Min;
            intervalResource = R_IMAS_SETTINGS_DIALOG_INTERVALS_FIRST;
            break;

        case KIMASAoInterval15Min:
            aInterval = EIMASAoInterval15Min;
            intervalResource = R_IMAS_SETTINGS_DIALOG_INTERVALS_SECOND;
            break;

        case KIMASAoInterval30Min:
            aInterval = EIMASAoInterval30Min;
            intervalResource = R_IMAS_SETTINGS_DIALOG_INTERVALS_THIRD;
            break;

        case KIMASAoInterval1Hour:
            aInterval = EIMASAoInterval1Hour;
            intervalResource = R_IMAS_SETTINGS_DIALOG_INTERVALS_FOURTH;
            break;

        default:
        case KIMASAoInterval2Hours:
            aInterval = EIMASAoInterval2Hours;
            intervalResource = R_IMAS_SETTINGS_DIALOG_INTERVALS_FIFTH;
            break;

        case KIMASAoInterval4Hours:
            aInterval = EIMASAoInterval4Hours;
            intervalResource = R_IMAS_SETTINGS_DIALOG_INTERVALS_SIXTH;
            break;

        case KIMASAoInterval6Hours:
            aInterval = EIMASAoInterval6Hours;
            intervalResource = R_IMAS_SETTINGS_DIALOG_INTERVALS_SEVENTH;
            break;
        }

    SafeStringLoad( intervalResource, aText );

    __ASSERT_DEBUG( aText != NULL, User::Panic(
        KIMSSettingsDialogPanic, KErrNotFound ) );
    }


// ----------------------------------------------------------------------------
// CIMSSettingsDialog::InitializeAoInterval()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::InitializeAoInterval()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::InitializeAoInterval, 0, KLogUi );

    TInt interval = iAccountSettings->iExtendedSettings->InboxRefreshTime();
    HBufC* text = NULL;

    InitializeAoIntervalEditor( interval, text );

    // If the text can't be fetched
    if ( text )
        {
        InitializeNext( ToUid( EIMASAOInterval ), interval, *text );
        delete text;
        text = NULL;
        }
    else
        {
        InitializeNext( ToUid( EIMASAOInterval ), interval );
        }
    }


/******************************************************************************

    Setting validating

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsToAccountL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsToAccountL(
    CImumInSettingsData& aSettings )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsToAccountL, 0, KLogUi );

    // This function is called, when user is about to exit the mailbox
    // settings. This function checks if changes are made for any settings
    // and stores the changed settings.

    TInt error = KErrNone;
    // Incoming settings
    if ( Flag( EDialogIncomingChanged ) )
        {
        TRAP( error, StoreSettingsIncomingL() );
        __ASSERT_DEBUG( !error, User::Panic( KIMSSettingsDialogPanic, error ) );
        }

    // Outgoing settings
    if ( Flag( EDialogOutgoingChanged ) )
        {
        TRAP( error, StoreSettingsOutgoingL() );
        __ASSERT_DEBUG( !error, User::Panic( KIMSSettingsDialogPanic, error ) );
        }

    // User preferences
    if ( Flag( EDialogUserPrefChanged ) )
        {
        TRAP( error, StoreSettingsUserPrefL() );
        __ASSERT_DEBUG( !error, User::Panic( KIMSSettingsDialogPanic, error ) );
        }

    // Retrieval limit
    if ( Flag( EDialogRetLimitChanged ) )
        {
        TRAP( error, StoreSettingsRetLimitL() );
        __ASSERT_DEBUG( !error, User::Panic( KIMSSettingsDialogPanic, error ) );
        }

    // Download preferences
    if ( Flag( EDialogDownloadChanged ) )
        {
        TRAP( error, StoreSettingsDownloadL() );
        __ASSERT_DEBUG( !error, User::Panic( KIMSSettingsDialogPanic, error ) );
        }

    // Always Online settings
    if ( Flag( EDialogAlOnlineChanged ) )
        {
        TRAP( error, StoreSettingsAlwaysOnlineL() );
        __ASSERT_DEBUG( !error, User::Panic( KIMSSettingsDialogPanic, error ) );
        }

    User::LeaveIfError( error );
    CImumMboxInternalDataConverter::ConvertToInternalMboxDataL(
        iMailboxApi, *iAccountSettings, aSettings );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsIncomingL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsIncomingL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsIncomingL, 0, KLogUi );

    // Prepare for initialize
    iInitResource = R_IMAS_INCOMING_SETTINGS;
    InitializeNewResourceL();
    iInitSetFlag = EDialogAlOnlineChanged;
    iInitCurrentIndex = KErrNotFound;

    // Get the protocol and store it
    if ( iAccountSettings->iIsImap4 )
        {
        StoreSettingsIncomingImap4L();
        }
    else
        {
        StoreSettingsIncomingPop3L();
        }

    // clear last retrieval status
    TAOInfo emptyInfo = iAccountSettings->iExtendedSettings->LastUpdateInfo();
    emptyInfo.iUpdateSuccessfulWithCurSettings = EFalse;
    iAccountSettings->iExtendedSettings->SetLastUpdateInfo(
      emptyInfo );

    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsIncomingImap4L()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsIncomingImap4L()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsIncomingImap4L, 0, KLogUi );

    // Prepare for initialize
    iInitResource = R_IMAS_INCOMING_SETTINGS;
    InitializeNewResourceL();
    iInitSetFlag = EDialogAlOnlineChanged;
    iInitCurrentIndex = KErrNotFound;

    // User name
    iAccountSettings->iImap4Settings->SetLoginNameL(
        StoreUsername( *StoreGetNextText(
            ToUid( EIMASIncomingUserName ) ) ) );

    // User password
    iAccountSettings->iImap4Settings->SetPasswordL(
        StorePassword( *StoreGetNextText( ToUid( EIMASIncomingUserPwd ) ),
        EIMASStatusTempRcvPassword ) );

    // Used mail server
    iAccountSettings->iImap4Settings->SetServerAddressL(
        *StoreGetNextText( ToUid( EIMASIncomingMailServer ) ) );

    // Access point in use
    StoreSettingsIapL( ETrue );

    // Mailbox name
    iAccountSettings->iName.Copy(
        *StoreGetNextText( ToUid( EIMASIncomingMailboxName ) ) );

    // Mailbox type (Readonly)
    ++iInitCurrentIndex;

    // Security
    StoreSettingsSecurityL( ETrue );

    // Port
    StorePortL( ETrue );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsIncomingPop3L()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsIncomingPop3L()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsIncomingPop3L, 0, KLogUi );

    // Prepare for initialize
    iInitResource = R_IMAS_INCOMING_SETTINGS;
    InitializeNewResourceL();
    iInitSetFlag = EDialogAlOnlineChanged;
    iInitCurrentIndex = KErrNotFound;

    // User name
    iAccountSettings->iPop3Settings->SetLoginNameL(
        StoreUsername( *StoreGetNextText(
            ToUid( EIMASIncomingUserName ) ) ) );

    // User password
    iAccountSettings->iPop3Settings->SetPasswordL(
        StorePassword( *StoreGetNextText( ToUid( EIMASIncomingUserPwd ) ),
        EIMASStatusTempRcvPassword ) );

    // Used mail server
    iAccountSettings->iPop3Settings->SetServerAddressL(
        *StoreGetNextText( ToUid( EIMASIncomingMailServer ) ) );

    // Access point in use
    StoreSettingsIapL( ETrue );

    // Mailbox name
    iAccountSettings->iName.Copy( *StoreGetNextText(
        ToUid( EIMASIncomingMailboxName ) ) );

    // Mailbox type (Readonly)
    ++iInitCurrentIndex;

    // Security
    StoreSettingsSecurityL( ETrue );

    // Port
    StorePortL( ETrue );

    // APOP secure login
    iAccountSettings->iPop3Settings->SetApop(
        StoreGetNextBoolean( ToUid( EIMASIncomingAPop ) ) == EIMASOn ?
            ETrue : EFalse );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsOutgoingL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsOutgoingL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsOutgoingL, 0, KLogUi );

    // Prepare for initialize
    iInitResource = R_IMAS_OUTGOING_SETTINGS;
    InitializeNewResourceL();
    iInitSetFlag = EDialogAlOnlineChanged;
    iInitCurrentIndex = KErrNotFound;

    const TDesC& emailAddress = *StoreGetNextText(
        ToUid( EIMASOutgoingEmailAddress ) );

    // Email address
    iAccountSettings->iSmtpSettings->SetEmailAddressL( emailAddress );

    // Email address must be updated also to extended settings
    iAccountSettings->iExtendedSettings->SetEmailAddress( emailAddress );

     // User name
    iAccountSettings->iSmtpSettings->SetLoginNameL(
        StoreUsername( *StoreGetNextText(
            ToUid( EIMASOutgoingUserName ) ) ) );

    // User password
    iAccountSettings->iSmtpSettings->SetPasswordL(
        StorePassword( *StoreGetNextText( ToUid( EIMASOutgoingUserPwd ) ),
        EIMASStatusTempSndPassword ) );

    // Used mail server
    iAccountSettings->iSmtpSettings->SetServerAddressL(
        *StoreGetNextText( ToUid( EIMASOutgoingMailServer ) ) );

    // Access point in use
    StoreSettingsIapL( EFalse );

    // Security
    StoreSettingsSecurityL( EFalse );

    // Port
    StorePortL( EFalse );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsUserPrefL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsUserPrefL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsUserPrefL, 0, KLogUi );

    // Prepare for initialize
    iInitResource = R_IMAS_USER_PREFERENCES;
    InitializeNewResourceL();
    iInitSetFlag = EDialogAlOnlineChanged;
    iInitCurrentIndex = KErrNotFound;

    // Own Name
    iAccountSettings->iSmtpSettings->SetEmailAliasL(
        *StoreGetNextText( ToUid( EIMASUserOwnName ) ) );

    // Send Message
    iAccountSettings->iSmtpSettings->SetSendMessageOption(
        StoreGetNextBoolean( ToUid( EIMASUserSendMsg ) ) ?
             ESendMessageOnNextConnection : ESendMessageImmediately );

    TImSMTPSendCopyToSelf sendCopy = StoreGetNextBoolean( ToUid( EIMASUserSendCopy ) ) ?
                                        ESendNoCopy : ESendCopyAsCcRecipient;
    // Send copy to self
    iAccountSettings->iSmtpSettings->SetSendCopyToSelf(sendCopy);

    if(sendCopy == ESendCopyAsCcRecipient)
        {
        iAccountSettings->iSmtpSettings->SetReceiptAddressL(iAccountSettings->iSmtpSettings->EmailAddress());
        }
    else
        {
        iAccountSettings->iSmtpSettings->SetReceiptAddressL(KNullDesC);
        }


    // Include Signature
    StoreSettingsSignature();

    // Show HTML mails
    iAccountSettings->iExtendedSettings->SetOpenHtmlMail(
        StoreGetNextBoolean( ToUid( EIMASUserShowHtml ) ) == EIMASYes ?
            ETrue : EFalse );

    // New mail indicators
    iAccountSettings->iExtendedSettings->SetNewMailIndicators(
        StoreGetNextBoolean( ToUid( EIMASUserNewMailInd ) ) == EIMASYes ?
            ETrue : EFalse );

    // Mail deletion
    iAccountSettings->iExtendedSettings->SetMailDeletion(
      static_cast< TIMASMailDeletionMode >(
      StoreGetNextValue( ToUid( EIMASUserMailDeletion ) ) ) );

    // Reply-to Address
    const TDesC& replyToAddress = *StoreGetNextText(
        ToUid( EIMASUserReplyTo ) );
    iAccountSettings->iSmtpSettings->SetReplyToAddressL( replyToAddress );

    // Default encoding
    iAccountSettings->iSmtpSettings->SetDefaultMsgCharSet(
        TUid::Uid( StoreGetNextValue( ToUid( EIMASUserEncoding ) ) ) );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsSignature()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsSignature()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsSignature, 0, KLogUi );

    // First get the radio button value
    CMuiuSettingBase* baseItem = GetItem( ToUid( EIMASUserIncludeSignature ) );
    iAccountSettings->iSmtpSettings->SetAddSignatureToEmail(
        baseItem->Value() == EIMASYes ? ETrue : EFalse );

    // Second get the signature text
    baseItem = SettingFindEditor( *baseItem, ToUid( EIMASBtnSignatureYes ) );
    iAccountSettings->iSignature->iRichText->Reset();
    TRAP_IGNORE( iAccountSettings->iSignature->iRichText->InsertL(
        0, *baseItem->Text() ) );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsRetLimitL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsRetLimitL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsRetLimitL, 0, KLogUi );

    // Prepare for initialize
    iInitResource = R_IMAS_RETRIEVAL_LIMIT;
    InitializeNewResourceL();
    iInitSetFlag = EDialogAlOnlineChanged;
    iInitCurrentIndex = KErrNotFound;

    CMuiuSettingsLinkExtended* radioEditor =
        static_cast<CMuiuSettingsLinkExtended*>(
            GetItem( TUid::Uid( EIMASLimitInbox ) ) );
    CMuiuSettingsEditValue* editor =
        static_cast<CMuiuSettingsEditValue*>(
            SettingFindEditor( *radioEditor, ToUid(
                EIMASBtnFetchUserDefined ) ) );

    // Imap4
    if ( iAccountSettings->iIsImap4 )
        {
        // 1. Inbox limit
        iAccountSettings->iImap4Settings->SetInboxSynchronisationLimit(
            radioEditor->Value() == EIMSLimitFetchAll ?
            KErrNotFound : editor->Value() );

        // 2. Subscribed folders limit EIMASLimitFolders
        radioEditor = static_cast<CMuiuSettingsLinkExtended*>(
            GetItem( TUid::Uid( EIMASLimitFolders ) ) );
        CMuiuSettingsLinkExtended* editor =
            static_cast<CMuiuSettingsLinkExtended*>(
                SettingFindEditor( *radioEditor, ToUid(
                    EIMASBtnFetchUserDefined ), EFalse ) );

        iAccountSettings->iImap4Settings->SetMailboxSynchronisationLimit(
            radioEditor->Value() == EIMSLimitFetchAll ?
            KErrNotFound : editor->Value() );
        }
    // Pop3
    else
        {
        // 1. Inbox limit
        iAccountSettings->iPop3Settings->SetInboxSynchronisationLimit(
            radioEditor->Value() == EIMSLimitFetchAll ?
            KErrNotFound : editor->Value() );
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsDownloadL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsDownloadL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsDownloadL, 0, KLogUi );

    // Prepare for initialize
    iInitResource = R_IMAS_DOWNLOAD_SETTINGS;
    InitializeNewResourceL();
    iInitSetFlag = EDialogAlOnlineChanged;
    iInitCurrentIndex = KErrNotFound;

    // Retrieved parts
    StoreRetrievedPartsL();

    // Retrieval limit
    ++iInitCurrentIndex;

    // Imap4 folder path
    StoreFolderPathL();

    // Folder subscriptions
    ++iInitCurrentIndex;

    // Del. msgs. over limit
    iAccountSettings->iExtendedSettings->SetHideMsgs(
        StoreGetNextBoolean( ToUid(
            EIMASDownloadMsgsOverLimit ) ) == EIMASYes );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsAlwaysOnlineL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsAlwaysOnlineL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsAlwaysOnlineL, 0, KLogUi );

    // Prepare for initialize
    iInitResource = R_IMAS_ALWAYSONLINE_SETTINGS;
    InitializeNewResourceL();
    iInitSetFlag = EDialogAlOnlineChanged;
    iInitCurrentIndex = KErrNotFound;

    // Mail Message Notifications
    iAccountSettings->iExtendedSettings->SetEmailNotificationState(
        static_cast<TMailEmnStates>( StoreGetNextValue(
            ToUid( EIMASMailNotifications ) ) ) );

    TMailAoStates aoNewState = static_cast<TMailAoStates>(
        StoreGetNextValue( ToUid( EIMASAORolling ) ) );

    IMUM1( 0, "aoNewState = %d", aoNewState );

    // Retrieval amount have to set "all" value, when always online is
    // set on. There is also dynamic feature variation flag do always
    // online fetch all or specific amount of headers

    TBool doFetchAllVariation = MsvEmailMtmUiFeatureUtils::LocalFeatureL(
            KCRUidMuiuVariation, KMuiuEmailConfigFlags,
            KEmailFeatureIdAlwaysonlineHeaders );

    IMUM1( 0, "doFetchAllVariation = %d", doFetchAllVariation);

    // if user sets always online on, current state is OFF and new is ON
    if( iAccountSettings->iExtendedSettings->AlwaysOnlineState() == EMailAoOff &&
        aoNewState != EMailAoOff &&
        doFetchAllVariation )
        {
        if ( iAccountSettings->iIsImap4 )
            {
            iAccountSettings->iImap4Settings->SetInboxSynchronisationLimit(
                KErrNotFound );
            }
        else
            {
            iAccountSettings->iPop3Settings->SetInboxSynchronisationLimit(
                KErrNotFound );
            }

        IMUM0( 0, "SetInboxSynchronisationLimit = ALL" );
        }
    // user sets ao OFF, change retrieval amount back to default
    // we don't remember old retrieval amout value
    else if( iAccountSettings->iExtendedSettings->AlwaysOnlineState() != EMailAoOff &&
             aoNewState == EMailAoOff &&
             doFetchAllVariation )
        {
        if ( iAccountSettings->iIsImap4 )
            {
            iAccountSettings->iImap4Settings->SetInboxSynchronisationLimit(
                KImumMboxDefaultInboxUpdateLimit );
            }
        else
            {
            iAccountSettings->iPop3Settings->SetInboxSynchronisationLimit(
                KImumMboxDefaultInboxUpdateLimit );
            }
        IMUM0( 0, "SetInboxSynchronisationLimit = KImumMboxDefaultInboxUpdateLimit" );
        }

    // Automatic Retrieval
    iAccountSettings->iExtendedSettings->SetAlwaysOnlineState( aoNewState );

    // Retrieval Days
    iAccountSettings->iExtendedSettings->SetSelectedWeekDays(
        StoreGetNextValue( ToUid( EIMASAODays ) ) );

    // Retrieval hours
    StoreAoHoursL();

    // Retrieval interval
    StoreAoInterval();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsIapL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsIapL( const TBool aIsIncoming )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsIapL, 0, KLogUi );

    // Fetch next item
    TUid id = ToUid( aIsIncoming ? EIMASIncomingIap : EIMASOutgoingIap );
    CMuiuSettingBase* base = NextItemSearch( id );
    CIMSSettingsAccessPointItem* iapItem =
        static_cast<CIMSSettingsAccessPointItem*>( base );

    // Change the state depending on the protocol
    // The item validator sets the states for the item itself, while storing
    // needs only to copy the value into the settings
    if ( aIsIncoming )
        {
        // Incoming settings
        if( iapItem->iIap.iResult == CMManager::EAlwaysAsk )
            {
            // Set to Default connection, always ask is handled that way
            iAccountSettings->iIncomingIap = 0;
            }
        else
            {
            iAccountSettings->iIncomingIap = iapItem->iIap.iId;
            }

        if ( iAccountSettings->iIsImap4 )
            {
            iAccountSettings->iImap4Settings->SetFetchSizeL(
                iMailboxApi.CommDbUtilsL().DetermineImap4BufferSize(
                    iAccountSettings->iIncomingIap, *iDefaultData ) );
            }
        }
    else
        {
        // Outgoing settings
        if( iapItem->iIap.iResult == CMManager::EAlwaysAsk )
            {
            // Set to Default connection, always ask is handled that way
            iAccountSettings->iOutgoingIap = 0;
            }
        else
            {
            iAccountSettings->iOutgoingIap = iapItem->iIap.iId;
            }
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsSecurityL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsSecurityL( const TBool aIsIncoming )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsSecurityL, 0, KLogUi );

    // Make a protocol check
    if ( aIsIncoming )
        {
        TIMASMailSecurity security =
            static_cast<TIMASMailSecurity>( StoreGetNextValue(
                ToUid( EIMASIncomingSecurity ) ) );


        if ( iAccountSettings->iIsImap4 )
            {
            // Imap4 settings
            MsvEmailMtmUiUtils::StoreSecuritySettings(
                *iAccountSettings->iImap4Settings, security );
            }
        else
            {
            // Pop3 settings
            MsvEmailMtmUiUtils::StoreSecuritySettings(
                *iAccountSettings->iPop3Settings, security );
            }

        }
    else
        {
        TIMASMailSecurity security =
            static_cast<TIMASMailSecurity>( StoreGetNextValue(
                ToUid( EIMASOutgoingSecurity ) ) );
        // Smtp settings
        MsvEmailMtmUiUtils::StoreSecuritySettings(
            *iAccountSettings->iSmtpSettings, security );
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSettingsRetrievedPartsImap4L()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSettingsRetrievedPartsImap4L(
    const TImap4GetMailOptions aOptions,
    const TImImap4PartialMailOptions aPartial )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSettingsRetrievedPartsImap4L, 0, KLogUi );

    // Set mail fetching options
    iAccountSettings->iImap4Settings->SetGetMailOptions( aOptions );

    // Set partial options
    iAccountSettings->iImap4Settings->SetPartialMailOptionsL( aPartial );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreRetrievedValuesImap4L()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreRetrievedValuesImap4L( const TInt32 aSize )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreRetrievedValuesImap4L, 0, KLogUi );

    iAccountSettings->iImap4Settings->SetBodyTextSizeLimitL( aSize );
    iAccountSettings->iImap4Settings->SetAttachmentSizeLimitL( aSize );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreRetrievedPartsL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreRetrievedPartsL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreRetrievedPartsL, 0, KLogUi );

    // Get the selected button
    CMuiuSettingBase* base =
        NextItemSearch( ToUid( EIMASDownloadRetrievedParts ) );
    CMuiuSettingsLinkExtended* radioArray =
        static_cast<CMuiuSettingsLinkExtended*>( base );

    TUid id = ToUid( 0 );
    TInt index = 0;
    FindCheckedRadiobutton( *radioArray, id, index );

    // Individual handling for imap4 and pop3 protocols
    if ( iAccountSettings->iIsImap4 )
        {
        // Only headers are fetched during the sync
        StoreSettingsRetrievedPartsImap4L( EGetImap4EmailHeaders );
        StoreRetrievedValuesImap4L();
        }
    else
        {
        switch ( id.iUid )
            {
            case EIMASBtnRetrieveBodyAtt:
                iAccountSettings->iPop3Settings->SetPopulationLimitL(
                    EIMASMapFullBody );
                break;

            case EIMASBtnRetrieveHeader:
                iAccountSettings->iPop3Settings->SetPopulationLimitL(
                    EIMASMapHeadersOnly );
                break;

            case EIMASBtnRetrieveLimit:
                {
                // Get the value from item
                base = SettingFindEditor( *base, id );
                iAccountSettings->iPop3Settings->SetPopulationLimitL(
                    base->Value() );
                }
                break;

            default:
                __ASSERT_DEBUG( EFalse, User::Panic(
                    KIMSSettingsDialogPanic, KErrUnknown ) );
                break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreFolderPathL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreFolderPathL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreFolderPathL, 0, KLogUi );

    // Only possible with Imap4
    if ( iAccountSettings->iIsImap4 )
        {
        // Fetch the text from the setting
        TMuiuSettingsText buf16 =
            *StoreGetNextText( ToUid( EIMASUserUnixPath ) );

        HBufC* text = StringLoader::LoadLC(
            R_IMUM_SETTINGS_UNIX_PATH );

        // If the text matches with the default text clean the text field
        if ( !text->CompareC( buf16 ) )
            {
            buf16.Zero();
            }

        CleanupStack::PopAndDestroy( text );
        text = NULL;

        TBuf8<KMuiuDynMaxSettingsTextLength> buf8;
        buf8.Copy( buf16.Left( KMuiuDynMaxSettingsTextLength ) );

        iAccountSettings->iImap4Settings->SetFolderPathL( buf8 );
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreUsername()
// ----------------------------------------------------------------------------
//
const TBuf8<KMuiuDynMaxSettingsTextLength> CIMSSettingsDialog::StoreUsername(
    const TDesC& aUsername )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreUsername, 0, KLogUi );

    TBuf8<KMuiuDynMaxSettingsTextLength> buffer;
    buffer.Copy( aUsername );

    return buffer;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StorePassword()
// ----------------------------------------------------------------------------
//
const TBuf8<KMuiuDynMaxSettingsTextLength> CIMSSettingsDialog::StorePassword(
    const TDesC& aPassword,
    const TInt aFlag )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StorePassword, 0, KLogUi );

    TBuf8<KMuiuDynMaxSettingsTextLength> buffer;
    buffer.Copy( aPassword );

    // Clear the flag, as it is valid now
    TMuiuFlags flags = iAccountSettings->iExtendedSettings->StatusFlags();
    flags.ClearFlag( aFlag );
    iAccountSettings->iExtendedSettings->SetStatusFlags( flags );

    return buffer;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSignatureL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSignatureL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSignatureL, 0, KLogUi );

    iAccountSettings->iSmtpSettings->SetAddSignatureToEmail(
        StoreGetNextValue( ToUid( EIMASUserIncludeSignature ) ) > 0 );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StorePortL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StorePortL( const TBool aIncoming )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StorePortL, 0, KLogUi );

    // Fetch next item
    TInt id = aIncoming ? EIMASIncomingPort : EIMASOutgoingPort;
    CMuiuSettingBase* base = NextItemSearch( ToUid( id ) );

    CMuiuSettingsEditValue* valueEditor =
        static_cast<CMuiuSettingsEditValue*>(
            SettingFindEditor( *base, ToUid( EIMASBtnPortUserDefined ) ) );

	CheckPort( *base, aIncoming );

    __ASSERT_DEBUG( valueEditor->Value() >= 0, User::Panic(
        KIMSSettingsDialogPanic, KErrUnknown ) );

    // Check incoming&outgoing
    if ( aIncoming )
        {
        // Check imap4&pop3
        if ( iAccountSettings->iIsImap4 )
            {
            iAccountSettings->iImap4Settings->SetPort(
                valueEditor->Value() );
            }
        else
            {
            iAccountSettings->iPop3Settings->SetPort(
                valueEditor->Value() );
            }
        }
    else
        {
        iAccountSettings->iSmtpSettings->SetPort(
            valueEditor->Value() );
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreAoHoursL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreAoHoursL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreAoHoursL, 0, KLogUi );

    // Hours are stored into two seperate settings, which needs to be combined
    CMuiuSettingBase* aoHours =
        NextItemSearch( ToUid( EIMASAOHours ) );

    // Define default times
    TTime thisTime = 0;
    TTime nextTime = 0;
    TInt radiobutton = aoHours->Value();

    // When user has defined the hours, get them manually
    if ( radiobutton )
        {
        CMuiuSettingsEditValue* userdef =
            static_cast<CMuiuSettingsEditValue*>(
                GetSubItem( *aoHours, radiobutton ) );
        CMuiuSettingsEditValue* sub1 = NULL;
        CMuiuSettingsEditValue* sub2 = NULL;
        FindMultilineEditorL( ToUid( EIMASEditAoHoursEditor ), sub1, sub2 );
        __ASSERT_DEBUG( sub1 && sub2, User::Panic(
            KIMSSettingsDialogPanic, KErrNotFound ) );

        // Take times
        thisTime = sub1->Value();
        nextTime = sub2->Value();

        userdef = NULL;
        sub1 = NULL;
        sub2 = NULL;
        }

    // Store the values
    iAccountSettings->iExtendedSettings->SetSelectedTimeStart( thisTime );
    iAccountSettings->iExtendedSettings->SetSelectedTimeStop( nextTime );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreAoInterval()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreAoInterval()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreAoInterval, 0, KLogUi );

    TInt interval = GetAoIntervalTimeInMinutes();
    iAccountSettings->iExtendedSettings->SetInboxRefreshTime( interval );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::GetAoIntervalTimeInMinutes()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::GetAoIntervalTimeInMinutes()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::GetAoIntervalTimeInMinutes, 0, KLogUi );

    TInt ret;

    CMuiuSettingsLinkExtended* radioButtons =
        static_cast<CMuiuSettingsLinkExtended*>(
            GetItem( TUid::Uid( EIMASAOInterval ) ) );

    TUid id = ToUid( 0 );
    TInt index = 0;
    FindCheckedRadiobutton( *radioButtons, id, index );

    // Set the correct amount of minutes to setting
    switch ( id.iUid )
        {
        case EIMASBtnAo5Min:
            ret = KIMASAoInterval5Min;
            break;

        case EIMASBtnAo15Min:
            ret = KIMASAoInterval15Min;
            break;

        case EIMASBtnAo30Min:
            ret = KIMASAoInterval30Min;
            break;

        case EIMASBtnAo60Min:
            ret = KIMASAoInterval1Hour;
            break;

        default:
        case EIMASBtnAo2Hours:
            ret = KIMASAoInterval2Hours;
            break;

        case EIMASBtnAo4Hours:
            ret = KIMASAoInterval4Hours;
            break;

        case EIMASBtnAo6Hours:
            ret = KIMASAoInterval6Hours;
            break;
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreGetNextText()
// ----------------------------------------------------------------------------
//
const TMuiuSettingsText* CIMSSettingsDialog::StoreGetNextText(
    const TUid& aItemId )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreGetNextText, 0, KLogUi );

    // Fetch next item
    CMuiuSettingBase* base = NextItemSearch( aItemId );

    return base->Text();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreGetNextValue()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::StoreGetNextValue(
    const TUid& aItemId )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreGetNextValue, 0, KLogUi );

    // Fetch next item
    CMuiuSettingBase* base = NextItemSearch( aItemId );

    return base->Value();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreGetNextBoolean()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsDialog::StoreGetNextBoolean(
    const TUid& aItemId )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreGetNextBoolean, 0, KLogUi );

    // Fetch next item
    CMuiuSettingBase* base = NextItemSearch( aItemId );

    return base->Value() != 0;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreGetLoginInformation()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreGetLoginInformation( TDes8& aUserName,
    TDes8& aPassword )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreGetLoginInformation, 0, KLogUi );
    IMUM_IN();

    iInitResource = R_IMAS_INCOMING_SETTINGS;
    TRAPD( err, InitializeNewResourceL() );
    if( err != KErrNone )
	    {
		IMUM2( 0, "InitializeNewResourceL(res = 0x%x) leaved with code %d", R_IMAS_INCOMING_SETTINGS, err);
		return;
	    }

    aUserName.Copy( *StoreGetNextText( ToUid( EIMASIncomingUserName ) ) );
    aPassword.Copy( *StoreGetNextText( ToUid( EIMASIncomingUserPwd ) ) );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::StoreSetLoginInformation()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::StoreSetLoginInformation( const TDesC8& aUserName,
    const TDesC8& aPassword )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::StoreSetLoginInformation, 0, KLogUi );
    IMUM_IN();

    iInitResource = R_IMAS_INCOMING_SETTINGS;
    TRAPD( err, InitializeNewResourceL() );
    if( err != KErrNone )
	    {
		IMUM2( 0, "InitializeNewResourceL(res = 0x%x) leaved with code %d", R_IMAS_INCOMING_SETTINGS, err);
		return;
	    }

    TMuiuSettingsText text;
    text.Copy( aUserName );
    NextItemSearch( ToUid( EIMASIncomingUserName  ) )->SetText( &text );
    text.Copy( aPassword );
    NextItemSearch( ToUid( EIMASIncomingUserPwd ) )->SetText( &text );

    IMUM_OUT();
    }

/******************************************************************************

    Brand new itemcheck code

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::EventHandleApproverEvent()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::EventHandleApproverEvent(
    const TIMSApproverEvent aEvent )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::EventHandleApproverEvent, 0, KLogUi );

    switch ( aEvent )
        {
        case EIMSApproverRemoveOmaEmn:
            EventEMNForceTurnOff();
            break;

        // Always online has to be removed
        case EIMSApproverRemoveAlwaysOnline:
            EventAOForceTurnOff();
            break;

        default:
            break;
        }
    }

/******************************************************************************

    Aquire custom pages

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::AcquireCustomSettingPageL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::AcquireCustomSettingPageL(
    CAknSettingPage*& aReturnPage,
    TMuiuDynSetUpdateMode& aReturnUpdateMode,
    TInt& aReturnSettingValue,
    TMuiuSettingsText& /* aReturnSettingText */,
    CDesCArrayFlat& aReturnRadioButtonArray,
    CMuiuSettingBase& aParamBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::AcquireCustomSettingPageL, 0, KLogUi );

    switch ( aParamBaseItem.iItemId.iUid )
        {
        // Handle encoding
        case EIMASUserEncoding:
            aReturnPage = AcquireCustomEncodingPageL(
                aReturnUpdateMode, aReturnSettingValue,
                aReturnRadioButtonArray, aParamBaseItem );
            break;

        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::AcquireCustomEncodingPageL()
// ----------------------------------------------------------------------------
//
CAknSettingPage* CIMSSettingsDialog::AcquireCustomEncodingPageL(
    TMuiuDynSetUpdateMode& aReturnUpdateMode,
    TInt& aReturnSettingValue,
    CDesCArrayFlat& aReturnRadioButtonArray,
    CMuiuSettingBase& aParamBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::AcquireCustomEncodingPageL, 0, KLogUi );

    // Create radiobutton array of all existing character converters
    AcquireCustomEncodingButtonArrayL( aReturnRadioButtonArray );

    // Create the radiobutton page
    CAknRadioButtonSettingPage* encodingPage =
        new ( ELeave ) CAknRadioButtonSettingPage(
            SettingPageResource( EIPRRadioButton ),
            aReturnSettingValue, &aReturnRadioButtonArray );
    CleanupStack::PushL( encodingPage );

    // Define the page properties
    encodingPage->SetSettingTextL( *aParamBaseItem.iItemLabel );

    // Set updatemode to accept setting only when the setting is
    // accepted
    aReturnUpdateMode = CAknSettingPage::EUpdateWhenAccepted;

    // Pop and return
    CleanupStack::Pop( encodingPage );
    return encodingPage;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::AcquireCustomEncodingButtonArrayL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::AcquireCustomEncodingButtonArrayL(
    CDesCArrayFlat& aButtons )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::AcquireCustomEncodingButtonArrayL, 0, KLogUi );

    RFs& fs = CCoeEnv::Static()->FsSession();

    // Create character converter object and the array of available converters
    CCnvCharacterSetConverter* charConv = CCnvCharacterSetConverter::NewLC();
    CIMACharConvList* converters =
        CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC( fs );
    const TInt count = converters->Count();

    // Append items to array
    for ( TInt converter = 0; converter < count; converter++ )
        {
        HBufC8* encoderName =
            charConv->ConvertCharacterSetIdentifierToStandardNameL(
                ( *converters )[converter].Identifier(), fs );

        if ( encoderName )
            {
            CleanupStack::PushL( encoderName );

            TMuiuSettingsText temp;
            temp.Copy( *encoderName );
            aButtons.AppendL( temp );

            CleanupStack::PopAndDestroy( encoderName );
            encoderName = NULL;
            }
        }

    // Remove the objects
    CleanupStack::PopAndDestroy( converters );
    converters = NULL;
    CleanupStack::PopAndDestroy( charConv );
    charConv = NULL;
    }

/******************************************************************************

    Misc functions

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::NextItemSearch()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CIMSSettingsDialog::NextItemSearch(
    const TUid& aExpectedId )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::NextItemSearch, 0, KLogUi );

    // Try with cumulative search. It's highly likely that the next item
    // will be the item that is needed
    TInt count = iInitArray->Count();
    TInt previousIndex = iInitCurrentIndex;

    // First start the searching from the previous index
    CMuiuSettingBase* base = NextItemDoSearch( count, aExpectedId );

    if ( !base )
        {
        // Set the current index to 0 and continue from the beginning to
        // where the previous search started
        iInitCurrentIndex = KErrNotFound;
        base = NextItemDoSearch( ++previousIndex, aExpectedId );

        // Item can't be found at all, no means to continue, panic
        __ASSERT_DEBUG( base != NULL,
            User::Panic( KIMSSettingsDialogPanic, KErrNotFound ) );
        }

    return base;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::NextItemDoSearch()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CIMSSettingsDialog::NextItemDoSearch(
    const TInt aLastItem,
    const TUid& aExpectedId )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::NextItemDoSearch, 0, KLogUi );

    CMuiuSettingBase* base = NULL;

    // Continue search until the item is found
    while ( !base && ++iInitCurrentIndex < aLastItem )
        {
        base = iInitArray->At( iInitCurrentIndex ).iItem;
        base = ( base->iItemId == aExpectedId ) ? base : NULL;
        }

    return base;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::SetInitializeFlags()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::SetInitializeFlags(
    const TInt aError )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::SetInitializeFlags, 0, KLogUi );

    SetFlags( aError, iInitSetFlag );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::SetFlags()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::SetFlags(
    const TInt aError,
    const TInt aFlag )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::SetFlags, 0, KLogUi );

    ChangeFlag( aFlag, ( aError != KErrNone || Flag( aFlag ) ) );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::SetStoreFlags()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::SetStoreFlags(
    const TBool aState,
    const TInt aFlag )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::SetStoreFlags, 0, KLogUi );

    ChangeFlag( aFlag, ( aState || Flag( aFlag ) ) );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::SetEmailSettingsState()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::SetEmailSettingsState()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::SetEmailSettingsState, 0, KLogUi );

    TBool save = EFalse;

    // Check if any item in the array has changed
    for ( TInt flag = EDialogMainMenu;
          --flag >= EDialogIncomingChanged && !save; )
        {
        // Check if any setting has changed
        save = Flag( flag );
        }

    // Set the save status
    ChangeFlag( EImumSettingSave, save );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::GetStorerFlag()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::GetStorerFlag(
    const CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::GetStorerFlag, 0, KLogUi );

    switch ( aBaseItem.iItemResourceId )
        {
        case R_IMAS_INCOMING_SETTINGS:
            return EDialogIncomingChanged;

        case R_IMAS_OUTGOING_SETTINGS:
            return EDialogOutgoingChanged;

        case R_IMAS_USER_PREFERENCES:
            return EDialogUserPrefChanged;

        case R_IMAS_RETRIEVAL_LIMIT:
            return EDialogRetLimitChanged;

        case R_IMAS_DOWNLOAD_SETTINGS:
            return EDialogDownloadChanged;

        case R_IMAS_ALWAYSONLINE_SETTINGS:
            return EDialogAlOnlineChanged;

        default:
            break;
        }

    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::SubscribeFoldersL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::SubscribeFoldersL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::SubscribeFoldersL, 0, KLogUi );

    TMsvId service;
    TMsvEntry tEntry;
    User::LeaveIfError( iMailboxApi.MsvSession().GetEntry(
        iAccountSettings->iMailboxId, service, tEntry ) );


    // Flag whether username/password may have changed
    TBool logingInfoChanged = EFalse;
    TBuf8<KImasLoginLength> userName;
    TBuf8<KImasPasswordLength> password;

    if ( Flag( EDialogIncomingChanged ) )
        {
        // Username/password may have changed. They need to be pushed back
        // into settings database by CImapSubscriptionDialog if it tries to
        // log into email server to refresh folder list.
        logingInfoChanged = ETrue;
        StoreGetLoginInformation( userName, password );
        }

    // Run the dialog
    CImapSubscriptionDialog* subDlg =
        CImapSubscriptionDialog::NewL(
            iBaseUiFlags, tEntry.iRelatedId, CurrentMailboxId(),
            logingInfoChanged, userName, password, iMailboxApi );

    subDlg->PrepareLC( R_IMUM_FOLDER_SUBSRCIPTION_DIALOG );
    subDlg->RunLD();

    // We need to delay the CIMSSettingsDialog exit so that dialogs beneath it
    // has time to close properly
    if ( !iIdle->IsActive() )
    	{
    	iIdle->Start( TCallBack( &CIMSSettingsDialog::DelayedExecFlags, this));
    	}

    // If username/password are not defined or are wrong and subscription
    // dialog logged into IMAP server, login dialog is presented to user
    // and login info may have changed .
    // Note: logingInfoChanged is in/out parameter to subscription dialog.
    // On return it is true if the dialog logged into IMAP server.
    if ( logingInfoChanged )
        {
        TBool isPasswordTemporary;
        GetLoginInformationL( userName, password, isPasswordTemporary );
        if ( isPasswordTemporary )
            {
            password.Zero();
            }
        if ( Flag( EDialogIncomingChanged ) )
            {
            // Store into changed values
            StoreSetLoginInformation( userName, password );
            }
        else
            {
            // Update login info into our copy of settings
            iAccountSettings->iImap4Settings->SetLoginNameL( userName );
            iAccountSettings->iImap4Settings->SetPasswordL( password );
            }
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::CheckSubscribedFolderStateL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::CheckSubscribedFolderStateL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::CheckSubscribedFolderStateL, 0, KLogUi );

    TMsvId mailboxId = ( iAccountSettings->iIsImap4 ) ?
        iAccountSettings->iImap4AccountId.iImapService :
        iAccountSettings->iPop3AccountId.iPopService;
    TBool hasFolders =
        iMailboxApi.MailboxUtilitiesL().HasSubscribedFoldersL( mailboxId );
    SetHideItem( !hasFolders, ToUid( EIMASLimitFolders ), ETrue );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::LaunchIapPageL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::LaunchIapPageL(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::LaunchIapPageL, 0, KLogUi );

    CIMSSettingsAccessPointItem* iapItem =
        static_cast<CIMSSettingsAccessPointItem*>( &aBaseItem );

    TInt result = iMailboxApi.CommDbUtilsL().LaunchIapPageL( *iapItem );

    // Finalize the accesspoint item, after the setting is successfully set
    switch ( result )
        {
        case KErrNone:
            SetItemIapItemL( *iapItem );
            break;
        // Quit immediately
        case KErrAbort:
            SetFlag( EImumSettingShouldExit );
            break;
        case KErrCancel:
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::FindMultilineEditorL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::FindMultilineEditorL(
    const TUid& aId,
    CMuiuSettingsEditValue*& aTopEditor,
    CMuiuSettingsEditValue*& aBottomEditor )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::FindMultilineEditorL, 0, KLogUi );

    CMuiuSettingBase* baseItem = GetItem( aId );

    if ( baseItem )
        {
        CMuiuSettingsEditValue* top =
            static_cast<CMuiuSettingsEditValue*>( GetSubItem( *baseItem, 0 ) );
        aTopEditor = top;
        CMuiuSettingsEditValue* bottom =
            static_cast<CMuiuSettingsEditValue*>( GetSubItem( *baseItem, 1 ) );
        aBottomEditor = bottom;
        top = NULL;
        bottom = NULL;
        }

    User::LeaveIfNull( baseItem );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::SettingLaunchMultilineEditorL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsDialog::SettingLaunchMultilineEditorL(
    CMuiuSettingBase& /* aBaseItem */,
    TMuiuDynSetUpdateMode& /* aUpdateMode */ )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::SettingLaunchMultilineEditorL, 0, KLogUi );

    // Launches Always Online multiline editor, although this function
    // is meant to be generic function.

    CMuiuSettingsEditValue* sub1 = NULL;
    CMuiuSettingsEditValue* sub2 = NULL;
    FindMultilineEditorL( ToUid( EIMASEditAoHoursEditor ), sub1, sub2 );
    __ASSERT_DEBUG( sub1 && sub2, User::Panic(
        KIMSSettingsDialogPanic, KErrNotFound ) );

    // Take times
    TTime thisTime = sub1->Value();
    TTime nextTime = sub2->Value();


    CAknMultiLineDataQueryDialog* multiLineDlg =
        CAknMultiLineDataQueryDialog::NewL( thisTime, nextTime );

    CAknLocalScreenClearer* localScreenClearer = CAknLocalScreenClearer::NewLC( EFalse );


    // Execute the dialog
    if ( multiLineDlg->ExecuteLD( R_IMAS_SETTINGS_DIALOG_HOURS_QUERY ) )
        {
        CleanupStack::PopAndDestroy(localScreenClearer);

        // Save the values
        sub1->SetValue( thisTime.Int64() );
        sub2->SetValue( nextTime.Int64() );

        // Update the setting text
        CMuiuSettingsLinkExtended* linkItem =
            static_cast<CMuiuSettingsLinkExtended*>(
                GetItem( ToUid( EIMASAOHours ) ) );

        // Check if the times are the same, which means the system should be
        // on for next 24 hours
        if ( thisTime == nextTime )
            {
            UpdateItemDefaultSettingTextL( *linkItem );
            }
        else
            {
            UpdateItemCustomSettingTextL( *linkItem, *sub1, *sub2 );
            }

        linkItem = NULL;
        sub1 = NULL;
        sub2 = NULL;

        return EMuiuPageEventResultApproved;
        }
    else
        {
        CleanupStack::PopAndDestroy(localScreenClearer);

        sub1 = NULL;
        sub2 = NULL;

        return EMuiuPageEventResultCancel;
        }
    }
// ----------------------------------------------------------------------------
// CIMSSettingsDialog::UpdateItemDefaultSettingTextL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::UpdateItemDefaultSettingTextL(
    CMuiuSettingsLinkExtended& aLinkItem ) const
    {
    IMUM_CONTEXT( CIMSSettingsDialog::UpdateItemDefaultSettingTextL, 0, KLogUi );

    HBufC* format = StringLoader::LoadLC(
            R_IMAS_SETTINGS_DIALOG_ALWAYS_ALLHOURS );

    aLinkItem.iItemSettingText->Copy( *format );
    aLinkItem.SetValue( 0 );

    CleanupStack::PopAndDestroy( format );
    format = NULL;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::UpdateItemCustomSettingTextL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::UpdateItemCustomSettingTextL(
    CMuiuSettingsLinkExtended& aLinkItem,
    CMuiuSettingsEditValue& aItemFrom,
    CMuiuSettingsEditValue& aItemTo ) const
    {
    IMUM_CONTEXT( CIMSSettingsDialog::UpdateItemCustomSettingTextL, 0, KLogUi );

    // Define variables for time
    TTime thisTime = aItemFrom.Value();
    TTime nextTime = aItemTo.Value();

    HBufC* format = StringLoader::LoadLC(
        R_QTN_TIME_USUAL_WITH_ZERO );

    // Copy the time to text
    RBuf timeText1;
    RBuf timeText2;
    timeText1.CreateMaxL( KMuiuDynMaxSettingsLongTextLength );
    timeText2.CreateMaxL( KMuiuDynMaxSettingsLongTextLength );

    thisTime.FormatL( timeText1, *format );
    nextTime.FormatL( timeText2, *format );
    CleanupStack::PopAndDestroy( format );
    format = NULL;

    RBuf tempText1;
    RBuf tempText2;
    tempText1.CreateMaxL( KMuiuDynMaxSettingsLongTextLength );
    tempText2.CreateMaxL( KMuiuDynMaxSettingsLongTextLength );

    // Load the template
    StringLoader::Load(
        tempText1, R_IMAS_SETTINGS_DIALOG_BETWEEN_FORMAT );

    // Fetch the times
    StringLoader::Format( tempText2, tempText1, 0, timeText1 );
    StringLoader::Format( tempText1, tempText2, 1, timeText2 );

    AknTextUtils::LanguageSpecificNumberConversion( tempText1 );

    // Finally copy the text to item
    aLinkItem.iItemSettingText->Copy( tempText1 );
    aLinkItem.SetValue( 1 );

    timeText1.Close();
    timeText2.Close();
    tempText1.Close();
    tempText2.Close();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::UpdateItemText()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::UpdateItemText(
    const TUid& aItemId )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::UpdateItemText, 0, KLogUi );

    CMuiuSettingsLinkExtended* radioEditor =
        static_cast<CMuiuSettingsLinkExtended*>(
            GetItem( aItemId ) );

    CMuiuSettingsEditValue* editor =
        static_cast<CMuiuSettingsEditValue*>(
            SettingFindEditor( *radioEditor, radioEditor->Value(), EFalse ) );

    if ( editor )
        {
        SetItemText( *radioEditor, *editor->iItemSettingText );
        }
    else
        {
        CMuiuSettingBase* base = GetSubItem( *radioEditor,
            radioEditor->Value() );

        if ( base )
            {
            SetItemText( *radioEditor, *base->iItemLabel );
            }
        base = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::UpdateFetchLimitSetting()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::UpdateFetchLimitSetting()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::UpdateFetchLimitSetting, 0, KLogUi );

    TUid inboxId = ToUid( EIMASLimitInbox );
    TUid folderId = ToUid( EIMASLimitFolders );

    // Always Online is turned on
    if ( Flag( EDialogAlwaysOnlineOn ) )
        {
        SetReadOnly( inboxId, ETrue );
        SetReadOnly( folderId, ETrue );

        TMuiuSettingsText fetchAllText;
        StringLoader::Load(
            fetchAllText, R_IMAS_SETTINGS_DIALOG_FETCH_HEADERS_ALL );
        SetItemText( *GetItem( inboxId ), fetchAllText );
        SetItemText( *GetItem( folderId ), fetchAllText );
        }
    else
        {
        SetReadOnly( inboxId, EFalse );
        SetReadOnly( folderId, EFalse );

        // Update the setting text for the items
        UpdateItemText( ToUid( EIMASLimitInbox ) );
        UpdateItemText( ToUid( EIMASLimitFolders ) );
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::EventUpdateAlwaysOnline()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::EventUpdateAlwaysOnline(
    const TInt aNewState )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::EventUpdateAlwaysOnline, 0, KLogUi );

    // Turn always online off?
    if ( aNewState == EMailAoOff )
        {
        ClearFlag( EDialogAlwaysOnlineOn );
        }
    // Turn it on instead
    else
        {
        // show warning about increasing costs if always online switched on.
        // Warning on / warning off is variated by operators so we have
        // to check feature from central repository
        TBool doNotShowNote = EFalse;
        TRAP_IGNORE( doNotShowNote = MsvEmailMtmUiFeatureUtils::LocalFeatureL(
            KCRUidMuiuVariation, KMuiuEmailConfigFlags,
            KEmailFeatureIdSuppressBillingNote ) );
		// if LocalFeatureL should leave, doNotShowNote == EFalse

        if ( !doNotShowNote && !Flag( EDialogAlwaysOnlineOn ) )
            {
            TRAP_IGNORE( CIMSSettingsNoteUi::ShowNoteL(
                R_MCE_SETTINGS_MAIL_ENABLING_COST,
                EIMSInformationNote, ETrue ) );
            }

        SetFlag( EDialogAlwaysOnlineOn );
        }

    InitializeAlwaysOnlineState();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::EventUpdateMailNotifications()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::EventUpdateMailNotifications(
    const TInt aNewState )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::EventUpdateMailNotifications, 0, KLogUi );

    ChangeFlag( EDialogOmaEmnOn, aNewState != EMailEmnOff );

    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::EventAOForceTurnOff()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::EventAOForceTurnOff()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::EventAOForceTurnOff, 0, KLogUi );

    CMuiuSettingsLinkExtended* alwaysOnline =
        static_cast<CMuiuSettingsLinkExtended*>(
            GetUninitializedItem( TUid::Uid( EIMASAORolling ) ) );

	// In case we exit Settings view new state must be store
    SetFlag( EDialogAlOnlineChanged );

    InitAnyItem( *alwaysOnline, EMailAoOff, NULL );
    ClearFlag( EDialogAlwaysOnlineOn );
    InitializeAlwaysOnlineState();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::EventEMNForceTurnOff()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::EventEMNForceTurnOff()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::EventEMNForceTurnOff, 0, KLogUi );

    CMuiuSettingsLinkExtended* emn =
        static_cast<CMuiuSettingsLinkExtended*>(
            GetItem( TUid::Uid( EIMASMailNotifications ) ) );

    InitAnyItem( *emn, EMailEmnOff, NULL );
    ClearFlag( EDialogOmaEmnOn );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::EventUpdateAoDays()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::EventUpdateAoDays(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::EventUpdateAoDays, 0, KLogUi );
    IMUM_IN();

    TInt resource = 0;

    if ( aBaseItem.Value() == KIMASEveryDayMask )
    {
        resource = R_IMAS_SETTINGS_DIALOG_DAY_LIST_ALL;
    }
    else
    {
        resource = R_IMAS_SETTINGS_DIALOG_DAY_LIST_USERDEFINED;
    }

    RBuf text;
    TRAP_IGNORE( text.Assign( StringLoader::LoadL( resource ) ) );
    SetItemText( ToUid( EIMASAODays), text, ETrue );
    text.Close();

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::HideItemsPermanently()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::HideItemsPermanently()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::HideItemsPermanently, 0, KLogUi );

    // The HidePermanently functions contain build in refresh
    // system, so prevent refreshing to limit needless function
    // calls from the listbox
    TBool isImap = iAccountSettings->iIsImap4;
    iArrayFlags.ClearFlag( EMuiuDynArrayAllowRefresh );

    // In case Always Online is not supported,
    // hide the menuitem from the array
    HidePermanently(
        ToUid( EIMASMainAlwaysOnline ),
        !iBaseUiFlags.GF( EMailFeatureAlwaysOnlineEmail ) );

    HidePermanently(
        ToUid( EIMASMailNotifications ),
        !iBaseUiFlags.GF( EMailFeatureOmaEmn ) );

    // POP3 Protocol

    // Download Control: Unix path
    HidePermanently( ToUid( EIMASUserUnixPath ), !isImap );

    // Download Control: Retrieval Limit: Suscribed Folders
    HidePermanently( ToUid( EIMASLimitFolders ), !isImap );

    // Download Control: Folder subscription
    HidePermanently( ToUid( EIMASUserFolderSubscription ), !isImap );

    // Download Control: Retrieved parts: Total mailbody radio button
    HidePermanently( ToUid( EIMASBtnRetrieveBody ), ETrue );

    // IMAP4 Protocol

    // Download Control: Retrieved parts
    HidePermanently( ToUid( EIMASBtnRetrieveLimit ), isImap );
    HidePermanently( ToUid( EIMASBtnRetrieveBodyAtt ), isImap );

    // Incoming Settings: APOP
    iArrayFlags.SetFlag( EMuiuDynArrayAllowRefresh );
    HidePermanently( ToUid( EIMASIncomingAPop ), isImap );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::CheckPort()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::CheckPort(
    CMuiuSettingBase& aBaseItemPort,
    const TBool aIncoming )
    {
    IMUM_CONTEXT( CIMSSettingsDialog::CheckPort, 0, KLogUi );

    // Check whether port is user defined or not. If user define, do nothing
    // otherwise set default port based on security setting.
    TInt id = aIncoming ? EIMASIncomingSecurity : EIMASOutgoingSecurity;
    CMuiuSettingBase* base = GetItem( ToUid( id ) );

    // Get value of security setting.
    TIMASMailSecurity security =
        static_cast<TIMASMailSecurity>( base->Value() );

    // Check whether port setting is default or user defined
    if ( !aBaseItemPort.Value() )    // Default
        {
        TUint32 defaultPort( MsvEmailMtmUiUtils::GetDefaultSecurityPort(
            security, aIncoming, iAccountSettings->iIsImap4 ) );

        CMuiuSettingsEditValue* portValueEditor =
            static_cast<CMuiuSettingsEditValue*>(
                SettingFindEditor(
                    aBaseItemPort,
                    ToUid( EIMASBtnPortUserDefined ) ) );

        InitAnyItem( *portValueEditor, defaultPort, NULL );
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::ClosePortDialog()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::ClosePortDialog(
	CMuiuSettingBase& aBaseItem )
	{
	IMUM_CONTEXT( CIMSSettingsDialog::ClosePortDialog, 0, KLogUi );

	CheckPort( aBaseItem, aBaseItem.iItemId.iUid == EIMASIncomingPort );
    UpdateItemText( aBaseItem.iItemId );

    // Set setting dialog resource back to normal
	SetSettingPageResource(
	    EIPRNumber,
	    R_MUIU_SETTINGS_DIALOG_NUMBER );
	}

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::UpdateCSDFlagL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::UpdateCSDFlagL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::UpdateCSDFlagL, 0, KLogUi );

    CIMSSettingsAccessPointItem* iapItem = NULL;
    TInt error = FindItem( iapItem, TUid::Uid( EIMASIncomingIap ) );
    if ( ( error == KErrNone ) && ( iapItem->iIap.iResult == CMManager::EDestination ) )
        {
        iMailboxApi.CommDbUtilsL().IsApBearerCircuitSwitchedL( iapItem->iIap.iId ) ?
            SetFlag( EDialogCsdAccessPoint ) :
            ClearFlag( EDialogCsdAccessPoint );
        }
    else
        {
        ClearFlag( EDialogCsdAccessPoint );
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::UpdateAOIntervalsL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::UpdateAOIntervalsL()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::UpdateAOIntervalsL, 0, KLogUi );
    UpdateCSDFlagL();
    CMuiuSettingBase* aoIntervals = GetItem( ToUid( EIMASAOInterval ) );
    if ( aoIntervals )
        {
        HBufC* text = NULL;
        TMuiuSettingsText* muiuText = new (ELeave) TMuiuSettingsText;
        TInt valueInMinutes = GetAoIntervalTimeInMinutes();

        // After InitializeAoIntervalEditor function call, valueInMinutes
        // contains the index of the radio button in UI.
        InitializeAoIntervalEditor( valueInMinutes, text );

        muiuText->Copy( *text );
        InitAnyItem( *aoIntervals, valueInMinutes, muiuText );
        delete muiuText;
        delete text;
        text = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::GetHelpContext()
// ----------------------------------------------------------------------------
//
void CIMSSettingsDialog::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    IMUM_CONTEXT( CIMSSettingsDialog::GetHelpContext, 0, KLogUi );

    CMuiuSettingBase* baseItem = CurrentItem();

    switch ( baseItem->iItemResourceId )
        {
        case R_IMAS_MAIN_MENU:
            aContext.iContext = KMCE_HLP_SETT_EMAILMAIN;
            break;

        case R_IMAS_INCOMING_SETTINGS:
            aContext.iContext = KMBXS_HLP_SETT_MBX_IN;
            break;

        case R_IMAS_OUTGOING_SETTINGS:
            aContext.iContext = KMBXS_HLP_SETT_MBX_OUT;
            break;

        case R_IMAS_USER_PREFERENCES:
            aContext.iContext = KMCE_HLP_SETT_MAIL_USER;
            break;

        case R_IMAS_RETRIEVAL_LIMIT:
            aContext.iContext = KMBXS_HLP_SETT_DOWNL_RETR;
            break;

        case R_IMAS_DOWNLOAD_SETTINGS:
            aContext.iContext = KMBXS_HLP_SETT_DOWNL_CTRL;
            break;

        case R_IMAS_ALWAYSONLINE_SETTINGS:
            aContext.iContext = KMCE_HLP_SETT_MAIL_ONLI;
            break;

        case R_IMAS_MAILBOX_MENU:
            aContext.iContext = KMBXS_HLP_SETT_MBX_IN_OUT;
            break;

        default:
            break;
        }

    aContext.iMajor = KImumUid;
    }

// ---------------------------------------------------------------------------
// CIMSSettingsDialog::CurrentMailboxId()
// ---------------------------------------------------------------------------
//
TMsvId CIMSSettingsDialog::CurrentMailboxId()
    {
    IMUM_CONTEXT( CIMSSettingsDialog::CurrentMailboxId, 0, KLogUi );

    return iAccountSettings->iMailboxId;
    }

// ---------------------------------------------------------------------------
// CIMSSettingsDialog::HandleListBoxEventL()
// ---------------------------------------------------------------------------
//
void CIMSSettingsDialog::HandleListBoxEventL( CEikListBox* aListBox,
    TListBoxEvent aEventType )
    {
    // CMuiuDynamicSettingsDialog::HandleListBoxEventL() is overriden
    // in order to handle opening of settings item
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
        case EEventItemActioned:
            // Tapping on focused list box item has same effect as pressing msk
            KeyPressOKButtonL( MskCommand() );
            break;
        default:
            // CMuiuDynamicSettingsDialog takes care of changing middle
            // sotkey
            CMuiuDynamicSettingsDialog::HandleListBoxEventL( aListBox,
                aEventType );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CIMSSettingsDialog::DelayedExecFlags()
// ---------------------------------------------------------------------------
//
TInt CIMSSettingsDialog::DelayedExecFlags( TAny* aPtr )
    {
    static_cast<CIMSSettingsDialog*>(aPtr)->iDialog.ExecFlags();
    return EFalse;
    }


// ---------------------------------------------------------------------------
// CIMSSettingsDialog::GetLoginInformationL()
// ---------------------------------------------------------------------------
//
void CIMSSettingsDialog::GetLoginInformationL(
    TDes8& aUserName,
    TDes8& aPassword,
    TBool& aPasswordTemporary )
    {
    // Retrieve the settings of the mailbox
    const MImumInMailboxUtilities& utils = iMailboxApi.MailboxUtilitiesL();
    TMsvId mboxId = CurrentMailboxId();
    TMsvEntry entry = utils.GetMailboxEntryL( mboxId );
    CImumInSettingsData* accountsettings =
        iMailboxApi.MailboxServicesL().LoadMailboxSettingsL( mboxId );
    CleanupStack::PushL( accountsettings );
    // Incoming settings
    MImumInSettingsDataCollection* connection = &accountsettings->GetInSetL( 0 );

    // Read the data
    User::LeaveIfError( connection->GetAttr(
        TImumDaSettings::EKeyUsername, aUserName ) );
    User::LeaveIfError( connection->GetAttr(
        TImumDaSettings::EKeyPassword, aPassword ) );
    User::LeaveIfError( connection->GetAttr(
        TImumInSettings::EKeyTemporaryPassword, aPasswordTemporary ) );

    // Prepare the ending
    CleanupStack::PopAndDestroy( accountsettings );
    }

// End of File
