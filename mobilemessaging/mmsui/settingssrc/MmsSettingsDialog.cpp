/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides MMS Settings Dialog methods.
*
*/



// INCLUDE FILES
#include <barsread.h>
#include <mtclreg.h>

#include <eiktxlbx.h>
#include <aknnavi.h>
#include <akntitle.h>
#include <aknsettingpage.h>
#include <aknradiobuttonsettingpage.h>
#include <aknnotewrappers.h> 

#include <mmssettings.h> // MMS Engine settings

#include <MmsSettings.rsg>
#include <mmssettingsdefs.h>    // image size definitions

#include <ApSettingsHandlerUI.h>
#include <ApSettingsHandlerCommons.h>
#include <ApEngineConsts.h>
#include <ApUtils.h>
#include <ApDataHandler.h>
#include <ApAccessPointItem.h>
#include <ApSelect.h> //CApSelect

#include <muiusettingsarray.h>
#include <StringLoader.h>

#include <centralrepository.h>    // link against centralrepository.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys

#include <mmsclient.h>
#include <mmsconst.h>

// Features
#include <featmgr.h>    
#include <bldvariant.hrh>
#include <messagingvariant.hrh>
#include <csxhelp/mce.hlp.hrh>
#include <hlplch.h>   // For HlpLauncher

#include "unieditorfeatures.h"  // EUniFeaturePriority

#include "MmsMtmConst.h" 

#include "MmsApSelect.h"
#include "MmsSettingsDialog.h"
#include "MmsSettingsPanic.h"
#include "MmsSettings.hrh"

// CONSTANTS

const TUid KUidMce = {0x100058C5};

// Possible values for mms validity period in seconds
const TInt32 KMmsValidityPeriod1h = 3600;
const TInt32 KMmsValidityPeriod6h = 21600;
const TInt32 KMmsValidityPeriod24h = 86400;
const TInt32 KMmsValidityPeriod3Days = 259200;
const TInt32 KMmsValidityPeriodWeek = 604800;
const TInt32 KMmsValidityPeriodMax = 0;


// ---------------------------------------------------------
// CMmsSettingsDialog
// C++ constructor
// ---------------------------------------------------------
//
CMmsSettingsDialog::CMmsSettingsDialog(
    //CMmsClientMtm *aClientMtm,
    CMmsSettings* aMmsSettings,
    TMmsExitCode& aExitCode ) :
    //iMmsClient( aClientMtm ),
    iMmsSettings( aMmsSettings ),
    iExitCode( aExitCode ),
    iSettingsFlags( 0 )
    {
    iExitCode = EMmsExternalInterrupt; // reference -> cannot be initialized above
    }


// ---------------------------------------------------------
// ~CMmsSettingsDialog()
// Destructor
// ---------------------------------------------------------
//
CMmsSettingsDialog::~CMmsSettingsDialog()
    {
    delete iPreviousTitleText;
    delete iCommsDb;

    // remove default navi pane
    if (    iSettingsFlags & EDefaultNaviPane )
        {
        if ( iNaviPane )
            {                    
            // if "EDefaultNaviPane" flag is set
            // iNaviPane != 0
            iNaviPane->Pop(NULL);
            }
        }

    // For lint
    iNaviPane = NULL;
    iItems = NULL;
    //iMmsClient = NULL;
    iMmsSettings = NULL;
    iTitlePane = NULL;
    iListbox = NULL;
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::ProcessCommandL
//
// Processes menu item commands
// ---------------------------------------------------------
//
void CMmsSettingsDialog::ProcessCommandL( TInt aCommandId ) 
    {
    CAknDialog::ProcessCommandL( aCommandId );

    switch ( aCommandId )
        {
        case EChangeItem:
            HandleEditRequestL( iListbox->CurrentItemIndex(), EFalse );
            break;

        case EAknCmdHelp:
            {
            LaunchHelpL();                
            }
            break;

        case EExitItem:            
            ExitDialogL();
            break;
        default:
            break;
        }
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::ExitDialogL()
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::ExitDialogL()
    {
    iExitCode = EMmsExit;
    TryExitL( EAknSoftkeyCancel );
    }
        

// ---------------------------------------------------------
// DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane)
//  Initializes items on the menu
// ---------------------------------------------------------
//
void CMmsSettingsDialog::DynInitMenuPaneL(
    TInt aMenuId, 
    CEikMenuPane* aMenuPane)
    {
    if (    aMenuPane
        &&  aMenuId == R_SETTINGSDIALOG_MENUPANE )
        {
        // Set help menu-item visibility according featureManager
        if ( ! ( iSettingsFlags & EHelpFeatureSupported ) )
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }
        }
    }

    
// ---------------------------------------------------------
// CMmsSettingsDialog::OkToExitL
//
// ---------------------------------------------------------
//
TBool CMmsSettingsDialog::OkToExitL( TInt aKeyCode )
    {
    switch ( aKeyCode )
        {
        case EChangeItem:
            HandleEditRequestL( iListbox->CurrentItemIndex(), ETrue );
            return EFalse;
           
        case EAknSoftkeyCancel:
            {
            iSettingsFlags |= EExitSelected;
            iSettingsFlags |= EGoingToExit;
            if ( ClosingDialogL() )
                {
                iExitCode = EMmsExit;
                }
            else
                {
                iExitCode = EMmsExitWithSave;
                }

            return ETrue;
            }
        case EAknSoftkeyBack:
            {
            iSettingsFlags &= ~EExitSelected;
            ClosingDialogL();

            //if exit wanted during back operation
            if( iSettingsFlags & ESwitchToExit )
                {
                iExitCode = EMmsExit;
                }
            else//normal back
                {
                iExitCode = EMmsBack;
                }

            // Replace title with old title text
            if ( iPreviousTitleText  )
                {
                iTitlePane->SetTextL( *iPreviousTitleText );
                }
            return ETrue;
            }
        default :
            return CAknDialog::OkToExitL( aKeyCode );
        }
   
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::CheckAndCorrectAccessPointL()
//
// Checks the access point as a part of closing dialog
// (back / exit). AP must exist.
// ---------------------------------------------------------
//
TBool CMmsSettingsDialog::CheckAndCorrectAccessPointL()
    {
    TInt32 ap = iMmsSettings->AccessPoint( 0 );

    CApUtils* apUtils = CApUtils::NewLC(*iCommsDb);
    
    // Access point is compulsory
    TBool closingWithErrors = EFalse;   // ETrue if user wants to close down even with errors
    while ( !ApExistsL( apUtils, ap ) )
        {
        // Do not save if exit with errors
        if ( iSettingsFlags & EExitSelected )
            {
            closingWithErrors = ETrue;
            }
        else
            {
            closingWithErrors = ShowExitAnywayQueryL();
            }

        if ( closingWithErrors )
            {
            break;
            }
        //if user selects exit in ap dialog, we will exit.
        if ( EditAccessPointL() )
            {
            //when we return, this tells that we are exiting.
            iSettingsFlags |= ESwitchToExit;
            break;
            }
        ap = iMmsSettings->AccessPoint( 0 );
        }
    CleanupStack::PopAndDestroy(); // apUtils
    return closingWithErrors;
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::ShowExitAnywayQueryL() const
//
// ---------------------------------------------------------
//
TBool CMmsSettingsDialog::ShowExitAnywayQueryL() const
    {
    CAknQueryDialog* confirmQuery = CAknQueryDialog::NewL();
    return ( confirmQuery->ExecuteLD( R_MMSUI_QUERY_EXIT_ANYWAY ) );
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::ApExistsL(CApUtils* aUtils, TUint32 aAP) const
// Checks whether or not given AP exists
//
// ---------------------------------------------------------
//
TBool CMmsSettingsDialog::ApExistsL( CApUtils* aUtils, TInt32 aAP ) const
    {
    if ( aAP == KErrNotFound )
        {
        return EFalse;
        }
    TBool exists = EFalse;
    TRAPD( error, ( exists = aUtils->WapApExistsL( aAP ) ) );
    if ( exists )
        {
        return ETrue;
        }
    // we are only handling "not found" leave
    if ( error != KErrNotFound )
        {
        User::LeaveIfError( error );
        }
    return EFalse;
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::PreLayoutDynInitL()
//
// Creates a listbox for the settings.
// Values are read from the resource file.
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::PreLayoutDynInitL()
    {

    // Get supported features from feature manager.
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        iSettingsFlags |= EHelpFeatureSupported;
        }
    FeatureManager::UnInitializeLib();

    TInt featureBitmask = 0;
    CRepository* repository = CRepository::NewL( KCRUidMmsui );
    repository->Get( KMmsuiCsdEnabled, featureBitmask );
    if ( featureBitmask )
        {
        iSettingsFlags |= ECSDEnabled;
        }
    delete repository;
    repository = NULL;
    featureBitmask = 0;

    repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, featureBitmask );
    delete repository;
    repository = NULL;
    if ( featureBitmask & KMmsFeatureIdUserCreationMode )
        {
        iSettingsFlags |= ECreationModeChangeEnabled;
        }
    if ( featureBitmask & KMmsFeatureIdHideReceivingOff )
        {
        iSettingsFlags |= EReceivingOffDisabled;
        }
    if ( featureBitmask & KMmsFeatureIdHideManualFetchMode )
        {
        iSettingsFlags |= EReceivingAlwaysManualDisabled;
        }
    if ( featureBitmask & KMmsFeatureIdHideAutomaticAtHome )
        {
        iSettingsFlags |= EReceivingAutomaticHomeDisabled;
        }

    SetTitleTextL();

    iCommsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );

    // Get listbox pointer
    iListbox = static_cast<CEikTextListBox*>( Control( EDlgSettingListBox ) );
    iListbox->SetListBoxObserver( this );
    

    // Sets correct size for the listbox    
    iListbox->SetRect( iAvkonAppUi->ClientRect() );
    
    iItems = CMuiuSettingsArray::NewL( R_MMS_SERVICE_SETTINGS_LIST );
    GetServiceSettingsValuesL();

    // scroll bars
    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL( 
        CEikScrollBarFrame::EOff, 
        CEikScrollBarFrame::EAuto
        );

    // replace navi pane with empty one.
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( iEikonEnv->AppUiFactory()->StatusPane()->ControlL(
            TUid::Uid(EEikStatusPaneUidNavi) ) );
    iNaviPane->PushDefaultL();
    iSettingsFlags |= EDefaultNaviPane;

    CTextListBoxModel* model = iListbox->Model();
    model->SetItemTextArray( iItems );
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::ClosingDialogL
//
// ---------------------------------------------------------
//
TBool CMmsSettingsDialog::ClosingDialogL()
    {
    //if user selects exit or outside exit occurs, no ap checks are made.
    return CheckAndCorrectAccessPointL();
    }

// ---------------------------------------------------------
// CMmsSettingsDialog::GetServiceSettingsValuesL
// Gets the values for the mms service settings and puts them in
// to the listbox. Text strings are from loc files.
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::GetServiceSettingsValuesL()
    {
    // Access point
    TInt32 ap = iMmsSettings->AccessPoint( 0 );
    TBuf<KCommsDbSvrMaxFieldLength> apName;

    GetNameForApL( ap, apName );
    SetArrayItem( EMmsSettingsAccessPoint, static_cast<TInt>( ap ), apName );

    TInt fetchHome = iMmsSettings->ReceivingModeHome();
    TInt fetchRoam = iMmsSettings->ReceivingModeForeign();

    // if default value is not acceptable, choose next by keeping costs low
    if ( fetchHome == EMmsReceivingAutomatic &&
        fetchRoam == EMmsReceivingAutomatic )
        {
        SetArrayItem( EMmsSettingsReceivingMode,
                      EMmsSettingsReceivingAutomaticAlways );
        }
    else if ( fetchHome == EMmsReceivingManual &&
            fetchRoam == EMmsReceivingManual )
        {
        if ( ! ( iSettingsFlags & EReceivingAlwaysManualDisabled ) )
            {
            SetArrayItem(   EMmsSettingsReceivingMode,
                            EMmsSettingsReceivingManualAlways );
            }
        else
            {
            if ( ! ( iSettingsFlags & EReceivingAutomaticHomeDisabled ) )
                {    
                SetArrayItem(   EMmsSettingsReceivingMode,
                                EMmsSettingsReceivingAutomaticHome );
                }
            else if ( ! ( iSettingsFlags & EReceivingOffDisabled ) )
                {
                SetArrayItem(   EMmsSettingsReceivingMode,
                                EMmsSettingsReceivingDisabledAlways );
                }
            else
                {
                SetArrayItem( EMmsSettingsReceivingMode,
                              EMmsSettingsReceivingAutomaticAlways );
                }
            }
        }
    else if ( fetchHome == EMmsReceivingReject &&
        fetchRoam == EMmsReceivingReject )
        {
        if ( ! ( iSettingsFlags & EReceivingOffDisabled ) )
            {
            SetArrayItem( EMmsSettingsReceivingMode,
              EMmsSettingsReceivingDisabledAlways );
            }
        else
            {
            if ( ! ( iSettingsFlags & EReceivingAlwaysManualDisabled ) )
                {
                SetArrayItem(   EMmsSettingsReceivingMode,
                                EMmsSettingsReceivingManualAlways );
                }
            else if ( ! ( iSettingsFlags & EReceivingAutomaticHomeDisabled ) )
                {    
                SetArrayItem(   EMmsSettingsReceivingMode,
                                EMmsSettingsReceivingAutomaticHome );
                }
            else
                {
                SetArrayItem( EMmsSettingsReceivingMode,
                              EMmsSettingsReceivingAutomaticAlways );
                }
            }
        }
    else
        {
        // Should always be automatic@home & manaul@roam
        if ( ! ( iSettingsFlags & EReceivingAutomaticHomeDisabled ) )
            {    
            SetArrayItem(   EMmsSettingsReceivingMode,
                            EMmsSettingsReceivingAutomaticHome );
            }
        else
            {
            if ( ! ( iSettingsFlags & EReceivingAlwaysManualDisabled ) )
                {
                SetArrayItem(   EMmsSettingsReceivingMode,
                                EMmsSettingsReceivingManualAlways );
                }
            else if ( ! ( iSettingsFlags & EReceivingOffDisabled ) )
                {
                SetArrayItem(   EMmsSettingsReceivingMode,
                                EMmsSettingsReceivingDisabledAlways );
                }
            else
                {
                SetArrayItem( EMmsSettingsReceivingMode,
                              EMmsSettingsReceivingAutomaticAlways );
                }
            }
        }

    // Receiving anonymous messages
    if ( iMmsSettings->AcceptAnonymousMessages() )    
        {
        SetArrayItem( EMmsSettingsReceivingAnonymous, EMmsSettingsYes );
        }
    else
        {
        SetArrayItem( EMmsSettingsReceivingAnonymous, EMmsSettingsNo );
        }

    
    // Receiving ads
    if ( iMmsSettings->AcceptAdvertisementMessages() )
        {
        SetArrayItem( EMmsSettingsReceivingAds, EMmsSettingsYes );
        }
    else
        {
        SetArrayItem( EMmsSettingsReceivingAds, EMmsSettingsNo );
        }

    // Receive report
    TBool deliveryReport = ( iMmsSettings->DeliveryReportWanted() == EMmsYes );
    if ( deliveryReport && iMmsSettings->ReadReplyReportWanted() == EMmsYes  )
        {
        SetArrayItem( EMmsSettingsReceiveReport, EMmsSettingsReceiveReportBoth );
        }
    else if ( deliveryReport )
        {
        SetArrayItem( EMmsSettingsReceiveReport, EMmsSettingsReceiveReportDelivery );
        }
    else // read report only is not supported 
        {
        SetArrayItem( EMmsSettingsReceiveReport, EMmsSettingsReceiveReportNone );
        }

    // Delivery report sending allowed and read report sending has both their own
    // attribute. Writing the value takes care that both have same value.
    // Read here only delivery report sending allowed attribute.
    if ( iMmsSettings->DeliveryReportSendingAllowed() == EMmsDeliveryReportAllowedYes )
        {
        SetArrayItem( EMmsSettingsDenyDeliveryReport, EMmsSettingsNo );
        }
    else
        {
        SetArrayItem( EMmsSettingsDenyDeliveryReport, EMmsSettingsYes );
        }

    // Validity period
    switch ( iMmsSettings->ExpiryInterval() )
        {
        case KMmsValidityPeriod1h:
            SetArrayItem( EMmsSettingsValidityPeriod, EMmsSettingsValidityPeriod1h );
            break;
        case KMmsValidityPeriod6h:
            SetArrayItem( EMmsSettingsValidityPeriod, EMmsSettingsValidityPeriod6h );
            break;
        case KMmsValidityPeriod24h:
            SetArrayItem( EMmsSettingsValidityPeriod, EMmsSettingsValidityPeriod24h );
            break;
        case KMmsValidityPeriod3Days:
            SetArrayItem( EMmsSettingsValidityPeriod, EMmsSettingsValidityPeriod3d );
            break;
        case KMmsValidityPeriodWeek:
            SetArrayItem( EMmsSettingsValidityPeriod, EMmsSettingsValidityPeriodWeek );
            break;
        case KMmsValidityPeriodMax:
        default: // default to max
            SetArrayItem( EMmsSettingsValidityPeriod, EMmsSettingsValidityPeriodMaximum );
            break;
        }

    // Image size
    TInt32 imageWidth = iMmsSettings->ImageWidth();
    TInt32 imageHeight = iMmsSettings->ImageHeight();

    if ( imageWidth == KMmsUniImageLargeWidth && imageHeight == KMmsUniImageLargeHeight )
        {
        SetArrayItem( EMmsSettingsImageSize, EMmsSettingsImageLarge );
        }
    else
        {
        SetArrayItem( EMmsSettingsImageSize, EMmsSettingsImageSmall );
        }

    SetArrayItem( EMmsSettingsCreationMode, GetCreationMode() );

    }


// ---------------------------------------------------------
// CMmsSettingsDialog::SetArrayItem(TInt aIndex, TInt aCurrentNumber)
// Updates indexed item of listbox
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::SetArrayItem( TInt aIndex, TInt aCurrentNumber )
    {
    iItems->At( aIndex ).iUserText.Copy(
        ( *iItems->At( aIndex ).iMuiuSettingsItemArray )[aCurrentNumber]);
    ( *iItems )[aIndex].iCurrentNumber = aCurrentNumber;
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::SetArrayItem(TInt aIndex, TInt aCurrentNumber, const TDesC& aUserText)
// Updates indexed item of listbox - user text formatted by the caller
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::SetArrayItem(TInt aIndex, TInt aCurrentNumber, const TDesC& aUserText)
    {
    iItems->At( aIndex ).iUserText.Copy( aUserText );
    ( *iItems )[aIndex].iCurrentNumber = aCurrentNumber;
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
// Handles listbox event
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::HandleListBoxEventL( CEikListBox* /* aListBox */, 
                                             TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            HandleEditRequestL( iListbox->CurrentItemIndex(), ETrue );
            break;
        default:
            break;
        }

    }


// ---------------------------------------------------------
// CMmsSettingsDialog::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
// Handles listbox event
//
// ---------------------------------------------------------
//
TKeyResponse CMmsSettingsDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TBool isDestructed( EFalse );
    if (    aType ==EEventKey
        &&  aKeyEvent.iCode == EKeyEscape )
        {
        isDestructed = ETrue;
        }
    
    TKeyResponse ret = CAknDialog::OfferKeyEventL( aKeyEvent, aType );

    if (    !isDestructed
        &&  iSettingsFlags & EShouldExit ) 
        {
        iExitCode = EMmsExit;
        TryExitL( EAknSoftkeyCancel );
        }

    return ret;
    }

// ---------------------------------------------------------
// CMmsSettingsDialog::HandleEditRequestL(TInt aIndex, TBool aEnterPressed)
// Handles editing of a selected setting
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::HandleEditRequestL( TInt aIndex, TBool aEnterPressed )
    {
    switch ( aIndex )
        {
        case EMmsSettingsAccessPoint:
            if ( EditAccessPointL() )
                {
                // Cannot exit directly here - we are in the middle of HandleListBoxEvent
                // Real exit is activated in OfferKeyEventL()
                iSettingsFlags |= EShouldExit;
                iExitCode = EMmsExit;
                }
            break;
        case EMmsSettingsReceivingMode:
            EditReceivingModeL();
            break;
        case EMmsSettingsReceivingAnonymous:
            EditReceivingAnonymousL( aEnterPressed );
            break;
        case EMmsSettingsReceivingAds:
            EditReceivingAdsL( aEnterPressed );
            break;
        case EMmsSettingsReceiveReport:
            EditReceiveReportsL( );
            break;
        case EMmsSettingsDenyDeliveryReport:
            EditDenySendingDeliveryReportsL( aEnterPressed );
            break;
        case EMmsSettingsValidityPeriod:
            EditValidityPeriodL();
            break;
        case EMmsSettingsImageSize:
            EditImageSizeL( aEnterPressed );
            break;
        case EMmsSettingsCreationMode:     
            EditCreationModeL( aEnterPressed );
            break;
        default:
            __ASSERT_DEBUG( EFalse, Panic( EMmsSettingsWrongParameters ) );
            break;
        } // switch
    iListbox->DrawItem( aIndex );
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::SetAndGetSettingItemL(TInt& aCurrentItem, TUint aList, TInt aItem)
// Sets the ui for the setting item and returns true if item selection
// is accepted. Selected value is passed as a parameter.
//
// ---------------------------------------------------------
//
TBool CMmsSettingsDialog::SetAndGetSettingItemL( TInt& aCurrentItem, 
                                                TInt aList, 
                                                TInt aItem)
    {
    TBool isOk = EFalse;
    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC( reader, aList );
    
    CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( KSettingsGranularity );
    CleanupStack::PushL( items );
    
    // Get the labels from resources
    const TInt count = reader.ReadInt16();

    for ( TInt loop = 0; loop < count; loop++ )
        {
        HBufC* label = reader.ReadHBufCL();
        if ( ( aList == R_RECEIVING_MMS_LIST && ( iSettingsFlags & EReceivingAlwaysManualDisabled ) && loop == EMmsSettingsReceivingManualAlways ) ||
            ( aList == R_RECEIVING_MMS_LIST && ( iSettingsFlags & EReceivingOffDisabled ) && loop == EMmsSettingsReceivingDisabledAlways ) ||
            ( aList == R_RECEIVING_MMS_LIST && ( iSettingsFlags & EReceivingAutomaticHomeDisabled ) && loop == EMmsSettingsReceivingAutomaticHome ) ||
            ( aList == R_MMS_IMAGE_SIZE_LIST && iMmsSettings->CreationMode() == EMmsCreationModeRestricted && loop == EMmsSettingsImageLarge ) )
            {
            delete label;
            }
        else
            {
            CleanupStack::PushL( label );
            TPtr pLabel = label->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( pLabel );
            items->AppendL( pLabel );
            CleanupStack::PopAndDestroy( label );
            }
        label = NULL;
        }
    
    if ( aList == R_RECEIVING_MMS_LIST )
        {
        // 3. removed, update 4
        if (    ( iSettingsFlags & EReceivingAlwaysManualDisabled ) 
            &&  aCurrentItem == EMmsSettingsReceivingDisabledAlways )
            {
            aCurrentItem--;
            }
        // 2. removed, update 3 & 4
        if (    ( iSettingsFlags & EReceivingAutomaticHomeDisabled ) 
            &&  aCurrentItem >= EMmsSettingsReceivingAutomaticHome )
            {
            // Adjust the index for every setting that comes after AutomaticAtHome when it is hidden
            aCurrentItem--;
            }
        }

    CAknRadioButtonSettingPage* dlg = new ( ELeave ) CAknRadioButtonSettingPage(
        R_MMS_SETTING_PAGE, 
        aCurrentItem, 
        items );

    dlg->SetSettingTextL( iItems->At( aItem ).iLabelText );
    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) ) 
        {
        // Adjust the index back after the user has made the selection
        if ( aList == R_RECEIVING_MMS_LIST )
            {
            // 2. removed, update 3 & 4
            if (    ( iSettingsFlags & EReceivingAutomaticHomeDisabled ) 
                &&  aCurrentItem >= EMmsSettingsReceivingAutomaticHome )
                {
                // Adjust the index for every setting that comes after AutomaticAtHome when it is hidden
                aCurrentItem++;
                }

            // 3. removed, update 4
            if (    ( iSettingsFlags & EReceivingAlwaysManualDisabled ) 
                &&  aCurrentItem >= EMmsSettingsReceivingManualAlways )
                {
                
                aCurrentItem++;
                }
            }

        isOk = ETrue;
        }
    
    CleanupStack::PopAndDestroy( 2 ); //items & reader 

    return isOk;
    }

// ---------------------------------------------------------
// CMmsSettingsDialog::EditReceivingModeL()
// ---------------------------------------------------------
//
void CMmsSettingsDialog::EditReceivingModeL()
    {
    TInt currentItem = iItems->At( EMmsSettingsReceivingMode ).iCurrentNumber;

    if ( SetAndGetSettingItemL(
            currentItem,
            R_RECEIVING_MMS_LIST,
            EMmsSettingsReceivingMode ) )
        {
        switch ( currentItem )
            {
            case EMmsSettingsReceivingAutomaticHome:
                {
                iMmsSettings->SetReceivingModeHome( EMmsReceivingAutomatic );
                iMmsSettings->SetReceivingModeForeign( EMmsReceivingManual );
                break;
                }
            case EMmsSettingsReceivingAutomaticAlways:
                iMmsSettings->SetReceivingModeHome( EMmsReceivingAutomatic );
                iMmsSettings->SetReceivingModeForeign( EMmsReceivingAutomatic );
                break;
            case EMmsSettingsReceivingManualAlways:
                {
                iMmsSettings->SetReceivingModeHome( EMmsReceivingManual );
                iMmsSettings->SetReceivingModeForeign( EMmsReceivingManual );
                break;
                }
            case EMmsSettingsReceivingDisabledAlways:
                iMmsSettings->SetReceivingModeHome( EMmsReceivingReject );
                iMmsSettings->SetReceivingModeForeign( EMmsReceivingReject );
                break;
            default:
                break;
            }
        //to be sure that we have the selected item  
        iItems->At( EMmsSettingsReceivingMode ).iCurrentNumber = currentItem; 
        
        SetArrayItem( EMmsSettingsReceivingMode, currentItem );
        }
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::EditReceivingAnonymousL
// Edits the value for receiving anonymous messages.
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::EditReceivingAnonymousL( TBool aEnterPressed )
    {
    TBool isOk = EFalse;
    TInt currentItem = iItems->At( EMmsSettingsReceivingAnonymous ).iCurrentNumber;
    if ( aEnterPressed )
        {
        currentItem ^= 1;
        }
    else
        {
        isOk = SetAndGetSettingItemL( 
            currentItem, 
            R_MMS_YES_NO_LIST, 
            EMmsSettingsReceivingAnonymous );
        }
        
    if ( aEnterPressed || isOk )
        {
        switch ( currentItem )
            {
            case EMmsSettingsNo:
                iMmsSettings->SetAcceptAnonymousMessages( EFalse );
                break;
            case EMmsSettingsYes: // flow through
            default:
                iMmsSettings->SetAcceptAnonymousMessages( ETrue );
                break;
            }
        SetArrayItem( EMmsSettingsReceivingAnonymous, currentItem );
        CEikFormattedCellListBox* settingListBox =
            static_cast<CEikFormattedCellListBox*>( Control(EDlgSettingListBox ) );
        settingListBox->DrawNow();
        }                   
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::EditReceivingAdsL
// Edits the value for receiving advertisements
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::EditReceivingAdsL( TBool aEnterPressed )
    {
    TBool isOk = EFalse;
    TInt currentItem = iItems->At( EMmsSettingsReceivingAds ).iCurrentNumber;
    if ( aEnterPressed )
        {
        currentItem ^= 1;
        }
    else
        {
        isOk = SetAndGetSettingItemL( 
            currentItem, 
            R_MMS_YES_NO_LIST, 
            EMmsSettingsReceivingAds );
        }
        
    if (aEnterPressed || isOk)
        {
        switch (currentItem)
            {
            case EMmsSettingsNo:
                iMmsSettings->SetAcceptAdvertisementMessages( EFalse );
                break;
            case EMmsSettingsYes: // flow through
            default:
                iMmsSettings->SetAcceptAdvertisementMessages( ETrue );
                break;
            }
        SetArrayItem(EMmsSettingsReceivingAds, currentItem);
        CEikFormattedCellListBox* settingListBox =
            static_cast<CEikFormattedCellListBox*>( Control( EDlgSettingListBox ) );
        settingListBox->DrawNow();
        }                   
    }
// ---------------------------------------------------------
// CMmsSettingsDialog::EditReceiveReportsL
// Edits the value for receiving reports.
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::EditReceiveReportsL( )
    {       
    TInt currentItem = iItems->At( EMmsSettingsReceiveReport ).iCurrentNumber;

    if ( SetAndGetSettingItemL(
            currentItem, 
            R_MCE_MMS_REPORTS, 
            EMmsSettingsReceiveReport ) )
        {
        SetReceiveReportSetting( static_cast<TMmsSettingsReceiveReportValues>( currentItem ) );
        }
    SetArrayItem( EMmsSettingsReceiveReport, currentItem );
    CEikFormattedCellListBox* settingListBox =
        static_cast<CEikFormattedCellListBox*>( Control( EDlgSettingListBox ) );
    settingListBox->DrawNow();
    }

// ---------------------------------------------------------
// SetReceiveReportSetting( TMmsSettingsReceiveReportValues aReceiveReport )
// Stores the value for receive report settings.
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::SetReceiveReportSetting( TMmsSettingsReceiveReportValues aReceiveReport )
    {
    switch ( aReceiveReport )
        {
        case EMmsSettingsReceiveReportBoth:
            iMmsSettings->SetDeliveryReportWanted( EMmsYes );
            iMmsSettings->SetReadReplyReportWanted( EMmsYes );
            break;
        case EMmsSettingsReceiveReportDelivery:
            iMmsSettings->SetDeliveryReportWanted( EMmsYes );
            iMmsSettings->SetReadReplyReportWanted( EMmsNo );
            break;
        case EMmsSettingsReceiveReportNone:
            iMmsSettings->SetDeliveryReportWanted( EMmsNo );
            iMmsSettings->SetReadReplyReportWanted( EMmsNo );
            break;
        default:
            __ASSERT_DEBUG_NO_LEAVE( Panic( EMmsSettingsWrongParameters ) );
            iMmsSettings->SetDeliveryReportWanted( EMmsNo );
            iMmsSettings->SetReadReplyReportWanted( EMmsNo );
            break;
        }
    }
// ---------------------------------------------------------
// CMmsSettingsDialog::EditDenySendingDeliveryReportL
// Edits the value for sending delivery reports.
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::EditDenySendingDeliveryReportsL( TBool aEnterPressed )
    {
    TBool isOk = EFalse;
    TInt currentItem = iItems->At( EMmsSettingsDenyDeliveryReport ).iCurrentNumber;
    
    if ( aEnterPressed )
        {
        currentItem ^= 1;
        }
    else
        {
        isOk = SetAndGetSettingItemL(
            currentItem, 
            R_MMS_YES_NO_LIST, 
            EMmsSettingsDenyDeliveryReport );
        }
        
    if ( aEnterPressed || isOk )
        {
        switch ( currentItem )
            {
            case EMmsSettingsNo:
                iMmsSettings->SetDeliveryReportSendingAllowed( EMmsDeliveryReportAllowedYes );
                iMmsSettings->SetReadReplyReportSendingAllowed( ETrue );
                break;
            case EMmsSettingsYes: // flow through
            default:
                iMmsSettings->SetDeliveryReportSendingAllowed( EMmsDeliveryReportAllowedNo );
                iMmsSettings->SetReadReplyReportSendingAllowed( EFalse );
                break;
            }
        SetArrayItem( EMmsSettingsDenyDeliveryReport, currentItem );
        CEikFormattedCellListBox* settingListBox =
            static_cast<CEikFormattedCellListBox*>( Control( EDlgSettingListBox ) );
        settingListBox->DrawNow();
        }                   
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::EditValidityPeriodL
// Edits the value for validity period
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::EditValidityPeriodL()
    {
    TInt currentItem = iItems->At( EMmsSettingsValidityPeriod ).iCurrentNumber;
    
    if (SetAndGetSettingItemL( currentItem, R_MMS_VALIDITY_PERIOD_LIST, EMmsSettingsValidityPeriod ))
        {
        switch (currentItem)
            {
            case EMmsSettingsValidityPeriod1h:
                SetValidityPeriodSetting( KMmsValidityPeriod1h );
                break;
            case EMmsSettingsValidityPeriod6h:
                SetValidityPeriodSetting( KMmsValidityPeriod6h );
                break;
            case EMmsSettingsValidityPeriod24h:
                SetValidityPeriodSetting( KMmsValidityPeriod24h );
                break;
            case EMmsSettingsValidityPeriod3d:
                SetValidityPeriodSetting( KMmsValidityPeriod3Days );
                break;
            case EMmsSettingsValidityPeriodWeek:
                SetValidityPeriodSetting( KMmsValidityPeriodWeek );
                break;
            case EMmsSettingsValidityPeriodMaximum:
            default: // default to max
                SetValidityPeriodSetting( KMmsValidityPeriodMax );
                break;
            }
            SetArrayItem( EMmsSettingsValidityPeriod, currentItem );
        }                   
    }       


// ---------------------------------------------------------
// CMmsSettingsDialog::SetValidityPeriodSetting(TInt32 aValidityPeriod)
// Stores the value for validity period settings.
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::SetValidityPeriodSetting( TInt32 aValidityPeriod )
    {
    iMmsSettings->SetExpiryInterval( aValidityPeriod );
    }

// ---------------------------------------------------------
// CMmsSettingsDialog::EditAccessPointL
//
// ---------------------------------------------------------
//
TBool CMmsSettingsDialog::EditAccessPointL()
    {
    TUint32 currentItem = iItems->At( EMmsSettingsAccessPoint ).iCurrentNumber;
    

    TInt bearers = EApBearerTypeGPRS; 
    if ( iSettingsFlags & ECSDEnabled )
        {
        bearers |= ( EApBearerTypeCSD | EApBearerTypeHSCSD );
        }

    TUint32 selectedAp = KMaxTUint32;   // invalid value
    TInt taskDone = MmsApSelect::SelectMmsAccessPointL(
        *iCommsDb,
        currentItem,
        selectedAp,
        bearers );

    // Note: taskDone == KApUiEventDeleted is not handled here.
    // Basically the user can delete the AP that was selected. This is noticed
    // & complained when closing Settings dialog.
    CApUtils* apUtils = CApUtils::NewLC( *iCommsDb );
    TBuf<KCommsDbSvrMaxFieldLength> apName;

    if( !ApExistsL( apUtils, currentItem ) && taskDone == 0 ) //if ap has been deleted, set "must be defined"
        {
        selectedAp = 0;
        }//if
    else if( selectedAp == KMaxTUint32 )
        {
        selectedAp = currentItem;
        }
    GetNameForApL( selectedAp, apName );
    SetArrayItem( EMmsSettingsAccessPoint, selectedAp, apName );

    // Use cases numbered:
    if ( taskDone || selectedAp != 0 )
        {  
        // 1) sp selected 
        // 2) previously ap selected but 'back' now
        // 3) none selected 
        while ( iMmsSettings->AccessPointCount() )
            {
            iMmsSettings->DeleteAccessPointL( 0 );
            }
            
        if ( selectedAp != 0 )
            {
            // 1) sp selected, 2) previously ap selected but 'back' now
            iMmsSettings->AddAccessPointL( selectedAp, 0 );
            }
        }
    // else - 4) previously none selected and 'back' now    

    CleanupStack::PopAndDestroy(); // apUtils

    return ( taskDone & ( KApUiEventExitRequested | KApUiEventShutDownRequested ) );
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::GetNameForApL
// Get a name for the given access point id
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::GetNameForApL( TUint32 aId, TDes& aName )
    {
    aName.Zero();
    CApUtils* apUtils = CApUtils::NewLC( *iCommsDb );
    if ( !ApExistsL( apUtils, aId ) )
        {
        // No access point -> "Compulsory"
        HBufC* text = iEikonEnv->AllocReadResourceLC( R_MMS_SETTINGS_COMPULSORY );
        aName.Copy( *text );
        CleanupStack::PopAndDestroy(); // text
        }
    else
        {
        apUtils->NameL( aId, aName );
        }
    CleanupStack::PopAndDestroy(); // apUtils
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::EditImageSizeL
// Edits the value for image size setting.
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::EditImageSizeL( TBool aEnterPressed )
    {
    TBool isOk = EFalse;
    TInt currentItem = iItems->At( EMmsSettingsImageSize ).iCurrentNumber;

    if ( aEnterPressed &&
        iMmsSettings->CreationMode() != EMmsCreationModeRestricted )
        {
        currentItem ^= 1;
        isOk = ETrue;
        }
    else
        {
        // We come here either from "Options -> Change" or with selection key
        // in "restricted" mode (opens a list with a single item).
        isOk = SetAndGetSettingItemL(
            currentItem,
            R_MMS_IMAGE_SIZE_LIST,
            EMmsSettingsImageSize );
        }

    if ( isOk )
        {
        switch ( currentItem )
            {
            case EMmsSettingsImageLarge:
                iMmsSettings->SetImageWidth( KMmsUniImageLargeWidth );
                iMmsSettings->SetImageHeight( KMmsUniImageLargeHeight );
                break;
            case EMmsSettingsImageSmall: // flow through
            default:
                iMmsSettings->SetImageWidth( KMmsUniImageSmallWidth );
                iMmsSettings->SetImageHeight( KMmsUniImageSmallHeight );
                break;
            }
        SetArrayItem( EMmsSettingsImageSize, currentItem );
        CEikFormattedCellListBox* settingListBox =
            static_cast<CEikFormattedCellListBox*>( Control( EDlgSettingListBox ) );
        settingListBox->DrawNow();
        }                   
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::EditCreationModeL
// Edits the value for creation mode setting.
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::EditCreationModeL( TBool /*aEnterPressed*/ )
    {
    if ( !( iSettingsFlags & ECreationModeChangeEnabled ) )
        {
        ShowInformationNoteL( R_MMSUI_INFO_CANNOT_CHANGE_CMODE );
        return;
        }

    TInt enumMode = EMmsSettingsCreationMode;

    TInt currentItem = iItems->At( enumMode ).iCurrentNumber;
    
    if ( SetAndGetSettingItemL( currentItem, R_MMS_CREATION_MODE_LIST, enumMode) )
        {
        TInt32 oldMode = GetCreationMode();
        switch ( currentItem )
            {
            case EMmsSettingsCreationModeRestricted:
                iMmsSettings->SetCreationMode( EMmsCreationModeRestricted );
                if ( oldMode != currentItem &&
                    ( iMmsSettings->ImageWidth() != KMmsUniImageSmallWidth ||
                      iMmsSettings->ImageHeight() != KMmsUniImageSmallHeight ) )
                    {
                    iMmsSettings->SetImageWidth( KMmsUniImageSmallWidth );
                    iMmsSettings->SetImageHeight( KMmsUniImageSmallHeight );
                    SetArrayItem( EMmsSettingsImageSize, EMmsSettingsImageSmall );
                    ShowInformationNoteL( R_MMSUI_INFO_IMAGE_SIZE_CHANGED );
                    iListbox->DrawItem( EMmsSettingsImageSize );
                    }
                break;
            case EMmsSettingsCreationModeFree:
                {
                if ( oldMode != currentItem )
                    {
                    CAknQueryDialog* confirmQuery = CAknQueryDialog::NewL();
                    if ( confirmQuery->ExecuteLD( R_MMSUI_QUERY_CHANGE_TO_FREE ) )
                        {
                        iMmsSettings->SetCreationMode( EMmsCreationModeFree );
                        }
                    else
                        {
                        currentItem = oldMode;
                        }
                    }
                }
                break;
            default:
            case EMmsSettingsCreationModeGuided:
                iMmsSettings->SetCreationMode( EMmsCreationModeWarning );
                break;
            }
            SetArrayItem( enumMode, currentItem );
        }                   

    }

// ---------------------------------------------------------
// CMmsSettingsDialog::GetCreationMode
// Maps creation mode value from engine to SettingsDialog values
//
// ---------------------------------------------------------
//
TInt32 CMmsSettingsDialog::GetCreationMode()
    {
    switch ( iMmsSettings->CreationMode() )
        {
        case EMmsCreationModeRestricted:
            return EMmsSettingsCreationModeRestricted;
        case EMmsCreationModeFree:
            return EMmsSettingsCreationModeFree;
        case EMmsCreationModeWarning:
        default:
            return EMmsSettingsCreationModeGuided;
        }
    }

// ---------------------------------------------------------
// CMmsSettingsDialog::SetTitleTextL()
// Makes the status pane
//
// ---------------------------------------------------------
//
void CMmsSettingsDialog::SetTitleTextL()
    {   
    CEikStatusPane *sp = ( static_cast<CAknAppUi*>
        ( CEikonEnv::Static()->EikAppUi() ) )->StatusPane();
    iTitlePane = static_cast<CAknTitlePane*>
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    
    //save old title
    iPreviousTitleText = ( *iTitlePane->Text() ).Alloc();
        
    // set new title
    HBufC* text = text = StringLoader::LoadLC( R_MMS_SETTINGS_TITLE_TEXT );
    iTitlePane->SetTextL( *text );
    CleanupStack::PopAndDestroy(); // text
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::LaunchHelpL
// launch help using context
// 
// ---------------------------------------------------------
//
void CMmsSettingsDialog::LaunchHelpL()
    {
    if ( iSettingsFlags & EHelpFeatureSupported )
        {
        CCoeAppUi* editorAppUi = static_cast<CCoeAppUi*>( ControlEnv()->AppUi() );
      
        CArrayFix<TCoeHelpContext>* helpContext = editorAppUi->AppHelpContextL();   
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
        }
    }


// ---------------------------------------------------------
// CMmsSettingsDialog::GetHelpContext
// returns helpcontext as aContext
// 
// ---------------------------------------------------------
//
void CMmsSettingsDialog::GetHelpContext
        ( TCoeHelpContext& aContext ) const
    {
    if ( iSettingsFlags & EHelpFeatureSupported )
        {
        aContext.iMajor = KUidMce;
        aContext.iContext = KMCE_HLP_SETTINGS_MMS;
        }
    }

// ---------------------------------------------------------
// CMmsSettingsDialog::ShowInformationNoteL
// ---------------------------------------------------------
//
void CMmsSettingsDialog::ShowInformationNoteL( TInt aResourceId )
    {
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    HBufC* string = StringLoader::LoadLC( aResourceId );
    note->ExecuteLD( *string );
    CleanupStack::PopAndDestroy(); //string
    }

//  End of File
