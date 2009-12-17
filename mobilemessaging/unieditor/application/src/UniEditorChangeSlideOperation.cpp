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
* Description:  UniEditorChangeSlideOperation implementation
*
*/



// ========== INCLUDE FILES ================================
#include <MuiuMsvUiServiceUtilities.h>

#include <MsgEditorView.h>

#include "UniClientMtm.h"

#include <UniEditor.rsg>

#include <unidatautils.h>
#include <unismilmodel.h>
#include <unislideloader.h>
#include <unitextobject.h>

#include "UniEditorHeader.h"
#include "UniEditorChangeSlideOperation.h"
#include "UniEditorEnum.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CUniEditorChangeSlideOperation* CUniEditorChangeSlideOperation::NewL( 
        MUniEditorOperationObserver& aOperationObserver,
        CUniEditorDocument& aDocument,
        CUniEditorHeader& aHeader,
        CUniSlideLoader& aSlideLoader,
        CMsgEditorView& aView,
        RFs& aFs )
    {
    CUniEditorChangeSlideOperation* self = 
            new ( ELeave ) CUniEditorChangeSlideOperation( aOperationObserver, 
                                                           aDocument, 
                                                           aHeader, 
                                                           aSlideLoader, 
                                                           aView, 
                                                           aFs );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::CUniEditorChangeSlideOperation
//
// Constructor.
// ---------------------------------------------------------
//
CUniEditorChangeSlideOperation::CUniEditorChangeSlideOperation(
        MUniEditorOperationObserver& aOperationObserver,
        CUniEditorDocument& aDocument,
        CUniEditorHeader& aHeader,
        CUniSlideLoader& aSlideLoader,
        CMsgEditorView& aView,
        RFs& aFs ) :
    CUniEditorOperation( aOperationObserver, aDocument, aFs, EUniEditorOperationChangeSlide ),
    iHeader( aHeader ),
    iSlideLoader( aSlideLoader ),
    iView( aView )
    {
    }

// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniEditorChangeSlideOperation::ConstructL()
    {
    BaseConstructL();
    iSaveOperation = CUniEditorSaveOperation::NewL( *this, iDocument, iHeader, iView, iFs );
    }


// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::~CUniEditorChangeSlideOperation
// ---------------------------------------------------------
//
CUniEditorChangeSlideOperation::~CUniEditorChangeSlideOperation()
    {
    Cancel();
    delete iSaveOperation;
    }

// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::ChangeSlide
// ---------------------------------------------------------
//
void CUniEditorChangeSlideOperation::ChangeSlide( TInt aNextNum )
    {
    ResetErrors();
    
    iNextNum = aNextNum;
    iOperationState = EUniEditorChangeSlideSave;
    
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::DoChangeSlideStepL
// ---------------------------------------------------------
//
void CUniEditorChangeSlideOperation::DoChangeSlideStepL()
    {
    switch ( iOperationState )
        {
        case EUniEditorChangeSlideSave:
            {
            DoSaveL();
            break;
            }
        case EUniEditorChangeSlideLoad:
            {
            DoLoadL();
            break;
            }
        case EUniEditorChangeSlideFinalize:
            {
            DoFinalizeL();
            break;
            }
        case EUniEditorChangeSlideEnd:
            {
            iObserver.EditorOperationEvent( EUniEditorOperationChangeSlide,
                                            EUniEditorOperationComplete );
            break;
            }
        default:
            {
            SetErrorAndReport( KErrUnknown );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::DoLoadL
// ---------------------------------------------------------
//
void CUniEditorChangeSlideOperation::DoLoadL()
    {
    iSlideLoader.ResetViewL();
    
    iSlideLoader.LoadSlideL( *this, iNextNum );    
    SetPending();
    }

// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::DoSaveL
// ---------------------------------------------------------
//
void CUniEditorChangeSlideOperation::DoSaveL()
    {
    iSaveOperation->Save( ERegularSave );
    SetPending();
    }

// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::DoFinalizeL
// ---------------------------------------------------------
//
void CUniEditorChangeSlideOperation::DoFinalizeL()
    {
    // Moved to iSlideLoader.ResetViewL(), because sometimes removing
    // causes new calculation of message size, which requires existence of
    // text control, but it was already removed in ResetViewL()
    iView.SetScrollParts( iDocument.DataModel()->SmilModel().SlideCount() );
    iView.SetCurrentPart( iNextNum );
    iDocument.SetCurrentSlide( iNextNum );

    if ( iNextNum )
        {
        iHeader.RemoveFromViewL();
        }
    else
        {
        iHeader.AddToViewL();
        }

    iOperationState = EUniEditorChangeSlideEnd;
    CompleteSelf( KErrNone );
    }
    
// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::DoCancelCleanup
// ---------------------------------------------------------
//
void CUniEditorChangeSlideOperation::DoCancelCleanup()
    {
    if ( iSaveOperation )
        {
        iSaveOperation->Cancel();
        }
        
    iSlideLoader.Cancel();
    }

// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::RunL
// ---------------------------------------------------------
//
void CUniEditorChangeSlideOperation::RunL()
    {
    PrintOperationAndState();
    if ( !SetErrorAndReport( iStatus.Int() ) )
        {
        DoChangeSlideStepL();
        }
    }

// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::HandleOperationEvent
// ---------------------------------------------------------
//
void CUniEditorChangeSlideOperation::HandleOperationEvent( TUniEditorOperationType /*aOperation*/,
                                                           TUniEditorOperationEvent /*aEvent*/ )
    {
    CMDXMLDocument* dom = iSaveOperation->DetachDom();
    if ( dom )
        {
        iDocument.DataModel()->SetDom( dom );
        }
    
    iOperationState = EUniEditorChangeSlideLoad;  
    CompleteOperation( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorChangeSlideOperation::SlideLoadReady
// ---------------------------------------------------------
//
void CUniEditorChangeSlideOperation::SlideLoadReady( TInt aError )
    {
    iOperationState = EUniEditorChangeSlideFinalize;
    CompleteOperation( aError );
    }


// EOF
