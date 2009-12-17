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
*     Message centre's general settings dialog.
*
*/



// INCLUDE FILES

#include <aknViewAppUi.h>
#include <AknDlgShut.h>
#include <muiusettingsarray.h>  // CMuiuSettingsArray

#include <centralrepository.h>
#include <messaginginternalcrkeys.h>
#include <e32property.h>
#include <messaginginternalpskeys.h>
#include <e32capability.h>

#include "MceSettingsGeneralSettingsDialog.h"
#include "MceSettingsMoveProgress.h"    // CMceMoveProgress
#include "MceSettingsIds.hrh"
#include "MceSettingsAccountManager.h"
#include <MceSettings.rsg>

#include <CAknMemorySelectionSettingItem.h> // CAknMemorySelectionSettingPage
#include <msvuids.h>            // KUidMsvServiceEntry
#include <akninputblock.h>      // CAknInputBlock
#include <aknnotewrappers.h>    // CAknErrorNote
#include <bautils.h>            // BaflUtils

#include <MuiuOperationWait.h>  // CMuiuOperationWait
#include <sysutil.h>            // Sysutil

#include <csxhelp/mce.hlp.hrh>
#include <featmgr.h>
#include <hlplch.h>             // For HlpLauncher
#include "MceSettingsUtils.h"

#ifdef RD_MULTIPLE_DRIVE
#include <AknCommonDialogsDynMem.h> // Media type
#include <CAknMemorySelectionDialogMultiDrive.h> // CAknMemorySelectionDialogMultiDrive
#include <driveinfo.h>          // DriveInfo
#endif //RD_MULTIPLE_DRIVE
#include <e32cmn.h>             // RBuf

#include <bldvariant.hrh>
#include <data_caging_path_literals.hrh>

// CONSTANTS

#define KMceApplicationUidValue         0x100058C5
const TUid KMceApplicationUid           = {KMceApplicationUidValue};

const TInt KMceDefaultSentItemsCount    = 15;
const TInt KMceDefaultSentItemsOn       = 0; // first item in list array is 'On'
const TInt KMceDefaultSentItemsOff      = 1; // second item in list array is 'Off'
const TInt KMceDefaultSentItems         = 1; // == On

const TInt KMceEmptyMessageStore        = 40000; // size of empty message store
const TInt KMceEditorsWaitInterval      = 4000000; // wait one second,that message editors close

const TInt KMSKPosition = 3;

const TInt KMceListTypeTwoRow       = 0;
const TInt KMceListTypeOneRow       = 1;

#ifdef _DEBUG
_LIT( KPanicText, "MceSettings" );
const TInt KCRepositorySettingFailure = 1;
#endif
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
_LIT( KMceDirAndFile,"MceSettings.rsc" );
#endif // RD_MESSAGING_GENERAL_SETTINGS_RENOVATION

// MODULE DATA STRUCTURES
enum TMceGeneralSettings
    {
    EMceGeneralSettingsSentItemsInUse,
    EMceGeneralSettingsSentItemsCount,
    EMceGeneralSettingsMemoryInUse,
    EMceGeneralSettingsInboxListType,
    EMceGeneralSettingsMailListType
    };
#ifdef RD_MULTIPLE_DRIVE
const TInt KSpaceDelimiter=' ';
#endif //RD_MULTIPLE_DRIVE

// ================= MEMBER FUNCTIONS =======================

EXPORT_C TInt CMceGeneralSettingsDialog::CreateAndExecuteL(
    CMsvSession* aSession, MMsvSessionObserver& aObserver )
    {
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    // general settings are list item in the list
    CMceGeneralSettingsDialog* generalSettingsDlg =
        new (ELeave) CMceGeneralSettingsDialog( aSession, aObserver, NULL );
    CleanupStack::PushL( generalSettingsDlg );
    generalSettingsDlg->ConstructL( R_MCE_GENERAL_SETTINGS_CHOICE_MENUBAR );
    CleanupStack::Pop( generalSettingsDlg ); 
    return generalSettingsDlg->ExecuteLD( R_MCE_GENERAL_SETTINGS );
#else
    User::Leave( KErrNotSupported );
    return KErrNotSupported;
#endif // RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    }

// C++ default constructor can NOT contain any code that
// might leave.
// ----------------------------------------------------
// CMceGeneralSettingsDialog::Constructor
// ----------------------------------------------------
CMceGeneralSettingsDialog::CMceGeneralSettingsDialog( 
    CMsvSession* aSession, 
    MMsvSessionObserver& aObserver, 
    MMceSettingsAccountManager* aManager )
    :
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    CMceSettingsTitlePaneHandlerDialog(),
    iSession( aSession ), iObserver( aObserver ),
    iResources( *CCoeEnv::Static() ), iAccountManager( aManager )
#else
    CMceSettingsTitlePaneHandlerDialog(),
    iSession( aSession ), iObserver(aObserver ), iAccountManager( aManager )
#endif
    {
    }


void CMceGeneralSettingsDialog::ConstructL( TInt aResource )
    {
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    TParse fp;
    fp.Set( KMceDirAndFile, &KDC_RESOURCE_FILES_DIR , NULL );
    TFileName fileName = fp.FullName();

    iResources.OpenL( fileName );
#endif
    CMceSettingsTitlePaneHandlerDialog::ConstructL( aResource );
    }

// ----------------------------------------------------
// CMceGeneralSettingsDialog::Destructor
// ----------------------------------------------------
CMceGeneralSettingsDialog::~CMceGeneralSettingsDialog()
    {    
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    iResources.Close();
#endif // RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
    // don't delete iAccountManager
    }

// ----------------------------------------------------
// CMceGeneralSettingsDialog::OkToExitL
// ----------------------------------------------------
TInt CMceGeneralSettingsDialog::OkToExitL( TInt aButtonId )
    {
    TBool okToExit = CAknDialog::OkToExitL( aButtonId );
    if ( aButtonId == EAknSoftkeyBack || okToExit )
        {
        CRepository* repository = NULL;
        TRAPD( ret, repository = CRepository::NewL( KCRUidMuiuSettings ) );
        CleanupStack::PushL( repository );

        if ( ret == KErrNone )
            {
            ret = repository->Set( KMuiuSentItemsInUse, 
            ( *iMceSettingsArray )[EMceGeneralSettingsSentItemsInUse].iCurrentNumber == KMceDefaultSentItemsOn );
            __ASSERT_DEBUG( !ret, User::Panic( KPanicText,KCRepositorySettingFailure ) );
#ifdef _DEBUG
            TInt setValue = ( *iMceSettingsArray )[EMceGeneralSettingsSentItemsInUse].iCurrentNumber;
            setValue = ( setValue == KMceDefaultSentItemsOn );
#endif

            ret = repository->Set( KMuiuSentItemsCount,
                ( *iMceSettingsArray )[EMceGeneralSettingsSentItemsCount].iCurrentNumber );
            __ASSERT_DEBUG( !ret, User::Panic(KPanicText,KCRepositorySettingFailure) );
#ifdef _DEBUG
            setValue = ( *iMceSettingsArray )[EMceGeneralSettingsSentItemsCount].iCurrentNumber;
#endif

            ret = repository->Set( KMuiuInboxMessageListType,
                ( *iMceSettingsArray )[EMceGeneralSettingsInboxListType].iCurrentNumber );
            __ASSERT_DEBUG( !ret, User::Panic(KPanicText,KCRepositorySettingFailure) );            

            ret = repository->Set( KMuiuMailMessageListType,
                ( *iMceSettingsArray )[EMceGeneralSettingsMailListType].iCurrentNumber );
            __ASSERT_DEBUG( !ret, User::Panic(KPanicText,KCRepositorySettingFailure) );            
            }
        CleanupStack::Pop( repository );
        delete repository;
        }

    if ( aButtonId == EMceSettingsCmdSettingsDialogOpen || 
          aButtonId == EMceSettingsCmdSettingsDialogChange )
        {
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
// CMceGeneralSettingsDialog::HandleListBoxEventL
// ----------------------------------------------------
void CMceGeneralSettingsDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {    
    CEikTextListBox* list=STATIC_CAST( CEikTextListBox*, 
        Control( EMceSettingsIdOtherItems ) );
    switch(aEventType)
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            {            
            TInt item = list->CurrentItemIndex();            
            if ( iMemoryInUse && item == EMceGeneralSettingsMemoryInUse )               
                {
                HandleMemoryInUseDialogL();
                }
            else
                {
                iMceSettingsArray->EditItemL( item, ETrue );               
                list->DrawNow();
                }
            }
            break;
            
        default:
            break;
        }
    }

// ----------------------------------------------------
// CMceGeneralSettingsDialog::ProcessCommandL
// ----------------------------------------------------
void CMceGeneralSettingsDialog::ProcessCommandL( TInt aCommandId )
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

    
    CEikTextListBox* list=STATIC_CAST( CEikTextListBox*, 
        Control( EMceSettingsIdOtherItems ) );

    switch ( aCommandId )
        {
        case EMceSettingsCmdSettingsDialogOpen:
            {            
            TInt item = list->CurrentItemIndex();
            if ( iMemoryInUse && item == EMceGeneralSettingsMemoryInUse )               
                {
                HandleMemoryInUseDialogL();
                }
            else
                {
                iMceSettingsArray->EditItemL( item, EFalse );                
                list->DrawNow();
                }       
            }
            break;           

        case EMceSettingsCmdSettingsDialogHelp:
                {
                LaunchHelpL();                
                }
                break;           
        
        case EMceSettingsCmdSettingsDialogChange:
                {
                TInt item = list->CurrentItemIndex();
            if ( iMemoryInUse && item == EMceGeneralSettingsMemoryInUse )               
                {
                HandleMemoryInUseDialogL();
                }
            else
                {
                iMceSettingsArray->EditItemL( item,  ETrue );                
                list->DrawNow();
                }                        
            }
            break; 
        default :
            break;
        }
    }


// ----------------------------------------------------
// CMceGeneralSettingsDialog::PreLayoutDynInitL
// ----------------------------------------------------
void CMceGeneralSettingsDialog::PreLayoutDynInitL( )
    {
    //check if 'Memory in use' -option is enabled in mce settings
    iMemoryInUse = ETrue;
    iMemoryInUse = MceSettingsUtils::MemoryInUseOptionL();

    CEikTextListBox* list=STATIC_CAST( CEikTextListBox*, 
        Control( EMceSettingsIdOtherItems ) );

    // Setting scrollbar-indicators on
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    if ( iMemoryInUse )
        {
        iMceSettingsArray = CMuiuSettingsArray::NewL( R_MCE_GENERAL_MMC );
        }
    else
        {
        iMceSettingsArray = CMuiuSettingsArray::NewL( R_MCE_GENERAL_SETTINGS_ITEMS );
        } 

    list->SetListBoxObserver( this );

    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL( KCRUidMuiuSettings ) );
    CleanupStack::PushL( repository );

    if ( ret == KErrNone )
        {
        TInt i;
        if ( repository->Get( KMuiuSentItemsInUse, i ) != KErrNone )
            {
            i = KMceDefaultSentItems;
            }
        ( *iMceSettingsArray )[EMceGeneralSettingsSentItemsInUse].iCurrentNumber =
                i ? KMceDefaultSentItemsOn : KMceDefaultSentItemsOff;
 
        if ( repository->Get(KMuiuSentItemsCount, i ) != KErrNone )
            {
            i = KMceDefaultSentItemsCount;
            }
        ( *iMceSettingsArray )[EMceGeneralSettingsSentItemsCount].iCurrentNumber = i;
        ( *iMceSettingsArray )[EMceGeneralSettingsSentItemsCount].iUserText.Num( i );
        
        // Get one/two row list type selection
        if ( repository->Get( KMuiuInboxMessageListType, i ) != KErrNone )
            {
            i = KMceListTypeTwoRow;
            }
        ( *iMceSettingsArray )[EMceGeneralSettingsInboxListType].iCurrentNumber =
                i ? KMceListTypeOneRow : KMceListTypeTwoRow;

        // Get one/two row list type selection
        if ( repository->Get( KMuiuMailMessageListType, i ) != KErrNone )
            {
            i = KMceListTypeOneRow;
            }
        ( *iMceSettingsArray )[EMceGeneralSettingsMailListType].iCurrentNumber =
                i ? KMceListTypeOneRow : KMceListTypeTwoRow;
        }

    if ( iMemoryInUse )
        {
        UpdateMemorySelectionL();
        }

    CleanupStack::Pop( repository ); 
    delete repository;

    CTextListBoxModel* model=list->Model();
    model->SetItemTextArray( iMceSettingsArray );

    StoreTitleTextAndSetNewL( R_MCE_SETTINGS_TITLE_GENERAL );
    SetMSKButtonL();
    }

//  ----------------------------------------------------
// CMceGeneralSettingsDialog::SetTransferError
// Catches the error when failed moving message store
//  ----------------------------------------------------
void CMceGeneralSettingsDialog::SetTransferError( TInt aError )
    {
    iTransferError = aError;
    }    

// ----------------------------------------------------
// CMceGeneralSettingsDialog::MoveMessageStoreL
// starts copying/moving message store
// if move, aDeleteCopiedStore is ETrue, if copy EFalse
// ----------------------------------------------------
void CMceGeneralSettingsDialog::MoveMessageStoreL( TInt currentDrive, 
         TInt driveNumber, TBool aDeleteCopiedStore )
    {    
    if( !CheckIsServerBusyL() )
        {
        CAknInputBlock* comAbs=CAknInputBlock::NewLC();  
        iTransferError  = KErrNone;
        CMceMoveProgress* moveProgress=CMceMoveProgress::NewL(
            iSession, iObserver, currentDrive, driveNumber, aDeleteCopiedStore,*this, EFalse );
           
        CleanupStack::PushL( moveProgress );
        if ( iAccountManager )
            {
            iAccountManager->SetChangeMessageStore( EFalse );
            }

        moveProgress->InitializingL();
        
        // KErrAccessDenied: when MC is read-only
        // preventing messaging close. Todo: anyother erro cases?       
        if ( iTransferError != KErrAccessDenied && iAccountManager )
            {
            iAccountManager->SetChangeMessageStore( ETrue );
            }

        // cannot delete CMceMoveProgress, 
        // because CMceMessageTransHandler is cancelled-> has to be deleted in CMceMoveProgress
        CleanupStack::Pop( moveProgress ); 
        CleanupStack::PopAndDestroy( comAbs );
        }
    }


// ----------------------------------------------------
// CMceGeneralSettingsDialog::CheckIsServerBusyL
// checks, if connection is on
// return ETrue, if connection is on
// ----------------------------------------------------
TBool CMceGeneralSettingsDialog::CheckIsServerBusyL()
    {
    TInt outstanding=iSession->OutstandingOperationsL();

    if (outstanding==0)
        {
        // check to see if any mailboxes are open - otherwise we could miss client side operations
        CMsvEntry* root=CMsvEntry::NewL( *iSession, 
                     KMsvRootIndexEntryIdValue, 
                     TMsvSelectionOrdering(KMsvNoGrouping, EMsvSortByNone, ETrue ) );
        CleanupStack::PushL( root );
        CMsvEntrySelection* sel=root->ChildrenWithTypeL( KUidMsvServiceEntry );
        CleanupStack::PushL( sel );
        const TMsvEntry* tentry=NULL;
        for ( TInt cc=sel->Count(); --cc>=0 && !outstanding ;)
            {
            tentry=&( root->ChildDataL( ( *sel )[cc] ) );
            if ( tentry->Connected() )
                {
                outstanding++;
                }
            }

        CleanupStack::PopAndDestroy( sel );
        CleanupStack::PopAndDestroy( root );
        }

    if (outstanding>0)
        {
        HBufC* text = StringLoader::LoadLC( R_MCE_SETTINGS_SERVER_BUSY, CCoeEnv::Static() );
        CAknErrorNote* note = new ( ELeave ) CAknErrorNote();
        note->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text ); 
        return ETrue;
        }

    return EFalse;
    }

// ----------------------------------------------------
// CMceGeneralSettingsDialog::FindSourcePathL
// gives message store path in aPath, when drive number is got
//
// ----------------------------------------------------
void CMceGeneralSettingsDialog::FindSourcePathL( TInt aDrive, TFileName& aPath ) const
    {
    GetMessageDriveL( aDrive, aPath );
    TParsePtrC pptr( KMsvDefaultFolder );
    aPath.Append( pptr.Path() );
    }

// ----------------------------------------------------
// CMceGeneralSettingsDialog::GetMessageDriveL
// gives drive letter and drive delimiter in aPath,
// when drive number is got
// ----------------------------------------------------
void CMceGeneralSettingsDialog::GetMessageDriveL( TInt aDrive, TDes& aPath ) const
    {
    TChar driveLetter;
    User::LeaveIfError( iEikonEnv->FsSession().DriveToChar( aDrive, driveLetter ) );
    aPath.Zero();
    aPath.Append( driveLetter );
    aPath.Append( TChar( KDriveDelimiter ) );
    }

// ----------------------------------------------------
// CMceGeneralSettingsDialog::UpdateMemorySelectionL
// sets correct string in General settings memory in use -option
//
// ----------------------------------------------------
void CMceGeneralSettingsDialog::UpdateMemorySelectionL()
    {
    RFs& fs=iEikonEnv->FsSession();    

#ifndef RD_MULTIPLE_DRIVE
    if ( TInt( iSession->CurrentDriveL() ) == EDriveC )
        {
        HBufC* text = StringLoader::LoadLC( R_MCE_SETTINGS_PHONE_MEMORY, iEikonEnv );

        ( *iMceSettingsArray )[EMceGeneralSettingsMemoryInUse].iUserText.Copy( *text );
        CleanupStack::PopAndDestroy( text );  
        }
    else
        { 
        TVolumeInfo volumeinfo;
        fs.Volume( volumeinfo, TInt(iSession->CurrentDriveL() ) );
        if( volumeinfo.iName.Length() > 0 ) // Volume has a label
           { // Use volume label as memory card name
           ( *iMceSettingsArray )[EMceGeneralSettingsMemoryInUse].iUserText.Copy( volumeinfo.iName );
           }
        else
           {
           HBufC* text = StringLoader::LoadLC( R_MCE_SETTINGS_MEMORY_CARD, iEikonEnv );
           ( *iMceSettingsArray )[EMceGeneralSettingsMemoryInUse].iUserText.Copy( *text );
           CleanupStack::PopAndDestroy( text );
           }
        }

#else
    TInt currentDrive = TInt( iSession->CurrentDriveL() );
    
    TVolumeInfo volumeinfo;
    fs.Volume( volumeinfo, TInt( currentDrive ) );
    if( volumeinfo.iName.Length() > 0 ) // Volume has a label
        { // Use volume label as memory card name
        TDriveName driveName(TDriveUnit( currentDrive ).Name() );
        RBuf newtxt;
        newtxt.CreateL( driveName.Length() + volumeinfo.iName.Length() + 1 ); // 1 for space
        CleanupClosePushL( newtxt );
        
        newtxt.Append( driveName );
        newtxt.Append( KSpaceDelimiter );
        newtxt.Append( volumeinfo.iName );        
              
        ( *iMceSettingsArray )[EMceGeneralSettingsMemoryInUse].iUserText.Copy( newtxt );        
        CleanupStack::PopAndDestroy( &newtxt );
        }
    else
        {
        HBufC *driveNameText = MakeDriveNameStringLC( currentDrive );
        ( *iMceSettingsArray )[EMceGeneralSettingsMemoryInUse].iUserText.Copy( *driveNameText );
        CleanupStack::PopAndDestroy( driveNameText );        
        }

#endif //RD_MULTIPLE_DRIVE
    CEikTextListBox* list=STATIC_CAST( CEikTextListBox*, 
        Control( EMceSettingsIdOtherItems ) );
    list->DrawNow();

    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL( KCRUidMuiuSettings ) );
    CleanupStack::PushL( repository );

    if ( ret == KErrNone )
        {
        ret = repository->Set( KMuiuMemoryInUse, TInt( iSession->CurrentDriveL() ) );
        __ASSERT_DEBUG( !ret, User::Panic( KPanicText,KCRepositorySettingFailure ) );
        }

    CleanupStack::Pop( repository );
    delete repository;
    }

// ----------------------------------------------------
// CMceGeneralSettingsDialog::ShutdownAppsL
// closes sms, mms and mail applications,
// before changing message store
// ----------------------------------------------------
TBool CMceGeneralSettingsDialog::ShutdownAppsL() const
    {
    RProperty property;

    User::LeaveIfError( property.Attach( KPSUidMuiu,KMuiuSysOpenMsgEditors ) );
    CleanupClosePushL( property );
    
    TInt i = 0;
    if ( property.Get(i) != KErrNone )
        {
        i = 0;
        }
    if ( i != 0 )
        {
        // clients are open
        CAknQueryDialog* closeDialog = CAknQueryDialog::NewL();

        HBufC* text = NULL;
        text = StringLoader::LoadLC( R_MCE_CLOSE_MESSAGES_CONFIRM, CCoeEnv::Static() );
        closeDialog->SetPromptL( *text );
        CleanupStack::PopAndDestroy( text );

        // Do you want to close all the open messages?
        if ( !closeDialog->ExecuteLD( R_MCE_MOVE_CONFIRM ) )            
            {
            CleanupStack::PopAndDestroy( &property );
            return EFalse;
            }
        else
            {
            i = 0;
            property.Set( KPSUidMuiu, KMuiuSysOpenMsgEditors, i );
            
            // wait a second
            User::After( KMceEditorsWaitInterval );
            }
        }
    CleanupStack::PopAndDestroy( &property );
    return ETrue;
    }


// ----------------------------------------------------
// CMceGeneralSettingsDialog::HandleMemoryInUseDialogL
// handles Memory in Use option
// 
// ----------------------------------------------------
void CMceGeneralSettingsDialog::HandleMemoryInUseDialogL()
    {
    if ( !ShutdownAppsL() )
        {
        // User does not want to close open messages
        return;
        }

    RFs& fs=iEikonEnv->FsSession();
    
#ifdef RD_MULTIPLE_DRIVE     
    // Memory selection dialog from Avkon    
    CAknMemorySelectionDialogMultiDrive* dlg = CAknMemorySelectionDialogMultiDrive::NewL(
            ECFDDialogTypeNormal, 
            0, // Default resource Id
            EFalse,
            AknCommonDialogsDynMem::EMemoryTypePhone |
            AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage | 
            AknCommonDialogsDynMem::EMemoryTypeMMCExternal  );

    CleanupStack::PushL( dlg );
    TInt currentDrive = TInt( iSession->CurrentDriveL() ); 
    TDriveNumber driveNumber = static_cast<TDriveNumber>( currentDrive );
    CAknMemorySelectionDialogMultiDrive::TReturnKey value = dlg->ExecuteL( driveNumber );
    CleanupStack::PopAndDestroy( dlg );
     
    TInt selectedDrive; 
    //if the user didn't press cancel key
    if (value != CAknMemorySelectionDialogMultiDrive::ERightSoftkey )
        { 
        selectedDrive = static_cast<TInt>( driveNumber );
        }
    // if the user press cancel
    else
        { 
        return;
        }           
    
#else            
    TInt currentDrive = TInt( iSession->CurrentDriveL() );
    CAknMemorySelectionSettingPage::TMemory memory( CAknMemorySelectionSettingPage::EPhoneMemory );

    CAknMemorySelectionSettingPage* dlg = CAknMemorySelectionSettingPage::NewL();
    CleanupStack::PushL( dlg ); 

    HBufC* text = StringLoader::LoadLC( R_MCE_SETTINGS_GEN_HEADING, iEikonEnv );  
    dlg->SetTitleL( *text ); 
    CleanupStack::PopAndDestroy( text );                
            
    if ( currentDrive == EDriveC )
        {                
        memory = CAknMemorySelectionSettingPage::EPhoneMemory;
        }
    else
        {                
        memory = CAknMemorySelectionSettingPage::EMemoryCard;
        }

    CAknMemorySelectionSettingPage::TReturnKey value = dlg->ExecuteL( memory );
    CleanupStack::PopAndDestroy( dlg );

    if ( value == CAknMemorySelectionSettingPage::ERightSoftkey )
       //Cancel was pressed;
        {
        return;
        }

    TInt selectedDrive;
    if ( memory == CAknMemorySelectionSettingPage::EPhoneMemory )
        {
        selectedDrive = EDriveC;            
        }
    else
        {
        selectedDrive = EDriveE;
        }
#endif // RD_MULTIPLE_DRIVE

    if (selectedDrive == currentDrive )
        {
        // no change, memory has not been changed
        UpdateMemorySelectionL(); 
        return;                
        }        
    else
        {
        TBool storeExist = EFalse;
        TRAPD( error, storeExist = iSession->DriveContainsStoreL( selectedDrive ) );

        if ( !error && storeExist )              
            {            
            //if server is busy, do not change store
            if( CheckIsServerBusyL() )
                {
                return;
                }

            // change message store
            CMuiuOperationWait* waiter=CMuiuOperationWait::NewLC();

            CMsvProgressReporterOperation* reportOp =
                    CMsvProgressReporterOperation::NewL( *iSession, waiter->iStatus );
            CleanupStack::PushL( reportOp );

            HBufC* text = StringLoader::LoadLC( R_MCE_SETTINGS_SWITCHING_MESSAGE_STORE, 
                       iEikonEnv );  
            reportOp->SetTitleL( *text ); 

            reportOp->SetCanCancelL( EFalse);

            CleanupStack::PopAndDestroy( text );
            
            CMsvOperation* changeOp=iSession->ChangeDriveL( selectedDrive, reportOp->RequestStatus() );
            reportOp->SetOperationL( changeOp );

            waiter->Start();
    
            CleanupStack::PopAndDestroy( reportOp );
            CleanupStack::PopAndDestroy( waiter );
                  
            //memory has been changed
            UpdateMemorySelectionL();           
            }
        else       
            {
//#ifdef RD_MULTIPLE_DRIVE
//            //Todo: Check the memory statue here, if it's read-only then show error note
//            // Wait for UIcr approved            
//            TUint driveStatus( 0 ); 
//            User::LeaveIfError( DriveInfo::GetDriveStatus( fs, driveNumber, driveStatus ) );
//            if ( driveStatus & DriveInfo::EDriveReadOnly )
//                {
//                HBufC* text = StringLoader::LoadLC( R_MCE_SETTINGS_CANNOT_MOVE_STORE, 
//        	                                                           CCoeEnv::Static() );
//        	    CAknErrorNote* note = new (ELeave) CAknErrorNote();
//        	    note->ExecuteLD(*text);
//        	    CleanupStack::PopAndDestroy( text );
//        	
//        	    return;
//                }
//#endif     
   
            // copy/move message store
            CAknQueryDialog* moveDialog = CAknQueryDialog::NewL();  

            // Do you want to copy all messages to the messaging store?
            if ( moveDialog->ExecuteLD( R_MCE_MOVE_CONFIRM ) )
                {    
                // Yes:
                // other question: Do you want to save the original messages?
                CAknQueryDialog* movesaveDialog = CAknQueryDialog::NewL();
          
                HBufC* text = NULL;
                text = StringLoader::LoadLC( R_MCE_MOVE_SAVE_CONFIRM, CCoeEnv::Static() );
                movesaveDialog->SetPromptL( *text );
                CleanupStack::PopAndDestroy( text );
                        
                if ( movesaveDialog->ExecuteLD( R_MCE_MOVE_CONFIRM ) )
                    {
                    //Yes: Copy messages                     
                    MoveMessageStoreL( currentDrive, selectedDrive, EFalse );
                    }
                else
                    {
                    //No: Move messages
                    MoveMessageStoreL( currentDrive, selectedDrive, ETrue );
                    }
                }
            else
                {
                // No: show empty message storage

                //if server is busy, do not change store
                if( CheckIsServerBusyL() )
                    {
                    return;
                    }

                // when empty store is created, it takes memory about 40kb
                // check first free memory
                if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iEikonEnv->FsSession(), 
                        KMceEmptyMessageStore, selectedDrive ) )
                    {
                    User::Leave( KErrDiskFull );
                    }

                CMuiuOperationWait* waiter=CMuiuOperationWait::NewLC(); 

                CMsvProgressReporterOperation* reportOp =
                            CMsvProgressReporterOperation::NewL( *iSession, waiter->iStatus );
                CleanupStack::PushL( reportOp );

                HBufC* text = StringLoader::LoadLC( R_MCE_SETTINGS_SWITCHING_MESSAGE_STORE, 
                                                                        iEikonEnv );  
                reportOp->SetTitleL( *text ); 

                reportOp->SetCanCancelL( EFalse);

                CleanupStack::PopAndDestroy( text );
            
                CMsvOperation* changeOp=iSession->ChangeDriveL( selectedDrive, 
                                                                                                reportOp->RequestStatus() );
                reportOp->SetOperationL( changeOp );

                waiter->Start();
    
                CleanupStack::PopAndDestroy( reportOp ); 
                CleanupStack::PopAndDestroy( waiter );
                UpdateMemorySelectionL();
                }                
            }
		
        CEikTextListBox* list=STATIC_CAST( CEikTextListBox*, 
            Control( EMceSettingsIdOtherItems ) );
        list->DrawNow();        
        }    
   }


// ----------------------------------------------------
// CMceGeneralSettingsDialog::GetHelpContext
// returns helpcontext as aContext
//
// ----------------------------------------------------
void CMceGeneralSettingsDialog::GetHelpContext
        ( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KMceApplicationUid;
    aContext.iContext = KMCE_HLP_SETTINGS_GEN;
    }

// ----------------------------------------------------
// CMceGeneralSettingsDialog::DynInitMenuPaneL
// ----------------------------------------------------
void CMceGeneralSettingsDialog::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_MCE_GENERAL_SETTINGS_CHOICE_MENUPANE )
        {
        aMenuPane->SetItemDimmed( EMceSettingsCmdSettingsDialogHelp, 
            !FeatureManager::FeatureSupported( KFeatureIdHelp ) );               
        }
    }

// ----------------------------------------------------
// CMceGeneralSettingsDialog::LaunchHelpL
// Launch help using context
// 
// ----------------------------------------------------
void CMceGeneralSettingsDialog::LaunchHelpL() const
    { 
    CCoeAppUi* appUi = STATIC_CAST( CCoeAppUi*, ControlEnv()->AppUi() );       
    CArrayFix<TCoeHelpContext>* helpContext = appUi->AppHelpContextL();   
    HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
    }

// ----------------------------------------------------
// CMceGeneralSettingsDialog::ChangeMessageStoreL
// Changes the drive of the message store
//
// ----------------------------------------------------
#ifdef RD_MULTIPLE_DRIVE
void CMceGeneralSettingsDialog::ChangeMessageStoreL( TInt aTargetDrive )
#else
void CMceGeneralSettingsDialog::ChangeMessageStoreL()
#endif //RD_MULTIPLE_DRIVE
    {
    //if server is busy, do not change store
    if( CheckIsServerBusyL() )
        {
        return;
        }

    RFs& fs=iEikonEnv->FsSession();
    
    TInt driveNumber;
    
#ifdef RD_MULTIPLE_DRIVE
    driveNumber = aTargetDrive;
#else
    // Get the current drive
    TInt currentDrive=TInt( iSession->CurrentDriveL() );

    if ( currentDrive == EDriveC )
        {
        driveNumber = EDriveE;            
        }
    else
        {
        driveNumber = EDriveC;
        }
#endif //RD_MULTIPLE_DRIVE

    // change message store
    CMuiuOperationWait* waiter=CMuiuOperationWait::NewLC();

    CMsvProgressReporterOperation* reportOp =
                         CMsvProgressReporterOperation::NewL( *iSession, waiter->iStatus );
    CleanupStack::PushL( reportOp );

    HBufC* text = StringLoader::LoadLC( R_MCE_SETTINGS_SWITCHING_MESSAGE_STORE, 
                                                             iEikonEnv );  
    reportOp->SetTitleL( *text ); 

    reportOp->SetCanCancelL( EFalse );

    CleanupStack::PopAndDestroy( text );
            
    CMsvOperation* changeOp=iSession->ChangeDriveL( driveNumber, reportOp->RequestStatus() );
    reportOp->SetOperationL( changeOp );

    waiter->Start();
    
    CleanupStack::PopAndDestroy( reportOp );
    CleanupStack::PopAndDestroy( waiter ); 
    }
    
    
// ----------------------------------------------------
// void CMceGeneralSettingsDialog::SetMSKButtonL()
// ----------------------------------------------------
void CMceGeneralSettingsDialog::SetMSKButtonL()
    {
    CEikTextListBox* list=STATIC_CAST(
                    CEikTextListBox*,
                    Control( EMceSettingsIdOtherItems) );
    const TInt index = list->CurrentItemIndex();
    const TInt numberOfItem = list->Model()->NumberOfItems();
    const TInt resourceId = index==0 ?
         R_MCE_MSK_BUTTON_CHANGE : R_MCE_MSK_BUTTON_OPEN;
    CEikButtonGroupContainer& cba = ButtonGroupContainer();
    cba.SetCommandL( KMSKPosition, resourceId );
    cba.DrawNow();
    }

#ifdef RD_MULTIPLE_DRIVE
// ----------------------------------------------------
// CMceGeneralSettingsDialog::MakeDriveNameStringLC
// ----------------------------------------------------
HBufC* CMceGeneralSettingsDialog::MakeDriveNameStringLC( TInt aDriveNumber )
    {       
    // get drive later and delimiter, e.g. "C:"
    TDriveName driveName(TDriveUnit( aDriveNumber ).Name() );
    
    TUint driveStatus( 0 );
    RFs& fs=iEikonEnv->FsSession();
    User::LeaveIfError( DriveInfo::GetDriveStatus( fs, aDriveNumber, driveStatus ) );     
    
    //Following flitters are referred to filemanager application
    if ( driveStatus & DriveInfo::EDriveInternal )
        {
        //Device Memory and Mass storage drives
        if ( driveStatus & DriveInfo::EDriveExternallyMountable )
            {
            return StringLoader::LoadLC( R_MCE_SETT_MEM_MASS_STORAGE, driveName, iEikonEnv );
            }
        else 
            {
            return StringLoader::LoadLC( R_MCE_SETT_MEM_DEVICE, driveName, iEikonEnv );
            }
        }
    //memory card
    else
        {
        return StringLoader::LoadLC( R_MCE_SETT_MEM_MEMORY_CARD, driveName, iEikonEnv );            
        }
    }
#endif //RD_MULTIPLE_DRIVE
//  End of File
