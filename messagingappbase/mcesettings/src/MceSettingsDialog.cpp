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
*     Message centre's settings dialog.
*
*/



// INCLUDE FILES

#include <eikapp.h>
#include <MuiuMsvUiServiceUtilities.h> //MsvUiServiceUtilities
#include <MtmExtendedCapabilities.hrh>
#include <msvuids.h>
#include <mtudreg.h>
#include <MTMStore.h>
#include <aknnotewrappers.h>
#include <aknradiobuttonsettingpage.h>
#include <aknViewAppUi.h>

#include <apgcli.h>
#include <MceSettings.rsg>
#include "MceSettingsDialog.h"
#include "MceSettingsGeneralSettingsDialog.h"
#include "MceSettingsArrays.h"
#include "MceSettingsSessionObserver.h"
#include "MceSettingsIds.hrh"
#include "MceSettingsMultipleServicesDialog.h"

#include <featmgr.h>
#include <hlplch.h>             // For HlpLauncher
#include <csxhelp/mce.hlp.hrh>
#include <SenduiMtmUids.h>

#include <bldvariant.hrh>
#include <data_caging_path_literals.hrh>

#include "MceSettingsUtils.h"
#include <RCustomerServiceProfileCache.h>
// CONSTANTS

_LIT( KMceDirAndFile,"MceSettings.rsc" );

#define KMceApplicationUidValue     0x100058C5
const TUid KMceApplicationUid       = {KMceApplicationUidValue};

const TUid KCBSApplicationId        = {0x101F4CD3};
const TUid KCBSApplicationSettingsViewId    = {6};
const TInt KMceSettingsExtraItems           = 2;
const TInt KMceSettingsCBS                  = 2;
const TInt KMceSettingsCBSUid               = 42;



// ================= MEMBER FUNCTIONS =======================


// C++ default constructor can NOT contain any code that
// might leave.
//
CMceSettingsDialog::CMceSettingsDialog()
:   CMceSettingsTitlePaneHandlerDialog(),  
    iResources( *CCoeEnv::Static() ),    
    iLaunchedFromMce( EFalse ),
    iDialogOpen(EFalse)
    {
    }

// ----------------------------------------------------
// CMceSettingsDialog::Constructor
// ----------------------------------------------------
CMceSettingsDialog::CMceSettingsDialog(
    CMceUidNameArray& aSettingsItemsArray,
    MMceSettingsAccountManager* aManager, 
    CMsvSession* aSession )
:   CMceSettingsTitlePaneHandlerDialog(),  
    iResources( *CCoeEnv::Static() ),  
    iMsgTypesSettings( &aSettingsItemsArray ),
    iAccountManager( aManager ),
    iSession( aSession ),    
    iLaunchedFromMce( ETrue ),    
    iDialogOpen(EFalse)
    {
    }


// ----------------------------------------------------
// CMceSettingsDialog::Destructor
// ----------------------------------------------------
EXPORT_C CMceSettingsDialog::~CMceSettingsDialog()
    {
    iResources.Close();
    if ( !iLaunchedFromMce )
        {
        // if launched from mce then mce owns this array!
        delete iMsgTypesSettings;
        }
    delete iSessionObserver;
    // dont delete iSession, it is not owed by this class
    // dont delete iAccountManager, it is not owed by this class 
    }

// ----------------------------------------------------
// CMceSettingsDialog::ConstructL
// ----------------------------------------------------
void CMceSettingsDialog::ConstructL()
    {
    TParse fp;
    fp.Set( KMceDirAndFile, &KDC_RESOURCE_FILES_DIR , NULL );
    TFileName fileName = fp.FullName();

    iResources.OpenL( fileName );
    CAknDialog::ConstructL( R_MCE_SETTINGS_DIALOG_MENUBAR );

    iSessionObserver = CMceSettingsSessionObserver::NewL( iSession );
    if ( !iAccountManager )
        {
        iAccountManager = iSessionObserver;
        }
    if ( !iSession )
        {
        iSession = &iSessionObserver->Session();
        }
    if ( !iMsgTypesSettings )
        {
        iMsgTypesSettings = CMceUidNameArray::NewL();
        }
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdSelectableEmail ) )
        {
		iProductIncludesSelectableEmail = ETrue;
       	}
    else
       	{
		iProductIncludesSelectableEmail = EFalse;	
       	}
	FeatureManager::UnInitializeLib();
    }

// ----------------------------------------------------
// CMceSettingsDialog::ConstructL
// ----------------------------------------------------
EXPORT_C CMceSettingsDialog* CMceSettingsDialog::NewL()
    {
    CMceSettingsDialog* dialog = new (ELeave) CMceSettingsDialog();
    CleanupStack::PushL( dialog );
    dialog->ConstructL();
    CleanupStack::Pop( dialog );
    return dialog;
    }

// ----------------------------------------------------
// CMceSettingsDialog::ConstructL
// ----------------------------------------------------
EXPORT_C CMceSettingsDialog* 
    CMceSettingsDialog::NewL( CMceUidNameArray& aSettingsItemsArray,
    MMceSettingsAccountManager* aManager, CMsvSession* aSession ) 
    {
    CMceSettingsDialog* dialog = new (ELeave) CMceSettingsDialog( 
                                                    aSettingsItemsArray, aManager, aSession );
    CleanupStack::PushL( dialog );
    dialog->ConstructL();
    CleanupStack::Pop( dialog );
    return dialog;
    }

// ----------------------------------------------------
// CMceSettingsDialog::OkToExitL
// ----------------------------------------------------
EXPORT_C TInt CMceSettingsDialog::OkToExitL( TInt aButtonId )
    {
    TBool okToExit = CAknDialog::OkToExitL( aButtonId );
    if ( aButtonId == EMceSettingsCmdSettingsDialogOpen )
        {
        // Middle soft key pressed
        ProcessCommandL( aButtonId );
        okToExit = EFalse; // Cannot exit, since MSK was pressed
        }
    else if ( aButtonId != EEikBidCancel && okToExit )
        {
        RestoreTitleTextL();
        }
    return okToExit;
    }

// ----------------------------------------------------
// CMceSettingsDialog::ProcessCommandL
// ----------------------------------------------------
EXPORT_C void CMceSettingsDialog::ProcessCommandL( TInt aCommandId )
    {
    CAknDialog::ProcessCommandL( aCommandId ); // hides menu
    if ( aCommandId == EMceSettingsCmdSettingsDialogExit )
        {
        if (iAvkonViewAppUi->ExitHidesInBackground()) 
            { 
            iAvkonViewAppUi->ProcessCommandL( EAknCmdHideInBackground );
            }
        else
            {
            iAvkonViewAppUi->ProcessCommandL( EAknCmdExit );
            }
        
        return;
        }


    switch ( aCommandId )
        {
        case EMceSettingsCmdSettingsDialogOpen:
            OpenSubSettingsL();
            break;
        
        case EMceSettingsCmdSettingsDialogHelp:
            {
            LaunchHelpL();                
            }
            break;
           
        default :
            break;
        }
    }

// ----------------------------------------------------
// CMceSettingsDialog::DynInitMenuPaneL
// ----------------------------------------------------
EXPORT_C void CMceSettingsDialog::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_MCE_SETTINGS_DIALOG_MENUPANE )
        {
        aMenuPane->SetItemDimmed( EMceSettingsCmdSettingsDialogHelp,
            !FeatureManager::FeatureSupported( KFeatureIdHelp ) );

        }
    }

// ----------------------------------------------------
// CMceSettingsDialog::PreLayoutDynInitL
// ----------------------------------------------------
EXPORT_C void CMceSettingsDialog::PreLayoutDynInitL( )
    {
    TBool csp = MceSettingsUtils::CspBitsL();    
    iCbs = EFalse;
    if ( FeatureManager::FeatureSupported( KFeatureIdCellBroadcast ) )
        {
        iCbs = ETrue;
        if ( csp )
            {
            iCbs = CheckCspBitL();
            }
        }
    StoreTitleTextAndSetNewL( R_MCE_SETTINGS_TITLE );

    if ( !iLaunchedFromMce )
        {
        LoadAllMtmsL();
        }
    
    CEikTextListBox* list=STATIC_CAST( CEikTextListBox*, 
        Control( EMceSettingsDialogChoiceList ) );
    // Scrollbar
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    CTextListBoxModel* model=list->Model();
    model->SetItemTextArray( iMsgTypesSettings );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );
    list->SetListBoxObserver( this );
    }


// ----------------------------------------------------
// CMceSettingsDialog::ExecuteLD
// ----------------------------------------------------
EXPORT_C TInt CMceSettingsDialog::ExecuteLD( )
    {

    TInt returnValue = CAknDialog::ExecuteLD( R_MCE_SETTINGS_DIALOG );
    
    
    return returnValue;
    }

// ----------------------------------------------------
// CMceSettingsDialog::LoadAllMtmsL
// ----------------------------------------------------
void CMceSettingsDialog::LoadAllMtmsL()
    {
    iMsgTypesSettings->Reset();
    CMtmUiDataRegistry* uiRegistry = iSessionObserver->UiRegistry(); // owed by iSessionObserver
    CMtmStore* mtmStore = iSessionObserver->MtmStore(); // owed by iSessionObserver
    const TInt count = uiRegistry->NumRegisteredMtmDlls();
    TInt loop = 0;
    for ( loop = 0; loop < count; loop++ )
        {
        LoadMtmL( uiRegistry->MtmTypeUid( loop ), *mtmStore, *uiRegistry );
        }

    CMsvEntry* rootEntry = iSession->GetEntryL( KMsvRootIndexEntryIdValue );
    CleanupStack::PushL( rootEntry );  

    // Then add MTMs which have service but cannot create messages (wap push in practice)
    CMsvEntrySelection* sel =
        MsvUiServiceUtilities::GetListOfAccountsL( *iSession );
    CleanupStack::PushL( sel );
    TMsvId id;
    const TInt numAccounts=sel->Count();
    for ( loop = 0; loop<numAccounts; loop++)
        {
        id = sel->At(loop);
        TMsvEntry tentry;
        TMsvId serviceId;	
        TInt err = iSession->GetEntry( id, serviceId, tentry );
     
        if ( err == KErrNone  && uiRegistry->IsPresent( tentry.iMtm ) )
            {
            TUid techType = uiRegistry->TechnologyTypeUid( tentry.iMtm );
            TBool found = EFalse;

            for ( TInt loop2 = iMsgTypesSettings->Count()-1 ;loop2 >= 0; loop2-- )
                {
                TUid uidAlreadyAdded = (*iMsgTypesSettings)[loop2].iUid;
                if ( uiRegistry->TechnologyTypeUid( uidAlreadyAdded ) == techType )
                    {
                    found = ETrue;
                    break;
                    }
                }

            if ( !found )
                {
                iMsgTypesSettings->AppendL( TUidNameInfo(
                    tentry.iMtm,
                    uiRegistry->RegisteredMtmDllInfo( tentry.iMtm ).HumanReadableName()
                    ));
                }
            }
        }
    CleanupStack::PopAndDestroy( sel );    
    CleanupStack::PopAndDestroy( rootEntry ); 

    // this is not used anywhere, two last items in the list are
    // ALWAYS cbs settings and mce general settings

    SortAndChangeSmsMmsEmailFirstL();

    TUid uid;
    uid.iUid = KMceSettingsCBSUid;

    HBufC* text = StringLoader::LoadL( R_MCE_CBS_SETTINGS_TEXT, iEikonEnv );
    TUidNameInfo cbsSettings( uid, *text );
    delete text;
    iMsgTypesSettings->AppendL( cbsSettings );

    text = StringLoader::LoadL( R_MCE_GENERAL_SETTINGS_TEXT, iEikonEnv );
    TUidNameInfo generalSettings( uid, *text );
    delete text;
    iMsgTypesSettings->AppendL( generalSettings );

    }


// ----------------------------------------------------
// CMceSettingsDialog::LoadMtmL
// ----------------------------------------------------
void CMceSettingsDialog::LoadMtmL( 
            const TUid aMtmType, 
            CMtmStore& aMtmStore, 
            const CMtmUiDataRegistry& aUiRegistry )
    {
    CBaseMtmUiData* uiData = NULL;
    TRAPD( err, ( uiData = &(aMtmStore.MtmUiDataL(aMtmType)) ) );
    if( err != KErrNone )
        {
        return;
        }

    TInt response;
    TUid canSend={KUidMtmQueryCanCreateNewMsgValue};
    TInt canSendResponse=uiData->QueryCapability( canSend, response );

    TBool foundSettings = EFalse;
    const CArrayFix<CBaseMtmUiData::TMtmUiFunction>& funcs =
        uiData->MtmSpecificFunctions();
    const TInt numFuncs = funcs.Count();
    for (TInt funcIndex = 0; funcIndex < numFuncs && !foundSettings; funcIndex++ )
        {
        CBaseMtmUiData::TMtmUiFunction func = funcs.At( funcIndex );
        if ( func.iFunctionId == KMtmUiMceSettings )
            {
            iMsgTypesSettings->AppendL( TUidNameInfo(
                aMtmType,
                func.iCaption ) );
            foundSettings = ETrue;
            }
        } // end for
    if ( canSendResponse == KErrNone && !foundSettings )
        {
        iMsgTypesSettings->AppendL( TUidNameInfo(
            aMtmType,
            aUiRegistry.RegisteredMtmDllInfo(aMtmType).HumanReadableName()
            ));
        }
    }


// ----------------------------------------------------
// CMceSettingsDialog::HandleListBoxEventL
// ----------------------------------------------------
void CMceSettingsDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    switch(aEventType)
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            ProcessCommandL( EMceSettingsCmdSettingsDialogOpen );
        break;
        default:
        break;
        }
    }


// ----------------------------------------------------
// CMceSettingsDialog::OpenSubSettingsL
// ----------------------------------------------------
void CMceSettingsDialog::OpenSubSettingsL()
    {    
    CEikTextListBox* list=STATIC_CAST( CEikTextListBox*, 
        Control( EMceSettingsDialogChoiceList ) );
    const TInt numberOfItems = list->Model()->NumberOfItems();
    TInt choice = list->CurrentItemIndex();

    // Run time Cell Broadcast variant.
    TInt removedSettingItems = 0;
    if ( !iCbs )
    {
        removedSettingItems++;
    }

    if ( choice < numberOfItems - KMceSettingsExtraItems + removedSettingItems )
        {
        TBool  canCreateNewAccount = iAccountManager->CanCreateNewAccountL(
            (*iMsgTypesSettings)[choice].iUid );

        if ( canCreateNewAccount )
            {
            if(iDialogOpen)
             	{
             	return;
            	}
            iDialogOpen = ETrue;	
           	if ( iProductIncludesSelectableEmail )
				{
            	CMceSettingsMultipleServicesDialog::LaunchDialogL( 
                		           *iAccountManager,  
                		           ( *iMsgTypesSettings )[choice].iUid, 
                		           *iSession,
                		           R_MCE_SETTINGS_MULTIPLE_SERVICES_DIALOG_MENUBAR_WITH_SELECTABLE_EMAIL,
                		           R_MCE_SETTINGS_MULTIPLE_SERVICES_DIALOG );          
				}
			else
				{
            	CMceSettingsMultipleServicesDialog::LaunchDialogL( 
                		     *iAccountManager,  
                		     ( *iMsgTypesSettings )[choice].iUid, 
                		     *iSession,
                		     R_MCE_SETTINGS_MULTIPLE_SERVICES_DIALOG_MENUBAR,
                		     R_MCE_SETTINGS_MULTIPLE_SERVICES_DIALOG );          
				}
            iDialogOpen = EFalse;
            }
        else
            {
            // mtm cannot create new accounts, edit only FIRST service if exists
            CMsvEntrySelection* sel =
                MsvUiServiceUtilities::GetListOfAccountsWithMTML( *iSession, 
                                                            (*iMsgTypesSettings)[choice].iUid );

            CleanupStack::PushL( sel );

            if ( sel->Count() > 0 )
                {
                iAccountManager->EditAccountL( (*sel)[0] );
                }
            else
                { // service disappeared?

                }
            CleanupStack::PopAndDestroy( sel );
            }
        } // end if ( choice < numberOfItems )
    else if ( iCbs && choice == numberOfItems - KMceSettingsCBS )
        {
        // CBS settings are second from the end of the list
        TVwsViewId viewId( KCBSApplicationId, KCBSApplicationSettingsViewId );
        STATIC_CAST( CCoeAppUi*, ControlEnv()->AppUi())->ActivateViewL( viewId );
        }
    else
        {
        // general settings are list item in the list        
        CMceGeneralSettingsDialog* generalSettingsDlg =
            new (ELeave) CMceGeneralSettingsDialog( iSession, *iSessionObserver, iAccountManager );
        CleanupStack::PushL( generalSettingsDlg );
        generalSettingsDlg->ConstructL( R_MCE_GENERAL_SETTINGS_CHOICE_MENUBAR );
        CleanupStack::Pop( generalSettingsDlg ); // generalSettingsDlg
        generalSettingsDlg->ExecuteLD( R_MCE_GENERAL_SETTINGS );
        }

    }


// ---------------------------------------------------------
// CMceSettingsDialog::SortAndChangeSmsMmsEmailFirstL
// ---------------------------------------------------------
//
void CMceSettingsDialog::SortAndChangeSmsMmsEmailFirstL()
    {
    iMsgTypesSettings->Sort(ECmpFolded);    

    ChangeMsgTypeTopL( *iMsgTypesSettings, KSenduiMtmPostcardUid );
    ChangeMsgTypeTopL( *iMsgTypesSettings, KSenduiMtmSyncMLEmailUid );
    ChangeMsgTypeTopL( *iMsgTypesSettings, KSenduiMtmSmtpUid );
    ChangeMsgTypeTopL( *iMsgTypesSettings, KSenduiMtmAudioMessageUid );
    // SMS & MMS still have own settings although the editor is common.
    ChangeMsgTypeTopL( *iMsgTypesSettings, KSenduiMtmMmsUid );
    ChangeMsgTypeTopL( *iMsgTypesSettings, KSenduiMtmSmsUid );
    }

// ---------------------------------------------------------
// CMceSettingsDialog::ChangeMsgTypeTopL
// ---------------------------------------------------------
//
void CMceSettingsDialog::ChangeMsgTypeTopL( CUidNameArray& aArray, TUid aMsgType ) const
    {
    TInt loop = 0;
    // first change email to the top
    for (loop=1; loop < aArray.Count(); loop++)
        {
        if (aArray[loop].iUid == aMsgType)
            {
            TUidNameInfo info = aArray[loop];
            aArray.InsertL( 0, info );
            aArray.Delete( loop+1 );
            break;
            }
        }
    }

// ----------------------------------------------------
// CMceSettingsDialog::LaunchHelpL
// Launch help using context
// 
// ----------------------------------------------------
void CMceSettingsDialog::LaunchHelpL() const
    { 
    CCoeAppUi* appUi = STATIC_CAST(CCoeAppUi*, ControlEnv()->AppUi());       
    CArrayFix<TCoeHelpContext>* helpContext = appUi->AppHelpContextL();   
    HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
    }


// ----------------------------------------------------
// CMceSettingsDialog::GetHelpContext
// returns helpcontext as aContext
//
// ----------------------------------------------------
void CMceSettingsDialog::GetHelpContext
        (TCoeHelpContext& aContext) const
    {
    aContext.iMajor = KMceApplicationUid;
    aContext.iContext = KMCE_HLP_SETTINGS;
    }

// ----------------------------------------------------
// CMceSettingsDialog::CheckCspBitL
// returns ETrue if Cell Broadcast csp bit is on
//
// ----------------------------------------------------
TBool CMceSettingsDialog::CheckCspBitL( ) const
    {
    RCustomerServiceProfileCache csp;

    TInt error = csp.Open();
    if ( error )
        {        
        return ETrue;  
        }
    
    RMobilePhone::TCspTeleservices params;
    TInt retVal = csp.CspTeleServices( params );
    csp.Close();

    if ( retVal == KErrNone )
        {
        if ( ( params&RMobilePhone::KCspSMCB ) != 0 )
            {
            // Cell Broadcast CSP bit is on
            return ETrue;
            }
        else
            {
            // Cell Broadcast CSP bit is off
            return EFalse;
            }
        }
    else
        {
        // Error: By default show CBS service
        return ETrue;
        }

    }

//  End of File
