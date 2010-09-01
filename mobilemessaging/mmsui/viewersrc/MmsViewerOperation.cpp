/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       CMmsViewerOperation
*
*/



// ========== INCLUDE FILES ================================
#include <e32std.h>

#include "MmsMtmConst.h"	// For logging support

#include "MmsViewerOperation.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================
#ifdef CANCEL_TEST_CODE
// ---------------------------------------------------------
// Testing code of Cancel
// ---------------------------------------------------------
//
class CHighOperation : public CActive
    {
    public: 
        virtual ~CHighOperation()
            {
            Cancel();
            }

        CHighOperation( CMmsViewerOperation&    aObserver,
                        TInt                    aUseRandomCancel );
        void CompleteSelf();

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();
            
    public:
        CMmsViewerOperation&    iObserver;
        TInt                    iUseRandomCancel;
    };

CHighOperation::CHighOperation( CMmsViewerOperation&    aObserver,
                                TInt                    aUseRandomCancel ): 
    CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iUseRandomCancel( aUseRandomCancel )
    {
    CActiveScheduler::Add( this );
    if ( iUseRandomCancel > 100 )
        {
        SetPriority( EPriorityStandard + 1);
        }
    if ( iUseRandomCancel < 100 )
        {
        SetPriority( EPriorityStandard - 1);
        }
    }
void CHighOperation::CompleteSelf()
    {
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, KErrNone );
    }

void CHighOperation::RunL()
    {
    if ( iUseRandomCancel > 100 )
        {
        iObserver.Cancel();
        }
    else
        {
        /*// probability determines frequency
        TTime myTime;
        myTime.HomeTime();
        TTimeIntervalMicroSeconds micros = myTime.MicroSecondsFrom( 0 ); 
        TInt fraction = (TInt)micros.Int64()%99;
        if ( fraction < 0 )
            fraction = 0 - fraction;
        if ( fraction < iUseRandomCancel )
            {
            iObserver.Cancel();
            }*/
        iObserver.Cancel();
        }
        
    /*if ( iUseRandomCancel <= 100 )
        {
        CompleteSelf();
        }*/
    }
            
        /**
        * From CActive
        */
void CHighOperation::DoCancel()
    {
    }


#endif

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMmsViewerOperation::CMmsViewerOperation
//
// Constructor.
// ---------------------------------------------------------
//
CMmsViewerOperation::CMmsViewerOperation(
        MMmsViewerOperationObserver& aObserver,
        CMmsViewerDocument& aDocument,
        RFs& aFs ) :
    CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iDocument( aDocument ),
    iFs( aFs )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------
// CMmsViewerOperation::CMmsViewerOperation
//
// Destructor.
// ---------------------------------------------------------
//
CMmsViewerOperation::~CMmsViewerOperation()
    {
#ifdef CANCEL_TEST_CODE
    delete iHighOp;
#endif
    }

// ---------------------------------------------------------
// CompleteSelf
//
// Sets current operation active and completes it
// ---------------------------------------------------------
//
void CMmsViewerOperation::CompleteSelf( TInt aError )
    {
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, aError );
    }

// ---------------------------------------------------------
// SetActiveNoCompleteSelf
//
// Sets active but does not complete
// ---------------------------------------------------------
//
void CMmsViewerOperation::SetActiveNoCompleteSelf( )
    {
    iStatus = KRequestPending;
    SetActive();
    }

// ---------------------------------------------------------
// CompleteActiveSelf
//
// Completes already active operation
// ---------------------------------------------------------
//
void CMmsViewerOperation::CompleteActiveSelf( TInt aError )
    {
    LOGTEXT(_L8("MMSE: CMmsViewerOperation::CompleteActiveSelf"));
    // When CActive based operation is completed, User::RequestComplete sets 
    // iStatus to KRequestPending. Then User::RequestComplete calls RThread::RequestComplete(),
    // which copies aReason to iStatus. 
    // Operation stays active just until RunL() is called. When RunL() is running,
    // operation is no more active.
    // When RunL() is not yet called and operation is still active, we know that 
    // operation is NOT completed, if iStatus == KRequestPending. 
    // Note also that if operation has been completed by CompleteSelf(),
    // and Cancel is called(), Cancel() 'eats' the active object. RunL() is not more called.
    if (    IsActive() 
        &&  iStatus == KRequestPending )
        {
        TRequestStatus* pStatus = &iStatus;
        User::RequestComplete( pStatus, aError );
        }
    }

// ---------------------------------------------------------
// IsCompleted
// ---------------------------------------------------------
//
TBool CMmsViewerOperation::IsCompleted( )
    {
    return ( IsActive() &&  iStatus != KRequestPending );
    }
    
// ---------------------------------------------------------
// IsCompleted
// ---------------------------------------------------------
//
#ifdef CANCEL_TEST_CODE
void CMmsViewerOperation::LaunchCancelTestL( TInt aUseRandomCancel )
    {
    if ( !iHighOp )
        {
        iHighOp = new (ELeave) CHighOperation( *this, aUseRandomCancel );
        iHighOp->CompleteSelf();
        }
    }

void CMmsViewerOperation::CancelCancelTest( )
    {
    if ( iHighOp )
        iHighOp->Cancel();
    delete iHighOp;
    iHighOp = NULL;
    }

#endif    


// EOF
