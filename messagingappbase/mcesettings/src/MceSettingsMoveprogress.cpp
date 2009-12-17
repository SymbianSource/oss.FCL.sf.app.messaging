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
*     Starts copy/move message store progress dialog and transfer handler
*
*/



// INCLUDE FILES


#include <eikprogi.h>           // CEikProgressInfo
#include <aknnotewrappers.h>    // CAknErrorNote

#include "MceSettingsMoveProgress.h"    // CMceMoveProgress
#include "MceSettingsGeneralSettingsDialog.h" // CMceGeneralSettingsDialog
#include "MceSettingsIds.hrh"
#include <MceSettings.rsg>

// CONSTANTS
const TInt KMceCancelButton        = 2;
const TInt KMceTransferComplete    = 100;

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//

// ----------------------------------------------------
// CMceMoveProgress::Constructor
//
//
// ----------------------------------------------------
CMceMoveProgress::CMceMoveProgress( CMsvSession*& aSession, 
                  MMsvSessionObserver& aObserver, 
                  TInt& aSource, 
                  const TInt& aTarget,
                  TBool aDeleteCopiedStore, 
                  CMceGeneralSettingsDialog& aGeneralSettings, 
                  TBool aDeleteOnly )
                    :iSession(aSession), iObserver(aObserver), iSource(aSource), iTarget(aTarget), 
                    iDeleteCopiedStore(aDeleteCopiedStore), iCopyOperation(ETrue), 
                    iGeneralSettings (aGeneralSettings), iDeleteOnly (aDeleteOnly)
    {
    __DECLARE_NAME(_S( "CMceMoveProgress") );
    }

// ----------------------------------------------------
// CMceMoveProgress::Constructor
//
//
// ----------------------------------------------------
CMceMoveProgress* CMceMoveProgress::NewL(
    CMsvSession*& aSession,
    MMsvSessionObserver& aObserver, TInt& aSource, const TInt& aTarget,
    TBool aDeleteCopiedStore, CMceGeneralSettingsDialog& aGeneralSettings, TBool aDeleteOnly )
    {
    CMceMoveProgress* self=new ( ELeave ) CMceMoveProgress( aSession, 
                                                                    aObserver, 
                                                                    aSource, 
                                                                    aTarget,
                                                                    aDeleteCopiedStore, 
                                                                    aGeneralSettings, 
                                                                    aDeleteOnly );
    CleanupStack::PushL( self );
    self->ConstructL(); 
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CMceMoveProgress::ConstructL
//
//
// ----------------------------------------------------
void CMceMoveProgress::ConstructL()
    {
    ;// Nothing
    }


// ----------------------------------------------------
// CMceMoveProgress::Destructor
//
//
// ----------------------------------------------------
CMceMoveProgress::~CMceMoveProgress()
    {
    delete iProgressTimer;
    iProgressTimer = NULL;
    delete iHandler;
    iHandler = NULL;
    }

// ----------------------------------------------------
// CMceMoveProgress::InitializingL
// Starts transfering message store
//
// ----------------------------------------------------
void CMceMoveProgress::InitializingL()
    {
    TRAPD( err2, TransferingL() );
    if ( err2 )
       {
       if ( iProgressTimer )
            {
            iProgressTimer->Cancel();
            }

       if ( iHandler )
            {
            iHandler->Cancel();
            }

        // debug this
        // obsolete
        iObserver.HandleSessionEventL( MMsvSessionObserver::EMsvServerReady, NULL, NULL, NULL ); 

        User::LeaveIfError( err2 );
        }
    }

// ----------------------------------------------------
// CMceMoveProgress::TransferingL
// Starts transfer thread
//
// ----------------------------------------------------
void CMceMoveProgress::TransferingL()
    {
    iProgressDialog = new ( ELeave ) CMceMoveProgressDialog( REINTERPRET_CAST( CEikDialog**,
                    &iProgressDialog ),ETrue );

    iProgressDialog->SetCallback( this );

    iProgressDialog->PrepareLC( R_MCE_MOVE_PROGRESS );
    iProgressInfo = iProgressDialog->GetProgressInfoL();

    iProgressInfo->SetFinalValue( KMceTransferComplete );

    // messaging progress watching class
    iProgressTimer=CMsvRemoteOperationProgress::NewL( *this );

    if ( !iDeleteCopiedStore )
       {
       // copy messages
       HBufC* text = StringLoader::LoadLC( R_MCE_COPYING_MESSAGES, CCoeEnv::Static() );
       iProgressDialog->SetCurrentLabelL( EMceSettingsMoveProgressBar, *text );
       CleanupStack::PopAndDestroy( text );
       }

    // Active object kicking off the message store transfer thread
    iHandler=CMceMessageTransHandler::NewL(*this, iSession, iTarget, iSource, iDeleteCopiedStore);

    // Set the operation: copying the store
    CMsvOperation* operation = NULL;
    TDriveUnit targetUnit(iTarget);
    operation = iSession->CopyStoreL( targetUnit, iHandler->iStatus );
    iHandler->SetOperation( operation );

    // show the messaging progress
    ExecuteLD();
    
#if 0
    if ( iDeleteCopiedStore )
    	{
    	// Delete the old store
    	TDriveUnit sourceUnit( iSource );
    	operation = iSession->DeleteStoreL( sourceUnit, iHandler->iStatus );
    	iHandler->SetOperation( operation );
    	}
#endif
    }


// ----------------------------------------------------
// CMceMoveProgress::UpdateRemoteOpProgressL
// from MMsvRemoteOperationProgressObserver
//
// ----------------------------------------------------
void CMceMoveProgress::UpdateRemoteOpProgressL()
    {
    //This doesn't leave - need L as it is virtual
    if ( !iHandler )
        {
        return;
        }
    const TInt percentageDone=iHandler->ProgressL();
    CEikProgressInfo* progressBar=iProgressDialog->GetProgressInfoL();
    progressBar->SetAndDraw( percentageDone );
    }

// ----------------------------------------------------
// CMceMoveProgress::DisableCancelL
// from MMceMsgTransHandlerObserver
//
// ----------------------------------------------------
void CMceMoveProgress::DisableCancelL()
    {
    CMceMoveProgressDialog* dialog = STATIC_CAST( CMceMoveProgressDialog*, iProgressDialog );
    dialog->DisableCancelL();
    }

// ----------------------------------------------------
// CMceMoveProgress::CopyCompleteL
// from MMceMsgTransHandlerObserver
// 
// ----------------------------------------------------
void CMceMoveProgress::CopyCompleteL( TInt aErr )
    {
    //This is actually a non-leaving function - need the L because it is a virtual function    
    // For  EVSG-7V89T5
    CMceMoveProgress::DisableCancelL();
    iGeneralSettings.SetTransferError( aErr );// Catches the error when failed moving message store( aErr );
    UpdateRemoteOpProgressL();
    if (aErr!=KErrNone)
    	{
    	if ( aErr == KErrDiskFull )
       		{
       		CAknQueryDialog* lowmemoryDialog = CAknQueryDialog::NewL();
       		// Not enough memory
       		lowmemoryDialog->ExecuteLD( R_MCE_NOT_ENOUGH_MEMORY );
       		}
    	else
        	{
        	// Other error cases: KErrAccessDenied 
        	// Todo: if necessary add to CMceGeneralSettingsDialog::MoveMessageStoreL
        	// in order to prevent messaging close.        	
        	HBufC* text = StringLoader::LoadLC( R_MCE_SETTINGS_CANNOT_MOVE_STORE, 
        	                                                           CCoeEnv::Static() );
        	CAknErrorNote* note = new (ELeave) CAknErrorNote();
        	note->ExecuteLD(*text);
        	CleanupStack::PopAndDestroy( text );
        	}
    	}
    else
    	{
    	// The store has has been copied. Only change message store on success
#ifdef RD_MULTIPLE_DRIVE
        iGeneralSettings.ChangeMessageStoreL( iTarget );
#else
        iGeneralSettings.ChangeMessageStoreL();
#endif //RD_MULTIPLE_DRIVE
        iGeneralSettings.UpdateMemorySelectionL();
    	iSource=iTarget;
    	}    
    }

// ----------------------------------------------------
// CMceMoveProgress::TransferCompleteL
// from MMceMsgTransHandlerObserver
//
// ----------------------------------------------------
void CMceMoveProgress::TransferCompleteL( TInt /*aErr*/ )
    {
    iProgressTimer->Cancel();
    iHandler->Cancel();

    iObserver.HandleSessionEventL( MMsvSessionObserver::EMsvServerReady, NULL, NULL, NULL );

    iProgressDialog->ProcessFinishedL();

    delete iProgressDialog;
    iProgressDialog = NULL;

    // Copy operation competed
    iCopyOperation = EFalse;

    delete this;
    }

// ----------------------------------------------------
// CMceMoveProgress::DialogDismissedL
// from MProgressDialogCallback
//
// ----------------------------------------------------
void CMceMoveProgress::DialogDismissedL( TInt aButtonId )
    {
    iProgressTimer->Cancel();
    iHandler->Cancel();

    iObserver.HandleSessionEventL( MMsvSessionObserver::EMsvServerReady, NULL, NULL, NULL );

    if ( aButtonId == EAknSoftkeyCancel )
        {
        // cancel has been pressed
        delete this;
        }
    }

// ----------------------------------------------------
// CMceMoveProgress::ExecuteLD
// starts copy/move progress dialog
//
// ----------------------------------------------------
void CMceMoveProgress::ExecuteLD()
    {
    User::After(1000000); // Allow the message server to complete the pending operations
    iProgressDialog->RunDlgLD( CAknNoteDialog::ENoTone );
    }


// ----------------------------------------------------
// CMceMoveProgressDialog::DisableCancelL
// disables Cancel button in copy/move progress dialog
//
// ----------------------------------------------------
void CMceMoveProgressDialog::DisableCancelL()
    {
    HBufC *emptysoftkey = CCoeEnv::Static()->AllocReadResourceL( R_TEXT_SOFTKEY_EMPTY );
    CleanupStack::PushL( emptysoftkey );
    ButtonGroupContainer().SetCommandL( KMceCancelButton,EAknSoftkeyEmpty, *emptysoftkey );
    CleanupStack::PopAndDestroy( emptysoftkey );
    ButtonGroupContainer().DrawNow();
    }


//  End of File
