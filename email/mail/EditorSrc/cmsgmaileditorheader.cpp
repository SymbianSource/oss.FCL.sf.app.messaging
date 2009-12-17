/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Mail editor header content handler.
*
*/

#include <MsgEditorView.h>
#include <MsgEditor.hrh>
#include "msgmaileditor.hrh"
#include <MsgAddressControl.h>
#include <MsgAttachmentControl.h>
#include <MsgAttachmentModel.h>
#include "cmsgmaileditorheader.h"
#include "MsgMailEditorDocument.h"
#include <MsgMailEditor.rsg>
#include "CMailCRHandler.h"
#include "MailInternalCRKeys.h"
#include "MailLog.h"

// ---------------------------------------------------------------------------
// CMsgMailEditorHeader
// ---------------------------------------------------------------------------
//
CMsgMailEditorHeader::CMsgMailEditorHeader( CMsgMailEditorDocument& aDocument,
    CMsgEditorView& aView): iDocument( aDocument ), 
    iView( aView )
    {
    iDocument.AttachmentModel().SetObserver( this );
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CMsgMailEditorHeader::ConstructL()
    {
    CreateHeadersL();
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CMsgMailEditorHeader* CMsgMailEditorHeader::NewL(
    CMsgMailEditorDocument& aDocument,
    CMsgEditorView& aView )
    {
    CMsgMailEditorHeader* self = new( ELeave ) CMsgMailEditorHeader(
        aDocument,
        aView );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CMsgMailEditorHeader
// ---------------------------------------------------------------------------
//
CMsgMailEditorHeader::~CMsgMailEditorHeader()
    {

    } 
    
// ----------------------------------------------------------------------------
// UpdateHeaderVisibilityL
// ----------------------------------------------------------------------------
//
void CMsgMailEditorHeader::UpdateHeaderVisibilityL(
    RPointerArray<TAdditionalHeaderStatus>& aHeaders )
    {
	TInt focusedControl(KErrNotFound);
	TInt controlCount = 0; // to field is always present
	
	for ( TInt index=0; index< aHeaders.Count(); ++index )
		{
		TAdditionalHeaderStatus* header = aHeaders[index];
		TMsgControlId controlID = header->iHeaderValue;
		CMsgBaseControl* headerControl = 
			iView.ControlById( controlID );
		
		if ( headerControl )
			{
			if ( header->iStatus == EHeaderHidden )
				{
				iView.DeleteControlL(controlID );
				}
			else
				{
				++controlCount;
				}
				
			}
		else
			{
			if ( header->iStatus == EHeaderVisible || 
		    	header->iStatus == EHeaderOnlySave )
		    	{
				AddControlL( controlID );
				focusedControl = controlID;			    	
		    	}
			}

		}
	
	if ( focusedControl >=0 )	
		{
		iView.SetFocus( focusedControl );
		}	    
    }

// ----------------------------------------------------------------------------
// show / hide reply to control
// ----------------------------------------------------------------------------
//
void CMsgMailEditorHeader::UpdateReplyToControlL()
    {	    
    const CImHeader& header = iDocument.HeaderL();
    TPtrC replyto = header.ReplyTo();
    if ( replyto.Length() )
        {
        AddControlL( EMailEditorControlReplyTo );
    	CMsgAddressControl* addressCtrl = static_cast<CMsgAddressControl*> (
    	    iView.ControlById( EMailEditorControlReplyTo ));
    	ASSERT( addressCtrl );
    	addressCtrl->Reset();
        addressCtrl->SetReadOnly( ETrue );

        MVPbkContactLink* link( NULL );
		addressCtrl->AddRecipientL(
			        KNullDesC(), replyto, ETrue, link );       
        }
    else
        {
		CMsgBaseControl* field = iView.RemoveControlL( 
		    EMailEditorControlReplyTo );
		if ( field )
		    {
    		field->Reset();
    		delete field;
    		field = NULL;  		    
		    }      
        }        
    }

// ----------------------------------------------------------------------------
// AddControlL
// ----------------------------------------------------------------------------
//
void CMsgMailEditorHeader::AddControlL( TInt aControlType )
	{
    TInt resourceId(0);
    TInt controlType(0);

    switch (aControlType)
        {
        case EMsgComponentIdTo:
            {
            resourceId = R_MAIL_EDITOR_TO;
            controlType = EMsgAddressControl;
            break;
            }
        case EMsgComponentIdCc:
            {
            resourceId = R_MAIL_EDITOR_CC;
            controlType = EMsgAddressControl;
            break;
            }
        case EMsgComponentIdBcc:
            {
            resourceId = R_MAIL_EDITOR_BCC;
            controlType = EMsgAddressControl;
            break;
            }        
        case EMailEditorControlReplyTo:
            {
            resourceId = R_MAIL_EDITOR_REPLYTO;
            controlType = EMsgAddressControl;
            break;
            }                
        case EMsgComponentIdSubject:
            {
            resourceId = R_MAIL_EDITOR_SUBJECT;
            controlType = EMsgExpandableControl;
            break;
            }
        default:
            break;
        }

    if ( !iView.ControlById( aControlType ) )
        {
        // Mail editor control order
        const TInt TControls[] =
            {
            EMsgComponentIdTo,
            EMsgComponentIdCc,
            EMsgComponentIdBcc,
            EMailEditorControlReplyTo,
            EMsgComponentIdSubject
            };

        const TInt count( sizeof(TControls) / sizeof(TInt) );
    	TInt location( EMsgFirstControl ); // find correct location for control
    	for( TInt index(0); index<count; ++index )
    	    {
    	    __ASSERT_DEBUG( index >= 0 && index < count, User::Invariant() );
    	    
    	    if ( iView.ControlById( TControls[ index ] ) &&				// CSI: 2 # This is checked above in the assert macro.
    	        TControls[ index ] != aControlType )					// CSI: 2 # This is checked above in the assert macro.
    	        {
    	        ++location;
    	        }
    	    else if ( TControls[ index ] == aControlType )				// CSI: 2 # This is checked above in the assert macro.
    	        {
    	        // we are here, so no need to check rest of controls
    	        break;
    	        }
    	    }        
                
        iView.AddControlFromResourceL(
            resourceId, controlType,
            location, EMsgHeader); 
        }			
	}

// ----------------------------------------------------------------------------
// AttachmentControl
// ----------------------------------------------------------------------------
//
CMsgAttachmentControl* CMsgMailEditorHeader::AttachmentControl() const
    {
    CMsgBaseControl* ctrl = iView.ControlById( EMsgComponentIdAttachment );
    ASSERT( ctrl );
            
    return static_cast<CMsgAttachmentControl*>( ctrl );
    }
    
// ----------------------------------------------------------------------------
// CreateHeadersL
// ----------------------------------------------------------------------------
//
void CMsgMailEditorHeader::CreateHeadersL()
    {
    AddControlL( EMsgComponentIdTo );
	
	TInt headersvalue(0);
    CMsgMailPreferences& prefs = iDocument.SendOptions();
    iDocument.MailCRHandler()->GetCRInt(
    	KCRUidMail,KMailAdditionalHeaderSettings,headersvalue);
	prefs.SetAdditionalHeaders( headersvalue );
	
	TAdditionalHeaderStatus header_status_cc;
    TAdditionalHeaderStatus header_status_bcc;
    TAdditionalHeaderStatus header_status_subject;

    header_status_cc.iStatus = 
    	prefs.GetAdditionalHeaderVisibility(EMsgComponentIdCc);
    header_status_bcc.iStatus = 
    	prefs.GetAdditionalHeaderVisibility(EMsgComponentIdBcc);
    header_status_subject.iStatus = 
    	prefs.GetAdditionalHeaderVisibility(EMsgComponentIdSubject);
	
	const CImHeader& header = iDocument.HeaderL();
	const CDesCArray& ccArr = header.CcRecipients();
	if(header_status_cc.iStatus == EHeaderVisible || ccArr.Count() > 0)
		{
		AddControlL(EMsgComponentIdCc);
		}
    // Check if bcc field is allowed
    if ( iDocument.MailCRHandler()->MailAdditionalHeaders() )
        {   
    	const CDesCArray& bccArr = header.BccRecipients();
    	if(header_status_bcc.iStatus == EHeaderVisible || bccArr.Count() > 0)
    		{
    		AddControlL(EMsgComponentIdBcc);
    		}
        }
    // Reply-to
    UpdateReplyToControlL();
    
    // Subject   
	const TPtrC& subjectStr = header.Subject();
	if(header_status_subject.iStatus == 
		EHeaderVisible || 
		subjectStr.Length() > 0)
		{
		AddControlL(EMsgComponentIdSubject);
		}    
    }
        
// ----------------------------------------------------------------------------
// DoCreateAttachmentControlL
// ----------------------------------------------------------------------------
//
void CMsgMailEditorHeader::DoCreateAttachmentControlL()
    {
    if ( !iView.ControlById( EMsgComponentIdAttachment ) )
        {
        LOG( "CMsgMailEditorAppUi::DoCreateAttachmentControlL() - create" );
        CMsgAttachmentControl* control = 
            CMsgAttachmentControl::NewL( iView, iView );
        CleanupStack::PushL( control );
        iView.AddControlL( control,    
            EMsgComponentIdAttachment,    
            EMsgAppendControl, 
            EMsgHeader );
        CleanupStack::Pop( control );
        }
    }
    
// ----------------------------------------------------------------------------
// DoRemoveAttachmentControlL
// ----------------------------------------------------------------------------
//
void CMsgMailEditorHeader::DoRemoveAttachmentControlL()
    {
    CMsgBaseControl* control = 
        iView.RemoveControlL( EMsgComponentIdAttachment );
    if ( control )
        {
        delete control;
        control = NULL;
        }
    }    

// ----------------------------------------------------------------------------
// UpdateAttachmentControlL
// ----------------------------------------------------------------------------
//
void CMsgMailEditorHeader::UpdateAttachmentControlL( 
    TMsgAttachmentCommand aCommand,
    CMsgAttachmentInfo* aAttachmentInfo )
    {
    CMsgAttachmentModel& model = iDocument.AttachmentModel();
    if ( aCommand == EMsgAttachmentAdded )
        {
        DoCreateAttachmentControlL();
        AttachmentControl()->AddAttachmentsL( model );        
        }
    else // to be removed
        {
        if ( model.NumberOfItems() == 1 ) // 1 = we are removing last atta
            {
            DoRemoveAttachmentControlL();
            }
        else
            {
            ASSERT( aAttachmentInfo );
            TParsePtrC fileParser( aAttachmentInfo->FileName() );          
            AttachmentControl()->RemoveAttachmentL( 
                 fileParser.NameAndExt() );
            }
        }
    }

// ---------------------------------------------------------------------------
// From class MMsgAttachmentModelObserver.
// NotifyChanges
// ---------------------------------------------------------------------------
//
void CMsgMailEditorHeader::NotifyChanges( 
    TMsgAttachmentCommand aCommand, 
    CMsgAttachmentInfo* aAttachmentInfo )
    {
    LOG1( "CMsgMailEditorHeader::NotifyChanges: %d", aCommand );
    iDocument.NotifyChanges( aCommand, aAttachmentInfo );
    TRAP_IGNORE( UpdateAttachmentControlL( aCommand, aAttachmentInfo ) );
    }

// ---------------------------------------------------------------------------
// From class MMsgAttachmentModelObserver.
// GetAttachmentFileL
// ---------------------------------------------------------------------------
//    
RFile CMsgMailEditorHeader::GetAttachmentFileL( TMsvAttachmentId aId )
    {
    return iDocument.GetAttachmentFileL( aId );
    }

// End of File

