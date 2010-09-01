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
*     Mce delivery report view container control class implementation.
*
*/



// INCLUDE FILES
#include <aknnotedialog.h>      // Popup note
#include <mce.rsg>
#include <AknsUtils.h>

#include "MceDeliveryReportControlContainer.h"
#include "MceDeliveryReportAdapter.h"
#include "MceLogEngine.h"
#include "mceui.h"
#include "MceCommands.hrh"       // UID enumeration.

#include <bldvariant.hrh>

#include <csxhelp/mce.hlp.hrh>
#include "MceIds.h"

#include <muiu.mbg>
#include <akncontext.h>

// security data caging
#include <data_caging_path_literals.hrh>

#include <centralrepository.h>
#include <LogsDomainCRKeys.h>

#include "MceSessionHolder.h"

// CONSTANTS

_LIT( KMceDirAndFileMuiu,"muiu.mbm" );

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
_LIT( KPanicText, "Mce" );
const TInt KCRepositorySettingFailure = 1;
#endif

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif


// ================= MEMBER FUNCTIONS =======================

CMceDeliveryReportControlContainer* CMceDeliveryReportControlContainer::NewL(
    CMceDeliveryReportView* aMceDeliveryReportView,
    CMceSessionHolder& aSessionHolder, const TRect& aRect )
    {
    CMceDeliveryReportControlContainer* self = new( ELeave )
                    CMceDeliveryReportControlContainer( aMceDeliveryReportView,
                    									aSessionHolder );
    self->SetMopParent( aMceDeliveryReportView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }

CMceDeliveryReportControlContainer::CMceDeliveryReportControlContainer(
    CMceDeliveryReportView* aMceDeliveryReportView,
    CMceSessionHolder& aSessionHolder ) : iSessionHolder(aSessionHolder)
    {
    iMceDeliveryReportView = aMceDeliveryReportView;
    iSessionHolder.AddClient();
    }


void CMceDeliveryReportControlContainer::ConstructL( const TRect& aRect )
    {
    // security data caging
    TParse fp;
   
    fp.Set( KMceDirAndFileMuiu, &KDC_APP_BITMAP_DIR , NULL );
    iFilenameMuiu = fp.FullName();

    /// Register for model observervation.
    iMceDeliveryReportView->MceAppUi()->LogEngine()->SetContainerObserver( this );

    CreateWindowL();

    iListBox = new( ELeave ) CAknDoubleGraphicStyleListBox;
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, EAknListBoxSelectionList|EAknListBoxItemSpecificMenuDisabled );

    CMceDeliveryReportAdapter* adapter = CMceDeliveryReportAdapter::NewL(
                    iMceDeliveryReportView->MceAppUi()->LogEngine(), iListBox );
    iListBox->Model()->SetItemTextArray( adapter );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    // Load icons.
    CAknIconArray* icons = new( ELeave ) CAknIconArray( 2 );
    CleanupStack::PushL( icons );

     // First icon & mask.   
    AddIconL( KAknsIIDQgnPropMceSmsInfo, icons, iFilenameMuiu, EMbmMuiuQgn_prop_mce_sms_info,
              EMbmMuiuQgn_prop_mce_sms_info_mask );
    // Second icon & mask.
    AddIconL( KAknsIIDQgnPropMceMmsInfo, icons, iFilenameMuiu, EMbmMuiuQgn_prop_mce_mms_info,
              EMbmMuiuQgn_prop_mce_mms_info_mask );

    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );
    CleanupStack::Pop( icons );

    /// Check if Logs application has stopped logging?


    MakeEmptyTextListBoxL( iListBox, R_MCE_DELIVERY_REPORT_NO_LOG_LBX_TEXT );

    CEikScrollBarFrame* sBFrame = iListBox->CreateScrollBarFrameL(ETrue);
    sBFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto );

    SetRect( aRect );
    // Activate the control and all subcontrols
    ActivateL();
    }


CMceDeliveryReportControlContainer::~CMceDeliveryReportControlContainer()
    {
    /// Stop observing LogEngine.
    if( iMceDeliveryReportView )
        {
        iMceDeliveryReportView->MceAppUi()->LogEngine()
            ->RemoveContainerObserver();
        }

    /// Destroy the list box control
    delete iListBox;
    iSessionHolder.RemoveClient();
    }

TKeyResponse CMceDeliveryReportControlContainer::OfferKeyEventL(
                            const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if( aType == EEventKey )
        {
        if ( aKeyEvent.iCode == EKeyLeftArrow || aKeyEvent.iCode == EKeyRightArrow )
            {
            // needed for tab changing, should be left to MCE UI to handle these
            return EKeyWasNotConsumed;
            }

        if( aKeyEvent.iCode == EKeyPhoneSend )
            {
            if( iMceDeliveryReportView->MceAppUi()->
                        LogEngine()->IsNumberAvailable( iListBox->CurrentItemIndex() ) )
                {
                iMceDeliveryReportView->HandleCommandL( EMceCmdCall );
                }
            return EKeyWasConsumed;
            }
        if( aKeyEvent.iCode == EKeyOK ||  aKeyEvent.iCode == EKeyEnter )
            {
            if( iMceDeliveryReportView->MceAppUi()->
                        LogEngine()->IsNumberAvailable( iListBox->CurrentItemIndex() ) )
                {
                iMceDeliveryReportView->LaunchPopupMenuL( R_MCE_DELIVERY_REPORT_OK_MENUBAR );
                }
            return EKeyWasConsumed;
            }
        }
    if( aType == EEventKeyUp )
		{
		iMceDeliveryReportView->SetMSKButtonL();
		}
        
    return iListBox->OfferKeyEventL( aKeyEvent, aType );
    }

TInt CMceDeliveryReportControlContainer::CountComponentControls() const
    {
    return 1;
    }

CCoeControl* CMceDeliveryReportControlContainer::ComponentControl(
    TInt /*aIndex*/ ) const
    {
    return iListBox;
    }

void CMceDeliveryReportControlContainer::SizeChanged()
    {
    if( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }


CAknDoubleGraphicStyleListBox* CMceDeliveryReportControlContainer::ListBox() const
    {
    return iListBox;
    }


void CMceDeliveryReportControlContainer::PopUpNoteL()
    {
    CAknNoteDialog* noteDlg = new(ELeave) CAknNoteDialog(
                            CAknNoteDialog::ENoTone,
                            CAknNoteDialog::ELongTimeout );
    noteDlg->ExecuteLD( R_MCE_DR_NO_LOG_NOTE );
    }


void CMceDeliveryReportControlContainer::MceLogEngineListBoxStateHasChangedL()
    {
    iListBox->HandleItemAdditionL();
    iMceDeliveryReportView->SetMSKButtonL();
    }


void CMceDeliveryReportControlContainer::HandleControlEventL
        ( CCoeControl* /*aControl*/,
          TCoeEvent /*aEventType*/ )
    {
    }

void CMceDeliveryReportControlContainer::Draw( const TRect& /*aRect*/ ) const
    {
    }

void CMceDeliveryReportControlContainer::AddIconL(
    TAknsItemID aId,
    CAknIconArray* aIcons,
    const TDesC& aIconFileWithPath,
    TInt aIconGraphicsIndex,
    TInt aIconGraphicsMaskIndex )
    {    
    // icon & mask.   

    MAknsSkinInstance* skins = AknsUtils::SkinInstance();  
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    AknsUtils::CreateIconL( skins, aId, bitmap,
        mask, aIconFileWithPath, aIconGraphicsIndex, 
        aIconGraphicsMaskIndex );
    // Append it to icons array
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::PushL( icon );
    aIcons->AppendL( icon );    
    CleanupStack::Pop( icon );
    }


TInt CMceDeliveryReportControlContainer::LogDisabledL()
    {
    TInt getValue( 1 );

    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL(KCRUidLogs) );
    CleanupStack::PushL( repository );

    if ( ret == KErrNone )
        {
        TInt ret = repository->Get( KLogsLoggingEnabled, getValue );
        __ASSERT_DEBUG( !ret, User::Panic(KPanicText,KCRepositorySettingFailure) );
        }

    CleanupStack::Pop( repository );
    delete repository;

    return getValue;
    }

void CMceDeliveryReportControlContainer::MakeEmptyTextListBoxL(
          CEikFormattedCellListBoxTypedef* aListBox,
          TInt aResourceText )
    {
    HBufC* textFromResourceFile = iCoeEnv->AllocReadResourceLC( aResourceText );
    // Set the view's listbox empty text.
    aListBox->View()->SetListEmptyTextL( *textFromResourceFile );
    CleanupStack::PopAndDestroy();  // textFromResourceFile.
    }

// ----------------------------------------------------
// CMceDeliveryReportControlContainer::GetHelpContext
// returns helpcontext as aContext
//
// ----------------------------------------------------
void CMceDeliveryReportControlContainer::GetHelpContext
        (TCoeHelpContext& aContext) const
    {
    aContext.iMajor = KMceApplicationUid;
    aContext.iContext = KMCE_HLP_DELIVERY_REPORTS;

    }

// ----------------------------------------------------
// CMceListContainer::HandleResourceChange
// ----------------------------------------------------
void CMceDeliveryReportControlContainer::HandleResourceChange( TInt aType )
    {   
    CCoeControl::HandleResourceChange( aType );
    if( aType == KAknsMessageSkinChange )
        {
        // change skin bitmap
        TRAP_IGNORE( SkinBitmapChangesL( ) );        
        }    
    }

// ----------------------------------------------------
// CMceListContainer::SkinBitmapChangesL
// ----------------------------------------------------
void CMceDeliveryReportControlContainer::SkinBitmapChangesL( )
    {    
    //first change application icon
    
    CEikStatusPane *sp = ( (CAknAppUi*)CEikonEnv::Static()->EikAppUi() )->StatusPane();
    CAknContextPane* contextPane = (CAknContextPane *)sp->ControlL( TUid::Uid(EEikStatusPaneUidContext) );
    
    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    CFbsBitmap* appBitmap;
    CFbsBitmap* appBitmapMask;
    
#ifdef RD_MULTIPLE_DRIVE
    TInt phoneDrive;
    // At this point, try avoid using any predefined numbers E.G. EDriveC
    // in order to increase the safety and code flexibility
    User::LeaveIfError( DriveInfo::GetDefaultDrive( DriveInfo:: EDefaultPhoneMemory, phoneDrive ) );
    TInt currentDrive = TInt( iSessionHolder.Session()->CurrentDriveL() );
    if ( currentDrive != phoneDrive )
#else
    if ( TInt(iSessionHolder.Session()->CurrentDriveL()) == EDriveE )
#endif //RD_MULTIPLE_DRIVE
        {
#ifndef __SCALABLE_ICONS
        AknsUtils::CreateIconL( skins, KAknsIIDQgnMenuMceMmcCxt, appBitmap,
           appBitmapMask, iFilenameMuiu, EMbmMuiuQgn_menu_mce_mmc_cxt,
           EMbmMuiuQgn_menu_mce_mmc_cxt + 1 );        
#else
        AknsUtils::CreateIconL( skins, KAknsIIDQgnMenuMceMmcCxt, appBitmap,
           appBitmapMask, iFilenameMuiu, EMbmMuiuQgn_menu_mce_mmc,
           EMbmMuiuQgn_menu_mce_mmc + 1 );
#endif
        }
    else
        {
#ifndef __SCALABLE_ICONS
        AknsUtils::CreateIconL( skins, KAknsIIDQgnMenuMceCxt, appBitmap,
           appBitmapMask, iFilenameMuiu, EMbmMuiuQgn_menu_mce_cxt,
           EMbmMuiuQgn_menu_mce_cxt + 1 );
#else
        AknsUtils::CreateIconL( skins, KAknsIIDQgnMenuMceCxt, appBitmap,
           appBitmapMask, iFilenameMuiu, EMbmMuiuQgn_menu_mce,
           EMbmMuiuQgn_menu_mce + 1 );
#endif
        } 
    contextPane->SetPicture( appBitmap, appBitmapMask );
    //change other bitmaps according to the skins

    CArrayPtr<CGulIcon> *icons = 0;
    icons = iListBox->ItemDrawer()->
        ColumnData()->IconArray();
    TAknsItemID id; 
    id.iMajor = 0;
    id.iMinor = 0;    

    TInt count = icons->Count();
    TInt bitmapIndex = 0;
    TInt bitmapMaskIndex = 0;    
    
    for ( TInt cc=count; --cc>=0; )
        {
        switch( cc )
            {
            case EMceDeliverySmsInfo:                
                bitmapIndex = EMbmMuiuQgn_prop_mce_sms_info;                
                bitmapMaskIndex = EMbmMuiuQgn_prop_mce_sms_info_mask;
                id = KAknsIIDQgnPropMceSmsInfo;
                break;
            case EMceDeliveryMmsInfo:
            default:
                bitmapIndex = EMbmMuiuQgn_prop_mce_mms_info;
                bitmapMaskIndex = EMbmMuiuQgn_prop_mce_mms_info_mask;
                id = KAknsIIDQgnPropMceMmsInfo;
                break;           
            } 
        
        CFbsBitmap* bitmap;
        CFbsBitmap* mask;
        AknsUtils::CreateIconL( skins, id, bitmap,
            mask, iFilenameMuiu /*iFilenameLog*/, bitmapIndex, bitmapMaskIndex );      

        CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
     
        CleanupStack::PushL( icon );
        CGulIcon* iconToBeDelete = icons->At( cc );
        icons->At( cc ) = icon;
        delete iconToBeDelete;
        iconToBeDelete = NULL;

        CleanupStack::Pop(); // icon
    
        }  
    }
    
// ----------------------------------------------------
// CMceListContainer::FocusChanged
// ----------------------------------------------------
void CMceDeliveryReportControlContainer::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    if (iListBox)
        {
        iListBox->SetFocus(IsFocused());
        }
    }


//  End of File
