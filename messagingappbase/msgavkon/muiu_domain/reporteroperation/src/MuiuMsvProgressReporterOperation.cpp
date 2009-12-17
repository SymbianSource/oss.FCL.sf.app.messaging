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
*     Operation progress reporting class.
*
*/




// INCLUDE FILES
#include "MuiuMsvProgressReporterOperation.h"
#include "MuiuMsvWrappableProgressDialog.h"
#include <MTMStore.h>
#include "muiudomainpan.h"
#include "MuiuLogText.h"


// CONSTANTS
const TInt KPrgOpPriority = EActivePriorityWsEvents + 2;
_LIT(KMuiuDefaultProgressText, " ");




// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------
// CMsvProgressReporterOperation::NewL
// ---------------------------------------------------------
//
EXPORT_C CMsvProgressReporterOperation* CMsvProgressReporterOperation::NewL( 
    CMsvSession& aMsvSession, 
    TRequestStatus& aObserverRequestStatus, 
    TInt aImageIndex )
    {
    CMsvProgressReporterOperation* op = new(ELeave) CMsvProgressReporterOperation( 
                                        aMsvSession, aObserverRequestStatus, aImageIndex );
    CleanupStack::PushL( op );
    op->ConstructL( EFalse, EFalse );
    CleanupStack::Pop( op );
    return op;
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::NewL
// ---------------------------------------------------------
//
EXPORT_C CMsvProgressReporterOperation* CMsvProgressReporterOperation::NewL( 
                                        CMsvSession& aMsvSession, 
                                        TRequestStatus& aObserverRequestStatus)
    {
    CMsvProgressReporterOperation* op = CMsvProgressReporterOperation::NewL( aMsvSession, 
                                        aObserverRequestStatus, KErrNotFound );
    return op;
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::~CMsvProgressReporterOperation
// ---------------------------------------------------------
//
EXPORT_C CMsvProgressReporterOperation::~CMsvProgressReporterOperation()
    {   
    MUIULOGGER_ENTERFN( "CMsvProgressReporterOperation::~CMsvProgressReporterOperation" );
    // N.B. significant destruction order.
    Cancel();
    if ( iDialog )
        {
        TRAPD( err, iDialog->ProcessFinishedL() ); // this will delete iDialog
        if ( err < KErrNone )
            {
            delete iDialog;
            }
        }
    if( iOperation )
        {
        iOperation->Cancel();
        delete iOperation;
        MUIULOGGER_WRITE_FORMAT( "deleted iOperation: 0x%x", iOperation );
        }
    delete iMtmStore;
    delete iProgressString;

    MUIULOGGER_LEAVEFN( "CMsvProgressReporterOperation::~CMsvProgressReporterOperation" );
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::ProgressL
// ---------------------------------------------------------
//
EXPORT_C const TDesC8& CMsvProgressReporterOperation::ProgressL()
    {
    if( iOperation )
        {
        return iOperation->ProgressL();
        }
    return iDummyProgress;
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::MakeProgressVisibleL
// ---------------------------------------------------------
//
EXPORT_C void CMsvProgressReporterOperation::MakeProgressVisibleL( TBool aVisible )
    {
    if( aVisible )
        {
        if( !iDialog )
            {
            iDialog = CMsvWrappableProgressDialog::NewL(*iMtmStore, iImageIndex, 
                                                        EFalse, iDisplayWaitAnimation);
            if( iOperation )
                {
                iDialog->SetOperationL( iOperation );
                }  
            if ( iProgressString )                
                {
                iDialog->SetTitleTextL( *iProgressString );
                }
            else
                {
                iDialog->SetTitleTextL( KMuiuDefaultProgressText );
                }
            iDialog->SetProgressReporter( this );
            }
        }
    else
        {
        if( iDialog )
            {
            iDialog->ProcessFinishedL(); // this will delete iDialog
            iDialog = NULL;
            }
        }
    }

// ---------------------------------------------------------
// CMsvProgressReporterOperation::PublishNewDialogL
// ---------------------------------------------------------
//
EXPORT_C void CMsvProgressReporterOperation::PublishNewDialogL( TInt aDialogIndex, TUid aCategoryUid )
    {
    if( iDialog )
        {
        iDialog->ProcessFinishedL(); // this will delete iDialog
        iDialog = NULL;
        }

    iDialog = CMsvWrappableProgressDialog::NewL( *iMtmStore, 
                                                 aDialogIndex,
                                                 aCategoryUid,
                                                 iImageIndex, 
                                                 EFalse, 
                                                 iDisplayWaitAnimation );
    if( iOperation )
        {
        iDialog->SetOperationL( iOperation );
        }  
    if ( iProgressString )                
        {
        iDialog->SetTitleTextL( *iProgressString );
        }
    else
        {
        iDialog->SetTitleTextL( KMuiuDefaultProgressText );
        }
    iDialog->SetProgressReporter( this );
    }

// ---------------------------------------------------------
// CMsvProgressReporterOperation::SetTitleL
// ---------------------------------------------------------
//
EXPORT_C void CMsvProgressReporterOperation::SetTitleL( const TDesC& aTitle )
    {
    if( iDialog )
        {
        iDialog->SetTitleTextL( aTitle );
        delete iProgressString;
        iProgressString = NULL;
        iProgressString = aTitle.AllocL();
        }
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::SetTitleL
// ---------------------------------------------------------
//
EXPORT_C void CMsvProgressReporterOperation::SetTitleL( TInt aTitleResourceId )
    {
    if( iDialog )
        {
        delete iProgressString;
        iProgressString = NULL;
        iProgressString = iDialog->ControlEnv()->AllocReadResourceL( aTitleResourceId );
        iDialog->SetTitleTextL( *iProgressString );
        }
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::MtmStore
// ---------------------------------------------------------
//
EXPORT_C CMtmStore& CMsvProgressReporterOperation::MtmStore()
    {
    return *iMtmStore;
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::SetCanCancelL
// ---------------------------------------------------------
//
EXPORT_C void CMsvProgressReporterOperation::SetCanCancelL( TBool aCanCancel )
    {
    if( iDialog )
        {
        iDialog->SetCancel( aCanCancel );
        }
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::SetProgressDecoder
// ---------------------------------------------------------
//
EXPORT_C void CMsvProgressReporterOperation::SetProgressDecoder( MMsvProgressDecoder& aDecoder )
    {
    if( iDialog )
        {
        iDialog->SetProgressDecoder( aDecoder );
        }
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::UnsetProgressDecoder
// ---------------------------------------------------------
//
EXPORT_C void CMsvProgressReporterOperation::UnsetProgressDecoder()
    {
    if(iDialog)
        {
        iDialog->UnsetProgressDecoder();
        }
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::SetHelpContext
// ---------------------------------------------------------
//
EXPORT_C void CMsvProgressReporterOperation::SetHelpContext( const TCoeHelpContext& aContext )
    {
    if( iDialog )
        {
        iDialog->SetHelpContext( aContext );
        }
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::SetAnimationL
// ---------------------------------------------------------
//
EXPORT_C void CMsvProgressReporterOperation::SetAnimationL( TInt aResourceId )
    {
    if( iDialog )
        {
        iDialog->SetAnimationL( aResourceId );
        }
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::SetOperationL
// ---------------------------------------------------------
//
EXPORT_C void CMsvProgressReporterOperation::SetOperationL( CMsvOperation* aOp )
    {
    MUIULOGGER_ENTERFN( "CMsvProgressReporterOperation::SetOperationL" );
    __ASSERT_ALWAYS( aOp, Panic( EMuiuOpPrgNullOperation ) );
    __ASSERT_ALWAYS( !iOperation, Panic( EMuiuOpPrgOperationAlreadySet ) );
    iOperation = aOp;
    MUIULOGGER_WRITE_FORMAT( "Set iOperation: 0x%x", iOperation );
    iMtm = iOperation->Mtm();
    iService = iOperation->Service();
    if( iDialog )
        {
        iDialog->SetOperationL( aOp );
        iStatus = KRequestPending;
        SetActive();
        }
    MUIULOGGER_LEAVEFN( "CMsvProgressReporterOperation::SetOperationL" );
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::SetSeeding
// ---------------------------------------------------------
//
EXPORT_C void CMsvProgressReporterOperation::SetSeeding( TBool aSeed )
    {
    if( iDialog )
        {
        iDialog->SetSeeding( aSeed );
        }
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::RequestStatus
// ---------------------------------------------------------
//
EXPORT_C TRequestStatus& CMsvProgressReporterOperation::RequestStatus()
    {
    return iStatus;
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::SetProgressVisibilityDelay
// ---------------------------------------------------------
//
EXPORT_C void CMsvProgressReporterOperation::SetProgressVisibilityDelay( TBool aDelay )
    {
    if( iDialog )
        {
        iDialog->SetVisibilityDelay( aDelay );
        }
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::NewL
// ---------------------------------------------------------
//
EXPORT_C CMsvProgressReporterOperation* CMsvProgressReporterOperation::NewL( 
    TBool aProgressVisibilityDelayOff, 
    CMsvSession& aMsvSession, 
    TRequestStatus& aObserverRequestStatus, 
    TInt aImageIndex )
    {
    CMsvProgressReporterOperation* op = new( ELeave ) CMsvProgressReporterOperation( aMsvSession, 
                                                      aObserverRequestStatus, aImageIndex );
    CleanupStack::PushL( op );
    op->ConstructL( aProgressVisibilityDelayOff, EFalse );
    CleanupStack::Pop( op );
    return op;
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::NewL
// ---------------------------------------------------------
//
EXPORT_C CMsvProgressReporterOperation* CMsvProgressReporterOperation::NewL(
    TBool aProgressVisibilityDelayOff, 
    CMsvSession& aMsvSession, 
    TRequestStatus& aObserverRequestStatus)
    {
    CMsvProgressReporterOperation* op = CMsvProgressReporterOperation::NewL( 
         aProgressVisibilityDelayOff, aMsvSession, aObserverRequestStatus, KErrNotFound );
    return op;
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::NewL
// ---------------------------------------------------------
//
EXPORT_C CMsvProgressReporterOperation* CMsvProgressReporterOperation::NewL(
	TBool aProgressVisibilityDelayOff, 
	TBool aDisplayWaitAnimation,
	CMsvSession& aMsvSession, 
	TRequestStatus& aObserverRequestStatus, 
	TInt aImageIndex )
    {
    CMsvProgressReporterOperation* op = new(ELeave) CMsvProgressReporterOperation(
		aMsvSession, aObserverRequestStatus, aImageIndex);
    CleanupStack::PushL(op);
    op->ConstructL( aProgressVisibilityDelayOff, aDisplayWaitAnimation);
    CleanupStack::Pop( op );
    return op;
    }
    
// ---------------------------------------------------------
// CMsvProgressReporterOperation::NewL
// This method is used only when progress dialog is shown 
// also in cover side.
// ---------------------------------------------------------
//
EXPORT_C CMsvProgressReporterOperation* CMsvProgressReporterOperation::NewL( 
                                        CMsvSession& aMsvSession, 
                                        TRequestStatus& aObserverRequestStatus,
                                        TInt aDialogIndex,
                                        TUid aCategoryUid )
    {
    CMsvProgressReporterOperation* op = new(ELeave) CMsvProgressReporterOperation( 
                                        aMsvSession, aObserverRequestStatus, KErrNotFound );
    CleanupStack::PushL( op );
    op->ConstructL( aDialogIndex, aCategoryUid, EFalse, EFalse );
    CleanupStack::Pop( op );
    return op;
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::DoCancel
// ---------------------------------------------------------
//
void CMsvProgressReporterOperation::DoCancel()
    {
    if( iOperation )
        {
        iOperation->Cancel();
        }
    CompleteObserver( KErrCancel );
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::RunL
// ---------------------------------------------------------
//
void CMsvProgressReporterOperation::RunL()
    {
    if ( iCompleteWithExit )
        {
        CompleteObserver( EEikCmdExit );
        }
    else
        {
        CompleteObserver( iStatus.Int() );
        }
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::CompleteObserver
// ---------------------------------------------------------
//
void CMsvProgressReporterOperation::CompleteObserver( TInt aCode )
    {
    TRequestStatus* status = &iObserverRequestStatus;

    if ( ( *status ) == KRequestPending )
        {
        User::RequestComplete( status, aCode );
        }
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::CMsvProgressReporterOperation
// ---------------------------------------------------------
//
CMsvProgressReporterOperation::CMsvProgressReporterOperation(
    CMsvSession& aMsvSession, 
    TRequestStatus& aObserverRequestStatus, 
    TInt aImageIndex )
    : CMsvOperation( aMsvSession, KPrgOpPriority, aObserverRequestStatus ),
      iImageIndex( aImageIndex )
    {
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::ConstructL
// ---------------------------------------------------------
//
void CMsvProgressReporterOperation::ConstructL( TBool aProgressVisibilityDelayOff,
                                                TBool aDisplayWaitAnimation )
    {
    iMtmStore = CMtmStore::NewL( iMsvSession );

	iDisplayWaitAnimation = aDisplayWaitAnimation;
    iDialog = CMsvWrappableProgressDialog::NewL( *iMtmStore, iImageIndex, 
                                                 aProgressVisibilityDelayOff, 
                                                 aDisplayWaitAnimation );
    
    iDialog->SetTitleTextL( KMuiuDefaultProgressText );
    iDialog->SetProgressReporter( this );
    iObserverRequestStatus = KRequestPending;
    CActiveScheduler::Add(this);
    }
    
    
// ---------------------------------------------------------
// CMsvProgressReporterOperation::ConstructL
// This method is used only when progress dialog is shown 
// also in cover side.
// ---------------------------------------------------------
//
void CMsvProgressReporterOperation::ConstructL( TInt aDialogIndex,
                                                TUid aCategoryUid,
                                                TBool aProgressVisibilityDelayOff,
                                                TBool aDisplayWaitAnimation )
    {
    iMtmStore = CMtmStore::NewL( iMsvSession );

	  iDisplayWaitAnimation = aDisplayWaitAnimation;
    iDialog = CMsvWrappableProgressDialog::NewL( *iMtmStore, 
                                                 aDialogIndex,
                                                 aCategoryUid,
                                                 iImageIndex, 
                                                 aProgressVisibilityDelayOff, 
                                                 aDisplayWaitAnimation );
    
    iDialog->SetTitleTextL( KMuiuDefaultProgressText );
    iDialog->SetProgressReporter( this );
    iObserverRequestStatus = KRequestPending;
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------
// CMsvProgressReporterOperation::SetCompleteWithExit
// ---------------------------------------------------------
//
void CMsvProgressReporterOperation::SetCompleteWithExit( TBool aCompleteWithExit )
    {
    iCompleteWithExit = aCompleteWithExit;
    }


// End of file
