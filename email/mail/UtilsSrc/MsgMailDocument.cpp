/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for document classes of the 
*                 mail viewer and editor applications
*
*/


// INCLUDE FILES
#include "MailLog.h"
#include "MailUtils.h"
#include <miutset.h>                    // KMsvEmailTypeListXX enums
#include <miuthdr.h>                    // CIMHeader
#include <miutmsg.h>                    // CImEmailMessage
#include <smtpset.h>                    // Smtp Settings
#include <MuiuMsgEditorLauncher.h>      // launch parameter constants
#include <akninputblock.h>              // CAknInputBlock
#include <MuiuOperationWait.h>          // CMuiuOperationWait
#include <MuiuMsvUiServiceUtilities.h>	// MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL
#include <miutconv.h>                   // TImEmailTransformingInfo
#include <eikenv.h>
#include <mmsvattachmentmanager.h>
#include <StringLoader.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include <cimconvertcharconv.h>
#endif

#include <MsgMailUtils.rsg>
#include "MsgMailDocument.h"            // header
#include "MsgMailPreferences.h"         // Message preferences
#include <CMailMessage.h>
#include <apmrec.h>


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// C++ default constructor cannot contain any code that might leave.
// ----------------------------------------------------------------------------
//
EXPORT_C CMsgMailDocument::CMsgMailDocument(CEikApplication& aApp)
    : CMsgEditorDocument(aApp), iMessage(NULL), iHeader(NULL),
    iIdle(NULL)
    {
    }

    
// Destructor
EXPORT_C CMsgMailDocument::~CMsgMailDocument()
    {
    iApaSession.Close();    
    delete iHeader;
    delete iMessage;
    delete iMessageCEntry;
    delete iIdle;
    }

// ----------------------------------------------------------------------------
//  CMsgMailDocument::EntryChangedL()
//  Called after entry is changed by SetEntry Function. Deletes current
//  header and body and reads the new ones from message store. 
// ----------------------------------------------------------------------------
//
EXPORT_C void CMsgMailDocument::EntryChangedL()
    {    
    delete iHeader;
    delete iMessage;
    delete iMessageCEntry;
    iHeader = NULL;
    iMessage = NULL;
    iMessageCEntry = NULL;

    iHeader = CImHeader::NewLC();
    CleanupStack::Pop();
    //CMsvEntry& messageEntry = CurrentEntry();
    iMessageID = CurrentEntry().EntryId();
    LOG1("CMsgMailDocument::EntryChangedL %08x", iMessageID);
    iMessageCEntry = CurrentEntry().Session().GetEntryL(
    	iMessageID);
    iMessage = CImEmailMessage::NewL(*iMessageCEntry);
    if (iMessageCEntry->HasStoreL())
        {
        CMsvStore* msvStore = iMessageCEntry->ReadStoreL();
        CleanupStack::PushL(msvStore);
        TRAP_IGNORE(iHeader->RestoreL(*msvStore));
        // get body encoding
        if (msvStore->IsPresentL( KUidMsgFileTransformingInfo))
        	{        	
	    	TImEmailTransformingInfo info;
	    	// use defaults if fails
	    	TRAP_IGNORE(info.RestoreL(*msvStore));
	    	TUid charset;
	    	charset.iUid = info.BodyTextCharset();
	    	SendOptions().SetSendingCharacterSet(charset);
        	}
        CleanupStack::PopAndDestroy(); // store
        }
    }

// ----------------------------------------------------------------------------
//  CMsgMailDocument::CreateReplyL()
// ----------------------------------------------------------------------------
//    
EXPORT_C CMsvOperation* CMsgMailDocument::CreateReplyL( 
    TMsvId aDest, TMsvPartList aParts, TRequestStatus& aStatus)
    {
    LOG1("CMsgMailDocument::CreateReplyL %08x", 
        CurrentEntry().Entry().Id() );
    
    CImEmailMessage* mailMessage = CImEmailMessage::NewLC( CurrentEntry() );    
    
    TBool plainHTMLEntry =  IsPlainHTMLMessageL( mailMessage );
    HBufC* attachmentNames = GetAttachmentNamesFromMessageL( mailMessage );
    CleanupStack::PopAndDestroy( mailMessage );
    CleanupStack::PushL( attachmentNames );
    
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    CMsvOperation* oper = Mtm().ReplyL( aDest, aParts, wait->iStatus );    
    wait->Start();
    TInt status( wait->iStatus.Int() );
    CleanupStack::PopAndDestroy( wait );
    
    TMsvId replyMessage = GetOperId( *oper );
    CleanupStack::PushL( oper );
    SetEntryWithoutNotificationL( replyMessage );
    
    AddAttachmentNamesToMessageBodyL( *attachmentNames );
    CleanupStack::Pop( oper );
    CleanupStack::PopAndDestroy( attachmentNames ); // attachmentNames
        
    TRequestStatus* myStatus=&aStatus;
    User::RequestComplete( myStatus, status );
    
    if(plainHTMLEntry)
        {
        //Create Idle object so we can show note 
        //after options menu is closed and editor is opened.
        
        iIdle = CIdle::NewL(CActive::EPriorityStandard);
        
        iShowNoteCallBack = TCallBack( ShowHtmlNoteCallBackL, this );
        
        iIdle->Start(iShowNoteCallBack); // CSI: 10 #     
        }
   
    return oper;
    }
    

// ----------------------------------------------------------------------------
//  CMsgMailDocument::ShowHtmlNoteCallBackL()
// ----------------------------------------------------------------------------
//
TInt CMsgMailDocument::ShowHtmlNoteCallBackL( TAny* /*aCaller*/ )	// CSI: 40 # We must return 
																	// the integer value although this 
																	// is a leaving method.
    {    
    MailUtils::InformationNoteL( R_QTN_MAIL_INFO_HTML_ATTACH );
    return 0;
    } 

// ----------------------------------------------------------------------------
//  CMsgMailDocument::CreateForwardL()
// ----------------------------------------------------------------------------
//     
EXPORT_C CMsvOperation* CMsgMailDocument::CreateForwardL(
    TMsvId aDest, TMsvPartList aParts, TRequestStatus& aStatus )
    {
    LOG1("CMsgMailDocument::CreateForwardL %08x", 
        CurrentEntry().Entry().Id() );
    CImEmailMessage* mailMessage = CImEmailMessage::NewLC( CurrentEntry() );    
    
    if(IsPlainHTMLMessageL( mailMessage ))
        {
        MailUtils::InformationNoteL( R_QTN_MAIL_INFO_HTML_ATTACH );
        }
    
    CleanupStack::PopAndDestroy( mailMessage );    
    
    return CMsgEditorDocument::CreateForwardL( aDest, aParts, aStatus );          
    }
    
// ----------------------------------------------------------------------------
//  CMsgMailDocument::TMsvEMailEntryL()
// ----------------------------------------------------------------------------
//
EXPORT_C const TMsvEmailEntry& CMsgMailDocument::TMsvEMailEntryL()
	{
	// context might be wrong
	iMessageCEntry->SetEntryL(iMessageID);
		
	return static_cast<const TMsvEmailEntry&> (CMsgEditorDocument::Entry());
	}

// ----------------------------------------------------------------------------
//  CMsgMailDocument::MessageL()
// ----------------------------------------------------------------------------
//
EXPORT_C CImEmailMessage& CMsgMailDocument::MessageL()
	{
	// context might be wrong
	iMessageCEntry->SetEntryL(iMessageID);
	return *iMessage;
	}

// ----------------------------------------------------------------------------
//  CMsgMailDocument::CreateNewL()
// ----------------------------------------------------------------------------
//
EXPORT_C TMsvId CMsgMailDocument::CreateNewL(TMsvId aServiceType, 
    TMsvId aTarget, TInt /*aTypeList*/, TUid aMtm)
    {
    // Check that we're not under critical level
    DiskSpaceBelowCriticalLevelL( 0 );
	CAknInputBlock::NewLC();
	// Set context to target
	TMsvEntry serviceTEntry;
	TMsvId serviceID;
	
	TInt error = Session().GetEntry(aServiceType, serviceID, serviceTEntry);
	// if fails use default service (SMTP)
	if (error == KErrNone)
		{
		aMtm = serviceTEntry.iMtm;
		}
	PrepareMtmL(aMtm);
	Mtm().SwitchCurrentEntryL(aTarget);
	Mtm().CreateMessageL(aServiceType);
	CMsvEntry& newEntry = Mtm().Entry();
	
	TMsvId newId = newEntry.EntryId();
	CleanupStack::PopAndDestroy(); // CAknInputBlock
	
	return newId;
    }

// ----------------------------------------------------------------------------
//  CMsgMailDocument::CreateNewEntryL()
// ----------------------------------------------------------------------------
//
TMsvId CMsgMailDocument::CreateNewEntryL( TMsvEmailEntry& aEntry )
	{
    CImHeader& header = HeaderL();
    CDesCArrayFlat* toArray = new(ELeave) CDesCArrayFlat(3);		// CSI: 47 # <insert a comment here>
    CleanupStack::PushL(toArray);
    CDesCArrayFlat* ccArray = new(ELeave) CDesCArrayFlat(3);		// CSI: 47 # <insert a comment here>
    CleanupStack::PushL(ccArray);

    const TInt toCount(header.ToRecipients().Count());
    TInt i(0);
    for (; i < toCount; i++)
        {
        toArray->AppendL(header.ToRecipients()[i]);
        }
    const TInt ccCount(header.CcRecipients().Count());
    for (i=0; i < ccCount; i++)
        {
        ccArray->AppendL(header.CcRecipients()[i]);
        }

    // we'll preserve all data from original message
    const TMsvId serviceId(aEntry.iServiceId);
    const TMsvId relatedId(aEntry.iRelatedId);
    const TUid type(aEntry.iType);
    const TUid mtm(aEntry.iMtm);
    const TInt32 mtmData1(aEntry.iMtmData1);
    const TInt32 mtmData2(aEntry.iMtmData2);
    const TTime time(aEntry.iDate);

    TMsvEmailTypeList msvEmailTypeList(KMsvEmailTypeListMessageInPreparation);
    TMsvId newMessageId( CreateNewL(serviceId, 
        DefaultMsgFolder(), msvEmailTypeList, mtm) );
           
    CMsvEntry* centry = CMsvEntry::NewL( Session(), 
        newMessageId, TMsvSelectionOrdering() );
    CleanupStack::PushL(centry);
    aEntry = centry->Entry();
    aEntry.iServiceId = serviceId;
    aEntry.iRelatedId = relatedId;
    aEntry.iType = type;
    aEntry.iMtm = mtm;
    aEntry.iMtmData1 = mtmData1;
    aEntry.iMtmData2 = mtmData2;
    aEntry.SetMHTMLEmail(EFalse); // make sure that old flag is not copied
    aEntry.SetBodyTextComplete(ETrue);
    aEntry.SetComplete(ETrue);
    aEntry.SetInPreparation(aEntry.InPreparation());    
    // set visible if not in preparation state
    aEntry.SetVisible(!aEntry.InPreparation());
    aEntry.iDate = time;
    centry->ChangeL(aEntry);                
    CleanupStack::PopAndDestroy(); // centry
    SetEntryL(newMessageId);

    aEntry = Entry();
    CImHeader& newHeader = HeaderL();
    
    for (i=0; i < toCount; i++)
        {
        newHeader.ToRecipients().AppendL((*toArray)[i]);	// CSI: 2 # No need to check because of the i goes from 0 to toCount.
        }
    for (i=0; i < ccCount; i++)
        {
        newHeader.CcRecipients().AppendL((*ccArray)[i]);	// CSI: 2 # No need to check because of the i goes from 0 to ccCount.
        }
    CleanupStack::PopAndDestroy(2);			// CSI: 47 # toArray, ccArray

	return newMessageId;
	}
	
// ----------------------------------------------------------------------------
//  CMsgMailDocument::SaveHeaderToStoreL()
// ----------------------------------------------------------------------------
//
void CMsgMailDocument::SaveHeaderToStoreL()
    {
    ASSERT( iMessageID == CurrentEntry().Entry().Id() );
    	
    // get store for the message
    CMsvStore* store = CurrentEntry().EditStoreL();
    CleanupStack::PushL(store);

    // set header and body encoding
    TImEmailTransformingInfo info;
    info.SetToDefault(ESendAsMimeEmail);
    TUint charset = SendOptions().SendingCharacterSet().iUid;
    info.SetHeaderAndBodyCharset( charset );
    
    info.StoreL(*store);
    // store message header
    iHeader->StoreL(*store);
    store->CommitL();
    CleanupStack::PopAndDestroy(); // store    
    }

// -----------------------------------------------------------------------------
//  CMsgMailDocument::IsSyncMLMail()
//	@deprecated
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMsgMailDocument::IsSyncMLMail()
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CMsgMailDocument::ResolveMimeType
// ----------------------------------------------------------------------------
EXPORT_C TDataType CMsgMailDocument::ResolveMimeType( RFile aFile )
	{
    LOG(">CMsgMailDocument::ResolveMimeType");              	
	TDataType dataType;
    if ( aFile.SubSessionHandle() )
        {
        TUid appUid( KNullUid );
        TInt error(0);
        
        if (!iApaSession.Handle())
        	{
        	error = iApaSession.Connect();
        	}
		if ( error == KErrNone )
			{
		    TDataRecognitionResult recognitionResult;
		    
		    error = iApaSession.RecognizeData(aFile, recognitionResult);
	    	if(recognitionResult.iConfidence != CApaDataRecognizerType::ENotRecognized &&
	    	    error == KErrNone)
	    	    {
	    	    dataType = recognitionResult.iDataType;
	    	    }
	    	else
	    	    {
	    	    LOG1("CMsgMailDocument::ResolveMimeTypeL error:%d or not ENotRecognized", 
				error);        
	    	    }
			}

        }
    LOG("<CMsgMailDocument::ResolveMimeType");
    return dataType;    
	}

// -----------------------------------------------------------------------------
//  CMsgMailDocument::SetReceivedIntL()
// -----------------------------------------------------------------------------
//
TInt CMsgMailDocument::SetReceivedIntL(				// CSI: 40 # We must return 
													// the integer value although this 
													// is a leaving method.
    const TUint aMessageType, const TInt aAmount )
	{
	// Check that the index has correct value
	if( aMessageType >= EIndexLast )
		{
	    return KErrOverflow;
	    }

	switch(aMessageType)
		{
		case EIndexAddHeaders:
			{
			iPrefs->SetAdditionalHeaders(aAmount);
			break;
			}
		default:
			{
			break;
			}
		}
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CMsgMailDocument::SetReceivedRealL()
// -----------------------------------------------------------------------------
//	
TInt CMsgMailDocument::SetReceivedRealL(				// CSI: 40 # We must return 
														// the integer value although this 
														// is a leaving method.
    const TUint aMessageType, const TReal /*aValue*/ )
	{
	// Check that the index has correct value
	if( aMessageType >= EIndexLast )
		{
	    return KErrOverflow;
	    }
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CMsgMailDocument::SetReceivedBinaryL()
// -----------------------------------------------------------------------------
//	
TInt CMsgMailDocument::SetReceivedBinaryL(				// CSI: 40 # We must return 
														// the integer value although this 
														// is a leaving method.
    const TUint aMessageType, const TDesC8 /*aValue*/ )    
	{
	// Check that the index has correct value
	if( aMessageType >= EIndexLast )
		{
	    return KErrOverflow;
	    }
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CMsgMailDocument::SetReceivedStringL()
// -----------------------------------------------------------------------------
//	
TInt CMsgMailDocument::SetReceivedStringL(				// CSI: 40 # We must return 
														// the integer value although this 
															// is a leaving method.
    const TUint aMessageType, const TDesC16 /*aValue*/)
	{
	// Check that the index has correct value
	if( aMessageType >= EIndexLast )
		{
	    return KErrOverflow;
	    }
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//  CMsgMailDocument::DiskSpaceBelowCriticalLevelL()
// -----------------------------------------------------------------------------
//	
TBool CMsgMailDocument::DiskSpaceBelowCriticalLevelL( TInt aBytesToWrite )
    {
    // Check that we're not under critical level
    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
        Session(), aBytesToWrite) )
        {
        User::Leave(KErrDiskFull);
        }
    return ETrue;        
    }
    
// -----------------------------------------------------------------------------
//  CMsgMailDocument::AddAttachmentNamesToMessageBodyL()
// -----------------------------------------------------------------------------
//	
void CMsgMailDocument::AddAttachmentNamesToMessageBodyL( const TDesC& aString )
    {
    if ( aString.Length() )
        {            
        CImEmailMessage* mailMessage = CImEmailMessage::NewLC( CurrentEntry() );
        const TMsvId messageId = CurrentEntry().Entry().Id();
       
        CParaFormatLayer* globalParaLayer = CParaFormatLayer::NewL();
        CleanupStack::PushL( globalParaLayer );
        CCharFormatLayer* globalCharLayer = CCharFormatLayer::NewL();
        CleanupStack::PushL( globalCharLayer );
        
        CRichText* bodyText = CRichText::NewL(
            globalParaLayer,
            globalCharLayer);            
        CleanupStack::PushL( bodyText );
        
        CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();                        
        mailMessage->GetBodyTextL(
                wait->iStatus,
                messageId,
                CImEmailMessage::EThisMessageOnly,
                *bodyText,
                *globalParaLayer,
                *globalCharLayer );
        
        wait->Start();
        User::LeaveIfError( wait->iStatus.Int() );
        
        CleanupStack::PopAndDestroy();  // wait  
        
        // Add the file name into the body
        bodyText->InsertL( bodyText->DocumentLength(), aString);
        
        wait = CMuiuOperationWait::NewLC();    
        mailMessage->StoreBodyTextL( messageId, *bodyText, wait->iStatus );
        wait->Start();
        User::LeaveIfError( wait->iStatus.Int() );
        // wait, globalParaLayer, globalCharLayer, bodyText
        CleanupStack::PopAndDestroy(5, mailMessage);			// CSI: 47 # See a comment above.
        }
    }

// -----------------------------------------------------------------------------
//  CMsgMailDocument::GetAttachmentNamesFromMessageL()
// -----------------------------------------------------------------------------
//    
HBufC* CMsgMailDocument::GetAttachmentNamesFromMessageL(
    CImEmailMessage* aMailMessage )
    {
    ASSERT( aMailMessage );
        
    const TMsvId messageId = CurrentEntry().Entry().Id();           
    aMailMessage->GetAttachmentsListL( 
        messageId,
        CImEmailMessage::EAllAttachments,
        CImEmailMessage::EThisMessageOnly );
    MMsvAttachmentManager& attachmentManager = 
        aMailMessage->AttachmentManager();
    
    HBufC* namesString = KNullDesC().AllocLC();        
    TInt count( attachmentManager.AttachmentCount() );    
    for ( TInt index(0); index < count; ++index )
        {
        CMsvAttachment* info = 
            attachmentManager.GetAttachmentInfoL( index );
        CleanupStack::PushL( info );

        // <<filename>>
        HBufC* filename = StringLoader::LoadLC(
            R_MAIL_REPLYATTA, info->AttachmentName() );
        
        namesString = namesString->ReAllocL( 
                namesString->Length() + 
                filename->Length() + 1 ); // +1 for linebreak
        
        TPtr ptr = namesString->Des();
        ptr.Append( CEditableText::ELineBreak );
        ptr.Append( *filename );
        
        CleanupStack::PopAndDestroy(2);				// CSI: 47 # info, filename
        
        // refresh namesString pointer on stack
        CleanupStack::Pop(); // namesString
        CleanupStack::PushL( namesString );
        }
        
    SetEntryWithoutNotificationL( messageId );
    CleanupStack::Pop(); // namesString
    return namesString;
    }

// -----------------------------------------------------------------------------
//  CMsgMailDocument::IsPlainHTMLMessageL()
// -----------------------------------------------------------------------------
//  
TBool CMsgMailDocument::IsPlainHTMLMessageL( 
    CImEmailMessage* aMailMessage )
    {
    ASSERT( aMailMessage );
    
    TBool retVal = EFalse;
        
    TMsvEmailEntry mailEntry( CurrentEntry().Entry() );  
    if ( mailEntry.MHTMLEmail() )
        {
        CParaFormatLayer* globalParaLayer = CParaFormatLayer::NewL();
        CleanupStack::PushL( globalParaLayer );
        CCharFormatLayer* globalCharLayer = CCharFormatLayer::NewL();
        CleanupStack::PushL( globalCharLayer );
        
        CRichText* bodyText = CRichText::NewL(
            globalParaLayer,
            globalCharLayer);            
        CleanupStack::PushL( bodyText );
        
        CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();                        
        aMailMessage->GetBodyTextL(
                wait->iStatus,
                mailEntry.Id(),
                CImEmailMessage::EThisMessageOnly,
                *bodyText,
                *globalParaLayer,
                *globalCharLayer );
  
        wait->Start();
        CleanupStack::PopAndDestroy( wait );
        
        
        
        if ( !bodyText->DocumentLength() )
            {
            retVal = ETrue;
            }
        else
            {
            retVal = EFalse;
            }
        
        // 3 globalParaLayer, globalCharLayer, bodyText     
        CleanupStack::PopAndDestroy( 3 );			// CSI: 47 # See a comment above.
        }
        
    return retVal;
    }
	
//  End of File
 
