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
* Description:   CUniEditorSendOperation, asynchronous object for message sending
*
*/



// ========== INCLUDE FILES ================================
#include <MsgEditorView.h>
#include <MsgBodyControl.h>
#include <MsgExpandableControl.h>

#include <mmsconst.h>

// Features
#include <featmgr.h>    
#include <bldvariant.hrh>
#include <messagingvariant.hrh>

#include <unidatautils.h>
#include <MsgMimeTypes.h>
#include <unismilmodel.h>
#include <unismillist.h>
#include <uniobjectlist.h>
#include <MsgMediaInfo.h>
#include <MsgTextInfo.h>
#include <MsgImageInfo.h>
#include <uniaddresshandler.h>

#include "UniClientMtm.h"
#include "UniPluginApi.h"
#include "UniEditorHeader.h"
#include "UniEditorSendOperation.h"

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
// CUniEditorSendOperation::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CUniEditorSendOperation* CUniEditorSendOperation::NewL(
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        CUniEditorHeader& aHeader,
        CUniEditorPlugin& aPlugin,
        CMsgEditorView& aView,
        RFs& aFs )
    {
    CUniEditorSendOperation* self = 
        new ( ELeave ) CUniEditorSendOperation( aObserver, aDocument, aHeader, aPlugin, aView, aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CUniEditorSendOperation::CUniEditorSendOperation
//
// Constructor.
// ---------------------------------------------------------
//
CUniEditorSendOperation::CUniEditorSendOperation(
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        CUniEditorHeader& aHeader,
        CUniEditorPlugin& aPlugin,
        CMsgEditorView& aView,
        RFs& aFs ) :
    CUniEditorOperation( aObserver, aDocument, aFs, EUniEditorOperationSend ),
    iHeader( aHeader ),
    iPlugin( aPlugin ),
    iView( aView ),
    iConvertedEntry( 0 )
    {
    }

// ---------------------------------------------------------
// CUniEditorSendOperation::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniEditorSendOperation::ConstructL()
    {
    BaseConstructL();
    iSaveOperation = CUniEditorSaveOperation::NewL( *this, iDocument, iHeader, iView, iFs );
    }


// ---------------------------------------------------------
// CUniEditorSendOperation::~CUniEditorSendOperation
//
// Destructor.
// ---------------------------------------------------------
//
CUniEditorSendOperation::~CUniEditorSendOperation()
    {
    Cancel();
    delete iSaveOperation;
    }

// ---------------------------------------------------------
// CUniEditorSendOperation::Send
// ---------------------------------------------------------
//
void CUniEditorSendOperation::Send()
    {
    ResetErrors();
    iOperationState = EUniEditorSendRemoveDuplicateAddresses;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorSendOperation::DoSendStepL
// ---------------------------------------------------------
//
void CUniEditorSendOperation::DoSendStepL()
    {
    switch ( iOperationState )
        {
        case EUniEditorSendRemoveDuplicateAddresses:
            {
            DoRemoveDuplicateAddressesL();
            break;
            }
        case EUniEditorSendSave:
            {
            DoSave();
            break;
            }
        case EUniEditorSendConvert:
            {
            DoConvertL();
            break;
            }
        case EUniEditorSendPluginSend:
            {
            DoSendL();
            break;
            }
        case EUniEditorSendEnd:
            {
            iObserver.EditorOperationEvent(
                EUniEditorOperationSend,
                EUniEditorOperationComplete );
            break;
            }
        default:
            {
            iObserver.EditorOperationEvent(
                EUniEditorOperationSend,
                EUniEditorOperationError );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorSendOperation::DoRemoveDuplicateAddressesL
// ---------------------------------------------------------
//
void CUniEditorSendOperation::DoRemoveDuplicateAddressesL()
    {
    if ( iHeader.RemoveDuplicateAddressesL() )
        {
        iDocument.SetHeaderModified( ETrue );
        }
    
    iOperationState = EUniEditorSendSave;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorSendOperation::DoSave
// ---------------------------------------------------------
//
void CUniEditorSendOperation::DoSave()
    {
    iOperationState = EUniEditorSendConvert;
    iSaveOperation->Save( ESendingSave );
    SetPending();
    }

// ---------------------------------------------------------
// CUniEditorSendOperation::DoConvertL
// ---------------------------------------------------------
//
void CUniEditorSendOperation::DoConvertL()
    {
    iOperationState = EUniEditorSendPluginSend;
    iConvertedEntry = iPlugin.ConvertToL( iDocument.Mtm().Entry().EntryId() );
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorSendOperation::DoSendL
// ---------------------------------------------------------
//
void CUniEditorSendOperation::DoSendL()
    {
    iOperationState = EUniEditorSendEnd;
    iPlugin.SendL( iConvertedEntry );
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorSendOperation::DoCancelCleanup
//
// ---------------------------------------------------------
//
void CUniEditorSendOperation::DoCancelCleanup()
    {
    if ( iSaveOperation )
        {
        iSaveOperation->Cancel();
        }
    }

// ---------------------------------------------------------
// CUniEditorSendOperation::RunL
// ---------------------------------------------------------
//
void CUniEditorSendOperation::RunL()
    {
    PrintOperationAndState();
    if ( !SetErrorAndReport( iStatus.Int() ) )
        {
        DoSendStepL();
        }
    }

// ---------------------------------------------------------
// CUniEditorSendOperation::HandleOperationEvent
// ---------------------------------------------------------
//
void CUniEditorSendOperation::HandleOperationEvent(
    TUniEditorOperationType /*aOperation*/,
    TUniEditorOperationEvent /*aEvent*/ )
    {
    // Get errors from iSaveOperation
    CArrayFixFlat<TInt>* errors = iSaveOperation->GetErrors();
    for ( TInt i = 0; i < errors->Count(); i++ )
        {
        // if save operation has produced errors, the message
        // must not be sent because probably there is nothing to send!
        SetError( errors->At( i ) );
        iOperationState = EUniEditorSendEnd;
        }
        
    CompleteOperation( KErrNone );
    }

// EOF
