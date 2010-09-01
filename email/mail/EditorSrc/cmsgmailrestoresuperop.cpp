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
* Description:  
* 		State machine -based operation for restoring attachments and attached 
* 		messages.
*
*/



// INCLUDE FILES
#include "cmsgmailrestoresuperop.h"
#include "MsgMailEditorDocument.h"
#include "cmsgmailrestoreattmsgsop.h"
#include "cmsgmailrestoreattasop.h"
#include "MailUtils.h"
#include "MailLog.h"
#include <MsgEditorAppUi.rsg>       // resource identifiers
#include <MsgMailEditor.rsg>
#include <featmgr.h>                    // FeatureManager

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgMailRestoreSuperOp::CMsgMailRestoreSuperOp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgMailRestoreSuperOp::CMsgMailRestoreSuperOp(
    CMsgMailEditorDocument& aDocument )
    : CMsgMailBaseOp( aDocument )
    {
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreSuperOp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreSuperOp::ConstructL()
    {
    
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreSuperOp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgMailRestoreSuperOp* CMsgMailRestoreSuperOp::NewL(
    CMsgMailEditorDocument& aDocument )
    {
    CMsgMailRestoreSuperOp* self =
        new( ELeave ) CMsgMailRestoreSuperOp( aDocument );
    CleanupStack::PushL( self );
    self->ConstructL();
    
    CleanupStack::Pop(self);
    return self;
    }


// Destructor
CMsgMailRestoreSuperOp::~CMsgMailRestoreSuperOp()
    {
    LOG( "CMsgMailRestoreSuperOp::~CMsgMailRestoreSuperOp" );
    Cancel();
    delete iRestoreAttMsgsOp;
    delete iRestoreAttasOp; 
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreSuperOp::HandleStateActionL
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreSuperOp::HandleStateActionL()
    {
    switch ( iState )
        {
        case ERestoreAttachments:
            {            
            RestoreAttachmentsL();
            break;
            }
        case ERestoreAttachedMsgs:
            {            
            RestoreAttachedMsgsL();
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
// CMsgMailRestoreSuperOp::SetNextState
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreSuperOp::SetNextState()
    {
    switch ( iState )
        {
        case EIdleState:
            {
            iState = ERestoreAttachments;
            break;
            }        
        case ERestoreAttachments:
            {
            iState = ERestoreAttachedMsgs;
            break;
            }        
        case ERestoreAttachedMsgs:
        default:
            {            
            iState = EIdleState;
            break;
            }
        }        
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreSuperOp::HandleOperationCancel
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreSuperOp::HandleOperationCancel()
    {
    LOG( "CMsgMailRestoreSuperOp::HandleOperationCancel, deleting iRestoreAttasOp" );    
    delete iRestoreAttasOp;
    iRestoreAttasOp = NULL;
    LOG( "CHandleOperationCancel, ...deleting iRestoreAttMsgsOp..." );
    delete iRestoreAttMsgsOp;
    iRestoreAttMsgsOp = NULL;
    LOG( "HandleOperationCancel, ...finished" );        
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreSuperOp::RestoreAttachmentsL
// -----------------------------------------------------------------------------
//    
void CMsgMailRestoreSuperOp::RestoreAttachmentsL()
    {
    delete iRestoreAttasOp;
    iRestoreAttasOp = NULL;
    iRestoreAttasOp = CMsgMailRestoreAttasOp::NewL( iDocument );    
    iRestoreAttasOp->StartOp( iStatus );
    SetActive();
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreSuperOp::RestoreAttachedMsgsL
// -----------------------------------------------------------------------------
//    
void CMsgMailRestoreSuperOp::RestoreAttachedMsgsL()
    {
    delete iRestoreAttMsgsOp;
    iRestoreAttMsgsOp = NULL;
    iRestoreAttMsgsOp = CMsgMailRestoreAttMsgsOp::NewL( iDocument );        
    iRestoreAttMsgsOp->StartOp( iStatus );
    SetActive();
    }
    
// ----------------------------------------------------------------------------
//  CMsgMailRestoreSuperOp::ShowNoteIfInvalidAttaL()
// ----------------------------------------------------------------------------
//
TBool CMsgMailRestoreSuperOp::ShowNoteIfInvalidAttaL() const
    {
    TBool retVal( EFalse );
    TInt attachmentCount = iRestoreAttasOp->CountAllAttas();
    attachmentCount += iRestoreAttMsgsOp->CountAllAttaMsgs();
    TInt invalidAttachmentCount = iRestoreAttasOp->CountInvalidAttas();
    invalidAttachmentCount += iRestoreAttMsgsOp->CountInvalidAttaMsgs();
    
    TInt resourceId( NULL );   
    switch ( invalidAttachmentCount )
        {
        case 0:
            // Do nothing
            break;
        case 1:
            resourceId = attachmentCount != invalidAttachmentCount ?
                         R_MAIL_ONE_INVALID_ATTACHMENT :
                         R_MAIL_INVALID_ATTACHMENT;
            break;
        default:
            resourceId = attachmentCount != invalidAttachmentCount ?
                         R_MAIL_FEW_INVALID_ATTACHMENTS :
                         R_MAIL_ALL_INVALID_ATTACHMENTS;
            break;
        }
    // Corrupted info note
    if ( resourceId )
        {
        MailUtils::InformationNoteL( resourceId );
        retVal = ETrue;
        }
    return retVal;
    }
    

// ----------------------------------------------------------------------------
// CMsgMailRestoreSuperOp::ShowNoteIfDRMAttaL
// ----------------------------------------------------------------------------
//
TBool CMsgMailRestoreSuperOp::ShowNoteIfDRMAttaL() const
    {
    TBool retVal( EFalse );
    if ( FeatureManager::FeatureSupported( KFeatureIdDrmFull ) )
        {
        TInt drmAttaCount = iRestoreAttasOp->CountDRMAttas();
        if ( drmAttaCount > 0 )
            {
            MailUtils::InformationNoteL( drmAttaCount == 1 ?
                                         R_QTN_MAIL_SEND_FORBID_1 :
                                         R_QTN_MAIL_SEND_FORBID );
            retVal = ETrue;
            }
        }
    return retVal;
    }    
    
// End Of File
