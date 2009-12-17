/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CUniEditorOperation
*
*/



// ========== INCLUDE FILES ================================

#include <e32std.h>

#include "UniEditorLogging.h"    // For logging support

#include "UniEditorOperation.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================
const TInt KErrorGranularity = 3;

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniEditorOperation::CUniEditorOperation
//
// Constructor.
// ---------------------------------------------------------
//
CUniEditorOperation::CUniEditorOperation( MUniEditorOperationObserver& aObserver,
                                          CUniEditorDocument& aDocument,
                                          RFs& aFs,
                                          TUniEditorOperationType aOperationType ) :
    CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iDocument( aDocument ),
    iFs( aFs ),
    iOperationType( aOperationType )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------
// CUniEditorOperation::CUniEditorOperation
//
// Destructor.
// ---------------------------------------------------------
//
CUniEditorOperation::~CUniEditorOperation()
    {
    delete iErrors;
    }

// ---------------------------------------------------------
// CUniEditorOperation::BaseConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniEditorOperation::BaseConstructL()
    {
    iErrors = new ( ELeave ) CArrayFixFlat<TInt>( KErrorGranularity );
    }

// ---------------------------------------------------------
// CUniEditorOperation::CompleteSelf
//
// Completes current step of state machine
// ---------------------------------------------------------
//
void CUniEditorOperation::CompleteSelf( TInt aError )
    {
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, aError );
    }

// ---------------------------------------------------------
// CUniEditorOperation::SetPending
// ---------------------------------------------------------
//
void CUniEditorOperation::SetPending()
    {
    if ( !IsActive() )
        {
        iStatus = KRequestPending;
        SetActive();
        }
    }

// ---------------------------------------------------------
// CUniEditorOperation::CompleteOperation
// ---------------------------------------------------------
//
void CUniEditorOperation::CompleteOperation( TInt aError )
    {
    SetPending();
        
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, aError );
    }

// ---------------------------------------------------------
// CUniEditorOperation::RunError
//
// ---------------------------------------------------------
//
TInt CUniEditorOperation::RunError( TInt aError )
    {
#ifdef USE_LOGGER
    switch ( iOperationType )
        {
        case EUniEditorOperationChangeSlide:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::RunError EUniEditorOperationChangeSlide error: %d"), aError );
        break;
        case EUniEditorOperationInsert:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::RunError EUniEditorOperationInsert error: %d"), aError );
        break;
        case EUniEditorOperationLaunch:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::RunError EUniEditorOperationLaunch error: %d"), aError );
        break;
        case EUniEditorOperationPreview:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::RunError EUniEditorOperationPreview error: %d"), aError );
        break;
        case EUniEditorOperationSave:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::RunError EUniEditorOperationSave error: %d"), aError );
        break;
        case EUniEditorOperationProcessImage:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::RunError EUniEditorOperationProcessImage error: %d"), aError );
        break;
        case EUniEditorOperationSend:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::RunError EUniEditorOperationSend error: %d"), aError );
        break;
        case EUniEditorOperationSendUi:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::RunError EUniEditorOperationSendUi error: %d"), aError );
        break;
        case EUniEditorOperationSetSender:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::RunError EUniEditorOperationSetSender error: %d"), aError );
        break;
        case EUniEditorOperationVCard:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::RunError EUniEditorOperationVCard error: %d"), aError );
        break;
        case EUniEditorOperationConvertVideo:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::RunError EUniEditorOperationConvertVideo error: %d"), aError );
        break;
        default:        
        UNILOGGER_WRITEF(_L("CUniEditorOperation::RunError UnknownOperation error: %d"), aError );
        break;
        }
#endif

    if ( aError == KLeaveExit )
        {
        return KLeaveExit;
        }
    else
        {
        CompleteSelf( aError );
        return KErrNone;
        }
    }


// ---------------------------------------------------------
// CUniEditorOperation::GetErrors
// ---------------------------------------------------------
//
CArrayFixFlat<TInt>* CUniEditorOperation::GetErrors()
    {
    return iErrors;
    }

// ---------------------------------------------------------
// CUniEditorOperation::SetError
// ---------------------------------------------------------
//
void CUniEditorOperation::SetError( TInt aError )
    {
#ifdef USE_LOGGER
    switch ( iOperationType )
        {
        case EUniEditorOperationChangeSlide:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::SetError EUniEditorOperationChangeSlide error: %d"), aError );
        break;
        case EUniEditorOperationInsert:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::SetError EUniEditorOperationInsert error: %d"), aError );
        break;
        case EUniEditorOperationLaunch:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::SetError EUniEditorOperationLaunch error: %d"), aError );
        break;
        case EUniEditorOperationPreview:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::SetError EUniEditorOperationPreview error: %d"), aError );
        break;
        case EUniEditorOperationSave:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::SetError EUniEditorOperationSave error: %d"), aError );
        break;
        case EUniEditorOperationProcessImage:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::SetError EUniEditorOperationProcessImage error: %d"), aError );
        break;
        case EUniEditorOperationSend:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::SetError EUniEditorOperationSend error: %d"), aError );
        break;
        case EUniEditorOperationSendUi:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::SetError EUniEditorOperationSendUi error: %d"), aError );
        break;
        case EUniEditorOperationSetSender:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::SetError EUniEditorOperationSetSender error: %d"), aError );
        break;
        case EUniEditorOperationVCard:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::SetError EUniEditorOperationVCard error: %d"), aError );
        break;
        case EUniEditorOperationConvertVideo:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::SetError EUniEditorOperationConvertVideo error: %d"), aError );
        break;
        default:        
        UNILOGGER_WRITEF(_L("CUniEditorOperation::SetError UnknownOperation error: %d"), aError );
        break;
        }
#endif

    // Won't leave unless out of memory when allocating
    // new block of error codes.
    TRAPD( error, iErrors->AppendL( aError ) );
    if ( error != KErrNone )
        {
        iDocument.EikonEnv()->HandleError( error );
        } 
    }
    
// ---------------------------------------------------------
// CUniEditorOperation::ResetErrors
// ---------------------------------------------------------
//
void CUniEditorOperation::ResetErrors()
    {
    iErrors->Reset();
    }
    
// ---------------------------------------------------------
// CUniEditorOperation::SetErrorAndReport
// ---------------------------------------------------------
//
TInt CUniEditorOperation::SetErrorAndReport( TInt aError )
    {
    if ( aError != KErrNone )
        {
#ifdef USE_LOGGER
        switch ( iOperationType )
            {
            case EUniEditorOperationChangeSlide:
            UNILOGGER_WRITEF(_L("CUniEditorOperation::SetErrorAndReport EUniEditorOperationChangeSlide error: %d"), aError );
            break;
            case EUniEditorOperationInsert:
            UNILOGGER_WRITEF(_L("CUniEditorOperation::SetErrorAndReport EUniEditorOperationInsert error: %d"), aError );
            break;
            case EUniEditorOperationLaunch:
            UNILOGGER_WRITEF(_L("CUniEditorOperation::SetErrorAndReport EUniEditorOperationLaunch error: %d"), aError );
            break;
            case EUniEditorOperationPreview:
            UNILOGGER_WRITEF(_L("CUniEditorOperation::SetErrorAndReport EUniEditorOperationPreview error: %d"), aError );
            break;
            case EUniEditorOperationSave:
            UNILOGGER_WRITEF(_L("CUniEditorOperation::SetErrorAndReport EUniEditorOperationSave error: %d"), aError );
            break;
            case EUniEditorOperationProcessImage:
            UNILOGGER_WRITEF(_L("CUniEditorOperation::SetErrorAndReport EUniEditorOperationProcessImage error: %d"), aError );
            break;
            case EUniEditorOperationSend:
            UNILOGGER_WRITEF(_L("CUniEditorOperation::SetErrorAndReport EUniEditorOperationSend error: %d"), aError );
            break;
            case EUniEditorOperationSendUi:
            UNILOGGER_WRITEF(_L("CUniEditorOperation::SetErrorAndReport EUniEditorOperationSendUi error: %d"), aError );
            break;
            case EUniEditorOperationSetSender:
            UNILOGGER_WRITEF(_L("CUniEditorOperation::SetErrorAndReport EUniEditorOperationSetSender error: %d"), aError );
            break;
            case EUniEditorOperationVCard:
            UNILOGGER_WRITEF(_L("CUniEditorOperation::SetErrorAndReport EUniEditorOperationVCard error: %d"), aError );
            break;
            case EUniEditorOperationConvertVideo:
            UNILOGGER_WRITEF(_L("CUniEditorOperation::SetErrorAndReport EUniEditorOperationConvertVideo error: %d"), aError );
            break;
            default:        
            UNILOGGER_WRITEF(_L("CUniEditorOperation::SetErrorAndReport UnknownOperation error: %d"), aError );
            break;
            }
#endif

        SetError( aError );
        iObserver.EditorOperationEvent( iOperationType,
                                        EUniEditorOperationError );
        }

    return aError;
    }

// ---------------------------------------------------------
// CUniEditorOperation::PrintOperationAndState
// ---------------------------------------------------------
//
void CUniEditorOperation::PrintOperationAndState()
    {
#ifdef USE_LOGGER
    switch ( iOperationType )
        {
        case EUniEditorOperationChangeSlide:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::PrintOperationAndState EUniEditorOperationChangeSlide State: %d"), iOperationState );
        break;
        case EUniEditorOperationInsert:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::PrintOperationAndState EUniEditorOperationInsert State: %d"), iOperationState );
        break;
        case EUniEditorOperationLaunch:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::PrintOperationAndState EUniEditorOperationLaunch State: %d"), iOperationState );
        break;
        case EUniEditorOperationPreview:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::PrintOperationAndState EUniEditorOperationPreview State: %d"), iOperationState );
        break;
        case EUniEditorOperationSave:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::PrintOperationAndState EUniEditorOperationSave State: %d"), iOperationState );
        break;
        case EUniEditorOperationProcessImage:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::PrintOperationAndState EUniEditorOperationProcessImage State: %d"), iOperationState );
        break;
        case EUniEditorOperationSend:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::PrintOperationAndState EUniEditorOperationSend State: %d"), iOperationState );
        break;
        case EUniEditorOperationSendUi:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::PrintOperationAndState EUniEditorOperationSendUi State: %d"), iOperationState );
        break;
        case EUniEditorOperationSetSender:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::PrintOperationAndState EUniEditorOperationSetSender State: %d"), iOperationState );
        break;
        case EUniEditorOperationVCard:
        UNILOGGER_WRITEF(_L("CUniEditorOperation::PrintOperationAndState EUniEditorOperationVCard State: %d"), iOperationState );
        break;
        default:        
        UNILOGGER_WRITEF(_L("CUniEditorOperation::PrintOperationAndState UnknownOperation State: %d"), iOperationState );
        break;
        }
#endif
    }

// ---------------------------------------------------------
// CUniEditorOperation::DoCancel
//
// Calls derived class to cancel outstanding operations and perform
// cleanup. Notifies observer that operation has been cancelled. Reporting
// is needed as operation might be cancelled by other object than
// the observer (e.g. input blocker). Finally
// completes the operation if request is still pending. This should
// not happen but it is better to handle this situation to prevent
// deadlock.
// ---------------------------------------------------------
//
void CUniEditorOperation::DoCancel()
    {
    DoCancelCleanup();
    
    iObserver.EditorOperationEvent( iOperationType,
                                    EUniEditorOperationCancel );

    
    if ( iStatus == KRequestPending )
        {
        CompleteOperation( KErrCancel );
        }
    }

// ---------------------------------------------------------
// CUniEditorOperation::EditorOperationEvent
// ---------------------------------------------------------
//
void CUniEditorOperation::EditorOperationEvent( TUniEditorOperationType aOperation,
                                                TUniEditorOperationEvent aEvent )
    {
    if ( aEvent == EUniEditorOperationCancel )
        {
        iObserver.EditorOperationEvent( iOperationType, aEvent );
        CompleteOperation( KErrCancel );
        }
    else
        {
        HandleOperationEvent( aOperation, aEvent );
        }    
    }

// ---------------------------------------------------------
// CUniEditorOperation::EditorOperationQuery
// ---------------------------------------------------------
//
TBool CUniEditorOperation::EditorOperationQuery( TUniEditorOperationType aOperation,
                                                 TUniEditorOperationQuery aQuery )
    {
    return iObserver.EditorOperationQuery( aOperation, aQuery );
    }

// ---------------------------------------------------------
// CUniEditorOperation::HandleOperationEvent
// ---------------------------------------------------------
//
void CUniEditorOperation::HandleOperationEvent( TUniEditorOperationType /*aOperation*/,
                                                TUniEditorOperationEvent /*aEvent*/ )
    {   
    CompleteOperation( KErrNone );
    }

// EOF
