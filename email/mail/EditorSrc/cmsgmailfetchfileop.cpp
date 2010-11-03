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
* Description: State machine -based operation for fetching files as attachments.
*
*/



// INCLUDE FILES
#include "cmsgmailfetchfileop.h"
#include "MsgMailEditorDocument.h"
#include "MailLog.h"
#include "Msgattachmentverifier.h"
#include "cmsgmailrestoresuperop.h"
#include "MailUtils.h"
#include <MsgEditorAppUi.rsg>
#include <MsgMailEditor.rsg>
#include <coemain.h>
#include <mmsvattachmentmanager.h>

// LOCAL CONSTANT

// appoximated 3 attachments / message
const TInt KFetchFileGranularity = 3;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgMailFetchFileOp::CMsgMailFetchFileOp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgMailFetchFileOp::CMsgMailFetchFileOp(
    MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType,
    CMsgMailEditorDocument& aDocument )
    : CMsgMailBaseOp( aDocument ),
      iFetchType( aFetchType )
    {
    //We set a higher priority to fetch op so that touch events won´t get
    //leaked to editors toolbar
    SetPriority( EActivePriorityWsEvents + 1 );
    }

// -----------------------------------------------------------------------------
// CMsgMailFetchFileOp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgMailFetchFileOp::ConstructL()
    {
    LOG( "CMsgMailFetchFileOp::ConstructL" );
    iFetchArray = new( ELeave ) CDesCArrayFlat( KFetchFileGranularity );
    iManager = &( iDocument.GetAttachmentManagerL() );
    }

// -----------------------------------------------------------------------------
// CMsgMailFetchFileOp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgMailFetchFileOp* CMsgMailFetchFileOp::NewL(
    MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType,
    CMsgMailEditorDocument& aDocument )
    {
    CMsgMailFetchFileOp* self =
        new( ELeave ) CMsgMailFetchFileOp( aFetchType, aDocument );
    CleanupStack::PushL( self );
    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CMsgMailFetchFileOp::~CMsgMailFetchFileOp()
    {
    LOG( "CMsgMailFetchFileOp::~CMsgMailFetchFileOp" );
    if( iBlocker )
        {
		// iBlocker is deleted in AknInputBlockCancel
        iBlocker->Cancel();
        }
    Cancel();
    delete iFetchArray;
    delete iRestoreSuperOp;
    }

// -----------------------------------------------------------------------------
// CMsgMailFetchFileOp::HandleStateActionL
// -----------------------------------------------------------------------------
//
void CMsgMailFetchFileOp::HandleStateActionL()
    {
    switch ( iState )
        {
        case ESelectFiles:
            {
            SelectFilesL();
            break;
            }
        case EAddAttas:
            {
            AddAttasL();
            break;
            }
        case ERestoreAllAttas:
            {
            RestoreAllAttasL();
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
// CMsgMailFetchFileOp::SetNextState
// -----------------------------------------------------------------------------
//
void CMsgMailFetchFileOp::SetNextState()
    {
    switch ( iState )
        {
        case EIdleState:
            {
            iState = ESelectFiles;
            break;
            }
        case ESelectFiles:
            {
            iState = EAddAttas;
            break;
            }
        case EAddAttas:
            {
            iState = ERestoreAllAttas;
            break;
            }
        case ERestoreAllAttas:
            {
            iState = EFinalize;
            break;
            }
        case EFinalize:
        default:
            {
            iState = EIdleState;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailFetchFileOp::HandleOperationCancel
// -----------------------------------------------------------------------------
//
void CMsgMailFetchFileOp::HandleOperationCancel()
    {
    LOG( "CMsgMailFetchFileOp::HandleOperationCancel, deleting iRestoreSuperOp" );
    if( iBlocker )
        {
        iBlocker->Cancel();
        }
    delete iRestoreSuperOp;
    iRestoreSuperOp = NULL;
    if ( iState == EAddAttas )
        {
        // cancel possibly ongoing AddAttachmentL() request, doesn't do
        // anything if request isn't outstanding
        LOG( "HandleOperationCancel, cancel manager's request" );
        iManager->CancelRequest();
        }
    LOG( "HandleOperationCancel, ...finished" );
    }


// -----------------------------------------------------------------------------
// CMsgMailFetchFileOp::AknInputBlockCancel
// -----------------------------------------------------------------------------
//
void CMsgMailFetchFileOp::AknInputBlockCancel()
    {
    delete iBlocker;
    iBlocker = NULL;
    }

// -----------------------------------------------------------------------------
// CMsgMailFetchFileOp::SelectFilesL
// -----------------------------------------------------------------------------
//
void CMsgMailFetchFileOp::SelectFilesL()
    {
    TInt fetchResult( EFalse );

    // starting phase -> initialize operation data
    iFetchCount = 0;
    iFileName = KNullDesC;
    iFetchArray->Reset();

    CMsgAttachmentVerifier* verifier =
        CMsgAttachmentVerifier::NewLC( iDocument.Session() );

    if ( iFetchType == MsgAttachmentUtils::EUnknown )
    	{
    	if ( MsgAttachmentUtils::FetchAnyFileL( iFileName,
    	                                        *( CCoeEnv::Static() ),
    	                                        verifier ) )
    	    {
    	    fetchResult = ETrue;
    	    iFetchCount = 1;
    	    }
    	}
    else
    	{
    	fetchResult = MsgAttachmentUtils::FetchFileL( iFetchType,
                                        iFileName,
    	                                *iFetchArray,
    	                                ETrue,
    	                                EFalse,
    	                                verifier );
    	iFetchCount = iFetchArray->Count();
       	}
	// Check fetch results. EFalse if user cancels the fetch.
    if ( fetchResult == EFalse )
        {
        User::Leave( KErrCancel );
        }
    CleanupStack::PopAndDestroy( verifier ); // verifier
    CompleteStateAction();
    }

// -----------------------------------------------------------------------------
// CMsgMailFetchFileOp::AddAttasL
// In current implementation multiple attachments are added as a one step, and
// failure with one attachment interrupts entire procedure.
// -----------------------------------------------------------------------------
//
void CMsgMailFetchFileOp::AddAttasL()
    {
    StartWaitNoteL( R_WAIT_INSERTING_ATTACHMENT, EFalse, R_MEB_WAIT_NOTE );
    ASSERT( iFetchCount > 0 ); // op was cancelled if nothing was selected
    
	if( iBlocker )
        {
        iBlocker->Cancel();
        }
    // NewL version does not exist
    iBlocker = CAknInputBlock::NewCancelHandlerLC( this ); 
    CleanupStack::Pop( iBlocker );

    if ( iFetchCount > 1 )
    	{
    	AddMultipleAttachmentsL( *iFetchArray, iDocument, iStatus );
    	}
    else
        {
        AddSingleAttachmentL( iFileName, iDocument, iStatus );
        }
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMsgMailFetchFileOp::RestoreAllAttasL
// -----------------------------------------------------------------------------
//
void CMsgMailFetchFileOp::RestoreAllAttasL()
    {
    delete iRestoreSuperOp;
    iRestoreSuperOp = NULL;
    iRestoreSuperOp = CMsgMailRestoreSuperOp::NewL( iDocument );
    iRestoreSuperOp->StartOp( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMsgMailFetchFileOp::FinalizeL
// -----------------------------------------------------------------------------
//
void CMsgMailFetchFileOp::FinalizeL()
    {
    StopWaitNote();
    if( iBlocker )
        {
        iBlocker->Cancel();
        }
    
    TBool invalidAttas = iRestoreSuperOp->ShowNoteIfInvalidAttaL();
    TBool drmAttas = iRestoreSuperOp->ShowNoteIfDRMAttaL();
    if ( !invalidAttas && !drmAttas )
        { // Restore operation reported no problems so we determine that the
          // attachment was really added. Note that we actually restored ALL
          // the attachments and some of them could have theoretically reported
          // a problem, but in practise this should correctly indicate the
          // success of attachment adding operation.
        MailUtils::ConfirmationNoteL( R_MAIL_EDITOR_ADDED_ATTACH_TEXT );
        }
    CompleteStateAction();
    }

// -----------------------------------------------------------------------------
// CMsgMailFetchFileOp::AddMultipleAttachmentsL
// -----------------------------------------------------------------------------
//
void CMsgMailFetchFileOp::AddMultipleAttachmentsL(
    CDesCArray& aAttachments,
    CMsgMailEditorDocument& aDocument,
    TRequestStatus& aStatus ) const
    {
    TInt count( aAttachments.Count() );
    for ( TInt i( 0 ); i < count; i++ )
        {
        TPtrC attaName = aAttachments.MdcaPoint( i );
        AddSingleAttachmentL( attaName, aDocument, aStatus );
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailFetchFileOp::AddSingleAttachmentL
// -----------------------------------------------------------------------------
//
void CMsgMailFetchFileOp::AddSingleAttachmentL(
    const TDesC& aAttachmentName,
    CMsgMailEditorDocument& /*aDocument*/,
    TRequestStatus& aStatus ) const
    {
    // Add attachment to message
    TInt attaSize( MailUtils::FileSize( aAttachmentName ) );
    User::LeaveIfError( attaSize );

    CMsvAttachment* info = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL(info);

    // info needs to be updated before attachment
    // is saved to mail message.
    // Mime type needs to be set later, see RestoreAttachmentsL
    info->SetAttachmentNameL( aAttachmentName );
    info->SetSize( attaSize );
    iManager->AddAttachmentL( aAttachmentName, info, aStatus );
    CleanupStack::Pop( info ); // info, ownership transferred
    }

// End Of File
