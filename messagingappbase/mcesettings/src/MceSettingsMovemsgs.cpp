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
*     Copies/Moves message store from source drive to target drive
*
*/



// INCLUDE FILES

#include <eikenv.h>
#include "MceSettingsMovemsgs.h"
#include "MceSettingsUtils.h"

#include <systemwarninglevels.hrh>

#include <msvapi.h>
#include <e32cmn.h>
#include <msvipc.h>

#include <StringLoader.h>
#include <MceSettings.rsg>
#include <aknnotewrappers.h>

// CONSTANTS

#ifdef _DEBUG
_LIT( KPanicText, "MceSettings.dll" );
enum TMcesettingsPanic
	{
	EMceSingleOpWatcherAlreadyActive = 1,
	EMceSingleOpWatcherOperationAlreadySet,
	EMceSingleOpWatcherOperationNull
	};
#endif


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CMceMessageTransHandler::Constructor
//
//
// ----------------------------------------------------
CMceMessageTransHandler::CMceMessageTransHandler(
    MMceMsgTransHandlerObserver& aOwner, CMsvSession* aSession, TInt aTargetDrive,
    TInt aSourceDrive, TBool aDeleteCopiedStore )
: CActive( KMsgTransHndlrPriority ), iOwner( aOwner ), iSession( aSession ),
        iTargetDrive(aTargetDrive ), iSourceDrive( aSourceDrive ),
        iDeleteCopiedStore( aDeleteCopiedStore ),
        iState( EStateCopying )
    {
    __DECLARE_NAME(_S("CMceMessageTransHandler") );
    CActiveScheduler::Add( this );
    }


// ----------------------------------------------------
// CMceMessageTransHandler::Constructor
//
//
// ----------------------------------------------------
CMceMessageTransHandler  *CMceMessageTransHandler::NewL(
    MMceMsgTransHandlerObserver& aOwner, CMsvSession* aSession,
    TInt aTargetDrive, TInt aSourceDrive, TBool aDeleteCopiedStore )
    {
    CMceMessageTransHandler* self=new ( ELeave ) CMceMessageTransHandler(
            aOwner,aSession,aTargetDrive,aSourceDrive,aDeleteCopiedStore);
    CleanupStack::PushL( self );
    self->ConstructL(); // should be consts in next release
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CMceMessageTransHandler::ConstructL
// Creates delete thread and transfer thread
//
// ----------------------------------------------------
void CMceMessageTransHandler::ConstructL()
    {
    /* Nothing */;
    }

// ----------------------------------------------------
// CMceMessageTransHandler::Destructor
//
//
// ----------------------------------------------------
CMceMessageTransHandler::~CMceMessageTransHandler()
    {
    Cancel();
    delete iOperation;
    iOperation = NULL;
    }


// ---------------------------------------------------------
// CMceMessageTransHandler::SetOperation
// --- Setter ---
// Must only be called once during the lifetime of a CMsvSingleOpWatcher object.
// ---------------------------------------------------------
void CMceMessageTransHandler::SetOperation( CMsvOperation* aOperation )
    {
    __ASSERT_DEBUG( !IsActive(), User::Panic( KPanicText, EMceSingleOpWatcherAlreadyActive ) );
    __ASSERT_DEBUG( !iOperation, User::Panic( KPanicText, EMceSingleOpWatcherOperationAlreadySet ) );
    __ASSERT_DEBUG( aOperation, User::Panic( KPanicText, EMceSingleOpWatcherOperationNull ) );

    // Delete the old operation
    delete iOperation;
    iOperation = NULL;

    // Take ownership of operation and set our active status so we're handled 
    // by the active scheduler.
    iOperation = aOperation;
    SetActive();
    }


// ---------------------------------------------------------
// CMsvSingleOpWatcher::Operation
// --- Accessor ---
// ---------------------------------------------------------
//
CMsvOperation& CMceMessageTransHandler::Operation() const
    {
    __ASSERT_DEBUG( iOperation, User::Panic( KPanicText, EMceSingleOpWatcherOperationNull ) );
    return *iOperation;
    }


// ---------------------------------------------------------
// CMceMessageTransHandler::DoCancel
// --- From CActive ---
// ---------------------------------------------------------
//
void CMceMessageTransHandler::DoCancel()        
    {
    // Pass on cancel 
    iOperation->Cancel();
    }


// ----------------------------------------------------
// CMceMessageTransHandler::RunL
// Calls TransferCompleteL and
// sets message store drive on the message server
// ----------------------------------------------------
void CMceMessageTransHandler::RunL()
    {
    TInt err = iStatus.Int();
    if (err == KErrNone)
    	{
    	err = ProgressErrorL();
    	}  
    
    if ( err != KErrNone )
        {
        // Copy operation did not succeed
        HBufC* text = StringLoader::LoadLC( R_MCE_SETTINGS_CANNOT_MOVE_STORE, 
                               CCoeEnv::Static() );
        CAknErrorNote* note = new ( ELeave ) CAknErrorNote();
        note->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text );
        }

    switch ( iState )
        {
        case EStateCopying:
            // Inform client of watcher that the operation has completed
            iOwner.CopyCompleteL( err );  
            if ( err == KErrNone && iDeleteCopiedStore )
                {
                // Only delete if there is no error copying
                delete iOperation;
                iOperation = NULL;                
                // Delete the old store
                iOperation = iSession->DeleteStoreL( iSourceDrive, iStatus );
                iState = EStateDeleting;
                SetActive();
                }
            else
                iOwner.TransferCompleteL( err );
            break;
        case EStateDeleting:
            // Check whether operation worked OK
            iOwner.TransferCompleteL( err );
            break;
        default:
            break;
        }    
    }


// ----------------------------------------------------
// CMceMessageTransHandler::Progress
// return % copied so far
//
// ----------------------------------------------------
TInt CMceMessageTransHandler::ProgressL() const
    {
    // Let's say everything up to the actual copy is the first 1%,
    // the copying is the next 98% and everything else the last 1%.
    const TInt KPreCopyPercentage = 1;
    const TInt KPostCopyPercentage = 1;
    const TInt KHundredPercentage = 100;
    const TInt KCopyPercentage = KHundredPercentage - KPreCopyPercentage - KPostCopyPercentage;

    if ( iState == EStateCopying )
        {
        // Get the progress
        TMsvCopyProgress copyProgress;
        TPckgC<TMsvCopyProgress> progressPackage( copyProgress );
        progressPackage.Set( iOperation->ProgressL() );
        copyProgress = progressPackage();	    
        if ( copyProgress.iError == KErrNone )
        	{
        	if ( copyProgress.iState == TMsvCopyProgress::ENotYetStarted )
        		{
        		return 0; // Nothing done yet
        		}
        	else if ( copyProgress.iState < TMsvCopyProgress::ECopyStore )
        		{
        		return KPreCopyPercentage; // Started, but not yet copying
        		}
        	else if ( copyProgress.iState == TMsvCopyProgress::ECopyStore )
        		{
        		// Copying, so return percentage of copy done + pre-copy percentage
        		return ( ( ( copyProgress.iCurrent + 1 ) * KCopyPercentage ) 
        		                                                / copyProgress.iTotal ) + KPreCopyPercentage;
        		}
        	else if ( copyProgress.iState == TMsvCopyProgress::ECompleted && !iDeleteCopiedStore )
        		{
        		// Finished copying and no delete to do - All done!
        		return KHundredPercentage;
        		}
        	else
        		{
        		// Copy is done but there's still more to do					
        		return KHundredPercentage - KPostCopyPercentage;
        		}
        	}
        else
        	{
        	return 0; // Error
        	}
        }
    else // EStateDeleting
    	{
    	TMsvDeleteProgress deleteProgress;
    	TPckgC<TMsvDeleteProgress> progressPackage( deleteProgress );
    	progressPackage.Set( iOperation->ProgressL() );
    	deleteProgress = progressPackage();
    	if ( deleteProgress.iError != KErrNone )
    		{
    		if ( deleteProgress.iState == TMsvDeleteProgress::ECompleted )
    			{
    			// Finsihed copying and deleting - All done!
    			return KHundredPercentage;
    			}
    		else
    			{
    			// Still deleting
    			return KHundredPercentage - KPostCopyPercentage;
    			}					
    		}
    	else
		{
		return 0; // Error
		}
    	}
    }

// ----------------------------------------------------
// TInt CMceMessageTransHandler::ProgressErrorL() const
//
// ----------------------------------------------------
TInt CMceMessageTransHandler::ProgressErrorL() const
    {
    if ( iState == EStateCopying )
        {
        TMsvCopyProgress copyProgress;
        TPckgC<TMsvCopyProgress> copyProgressPckg( copyProgress );
        copyProgressPckg.Set( iOperation->ProgressL() );
        return copyProgressPckg().iError;
        }
    else // EStateDeleting
    	{
    	TMsvDeleteProgress deleteProgress;
    	TPckgC<TMsvDeleteProgress> deleteProgressPckg( deleteProgress );
    	deleteProgressPckg.Set( iOperation->ProgressL() );
    	return deleteProgressPckg().iError;
    	}	
    }


// ----------------------------------------------------
// Called by the framework if RunL leaves
//
// ----------------------------------------------------
TInt CMceMessageTransHandler::RunError( TInt )
    {
    return KErrNone;
    }    

// End of file
