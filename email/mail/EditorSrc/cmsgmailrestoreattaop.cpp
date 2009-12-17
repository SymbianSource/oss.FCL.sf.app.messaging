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
* Description:  State machine -based operation for restoring sinle attachment.
*
*/



// INCLUDE FILES
#include "cmsgmailrestoreattaop.h"
#include "MsgMailEditorDocument.h"
#include "MsgMailDRMHandler.h"
#include "MailUtils.h"
#include "MailLog.h"
#include <MsgEditorAppUi.rsg>       // resource identifiers
#include <MsgMailEditor.rsg>
#include <mmsvattachmentmanager.h>
#include <MsgAttachmentModel.h>
#include <f32file.h>
#include <caf/caferr.h>

//Content-Transfer-Encoding: types
_LIT8(KMimeApplicationOctet, " application/octet-stream");
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttaOp::CMsgMailRestoreAttaOp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgMailRestoreAttaOp::CMsgMailRestoreAttaOp(
    TInt aIndex,
    CMsgMailEditorDocument& aDocument )
    : CMsgMailBaseOp( aDocument ),
      iIndex( aIndex )
    {
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttaOp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreAttaOp::ConstructL()
    {
    LOG( "CMsgMailRestoreAttaOp::ConstructL, creating DRM handler..." );
    iDrmHandler = MsgMailDRMHandler::NewL();
    LOG( "CMsgMailRestoreAttaOp::ConstructL, getting atta mgr..." );
    iManager = &( iDocument.GetAttachmentManagerL() );
    LOG( "CMsgMailRestoreAttaOp::ConstructL, getting atta handle..." );    
    LOG( "CMsgMailRestoreAttaOp::ConstructL, ...finished" );
    }

// -----------------------------------------------------------------------------
// CMsgMailRestoreAttaOp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgMailRestoreAttaOp* CMsgMailRestoreAttaOp::NewL(
    TInt aIndex,
    CMsgMailEditorDocument& aDocument )
    {
    CMsgMailRestoreAttaOp* self =
        new( ELeave ) CMsgMailRestoreAttaOp( aIndex, aDocument );
    CleanupStack::PushL( self );
    self->ConstructL();
    
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CMsgMailRestoreAttaOp::~CMsgMailRestoreAttaOp()
    {
    LOG( "CMsgMailRestoreAttaOp::~CMsgMailRestoreAttaOp" );
    Cancel();    
    delete iDrmHandler;
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttaOp::HandleStateActionL
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreAttaOp::HandleStateActionL()
    {
    switch ( iState )
        {
        case ECheckAttaMime:
            {
            CheckAttaMimeL();
            break;
            }
        case ECheckAttaDRM:
            {
            CheckAttaDRML();
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
// CMsgMailRestoreAttaOp::SetNextState
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreAttaOp::SetNextState()
    {
    switch ( iState )
        {
        case EIdleState:
            {
            iState = ECheckAttaMime;
            break;
            }
        case ECheckAttaMime:
            {
            iState = ECheckAttaDRM;
            break;
            }
        case ECheckAttaDRM:     
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
// CMsgMailRestoreAttaOp::HandleOperationCancel
// -----------------------------------------------------------------------------
//
void CMsgMailRestoreAttaOp::HandleOperationCancel()
    {
    LOG( "CMsgMailRestoreAttaOp::HandleOperationCancel" );
    if ( iState == ECheckAttaMime )
        {     
        // cancel possibly ongoing ModifyAttachmentInfoL() request, doesn't do
        // anything if request isn't outstanding
        LOG( "HandleOperationCancel, cancel manager's request" );        
        iManager->CancelRequest();
        }
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttaOp::CheckAttaMimeL
// -----------------------------------------------------------------------------
//    
void CMsgMailRestoreAttaOp::CheckAttaMimeL()
    {
    CMsvAttachment* info = iManager->GetAttachmentInfoL( iIndex );
    CleanupStack::PushL( info );
    const TDesC8& infoMime = info->MimeType();

	if ( infoMime.Length() == 0 || 
	     !( KMimeApplicationOctet().FindF( infoMime ) == KErrNotFound ) )
	    {
	    // mime type missing. try to resolve it
	    RFile atta = iManager->GetAttachmentFileL( iIndex );
	    TDataType mime = iDocument.ResolveMimeType( atta );
	    atta.Close();
	    //Check if recognizing failed
	    if( mime.Des().Length() > 0 )
            {
            info->SetMimeTypeL( mime.Des8() );
            // takes ownership of info
            iManager->ModifyAttachmentInfoL( info, iStatus );
            SetActive(); // asynchronous request made -> set active
            CleanupStack::Pop( info );        
            }    
	    }

    // if atta info isn't modified then we must complete this step manually,
    // and also take care of deleting info
    if ( !IsActive() )	    
        {
        CompleteStateAction();
        CleanupStack::PopAndDestroy( info );        
        }
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttaOp::CheckAttaDRML
// -----------------------------------------------------------------------------
//    
void CMsgMailRestoreAttaOp::CheckAttaDRML()
    {
    CMsvAttachment* info = iManager->GetAttachmentInfoL( iIndex );
    CleanupStack::PushL( info );    
    
    RFile atta = iManager->GetAttachmentFileL( iIndex );
    CleanupClosePushL( atta );
	CMsgAttachmentInfo::TDRMDataType drmType = 
	    iDrmHandler->GetDRMDataTypeL( atta );
    TBool isClosedFile( MailUtils::IsClosedFileL( atta ) );
    // atta
    CleanupStack::PopAndDestroy(); // CSI: 12 # RFile
	    
	if ( drmType == CMsgAttachmentInfo::EForwardLockedOrCombinedDelivery ||
	     isClosedFile )
        {
        // Context might be changed, so refresh manager reference
        // This is some historical magic that could be made nicer
        iManager = &( iDocument.GetAttachmentManagerL() );
        // Remove attachment
        iManager->RemoveAttachmentL( info->Id(), iStatus );
        iIsFLAtta = ETrue;
        SetActive(); // asynchronous request made -> set active	
        }
	else
		{
        TDataType mime( info->MimeType() );
	    iDocument.AttachmentModel().AddAttachmentL( info->AttachmentName(),
		                                            info->Size(),
                                            		info->Id(),
                                            		info->Complete(),
                                            		mime,
                                            		drmType );   
		iIsFLAtta = EFalse;
		CompleteStateAction(); // nothing async -> complete manually
		}
    CleanupStack::PopAndDestroy( info ); 
    }
    
// -----------------------------------------------------------------------------
// CMsgMailRestoreAttaOp::Finalize
// -----------------------------------------------------------------------------
//    
void CMsgMailRestoreAttaOp::Finalize()
    {
    if ( iIsFLAtta )
        { // complete the operation with error code indicating that attachment
          // was removed due to DRM reasons
        CompleteStateAction( KErrCANoPermission );
        }    
    else
        { // no problem
        CompleteStateAction();
        }
    }
        
// End Of File
