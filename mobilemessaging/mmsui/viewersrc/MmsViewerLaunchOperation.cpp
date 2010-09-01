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
*       CMmsViewerLaunchOperation, Storage for single attachment in presentation. 
*
*/



// ========== INCLUDE FILES ================================

#include <AknsConstants.h>
#include <data_caging_path_literals.hrh> 
#include <MsgEditorView.h>          // for CMsgEditorView
#include <MsgBodyControl.h>
#include <MuiuMsgEditorLauncher.h>

#include <mmsconst.h>
#include <mmsclient.h>
#include <MmsViewer.rsg>

#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknUtils.h>
#include <MsgEditorCommon.h>
#include <MsgMediaInfo.h>
#include <msgimagecontrol.h>
#include <unislideloader.h>
#include "UniMmsViewerDocument.h"

#include <mmsui.mbg>
#include "MmsMtmConst.h"            // File logger

#include "MmsMtmUi.h"
#include "MmsViewerHeader.h"
#include "MmsViewerLaunchOperation.h"

_LIT( KMmsUiMbmFile, "mmsui.mbm" );

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
// CMmsViewerLaunchOperation::CMmsViewerLaunchOperation
//
// Constructor.
// ---------------------------------------------------------
//
CMmsViewerLaunchOperation::CMmsViewerLaunchOperation(
        MUniObjectObserver*         aUniObjectObserver,
        MMsgAsyncControlObserver&   aControlObserver,
        MMmsViewerOperationObserver& aOperationObserver,
        CMmsViewerDocument& aDocument,
        CMsgEditorView& aView,
        RFs& aFs ) :
    CMmsViewerOperation( aOperationObserver, aDocument, aFs ),
    iControlObserver( aControlObserver ),
    iView( aView ),
    iUniObjectObserver( aUniObjectObserver )
    {
    }


// ---------------------------------------------------------
// CMmsViewerLaunchOperation::CMmsViewerLaunchOperation
//
// Destructor.
// ---------------------------------------------------------
//
CMmsViewerLaunchOperation::~CMmsViewerLaunchOperation()
    {
#ifdef CANCEL_TEST_CODE
    CancelCancelTest( );
#endif
    Cancel();
    delete iSlideLoader;
    delete iHeader;
    }

// ---------------------------------------------------------
// CMmsViewerLaunchOperation::Launch
//
// ---------------------------------------------------------
//
void CMmsViewerLaunchOperation::Launch()
    {
    iNextState = EMmsLaunchInitialize;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CMmsViewerLaunchOperation::Launch
//
// SaveL
// ---------------------------------------------------------
//
void CMmsViewerLaunchOperation::DoLaunchStepL()
    {
    switch ( iNextState )
        {
        case EMmsLaunchInitialize:
            {
            LOGTEXT(_L8("CMmsViewerLaunchOperation::DoLaunchStepL EMmsLaunchInitialize") );
            DoInitializeL();
            break;
            }
        case EMmsLaunchPrepareHeader:
            {
            LOGTEXT(_L8("CMmsViewerLaunchOperation::DoLaunchStepL EMmsLaunchPrepareHeader") );
            DoPrepareHeaderL();
            break;
            }
        case EMmsLaunchPrepareBody:
            {
            LOGTEXT(_L8("CMmsViewerLaunchOperation::DoLaunchStepL EMmsLaunchPrepareBody") );
            DoPrepareBodyL();
            break;
            }
        case EMmsLaunchEnd:
            {
            LOGTEXT(_L8("CMmsViewerLaunchOperation::DoLaunchStepL EMmsLaunchEnd") );
            iObserver.ViewerOperationEvent(
                EMmsViewerOperationLaunch,
                EMmsViewerOperationComplete,
                KErrNone ); 
            break;
            }
        default:
            iObserver.ViewerOperationEvent(
                EMmsViewerOperationLaunch,
                EMmsViewerOperationError,
                KErrUnknown ); 
            break;
        }

    iNextState++;
    }

// ---------------------------------------------------------
// DoInitializeL
// ---------------------------------------------------------
void CMmsViewerLaunchOperation::DoInitializeL()
    {
    // For messages without SMIL iDocument.InitializeL() completes immediately even before
    // call of SetActiveNoCompleteSelf
    iDocument.InitializeL( *this );
    if ( !iIsInitialised )
        {
        SetActiveNoCompleteSelf();
        }
    }


// ---------------------------------------------------------
// DoPrepareHeaderL
// ---------------------------------------------------------
void CMmsViewerLaunchOperation::DoPrepareHeaderL()
    {    
    // Header is always drawn and populated
    iHeader = CMmsViewerHeader::NewL(
        iDocument.Mtm(),
        iDocument.DataModel().AttachmentList(),
        iView,
        iFs );
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// DoPrepareBodyL
// ---------------------------------------------------------
void CMmsViewerLaunchOperation::DoPrepareBodyL()
    {
    // Determine whether body objects should be drawn
    // NOTE: Viewable if conformant even if forwarded.
    if ( iDocument.ErrorResponseMessage() )
        {
        DoPrepareErrorResponseMessageL();
        CompleteSelf( KErrNone );
        }
    else if ( iDocument.SmilType() == EMmsSmil )
        {
        if ( !iDocument.SmilModel()->SlideCount() )
            {
            iDocument.SmilModel()->AddSlideL();
            }

        iSlideLoader = CUniSlideLoader::NewL(
            iUniObjectObserver,
            iControlObserver,
            iDocument.DataModel(),
            iView,
            EUniControlViewerMode );

        iSlideLoader->LoadSlideL( *this, 0 );
        SetActiveNoCompleteSelf( );
        }
    else
        {
        DoPrepare3GPPBodyL();
        CompleteSelf( KErrNone );
        }
    }

// ---------------------------------------------------------
// DoPrepareErrorResponseMessageL
// ---------------------------------------------------------
void CMmsViewerLaunchOperation::DoPrepareErrorResponseMessageL()
    {
    CMsgBodyControl* textControl = CMsgBodyControl::NewL( &iView );
    CleanupStack::PushL( textControl );
    textControl->SetControlId( EMsgComponentIdBody );

    textControl->InsertTextL(
        static_cast<CMmsClientMtm&>( iDocument.Mtm() ).ResponseText() );

    iView.AddControlL( textControl, EMsgComponentIdBody, EMsgFirstControl, EMsgBody );
    CleanupStack::Pop( textControl );
    }

// ---------------------------------------------------------
// DoPrepare3GPPBodyL
// ---------------------------------------------------------
void CMmsViewerLaunchOperation::DoPrepare3GPPBodyL()
    {
    CMsgImageControl* imageControl = CMsgImageControl::NewL( 
        iView, 
        &iControlObserver );
    CleanupStack::PushL( imageControl );

    TParse fileParse;

    fileParse.Set( KMmsUiMbmFile, &KDC_APP_BITMAP_DIR, NULL );
    imageControl->LoadIconL(
        fileParse.FullName( ),
        KAknsIIDQgnGrafMmsPlay,
        EMbmMmsuiQgn_graf_mms_play,
        EMbmMmsuiQgn_graf_mms_play_mask );

    TAknWindowLineLayout layOut( AknLayoutScalable_Apps::msg_data_pane_g7().LayoutLine() );
    TAknLayoutRect iconLayout;
    iconLayout.LayoutRect( MsgEditorCommons::MsgDataPane(),
                           layOut );

    imageControl->SetIconSizeL( iconLayout.Rect().Size() );
    imageControl->SetIconVisible( ETrue );
    
    imageControl->SetControlId( EMsgComponentIdImage );
    //The ownership of imageControl is transferred to iView
    iView.AddControlL( imageControl, EMsgComponentIdImage, EMsgFirstControl, EMsgBody );
    CleanupStack::Pop( imageControl );
    }

// ---------------------------------------------------------
// CMmsViewerLaunchOperation::DoCancel
//
// ---------------------------------------------------------
//
void CMmsViewerLaunchOperation::DoCancel()
    {
    LOGTEXT(_L8("CMmsViewerLaunchOperation::DoCancel") );
    if ( iSlideLoader )
        {
        iSlideLoader->Cancel();
        }
    iObserver.ViewerOperationEvent(
        EMmsViewerOperationLaunch,
        EMmsViewerOperationCancel,
        KErrNone );
    CompleteActiveSelf( KErrCancel );
    }

// ---------------------------------------------------------
// CMmsViewerLaunchOperation::RunL
//
// ---------------------------------------------------------
//
void CMmsViewerLaunchOperation::RunL()
    {
    LOGTEXT2( _L("CMmsViewerLaunchOperation::RunL iStatus=%d"), iStatus.Int() );
    if ( iStatus.Int() == KErrCancel )
        {
        iObserver.ViewerOperationEvent(
            EMmsViewerOperationLaunch,
            EMmsViewerOperationCancel,
            iStatus.Int() );
        return;
        }
    else if ( iStatus.Int() != KErrNone )
        {
        User::Leave( iStatus.Int() );
        }

    DoLaunchStepL();
    }

// ---------------------------------------------------------
// CMmsViewerLaunchOperation::RunError
//
// ---------------------------------------------------------
//
TInt CMmsViewerLaunchOperation::RunError( TInt aError )
    {
    LOGTEXT2(_L("CMmsViewerLaunchOperation::RunError aError %d"), aError );
    if ( aError == KLeaveExit )
        {
        return KLeaveExit;
        }
    iObserver.ViewerOperationEvent(
        EMmsViewerOperationLaunch,
        EMmsViewerOperationError,
        aError );
    return KErrNone;
    }

// ---------------------------------------------------------
// InitializeReady
// ---------------------------------------------------------
//
void CMmsViewerLaunchOperation::InitializeReady( TInt aParseResult, TInt aError )
    {
    LOGTEXT2(_L("CMmsViewerLaunchOperation::InitializeReady aError %d"), aError );
    iIsInitialised = ETrue;
    iParseResult = aParseResult;
    if ( !IsActive() )
        {
        SetActiveNoCompleteSelf();
        }
    CompleteActiveSelf( aError );
    }

// ---------------------------------------------------------
// UiSlideEvent
// ---------------------------------------------------------
//
void CMmsViewerLaunchOperation::SlideLoadReady( TInt aError )
    {
    LOGTEXT2(_L("CMmsViewerLaunchOperation::SlideLoadReady aError %d"), aError );
    CompleteActiveSelf( aError );
    }

// EOF
