/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides message sending methods. 
*
*/



#include <mmsclient.h>
#include <MsgEditorAppUi.h>   
#include <MsgAddressControl.h>  
#include <uniaddresshandler.h>

#include <audiomessage.rsg>
#include "audiomessagesendoperation.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// CAudioMessageSendOperation::NewL
// ---------------------------------------------------------
//
CAudioMessageSendOperation* CAudioMessageSendOperation::NewL(
		CMsgAddressControl& aAddressCtrl,
        CUniAddressHandler& aAddressHandler,
        MAmsOperationObserver& aObserver,
        CAudioMessageDocument& aDocument,
        CMsgEditorView& aView )
    {
    CAudioMessageSendOperation* self = new ( ELeave ) CAudioMessageSendOperation(
        aAddressCtrl,
        aAddressHandler,
        aObserver,
        aDocument, 
        aView );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CAudioMessageSendOperation::Constructor
// ---------------------------------------------------------
//
CAudioMessageSendOperation::CAudioMessageSendOperation(
		CMsgAddressControl& aAddressCtrl,
        CUniAddressHandler& aAddressHandler,
        MAmsOperationObserver& aObserver,
        CAudioMessageDocument& aDocument,
        CMsgEditorView& aView ) :
    CAudioMessageOperation( aObserver, aDocument ),
    iAddressCtrl( aAddressCtrl ),
    iAddressHandler( aAddressHandler ),
    iView( aView )
    {
    }


// ---------------------------------------------------------
// CAudioMessageSendOperation::Destructor
// ---------------------------------------------------------
//
CAudioMessageSendOperation::~CAudioMessageSendOperation()
    {
    Cancel();
    delete iSaveOperation;
    delete iMtmSendOperation;
    }

// ---------------------------------------------------------
// CAudioMessageSendOperation::Send
// ---------------------------------------------------------
//
void CAudioMessageSendOperation::Send()
    {
    ResetError();
    iSendState = EAmsSendRemoveDuplicateAddresses;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CAudioMessageSendOperation::DoSendStepL
// ---------------------------------------------------------
//
void CAudioMessageSendOperation::DoSendStepL()
    {
    switch ( iSendState )
        {
        case EAmsSendRemoveDuplicateAddresses:
            DoRemoveDuplicateAddressesL();
            break;
        case EAmsSendSave:
            DoSave();
            break;
        case EAmsSendMtmSend:
            DoSendL();
            break;
        case EAmsSendEnd:
            iObserver.EditorOperationEvent(
                EAmsOperationSend,
                EAmsOperationComplete );
            break;
        default:
            iObserver.EditorOperationEvent(
                EAmsOperationSend,
                EAmsOperationError );
            break;
        }
    }

// ---------------------------------------------------------
// CAudioMessageSendOperation::DoRemoveDuplicateAddressesL
// ---------------------------------------------------------
//
void CAudioMessageSendOperation::DoRemoveDuplicateAddressesL()
    {
    CArrayPtrFlat<CMsgAddressControl>* addressControls 
        = new ( ELeave ) CArrayPtrFlat<CMsgAddressControl>( 1 );
    CleanupStack::PushL( addressControls );
    addressControls->AppendL( &iAddressCtrl );
    if ( iAddressHandler.RemoveDuplicateAddressesL( *addressControls ) )
        {
        iDocument.SetHeaderModified( ETrue );
        }
    CleanupStack::PopAndDestroy( addressControls );
    iSendState = EAmsSendSave;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CAudioMessageSendOperation::DoSave
// ---------------------------------------------------------
//
void CAudioMessageSendOperation::DoSave()
    {
    iSendState = EAmsSendMtmSend;
    iSaveOperation->Save();
    }

// ---------------------------------------------------------
// CAudioMessageSendOperation::DoSendL
// ---------------------------------------------------------
//
void CAudioMessageSendOperation::DoSendL()
    {
    iSendState = EAmsSendEnd;
    iStatus = KRequestPending;
    iMtmSendOperation = iDocument.Mtm().SendL( iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CAudioMessageSendOperation::ConstructL
// ---------------------------------------------------------
//
void CAudioMessageSendOperation::ConstructL()
    {
    iSaveOperation = CAudioMessageSaveOperation::NewL( iAddressCtrl, *this, iDocument, 
    	iView );
    }

// ---------------------------------------------------------
// CAudioMessageSendOperation::DoCancel
// ---------------------------------------------------------
//
void CAudioMessageSendOperation::DoCancel()
    {
    if ( iSaveOperation )
        {
        iSaveOperation->Cancel();
        }
    if ( iMtmSendOperation )
        {
        iMtmSendOperation->Cancel();
        }
    }

// ---------------------------------------------------------
// CAudioMessageSendOperation::RunL
// ---------------------------------------------------------
//
void CAudioMessageSendOperation::RunL()
    {
    if ( iStatus.Int() != KErrNone )
        {
        SetError( iStatus.Int() );
        iObserver.EditorOperationEvent(
            EAmsOperationSend,
            EAmsOperationError );
        }
    else
        {
        DoSendStepL();
        }
    }

// ---------------------------------------------------------
// CAudioMessageSendOperation::EditorOperationEvent
// ---------------------------------------------------------
//
void CAudioMessageSendOperation::EditorOperationEvent(
    TAmsOperationType /*aOperation*/,
    TAmsOperationEvent /*aEvent*/ )
    {
    CompleteSelf( KErrNone );
    }

