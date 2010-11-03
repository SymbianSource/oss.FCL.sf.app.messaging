/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgEditorAppUi implementation
*
*/



// ========== INCLUDE FILES ================================

#include <featmgr.h>                // for feature manager

#include <eikenv.h>                 // Eikon Enviroment
#include <txtrich.h>                // Rich Text
#include <eikrted.h>                // CEikRichTextEditor
#include <eikmenub.h>               // CEikMenuBar
#include <eikspane.h>               // status panel

#include <eikapp.h>                 // CEikApplication
#include <eikserverapp.h>
#include <bautils.h>                // NearestLanguageFile
#include <eikdialg.h>               // CEikDialog
#include <data_caging_path_literals.hrh>

#include <msvapi.h>                 //
#include <mtmuibas.h>               // MTM UI
#include <mtmuidef.hrh>             // Preferences
#include <mtmdef.h>                 // TMsvPartList

#include <MuiuMsgEmbeddedEditorWatchingOperation.h>
#include <MuiuMsgEditorLauncher.h>  // for TEditorParameters
#include <MuiuMessageIterator.h>    // for CMessageIterator
#include <MuiuOperationWait.h>      // for CMuiuOperationWait
#include <muiu.mbg>

#include <aknenv.h>
#include <aknnavi.h>                // CAknNavigationControlContainer
#include <aknnavide.h>              // CAknNavigationDecorator
#include <akntabgrp.h>              // CAknTabGroup
#include <akninputblock.h>          // CAknInputBlock
#include <aknnotewrappers.h>
#include <AknIndicatorContainer.h>
#include <akncontext.h>             // For message store in MMC -indication
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <commonphoneparser.h>      // Common phone number validity checker
#include <apgicnfl.h>               // apamasked bitmaps
#include <ErrorUI.h>				// CErrorUI - to show global error notes
#include <StringLoader.h>           // StringLoader
#include <NpdApi.h>                 // Notepad API
#include <avkon.hrh>
#include <AknStatuspaneUtils.h>
#include <AknSettingCache.h>
#include <messaginginternalcrkeys.h>
#include <MessagingSDKCRKeys.h>
#include "MsgEditorAppUiExtension.h"

#include <aknlayoutscalable_avkon.cdl.h>

#include "MsgEditorShutter.h"

#include "MsgEditorCommon.h"
#include "MsgEditorAppUi.h"         // class header
#include "MsgEditorView.h"          // View
#include "MsgBodyControl.h"         // Body Control
#include "MsgSendKeyAcceptingQuery.h"
#include "MsgEditorAppUiPanic.h"    // Panics
#include "MsgEditorFlags.h"         // EditorFlags
#include "MsgAttachmentUtils.h"
#include <MsgEditorAppUi.rsg>       // resource identifiers
#include "MsgNaviPaneControl.h"

#include <aknpriv.hrh> 
#include <akntoolbar.h> 

#ifdef RD_MSG_FAST_PREV_NEXT
#include <messaginginternalpskeys.h>
#endif

#include "MsgEditorLogging.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

const TInt KMsgDiskSpaceForDelete = ( 5 * 1024 ); // minimum disk space needed when deleting messages

// ========== MACROS =======================================

_LIT( KMsgEditorAppUiResourceFileName, "msgeditorappui.rsc" );
_LIT( KMuiuBitmapFileName, "muiu.mbm" );
_LIT( KMsgEditorAppUiFilterRe, "RE:" );
_LIT( KMsgEditorAppUiFilterFw, "FW:" );
_LIT( KMsgEditorAppUiFilterFwd, "FWD:" );

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgEditorAppUi::CMsgEditorAppUi
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgEditorAppUi::CMsgEditorAppUi() : 
    iExitMode( MApaEmbeddedDocObserver::EKeepChanges ),
    iCloseWithEndKey( ETrue )
    {
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::ConstructL()
    {
    CMsgEditorBaseUi::ConstructL();
    
    iMsgNaviDirection = KErrNotFound; 

    // initialize feature manager to check supported features
    FeatureManager::InitializeLibL();
    iMmcFeatureSupported = FeatureManager::FeatureSupported( KFeatureIdMmc );

    TParse parse;
    parse.Set( KMsgEditorAppUiResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName fileName( parse.FullName() );

    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    iResourceFileOffset = iEikonEnv->AddResourceFileL( fileName );

    Document()->SetEditorModelObserver( this );

    iFileMan = CFileMan::NewL( iEikonEnv->FsSession() );

    // create msgeditor temp directory.
    TFileName temppath;
    MsgAttachmentUtils::GetMsgEditorTempPath( temppath );

    RFs& fs = iEikonEnv->FsSession();
    fs.MkDir( temppath );

    CEikStatusPane* sp = StatusPane();
    if ( iMmcFeatureSupported && 
        TInt( Document()->Session().CurrentDriveL() ) == EDriveE )
        {
        CAknContextPane* contextPane = static_cast<CAknContextPane*>
            ( sp->ControlL( TUid::Uid( EEikStatusPaneUidContext ) ) );
        TParse fileParse;
        fileParse.Set( KMuiuBitmapFileName, &KDC_APP_BITMAP_DIR, NULL );
        CFbsBitmap* bmp = NULL;
        CFbsBitmap* mask = NULL;
#ifdef __SCALABLE_ICONS
        AknsUtils::CreateIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDQgnMenuMceMmcCxt,
            bmp,
            mask,
            fileParse.FullName(),
            EMbmMuiuQgn_menu_mce_mmc,
            EMbmMuiuQgn_menu_mce_mmc_mask );
#else
        AknsUtils::CreateIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDQgnMenuMceMmcCxt,
            bmp,
            mask,
            fileParse.FullName(),
            EMbmMuiuQgn_menu_mce_mmc_cxt,
            EMbmMuiuQgn_menu_mce_mmc_cxt_mask );
#endif  //__SCALABLE_ICONS
        contextPane->SetPicture( bmp, mask );
        }
    iOwnNaviPane = static_cast<CAknNavigationControlContainer*>
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    iCoeEnv->FsSession().ReserveDriveSpace(
        TInt( Document()->Session().CurrentDriveL() ),
        KMsgDiskSpaceForDelete );
  
    CMsgEditorDocument* doc = Document();
    iEditorShutter = CMsgEditorShutter::NewL(*doc);
  
    iMsgEditorAppUiExtension = new(ELeave) CMsgEditorAppUiExtension( this );
    CAknEnv* env = iAvkonEnv;
    CAknSettingCache& cache = env->SettingCache();
    cache.RegisterPluginL( &( iMsgEditorAppUiExtension->iSettingCachePlugin ) );    
    
    iStatusPaneRes = StatusPane()->CurrentLayoutResId();
    
    iIterator = NULL;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::~CMsgEditorAppUi
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgEditorAppUi::~CMsgEditorAppUi()
    {
    delete iEditorShutter;

    Document()->SetEditorModelObserver( NULL );

    // remove resource file from CONE-maintained resource file list.
    if ( iResourceFileOffset )
        {
        iEikonEnv->DeleteResourceFile( iResourceFileOffset );
        }

    // call NotifyExit on completion of editing of an embedded document.
    if ( iDoorObserver )
        {
        MEBLOGGER_WRITEF( _L( "MEB: CMsgEditorAppUi: Closing with exit mode %d" ), iExitMode );
        iDoorObserver->NotifyExit( iExitMode );
        }

    delete iView;
    delete iLock;
    delete iNaviDecorator;

    // we are referring instance of nvaidecoratoir only
    // no need to delete, alreday deleted in  delete iNaviDecorator;
    //delete iIterator;

    // delete files from our temp directory.
    if ( iFileMan )
        {
        TFileName temppath;
        MsgAttachmentUtils::GetMsgEditorTempPath( temppath );

        /*TInt err =*/ iFileMan->Delete( temppath /*, CFileMan::ERecurse*/ );
        //TInt err2 = iFileMan->RmDir(temppath); // remove directory
        }

    delete iFileMan;

    // Uninitialize FeatureManager and members
    FeatureManager::UnInitializeLib();
    if( iMsgEditorAppUiExtension )
        {
        CAknEnv* env = iAvkonEnv;
        CAknSettingCache& cache = env->SettingCache();
        cache.DeRegisterPlugin( &( iMsgEditorAppUiExtension->iSettingCachePlugin ) );
        delete iMsgEditorAppUiExtension;
        }    
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleKeyEventL
//
// Route key events to view.
// ---------------------------------------------------------
//
EXPORT_C TKeyResponse CMsgEditorAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( iView )
        {
        return iView->OfferKeyEventL( aKeyEvent, aType );
        }
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleWsEventL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleWsEventL( const TWsEvent& aEvent,CCoeControl* aDestination )
    {
    TInt type = aEvent.Type();

    if ( type == EEventKey )
        {
        TKeyEvent* key = aEvent.Key();
        if ( ( key->iModifiers & EModifierShift ) &&
            ( key->iCode == EKeyBackspace ) )
            {
            if ( iView )
                {
                CMsgBodyControl* body = 
                        static_cast<CMsgBodyControl*>( iView->ControlById( EMsgComponentIdBody ) );

                if ( body && body->IsReadOnly() )
                    {
                    TRAP_IGNORE( SaveBodyL( body ) );
                    }
                }

            key->iModifiers &= ~EModifierShift;
            key->iCode = EKeyDelete;
            }
        }
        
    if ( type != KAknUidValueEndKeyCloseEvent ||
         iCloseWithEndKey )
        {
        CAknAppUi::HandleWsEventL( aEvent, aDestination );
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::NextMessageL
//
// Set a specific exit code for opening next or previous message. Codes are
// defined in MuiuMsgEmbeddedEditorWatchingOperation.h 
// and handled by MCE after msg viewer has been closed.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::NextMessageL( TBool aForward )
    {
    // Next is ETrue if layout is EAknLayoutIdABRW,
    // EFalse if EAknLayoutIdELAF or EAknLayoutIdAPAC
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        aForward = !aForward;
        }

    iMsgNaviDirection = aForward;
    
    // Check if next/previous message 
    __ASSERT_DEBUG( iIterator != NULL, Panic( ENullPointer1 ) );
     if ( aForward )
         {
         iIterator->SetNextMessage();
         
         }
     else
         {
         iIterator->SetPreviousMessage();
         }

     RProperty::Set( KPSUidMuiu, KMuiuKeyNextMsg,
              iIterator->CurrentMessage().Id() );
         
#ifdef RD_MSG_FAST_PREV_NEXT
    // Check if next/previous message can be fast opened by viewer
    
    if ( CanFastOpenL( *iIterator) )
        {
        // Load next/previous message
        TRAPD( err, Document()->SetEntryL( iIterator->CurrentMessage().Id() ) );
        // If fast open fails, view is incompletely constructed
        // Drop back to mce
        if( err == KErrCancel )
            {
            /* It is a known-valid failure(quick lauch cancelled in viewers due to heterogenous messages)
             * In that case, reset the current entry to original and exit with correct code so that message
             * will be launched completely from MCE.
             */
            if ( iMsgNaviDirection )
                {
                iIterator->SetPreviousMessage();
                }
            else
                {
                iIterator->SetNextMessage();
                }
            Document()->SetEntryWithoutNotificationL( iIterator->CurrentMessage().Id() );
            iExitMode = iMsgNaviDirection ?
                MApaEmbeddedDocObserver::TExitMode(
                    CMsgEmbeddedEditorWatchingOperation::EMsgExitNext) :
                MApaEmbeddedDocObserver::TExitMode(
                    CMsgEmbeddedEditorWatchingOperation::EMsgExitPrevious);
            Exit( iExitMode );
            }
        else if ( err != KErrNone )
            {
            // Reset the KMuiuKeyNextMsg to 0 
            RProperty::Set( KPSUidMuiu, KMuiuKeyNextMsg,0 );
            // Unknown error during quick launch.
            iCoeEnv->HandleError( err );
            iExitMode = MApaEmbeddedDocObserver::ENoChanges;
            Exit( EAknSoftkeyClose );
            }
        }
    else
#endif // RD_MSG_FAST_PREV_NEXT
        {
        iExitMode = aForward ?
            MApaEmbeddedDocObserver::TExitMode(
                CMsgEmbeddedEditorWatchingOperation::EMsgExitNext) :
            MApaEmbeddedDocObserver::TExitMode(
                CMsgEmbeddedEditorWatchingOperation::EMsgExitPrevious);
        Exit( iExitMode );
        }
    iMsgNaviDirection = KErrNotFound;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::IsNextMessageAvailableL
//
// Check if next or previous msg is available.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgEditorAppUi::IsNextMessageAvailableL( TBool aForward )
    {
    TBool ret = EFalse;
    
    if ( iEikonEnv->StartedAsServerApp() )
        {
        CMsgEditorDocument* doc = Document();

        //We should not create iterator here  navi pane iterator
        //will be refered  in  CMsgEditorAppUi
         

        // Next is ETrue if layout is EAknLayoutIdABRW,
        // EFalse if EAknLayoutIdELAF or EAknLayoutIdAPAC
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            aForward = !aForward;
            }
		if(iIterator)
		{
        	if ( aForward )
            	{
            	if ( iIterator->NextMessageExists() )
                	{
                	ret = ETrue;
                	}
            	}
        	else
            	{
            	if ( iIterator->PreviousMessageExists() )
                	{
                	ret = ETrue;
                	}
            	}
		}
	
        }
        
    return ret;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::MessageIndexInFolderL
//
// Returns index of current msg and also total number of msgs in folder.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::MessageIndexInFolderL( TInt& aCurrentIndex, 
                                                      TInt& aMsgsInFolder )
    {
    if ( iEikonEnv->StartedAsServerApp() )
        {
        CMsgEditorDocument* doc = Document();
        //We should not create iterator here, navi pane iterator
        //will be refered  in  CMsgEditorAppUi
        if(iIterator)
            {
        	aCurrentIndex = iIterator->CurrentMessageIndex();
        	aMsgsInFolder = iIterator->MessagesInFolder();
            }
        }
    else
        {
        aCurrentIndex = 0;
        aMsgsInFolder = 0;
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::MoveMessageEntryL
//
// Moves an entry to another location.
// ---------------------------------------------------------
//
EXPORT_C TMsvId CMsgEditorAppUi::MoveMessageEntryL( TMsvId aTarget ) const
    {
    TMsvEntry msvEntry = Document()->Entry();
    TMsvId id = KMsvNullIndexEntryId;

    if ( msvEntry.Parent() != aTarget )
        {
        CAknInputBlock::NewLC();

        CMsvEntry* parentEntry = Document()->Session().GetEntryL( msvEntry.Parent() );
        CleanupStack::PushL( parentEntry );

        // -- Change sort parameters so that parentEntry->MoveL does not leave as the child is invisible.
        TMsvSelectionOrdering sort = parentEntry->SortType();
        sort.SetShowInvisibleEntries( ETrue );
        parentEntry->SetSortTypeL( sort );

        // Copy original from the parent to the new location.
        CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();

        CMsvOperation* op = parentEntry->MoveL(
            msvEntry.Id(), aTarget, wait->iStatus );
        CleanupStack::PushL( op );
        wait->Start(); // wait for asynch operation

        TMsvLocalOperationProgress prog = McliUtils::GetLocalProgressL( *op );
        User::LeaveIfError( prog.iError );
        id = prog.iId;

        CleanupStack::PopAndDestroy( 4 );  // op, wait, parentEntry, absorber
        }

    return id;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::DeleteAndExitL
//
// Deletes current entry and then exits.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::DeleteAndExitL()
    {
    DeleteCurrentEntryL();
    Exit( EAknSoftkeyBack );
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::DeleteCurrentEntryL
//
// Deletes current entry.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::DeleteCurrentEntryL()
    {
    CMsgEditorDocument* doc = Document();
    UnlockEntry();

    TInt msgStoreDrive = TInt( doc->Session().CurrentDriveL() );
    iCoeEnv->FsSession().GetReserveAccess( msgStoreDrive );

    if ( doc->CurrentEntry().OwningService() == KMsvLocalServiceIndexEntryId )
        {
        const TMsvId id = doc->Entry().Id();
        //doc->CurrentEntry().SetEntryL( doc->Entry().Parent() );
        TRAPD( 
                err, 
                {
                doc->CurrentEntry().SetEntryL( doc->Entry().Parent() ); 
                doc->CurrentEntry().DeleteL( id ); 
                }
            );
        if ( err != KErrNone )
            {
            doc->Session().RemoveEntry( id );
            }
        }
    else
        {
        CMsvEntrySelection* sel = new ( ELeave ) CMsvEntrySelection;
        CleanupStack::PushL( sel );
        TMsvId id = doc->Entry().Id();
        sel->AppendL( id );
        doc->Mtm().SwitchCurrentEntryL( doc->Entry().Parent() );

        CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();

        CMsvOperation* op = doc->MtmUi().DeleteFromL( *sel, wait->iStatus );
        CleanupStack::PushL( op );

        wait->Start();

        const TInt err = doc->MtmUi().DisplayProgressSummary( op->ProgressL() );
        CleanupStack::PopAndDestroy( 3, sel );  // sel, wait, op
        if ( err == KErrCancel )
            {
            doc->Mtm().SwitchCurrentEntryL( id );
            LockEntryL();
            return ;
            }
        User::LeaveIfError( err );
        }

    iCoeEnv->FsSession().ReleaseReserveAccess( msgStoreDrive );
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::PrepareToExit
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::PrepareToExit()
    {
    CMsgEditorDocument* doc = Document();

    if ( doc->HasModel() )
        {
        if ( doc->Entry().InPreparation() )
            {
            doc->Session().RemoveEntry( doc->Entry().Id() );
            }
        }

    CEikAppUi::PrepareToExit();
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::ReplyL
//
// Asynchronous Mtm reply command.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::ReplyL(
    TMsvId aTarget, TBool aIncludeOriginal /*= ETrue*/ )
    {
    TMsvPartList parts =
        KMsvMessagePartDescription |
        KMsvMessagePartOriginator;

    if ( aIncludeOriginal )
        {
        parts |= KMsvMessagePartBody;
        }

    CBaseMtmUi& mtmUi = Document()->MtmUi();

    // to by-pass scanner warning about member variable being stored into
    // cleanup stack blocker is first stored into temporary variable. Storing
    // is allowed here as it is not owned.

    CMuiuOperationWait* wait =
        CMuiuOperationWait::NewLC( EActivePriorityWsEvents + 10 );

    mtmUi.SetPreferences( mtmUi.Preferences() | EMtmUiFlagEditorPreferEmbedded );
    CMsvOperation* oper = mtmUi.ReplyL( aTarget, parts, wait->iStatus );
    CleanupStack::PushL( oper );

    wait->Start();
    
    CleanupStack::PopAndDestroy( 2 );  // absorber, wait, oper
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::ReplyToAllL
//
// Asynchronous Mtm reply all command.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::ReplyToAllL(
    TMsvId aTarget, TBool aIncludeOriginal /*= ETrue*/ )
    {
    TMsvPartList parts =
        KMsvMessagePartDescription |
        KMsvMessagePartRecipient |
        KMsvMessagePartOriginator ;

    if ( aIncludeOriginal )
        {
        parts |= KMsvMessagePartBody;
        }

    CBaseMtmUi& mtmUi = Document()->MtmUi();
    
    // to by-pass scanner warning about member variable being stored into
    // cleanup stack blocker is first stored into temporary variable. Storing
    // is allowed here as it is not owned.
    CAknInputBlock* blocker = CAknInputBlock::NewCancelHandlerLC( this );
    iInputBlocker = blocker;

    CMuiuOperationWait* wait =
        CMuiuOperationWait::NewLC( EActivePriorityWsEvents + 10 );

    mtmUi.SetPreferences( mtmUi.Preferences() | EMtmUiFlagEditorPreferEmbedded );
    CMsvOperation* oper = mtmUi.ReplyL( aTarget, parts, wait->iStatus );
    CleanupStack::PushL( oper );

    wait->Start();
    
    CleanupStack::PopAndDestroy( 3, iInputBlocker );  // absorber, wait, oper
    iInputBlocker = NULL;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::ForwardL
//
// Asynchronous Mtm forward command.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::ForwardL( TMsvId aTarget )
    {
    TMsvPartList parts =
        KMsvMessagePartBody |
        KMsvMessagePartDescription |
        KMsvMessagePartAttachments;

    CBaseMtmUi& mtmUi = Document()->MtmUi();
    mtmUi.SetPreferences( mtmUi.Preferences() | EMtmUiFlagEditorPreferEmbedded );
    
    // to by-pass scanner warning about member variable being stored into
    // cleanup stack blocker is first stored into temporary variable. Storing
    // is allowed here as it is not owned.
    CAknInputBlock* blocker = CAknInputBlock::NewCancelHandlerLC( this );
    iInputBlocker = blocker;
    
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC( EActivePriorityWsEvents + 10 );
    
    CMsvOperation* oper = mtmUi.ForwardL( aTarget, parts, wait->iStatus );
    CleanupStack::PushL( oper );

    wait->Start();

    CleanupStack::PopAndDestroy( 3, iInputBlocker );  // absorber, wait, oper
    iInputBlocker = NULL;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::InsertTemplate
//
//
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgEditorAppUi::InsertTemplateL( TInt aMaxChars /*= -1*/ )
    {
    __ASSERT_DEBUG( iView != NULL, Panic( ENullPointer1 ) );

    // get memo content from notepad to buffer.
    HBufC* title = iEikonEnv->AllocReadResourceLC( R_QTN_MAIL_POPUP_TEXT_SEL_TEMP );

    HBufC* buf = CNotepadApi::FetchTemplateL( title );
    CleanupStack::PushL( buf );

    TInt err = DoInsertTextL( buf, aMaxChars );

    CleanupStack::PopAndDestroy( 2, title ); // buf, title
    return err;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::InsertTextMemoL
//
//
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgEditorAppUi::InsertTextMemoL( TInt aMaxChars )
    {
    __ASSERT_DEBUG( iView != NULL, Panic( ENullPointer1 ) );

    // get memo content from notepad to buffer.
    HBufC* title = iEikonEnv->AllocReadResourceLC( R_QTN_NMAKE_FETCH_MEMO_PRMPT );

    HBufC* buf = CNotepadApi::FetchMemoL( title );
    CleanupStack::PushL( buf );

    TInt err = DoInsertTextL( buf, aMaxChars );
    
    CleanupStack::PopAndDestroy( 2, title ); // buf, title
    return err;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::DoInsertTextL
//
//
// ---------------------------------------------------------
//
TInt CMsgEditorAppUi::DoInsertTextL( HBufC* aBuffer, TInt aMaxChars )
    {
    TInt err( KErrNone );
    if ( aBuffer )
        {
        TInt buflen = aBuffer->Length();

        if ( aMaxChars == -1 || buflen <= aMaxChars )
            {
            CMsgBodyControl* body = 
                    static_cast<CMsgBodyControl*>( iView->ControlById( EMsgComponentIdBody ) );

            if ( body )
                {
                CEikRichTextEditor& editor = body->Editor();

                if ( body != iView->FocusedControl() )
                    {
                    editor.SetCursorPosL( editor.TextLength(), EFalse );
                    }

                body->InsertTextL( *aBuffer );

                iView->SetFocus( EMsgComponentIdBody );

                Document()->SetChanged( ETrue );
                }
            }
        else
            {
            err = KErrOverflow;
            }
        }
    else
        {
        err = KErrCancel;
        }
        
    return err;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::UpdateNaviPaneL
//
// Updates navi pane.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::UpdateNaviPaneL(
    const CFbsBitmap* aBitmap     /*= NULL*/,
    const CFbsBitmap* aBitmapMask /*= NULL*/ )
    {
    UpdateNaviPaneL( EFalse, aBitmap, aBitmapMask );
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::UpdateNaviPaneL
//
// Updates navi pane.
// ---------------------------------------------------------
//
void CMsgEditorAppUi::UpdateNaviPaneL(
    TBool             aUpdateExisting,
    const CFbsBitmap* aBitmap     /*= NULL*/,
    const CFbsBitmap* aBitmapMask /*= NULL*/ )
    {
    if ( !iOwnNaviPane )
        {
        return;
        }
    TInt index = 0;
    TInt msgs = 0;
    HBufC* buf;

    MessageIndexInFolderL( index, msgs );

    TBool prevAvailable = IsNextMessageAvailableL( EFalse );
    TBool nextAvailable = IsNextMessageAvailableL( ETrue );

    CArrayFixFlat<TInt>* array = new( ELeave ) CArrayFixFlat<TInt>( 2 );
    CleanupStack::PushL( array );

    if ( msgs )
        {
        array->AppendL( index + 1 );
        array->AppendL( msgs );
        buf = StringLoader::LoadLC( R_QTN_STAT_MSG_NUMBER, *array, iCoeEnv );
        }
    else
        {
        // if no messages make empty message label.
        buf = HBufC::NewLC( 0 );
        prevAvailable = nextAvailable = 0;
        }

    if ( aUpdateExisting )
        {
        if ( iNaviDecorator )
            {
            CAknTabGroup* tabGroup =
                static_cast<CAknTabGroup*>( iNaviDecorator->DecoratedControl() );
            tabGroup->ReplaceTabTextL( 0, *buf );

            iNaviDecorator->MakeScrollButtonVisible( ETrue );
            iNaviDecorator->SetScrollButtonDimmed(
                CAknNavigationDecorator::ELeftButton,
                !prevAvailable );
            iNaviDecorator->SetScrollButtonDimmed(
                CAknNavigationDecorator::ERightButton,
                !nextAvailable );

            iOwnNaviPane->PushL( *iNaviDecorator );
            }
        }
    else
        {
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        iNaviDecorator = iOwnNaviPane->CreateTabGroupL();
        
        CAknTabGroup* tabGroup =
            static_cast<CAknTabGroup*>( iNaviDecorator->DecoratedControl() );
        tabGroup->AddTabL( 0, *buf );            
        tabGroup->SetActiveTabById( 0 );
        // does not leave with one tab.
        tabGroup->SetTabFixedWidthL( KTabWidthWithOneTab );

        iNaviDecorator->MakeScrollButtonVisible( ETrue );
        iNaviDecorator->SetScrollButtonDimmed(
            CAknNavigationDecorator::ELeftButton,
            !prevAvailable );
        iNaviDecorator->SetScrollButtonDimmed(
            CAknNavigationDecorator::ERightButton,
            !nextAvailable );

        iOwnNaviPane->PushL( *iNaviDecorator );

        if ( aBitmap && aBitmapMask )
            {
            // replace tab with new one containing text and bitmaps.
            // this must be done after PushL because this transfers
            // ownership of caller's bitmaps to tab group.
            tabGroup->ReplaceTabL( 0, *buf, aBitmap, aBitmapMask );
            }
        }
    CleanupStack::PopAndDestroy( 2, array ); // buf, array
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleIteratorEventL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleIteratorEventL(
    TMessageIteratorEvent aEvent )
    {
    if ( aEvent == EFolderCountChanged )
        {
        UpdateNaviPaneL( ETrue );
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::CallToSenderQueryL
//
//
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgEditorAppUi::CallToSenderQueryL(
    const TDesC& aNumber, const TDesC& aAlias )
    {
    CDesCArrayFlat* strings = new ( ELeave ) CDesCArrayFlat( 2 );
    CleanupStack::PushL( strings );

    strings->AppendL( aAlias );  // %0U
    strings->AppendL( aNumber ); // %1U

    HBufC* prompt = StringLoader::LoadLC(
        R_QTN_SMS_QUEST_CALL, *strings, iCoeEnv );

    CMsgSendKeyAcceptingQuery* dlg = CMsgSendKeyAcceptingQuery::NewL( *prompt );
    TInt ret = dlg->ExecuteLD( R_MEB_CALLBACK_QUERY );

    CleanupStack::PopAndDestroy( 2, strings ); // strings, prompt

    return ( ret != 0 ); // No = EFalse, others = ETrue.
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::ProcessCommandParametersL
//
// Parses command line parameter and launch model accoring to them.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgEditorAppUi::ProcessCommandParametersL(
    TApaCommand /*aCommand*/,
    TFileName& /*aDocumentName*/,
    const TDesC8& aTail )
    {
    CMsgEditorDocument& doc = *Document();

    if ( aTail.Length() > 0 )
        {
        // Get the parameters passed by the launching MTMUI.
        TPckgBuf<TEditorParameters> paramPack;
        const TInt paramSize = sizeof( TEditorParameters );

        __ASSERT_ALWAYS( aTail.Length() == paramSize, Panic( EInvalidArgument ) );

        paramPack.Copy( aTail );
        const TEditorParameters& params = paramPack();
        
        // to by-pass scanner warning about member variable being stored into
        // cleanup stack blocker is first stored into temporary variable. Storing
        // is allowed here as it is not owned.
        CAknInputBlock* blocker = CAknInputBlock::NewCancelHandlerLC( this );
        iInputBlocker = blocker;

        doc.LaunchParametersL( params );
        doc.EntryChangedL();  // notified after is unique (and unlocked).
        doc.PrepareToLaunchL( this );

        CleanupStack::PopAndDestroy( iInputBlocker );
        iInputBlocker = NULL;
        }
    else
        {
        // no parameters enable TestEditor launching...
        doc.PrepareToLaunchL( this );
        }

    return EFalse;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::MenuBar
//
// Returns application menubar.
// ---------------------------------------------------------
//
EXPORT_C CEikMenuBar* CMsgEditorAppUi::MenuBar() const
    {
    return iEikonEnv->AppUiFactory()->MenuBar();
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::SetEmphasis
//
// From MEikMenuObserver (called when menu is opened).
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::SetEmphasis(
    CCoeControl* aMenuControl, TBool aEmphasis )
    {
    if ( iView )
        {
        UpdateStackedControlFlags(
            iView,
            aEmphasis ? ECoeStackFlagRefusesFocus : 0,
            ECoeStackFlagRefusesFocus );
        }

    UpdateStackedControlFlags(
        aMenuControl,
        aEmphasis ? 0 : ECoeStackFlagRefusesFocus,
        ECoeStackFlagRefusesFocus );

    HandleStackChanged();
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleEntryChangeL
//
// Default error handling for all messaging editors and viewers.
// HandleEntryChangeL is called when the currently open message
// has been modified by some other messaging client. The default
// action is "do nothing", but editors can define their own event
// processing by overriding this function.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleEntryChangeL()
    {
    // no action
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleEntryDeletedL
//
// Default error handling for all messaging editors and viewers.
// HandleEntryDeletedL is called when the currently open message
// has been deleted by some other messaging client.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleEntryDeletedL()
    {
    // TODO: get error code from errorres.
    ViewErrorL(-7005);

    // close the application (and possible chain of viewers, editors and mce etc.).
    ProcessCommandL( EAknCmdExit );
    //Exit();
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleEntryMovedL
//
// Default error handling for all messaging editors and viewers.
// HandleEntryMovedL is called when the currently open message
// has been moved to some other folder, by some other messaging
// client.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleEntryMovedL(
    TMsvId /*aOldParent*/, TMsvId /*aNewParent*/ )
    {
    // refresh model content.
    Document()->SetEntryWithoutNotificationL( Document()->Entry().Id() );
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleMtmGroupDeinstalledL
//
// Default error handling for all messaging editors and viewers.
// HandleMtmGroupDeinstalledL is called when the MTM Group used
// by the currently open message has been removed from the device.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleMtmGroupDeinstalledL()
    {
    // TODO: get error code from errorres.
    ViewErrorL( -7006 );

    // close the application (and possible chain of viewers, editors and mce etc.).
    ProcessCommandL( EAknCmdExit );
    //Exit();
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleGeneralErrorL
//
// Default error handling for all messaging editors and viewers.
// HandleGeneralErrorL is called when some unknown error has
// happened. The aErrorCode parameter contains the error code.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleGeneralErrorL( TInt /*aErrorCode*/ )
    {
    // TODO: show note?

    // close the application (and possible chain of viewers, editors and mce etc.).
    ProcessCommandL( EAknCmdExit );
    //Exit();
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleCloseSessionL
//
// Default error handling for all messaging editors and viewers.
// HandleCloseSessionL is called when the messaging server wants
// to close the session (e.g. because it's closing down and wants
// to get rid of all messaging clients).
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleCloseSessionL()
    {
    // save changes and close the application.
    DoMsgSaveExitL();
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleServerFailedToStartL
//
// Default error handling for all messaging editors and viewers.
// HandleServerFailedToStartL is called when the message server
// start-up was not successful.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleServerFailedToStartL()
    {
    // TODO: get error code from errorres.
    ViewErrorL( -7007 );

    // close the application (and possible chain of viewers, editors and mce etc.).
    ProcessCommandL( EAknCmdExit );
    //Exit();
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleServerTerminatedL
//
// Default error handling for all messaging editors and viewers.
// HandleServerTerminatedL is called when the messaging server has
// died. Application cannot use the message server session any more.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleServerTerminatedL()
    {
    // TODO: get error code from errorres.
    ViewErrorL( -7008 );

    // close the application (and possible chain of viewers, editors and mce etc.).
    ProcessCommandL( EAknCmdExit );
    //Exit();
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleMediaChangedL
//
// Default error handling for all messaging editors and viewers.
// HandleMediaChangedL is called when the messaging storage has
// been moved into another disk, This means that the current
// message entry is not valid any more. This function creates
// new entry in the new messaging store, in order to enable saving
// the message. The new entry is left in InPreparation state, so
// that it will be deleted if the application (e.g. viewer) does
// not save anything in it.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleMediaChangedL()
    {
    if( iMmcFeatureSupported )
        {
        // create new InPreparation entry in the new messaging store.
        CMsgEditorDocument* doc = Document();
        doc->CreateNewL( doc->DefaultMsgService(), doc->DefaultMsgFolder() );
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleMediaUnavailableL
//
// Default error handling for all messaging editors and viewers.
// The media state is recorded in model and default action in AppUi
// is to display warning message to the user.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleMediaUnavailableL()
    {
    if( iMmcFeatureSupported )
        {
        ProcessCommandL( EAknCmdExit );
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleMediaAvailableL
//
// Default error handling for all messaging editors and viewers.
// No action made by default, the media state is recorded in model.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleMediaAvailableL()
    {
    // no action
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleMediaIncorrectL
//
// Default error handling for all messaging editors and viewers.
// The media state is recorded in model, and warning note is displayed
// to the user here.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleMediaIncorrectL()
    {
    if( iMmcFeatureSupported )
        {
        ViewErrorL( KMsvMediaIncorrect );
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleCorruptedIndexRebuildingL
//
// Default error handling for all messaging editors and viewers.
// The default action is to put busy note on the screen. The busy
// note will be displayed while the index is being rebuilt - until
// we get "index rebuilt" event in HandleCorruptedIndexRebuiltL().
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleCorruptedIndexRebuildingL()
    {
    HBufC* line = iEikonEnv->AllocReadResourceLC( R_MEB_N_IN_REBUILDING_INDEX );
    iEikonEnv->BusyMsgL( *line );
    CleanupStack::PopAndDestroy( line );  // line
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleCorruptedIndexRebuiltL
//
// Default error handling for all messaging editors and viewers.
// The default action is to remove the busy note displayed when
// the index rebuilding event was received.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleCorruptedIndexRebuiltL()
    {
    iEikonEnv->BusyMsgCancel();
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::CreateCustomControlL
//
// Default implementation is NULL since viewers do not need this.
// ---------------------------------------------------------
//
EXPORT_C CMsgBaseControl* CMsgEditorAppUi::CreateCustomControlL( TInt /*aControlType*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::IsLaunchedL
//
// Checks if document has finished launching.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgEditorAppUi::IsLaunchedL() const
    {
    return Document()->IsLaunched();
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::EditorObserver
//
// Default implementation.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::EditorObserver(
    TMsgEditorObserverFunc aFunc,
    TAny* /*aArg1*/,
    TAny* aArg2,
    TAny* /*aArg3*/ )
    {
    switch (aFunc)
        {
        case EMsgDenyClipboardOperation:
            {
            TInt* ret = static_cast<TInt*>( aArg2 );
            *ret = 0;
            }
            break;

        case EMsgHandleFocusChange:
        default:
            break;
        }
    }


// ---------------------------------------------------------
// CMsgEditorAppUi::DoMtmCmdL
//
// Get services from mtm's and offers them to APP UI.
// ---------------------------------------------------------
//
void CMsgEditorAppUi::DoMtmCmdL( const TUid& /*aQuery*/, TInt aCmd ) const
    {
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->AppendL( Document()->Entry().Id() );
    TBuf8<1> null;
    TMsvId origMessageId = Document()->Entry().Id();
    Document()->MtmUi().InvokeSyncFunctionL( aCmd, *selection, null );
    CleanupStack::PopAndDestroy( selection );  // selection
    Document()->SetEntryL( origMessageId );
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::ViewErrorL
//
// Helper function for Handle... methods.
// ---------------------------------------------------------
//
void CMsgEditorAppUi::ViewErrorL( TInt aTitle )
    {
    // TODO: check if app in foreground and show error note only then.
    CErrorUI* errorUI = CErrorUI::NewLC( *iCoeEnv );
    errorUI->ShowGlobalErrorNoteL( aTitle );
    CleanupStack::PopAndDestroy( errorUI ); // errorUI
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleResourceChangeL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleResourceChangeL(TInt aType)
    {
    CAknAppUi::HandleResourceChangeL( aType );
    
    if( ( aType == KAknSplitInputEnabled ) || 
        ( aType == KAknSplitInputDisabled ) )
         { 
        TBool splitDisabled(aType == KAknSplitInputDisabled);
         if (CurrentFixedToolbar())
             {
             CurrentFixedToolbar()->SetToolbarVisibility( splitDisabled );
             }
         StatusPane()->MakeVisible( splitDisabled );
         iView->HandleResourceChange( KEikDynamicLayoutVariantSwitch );
         }
    else if ( iView )
        {
        iView->HandleResourceChange( aType );
        }
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iStatusPaneRes = StatusPane()->CurrentLayoutResId();
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::SaveBodyL
//
//
// ---------------------------------------------------------
//
void CMsgEditorAppUi::SaveBodyL( CMsgBodyControl* aBody )
    {
    CEikRichTextEditor* bodyEditor = &aBody->Editor();
    if ( bodyEditor )
        {
        CRichText* richText = bodyEditor->RichText();
        if ( richText )
            {
            TInt len = richText->DocumentLength();
            if ( len )
                {
                HBufC* buf = HBufC::NewLC( len );
                TPtr ptr = buf->Des();
                richText->Extract( ptr, 0 );
                CNotepadApi::AddContentL( ptr );
                CleanupStack::PopAndDestroy( buf );
                }
            }
        } 
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::ShowCloseQueryL
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgEditorAppUi::ShowCloseQueryL()
    {
    TInt selectedIndex( 0 );
    CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &selectedIndex );
    dlg->PrepareLC( R_MEB_CLOSE_QUERY );
    if ( dlg->RunLD() )
        {
        return ( selectedIndex == 0 )
            ? EMsgCloseSave
            : EMsgCloseDelete;
        }
    else
        {
        return EMsgCloseCancel;
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::CreateSubjectPrefixStringL
// ---------------------------------------------------------
//
EXPORT_C HBufC* CMsgEditorAppUi::CreateSubjectPrefixStringL(
        const TDesC& aSubject,
        TBool aReply )
    {
    HBufC* newSubject = NULL;
    TInt subjectLength = aSubject.Length();
    if ( subjectLength >= 0 )
        {
        TInt formatResource = aReply
            ? R_MSG_REPLY_PREFIX
            : R_MSG_FORWARD_PREFIX;

        HBufC* subjectFormat = StringLoader::LoadLC(
            formatResource,
            iCoeEnv );
        
        HBufC* filteredSubject = RemovePrefixesL( aSubject );    
        CleanupStack::PushL( filteredSubject );
        
        if ( filteredSubject->Length() > 0 )
        	{
	        // Create a buffer large enough to hold the re-formated subject
	        newSubject = HBufC::NewL( subjectLength + subjectFormat->Length());
	        
	        TPtr ptr( newSubject->Des() );            
	        ptr.Append( *subjectFormat );
	        ptr.Append( *filteredSubject );	        
        	}
        else
        	{
        	newSubject = HBufC::NewL( 0 );
        	}
        
        CleanupStack::PopAndDestroy( filteredSubject );
        CleanupStack::PopAndDestroy( subjectFormat );
        }
    return newSubject;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::RemovePrefixesL
// ---------------------------------------------------------
//
HBufC* CMsgEditorAppUi::RemovePrefixesL( const TDesC& aSubject )
    {
    // filter setup
    RPointerArray<TDesC> prefixArray;
    CleanupClosePushL( prefixArray );
    
    // add prefixes to remove
    TPtrC filterRe( KMsgEditorAppUiFilterRe );
    prefixArray.AppendL( &filterRe );  
    
    TPtrC filterFw( KMsgEditorAppUiFilterFw );
    prefixArray.AppendL( &filterFw );
    
    TPtrC filterFwd( KMsgEditorAppUiFilterFwd );
    prefixArray.AppendL( &filterFwd ); 
    
    // corresponding prefixes for the current language variant
    HBufC* prefixReply = StringLoader::LoadLC( R_MSG_REPLY_PREFIX, iCoeEnv );
    TPtr prefixReplyPtr = prefixReply->Des();
    prefixReplyPtr.Trim();        
    prefixArray.AppendL( &prefixReplyPtr );
    
    HBufC* prefixForward = StringLoader::LoadLC( R_MSG_FORWARD_PREFIX, iCoeEnv );
    TPtr prefixForwardPtr = prefixForward->Des();
    prefixForwardPtr.Trim();
    prefixArray.AppendL( &prefixForwardPtr );
    
    // Remove prefixes 
    HBufC* subject = aSubject.AllocL();
    TPtr ptr( subject->Des() );
    TBool prefixFound;
    // Search until no prefixes found
    do
        {
        // Loop all prefixes
        prefixFound = EFalse;
        ptr.TrimLeft();
        for ( TInt i = 0; i < prefixArray.Count(); i++ )  
            {
            TPtrC prefix( *prefixArray[i] );
            
            // Remove prefixes while same prefix found at begining of subject    
            while ( ptr.FindC( prefix ) == 0 )
                {
                ptr.Delete( 0, prefix.Length() ); 
                prefixFound = ETrue;
                ptr.TrimLeft();
                }
            }
        } while ( prefixFound );
    
    CleanupStack::PopAndDestroy(3, &prefixArray ); 
    
    return subject;        
    }    
// ---------------------------------------------------------
// CMsgEditorAppUi::LockEntryL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::LockEntryL()
    {
    if ( iLock == NULL )
        {
        iLock = CLock::LockL( *Document() );
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::UnlockEntry
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::UnlockEntry()
    {
    delete iLock;
    iLock = NULL;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::IsLockedEntry
//
//
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgEditorAppUi::IsLockedEntry() const
    {
    return iLock != NULL;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::Exit
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::Exit( TInt aReason /*= EEikCmdExit*/ )
	{
#ifdef RD_MSG_FAST_PREV_NEXT
    // Communicate message id via publish and subscribe to MCE
    RProperty::Set( KPSUidMuiu, KMuiuKeyCurrentMsg,
        Document()->Entry().Id() );
#endif // RD_MSG_FAST_PREV_NEXT

	CEikAppServer* server = iEikonEnv->AppServer();
	if ( server )
        {
		server->NotifyServerExit( aReason );
        }
	CEikAppUi::Exit();
	}

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleLocalZoomChangeL
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleLocalZoomChangeL( TMsgCommonCommands aNewZoom )
    {
    TInt localZoom = 0;
    TInt oldLocalZoom = EAknUiZoomAutomatic;
    CRepository* repository = CRepository::NewL( KCRUidMessagingUiSettings );
    TInt ret = repository->Get( KMessagingEditorLocalUiZoom, oldLocalZoom );
    if ( ret == KErrNone )
        {
        switch( aNewZoom )
            {
            case EMsgDispSizeAutomatic:
                {
                localZoom = EAknUiZoomAutomatic;
                break;
                }
            case EMsgDispSizeLarge:
                {
                localZoom = EAknUiZoomLarge;
                break;
                }
            case EMsgDispSizeNormal:
                {
                localZoom = EAknUiZoomNormal;
                break;
                }
            case EMsgDispSizeSmall:
                {
                localZoom = EAknUiZoomSmall;
                break;
                }
            default:
                break; 
            }
        if( oldLocalZoom != localZoom )
            {
            ret = repository->Set( KMessagingEditorLocalUiZoom, localZoom );
            __ASSERT_DEBUG( !ret, Panic( EMsgRepositorySettingFailure ) );
            TWsEvent event; 
            event.SetType( KAknLocalZoomLayoutSwitch ); 
            ret = iEikonEnv->WsSession().SendEventToAllWindowGroups( event );
            }
        }

    delete repository;          
    }
    
// ---------------------------------------------------------
// CMsgEditorAppUi::CreateViewerNaviPaneL
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::CreateViewerNaviPaneL( TTime aTime, 
                                                      TMsgEditorMsgPriority aPriority,
                                                      TBool aUtcTime )
    {
    CAknNavigationControlContainer* naviContainer = 
        static_cast<CAknNavigationControlContainer*>( StatusPane()->ControlL( 
                                                            TUid::Uid( EEikStatusPaneUidNavi ) ) );
    if ( !iNaviDecorator )
        {
        CMsgNaviPaneControl* naviPaneControl = CMsgNaviPaneControl::NewL( naviContainer );
        naviPaneControl->SetNavigationControlObserver( this );
        
        // naviPaneControl ownership transferred to iNaviDecorator.
        iNaviDecorator = CAknNavigationDecorator::NewL( naviContainer, naviPaneControl );
        
        naviPaneControl->SetTimeIndicatorL( aTime, aUtcTime );
        naviPaneControl->SetPriorityIndicatorL( aPriority );
        
        CMsgEditorDocument* doc = Document();
        naviPaneControl->SetNavigationIndicatorL( doc->Session(), doc->Entry() );
        
        // this function will always be called when viwer is launched
        // always navi decorater will be created.
        // refer navi decorator iterator instance here
        iIterator =  naviPaneControl->GetMessageIterator();
        iIterator->SetMessageIteratorObserver( this );
        
        iNaviDecorator->SetContainerWindowL( *naviContainer );
        iNaviDecorator->MakeScrollButtonVisible( EFalse );
        iNaviDecorator->SetComponentsToInheritVisibility( ETrue );
        iOwnNaviPane = NULL;
        }
    else
        {
        User::Leave( KErrAlreadyExists );
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleNavigationControlEventL
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleNavigationControlEventL( TMsgNaviControlEventId aEvent )
    {
    if ( aEvent ==  MMsgNaviPaneControlObserver::EMsgNaviLeftArrowPressed ||
         aEvent == MMsgNaviPaneControlObserver::EMsgNaviRightArrowPressed )
        {
        if ( IsNextMessageAvailableL( aEvent == MMsgNaviPaneControlObserver::EMsgNaviRightArrowPressed ) )
            {
            /* no need for separate checks for right and left arrows
            because IsNextMessageAvailableL() and NextMessageL
            are called with the truth-value of the same comparison */
            NextMessageL( aEvent == MMsgNaviPaneControlObserver::EMsgNaviRightArrowPressed );
            } 
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::CLock::LockL
//
//
// ---------------------------------------------------------
//
CMsgEditorAppUi::CLock* CMsgEditorAppUi::CLock::LockL(
    const CMsgEditorDocument& aDoc)
    {
    CLock* lock = new ( ELeave ) CLock();
    CleanupStack::PushL( lock );
    
    lock->iStore = aDoc.CurrentEntry().EditStoreL();
    lock->iStream.AssignL( *( lock->iStore ), KMsvEntryRichTextBody );
    
    CleanupStack::Pop( lock );
    return lock;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::CLock::CLock
//
//
// ---------------------------------------------------------
//
CMsgEditorAppUi::CLock::CLock()
    {
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::CLock::~CLock
//
//
// ---------------------------------------------------------
//
CMsgEditorAppUi::CLock::~CLock()
    {
    if ( iStore != NULL )
        {
        iStream.Release();
        iStream.Close();
        iStore->Revert();
        delete iStore;
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::CanFastOpenL
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgEditorAppUi::CanFastOpenL( const CMessageIterator& /*aIterator*/ )
    {
    return EFalse;
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::AknInputBlockCancel
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::AknInputBlockCancel()
    {
    if ( iInputBlocker )
        {
        iInputBlocker->MakeVisible( EFalse );
        iInputBlocker->ControlEnv()->AppUi()->RemoveFromStack( iInputBlocker );
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::SetTitleIconSizeL
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::SetTitleIconSizeL( CFbsBitmap* aTitleBitmap )
    {
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ETitlePane, mainPane );
    TAknLayoutRect titleIconPaneLayoutRect;
    
    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        titleIconPaneLayoutRect.LayoutRect( mainPane,
                                            AknLayoutScalable_Avkon::title_pane_stacon_g1( 0 ).LayoutLine() );
        }
    else
        {
        titleIconPaneLayoutRect.LayoutRect( mainPane,
                                            AknLayoutScalable_Avkon::title_pane_g2( 0 ).LayoutLine() );
        }
    
    TSize iconSize = titleIconPaneLayoutRect.Rect().Size();

    User::LeaveIfError( AknIconUtils::SetSize( aTitleBitmap, iconSize, EAspectRatioPreserved ) );
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::HandleStatusPaneSizeChange
//
// Performs layout switch on view area if statuspane is changed to 
// same statuspane that is used by CMsgEditorAppUi. This is for
// correcting the layout if layout switch was performed when 
// SMIL player or SMIL editor that use different statuspane
// were displayed. Without this the layout would be shown as if
// statuspane that SMIL components use would be used.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorAppUi::HandleStatusPaneSizeChange()
    {
    CAknAppUi::HandleStatusPaneSizeChange();
    }

//  End of File
