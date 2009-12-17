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
* Description:  State machine -based operation for mail editor view launching.
*
*/



// INCLUDE FILES
#include "cmsgmaillaunchop.h"
#include "mmsgmailappuiopdelegate.h"
#include "MsgMailEditorDocument.h"
#include "cmsgmailrestoresuperop.h"
#include "MailLog.h"
#include <MsgEditorAppUi.rsg>       // resource identifiers
#include <MsgMailEditor.rsg>
#include <MsgBodyControl.h>
#include <eikrted.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgMailLaunchOp::CMsgMailLaunchOp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgMailLaunchOp::CMsgMailLaunchOp(
    CMsgMailEditorDocument& aDocument,
    MMsgMailAppUiOpDelegate& aOpDelegate )
    : CMsgMailBaseOp( aDocument ),
      iOpDelegate( aOpDelegate )
    {
    }

// -----------------------------------------------------------------------------
// CMsgMailLaunchOp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgMailLaunchOp::ConstructL()
    {
    
    }

// -----------------------------------------------------------------------------
// CMsgMailLaunchOp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgMailLaunchOp* CMsgMailLaunchOp::NewL(
    CMsgMailEditorDocument& aDocument,
    MMsgMailAppUiOpDelegate& aOpDelegate )
    {
    CMsgMailLaunchOp* self = new( ELeave ) CMsgMailLaunchOp( aDocument,
                                                             aOpDelegate );
    CleanupStack::PushL( self );
    self->ConstructL();
    
    CleanupStack::Pop(self);
    return self;
    }


// Destructor
CMsgMailLaunchOp::~CMsgMailLaunchOp()
    {
    LOG( "CMsgMailLaunchOp::~CMsgMailLaunchOp" );
    Cancel();
    delete iRestoreSuperOp;
    }

// -----------------------------------------------------------------------------
// CMsgMailLaunchOp::HandleStateActionL
// -----------------------------------------------------------------------------
//
void CMsgMailLaunchOp::HandleStateActionL()
    {
    switch ( iState )
        {
        case EInitLaunch:
            {
            InitLaunchL();
            break;
            }
        case ERestoreBody:
            {
            RestoreBodyL();
            break;
            }
        case EConvertLineBreaks:
            {            
            ConvertLineBreaksL();
            break;
            }            
        case ERestoreAllAttas:
            {            
            RestoreAllAttasL();
            break;
            }
        case EExecuteView:
            {            
            ExecuteViewL();
            break;
            }            
        case EFinalize:
            {
            FinalizeL();
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
// CMsgMailLaunchOp::SetNextState
// -----------------------------------------------------------------------------
//
void CMsgMailLaunchOp::SetNextState()
    {
    switch ( iState )
        {
        case EIdleState:
            {
            iState = EInitLaunch;
            break;
            }        
        case EInitLaunch:
            {
            iState = ERestoreBody;
            break;
            }
        case ERestoreBody:
            {
            iState = EConvertLineBreaks;
            break;
            }        
        case EConvertLineBreaks:
            {
            iState = ERestoreAllAttas;
            break;
            }            
        case ERestoreAllAttas:
            {
            iState = EExecuteView;
            break;
            }        
        case EExecuteView:
            {
            iState = EFinalize;
            break;
            }                                                                    
        case EFinalize: // fall through
        default:
            {            
            iState = EIdleState;
            break;
            }
        }        
    }

// -----------------------------------------------------------------------------
// CMsgMailLaunchOp::HandleOperationCancel
// -----------------------------------------------------------------------------
//
void CMsgMailLaunchOp::HandleOperationCancel()
    {
    LOG( "CMsgMailLaunchOp::HandleOperationCancel, deleting iRestoreSuperOp" );    
    delete iRestoreSuperOp;
    iRestoreSuperOp = NULL;
    
    if ( iState == ERestoreBody )
        {
        LOG( "HandleOperationCancel, ...MessageL().Cancel()..." );
        TRAP_IGNORE( iDocument.MessageL().Cancel() );
        }
    LOG( "HandleOperationCancel, ...finished" );        
    }
    
// -----------------------------------------------------------------------------
// CMsgMailLaunchOp::InitLaunchL
// -----------------------------------------------------------------------------
//    
void CMsgMailLaunchOp::InitLaunchL()
    {
    // start wait note showing at this point
    StartWaitNoteL( R_WAIT_OPENING, EFalse, R_MEB_WAIT_NOTE );
    
    CMsgBodyControl& bodyControl( iOpDelegate.DelegateInitLaunchL() );
    
    TInt editorFlags = bodyControl.Editor().AknEdwinFlags();
    editorFlags &= ~EAknEditorFlagEnablePictographInput;
    bodyControl.Editor().SetAknEditorFlags( editorFlags );
    iBodyText = &( bodyControl.TextContent() );
    
    CompleteStateAction();    
    }
        
// -----------------------------------------------------------------------------
// CMsgMailLaunchOp::RestoreBodyL
// -----------------------------------------------------------------------------
//    
void CMsgMailLaunchOp::RestoreBodyL()
    {
    iDocument.MessageL().GetBodyTextL(
        iStatus,
        iDocument.Entry().Id(),
        CImEmailMessage::EThisMessageOnly,
        *iBodyText,
        const_cast<CParaFormatLayer&> (*(iBodyText->GlobalParaFormatLayer())),
        const_cast<CCharFormatLayer&> (*(iBodyText->GlobalCharFormatLayer())));
    SetActive();
    }
    
// -----------------------------------------------------------------------------
// CMsgMailLaunchOp::ConvertLineBreaksL
// -----------------------------------------------------------------------------
//    
void CMsgMailLaunchOp::ConvertLineBreaksL()
    {
    ASSERT( iBodyText ); // must be available in this state already
    ConvertLineBreaksL( *iBodyText );
    CompleteStateAction();        
    }

// -----------------------------------------------------------------------------
// CMsgMailLaunchOp::RestoreAllAttasL
// -----------------------------------------------------------------------------
//    
void CMsgMailLaunchOp::RestoreAllAttasL()
    {
    delete iRestoreSuperOp;
    iRestoreSuperOp = NULL;
    iRestoreSuperOp = CMsgMailRestoreSuperOp::NewL( iDocument );    
    iRestoreSuperOp->StartOp( iStatus );
    SetActive();
    }
    
// -----------------------------------------------------------------------------
// CMsgMailLaunchOp::ExecuteViewL
// -----------------------------------------------------------------------------
//    
void CMsgMailLaunchOp::ExecuteViewL()
    {
    iOpDelegate.DelegateExecuteViewL();
    StopWaitNote();    
    CompleteStateAction();  
    }
    
// -----------------------------------------------------------------------------
// CMsgMailLaunchOp::FinalizeL
// -----------------------------------------------------------------------------
//    
void CMsgMailLaunchOp::FinalizeL()
    {
    iOpDelegate.DelegateFinalizeLaunchL();
    iRestoreSuperOp->ShowNoteIfInvalidAttaL();
    iRestoreSuperOp->ShowNoteIfDRMAttaL();    
    CompleteStateAction();  
    }
    
// ----------------------------------------------------------------------------
//  CMsgMailLaunchOp::ConvertLineBreaksL()
// ----------------------------------------------------------------------------
//
void CMsgMailLaunchOp::ConvertLineBreaksL( CRichText& aRtf ) const
    {
    HBufC* buf = HBufC::NewLC(aRtf.DocumentLength());
    TPtr bufPtr = buf->Des();
    aRtf.Extract(bufPtr);
    TInt count(0);
    for (TInt i(0); i < bufPtr.Length(); i++)
        {
        if (bufPtr[i] == CEditableText::ELineBreak)
            {
            count++;
            // Replace every 10th linebreak with paragraph to
            // make body text scrolling smoother.
            if (count == 10)										// CSI: 47 # See a comment above.
                {
                bufPtr[i] = CEditableText::EParagraphDelimiter;
                count = 0;
                }
            }
        // Replace 'CRLF' or 'LF' with LineBreak.
        else if (bufPtr[i] == 0x0A) // Linefeed character (LF)
            {
            bufPtr[i] = CEditableText::ELineBreak;

            // Check if previous char was CR
            if ( i>0 && bufPtr[i-1] == 0x0D) // ascii (CR)
                {
                bufPtr.Delete( i-1, 1 ); // Delete CR
                }
            }
        }
    aRtf.DeleteL(0, aRtf.DocumentLength());
    aRtf.InsertL(0, *buf);
    CleanupStack::PopAndDestroy( buf ); // buf
    }    
    
// End Of File
