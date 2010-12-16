/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       CMmsViewerChangeSlideOperation, Storage for single attachment in presentation. 
*
*/



// ========== INCLUDE FILES ================================

#include <txtrich.h>
#include <eikrted.h>

#include <MuiuMsvUiServiceUtilities.h>

#include <MsgEditorView.h>

#include <mmsclient.h>
#include <mmsconst.h>
#include "MmsMtmConst.h"            // File logger

#include <MmsViewer.rsg>

#include <unidatamodel.h>
#include <unidatautils.h>
#include <unismilmodel.h>
#include <unitextobject.h>

#include "MmsViewerHeader.h"
#include "MmsViewerChangeSlideOperation.h"

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
// CMmsViewerChangeSlideOperation::CMmsViewerChangeSlideOperation
//
// Constructor.
// ---------------------------------------------------------
//
CMmsViewerChangeSlideOperation::CMmsViewerChangeSlideOperation(
        MMmsViewerOperationObserver& aOperationObserver,
        CMmsViewerDocument& aDocument,
        CMmsViewerHeader& aHeader,
        CUniSlideLoader& aSlideLoader,
        CMsgEditorView& aView,
        RFs& aFs ) :
    CMmsViewerOperation( aOperationObserver, aDocument, aFs ),
    iHeader( aHeader ),
    iSlideLoader( aSlideLoader ),
    iView( aView )
    {
    }


// ---------------------------------------------------------
// CMmsViewerChangeSlideOperation::CMmsViewerChangeSlideOperation
//
// Destructor.
// ---------------------------------------------------------
//
CMmsViewerChangeSlideOperation::~CMmsViewerChangeSlideOperation()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CMmsViewerChangeSlideOperation::ChangeSlide
//
// ChangeSlideL
// ---------------------------------------------------------
//
void CMmsViewerChangeSlideOperation::ChangeSlide(
        TInt aNextNum )
    {
    iNextNum = aNextNum;
    iNextState = EMmsViewerChangeSlideResetView;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CMmsViewerChangeSlideOperation::ReLoadControlL
// ---------------------------------------------------------
//
void CMmsViewerChangeSlideOperation::ReLoadControlL( CMsgMediaControl* aControl, CUniObject* aObject )
    {
    // this is not async operation
    iSlideLoader.ReLoadControlL( aControl, aObject );
    iNextState = EMmsViewerChangeSlideEnd;
    CompleteSelf( KErrNone );
    }


// ---------------------------------------------------------
// CMmsViewerChangeSlideOperation::DoChangeSlideStepL
// ---------------------------------------------------------
//
void CMmsViewerChangeSlideOperation::DoChangeSlideStepL()
    {
    switch ( iNextState )
        {
        case EMmsViewerChangeSlideResetView:
            {
            // Have to do this first since we cannot
            // have to video clips open at the same time.
            LOGTEXT(_L8("CMmsViewerChangeSlideOperation::DoChangeSlideStepL DoResetViewL") );
            DoResetViewL();
            break;
            }
        case EMmsViewerChangeSlideLoad:
            {
            LOGTEXT(_L8("CMmsViewerChangeSlideOperation::DoChangeSlideStepL DoLoadL()") );
            DoLoadL();
            break;
            }
        case EMmsViewerChangeSlideFinalize:
            {
            LOGTEXT(_L8("CMmsViewerChangeSlideOperation::DoChangeSlideStepL DoLoadL()") );
            DoFinalizeL();
            break;
            }
        case EMmsViewerChangeSlideEnd:
            {
            LOGTEXT(_L8("CMmsViewerChangeSlideOperation::DoChangeSlideStepL EMmsViewerChangeSlideEnd") );
            iObserver.ViewerOperationEvent(
                EMmsViewerOperationChangeSlide,
                EMmsViewerOperationComplete,
                KErrNone );
            break;
            }
        default:
            {
            // Shouldn't be here!
            iObserver.ViewerOperationEvent(
                EMmsViewerOperationChangeSlide,
                EMmsViewerOperationError,
                KErrUnknown );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMmsViewerChangeSlideOperation::DoResetViewL
// ---------------------------------------------------------
void CMmsViewerChangeSlideOperation::DoResetViewL()
    {
    iSlideLoader.ResetViewL();
    iNextState = EMmsViewerChangeSlideLoad;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CMmsViewerChangeSlideOperation::DoLoadL
// ---------------------------------------------------------
void CMmsViewerChangeSlideOperation::DoLoadL()
    {
    iNextState = EMmsViewerChangeSlideFinalize;
    iSlideLoader.LoadSlideL( *this, iNextNum );
    SetActiveNoCompleteSelf( );
    }

// ---------------------------------------------------------
// CMmsViewerChangeSlideOperation::DoFinalizeL
// ---------------------------------------------------------
void CMmsViewerChangeSlideOperation::DoFinalizeL()
    {
    CUniObject* obj = iDocument.SmilModel()->GetObject( iDocument.CurrentSlide(), EUniRegionText );
    if ( obj && 
         ( obj->MediaType() == EMsgMediaText ||
           obj->MediaType() == EMsgMediaXhtml ) )
        {
        static_cast<CUniTextObject*>( obj )->SetText( NULL );
        }
        
    iView.SetCurrentPart( iNextNum );
    iDocument.SetCurrentSlide( iNextNum );

    if(!iView.IsSlideFlowEnable()) // don't remove control for slide flow
        {
        if ( iNextNum )
            {
            // without this change Japanese pictographs in subject field 
            // appear on slides 2,...
            iHeader.RemoveSubjectContentL();
            iHeader.RemoveFromViewL();
            }
        else
            {
            // restore mms subject, as it has been removed, if changing slide 2->1.
            iHeader.RestoreSubjectContentL();
            iHeader.AddToViewL();
            }
        }

    iNextState = EMmsViewerChangeSlideEnd;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CMmsViewerChangeSlideOperation::DoCancel
//
// ---------------------------------------------------------
//
void CMmsViewerChangeSlideOperation::DoCancel()
    {
    LOGTEXT(_L8("CMmsViewerChangeSlideOperation::DoCancel") );
    iSlideLoader.Cancel();
    iObserver.ViewerOperationEvent(
        EMmsViewerOperationChangeSlide,
        EMmsViewerOperationCancel,
        KErrNone );
    CompleteActiveSelf( KErrCancel );
    }

// ---------------------------------------------------------
// CMmsViewerChangeSlideOperation::RunL
//
// ---------------------------------------------------------
//
void CMmsViewerChangeSlideOperation::RunL()
    {
    if ( iStatus.Int() == KErrCancel )
        {
        iObserver.ViewerOperationEvent(
            EMmsViewerOperationChangeSlide,
            EMmsViewerOperationCancel,
            iStatus.Int() );
        return;
        }

    if ( iStatus.Int() != KErrNone )
        {
        LOGTEXT2(_L("CMmsViewerChangeSlideOperation::RunL iStatus=%d"), iStatus.Int() );
        iObserver.ViewerOperationEvent(
            EMmsViewerOperationChangeSlide,
            EMmsViewerOperationError,
            iStatus.Int() );
        }
    else
        {
        DoChangeSlideStepL();
        }
    }

// ---------------------------------------------------------
// CMmsViewerChangeSlideOperation::RunError
//
// ---------------------------------------------------------
//
TInt CMmsViewerChangeSlideOperation::RunError( TInt aError )
    {
    LOGTEXT2(_L("CMmsViewerChangeSlideOperation::RunError aError"), aError );
    if ( aError == KLeaveExit )
        {
        return KLeaveExit;
        }
    iObserver.ViewerOperationEvent(
        EMmsViewerOperationChangeSlide,
        EMmsViewerOperationError,
        aError );
    return KErrNone;
    }

// ---------------------------------------------------------
// UiSlideEvent
// ---------------------------------------------------------
void CMmsViewerChangeSlideOperation::SlideLoadReady( TInt aError )
    {
    LOGTEXT2(_L("CMmsViewerChangeSlideOperation::SlideLoadReady aError %d"), aError );
    CompleteActiveSelf( aError );
    }


// EOF
