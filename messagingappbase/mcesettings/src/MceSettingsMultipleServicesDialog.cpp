/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Message centre's mail/smail dialog.
*     Includes "remote mailboxes" and "Mailbox in use" setting items.
*
*/



// INCLUDE FILES

#include <muiumsvuiserviceutilitiesinternal.h> //MsvUiServiceUtilities
#include <msvuids.h>
#include <AknQueryDialog.h> // CAknQueryDialog
#include <aknViewAppUi.h>
#include <aknradiobuttonsettingpage.h>
#include <StringLoader.h>   // StringLoader
#include <e32base.h>

#include <msvids.h>         // KMsvUnknownServiceIndexEntryId
#include "MceSettingsMultipleServicesDialog.h"
#include "MceSettingsAccountManager.h"
#include "MceSettingsIds.hrh"

#include "MceSettingsArrays.h"
#include "MceSettingsMtmServicesDialog.h"
#include <MceSettings.rsg>

#include <featmgr.h>
#include <hlplch.h>             // For HlpLauncher
#include <csxhelp/mbxs.hlp.hrh>
#include "mce.hlp.hrh"

#include <bldvariant.hrh>
#include <SenduiMtmUids.h>

#include <ImumInternalApi.h>
#include <ImumInHealthServices.h>
#include <muiuflags.h>
#include "MceSettingsUtils.h"
#include <muiuemailtools.h>
#include <mtudreg.h>

#include <centralrepository.h>
#include <miutset.h>
#include <MessagingDomainCRKeys.h>
#include <akninputblock.h> // CAknInputBlock
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
#include <data_caging_path_literals.hrh>
#endif // RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
#include <imum.rsg>

// CONSTANTS

#define KMceApplicationUidValue         0x100058C5
const TUid KMceApplicationUid           = {KMceApplicationUidValue};

const TInt KMceSettingsRemoteMailboxInUseSelectableEmailInUse   = 0;
const TInt KMceSettingsRemoteMailboxInUse   = 1;
const TInt KMceSettingsMailboxTextLength    = 20;
const TInt KMceSettingsArrayGranularity     = 4;

const TInt KMceSettingsMtmNameBufLength     = KHumanReadableNameLength + 5;
const TInt KMSKPosition = 3;

_LIT(KMceMtmName1,"\t");
_LIT(KMceMtmName2,"\t\t");
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
_LIT( KMceDirAndFile,"MceSettings.rsc" );
#endif // RD_MESSAGING_GENERAL_SETTINGS_RENOVATION

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------------------------
// CMceSettingsMultipleServicesDialog::CreateAndExecuteL()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CMceSettingsMultipleServicesDialog::CreateAndExecuteL(
                                              MMceSettingsAccountManager& aManager,
                                              TUid aMessageType,
                                              CMsvSession* aSession )
    {
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    CMceSettingsMultipleServicesDialog* mailDlg =new( ELeave ) CMceSettingsMultipleServicesDialog(aManager,  aMessageType, aSession );
    CleanupStack::PushL( mailDlg );
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdSelectableEmail ) )  
    {
      mailDlg->ConstructL(R_MCE_SETTINGS_MULTIPLE_SERVICES_DIALOG_MENUBAR_WITH_SELECTABLE_EMAIL );
  	  CleanupStack::Pop( mailDlg );
      return mailDlg->ExecuteLD( R_MCE_SETTINGS_MULTIPLE_SERVICES_DIALOG_MENUBAR_WITH_SELECTABLE_EMAIL );
    }
   else
   {
  	  mailDlg->ConstructL( R_MCE_SETTINGS_MULTIPLE_SERVICES_DIALOG_MENUBAR );
  	  CleanupStack::Pop( mailDlg );
      return mailDlg->ExecuteLD( R_MCE_SETTINGS_MULTIPLE_SERVICES_DIALOG );
    
   }
   FeatureManager::UnInitializeLib();
#else
    User::Leave( KErrNotSupported );
    return KErrNotSupported;
#endif // RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    }

// ----------------------------------------------------
// C++ default constructor can NOT contain any code that
// might leave.
// ----------------------------------------------------
CMceSettingsMultipleServicesDialog::CMceSettingsMultipleServicesDialog(
                                                MMceSettingsAccountManager& aManager, 
                                                TUid aMessageType,
                                                CMsvSession* aSession )
    : 
    CMceSettingsTitlePaneHandlerDialog(),
    iManager( aManager ),
    iMessageType( aMessageType ),
    iSession( aSession )
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    ,iResources( *CCoeEnv::Static() )
#endif // RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
	,iSubDialogOpen(EFalse)
    ,iS60WizLaunched( EFalse )
    {
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::Destructor
// ----------------------------------------------------
CMceSettingsMultipleServicesDialog::~CMceSettingsMultipleServicesDialog()
    {
    if ( iMceIntegration )
        {
        if ( iAiwServiceHandler )
            {
            iAiwServiceHandler->Reset();
            }
        delete iAiwServiceHandler;
        iAiwServiceHandler = NULL;
        }
    // do not delete iMtmAccountTypesDlg only used to notify msgs changes
    if ( iSessionObserverAdded )
        {
        iManager.Session().RemoveObserver( *this );
        }
    delete iEmailApi;
    iEmailApi = NULL;
    iMailboxArray.Reset();  
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    iResources.Close();
#endif // RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    if ( iProductIncludesSelectableEmail )
        {
        if ( iAccountArray )
            {
            delete iAccountArray;		
            }
        }
    }


// ---------------------------------------------------------------------------
// CMceSettingsMultipleServicesDialog::ConstructL()
// ---------------------------------------------------------------------------
//
void CMceSettingsMultipleServicesDialog::ConstructL(TInt aResource )
    {
    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL( KCRUidSelectableDefaultEmailSettings  ) );

    if ( ret == KErrNone )
        {
        // Check if there is other than platform email application registered
        // to handle S60 mailboxes  
        TInt err = repository->Get( KIntegratedEmailAppMtmPluginId , iMtmPluginId );
        if ( err != KErrNone )
            {
            iMtmPluginId = 0;
            }
        }
    delete repository;

    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdSelectableEmail ) )
        {
        iProductIncludesSelectableEmail = ETrue;
        }
    else
        {
        iProductIncludesSelectableEmail = EFalse;	
        }
    if( FeatureManager::FeatureSupported( KFeatureIdEmailMceIntegration ) )
        {
        iMceIntegration = ETrue;
        }
    else
        {
        iMceIntegration = EFalse;
        }
    FeatureManager::UnInitializeLib();
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    TParse fp;
    fp.Set( KMceDirAndFile, &KDC_RESOURCE_FILES_DIR , NULL );
    TFileName fileName = fp.FullName();
    iResources.OpenL( fileName );
    CMceSettingsTitlePaneHandlerDialog::ConstructL( aResource );
#endif // RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    iEmailApi = CreateEmailApiL( iSession );
    UpdateMailboxDataL();
    }
   
// ---------------------------------------------------------------------------
// CMceSettingsMultipleServicesDialog::ConstructL()
// ---------------------------------------------------------------------------
//
void CMceSettingsMultipleServicesDialog::ConstructL()
    {
    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL( KCRUidSelectableDefaultEmailSettings  ) );

    if ( ret == KErrNone )
        {
        // Check if there is other than platform email application registered
        // to handle S60 mailboxes  
        TInt err = repository->Get( KIntegratedEmailAppMtmPluginId , iMtmPluginId );
        if ( err != KErrNone )
            {
            iMtmPluginId = 0;
            }
        }
    delete repository;

    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdSelectableEmail ) )
        {
        iProductIncludesSelectableEmail = ETrue;
        }
    else
        {
        iProductIncludesSelectableEmail = EFalse;	
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdEmailMceIntegration ) )
        {
        iMceIntegration = ETrue;
        }
    else
        {
        iMceIntegration = EFalse;
        }
    FeatureManager::UnInitializeLib();
    iEmailApi = CreateEmailApiL( iSession );
    UpdateMailboxDataL();
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::OkToExitL
// ----------------------------------------------------
TInt CMceSettingsMultipleServicesDialog::OkToExitL( TInt aButtonId )
    {
    if ( iSubDialogOpen )
        {
        // Subdialog open, don't close yet
        return EFalse;
        }
    TBool okToExit = CAknDialog::OkToExitL( aButtonId );
    if ( ( aButtonId==EMceSettingsCmdSettingsDialogOpen ) ||
         ( aButtonId==EMceSettingsCmdSettingsDialogChange ) )
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
 
// ---------------------------------------------------------------------------
// CMceSettingsMultipleServicesDialog::UpdateMailboxDataL()
// ---------------------------------------------------------------------------
//
void CMceSettingsMultipleServicesDialog::UpdateMailboxDataL()
    {
    const MImumInHealthServices* healthApi = &iEmailApi->HealthServicesL();
    
    // Get the array of the mailboxes for the current view
    MceSettingsUtils::GetHealthyMailboxListL( 
        *healthApi,
        iMailboxArray,
        iMessageType != KSenduiMtmSyncMLEmailUid,
        iMessageType != KSenduiMtmSyncMLEmailUid,
        iMessageType == KSenduiMtmSyncMLEmailUid,
        EFalse );
    iNumberOfMailboxes = iMailboxArray.Count();
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::ProcessCommandL
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::ProcessCommandL( TInt aCommandId )
    {
    if ( aCommandId == EMceSettingsCmdSettingsDialogExit )
        {
        CAknDialog::ProcessCommandL( aCommandId ); // hides menu
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

    CAknDialog::ProcessCommandL( aCommandId ); // hides menu

    if ( !iProductIncludesSelectableEmail )
        {
        if ( !iEmailApi )
            {
            iEmailApi = CreateEmailApiL( iSession );
            }

        const MImumInHealthServices* healthApi = &iEmailApi->HealthServicesL();

        TInt error = MceSettingsUtils::GetHealthyMailboxListL( *healthApi,
                                                               iMailboxArray,
                                                               ETrue,
                                                               ETrue,
                                                               ETrue,ETrue );

        iNumberOfMailboxes = iMailboxArray.Count();        
        iMailboxArray.Reset();        
        error = MceSettingsUtils::GetHealthyMailboxListL( *healthApi,
                                  iMailboxArray,
                                  iMessageType != KSenduiMtmSyncMLEmailUid,
                                  iMessageType != KSenduiMtmSyncMLEmailUid,
                                  iMessageType == KSenduiMtmSyncMLEmailUid,ETrue );
        }
    if ( iProductIncludesSelectableEmail )
        {
        CEikTextListBox* list=STATIC_CAST( CEikTextListBox*, 
                Control( EMceSettingsMultipleServicesDialogChoiceList ) );
        TInt item = list->CurrentItemIndex();

        // first item in listbox is not account and must not be used
        if ( item > 0 )
            {
            item--;
            }

        TUid mtmType;

        if ( item >= 0 && iAccountArray->Count() &&
             ( iAccountArray->Count() > item ) ) 
            {
            TMsvEntry tentry;
            TMsvId dummyserviceid;
            if ( iSession->GetEntry( (*iAccountArray)[item].iUid.iUid,
                 dummyserviceid,
                 tentry ) == KErrNone )
                {
                mtmType = tentry.iMtm;
                }
            else
                {
                mtmType.iUid = 0;
                }

            if ( ( mtmType == KSenduiMtmImap4Uid ) ||
                 ( mtmType == KSenduiMtmPop3Uid ) )
                {
                mtmType = KSenduiMtmSmtpUid;
                }
            }

        if ( iActiveMenuBar == EMultipleServices ) 
            {
            switch ( aCommandId )
                {
                case EMceSettingsCmdSettingsDialogChange:
                    {
                    ShowListBoxSettingPageL();
                    }
                    break;       
                case EMceSettingsCmdSettingsDialogOpen:
                    iManager.EditAccountL( (*iAccountArray)[item].iUid.iUid );
                    break;
                case EMceSettingsCmdSettingsDialogCreateNewEmpty:				
                    iManager.CreateNewAccountL( mtmType, KMsvNullIndexEntryId );
                    break;
                case EMceSettingsCmdSettingsDialogCreateNewEmptyImap:				
                    CreateNewImapPopL();
                    break;
                case EMceSettingsCmdSettingsDialogDelete:
                    iManager.DeleteAccountL( (*iAccountArray)[item].iUid.iUid );
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
        }
    else // iProductIncludesSelectableEmail
        {
        switch ( aCommandId )
            {
            case EMceSettingsCmdSettingsDialogOpen:
                EditServicesL();
                break;
            case EMceSettingsCmdSettingsDialogChange:
                ShowListBoxSettingPageL();
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
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::DynInitMenuPaneL
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::DynInitMenuPaneL( TInt aResourceId, 
                                                                    CEikMenuPane* aMenuPane )
    {
    if ( iProductIncludesSelectableEmail )
        {
        CEikTextListBox* list = STATIC_CAST( CEikTextListBox*,
                Control( EMceSettingsMultipleServicesDialogChoiceList ) );
        TInt item = list->CurrentItemIndex();    

        if ( aResourceId == R_MCE_SETTINGS_MULTIPLE_SERVICES_DIALOG_MENUPANE_WITH_SELECTABLE_EMAIL )
            {    			
            TInt offset = 0;

            if ( item > offset )
                {
                TUid mtmType;

                aMenuPane->DeleteMenuItem(
                        EMceSettingsCmdSettingsDialogChange );				
                aMenuPane->DeleteMenuItem(
                        EMceSettingsCmdSettingsDialogCreateNewEmpty );    	                      

                aMenuPane->SetItemDimmed( EMceSettingsCmdSettingsDialogHelp,
                        !FeatureManager::FeatureSupported( KFeatureIdHelp ) );					

                if ( iAccountArray->Count() &&
                   ( iAccountArray->Count() >= item ) )
                    {   	        		
                    CMsvEntry* entry = iSession->GetEntryL(
                                       (*iAccountArray)[item-1].iUid.iUid ); 

                    TMsvEntry tentry = entry->Entry();
                    mtmType = tentry.iMtm;
                    delete entry;

                    if ( iAccountArray->Count() >= KMceMaxNumberOfMailboxes )
                        {             
                        if ( ( mtmType != KSenduiMtmImap4Uid ) &&
                             ( mtmType != KSenduiMtmPop3Uid ) )
                            {
                            aMenuPane->DeleteMenuItem(
                                    EMceSettingsCmdSettingsDialogDelete ); 
                            }                		  	
                        aMenuPane->DeleteMenuItem(
                                EMceSettingsCmdSettingsDialogCreateNewEmptyImap );
                        }        		
                    else if ( ( mtmType != KSenduiMtmImap4Uid ) &&
                              ( mtmType != KSenduiMtmPop3Uid ) &&
                              ( mtmType != KSenduiMtmSyncMLEmailUid ) )
                        {
                        aMenuPane->DeleteMenuItem(
                                EMceSettingsCmdSettingsDialogCreateNewEmptyImap );
                        aMenuPane->DeleteMenuItem(
                                EMceSettingsCmdSettingsDialogDelete );         	        
                        }
                    }
                }

            else if ( item == KMceSettingsRemoteMailboxInUseSelectableEmailInUse )
                {
                aMenuPane->DeleteMenuItem(
                        EMceSettingsCmdSettingsDialogOpen );
                aMenuPane->DeleteMenuItem(
                        EMceSettingsCmdSettingsDialogDelete );        
                aMenuPane->DeleteMenuItem(
                        EMceSettingsCmdSettingsDialogCreateNewEmpty );      	    

                aMenuPane->SetItemDimmed( EMceSettingsCmdSettingsDialogHelp,
                        !FeatureManager::FeatureSupported( KFeatureIdHelp ) ); 	

                if ( iAccountArray->Count() >= KMceMaxNumberOfMailboxes )
                    {
                    aMenuPane->DeleteMenuItem(
                            EMceSettingsCmdSettingsDialogCreateNewEmptyImap );
                    }	            	                  	
                }
            else
                {
                aMenuPane->DeleteMenuItem(
                        EMceSettingsCmdSettingsDialogChange );				
                aMenuPane->DeleteMenuItem(
                        EMceSettingsCmdSettingsDialogCreateNewEmptyImap ); 

                aMenuPane->SetItemDimmed( EMceSettingsCmdSettingsDialogHelp,
                        !FeatureManager::FeatureSupported( KFeatureIdHelp ) ); 					

                if ( iAccountArray->Count() >= KMceMaxNumberOfMailboxes )
                    {            		
                    TUid mtmType;

                    CMsvEntry* entry = iSession->GetEntryL(
                                       (*iAccountArray)[item-1].iUid.iUid ); 

                    TMsvEntry tentry = entry->Entry();
                    mtmType = tentry.iMtm;
                    delete entry;            		

                    if ( ( mtmType != KSenduiMtmImap4Uid ) &&
                         ( mtmType != KSenduiMtmPop3Uid ) )
                        {
                        aMenuPane->DeleteMenuItem(
                                EMceSettingsCmdSettingsDialogDelete ); 
                        }              		
                    aMenuPane->DeleteMenuItem(
                            EMceSettingsCmdSettingsDialogCreateNewEmpty );
                    }
                }
            }
        } // iProductIncludesSelectableEmail
    else
        {
    if ( aResourceId == R_MCE_SETTINGS_MULTIPLE_SERVICES_DIALOG_MENUPANE )
        {
            aMenuPane->SetItemDimmed( EMceSettingsCmdSettingsDialogHelp,
            !FeatureManager::FeatureSupported( KFeatureIdHelp ) );

        CEikTextListBox* list = STATIC_CAST( CEikTextListBox*, 
                                  Control( EMceSettingsMultipleServicesDialogChoiceList ) );
        if ( list->CurrentItemIndex() == 0 )
            {
            aMenuPane->DeleteMenuItem( EMceSettingsCmdSettingsDialogChange );
            }
        else
            {
            aMenuPane->DeleteMenuItem( EMceSettingsCmdSettingsDialogOpen );
            }
        }
    }
    }
// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::PreLayoutDynInitL
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::PreLayoutDynInitL( )
    {
    CEikTextListBox* list = STATIC_CAST( CEikTextListBox*, 
                                Control( EMceSettingsMultipleServicesDialogChoiceList ) );

    THumanReadableName mtmName;

    iSettingsItemArray = new (ELeave) CDesCArrayFlat(
                         KMceSettingsArrayGranularity );
    if ( iProductIncludesSelectableEmail )
        {
        if ( !iAccountArray )
            {
            iAccountArray = new (ELeave) CUidNameArray(
                            KMceSettingsArrayGranularity );
            }	

        SetAccountInUseL();
        mtmName = iManager.MtmName( iMessageType );
        UpdateServicesArrayL();
        // Scrollbar
        list->CreateScrollBarFrameL( ETrue );
        list->ScrollBarFrame()->SetScrollBarVisibilityL(
                CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
        }
    else // iProductIncludesSelectableEmail
        {
        mtmName = iManager.MtmName( iMessageType );
    if ( iMessageType == KSenduiMtmSmtpUid )
        {
        // email
        HBufC* text = StringLoader::LoadLC(
            R_MCE_SETTINGS_MAIL_REMOTE_MAILBOXES, iEikonEnv );
        iSettingsItemArray->AppendL( *text );
        CleanupStack::PopAndDestroy( text );
        }
    else if ( iMessageType == KSenduiMtmSyncMLEmailUid )
        {
        // syncML
        HBufC* text = StringLoader::LoadLC(
            R_MCE_SETTINGS_SYNCMAIL_MAILBOXES, iEikonEnv );
        iSettingsItemArray->AppendL( *text );
        CleanupStack::PopAndDestroy( text );
        }
    else
        {
        // not email/syncML
        TBuf<KMceSettingsMtmNameBufLength> mtms;
        mtms.Copy( KMceMtmName1 );
        mtms.Append( mtmName );
        mtms.Append ( KMceMtmName2 );
        iSettingsItemArray->AppendL( mtms );
        }
    
    SetAccountInUseL();
        } // iProductIncludesSelectableEmail

    CTextListBoxModel* model=list->Model();
    model->SetItemTextArray( iSettingsItemArray );
    list->SetListBoxObserver(this);

    if ( iMessageType == KSenduiMtmSmtpUid )
        {
        // email
        HBufC* newText = StringLoader::LoadLC(
            R_MCE_EMAIL_SETTINGS_TITLE, iEikonEnv );
        StoreTitleTextAndSetNewL( *newText );
        CleanupStack::PopAndDestroy( newText );
        }
    else if ( iMessageType == KSenduiMtmSyncMLEmailUid )
        {
        // syncML
        HBufC* newText = StringLoader::LoadLC(
            R_MCE_TITLE_SETTINGS_SYNCMAIL, iEikonEnv );
        StoreTitleTextAndSetNewL( *newText );
        CleanupStack::PopAndDestroy(  newText );
        }
    else
        {
        // not email/syncML
        StoreTitleTextAndSetNewL( mtmName );
        }     

    iManager.Session().AddObserverL( *this );
    iSessionObserverAdded = ETrue;
    SetMSKButtonL();
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::ShowListBoxSettingPageL
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::ShowListBoxSettingPageL()
    {
    TInt count = 0;
    TBool nbrOfMailboxesOk = ETrue;
    CUidNameArray* sel = NULL; // Used for iProductIncludesSelectableEmail

    //Produce radiobutton list of mailboxes for selecting the default mailbox
    if ( iProductIncludesSelectableEmail )
        {
        sel = MtmEmailAccountsL();
        CleanupStack::PushL( sel );
        count = sel->Count();
        }
    else
        {
        count = iMailboxArray.Count();
        if ( iNumberOfMailboxes >= KMceMaxNumberOfMailboxes )
            {
            nbrOfMailboxesOk = EFalse;
            }
        }
    if ( count == 0 && nbrOfMailboxesOk )
        {
        if ( HandleNoMailboxesL() )
            {
            if( iMceIntegration )
                {
                if ( iS60WizLaunched )
                    {
                    iS60WizLaunched = EFalse;
                    EditServicesL();
                    }
                }
            else // iMceIntegration
                {
                EditServicesL();
                } // iMceIntegration
            }
        if ( iProductIncludesSelectableEmail )
            {
            CleanupStack::PopAndDestroy( sel );
            }
        return;
        }
    UpdateMailboxDataL();
    CDesCArrayFlat* items = new( ELeave )CDesCArrayFlat(
        KMceSettingsArrayGranularity );
    CleanupStack::PushL( items );

    TMsvId id = MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
        iManager.Session(),
        iMessageType,
        ETrue );

    if ( iProductIncludesSelectableEmail &&
            ( iMessageType != KSenduiMtmSyncMLEmailUid ) )
        {
        CRepository* repository = NULL;
        TRAPD( ret, repository = CRepository::NewL(
                                 KCRUidSelectableDefaultEmailSettings  ) );
        CleanupStack::PushL( repository );
        TInt val;

        if ( ret == KErrNone )
            {
            if ( repository->Get(
                    KSelectableDefaultMailAccount , val ) == KErrNone )
                {
                id = val;
                }
            }
        CleanupStack::PopAndDestroy( repository );		    
        } // iProductIncludesSelectableEmail
    TMsvEntry mailboxEntry;
    TMsvId serviceId; // not used but needed by GetEntry function
    TBool foundService = iManager.Session().GetEntry( id, serviceId, mailboxEntry ) == KErrNone;

    TInt currentItem = 0;
    for ( TInt loop = 0; loop < count; loop++ )
        {
        TMsvId arrayItemId;
        if ( iProductIncludesSelectableEmail )
            {
            items->AppendL( (*sel)[loop].iName );
            arrayItemId = (*sel)[loop].iUid.iUid;
            }
        else
            {
            TMsvEntry tentry =
                    iEmailApi->MailboxUtilitiesL().GetMailboxEntryL( 
                    iMailboxArray[loop],
                    MImumInMailboxUtilities::ERequestSending );
            items->AppendL( tentry.iDetails );
            arrayItemId = iMailboxArray[loop];
            }
        // iRelatedId is compared because array can include both smtp and
        // imap4/pop3 services
        if ( foundService &&
           ( id == arrayItemId || mailboxEntry.iRelatedId == arrayItemId ) )
            {
            currentItem = loop;
            }
        } // for

    CAknRadioButtonSettingPage* dlg =
            new ( ELeave )CAknRadioButtonSettingPage(
            R_MCE_GENERAL_SETTINGS_LISTBOX, 
            currentItem, items);
    CleanupStack::PushL( dlg );    

    THumanReadableName mtmName = iManager.MtmName( iMessageType );    
    if ( iMessageType == KSenduiMtmSmtpUid )
        {
        // email
        HBufC* titleText = StringLoader::LoadLC(
                           R_MCE_SETTINGS_MAIL_TITLE_TITLEDEFAULT,
                           iEikonEnv );
        dlg->SetSettingTextL( *titleText );
        CleanupStack::PopAndDestroy( titleText );
        }
    else if ( iMessageType == KSenduiMtmSyncMLEmailUid )
        {
        // syncML
        HBufC* titleText = StringLoader::LoadLC(
                           R_MCE_SETTINGS_MAIL_TITLE_TITLEDEFAULT,
                           iEikonEnv );
        dlg->SetSettingTextL( *titleText );
        CleanupStack::PopAndDestroy( titleText );
        }
    else
        {
        // not email/syncML
        dlg->SetSettingTextL( mtmName );
        }

    CleanupStack::Pop( dlg );

    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        if ( iProductIncludesSelectableEmail )
            {
            id = (*sel)[currentItem].iUid.iUid;
            }
        else
            {
            id = iMailboxArray[currentItem];
            }
        CMsvSession& session = iManager.Session();
        TMsvEntry entry;
        TMsvId serviceid;
        if ( session.GetEntry( id, serviceid, entry ) == KErrNone )
            {
            // idTemp is used only if iProductIncludesSelectableEmail
            TInt idTemp = entry.Id();
            MsvUiServiceUtilitiesInternal::SetDefaultServiceForMTML(
                                           session, entry.iMtm, id );

            // set related service's default too...
            if ( session.GetEntry(
                    entry.iRelatedId, serviceid, entry ) == KErrNone )
                {
                MsvUiServiceUtilitiesInternal::SetDefaultServiceForMTML(
                                               session, entry.iMtm, entry.Id() );
                }

            if( iProductIncludesSelectableEmail &&
                    ( ( entry.iMtm == KSenduiMtmImap4Uid ) ||
                            ( entry.iMtm == KSenduiMtmPop3Uid ) ||
                            ( entry.iMtm == KSenduiMtmSmtpUid ) ) )
                {		               
                CRepository* repository = NULL;
                TRAPD( ret, repository =
                CRepository::NewL( KCRUidSelectableDefaultEmailSettings  ) );
                CleanupStack::PushL( repository );

                if ( ret == KErrNone )
                    {
                    TInt value = entry.Id();
                    repository->Set( KSelectableDefaultMailAccount , idTemp );
                    }
                CleanupStack::PopAndDestroy( repository );		
                } // iProductIncludesSelectableEmail
            }               
        SetAccountInUseL();

        CEikTextListBox* list = STATIC_CAST( CEikTextListBox*, 
                Control( EMceSettingsMultipleServicesDialogChoiceList ) );
        list->DrawDeferred();
        }

    CleanupStack::PopAndDestroy( items );       

    if ( iProductIncludesSelectableEmail )
        {
        CleanupStack::PopAndDestroy( sel );
        }
    }

// ---------------------------------------------------------------------------
// CMceSettingsMultipleServicesDialog::LaunchDialogL()
// ---------------------------------------------------------------------------
//
void CMceSettingsMultipleServicesDialog::LaunchDialogL( 
    MMceSettingsAccountManager& aManager, 
    const TUid& aMessageType, 
    CMsvSession& aSession,
    const TInt& aMenuResource,
    const TInt& aDialogResource )
    {
    // if mtm supports account creation then open new dialog to 
    // handle many accounts
    CMceSettingsMultipleServicesDialog* mailDlg =
        new( ELeave ) CMceSettingsMultipleServicesDialog( 
        aManager, aMessageType, &aSession );
    CleanupStack::PushL( mailDlg );
    mailDlg->ConstructL();
    static_cast<CAknDialog*>( mailDlg )->ConstructL( aMenuResource );
    CleanupStack::Pop( mailDlg );
    mailDlg->ExecuteLD( aDialogResource );    
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::EditServicesL
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::EditServicesL()
    {
    CUidNameArray* sel = iManager.MtmAccountsL( iMessageType );
    UpdateMailboxDataL();
    iSubDialogForceClosed = ETrue;
    
    if ( ( iNumberOfMailboxes < KMceMaxNumberOfMailboxes ) &&
         ( iMailboxArray.Count() == 0 ) )
        {
        delete sel;
        if ( HandleNoMailboxesL() && ( iSubDialogForceClosed == TRUE ) )
            {    	    
			if( iMceIntegration )
				{
            	if ( iS60WizLaunched )
                	{
                	iS60WizLaunched = EFalse;
                    EditServicesL();
                	}
				} // iMceIntegration
			else
				{
            	EditServicesL();
                }
            }
        return;
        }
    delete sel;
    if ( iProductIncludesSelectableEmail )
        {
        CEikTextListBox* list = STATIC_CAST( CEikTextListBox*,
                Control( EMceSettingsMultipleServicesDialogChoiceList ) );
        list->DrawDeferred();

        SetAccountInUseL();
        SetMSKButtonL();
        }
    else // iProductIncludesSelectableEmail
        {
        CMceSettingsMtmServicesDialog* dlg =
        new( ELeave ) CMceSettingsMtmServicesDialog( iMessageType,
                                                     iManager,
                                                     iSession,
                                                     *this );
    CleanupStack::PushL( dlg );
    dlg->ConstructL( R_MCE_SETTINGS_MTM_SERVICES_DIALOG_MENUBAR );
    CleanupStack::Pop( dlg );

    CleanupStack::PushL( TCleanupItem( ResetMtmServicesDialog, this ) );
    iSubDialogOpen = ETrue;
    dlg->ExecuteLD( R_MCE_SETTINGS_MTM_SERVICES_DIALOG );
    CleanupStack::Pop(); //ResetMtmServicesDialog
    iSubDialogOpen = EFalse;
    
    if ( iSubDialogForceClosed == TRUE )
        {
        SetAccountInUseL();    

        CEikTextListBox* list=STATIC_CAST( CEikTextListBox*, 
                                                                    Control( EMceSettingsMultipleServicesDialogChoiceList ) );
        list->SetCurrentItemIndex( 0 );
        list->DrawDeferred();
        SetMSKButtonL();
        }
        }
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::HandleNoMailboxesL
// ----------------------------------------------------
TBool CMceSettingsMultipleServicesDialog::HandleNoMailboxesL()
    {
    //Deny creating new mail account if TARM is active.
    if( !iEmailApi->IsEmailFeatureSupportedL( 
        CImumInternalApi::EMailLockedSettings, R_QTN_SELEC_PROTECTED_SETTING ) )
        {
        CAknQueryDialog* confDialog = CAknQueryDialog::NewL();

        THumanReadableName mtmName = iManager.MtmName( iMessageType );
        
        if ( iMessageType == KSenduiMtmSyncMLEmailUid )
            {
            // syncML
            HBufC* text = StringLoader::LoadLC( 
            R_MCE_SETTINGS_SMAIL_NEW, iEikonEnv );               
            confDialog->SetPromptL( *text );
            CleanupStack::PopAndDestroy( text ); // text        
            }
        else if ( iMessageType != KSenduiMtmSmtpUid )
            {
            // not email/SyncML
            HBufC* text = StringLoader::LoadLC( 
            R_MCE_MTM_CREATE_NEW, mtmName, iEikonEnv );               
            confDialog->SetPromptL( *text );
            CleanupStack::PopAndDestroy( text ); // text
            }
        else
            {        
            // if KSenduiMtmSmtpUid, the confDialog text has been set in
            // R_MCE_SETTINGS_CREATE_NEW_MBOX_CONFIRMATION
            }

        if ( confDialog->ExecuteLD(
                R_MCE_SETTINGS_CREATE_NEW_MBOX_CONFIRMATION ) )
            {
            if( iMceIntegration )
                {
                // In MCE wizard integration's case this return value
                // is not used
                return LaunchWizardL();
                }
            else // iMceIntegration
                {
                return iManager.CreateNewAccountL(
                        iMessageType, KMsvNullIndexEntryId );
                } // iMceIntegration
            }
        }
    return EFalse;
    }
// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::CreateNewImapPopL
// ----------------------------------------------------
TBool CMceSettingsMultipleServicesDialog::CreateNewImapPopL()
    {
    TBool ret;
    if( iMceIntegration )
        {
        // In MCE wizard integration's case this return value is not used
        // just resetting this boolean to its original value
        ret = LaunchWizardL();     
        iS60WizLaunched = EFalse;  
        }
    else
        {
        ret = iManager.CreateNewAccountL(
                iMessageType, KMsvNullIndexEntryId );
        }
    return ret;
    }


// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::LaunchWizardL
// ----------------------------------------------------
TBool CMceSettingsMultipleServicesDialog::LaunchWizardL()
    {
    TBool ret;
    // launch S60 wizard
    iS60WizLaunched = ETrue;
    ret = iManager.CreateNewAccountL(
            iMessageType, KMsvNullIndexEntryId );
    return ret;
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::SetAccountInUseL
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::SetAccountInUseL()
    {
    TMsvId id = MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
        iManager.Session(),
        iMessageType,
        ETrue );

    if( iProductIncludesSelectableEmail )
        {
        // Get all healthy mailbox array, especially necessary after a new
        // account is created while there are invalid email accounts.
        if ( !iEmailApi )
            {
            iEmailApi = CreateEmailApiL( iSession );
            }

        const MImumInHealthServices* healthApi =
        &iEmailApi->HealthServicesL();

        TInt error;
        iMailboxArray.Reset();
        if ( iMessageType == KSenduiMtmSyncMLEmailUid )
            {
            error = MceSettingsUtils::GetHealthyMailboxListL( *healthApi,
                                                              iMailboxArray,
                                                              EFalse,
                                                              EFalse,
                                                              ETrue,EFalse );
            }
        else
            {
            error = MceSettingsUtils::GetHealthyMailboxListL( *healthApi,
                                                              iMailboxArray,
                                                              ETrue,
                                                              ETrue,
                                                              EFalse,ETrue );
            }

        CRepository* repository = NULL;
        TRAPD( ret, repository = CRepository::NewL(
                                 KCRUidSelectableDefaultEmailSettings  ) );
        CleanupStack::PushL( repository );
        TInt val;
        THumanReadableName mtmName;
        TMsvEntry entryMail;
        TBool nameGet = EFalse;
        TBool setDefault( EFalse );
        if ( ret == KErrNone && iMessageType != KSenduiMtmSyncMLEmailUid )
            {
            if ( repository->Get(
                    KSelectableDefaultMailAccount , val ) == KErrNone )
                {
                id = val;
                TMsvId serviceId;
                TInt err = iSession->GetEntry( id, serviceId, entryMail );
                CMtmUiDataRegistry* uiRegistry =
                    CMtmUiDataRegistry::NewL( *iSession );
                CleanupStack::PushL( uiRegistry );
                TUid mailMTMTechType =
                    uiRegistry->TechnologyTypeUid( KUidMsgTypePOP3 );

                // Need check if the mailbox is healthy.
                if ( ( healthApi->IsMailboxHealthy( entryMail.Id() ) || 
                     ( ( entryMail.iType == KUidMsvServiceEntry) && 
                       ( entryMail.Id() != KMsvLocalServiceIndexEntryId ) && 
                       ( !MuiuEmailTools::IsMailMtm(
                         entryMail.iMtm, ETrue ) ) ) )
                         && err == KErrNone
                         && ( entryMail.Visible() ||
                         uiRegistry->TechnologyTypeUid( entryMail.iMtm )
                         == mailMTMTechType ) )
                    {
                    mtmName.Append( entryMail.iDetails );
                    nameGet = ETrue;
                    }
                else
                    {
                    err = iSession->GetEntry(
                            entryMail.iRelatedId, serviceId, entryMail );
                    if ( ( healthApi->IsMailboxHealthy( entryMail.Id() ) || 
                         ( ( entryMail.iType == KUidMsvServiceEntry) && 
                           ( entryMail.Id() != KMsvLocalServiceIndexEntryId )
                             && ( !MuiuEmailTools::IsMailMtm(
                             entryMail.iMtm, ETrue ) ) ) )
                             && err == KErrNone 
                             && ( entryMail.Visible() ||
                             uiRegistry->TechnologyTypeUid( entryMail.iMtm )
                             == mailMTMTechType ) )
                        {
                        mtmName.Append( entryMail.iDetails );
                        nameGet = ETrue;
                        }
                    }
                CleanupStack::PopAndDestroy( uiRegistry );

                // If it cannot get the default account and there is a
                // healthy mailbox created by user, then the mailbox should
                // be set as default.
                if ( !nameGet && iMailboxArray.Count() != 0 )
                    {
                    id = iMailboxArray[0];
                    CMsvSession& session = iManager.Session();
                    TMsvEntry entry;
                    TMsvId serviceid;
                    if ( session.GetEntry( id, serviceid, entry ) == KErrNone
                            && ( entry.iMtm == KSenduiMtmImap4Uid
                            || entry.iMtm == KSenduiMtmPop3Uid 
                            || entry.iMtm == KSenduiMtmSmtpUid ) )
                        {
                        setDefault = ETrue;
                        mtmName.Append( entry.iDetails );
                        nameGet = ETrue;

                        TInt idTemp = entry.Id();
                        MsvUiServiceUtilitiesInternal::
                        SetDefaultServiceForMTML( session, entry.iMtm, id );

                        // set related service's default too...
                        if ( session.GetEntry(
                                entry.iRelatedId, serviceid, entry ) == KErrNone )
                            {
                            MsvUiServiceUtilitiesInternal::
                            SetDefaultServiceForMTML( session, entry.iMtm, entry.Id() );
                            }

                        // Set default mailbox.
                        repository->Set( KSelectableDefaultMailAccount ,idTemp );                    
                        }
                    }
                }
            }
        CleanupStack::PopAndDestroy( repository );
        iMailboxArray.Reset();

        HBufC* mailboxInUseListBoxText = NULL;
        TMsvEntry mailboxEntry;
        TMsvId serviceId; // not used but needed by GetEntry function

        if(!nameGet)
            {
            THumanReadableName mtmName = iManager.MtmName( iMessageType );
            }

        if ( id != KMsvUnknownServiceIndexEntryId
             && iManager.Session().GetEntry( id, serviceId, mailboxEntry )
             == KErrNone
             && ( healthApi->IsMailboxHealthy( mailboxEntry.Id() ) || 
             ( ( mailboxEntry.iType == KUidMsvServiceEntry ) && 
             ( mailboxEntry.Id() != KMsvLocalServiceIndexEntryId ) && 
             ( !MuiuEmailTools::IsMailMtm( mailboxEntry.iMtm, ETrue ) ) ) ) )
            {
            TInt mailboxNameLength;
            if(!nameGet)
                {
                mailboxNameLength = mailboxEntry.iDetails.Left(
                        KMceSettingsMailboxTextLength ).Length();
                }
            else
                {
                mailboxNameLength =	entryMail.iDetails.Left(
                        KMceSettingsMailboxTextLength ).Length();
                }

            HBufC* mailboxInUseDefaultText = NULL;

            if ( iMessageType == KSenduiMtmSmtpUid )
                {
                //remote mailboxes
                mailboxInUseDefaultText = StringLoader::LoadLC(
                                          R_MCE_SETTINGS_MAIL_DEFAULT, iEikonEnv );
                }
            else if ( iMessageType == KSenduiMtmSyncMLEmailUid )
                {
                //syncML remote mailboxes
                mailboxInUseDefaultText = StringLoader::LoadLC(
                                          R_MCE_SETTINGS_SYNCMAIL_INUSE, iEikonEnv );
                }
            else
                {
                mailboxInUseDefaultText = StringLoader::LoadLC(
                                          R_MCE_MTM_IN_USE, mtmName, iEikonEnv );
                }

            mailboxInUseListBoxText = HBufC::NewLC(
                mailboxInUseDefaultText->Length() + mailboxNameLength );

            mailboxInUseListBoxText->Des().Copy( *mailboxInUseDefaultText );
            if ( !nameGet )
                {
                mailboxInUseListBoxText->Des().Append(
                        mailboxEntry.iDetails.Left( mailboxNameLength ) );
                }
            else
                {
                if ( setDefault )
                    {
                    mailboxInUseListBoxText->Des().Append(
                            mtmName.Left( mailboxNameLength ) );
                    }
                else
                    {
                    mailboxInUseListBoxText->Des().Append(
                            entryMail.iDetails.Left( mailboxNameLength ) );
                    }
                }

            CleanupStack::Pop( mailboxInUseListBoxText );
            CleanupStack::PopAndDestroy( mailboxInUseDefaultText );
            }
        else
            {
            if ( iMessageType == KSenduiMtmSmtpUid )
                {
                //remote mailboxes
                mailboxInUseListBoxText = StringLoader::LoadL(
                                          R_MCE_SETTINGS_MAIL_DEFAULT_IN_USE_NONE, iEikonEnv );
                }
            else if ( iMessageType == KSenduiMtmSyncMLEmailUid )
                {
                //syncML remote mailboxes
                mailboxInUseListBoxText = StringLoader::LoadL(
                                          R_MCE_SETTINGS_SYNCMAIL_INUSE_NONE, iEikonEnv );
                }
            else
                {
                mailboxInUseListBoxText = StringLoader::LoadL( 
                                          R_MCE_MTM_IN_USE_NONE, mtmName, iEikonEnv );  
                }        
            }

        if ( iSettingsItemArray->Count() > 1 )
            {
            // delete dummy item because we are adding 'real' item below
            iSettingsItemArray->Delete(
                    KMceSettingsRemoteMailboxInUseSelectableEmailInUse );
            }

        CleanupStack::PushL( mailboxInUseListBoxText );
        iSettingsItemArray->InsertL( 0, *mailboxInUseListBoxText );
        CleanupStack::PopAndDestroy( mailboxInUseListBoxText );

        }
    else // iProductIncludesSelectableEmail
        {
        HBufC* mailboxInUseListBoxText = NULL;
    TMsvEntry mailboxEntry;
    TMsvId serviceId; // not used but needed by GetEntry function
    
    THumanReadableName mtmName = iManager.MtmName( iMessageType );

    if ( id != KMsvUnknownServiceIndexEntryId &&
         iManager.Session().GetEntry( id, serviceId, mailboxEntry ) == KErrNone )
        {
        const TInt mailboxNameLength =
            mailboxEntry.iDetails.Left( KMceSettingsMailboxTextLength ).Length();

        HBufC* mailboxInUseDefaultText = NULL;
        
        if ( iMessageType == KSenduiMtmSmtpUid )
            {
            //remote mailboxes
            mailboxInUseDefaultText = StringLoader::LoadLC(
                R_MCE_SETTINGS_MAIL_RM_IN_USE, iEikonEnv );            
            }
        else if ( iMessageType == KSenduiMtmSyncMLEmailUid )
            {
            //syncML remote mailboxes
            mailboxInUseDefaultText = StringLoader::LoadLC(
                R_MCE_SETTINGS_SYNCMAIL_INUSE, iEikonEnv );            
            }
        else
            {
            mailboxInUseDefaultText = StringLoader::LoadLC( 
                R_MCE_MTM_IN_USE, mtmName, iEikonEnv );            
            }

        mailboxInUseListBoxText = HBufC::NewLC(
            mailboxInUseDefaultText->Length() + mailboxNameLength );

        mailboxInUseListBoxText->Des().Copy( *mailboxInUseDefaultText );
        mailboxInUseListBoxText->Des().Append(
            mailboxEntry.iDetails.Left( mailboxNameLength ) );

        CleanupStack::Pop( mailboxInUseListBoxText );
        CleanupStack::PopAndDestroy( mailboxInUseDefaultText );
        }
    else
        {
        if ( iMessageType == KSenduiMtmSmtpUid )
            {
            //remote mailboxes
            mailboxInUseListBoxText = StringLoader::LoadL( 
                                                            R_MCE_SETTINGS_MAIL_RM_IN_USE_NONE, 
                                                            iEikonEnv );
            }
        else if ( iMessageType == KSenduiMtmSyncMLEmailUid )
            {
            //syncML remote mailboxes
            mailboxInUseListBoxText = StringLoader::LoadL( 
                                                            R_MCE_SETTINGS_SYNCMAIL_INUSE_NONE, 
                                                            iEikonEnv );
            }
        else
            {
            mailboxInUseListBoxText = StringLoader::LoadL( 
                R_MCE_MTM_IN_USE_NONE, mtmName, iEikonEnv );  
            }        
        }

    if ( iSettingsItemArray->Count() > 1 )
        {
        // delete dummy item because we are adding 'real' item below
        iSettingsItemArray->Delete( KMceSettingsRemoteMailboxInUse );
        }

    CleanupStack::PushL( mailboxInUseListBoxText );
    iSettingsItemArray->AppendL( *mailboxInUseListBoxText );
    CleanupStack::PopAndDestroy( mailboxInUseListBoxText );
        }
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::HandleSessionEventL
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/)
    {
    switch ( aEvent )
        {
        case EMsvEntriesCreated:
            if ( KMsvRootIndexEntryId == (*(TMsvId*) (aArg2)) )
                {                
                HandleEntriesCreatedL( (CMsvEntrySelection*) aArg1 );               
				if( iProductIncludesSelectableEmail )
					{
                	UpdateServicesArrayL();
					}
                }
            break;
        case EMsvEntriesChanged:
        case EMsvEntriesDeleted:
        case EMsvMediaAvailable:
            UpdateMailboxDataL();           
        default:
            // just ignore
            break;
        }
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::HandleEntriesCreatedL
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::HandleEntriesCreatedL( CMsvEntrySelection* aSelection )
    {     
    UpdateMailboxDataL();   
    if ( aSelection->Count() )
        {
        TMsvEntry currentEntry;
        TMsvId serviceId; // not used here but needed by GetEntry function
        if ( iManager.Session().GetEntry( aSelection->At( 0 ), serviceId, currentEntry ) == KErrNone
                && currentEntry.iType == KUidMsvServiceEntry 
                && currentEntry.iMtm == iMessageType )
            {
            SetAccountInUseL();
            }
	CEikTextListBox* list = STATIC_CAST( CEikTextListBox*, 
	                                 Control( EMceSettingsMultipleServicesDialogChoiceList ) );
	list->DrawDeferred();
        }
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::HandleListBoxEventL
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    CEikTextListBox* list = STATIC_CAST( CEikTextListBox*, 
                                        Control( EMceSettingsMultipleServicesDialogChoiceList ) );
    switch( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
			if ( iProductIncludesSelectableEmail )
				{
	            if ( list->CurrentItemIndex() == 0 )
    	            {
                	ProcessCommandL( EMceSettingsCmdSettingsDialogChange );                
                	}
            	else
                	{
					ProcessCommandL( EMceSettingsCmdSettingsDialogOpen );
                	}
                }
            else // iProductIncludesSelectableEmail
            	{
	            if ( list->CurrentItemIndex() == 0 )
					{
                	ProcessCommandL( EMceSettingsCmdSettingsDialogOpen );
                	}
            	else
                	{
                	ProcessCommandL( EMceSettingsCmdSettingsDialogChange );
                	}
                }
        break;
        case EEventItemDraggingActioned:
            SetMSKButtonL();
        break;
        default:
        break;
        }
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::GetHelpContext
// returns helpcontext as aContext
//
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::GetHelpContext
        ( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KMceApplicationUid;
	if ( iProductIncludesSelectableEmail )
	    {
	    CEikTextListBox* list = STATIC_CAST(
	            CEikTextListBox*,
	            Control( EMceSettingsMultipleServicesDialogChoiceList ) );       

	    if ( list->CurrentItemIndex() ==
	    KMceSettingsRemoteMailboxInUseSelectableEmailInUse )
	        {
	        aContext.iContext = KES_HLP_EMAIL_SETTING;
	        }
	    else
	        {
	        // start        	
	        TUid mtmType;
	        TInt item = list->CurrentItemIndex();      	        		
	        CMsvEntry* entry = NULL;

	        TRAPD( error, entry = iSession->GetEntryL(
	                (*iAccountArray)[item-1].iUid.iUid )); 

	        if ( entry )
	            {
	            TMsvEntry tentry = entry->Entry();
	            mtmType = tentry.iMtm;
	            delete entry;            		

	            if ( ( mtmType != KSenduiMtmImap4Uid ) &&
	                    ( mtmType != KSenduiMtmPop3Uid ) )
	                {
	                aContext.iContext = KES_HLP_THIRDPARTY_MAILBOX;
	                }
	            else
	                {
	                aContext.iContext = KES_HLP_SMTP_MAILBOX;
	                }						              		
	            }
	        }
	    }
	else // iProductIncludesSelectableEmail
	    { 
    if ( iMessageType == KSenduiMtmSyncMLEmailUid )
        {
        aContext.iContext = KMCE_HLP_SETT_SMAILBOX/*KMCE_HLP_SETTINGS_SMAIL*/;
        }
    else
        {
        aContext.iContext = KMCE_HLP_SETTINGS_EMAIL;
        } 
    }
	}

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::LaunchHelpL
// Launch help using context
// 
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::LaunchHelpL() const
    { 
    CCoeAppUi* appUi = STATIC_CAST( CCoeAppUi*, ControlEnv()->AppUi() );       
    CArrayFix<TCoeHelpContext>* helpContext = appUi->AppHelpContextL();   
    HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
    }


// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::ResetMtmServicesDialog
// Sets the flag to indicate that the MTM services subdialog
// is not open
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::ResetMtmServicesDialog( TAny* aAny )
    {
    CMceSettingsMultipleServicesDialog* serviceDialog = ( CMceSettingsMultipleServicesDialog*)aAny;
    serviceDialog->iSubDialogOpen = EFalse;
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::OfferKeyEventL
// Handles this setting dialog view's arrow keys.
// Forwards other keys.
//
// ----------------------------------------------------
TKeyResponse CMceSettingsMultipleServicesDialog::OfferKeyEventL(
                                                const TKeyEvent& aKeyEvent,
                                                TEventCode aType )
    {
    if ( aType == EEventKey &&
         ( aKeyEvent.iCode==EKeyUpArrow || aKeyEvent.iCode==EKeyDownArrow ) )
        {
        CEikTextListBox* list=STATIC_CAST(
                        CEikTextListBox*,
                        Control( EMceSettingsMultipleServicesDialogChoiceList ) );
        list->OfferKeyEventL( aKeyEvent, aType );        
        SetMSKButtonL();
        return EKeyWasConsumed;
        }    
    return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::SetMSKButtonL
// Sets the MSK for the view
//
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::SetMSKButtonL()
    {
    CEikTextListBox* list=STATIC_CAST(
                    CEikTextListBox*,
                    Control( EMceSettingsMultipleServicesDialogChoiceList ) );
    const TInt index = list->CurrentItemIndex();
    const TInt numberOfItem = list->Model()->NumberOfItems();
    TInt resourceId;
	if ( iProductIncludesSelectableEmail )
		{
		 resourceId =
	              index==0 ? R_MCE_MSK_BUTTON_CHANGE : R_MCE_MSK_BUTTON_OPEN;
		}
	else
		{
		resourceId =
                  index==0 ? R_MCE_MSK_BUTTON_OPEN : R_MCE_MSK_BUTTON_CHANGE;
		}
    CEikButtonGroupContainer& cba = ButtonGroupContainer();
    cba.SetCommandL( KMSKPosition, resourceId );
    cba.DrawNow();
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::SubdialogClosed
// Informs that subdialog has been closed
//
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::SubdialogClosed()
    {
    iSubDialogOpen = EFalse;
    iSubDialogForceClosed = EFalse;
    }

// ----------------------------------------------------
CMsvEntrySelection* CMceSettingsMultipleServicesDialog::GetEmailAccountsL(
		CMsvSession& aSession )
    {
    CMsvEntrySelection* sel = new( ELeave ) CMsvEntrySelection();
    CleanupStack::PushL( sel );
    CMsvEntry* entry = aSession.GetEntryL( KMsvRootIndexEntryIdValue );
    CleanupStack::PushL( entry );
    if ( iMceIntegration )
    	{
    TInt cnt = entry->Count();
    if ( cnt != 0 )
        {
        entry->SetSortTypeL( TMsvSelectionOrdering(
        		                KMsvGroupByType | KMsvGroupByStandardFolders,
        		                EMsvSortByDetailsReverse, ETrue ) );
        const TMsvEntry* tentry;
        for ( TInt cc = entry->Count(); --cc >= 0; )
           {
            tentry = &(*entry)[cc];
                
            if ( tentry->iType.iUid == KUidMsvServiceEntryValue &&
            	 tentry->Id() != KMsvLocalServiceIndexEntryIdValue )
                {
                const TBool noRelatedId =
                                tentry->iRelatedId == KMsvNullIndexEntryId ||
                                tentry->iRelatedId == tentry->Id();
                
                TBool appendEmail = EFalse;
                
                if ( tentry->Visible() )
                    {
                    appendEmail = ETrue;
                    }
                else
                    {
                    CMtmUiDataRegistry* uiRegistry =
                                       CMtmUiDataRegistry::NewL( *iSession );
                    CleanupStack::PushL(uiRegistry);             
                    if ( uiRegistry->IsPresent( tentry->iMtm ) &&
                    	 uiRegistry->IsPresent( KUidMsgTypePOP3 ) )
                        {            
                        if( uiRegistry->TechnologyTypeUid( tentry->iMtm ) ==
                                                        KSenduiTechnologyMailUid  )
                            {
                            appendEmail = ETrue;                    
                            }
                        }
                    CleanupStack::PopAndDestroy( uiRegistry );
                    }                    
                
                if ( appendEmail )
                    {
                    // Add this service if:
                    // it is visible, or,there is no associated related
                    // service, or, we have been asked to list all services.
                    sel->AppendL( tentry->Id() );
    					}
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( entry );
    CleanupStack::Pop( sel );
    return sel;
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::MtmAccountsL
// ----------------------------------------------------
CUidNameArray* CMceSettingsMultipleServicesDialog::MtmEmailAccountsL()
    {
    CUidNameArray* accounts = new (ELeave) CUidNameArray(
                              KMceSettingsArrayGranularity );
    CleanupStack::PushL( accounts );

    CMsvEntrySelection* sel = NULL;

	sel = GetEmailAccountsL( *iSession );

    CleanupStack::PushL( sel );

    CMsvEntry* rootEntry=CMsvEntry::NewL(
        *iSession,
        KMsvRootIndexEntryId,
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
	CleanupStack::PushL( rootEntry );

	if ( !iEmailApi )
        {
        iEmailApi = CreateEmailApiL( iSession );
        }

   	const MImumInHealthServices* healthApi = &iEmailApi->HealthServicesL();

   	TInt error = MceSettingsUtils::GetHealthyMailboxListL(
   			                      *healthApi,
        	                      iMailboxArray,
								  iMessageType != KSenduiMtmSyncMLEmailUid,
        						  iMessageType != KSenduiMtmSyncMLEmailUid,
        				          iMessageType == KSenduiMtmSyncMLEmailUid,ETrue );
    if ( error == KErrNone &&
            ( iMtmPluginId == 0 || iMessageType == KSenduiMtmSyncMLEmailUid ) )
        {
        const TInt mailboxArrayCount = iMailboxArray.Count();
        TBool foundServiceArray = EFalse;
        TMsvId msvId;

        for ( TInt cc = 0; cc < mailboxArrayCount; cc++)
            {
            msvId = iMailboxArray[cc];

            TMsvEntry tentry2;
            TRAPD( err2, ( tentry2 = rootEntry->ChildDataL( msvId ) ) );

            const TInt count = accounts->Count();
            TBool foundService = EFalse;
            
            if ( err2 == KErrNone )
                {
                
                //check that related service is not already added to array
                for ( TInt loop = 0; loop < count; loop ++ )
                    {
                    if ( tentry2.iRelatedId == iMailboxArray[loop] )
                        {
                        foundService = ETrue;
                        break;
                        }
                    }
                if ( !foundService )
                    {               
                    TUid uid;
                    uid.iUid = tentry2.Id();
                    TUidNameInfo info( uid, tentry2.iDetails.Left(
                    		               KMceSettingsMailboxTextLength ) );
                    accounts->AppendL( info );
                    }    
            
                }
            }
        }
    TUid uid;
	CMtmUiDataRegistry* uiRegistry = CMtmUiDataRegistry::NewL( *iSession );
    CleanupStack::PushL( uiRegistry );
    const TInt numAccounts = sel->Count();
    for ( TInt cc = 0; cc < numAccounts; cc++)
        {
        uid.iUid = sel->At( cc );
        TMsvEntry tentry;
        TRAPD( err, ( tentry = rootEntry->ChildDataL( uid.iUid ) ) );
        
        TBool appendEmail = EFalse;
        
        // This avoids FS boxes to show in SyncML list
        if ( iMessageType != KSenduiMtmSyncMLEmailUid )
            {
            if ( tentry.Visible() )
                {
                appendEmail = ETrue;
                }
            else
                {                
                if ( uiRegistry->IsPresent( tentry.iMtm ) &&
                     uiRegistry->IsPresent( KUidMsgTypePOP3 ) )
                    {            
                    TUid mailMTMTechType =
                        uiRegistry->TechnologyTypeUid( KUidMsgTypePOP3 );                                          
                    if( uiRegistry->TechnologyTypeUid( tentry.iMtm ) ==
                        mailMTMTechType )
                        {
                        appendEmail = ETrue;                    
                        }
                    }
                }            

            // Remove 3'rd party mailboxes which are not registered to handle
            // S60 mailboxes 
            if ( iMtmPluginId != 0 &&
                    tentry.iMtm.iUid != iMtmPluginId )
                {
                appendEmail = EFalse;
                }
            } // != KSenduiMtmSyncMLEmailUid

        if ( err == KErrNone &&
                uiRegistry->IsPresent( tentry.iMtm ) && appendEmail )
            {
            const TInt count = accounts->Count();
            TBool foundService = EFalse;
            //check that related service is not already added to array
            for ( TInt loop = 0; loop < count; loop ++ )
                {
                if ( tentry.iRelatedId == (*accounts)[loop].iUid.iUid )
                    {
                    foundService = ETrue;
                    break;
                    }
                }
            if ( !foundService )
                {
                TUidNameInfo info( uid, tentry.iDetails.Left(
                        KMceSettingsMailboxTextLength ) );
                accounts->AppendL( info );
                }
            }
        }
    CleanupStack::PopAndDestroy( uiRegistry );
    CleanupStack::PopAndDestroy( rootEntry );
    CleanupStack::PopAndDestroy( sel );
    CleanupStack::Pop( accounts );
    return accounts;
    }

// ----------------------------------------------------
// CMceSettingsMultipleServicesDialog::UpdateForArrayChangeL
// ----------------------------------------------------
void CMceSettingsMultipleServicesDialog::UpdateServicesArrayL()
    {        
    CEikTextListBox* list = STATIC_CAST( CEikTextListBox*, 
        Control( EMceSettingsMultipleServicesDialogChoiceList ) );

    TInt selectedIndex = list->CurrentItemIndex();
    const TInt count = iAccountArray->Count();
    CUidNameArray* sel = MtmEmailAccountsL();
    CleanupStack::PushL( sel );
    const TInt newCount = sel->Count();
    iAccountArray->Reset();
	iSettingsItemArray->Reset();
	SetAccountInUseL();
	TBuf <KMceSettingsMtmNameBufLength> mtms;
    
	for ( TInt loop = 0; loop < newCount; loop++ )
        {
        iAccountArray->AppendL( sel->At( loop ) );


		mtms.Copy( KMceMtmName1 );
		mtms.Append( (*sel)[loop].iName );
		mtms.Append( KMceMtmName2 );
		iSettingsItemArray->AppendL( mtms );
		mtms.Zero();
        }
    CleanupStack::PopAndDestroy( sel );

    if ( newCount > count )
        {
        list->HandleItemAdditionL();
        }
    else if ( newCount < count )
        {
        list->HandleItemRemovalL();
        }
    else
        {
        // no action
        }

    if ( newCount > 0 )
        {
        if ( selectedIndex >= 0 && selectedIndex < newCount )
            {
            list->SetCurrentItemIndex( selectedIndex );
            }
        else
            {
            list->SetCurrentItemIndex( 0/*newCount - 1*/ );
            }
        }
    list->DrawNow();
    }

//  End of File
