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
* Description:   Messaging Settings sub-menu in General Settings.
*
*/



// INCLUDE FILES



#include <MuiuMsvUiServiceUtilities.h> //MsvUiServiceUtilities
#include <MtmExtendedCapabilities.hrh>  // KUidMtmQueryCanCreateNewMsgValue

#include <mtudreg.h>        // CMtmUiDataRegistry
#include <MTMStore.h>       // CMtmStore
#include <msvids.h>       // KMsvRootIndexEntryId, KMsvNullIndexEntryId

#include <MceSettingsArrays.h>                 // CMceUidNameArray
#include <MceSettingsSessionObserver.h>        // CMceSettingsSessionObserver
#include <MceSettingsGeneralSettingsDialog.h>  // CMceGeneralSettingsDialog
#include <MceSettingsMultipleServicesDialog.h> // CMceSettingsMultipleServicesDialog
#include <aknlists.h>                          // CAknSettingStyleListBox
#include <MceSettingsGSPluginRsc.rsg>          // Resources
#include <RCustomerServiceProfileCache.h>      // RCustomerServiceProfileCache
#include <SenduiMtmUids.h>                     // KSenduiMtmSmtpUid, KSenduiMtmSmsUid,
                                               // KSenduiMtmMmsUid
#include <csxhelp/mce.hlp.hrh>                 // Help file
#include "MceSettingsGSPluginContainer.h"      // own header

#include <featmgr.h>  // CR : 401-1806


// CONSTANTS
const TUid KCBSApplicationId                = {0x101F4CD3};
const TUid KCBSApplicationSettingsViewId    = {6};
const TInt KMceSettingsExtraItems           = 2;
const TInt KMceSettingsCBS                  = 2;
const TInt KMceSettingsCBSUid               = 42;
const TUid KMceApplicationId                = { 0x100058C5 };

//CMAIL Update
#define KUidMsgTypeFsMtmVal               0x2001F406

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMceSettingsGSPluginContainer::CMceSettingsGSPluginContainer()
// Default constructor.
// ---------------------------------------------------------------------------
//
CMceSettingsGSPluginContainer::CMceSettingsGSPluginContainer()
    {
    iDialogOpen = EFalse;
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPluginContainer::ConstructL(const TRect& aRect)
// Symbian OS phase 2 constructor
// ---------------------------------------------------------------------------
//
void CMceSettingsGSPluginContainer::ConstructL( const TRect& aRect )
    {
    // Construct iListBox
    iListBox = new ( ELeave ) CAknSettingStyleListBox;
    iMsvSession = CMsvSession::OpenSyncL(*this);
    iAccountManager = CMceSettingsSessionObserver::NewL( iMsvSession );
    iEmailFramework = EFalse;
    
    FeatureManager::InitializeLibL();
    
    if ( FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework ) )
     {
            iEmailFramework = ETrue;
     }
    
    FeatureManager::UnInitializeLib();
   
    
    BaseConstructL( aRect, R_MCEUI_MAINVIEW_TITLE, 0 );// last parameter 0, since dynamic listbox
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPluginContainer::~CMceSettingsGSPluginContainer()
// Destructor
// ---------------------------------------------------------------------------
//
CMceSettingsGSPluginContainer::~CMceSettingsGSPluginContainer()
    {
    delete iAccountManager;
    delete iMsgTypesSettings;
    delete iMsvSession;
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPluginContainer::ConstructListBoxL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CMceSettingsGSPluginContainer::ConstructListBoxL( TInt /*aResLbxId*/ )
    {
    // listbox creation and initialization
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, EAknListBoxLoopScrolling );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    UpdateSettingItemsL();
    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray( iMsgTypesSettings );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPluginContainer::OfferKeyEventL()
// See header for details.
// ---------------------------------------------------------------------------
//
TKeyResponse CMceSettingsGSPluginContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    return iListBox->OfferKeyEventL( aKeyEvent, aType );
    }


// -----------------------------------------------------------------------------
// CMceSettingsGSPluginContainer::GetHelpContext()
// See header for details.
// -----------------------------------------------------------------------------
//
void CMceSettingsGSPluginContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KMceApplicationId;
    aContext.iContext = KMCE_HLP_SETTINGS;
    }
    
    
// -----------------------------------------------------------------------------
// CMceSettingsGSPluginContainer::UpdateSettingItemsL()
// See header for details.
// -----------------------------------------------------------------------------
//    
void CMceSettingsGSPluginContainer::UpdateSettingItemsL()
    {
    if ( !iMsgTypesSettings )
        {
        iMsgTypesSettings = CMceUidNameArray::NewL();
        }
    else
        {
        iMsgTypesSettings->Reset();
        }

    CMtmUiDataRegistry* uiRegistry = iAccountManager->UiRegistry();
    CMtmStore* mtmStore = iAccountManager->MtmStore();
    const TInt count = uiRegistry->NumRegisteredMtmDlls();
    TInt loop = 0;
    for ( loop = 0; loop < count; loop++ )
        {
        LoadMtmL( uiRegistry->MtmTypeUid( loop ), *mtmStore );
        }

    // this is not used anywhere, two last items in the list are
    // ALWAYS cbs settings and mce general settings

    // Sort settings groups
    SortAndChangeSmsMmsEmailFirstL();

    // Check Cell Broadcast Csp bit
    iCbs = CheckCspBitL();


    TUid uid;
    uid.iUid = KMceSettingsCBSUid;
	if(iCbs)
	{
    	HBufC* text = StringLoader::LoadL( R_MCEUI_CBS_SETTINGS_TEXT, iEikonEnv );
    	TUidNameInfo cbsSettings( uid, *text );
    	delete text;
    	iMsgTypesSettings->AppendL( cbsSettings );
	}

    HBufC* text = StringLoader::LoadL( R_MCEUI_GENERAL_SETTINGS_TEXT, iEikonEnv );
    TUidNameInfo generalSettings( uid, *text );
    delete text;
    iMsgTypesSettings->AppendL( generalSettings );
    }


// -----------------------------------------------------------------------------
// CMceSettingsGSPluginContainer::LoadMtmL
//
// -----------------------------------------------------------------------------
//
void CMceSettingsGSPluginContainer::LoadMtmL(
    const TUid aMtmType,
    CMtmStore& aMtmStore )
    {
    
    TUid   mtmUid = TUid::Uid(aMtmType.iUid);
    
    TBool notSelectableMtm = ( (mtmUid == KSenduiMtmSmtpUid)  
            || (mtmUid == KSenduiMtmSyncMLEmailUid) 
            || (aMtmType.iUid == KUidMsgTypeFsMtmVal) ) ;
    
    // Cmail changes - EMail, Sync EMail, Freestyle Settings will not be populated in the Messaging settings 
    
    if( !( (iEmailFramework) && (notSelectableMtm) ) )
    {
      
    CBaseMtmUiData* uiData = NULL;
    TRAPD( err, ( uiData = &(aMtmStore.MtmUiDataL(aMtmType)) ) );
    if( err != KErrNone )
        {
        return;
        }
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
    }//end if
        
    }


// -----------------------------------------------------------------------------
// CMceUi::HandleSessionEventL
//
// -----------------------------------------------------------------------------
//
void CMceSettingsGSPluginContainer::HandleSessionEventL(
    TMsvSessionEvent aEvent,
    TAny* /*aArg1*/,
    TAny* aArg2,
    TAny* /*aArg3*/ )
    {
    TMsvId folderId = KMsvNullIndexEntryId;

    // ConstructL must have left as we have returned to the active scheduler
    // before complete construction
    switch (aEvent)
        {
    case EMsvServerReady:
    case EMsvCorruptedIndexRebuilt:
        break;
    case EMsvMtmGroupInstalled:
    case EMsvMtmGroupDeInstalled:
        UpdateSettingItemsL();
        break;
    case EMsvEntriesCreated:
    case EMsvEntriesDeleted:
    case EMsvEntriesChanged:
        folderId = (*(TMsvId*) (aArg2));
        if ( folderId == KMsvRootIndexEntryId )
            {
            UpdateSettingItemsL();
            }
        break;

    case EMsvServerFailedToStart:
    case EMsvServerTerminated:
    case EMsvCloseSession:
    case EMsvGeneralError:
    case EMsvMediaUnavailable:
    case EMsvMediaAvailable:
    default:
        break;
        }
    }
    
    
// -----------------------------------------------------------------------------
// CMceSettingsGSPluginContainer::EditSubSettingsL()
//
// -----------------------------------------------------------------------------
//
void CMceSettingsGSPluginContainer::EditSubSettingsL()
    {
    const TInt numberOfItems = iListBox->Model()->NumberOfItems();
    TInt choice = iListBox->CurrentItemIndex();
    // Run time Cell Broadcast variant.
    TInt removedSettingItems = 0;

    if ( !iCbs )
    {
        removedSettingItems++;
    }

    if ( choice < ( numberOfItems - KMceSettingsExtraItems ) + removedSettingItems )
        {

        TBool  canCreateNewAccount = iAccountManager->CanCreateNewAccountL(
            (*iMsgTypesSettings)[choice].iUid );

        if ( canCreateNewAccount )
            {
            if( iDialogOpen )
            	{
				return;            	
            	}
            iDialogOpen = ETrue;
            // if mtm supports account creation then open new dialog to handle many accounts
            TInt value = CMceSettingsMultipleServicesDialog::CreateAndExecuteL(
                *iAccountManager,  (*iMsgTypesSettings)[choice].iUid, iMsvSession );
            iDialogOpen = EFalse;
            }
        else
            {
            // mtm cannot create new accounts, edit only FIRST service if exists
            CMsvEntrySelection* sel =
                MsvUiServiceUtilities::GetListOfAccountsWithMTML( *iMsvSession, (*iMsgTypesSettings)[choice].iUid );

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
    else if ( /*iCbs &&*/ choice == numberOfItems - KMceSettingsCBS )
        {
        // CBS settings are second from the end of the list
        TVwsViewId viewId( KCBSApplicationId, KCBSApplicationSettingsViewId );
        STATIC_CAST( CCoeAppUi*, ControlEnv()->AppUi())->ActivateViewL( viewId );
        }
    else
        {
        // general settings are list item in the list
        TInt value = CMceGeneralSettingsDialog::CreateAndExecuteL(
            iMsvSession,
            *this );
        }
    }


// ---------------------------------------------------------
// CMceSettingsGSPluginContainer::SortAndChangeSmsMmsEmailFirstL
// ---------------------------------------------------------
//
void CMceSettingsGSPluginContainer::SortAndChangeSmsMmsEmailFirstL()
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
// CMceSettingsGSPluginContainer::ChangeMsgTypeTopL
// ---------------------------------------------------------
//
void CMceSettingsGSPluginContainer::ChangeMsgTypeTopL( 
    CUidNameArray& aArray, 
    TUid aMsgType ) const
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
// CMceSettingsGSPluginContainer::CheckCspBitL
// returns ETrue if Cell Broadcast csp bit is on
//
// ----------------------------------------------------
TBool CMceSettingsGSPluginContainer::CheckCspBitL( ) const
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


// End of File
