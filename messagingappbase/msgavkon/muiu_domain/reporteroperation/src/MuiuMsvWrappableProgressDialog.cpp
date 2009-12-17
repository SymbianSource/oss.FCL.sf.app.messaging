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
*     Operation progress reporting dialog class.
*     Encapsulates the display and update of progress information.
*
*/




// INCLUDE FILES
#include "MuiuMsvWrappableProgressDialog.h"
#include "MuiuMsvProgressReporterOperation.h"
#include "muiudomainpan.h"
#include "MuiuLogText.h"
#include <coemain.h>
#include <eiklabel.h>
#include <eikmover.h>
#include <eikprogi.h>
#include <eikbtgpc.h>
#include <eikbtgrp.h>   // MeikButtonGroup
#include <eikimage.h>
#include <bautils.h>    // BaflUtils
#include <eikenv.h>
#include <eikrutil.h>
#include <mtmuibas.h>
#include <MTMStore.h>
#include <msvuids.h>
#include <muiu.rsg>
#include <opprgdlg.hrh>
#include <StringLoader.h>
#include <aknstaticnotedialog.h>
#include <avkon.hrh>
#include <gulfont.h>
#include <AknIconUtils.h>  // AknIconUtils::AvkonIconFileName()
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include <aknnotecontrol.h>              // CAknNoteControl
#include <avkon.rsg>                     // R_QGN_GRAF_WAIT_BAR_ANIM
#include <AknsUtils.h>     // SkinInstance
#include <AknsConstants.h> // KAknsIIDQgnNoteErased, KAknsIIDQgnNoteMove, KAknsIIDQgnNoteProgress
                           // KAknsIIDQgnNoteCopy, KAknsIIDQgnNoteMail
#include <avkon.mbg> // EMbmAvkonQgn_note_erased, EMbmAvkonQgn_note_move, 
                     // EMbmAvkonQgn_note_progress,
                     // EMbmAvkonQgn_note_copy, EMbmAvkonQgn_note_mail


// CONSTANTS
const TInt KOneItem = 1;
// We need to make sure that the timer runs regardless of what the app is doing.
const TInt KPrgOpTimerPriority = CActive::EPriorityHigh;
const TInt KMuiuLocalProgressEntriesDoneReplaceIndex = 0;
const TInt KMuiuLocalProgressTotalEntriesReplaceIndex = 1;
_LIT(KDirAndMuiuResFileName,"muiu.rsc");



// ============================ MEMBER FUNCTIONS ===============================



// ---------------------------------------------------------
// CMsvWrappableProgressDialog::NewL
//
// ---------------------------------------------------------
//
CMsvWrappableProgressDialog* CMsvWrappableProgressDialog::NewL( CMtmStore& aMtmStore, 
                                                          TInt aImageIndex /*= KErrNotFound */, 
                                                          TBool aProgressVisibilityDelayOff,
                                                          TBool aDisplayWaitAnimation )
    {
    CMsvWrappableProgressDialog* dlg = new( ELeave ) CMsvWrappableProgressDialog( 
        aMtmStore, EOpPrgDlgCIDProgress, aImageIndex, aProgressVisibilityDelayOff, 
        aDisplayWaitAnimation );
    
    CleanupStack::PushL( dlg );
    TParse parse;
    parse.Set( KDirAndMuiuResFileName, &KDC_RESOURCE_FILES_DIR, NULL ); 
    TFileName fileName( parse.FullName() );
    dlg->iResources.OpenL( fileName );
    CleanupStack::Pop( dlg );
    if ( aDisplayWaitAnimation )
        {
        dlg->ExecuteLD( R_OPERATION_WAITANIMATION_DIALOG );//CAknNoteDialog::ENoTone,
        }
    else
        {
        dlg->ExecuteLD( R_OPERATION_PROGRESS_DIALOG );//CAknNoteDialog::ENoTone,
        }
    return dlg;
    }
    
    
    
// ---------------------------------------------------------
// CMsvWrappableProgressDialog::NewL
// This method is used only when progress dialog is shown 
// also in cover side.
// ---------------------------------------------------------
//
CMsvWrappableProgressDialog* CMsvWrappableProgressDialog::NewL( CMtmStore& aMtmStore,
                                                          TInt aDialogIndex, 
                                                          TUid aCategoryUid,
                                                          TInt aImageIndex /*= KErrNotFound */, 
                                                          TBool aProgressVisibilityDelayOff,
                                                          TBool aDisplayWaitAnimation )
    {
    CMsvWrappableProgressDialog* dlg = new( ELeave ) CMsvWrappableProgressDialog( 
        aMtmStore, EOpPrgDlgCIDProgress, aImageIndex, aProgressVisibilityDelayOff, 
        aDisplayWaitAnimation );
    
    CleanupStack::PushL( dlg );
    TParse parse;
    parse.Set( KDirAndMuiuResFileName, &KDC_RESOURCE_FILES_DIR, NULL ); 
    TFileName fileName( parse.FullName() );
    dlg->iResources.OpenL( fileName );
    dlg->PublishDialogL( aDialogIndex, aCategoryUid  );
    CleanupStack::Pop( dlg );
    if ( aDisplayWaitAnimation )
        {
        dlg->ExecuteLD( R_OPERATION_WAITANIMATION_DIALOG );//CAknNoteDialog::ENoTone,
        }
    else
        {
        dlg->ExecuteLD( R_OPERATION_PROGRESS_DIALOG );//CAknNoteDialog::ENoTone,
        }
    return dlg;
    }


// ---------------------------------------------------------
// CMsvWrappableProgressDialog::~CMsvWrappableProgressDialog
//
// ---------------------------------------------------------
//
CMsvWrappableProgressDialog::~CMsvWrappableProgressDialog()
    {
    MUIULOGGER_ENTERFN( "CMsvWrappableProgressDialog::~CMsvWrappableProgressDialog" );
    
    CleanBeforeFinish();
    iResources.Close();
    // iOperation not own by CMsvWrappableProgressDialog
    MUIULOGGER_LEAVEFN( "CMsvWrappableProgressDialog::~CMsvWrappableProgressDialog" );
    }



// ---------------------------------------------------------
// CMsvWrappableProgressDialog::SetOperationL
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::SetOperationL( CMsvOperation* aOp )
    {
    MUIULOGGER_ENTERFN( "CMsvWrappableProgressDialog::SetOperationL" );
    __ASSERT_DEBUG( aOp, Panic( EMuiuOpPrgNullOperation ) );
    MUIULOGGER_WRITE_FORMAT( "iOperation: 0x%x", aOp );
    iOperation = aOp;
    if( !iTimer )
        {
        iTimer = CMsvRemoteOperationProgress::NewL( *this, 
                                                    KPrgOpTimerPriority, 
                                                    KRemoteOpProgressDefaultPeriod );
        }
    if( iSeed )
        {
        UpdateRemoteOpProgressL();
        }
    MUIULOGGER_LEAVEFN( "CMsvWrappableProgressDialog::SetOperationL" );
    }



// ---------------------------------------------------------
// CMsvWrappableProgressDialog::SetTitleTextL
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::SetTitleTextL( const TDesC& aText )
    {
    SetTextL( aText );
    }


// ---------------------------------------------------------
// CMsvWrappableProgressDialog::SetTitleTextL
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::SetTitleTextL( TInt aResource )
    {
    HBufC* text = iEikonEnv->AllocReadResourceLC( aResource );
    SetTitleTextL( *text );
    CleanupStack::PopAndDestroy( text ); // text
    }



// ---------------------------------------------------------
// CMsvWrappableProgressDialog::SetCancel
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::SetCancel( TBool aAllowCancel )
    {
    iAllowCancel = aAllowCancel;
    // Hide cancel button if there is one
    MEikButtonGroup* btg = ButtonGroupContainer().ButtonGroup();
    if ( btg )
        {
        if ( btg->CommandPos( EMuiuProgressDialogButtonCancelId ) != KErrNotFound )
            {
            MakePanelButtonVisible( EMuiuProgressDialogButtonCancelId, aAllowCancel );
            }
        else if ( btg->CommandPos( EAknSoftkeyCancel ) != KErrNotFound )
            {
            MakePanelButtonVisible( EAknSoftkeyCancel, aAllowCancel );
            }
        }
    }


// ---------------------------------------------------------
// CMsvWrappableProgressDialog::SetAnimationL
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::SetAnimationL(TInt /*aResourceId*/) const
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CMsvWrappableProgressDialog::CreateCustomControlL
//
// ---------------------------------------------------------
//
SEikControlInfo CMsvWrappableProgressDialog::CreateCustomControlL( TInt aControlType )
    {
    SEikControlInfo controlInfo;
    controlInfo = CEikDialog::CreateCustomControlL( aControlType );
    return controlInfo;
    }


// ---------------------------------------------------------
// CMsvWrappableProgressDialog::GetHelpContext
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext = iHelpContext;
    }



// ---------------------------------------------------------
// CMsvWrappableProgressDialog::OkToExitL
//
// ---------------------------------------------------------
//
TBool CMsvWrappableProgressDialog::OkToExitL( TInt aButtonId )
    {
    if ( EAknSoftkeyDone == aButtonId )
        {
        return ETrue;
        }
    
    if( ( aButtonId == EMuiuProgressDialogButtonCancelId ) && ( iOperation ) && ( iAllowCancel ) )
        {
        iOperation->Cancel();
        iTimer->Cancel();           
        }   
    else if( ( aButtonId == EAknSoftkeyCancel ) && ( iOperation ) && ( iAllowCancel ) )
        {       
        iOperation->Cancel();
        iTimer->Cancel();
        if ( iProgressReporter )
            {
            iProgressReporter->SetCompleteWithExit( ETrue );
            }
        }
    else
        {
        //Does nothing
        }
    return EFalse;  // Don't exit, CMsvProgressReporterOperation is responsible for destroying us.
    }



// ---------------------------------------------------------
// CMsvWrappableProgressDialog::PostLayoutDynInitL
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::PostLayoutDynInitL()
    {
    UpdateRemoteOpProgressL();

    if ( iImageIndex != KErrNotFound )
        {
        CFbsBitmap* bitmap;
        CFbsBitmap* bitmapMask;
        MAknsSkinInstance* skins = AknsUtils::SkinInstance();
        CEikImage* image = new( ELeave ) CEikImage();
        CleanupStack::PushL( image );
        const TAknsItemID id = SkinItemId( iImageIndex);
        TFileName avkonIconFileNameAndPath;
        avkonIconFileNameAndPath = AknIconUtils::AvkonIconFileName();

        AknsUtils::CreateIconL( skins, id, bitmap,
           bitmapMask, avkonIconFileNameAndPath, iImageIndex, iImageIndex + 1 );
        image->SetPicture( bitmap,bitmapMask );
        SetImageL( image );
        CleanupStack::Pop( image ); // dialog now has the ownership
        }

    if ( iDisplayWaitAnimation )
        {
        CAknNoteControl* noteControl = NoteControl();
        if ( noteControl )
            {
            noteControl->StartAnimationL();
            iWaitAnimationStarted = ETrue;
            }
        }
    }


// ---------------------------------------------------------
// CMsvWrappableProgressDialog::PreLayoutDynInitL
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::PreLayoutDynInitL()
    {
    CEikProgressInfo* bar = GetProgressInfoL();
    if( bar )
        {
        bar->MakeVisible( EFalse );
        }

    CAknProgressDialog::PreLayoutDynInitL();
    if ( iDisplayWaitAnimation )
        {
        CAknNoteControl* noteControl = NoteControl();
        if ( noteControl )
            {
            noteControl->SetAnimationL( R_QGN_GRAF_WAIT_BAR_ANIM );
            }
        }
    
    }



// ---------------------------------------------------------
// CMsvWrappableProgressDialog::CleanBeforeFinish
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::CleanBeforeFinish()
    {
    if( iMtmUi )
        {
        iMtmStore.ReleaseMtmUi( iMtmUi->Type() );
        iMtmUi = NULL;
        }
    if( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        MUIULOGGER_WRITE_FORMAT( "iTimer 0x%x cancelled!", iTimer );
        iTimer = NULL;
        }
    
    MUIULOGGER_WRITE_FORMAT( "CleanBeforeFinish: iDialogDeletedFlag address 0x%x", 
                              iDialogDeletedFlag );
    if( iDialogDeletedFlag )
        {
        *iDialogDeletedFlag = ETrue;
        MUIULOGGER_WRITE_FORMAT( "CleanBeforeFinish: iDialogDeletedFlag value %d", 
                                  *iDialogDeletedFlag );
        }
    
    iOperation = NULL;
    iProgressReporter = NULL;
    }


// ---------------------------------------------------------
// CMsvWrappableProgressDialog::ProcessFinishedL
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::ProcessFinishedL()
    {
    // iOperation is deleted by CMsvProgressReporterOperation but this dialog
    // can be visible even after iOperation is deleted. That is why progress
    // must not be updated anymore UpdateRemoteOpProgressL anymore so need to cancel
    MUIULOGGER_ENTERFN( "CMsvWrappableProgressDialog::ProcessFinishedL" );
    CleanBeforeFinish();

    if( iState == EProcessOnDisplayOn )
        {
        iState = EProcessOnDisplayOnCanBeDismissed;
        }

    CAknProgressDialog::ProcessFinishedL();
    MUIULOGGER_LEAVEFN( "CMsvWrappableProgressDialog::ProcessFinishedL" );
    }


// ---------------------------------------------------------
// CMsvWrappableProgressDialog::UpdateRemoteOpProgressL
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::UpdateRemoteOpProgressL()
    {
    // Don't leave, not fatal.
    TRAPD( ignore, DoUpdateRemoteOpProgressL() );
    if( ignore )
        {
        ignore = KErrNone; //Prevent ARMV5 compilation varning
        }
    }


// ---------------------------------------------------------
// CMsvWrappableProgressDialog::DoUpdateRemoteOpProgressL
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::DoUpdateRemoteOpProgressL()
    {
    MUIULOGGER_ENTERFN( "CMsvWrappableProgressDialog::DoUpdateRemoteOpProgressL" );
    if( iOperation )
        {
        MUIULOGGER_WRITE_FORMAT( "iOperation: 0x%x", iOperation );
        MUIULOGGER_WRITE_TIMESTAMP( "Time: " );
        if( !iOperation->IsActive() )
            {
            MUIULOGGER_LEAVEFN( "CMsvWrappableProgressDialog::DoUpdateRemoteOpProgressL" );
            return;
            }
        
        
        TInt err = KErrNone;
        TInt entryCount = 0;
        TInt entriesDone = 0;
        TInt currentEntrySize = -1;
        TInt currentBytesTrans = -1;
        TBuf<CBaseMtmUi::EProgressStringMaxLen> progressString;
        
        if( iOperation->Mtm() == KUidMsvLocalServiceMtm )
            {
            TMsvLocalOperationProgress localprogress=McliUtils::GetLocalProgressL( *iOperation );
            TInt rid = 0;
            const TBool oneItem = localprogress.iTotalNumberOfEntries == KOneItem;
            currentEntrySize = entryCount = localprogress.iTotalNumberOfEntries;
            currentBytesTrans = entriesDone = localprogress.iNumberCompleted;   
            err = localprogress.iError;
            switch ( localprogress.iType )
                {
                case TMsvLocalOperationProgress::ELocalMove:
                    rid = 
                    oneItem ? R_MUIU_LOCAL_PROGRESS_MOVING_1 : R_MUIU_LOCAL_PROGRESS_MOVING_X;
                    break;
                case TMsvLocalOperationProgress::ELocalDelete:
                    {
                    //If we are deleting one message or a folder, 
                    //string is set in mceui.cpp HandleDeleteL()
                    //because we don't have the visibility here wether 
                    //we are deleting a message or a folder.
                    //Here we take care only of multiple file deletion string.
                    if( !oneItem )
                        {
                        rid = R_MUIU_LOCAL_PROGRESS_DELETING_X;
                        }
                    }
                    break;
                case TMsvLocalOperationProgress::ELocalCopy: // no local copy
                case TMsvLocalOperationProgress::ELocalNew:  // display nothing currently
                case TMsvLocalOperationProgress::ELocalChanged:
                case TMsvLocalOperationProgress::ELocalNone:
                default:
                    ;
#if defined (_DEBUG)
                    Panic(EMuiuOpPrgBadLocalOpType);
#endif
                }
            if ( rid )
                {
                StringLoader::Load( progressString, rid, iCoeEnv );
                if ( entryCount > 1 )
                    {
                    TBuf<CBaseMtmUi::EProgressStringMaxLen> tempBuffer;
                    StringLoader::Format( 
                        tempBuffer, 
                        progressString, 
                        KMuiuLocalProgressEntriesDoneReplaceIndex, 
                        entriesDone );
                    StringLoader::Format( 
                        progressString, 
                        tempBuffer, 
                        KMuiuLocalProgressTotalEntriesReplaceIndex, 
                        entryCount );
                    }
                }
            }
        else
            {
            TBool dialogDeleted = EFalse;
            iDialogDeletedFlag=&dialogDeleted;
            const TPtrC8& progressBuf = iOperation->ProgressL();
            if( dialogDeleted )
                {
                return; //  Everything has gone
                }
            iDialogDeletedFlag = NULL;
            
            err = iDecoder->DecodeProgress( progressBuf, 
                                            progressString, 
                                            entryCount, 
                                            entriesDone, 
                                            currentEntrySize, 
                                            currentBytesTrans, 
                                            ETrue );
            }
        
        if( err == KErrNone )
            {
            if( !currentEntrySize )
                {
                currentEntrySize = entryCount;
                currentBytesTrans = entriesDone;
                }
            if ( progressString.Length() )
                {
                UpdateLabelText( progressString );
                }
            MUIULOGGER_WRITE_FORMAT( "entryCount: %d", entryCount );
            MUIULOGGER_WRITE_FORMAT( "entriesDone: %d", entriesDone );
            MUIULOGGER_WRITE_FORMAT( "currentBytesTrans: %d", currentBytesTrans );
            MUIULOGGER_WRITE_FORMAT( "currentEntrySize: %d", currentEntrySize );
            UpdateProgressBarL( currentEntrySize, currentBytesTrans );
            }
        }
        MUIULOGGER_LEAVEFN( "CMsvWrappableProgressDialog::DoUpdateRemoteOpProgressL" );
    }
    
    
    
// ---------------------------------------------------------
// CMsvWrappableProgressDialog::DecodeProgress
//
// ---------------------------------------------------------
//
TInt CMsvWrappableProgressDialog::DecodeProgress(const TDesC8& aProgress, 
                TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString, 
                TInt& aTotalEntryCount, TInt& aEntriesDone, 
                TInt& aCurrentEntrySize, TInt& aCurrentBytesTrans, TBool /*aInternal*/)
    {
    TInt err = KErrNone;
    TRAPD( leaveErr, err = DoDecodeProgressL( aProgress, 
                                              aReturnString, 
                                              aTotalEntryCount, 
                                              aEntriesDone,
                                              aCurrentEntrySize, 
                                              aCurrentBytesTrans ) );
    if( KErrNone == leaveErr )
        {
        return err;
        }
    return leaveErr;
    }
    
    
    
// ---------------------------------------------------------
// CMsvWrappableProgressDialog::DoDecodeProgressL
//
// ---------------------------------------------------------
//
TInt CMsvWrappableProgressDialog::DoDecodeProgressL( const TDesC8& aProgress, 
        TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString, 
        TInt& aTotalEntryCount, TInt& aEntriesDone, 
        TInt& aCurrentEntrySize, TInt& aCurrentBytesTrans )
    {
    TUid opType = iOperation->Mtm();
    if( !iMtmUi )
        {
        iMtmUi = &(iMtmStore.ClaimMtmUiL(opType));
        }
    else if( iMtmUi->Type() != opType )
        {
        iMtmStore.ReleaseMtmUi( iMtmUi->Type() );
        iMtmUi = NULL;
        iMtmUi = &( iMtmStore.ClaimMtmUiL(opType) );
        }
    else
        {
        //Does nothing
        }
    return iMtmUi->GetProgress( aProgress, aReturnString, aTotalEntryCount, aEntriesDone,
        aCurrentEntrySize, aCurrentBytesTrans );
    }
    
    
    
// ---------------------------------------------------------
// CMsvWrappableProgressDialog::CMsvWrappableProgressDialog
//
// ---------------------------------------------------------
//
CMsvWrappableProgressDialog::CMsvWrappableProgressDialog( CMtmStore& aMtmStore, 
                                                          TInt /*aNoteControlId*/, 
                                                          TInt aImageIndex )
    : CAknProgressDialog( NULL ),
    iMtmStore( aMtmStore ), 
    iResources( *iCoeEnv ), 
    iAllowCancel( ETrue ), 
    iSeed( ETrue ),
    iImageIndex( aImageIndex )
    {
    iDecoder = this;  //  _We_ are the default progress decoder
    }

    
// ---------------------------------------------------------
// CMsvWrappableProgressDialog::CMsvWrappableProgressDialog
//
// ---------------------------------------------------------
//
CMsvWrappableProgressDialog::CMsvWrappableProgressDialog( CMtmStore& aMtmStore, 
                                                          TInt /*aNoteControlId*/, 
                                                          TInt aImageIndex, 
                                                          TBool aProgressVisibilityDelayOff,
                                                          TBool aDisplayWaitAnimation )
    : CAknProgressDialog( NULL, aProgressVisibilityDelayOff ),
    iMtmStore( aMtmStore ), 
    iResources( *iCoeEnv ), 
    iAllowCancel( ETrue ), 
    iSeed( ETrue ),
    iImageIndex( aImageIndex ),
    iDisplayWaitAnimation( aDisplayWaitAnimation )
    {
    iDecoder = this;  //  _We_ are the default progress decoder
    }
    
    
// ---------------------------------------------------------
// CMsvWrappableProgressDialog::UpdateLabelText
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::UpdateLabelText( const TDesC& aProgressString )
    {
    TRAPD( ignore, SetTextL( aProgressString ) );
    if( ignore )
        {
        ignore = KErrNone; //Prevent ARMV5 compilation varning
        }
    }

    
    
// ---------------------------------------------------------
// CMsvWrappableProgressDialog::UpdateProgressBarL
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::UpdateProgressBarL( TInt aTotal, TInt aCurrent )
    {
    CEikProgressInfo* bar = GetProgressInfoL();
    if( bar )
        {
        if( (aTotal <= 0 ) && (aCurrent <= 0 ) )
            {
            bar->MakeVisible( EFalse );       // hide the bar.
            if ( iDisplayWaitAnimation  && !iWaitAnimationStarted )
                {
                // If not used iWaitAnimationStarted then start animation
                // many times and animation is not shown properly
                CAknNoteControl* noteControl = NoteControl();
                if ( noteControl )
                    {
                    noteControl->StartAnimationL();
                    iWaitAnimationStarted = ETrue;
                    }
                }
            return;
            }
        if( !bar->IsVisible() )
            {
            if ( aTotal > 0 && aTotal == aCurrent )
                {
                return;
                }
            if ( iDisplayWaitAnimation )
                {
                CAknNoteControl* noteControl = NoteControl();
                if ( noteControl )
                    {
                    noteControl->CancelAnimation();
                    iWaitAnimationStarted = EFalse;
                    }
                }
            bar->MakeVisible( ETrue );
            }
        bar->SetFinalValue( aTotal );
        bar->SetAndDraw( aCurrent );
        }
    }

    
// ---------------------------------------------------------
// CMsvWrappableProgressDialog::SetVisibilityDelay
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::SetVisibilityDelay( TBool aDelay )
    {
    if ( aDelay ) 
        { 
        iVisibilityDelayOff = EFalse; 
        } 
    else 
        { 
        iVisibilityDelayOff = ETrue; 
        MakeVisible( ETrue ); 
        }
    }


    
// ---------------------------------------------------------
// CMsvWrappableProgressDialog::SetProgressReporter
//
// ---------------------------------------------------------
//
void CMsvWrappableProgressDialog::SetProgressReporter( 
                                  CMsvProgressReporterOperation* aProgressReporter )
    {
    iProgressReporter = aProgressReporter;
    }



// ---------------------------------------------------------
// CMsvWrappableProgressDialog::SkinItemId
// 
// ---------------------------------------------------------
//
const TAknsItemID CMsvWrappableProgressDialog::SkinItemId( const TInt aBitmapIndex ) const
    {
    switch ( aBitmapIndex )
        {
        case   EMbmAvkonQgn_note_erased:
            return KAknsIIDQgnNoteErased;
        case EMbmAvkonQgn_note_move:
            return KAknsIIDQgnNoteMove;
        case EMbmAvkonQgn_note_progress:
            return KAknsIIDQgnNoteProgress;
        case EMbmAvkonQgn_note_copy:
            return KAknsIIDQgnNoteCopy;
        case EMbmAvkonQgn_note_mail:
            return KAknsIIDQgnNoteMail;
        default: 
            return KAknsIIDQgnNoteProgress;
        }
    }

// End of file
    
