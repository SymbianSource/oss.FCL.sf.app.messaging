/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MeetingRequestSender implementation
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include <SendUi.h>                     // TSendingCapabilities
#include <e32svr.h>
#include <mtclreg.h>                    // CClientMtmRegistry
#include <mtclbase.h>                   // CBaseMtm
#include <miuthdr.h>
#include <txtrich.h>
#include <MuiuMsvUiServiceUtilities.h>
#include <s32mem.h>                     // CBufStore
#include <apmstd.h>
#include <mtmextendedcapabilities.hrh>  // KUidMsvMtmQuerySupportLinks
#include <apgcli.h>
#include <mtmuids.h>
#include <mtmdef.h>

#include <CSendingServiceInfo.h>
#include <CMessageAddress.h>
#include <SendUiConsts.h>               // UIDs for provider and service
#include <CMessageData.h>               // CMessageData
#include <MeetingRequestData.h>         // CMeetingRequestData
#include <senduisingleton.h>

#include "SendUiDummyMtm.h"
#include "SendUiOperationWait.h"
#include "SenduiDataUtils.h"
#include "SenduiFileRightsEngine.h"
#include "CSendUiAttachment.h"
#include <MeetingRequestIds.hrh>
#include "MeetingRequestSender.h"

namespace
    {
    _LIT8( KContentType, "multipart" );
    _LIT8( KContentSubType,"/alternative; boundary=mr_part; method=");
    _LIT( KMsgInvalidSessionEvent, "CMeetingRequestSender: Invalid session event %x" );
    _LIT( KMsgEmptyAddreesseeArray,"CMeetingRequestSender:: Addressee array is empty! Message cannot be created." );
    _LIT( KMailtoMatchPattern, "mailto:*" ); // this is never localized
    const TInt KMailtoLength = 7; // "mailto:" length
    
    //  LOCAL CONSTANTS 
    // For address information separation (start)
    const TUint KSendUiMsgAddressStartChar         ('<');

    // For address information separation (end)
    const TUint KSendUiMsgAddressEndChar           ('>');

    const TInt KRichTextStoreGranularity = 512;
    const TInt KContentSubTypeMaxLength( 60 );
    
    const TUint KTransferBufAdditionalInfoMaxLen = 20;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMeetingRequestSender::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMeetingRequestSender* CMeetingRequestSender::NewL( TSendingServiceParams* aParams )
    {
    CMeetingRequestSender* self = CMeetingRequestSender::NewLC( aParams );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMeetingRequestSender* CMeetingRequestSender::NewLC( TSendingServiceParams* aParams )
    {
    CMeetingRequestSender* self = new( ELeave ) CMeetingRequestSender( aParams->iCoeEnv, aParams->iSingleton );
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::~CMeetingRequestSender()
// Destructor.
// -----------------------------------------------------------------------------
//
CMeetingRequestSender::~CMeetingRequestSender()
    {
    iServiceArray.ResetAndDestroy();
    iServiceArray.Close();

    delete iStore;

    delete iClientMtm;
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::CMeetingRequestSender
// Constructor.
// -----------------------------------------------------------------------------
//

CMeetingRequestSender::CMeetingRequestSender( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton ) : 
    CSendingService( aCoeEnv, aSingleton )
    {
    }
    
// -----------------------------------------------------------------------------
// CMeetingRequestSender::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::ConstructL()
    {
    InitializeServiceL();   
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::PopulateServicesListL
// Populates given list with the services provided by this plugin.
// The ownership of the pointed objects remains.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::PopulateServicesListL(
    RPointerArray<CSendingServiceInfo>& aList )
    {
    for ( TInt cc = 0; cc < iServiceArray.Count(); cc++ )
        {
        User::LeaveIfError( aList.Append( iServiceArray[cc] ) );
        }
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::CreateAndSendMessageL
// Creates message to be sent and calls MTM to send the message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::CreateAndSendMessageL(
    TUid aServiceUid,
    const CMessageData* aMessageData,
    TBool /*aLaunchEditorEmbedded*/)
    {

    if( !aMessageData ||
        aServiceUid != KMeetingRequestSender ||
        aMessageData->DataType() != KSenduiMsgDataMeetingRequest ) 
        {
        User::Leave( KErrNotSupported );
        }

    CMeetingRequestData* mrData = CMeetingRequestData::NewLC();
    mrData->ReadFromBufferL( aMessageData->OpaqueData() );

    InitResourcesL( mrData );

    //In case of SMTP we just create message and don't check capabilities,
    //because SMTP MTM doesn't understand capabilities at the moment.
    if( iClientMtm->Type() == KSenduiMtmSmtpUid )
        {
        DoCreateAndSendMessageL( *aMessageData, mrData );
        }
    else
        {
        TUid supportMeetingRequestUid =  TUid::Uid(KMtmUiSupportMeetingRequest);
        TUid supportICalCapability =  TUid::Uid(KMtmUiSupportICalCapability);
        TInt capabilityResponse;

        TInt errorCode = iClientMtm->QueryCapability(supportMeetingRequestUid, capabilityResponse);
                
        //Check if Meeting request is supported at all. If not then leave.
        if(errorCode == KErrNotSupported || capabilityResponse == 0)
            {
            User::Leave( KErrNotSupported );
            }

        errorCode = iClientMtm->QueryCapability(supportICalCapability, capabilityResponse);
        
        //Check if MTM supports supportICalCapability. 
        if ( (errorCode != KErrNotSupported) && (capabilityResponse != 0))
            {
            DoCreateAndSendMessageL( *aMessageData, mrData );
            }
        else
            {
            TransferIdsToClientL( *aMessageData );
            
            DoCreateAndSendMessageL( *aMessageData, mrData );
            }
        }
    CleanupStack::PopAndDestroy( mrData );
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::DoCreateAndSendMessageL
// Creates message to be sent and calls MTM to send the message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::DoCreateAndSendMessageL(
    const CMessageData& aMessageData,
    CMeetingRequestData* aMeetingRequestData )
    {
    CreateMessageL( aMessageData );
    
    TInt capabilityResponse;
    //Capability to support some kind of body checked 
    TInt errorCode = iClientMtm->QueryCapability( 
        TUid::Uid( KUidMtmQuerySupportedBodyValue ), 
        capabilityResponse );
    
    if(capabilityResponse)
        {
        SetBodyL( aMessageData );    
        }

    AddAttachmentsL( aMessageData );

    // Create message header(s)
    CreateHeaderL( aMessageData, aMeetingRequestData );

    iClientMtm->SaveMessageL();
        
    //QueryCapability() can't be used here because client gives false answer.
    //So only way to do this is check MTM type.
    if(iClientMtm->Type() == KSenduiMtmSmtpUid)
        {
        if( !ValidateEmail( ) )
            {
            User::Leave( KErrCompletion );
            }
        }
        
    SendEmailL( );
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::TransferIdsToClientL
// Transfers meeting request IDs to client MTM.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::TransferIdsToClientL(
    const CMessageData& aMessageData )
    {
    // Create copy of opaque data. Needed because InvokeSyncFunctionL wants 
    // non-const buffer.
    HBufC8* tempBuf = aMessageData.OpaqueData().AllocLC();
    TPtr8 ptr = tempBuf->Des();

    // Dummy entry selection
    CMsvEntrySelection* entrySelection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( entrySelection );
    
    //Send data to MTM with InvokeSyncFunction()
    iClientMtm->InvokeSyncFunctionL(
            KMTMSendMeetingRequestFunctionId,
            *entrySelection,
            ptr );
                        
   CleanupStack::PopAndDestroy( 2, tempBuf ); // entrySelection, buf
    
   }

// ---------------------------------------------------------
// CMeetingRequestSender::ServiceProviderId
// Returns the id of the service provider.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TUid CMeetingRequestSender::ServiceProviderId() const
    {
    return KMeetingRequestSender; // defined in SendUiConsts.h
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::InitializeServiceL
// Initializes service info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::InitializeServiceL()
    {
    if( iServiceArray.Count() )
        {
        // Already initialized so do nothing
        return;
        }

    CSendingServiceInfo* serviceInfo = CSendingServiceInfo::NewLC();

    // Set service type.
    serviceInfo->SetServiceProviderId( KMeetingRequestSender );
    serviceInfo->SetServiceId( KMeetingRequestSender );
    iServiceArray.AppendL( serviceInfo );

    CleanupStack::Pop( serviceInfo );
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::CreateMessageL
// Creates new message with subject. Index entry is initialized and 
// new message is created in drafs folder. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::CreateMessageL(
    const CMessageData& aMessageData )
    {
    CMsvEntry* draftEntry = iSingleton.MsvSessionL().GetEntryL( KMsvDraftEntryId );
    CleanupStack::PushL(draftEntry);
    iClientMtm->SetCurrentEntryL( draftEntry ); // mtm takes ownership
    CleanupStack::Pop(draftEntry);

    // Create new message under drafts
    iClientMtm->CreateMessageL( iMailServiceId );

    // Load email signature, if any
    iClientMtm->LoadMessageL();

    // Set message subject
    iClientMtm->SetSubjectL( aMessageData.Subject() );

    // Set attributes on index entry
    TMsvEntry indexEntry = iClientMtm->Entry().Entry();
    indexEntry.SetInPreparation( ETrue );
    indexEntry.SetVisible( EFalse );
    indexEntry.iDate.UniversalTime();
    indexEntry.SetAttachment( EFalse );

    iClientMtm->Entry().ChangeL( indexEntry );
    
    
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::InitResourcesL
// Client MTM is initialized. Used SMTP service is validated. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::InitResourcesL(
    CMeetingRequestData* aMeetingRequestData )
    {
    iMailServiceId = aMeetingRequestData->Mailbox();

    if ( iMailServiceId == KMsvNullIndexEntryId )
        {
        User::Leave( KErrArgument );
        }

    ResolveClientMTMTypeL( iMailServiceId );

    if ( iClientMtmType == KNullUid )
        {
        User::Leave( KErrArgument );
        }

    // Create client side MTM object.
    delete iClientMtm;
    iClientMtm = NULL;

    iClientMtm = iSingleton.ClientMtmRegistryL().NewMtmL( iClientMtmType );
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::ResolveClientMTMTypeL
// Resolves the client MTM type by using service id. KNullUid is returned if
// service id doesn't match to any available service.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::ResolveClientMTMTypeL( TMsvId aServiceId )
    {
    TInt i = 0;

    CMsvEntrySelection* entrySelection = 
        MsvUiServiceUtilities::GetListOfAccountsL( iSingleton.MsvSessionL(), ETrue );
    CleanupStack::PushL( entrySelection );

    CMsvEntry* rootEntry = CMsvEntry::NewL(
        iSingleton.MsvSessionL(),
        KMsvRootIndexEntryId,
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    CleanupStack::PushL( rootEntry );    

    TMsvId msvId;
    for ( i = 0; i < entrySelection->Count(); i++ )
        {
        msvId = entrySelection->At( i );
        
        if ( msvId == aServiceId )
            {
            TMsvEntry tEntry = rootEntry->ChildDataL( msvId );
            iClientMtmType = tEntry.iMtm;
            break;    
            }
        }

    CleanupStack::PopAndDestroy( 2, entrySelection ); // entrySelection, rootEntry
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::SetBodyL
// Sets email body. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::SetBodyL(
    const CMessageData& aMessageData )
    {
    // If body has been not set return immediately
    if( !aMessageData.BodyText()->DocumentLength() )
        {
        return;
        }

    const CRichText& bodyText = *( aMessageData.BodyText() );
    CRichText& mtmBody = iClientMtm->Body();
    
    delete iStore;
    iStore = NULL;
    
    iStore = CBufStore::NewL( KRichTextStoreGranularity );

    // If signature has been set then append it at the end of email
    if ( mtmBody.DocumentLength() )
        {
        CRichText* text = CRichText::NewL(
            static_cast<CEikonEnv&>( iCoeEnv ).SystemParaFormatLayerL(),
            static_cast<CEikonEnv&>( iCoeEnv ).SystemCharFormatLayerL() );
        CleanupStack::PushL( text );

        TInt len1 = bodyText.DocumentLength();
        TInt len2 = mtmBody.DocumentLength();

        HBufC* buf = HBufC::NewLC( len1 );
        TPtr ptr = buf->Des();
        bodyText.Extract( ptr, 0 );
        text->InsertL( 0, *buf );
        CleanupStack::PopAndDestroy( buf );

        len1 = text->DocumentLength();
        text->InsertL( len1, CEditableText::EParagraphDelimiter );
        len1 = text->DocumentLength();
        text->InsertL( len1, mtmBody.Read( 0, len2 ) );

        // Save and restore the rich text
        TStreamId id = text->StoreL( *iStore );
        mtmBody.RestoreL( *iStore, id );

        CleanupStack::PopAndDestroy(text);
        }
    else
        {
        // Save and restore the rich text
        TStreamId id = bodyText.StoreL( *iStore );
        mtmBody.RestoreL( *iStore, id );
        }

    // Get access to protected data
    CSendUiDummyMtm* mtm = ( CSendUiDummyMtm* ) iClientMtm;

    mtm->SetCharFormatL( *bodyText.GlobalCharFormatLayer() );
    mtm->SetParaFormatL( *bodyText.GlobalParaFormatLayer() );
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::CreateHeaderL
// Create email headers. "Standard" and MIME header is initialized.  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::CreateHeaderL(
    const CMessageData& aMessageData,
    CMeetingRequestData* aMeetingRequestData )
    {
    InitStdHeaderL( aMessageData );
    
    //At the moment only way to check mime capability is to use iClientMtm->Type()
    if(iClientMtm->Type() == KSenduiMtmSmtpUid)
        {
        InitMimeHeaderL( aMeetingRequestData );
        }
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::InitStdHeaderL
// Initialize standard header. To, cc and bcc fields are set. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::InitStdHeaderL(
    const CMessageData& aMessageData )
    {
    // Nothing to do if aTo array is empty
    if ( aMessageData.ToAddressArray().Count() == 0 ) 
        {
        RDebug::Print( KMsgEmptyAddreesseeArray );
        User::Leave( KErrArgument );
        }

    AddAddresseesL( EMsvRecipientTo, aMessageData.ToAddressArray() );

    AddAddresseesL( EMsvRecipientCc, aMessageData.CcAddressArray() );    

    AddAddresseesL( EMsvRecipientBcc, aMessageData.BccAddressArray() );
    }
    
// -----------------------------------------------------------------------------
// CMeetingRequestSender::AddAddresseesL
// Add To addressees.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CMeetingRequestSender::AddAddresseesL(
    TMsvRecipientType               aRecipientType,
    const CMessageAddressArray&     aAddressees )
    {
    TInt i(0);

    for( i = 0; i < aAddressees.Count(); i++ )
        {
        if( aAddressees[i]->Address().Length() > 0 )
            {
            // If address has an alias, process it            
            if( aAddressees[i]->Alias().Length() > 0 )
                {
                TPtrC alias = aAddressees[i]->Alias();

                // Check if alias contains illegal characters. If it does
                // remove them. 
                if( ( alias.Locate( KSendUiMsgAddressStartChar ) != KErrNotFound  ) ||
                    ( alias.Locate( KSendUiMsgAddressEndChar ) != KErrNotFound ) )
                    {
                    HBufC* aliasNew = alias.AllocLC();
                    RemoveIllegalChars( aliasNew );
                    iClientMtm->AddAddresseeL(
                        aRecipientType, 
                        AttendeeAdressWithoutPrefix( aAddressees[i]->Address() ), 
                        *aliasNew );

                    CleanupStack::PopAndDestroy( aliasNew );
                    }
                else
                    {
                    iClientMtm->AddAddresseeL(
                        aRecipientType, 
                        AttendeeAdressWithoutPrefix(aAddressees[i]->Address() ), 
                        alias );
                    }
                }
            else 
                {
                // Address entry didn't have alias
                iClientMtm->AddAddresseeL( aRecipientType, 
                    AttendeeAdressWithoutPrefix( aAddressees[i]->Address() ));
                }
            }
        // Done setting one addressee
        }
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::RemoveIllegalChars
// Remove illegal chars from string. Illegal chars are replaced with space
// and trimmed string is returned.
// Should everything be removed between KSendUiMsgAddressStartChar and
// KSendUiMsgAddressEndChar?
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CMeetingRequestSender::RemoveIllegalChars( HBufC* aCheckedString )
    {
    TInt stringLength = aCheckedString->Length();
    TPtr string = aCheckedString->Des();

    while( stringLength-- )
        {
        if( string[stringLength] == KSendUiMsgAddressStartChar ||
            string[stringLength] == KSendUiMsgAddressEndChar )
            {
            string[stringLength] = CEditableText::ESpace;
            }
        }
    string.TrimAll();
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::InitMimeHeaderL
// Mime message header is initialized with user defined content type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CMeetingRequestSender::InitMimeHeaderL(
    CMeetingRequestData* aMeetingRequestData )
    {
    CMsvEntry& entry = iClientMtm->Entry();
    CMsvStore* msvStore = entry.EditStoreL();
    CleanupStack::PushL( msvStore );

    CImMimeHeader* header = CImMimeHeader::NewLC();

    // Set content type
    header->SetContentTypeL( KContentType ); 
    
    TBuf8<KContentSubTypeMaxLength> subtype( KContentSubType() );
    subtype.Append( aMeetingRequestData->MailHeaderMethod() );
    header->SetContentSubTypeL( subtype ); 

    header->StoreL( *msvStore );
    msvStore->CommitL();

    CleanupStack::PopAndDestroy( 2, msvStore );
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::AddAttachmentsL
// Attachments are added to the email. Method will leave if there is not 
// enough space for attachments.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CMeetingRequestSender::AddAttachmentsL(
    const CMessageData& aMessageData )
    {
    TInt i(0);
    

    CArrayPtrFlat<CSendUiAttachment>* attachments;
    attachments = CSendUiAttachment::InitAttachmentArrayLCC( 
        aMessageData.AttachmentArray(), 
        aMessageData.AttachmentHandleArray(),
        iCoeEnv.FsSession() );
    
    
    // If there are no attachments, return immediately. 
    if( !attachments->Count() )
        {
        return;
        }

    // Get total size of attachments.
    CSendUiFileRightsEngine* fileRightsEngine =
        CSendUiFileRightsEngine::NewLC( iCoeEnv.FsSession() );

    // Check if linked attachments are supported. 
    TInt supportsLinks = 0;
    TUid uidQuery = { KUidMsvMtmQuerySupportLinks };
    iClientMtm->QueryCapability( uidQuery, supportsLinks );
  
    TInt totalSize = fileRightsEngine->CalculateTotalSizeOfFiles( attachments, supportsLinks );

    CleanupStack::PopAndDestroy( fileRightsEngine );
    // following will leave if there is not enough space
    CheckDiskSpaceL( totalSize );
  
    const RArray<RFile>& attHandles = aMessageData.AttachmentHandleArray();
    const CDesCArray& attFilePaths = aMessageData.AttachmentArray();


    for( i = 0; i < attachments->Count(); i++ )
        {
        AddAttachmentL( attachments->At(i), supportsLinks );
        }

    CleanupStack::PopAndDestroy(2); // *attachments, attacments
    TMsvEntry entry = iClientMtm->Entry().Entry();
    entry.SetAttachment( ETrue );
    iClientMtm->Entry().ChangeL(entry);
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::AddAttachmentL
// Add attachment (file descriptor)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMeetingRequestSender::AddAttachmentL(
    CSendUiAttachment* aAttachment, 
    TInt aLinksSupported )
    {
    TDataType mimeType;
    
    CSendUiDataUtils* dataUtils = CSendUiDataUtils::NewL( iCoeEnv.FsSession() );
    CleanupStack::PushL( dataUtils );
    CSendUiOperationWait* waiter = CSendUiOperationWait::NewLC();

    if ( aAttachment->Type() == CSendUiAttachment::EAttachmentPath )
    	{
        dataUtils->ResolveFileMimeTypeL( *(aAttachment->Path()), mimeType );
	  	
	  	if ( aLinksSupported )
	  		{
	  		iClientMtm->AddLinkedAttachmentL(
                *(aAttachment->Path()), mimeType.Des8(), 0, waiter->iStatus );
        	}
        	else
        		{
        	    iClientMtm->AddAttachmentL(
                    *(aAttachment->Path()), mimeType.Des8(), 0, waiter->iStatus );
        		}
	  	}
	  else
  		{
        dataUtils->ResolveFileMimeTypeL( *(aAttachment->Handle()), mimeType );
	  	RFile attachmentFile;
	    User::LeaveIfError( attachmentFile.Duplicate( *(aAttachment->Handle()) ) );
	    CleanupClosePushL( attachmentFile );
	    
	    // File handle ownership transferred.
	    iClientMtm->AddAttachmentL(
	        attachmentFile,
	        mimeType.Des8(),
	        NULL,
	        waiter->iStatus );
        CleanupStack::Pop( &attachmentFile );

  		}
    

    waiter->Start( iClientMtm );

    CleanupStack::PopAndDestroy( 2, dataUtils ); // waiter, dataUtils
    }
// -----------------------------------------------------------------------------
// CMeetingRequestSender::CheckDiskSpaceL
// Check if there are enough space for email (includes attachments). Leaves 
// if there are not enough space. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//            
void CMeetingRequestSender::CheckDiskSpaceL(
    TInt aMsgSize )
    {
    if( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
            iSingleton.MsvSessionL(), aMsgSize ) )
        {
        User::Leave( KErrDiskFull );
        }
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::ValidateEmail
// Validates email before sending it. Returns true if email is valid. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//            
TBool CMeetingRequestSender::ValidateEmail( )
    {
    TMsvPartList msgCheckParts =
        KMsvMessagePartBody |
        KMsvMessagePartRecipient |
        KMsvMessagePartOriginator |
        KMsvMessagePartDate |
        KMsvMessagePartDescription;

    TMsvPartList msgFailParts =
        iClientMtm->ValidateMessage( msgCheckParts );

    return ( msgFailParts == KMsvMessagePartNone );
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::SendEmailL
// Sends an email.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//            
void CMeetingRequestSender::SendEmailL( )
    {
    TMsvEntry entry = iClientMtm->Entry().Entry();

    entry.SetInPreparation( EFalse );
    entry.SetVisible( ETrue );
    entry.SetSendingState( KMsvSendStateWaiting );
    
    iClientMtm->Entry().ChangeL( entry );

    iClientMtm->LoadMessageL();
    entry = iClientMtm->Entry().Entry();
    TMsvId id = entry.Id();

    if( entry.Parent() != KMsvGlobalOutBoxIndexEntryId )
        {
        // move message to outbox
        MoveMessageEntryL( KMsvGlobalOutBoxIndexEntryId );
        }

    CMsvEntrySelection*  entrySelection;
    entrySelection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( entrySelection );

    entrySelection->AppendL( iMailServiceId );
    entrySelection->AppendL( id );
    
    TBuf8<1> params;

    CSendUiOperationWait* waiter = CSendUiOperationWait::NewLC();

    CMsvOperation* op = 
        iClientMtm->InvokeAsyncFunctionL(
            KMTMStandardFunctionsSendMessage,
            *entrySelection,
            params,
            waiter->iStatus );

    waiter->Start( op );

    CleanupStack::PopAndDestroy( 2, entrySelection ); // entrySelectoin, waiter
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::MoveMessageEntryL
// Moves email to target folder (usually outbox). 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//            
void CMeetingRequestSender::MoveMessageEntryL( TMsvId aTarget )
    {
    TMsvSelectionOrdering sort;
    // we want to also handle the invisible entries
    sort.SetShowInvisibleEntries( ETrue );

    // Take a handle to the parent entry
    TMsvEntry msvEntry( iClientMtm->Entry().Entry() );
    CMsvEntry* parentEntry = 
        CMsvEntry::NewL(
            iClientMtm->Session(),
            msvEntry.Parent(),
            sort );
    CleanupStack::PushL( parentEntry );

    // Move original from the parent to the new location
    parentEntry->MoveL( msvEntry.Id(), aTarget );

    CleanupStack::PopAndDestroy( parentEntry );
    }

// -----------------------------------------------------------------------------
// CMeetingRequestSender::AttendeeAdressWithoutPrefix
// Returns a new descriptor where "MAILTO:" prefix is removed if it existed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CMeetingRequestSender::AttendeeAdressWithoutPrefix( const TDesC& aAddress )
    {
    TPtrC attendeeAddrWithoutPrefix;
    if ( aAddress.MatchF( KMailtoMatchPattern ) != KErrNotFound )
        {
        attendeeAddrWithoutPrefix.Set( aAddress.Mid( KMailtoLength ) );
        }
    else
        {
        attendeeAddrWithoutPrefix.Set( aAddress );
        }
    return attendeeAddrWithoutPrefix;
    }

// -----------------------------------------------------------------------------
// TechnologyType
// -----------------------------------------------------------------------------
//
TUid CMeetingRequestSender::TechnologyTypeId( ) const
    {
    return KNullUid;
    }
    
//  END OF FILE
