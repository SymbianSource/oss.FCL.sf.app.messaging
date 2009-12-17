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
* Description:  Base class for mail settings and wizard
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <avkon.hrh>
#include <muiuflagger.h>
#include <e32base.h>
#include <hlplch.h>                     // HlpLauncher
#include <coeaui.h>                     // CCoeAppUi

#include "IMSSettingsBaseUi.h"
#include "ImumInSettingsData.h"
#include "IMSSettingsItemApprover.h"    // CIMSSettingsItemApprover
#include "IMSSettingsItemAccessPoint.h" // CIMSSettingsAccessPointItem
#include "ImumMboxDefaultData.h"            // CImumMboxDefaultData
#include "ImumMboxSettingsCtrl.h"          // CImumMboxSettingsCtrl
#include "ImumInternalApiImpl.h"            // CImumInternalApiImpl
#include "ImumUtilsLogging.h"
#include "EmailFeatureUtils.h"

#include <cmapplicationsettingsui.h>

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
// CIMSSettingsBaseUI::CIMSSettingsBaseUI()
// ----------------------------------------------------------------------------
//
CIMSSettingsBaseUI::CIMSSettingsBaseUI(
    CIMSSettingsUi& aDialog,
    CImumInternalApiImpl& aMailboxApi,
    CEikFormattedCellListBox& aListBox,
    CAknTitlePane& aTitlePane,
    CMuiuFlags& aFlags )
    :
    CMuiuDynamicSettingsDialog( aListBox, aTitlePane ),
    iBaseUiFlags( aFlags ),
    iDialog( aDialog ),
    iMailboxApi( aMailboxApi ),
    iItemApprover( NULL ),
    iNoteUi( NULL ),
    iDefaultData( NULL )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::CIMSSettingsBaseUI, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::~CIMSSettingsBaseUI()
// ----------------------------------------------------------------------------
//
CIMSSettingsBaseUI::~CIMSSettingsBaseUI()
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::~CIMSSettingsBaseUI, 0, KLogUi );

    delete iNoteUi;
    delete iItemApprover;
    delete iDefaultData;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::BaseUiConstructL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::BaseUiConstructL(
    CEikButtonGroupContainer& aSoftkeys,
    const TInt aSettingResourceId,
    const TBool aUpdateMSK )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::BaseUiConstructL, 0, KLogUi );

    // Add the complete menu tree to settings
    SettingsConstructL(
        aSoftkeys, aSettingResourceId, aUpdateMSK,
        iBaseUiFlags.GF( EMailFeatureIdSapPolicyManagement ) );

    iNoteUi = CIMSSettingsNoteUi::NewL( iMailboxApi, *this );

    iItemApprover =
        ( CIMSSettingsItemApprover::NewL(
          iMailboxApi, iMailboxApi.CommDbUtilsL(),
          *iNoteUi, *this ) );

    // Define default data
    iDefaultData = RestoreDefaultSettingsDataL();
    }

/******************************************************************************

    Event Handling

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::HandleSettingPageEventL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::HandleSettingPageEventL(
    CAknSettingPage* /* aSettingPage */,
    TAknSettingPageEvent /* aEventType */ )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::HandleSettingPageEventL, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::EventHandlerL()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsBaseUI::EventHandlerL( const TInt aCommandId )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::EventHandlerL, 0, KLogUi );
    IMUM_IN();

    TBool ok = ETrue;

    switch ( aCommandId )
        {
        // Show help dialog
        case EAknCmdHelp:
            iDialog.LaunchHelp();
            break;

        // User has pressed ok key
        case EAknSoftkeyOpen:
        case EAknSoftkeySelect:
        case EAknSoftkeyOk:
            KeyPressOKButtonL( aCommandId );
            break;

        case EAknSoftkeyOptions:
            SetFlag( EImumSettingOpenOptionsMenu );
            break;

        case EAknSoftkeyShow:
        case EAknSoftkeyDone:
        case EAknSoftkeyBack:
            KeyPressSoftkey( aCommandId );
            break;

        // Emergency exit
        case EAknSoftkeyCancel:
        case EAknCmdExit:
            SetFlag( EImumSettingShouldExit );
            break;

        default:
            ok = EFalse;
            break;
        }

    IMUM_OUT();
    return ok;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::DoQuitL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::DoQuitL()
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::DoQuitL, 0, KLogUi );

    ChangeFlag( EImumSettingQueryClose, !Flag( EImumSettingShouldExit ) );
    iDialog.DoQuitL();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CIMSSettingsBaseUI::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::OfferKeyEventL, 0, KLogUi );
    IMUM_IN();

    TBool ok = EFalse;

    // Handle the key events for wizard
    if ( aType == EEventKey )
        {
        switch ( aKeyEvent.iCode )
            {
            // Ignore left and right keys
            case EKeyLeftArrow:
            case EKeyRightArrow:
                ok = ETrue;
                break;

            // Selection key
            case EKeyOK:
            case EKeyEnter:
                ok = EventHandlerL( EAknSoftkeyOk );
                break;

            // Red button
            case EKeyPhoneEnd:
                ok = EventHandlerL( EAknSoftkeyCancel );
                break;

            default:
                break;
            }
        }

    TKeyResponse ret = ok ? EKeyWasConsumed : EKeyWasNotConsumed;

    if ( ret == EKeyWasNotConsumed )
        {
        ret = CMuiuDynamicSettingsDialog::OfferKeyEventL( aKeyEvent, aType );
        }

    IMUM_OUT();
    return ret;
    }


// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::ProcessCommandL
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::ProcessCommandL( TInt aCommandId )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::ProcessCommandL, 0, KLogUi );

    EventHandlerL( aCommandId );
    // There are no key events if touch UI is used. Therefore requested commands
    // are executed here.
    iDialog.ExecFlags();
    }

/******************************************************************************

    Private tools

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::RestoreDefaultSettingsDataL()
// ----------------------------------------------------------------------------
//
CImumMboxDefaultData* CIMSSettingsBaseUI::RestoreDefaultSettingsDataL()
    {
    return CImumMboxDefaultData::NewL( iMailboxApi );
    }

/******************************************************************************

    Public tools

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::HidePermanently()
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::HidePermanently(
    const TUid& aId,
    const TBool aNewState )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::HidePermanently, 0, KLogUi );

    // Set next item
    HidePermanently( *GetItem( aId ), aNewState );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::HidePermanently()
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::HidePermanently(
    CMuiuSettingBase& aBaseItem,
    const TBool aNewState )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::HidePermanently, 0, KLogUi );

    // Set next item
    aBaseItem.iItemFlags.ChangeFlag(
        EMuiuDynItemPermanentlyHidden, aNewState );
    Refresh();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::CreateCustomItemToArrayLC()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CIMSSettingsBaseUI::CreateCustomItemToArrayLC(
    const TUid& aId )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::CreateCustomItemToArrayLC, 0, KLogUi );

    CMuiuSettingBase* base = NULL;

    switch ( aId.iUid )
        {
        // Access point editors require special items
        case EIMAWIap:
        case EIMASIncomingIap:
        case EIMASOutgoingIap:
            base = CIMSSettingsAccessPointItem::NewLC();
            break;

        case EIMASUserFolderSubscription:
            base = CMuiuSettingsLinkExtended::NewLC();
            break;

        default:
            break;
        }

    return base;
    }


// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::SetItemIapItem()
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::SetItemIapItemL( CIMSSettingsAccessPointItem& aIapItem )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::SetItemIapItemL, 0, KLogUi );

    CMsvCommDbUtilities& commdbUtils= iMailboxApi.CommDbUtilsL();
    commdbUtils.GetWizardRadioButton( aIapItem );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::Flag()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsBaseUI::Flag( TUint aFlag )
    {
    return iBaseUiFlags.Flag( aFlag );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::SetFlag()
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::SetFlag( TUint aFlag )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::SetFlag, 0, KLogUi );

    iBaseUiFlags.SetFlag( aFlag );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::ClearFlag()
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::ClearFlag( TUint aFlag )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::ClearFlag, 0, KLogUi );

    iBaseUiFlags.ClearFlag( aFlag );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::ChangeFlag()
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::ChangeFlag(
    TUint aFlag,
    TBool aNewState )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::ChangeFlag, 0, KLogUi );

    iBaseUiFlags.ChangeFlag( aFlag, aNewState );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::Flags()
// ----------------------------------------------------------------------------
//
const CMuiuFlags& CIMSSettingsBaseUI::Flags()
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::Flags, 0, KLogUi );

    return iBaseUiFlags;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::FindItem()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsBaseUI::FindItem(
    CIMSSettingsAccessPointItem*& aItem,
    const TUid& aId )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::FindItem, 0, KLogUi );

    CMuiuSettingBase* base = GetUninitializedItem( aId );

    if ( base != NULL )
        {
        aItem = static_cast<CIMSSettingsAccessPointItem*>( base );
        return KErrNone;
        }

    return KErrNotFound;
    }


// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::FindItem()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsBaseUI::FindItem(
    CMuiuSettingsLinkExtended*& aItem,
    const TUid& aId )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::FindItem, 0, KLogUi );

    CMuiuSettingBase* base = GetUninitializedItem( aId );

    if ( base != NULL )
        {
        aItem = static_cast<CMuiuSettingsLinkExtended*>( base );
        return KErrNone;
        }

    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::GetUninitializedItem()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CIMSSettingsBaseUI::GetUninitializedItem(
    const TUid& aId )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::GetUninitializedItem, 0, KLogUi );

    return GetItem( aId );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::BackupItemCreateUndefinedL()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CIMSSettingsBaseUI::CreateItemForBackupL(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::CreateItemForBackupL, 0, KLogUi );

    CIMSSettingsAccessPointItem* iapBackup = NULL;

    if ( aBaseItem.iItemId.iUid == EIMAWIap )
        {
        iapBackup =
            CIMSSettingsAccessPointItem::NewL();

        CIMSSettingsAccessPointItem* iapItem =
            static_cast<CIMSSettingsAccessPointItem*>( &aBaseItem );

        *iapBackup = *iapItem;
        }

    return iapBackup;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::UpdateAccessPointItem()
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::UpdateAccessPointItem(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::UpdateAccessPointItem, 0, KLogUi );

    // Preprocessing for item is only need for iap accesspoint setting
    CIMSSettingsAccessPointItem* iapItem =
        static_cast<CIMSSettingsAccessPointItem*>( &aBaseItem );

    // Copy the text to the item, ignore any errors on copy
    TRAP_IGNORE(
        iMailboxApi.CommDbUtilsL().SettingsItemIapStringL( *iapItem ) );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::SetReadOnly()
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::SetReadOnly(
    const TUid& aId,
    const TBool aNewState )
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::SetReadOnly, 0, KLogUi );

    GetItem( aId )->iItemFlags.ChangeFlag( EMuiuDynItemReadOnly, aNewState );
    }


// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::GetHelpContext()
// ----------------------------------------------------------------------------
//
void CIMSSettingsBaseUI::GetHelpContext(
    TCoeHelpContext& /* aContext */ ) const
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::GetHelpContext, 0, KLogUi );

    // Empty
    }

// ---------------------------------------------------------------------------
// CIMSSettingsBaseUI::CurrentMailboxId()
// ---------------------------------------------------------------------------
//
TMsvId CIMSSettingsBaseUI::CurrentMailboxId()
    {
    IMUM_CONTEXT( CIMSSettingsBaseUI::CurrentMailboxId, 0, KLogUi );

    return KErrNotFound;
    }


//  End of File
