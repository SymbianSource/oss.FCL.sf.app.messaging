/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  State machine -based operation for restoring attached messages.
*
*/



// INCLUDE FILES
#include "cmsgmailrestoreattmsgsop.h"
#include "MsgMailEditorDocument.h"
#include "MailLog.h"
#include <MsgEditorAppUi.rsg>       // resource identifiers
#include <MsgMailEditor.rsg>
#include <MsgAttachmentModel.h>

_LIT8(KMessageMimeType, "message/rfc822");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::CMsgMailRestoreAttMsgsOp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgMailRestoreAttMsgsOp::CMsgMailRestoreAttMsgsOp(
    CMsgMailEditorDocument& aDocument )
    : CMsgMailBaseOp( aDocument )
    {
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreAttMsgsOp::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgMailRestoreAttMsgsOp* CMsgMailRestoreAttMsgsOp::NewL(
    CMsgMailEditorDocument& aDocument )
    {
    CMsgMailRestoreAttMsgsOp* self =
        new( ELeave ) CMsgMailRestoreAttMsgsOp( aDocument );
    CleanupStack::PushL( self );
    self->ConstructL();
    
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CMsgMailRestoreAttMsgsOp::~CMsgMailRestoreAttMsgsOp()
    {
    LOG( "CMsgMailRestoreAttMsgsOp::~CMsgMailRestoreAttMsgsOp" );
    Cancel();
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::HandleStateActionL
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreAttMsgsOp::HandleStateActionL()
    {
    switch ( iState )
        {
        case EGetMsgDigestEntries:
            {
            GetMsgDigestEntriesL();
            break;
            }
        case EPrepareAddAttMsgs:
            {
            PrepareAddAttMsgsL();
            break;
            }
        case EAddAttMsgs:
            {
            AddAttMsgsL();
            break;
            }                               
        default:
            {
            // should never come here
            ASSERT( EFalse );
            break;
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::SetNextState
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreAttMsgsOp::SetNextState()
    {
    switch ( iState )
        {
        case EIdleState:
            {
            iState = EGetMsgDigestEntries;
            break;
            }
        case EGetMsgDigestEntries:
            {
            iState = EPrepareAddAttMsgs;
            break;
            }
        case EPrepareAddAttMsgs: // fall through
        case EAddAttMsgs:
            {
            if ( iIndex < iEntries->Count() )
                {
                iState = EAddAttMsgs;
                }
            else
                {
                iState = EIdleState;
                }
            break;
            }            
        default:
            {            
            iState = EIdleState;
            break;
            }
        }        
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::HandleOperationCancel
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreAttMsgsOp::HandleOperationCancel()
    {
    LOG( "CMsgMailRestoreAttMsgsOp::HandleOperationCancel" );
    if ( iState == EGetMsgDigestEntries )
        {
        LOG( "HandleOperationCancel, canceling message operation" );
        TRAP_IGNORE( iDocument.MessageL().Cancel() );
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::HandleStateActionError
// -----------------------------------------------------------------------------
//
TBool CMsgMailRestoreAttMsgsOp::HandleStateActionError( TInt /*aError*/ )
    {
    TBool retVal( EFalse );
    LOG( "CMsgMailRestoreAttMsgsOp::HandleStateActionError" );
    if ( iState == EAddAttMsgs )
        { // record the problem and return ETrue to indicate that operation
          // can be continued
        iInvalidAttaMsgCount++;
        retVal = ETrue;
        }
    return retVal;
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::GetMsgDigestEntriesL
// -----------------------------------------------------------------------------
//    
void CMsgMailRestoreAttMsgsOp::GetMsgDigestEntriesL()
    {    
    iDocument.MessageL().GetMessageDigestEntriesL( iStatus,
                                                   iDocument.Entry().Id() );
    SetActive();
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::PrepareAddAttMsgsL
// -----------------------------------------------------------------------------
//    
void CMsgMailRestoreAttMsgsOp::PrepareAddAttMsgsL()
    {    
    iEntries = &( iDocument.MessageL().Selection() );
    // initialize counters and index
    iAllAttaMsgCount = iEntries->Count();
    iInvalidAttaMsgCount = 0;    
    iIndex = 0;
    CompleteStateAction();
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::AddAttMsgsL
// -----------------------------------------------------------------------------
//    
void CMsgMailRestoreAttMsgsOp::AddAttMsgsL()
    {    
    DoAddAttMsgL( iIndex );
    iIndex++; // increase index for next iteration
    CompleteStateAction();
    }    
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::DoAddAttMsgL
// -----------------------------------------------------------------------------
//    
void CMsgMailRestoreAttMsgsOp::DoAddAttMsgL( TInt aIndex ) const
    {
    ASSERT( iEntries && aIndex < iEntries->Count() );
    const TDataType messageMimeType( KMessageMimeType );
    const TMsvId messId( iEntries->At( aIndex ) );
    CMsvEntry* parententry= iDocument.Session().GetEntryL( messId );
    CleanupStack::PushL( parententry );
    const TMsvEntry entry( parententry->Entry() );
    iDocument.AttachmentModel().AddAttachmentL( entry.iDescription,
                                                entry.iSize,
                                                messId,
                                                entry.Complete(),
                                                messageMimeType );
    CleanupStack::PopAndDestroy(parententry); // parententry
    }        

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::CountAllAttaMsgs
// -----------------------------------------------------------------------------
//    
TInt CMsgMailRestoreAttMsgsOp::CountAllAttaMsgs() const
    {
    return iAllAttaMsgCount;
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttMsgsOp::CountInvalidAttaMsgs
// -----------------------------------------------------------------------------
//    
TInt CMsgMailRestoreAttMsgsOp::CountInvalidAttaMsgs() const
    {
    return iInvalidAttaMsgCount;
    }
        
// End Of File
