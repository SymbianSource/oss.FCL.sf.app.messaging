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
*           State machine -based operation for restoring message's attachments.
*
*/



// INCLUDE FILES
#include "cmsgmailrestoreattasop.h"
#include "cmsgmailrestoreattaop.h"
#include "MsgMailEditorDocument.h"
#include "MailLog.h"
#include <mmsvattachmentmanager.h>
#include <MsgAttachmentModel.h>
#include <caf/caferr.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttasOp::CMsgMailRestoreAttasOp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgMailRestoreAttasOp::CMsgMailRestoreAttasOp(
    CMsgMailEditorDocument& aDocument )
    : CMsgMailBaseOp( aDocument )
    {
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttasOp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreAttasOp::ConstructL()
    {    
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttasOp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgMailRestoreAttasOp* CMsgMailRestoreAttasOp::NewL(
    CMsgMailEditorDocument& aDocument )
    {
    CMsgMailRestoreAttasOp* self =
        new( ELeave ) CMsgMailRestoreAttasOp( aDocument );
    CleanupStack::PushL( self );
    self->ConstructL();
    
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CMsgMailRestoreAttasOp::~CMsgMailRestoreAttasOp()
    {
    LOG( "CMsgMailRestoreAttasOp::~CMsgMailRestoreAttasOp" );
    Cancel();
    // just to be sure that allocations are freed:    
    delete iRestoreOneAtta;
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttasOp::HandleStateActionL
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreAttasOp::HandleStateActionL()
    {
    switch ( iState )
        {
        case EInitialize:
            {
            InitializeL();
            break;
            }
        case ERestoreAtta:
            {
            RestoreAttaL();
            break;
            }        
        case EFinalize:
            {
            Finalize();
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
// CMsgMailRestoreAttasOp::SetNextState
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreAttasOp::SetNextState()
    {
    switch ( iState )
        {
        case EIdleState:
            {
            iState = EInitialize;
            break;
            }
        case EInitialize:
            {
            if ( iManager->AttachmentCount() > 0 )
                {
                iState = ERestoreAtta;
                }
            else
                {
                iState = EFinalize;
                }
            break;
            }
        case ERestoreAtta:
            {
            if ( iIndex < iManager->AttachmentCount() )
                { // proceed to next attachment
                iState = ERestoreAtta;
                }
            else
                { // all attas handled -> finalize operation
                iState = EFinalize;
                }
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
// CMsgMailRestoreAttasOp::HandleOperationCancel
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreAttasOp::HandleOperationCancel()
    {
    LOG( "CMsgMailRestoreAttasOp::HandleOperationCancel" );
    // deletion cancels possibly running asynchronous task
    delete iRestoreOneAtta;
    iRestoreOneAtta = NULL;
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttasOp::HandleStateActionError
// -----------------------------------------------------------------------------
//
TBool CMsgMailRestoreAttasOp::HandleStateActionError( TInt aError )
    {
    TBool retVal( EFalse );
    LOG( "CMsgMailRestoreAttasOp::HandleStateActionError" );
    if ( iState == ERestoreAtta )
        { // record the problem and return ETrue to indicate that operation
          // can be continued
        if ( aError == KErrCANoPermission )
            {
            iDRMAttaCount++;
            // atta was removed and rest of the attachments move in the list,
            // so we must adjust index properly
            iIndex--;
            }
        else
            {
            iInvalidAttaCount++;
            }
        retVal = ETrue;
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttasOp::InitializeL
// -----------------------------------------------------------------------------
//    
void CMsgMailRestoreAttasOp::InitializeL()
    {
    LOG( "CMsgMailRestoreAttasOp::InitializeL" );
	iManager = &( iDocument.GetAttachmentManagerL() );    
    iAllAttaCount = iManager->AttachmentCount();
    iDRMAttaCount = 0;
    iInvalidAttaCount = 0;
    iIndex = 0; // in the beginning we start with index 0
    // hide confirmation note when restoring
    iDocument.SetConfirmation( EFalse );    
    iDocument.AttachmentModel().Reset();    
    CompleteStateAction();
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttasOp::RestoreAttaL
// -----------------------------------------------------------------------------
//    
void CMsgMailRestoreAttasOp::RestoreAttaL()
    {
    LOG( "CMsgMailRestoreAttasOp::RestoreAttaL" );
    delete iRestoreOneAtta;
    iRestoreOneAtta = NULL;    
    iRestoreOneAtta = CMsgMailRestoreAttaOp::NewL( iIndex, iDocument );
    iRestoreOneAtta->StartOp( iStatus ); // will complete when atta restored
    SetActive();
    iIndex++; // increment index by one for each new atta    
    }
        
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttasOp::Finalize
// -----------------------------------------------------------------------------
//    
void CMsgMailRestoreAttasOp::Finalize()
    {
    delete iRestoreOneAtta;
    iRestoreOneAtta = NULL;        
    LOG( "CMsgMailRestoreAttasOp::Finalize" );
    iDocument.SetConfirmation( ETrue );
    // this was updated incorrectly to ETrue in Document's NotifyChanges
    iDocument.SetChanged( EFalse );
    CompleteStateAction();
    }    

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttasOp::CountAllAttas
// -----------------------------------------------------------------------------
//    
TInt CMsgMailRestoreAttasOp::CountAllAttas() const
    {
    return iAllAttaCount;
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttasOp::CountDRMAttas
// -----------------------------------------------------------------------------
//    
TInt CMsgMailRestoreAttasOp::CountDRMAttas() const
    {
    return iDRMAttaCount;
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttasOp::CountInvalidAttas
// -----------------------------------------------------------------------------
//    
TInt CMsgMailRestoreAttasOp::CountInvalidAttas() const
    {
    return iInvalidAttaCount;
    }
        
// End Of File
