/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mail message class
*
*/



// INCLUDE FILES
#include    "CMailMessage.h"
#include    <MMailMessageView.h>
#include    <MMessageLoadObserver.h>
#include    "MailLog.h"
#include    <miuthdr.h> // CImHeader
#include    <miutmsg.h>
#include    <miutconv.h>
#include    <charconv.h>
#include    <featmgr.h>
#include    <MuiuOperationWait.h>
#include 	<mmsvattachmentmanager.h>
#include    <ErrorUI.h>

// MODULE DATA STRUCTURES
enum TLoadState
    {
    ELoadMessageBody = 1,
    EHtmlNotFound,
    ELoadAttachments,
    ELoadAttachedMessages,
    ELoadLinkedHtmlContent
    };

// Constants
enum MessageFlags
    {
    EFlagAttachment = KBit0,
    EFlagAttachedMessages = KBit1,
    EFlagStopAfterLoad = KBit2,
    EFlagHtmlError = KBit3,
    EFlagHtmlMessageCharSetOverridden = KBit4
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMailMessage::CMailMessage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMailMessage::CMailMessage()
: CActive(0), iFlags(0)
    {
    }

// -----------------------------------------------------------------------------
// CMailMessage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMailMessage::ConstructL(CMsvEntry& aEntry)
    {
    CActiveScheduler::Add(this);

    iHeader = CImHeader::NewLC();
    CleanupStack::Pop();
    iEntry = aEntry.Session().GetEntryL( aEntry.EntryId() );

    iMessage = CImEmailMessage::NewL(*iEntry);
    iMessageID = iEntry->EntryId();
    if (iEntry->HasStoreL())
        {
        CMsvStore* msvStore = iEntry->ReadStoreL();
        CleanupStack::PushL(msvStore);
        TRAP_IGNORE(iHeader->RestoreL(*msvStore));
        CleanupStack::PopAndDestroy(); // store
        }

    iGlobalParaLayer = CParaFormatLayer::NewL();
    iGlobalCharLayer = CCharFormatLayer::NewL();

    iBodyText = CRichText::NewL(
        iGlobalParaLayer,
        iGlobalCharLayer);

	iLinkedHtmlItems = new( ELeave ) RPointerArray<
		CLinkedHtmlItem>(5);		// CSI: 47 # approx granularity 5

    iAsyncWait = CMuiuOperationWait::NewLC(); //only NewLC variant exists
    CleanupStack::Pop( iAsyncWait );
    }

// -----------------------------------------------------------------------------
// CMailMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMailMessage* CMailMessage::NewL(CMsvEntry& aEntry)
    {
    CMailMessage* self = new( ELeave ) CMailMessage();
    CleanupStack::PushL( self );
    self->ConstructL(aEntry);

    CleanupStack::Pop();
    return self;
    }


// Destructor
CMailMessage::~CMailMessage()
    {
    Cancel();
    delete iAsyncWait; // gets cancelled if running
    delete iHeader;
    delete iMessage;
    delete iBodyText;
    delete iGlobalParaLayer;
    delete iGlobalCharLayer;
    delete iURI;
    delete iEntry;
    if ( iLinkedHtmlItems )
        {
        iLinkedHtmlItems->ResetAndDestroy();
        delete iLinkedHtmlItems;
        }

   	iWaitArray.ResetAndDestroy();
    }


EXPORT_C void CMailMessage::LoadMessageL(
    MMessageLoadObserver& aMessageObserver)
    {
    iState = ELoadMessageBody;
    iMessageObserver = &aMessageObserver;

    // Header is already loaded
    NotifyObserverL(EHeaderReady);

    if( !MessageEntry().MHTMLEmail() )
        {
        LoadBodyTextL();
        }
    else
        {
   		LoadAttachmentsL();
        }
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMailMessage::LoadLinkedHtmlContentL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMailMessage::LoadLinkedHtmlContentL(
	const TDesC& aBase, const TDesC& aURI)
	{
	LoadLinkedHTMLContentL(aBase, aURI);
	}

// -----------------------------------------------------------------------------
// CMailMessage::HtmlCharsetIdL
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CMailMessage::HtmlCharsetIdL()
    {
    TUint charset = 0;
    //
    if (iMhtmlPartId != 0)
        {
        if ( iFlags & EFlagHtmlMessageCharSetOverridden )
            {
            charset = iCharacterSetId;
            }
        else
            {
            CMsvEntry* htmlEntry = iEntry->Session().GetEntryL( iMhtmlPartId );
            CleanupStack::PushL( htmlEntry );
        	charset = DoGetMimeCharsetIdL( *htmlEntry );
            CleanupStack::PopAndDestroy( htmlEntry );
            }
        }

	LOG1( "CMailMessage::HtmlCharsetIdL %08x", charset );
    return charset;
    }

// -----------------------------------------------------------------------------
// CMailMessage::CharsetIdL
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CMailMessage::CharsetIdL()
    {
    TUint characterSetId(0);
    TBool override;
    iMessage->GetCharacterSetL( iMessageID, characterSetId, override);
	LOG1( "CMailMessage::CharsetIdL from CImEmailMessage %08x",
	    	characterSetId );
	if ( characterSetId != 0 )
	    {
	    return characterSetId;
	    }

    if( iEntry->Entry().Id() != iMessageID )
    	iEntry->SetEntryL(iMessageID);

   	return DoGetMimeCharsetIdL( *iEntry );
    }

// -----------------------------------------------------------------------------
// CMailMessage::ConvertToCharsetL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMailMessage::ConvertToCharsetL(TUint aCharsetId)
    {
    iMessage->SetCharacterSetL( iMessageID, aCharsetId );
    }


// -----------------------------------------------------------------------------
// CMailMessage::AttachmentManager
// -----------------------------------------------------------------------------
//
EXPORT_C MMsvAttachmentManager& CMailMessage::AttachmentManager() const
    {
    ASSERT( iState == ELoadAttachments );
	LOG2( "CMailMessage::AttachmentManager %08x - %08x",
	    iMessageID,
	    iEntry->EntryId() );
    // context still in message ?
    if( iEntry->Entry().Id() != iMessageID )
    	TRAP_IGNORE( iEntry->SetEntryL(iMessageID) );
    return iMessage->AttachmentManager();
    }

// -----------------------------------------------------------------------------
// CMailMessage::LoadAttachmentsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMailMessage::LoadAttachmentsL(MMessageLoadObserver& aObserver)
	{
	iMessageObserver = &aObserver;
	Cancel();
	LoadAttachmentsL();
	iFlags |= EFlagStopAfterLoad;
	SetActive();
	}

// -----------------------------------------------------------------------------
// CMailMessage::LoadAttachedMessagesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMailMessage::LoadAttachedMessagesL(
	MMessageLoadObserver& aObserver)
	{
	iMessageObserver = &aObserver;
	Cancel();
	LoadAttachedMessagesL();
	iFlags |= EFlagStopAfterLoad;
	SetActive();
	}

// -----------------------------------------------------------------------------
// CMailMessage::LoadHtmlContentL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMailMessage::LoadHtmlContentL(MMessageLoadObserver& aObserver)
	{
	iMessageObserver = &aObserver;
	Cancel();
	LoadHTMLContentL();
	iFlags |= EFlagStopAfterLoad;
	SetActive();
	}

// -----------------------------------------------------------------------------
// CMailMessage::LoadLinkedHtmlContentL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMailMessage::LoadLinkedHtmlContentL(
        	const TDesC& aBase,
        	const TDesC& aURI,
        	MMessageLoadObserver& aObserver )
	{
	iMessageObserver = &aObserver;
	Cancel();
	delete iURI;
	iURI = NULL;
	iURI = aURI.AllocL();

	LoadLinkedHTMLContentL( aBase, aURI );
	iFlags |= EFlagStopAfterLoad;
	SetActive();
	}

// -----------------------------------------------------------------------------
// CMailMessage::GetFileHandleL
// -----------------------------------------------------------------------------
//
EXPORT_C RFile CMailMessage::GetFileHandleL( TMsvAttachmentId aId )
    {
    LOG( ">CMailMessage::GetFileHandleL" );
    Cancel();
    // Force message cancel for special cases(lost network coverage)
    if ( iMessage )
    	{
    	iMessage->Cancel();
    	}

    // create an array for active scheduler waits if it does not exist
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC(); //only NewLC variant exists
    // append the wait to the array
    iWaitArray.AppendL( wait );
    CleanupStack::Pop( wait );

    RFile handle;

    if ( aId == iMhtmlPartId )
        {
        // html body part
        iMessage->FindFirstHTMLPageFileHandleL( iMessageID,
                                                wait->iStatus );
        wait->Start();
        User::LeaveIfError( wait->iStatus.Int() );

        User::LeaveIfError(
            iMessage->GetUniversalResourceIdentifierFileHandle(
            iMhtmlPartId, handle) );

        }
    else if ( IsLinkedItemL( handle, aId) )
        {
        // linked html item
        }
    else
        {
        // normal attachment

        //Async version of GetAttachmentsListL used here,
        //because it is quite slow operation and when there is
        //many attachments to be loaded Ui jams if synchrounous
        //method is called.

        iMessage->GetAttachmentsListL(
            wait->iStatus,
            iMessageID,
            CImEmailMessage::EAllAttachments,
            CImEmailMessage::EThisMessageAndEmbeddedMessages );

        LOG( "CMailMessage::GetFileHandleL: Starting wait" );
        wait->Start();
        LOG( "CMailMessage::GetFileHandleL: Wait finished" );

        User::LeaveIfError( wait->iStatus.Int() );

        LOG( "CMailMessage::GetFileHandleL: Status ok" );
        MMsvAttachmentManager& manager = iMessage->AttachmentManager();
        handle = manager.GetAttachmentFileL( aId );
        }

    // clean up the wait array for any non-active waits
    CleanupWaitArray();

    __ASSERT_DEBUG( handle.SubSessionHandle() != 0, User::Invariant() );

    LOG( "<CMailMessage::GetFileHandleL" );
    return handle;
    }

// -----------------------------------------------------------------------------
// CMailMessage::CleanupWaitArray
// -----------------------------------------------------------------------------
//
void CMailMessage::CleanupWaitArray()
	{
	TInt count( iWaitArray.Count() );
    // cleanup the whole array
    for( TInt i = count - 1; i >= 0; i-- )
    	{
    	CMuiuOperationWait* wait = iWaitArray[i];
    	if( !wait->IsActive() )
    		{
    		iWaitArray.Remove( i );
    		delete wait;
    		}
    	}
	}

// -----------------------------------------------------------------------------
// CMailMessage::OverrideHtmlMessageCharset
// -----------------------------------------------------------------------------
//
EXPORT_C void CMailMessage::OverrideHtmlMessageCharset( TUint aCharSet )
    {
    iCharacterSetId = aCharSet;

    if ( aCharSet )
        {
        iFlags |= EFlagHtmlMessageCharSetOverridden;
        }
    else
        {
        iFlags &= ~EFlagHtmlMessageCharSetOverridden;
        }
    }
// -----------------------------------------------------------------------------
// CMailMessage::DoCancel
// -----------------------------------------------------------------------------
//
void CMailMessage::DoCancel()
    {
    LOG1("DoCancel @ iState:%d", iState);
    if ( iMessage )
    	{
    	iMessage->Cancel();
    	}
    }

// -----------------------------------------------------------------------------
// CMailMessage::RunL
// -----------------------------------------------------------------------------
//
void CMailMessage::RunL()
    {
    switch(iState)
        {
        case ELoadMessageBody:
            {
            LOG("CMailMessage::RunL @ ELoadMessageBody");
            User::LeaveIfError( iStatus.Int() );
            // Body content loaded to iBodyText
            // if HTML mail get file handle to content (iBodyText is empty)
            if( MessageEntry().MHTMLEmail() && !(iFlags & EFlagHtmlError) )
                {
            	TInt error(KErrNone);
                // html file handle
                error = iMessage->
                    GetUniversalResourceIdentifierFileHandle(
                    iMhtmlPartId, iHtmlFile);

	            if (error == KErrNone)
	               	{
	                iFlags |= EFlagAttachment;
	               	}
	            else
	            	{
	               	// Can't load HTML, so try to load plain text body
                    LOG1("CMailMessage::RunL @ Can't load HTML: %d",
            	        error);
	               	iState = EHtmlNotFound;
	               	iFlags |= EFlagHtmlError;
	            	break;
	            	}
                }
            NotifyObserverL(EBodyTextReady);
            break;
            }
        case ELoadAttachments:
            {
            LOG("CMailMessage::RunL @ ELoadAttachments");
            // Attachment files loaded
            User::LeaveIfError( iStatus.Int() );
            NotifyObserverL(EAttachmentsReady);
            LOG("CMailMessage::RunL, NotifyObserverL returned");
            break;
            }
        case ELoadAttachedMessages:
            {
            // Attached files loaded
            User::LeaveIfError( iStatus.Int() );
            const CMsvEntrySelection& sel = iMessage->Selection();
            TInt attachedCount(sel.Count());
            LOG1("CMailMessage::RunL @ ELoadAttachedMessages %d",
            	attachedCount);
            if (attachedCount)
                {
                iFlags |= EFlagAttachedMessages;
                }
			NotifyObserverL(EAttachedMessagesReady);
            break;
            }
        case ELoadLinkedHtmlContent:
            {
            LOG("CMailMessage::RunL @ ELoadLinkedHtmlContent");
            // linked file loaded
            TInt error = iMessage->
                    GetUniversalResourceIdentifierFileHandle(
                    iLinkedfileId, iLinkedHtmlFile);
            if ( error == KErrNone )
                {
                CLinkedHtmlItem* htmlItem = new(ELeave) CLinkedHtmlItem(
                    *iURI, iLinkedfileId );
                CleanupStack::PushL( htmlItem );
                iLinkedHtmlItems->AppendL( htmlItem );
                CleanupStack::Pop( htmlItem );
                }
            else
                {
                // ignore errors. If fails linked item is not added to
                // attachment view.
                LOG1("CMailMessage::RunL @ ELoadLinkedHtmlContent error:%d",
                     error);
                iLinkedHtmlFile = RFile(); // return empty handle
                iEntry->SetEntryL( iMessageID );
                }
			NotifyObserverL(ELinkedFileReady);
            break;
            }
        default:
        	// all loading done. Do nothing
            break;
        }
    DoNextStateL();
    }

// -----------------------------------------------------------------------------
// CMailMessage::RunError
// -----------------------------------------------------------------------------
//
TInt CMailMessage::RunError(TInt aError)
    {
    LOG2("CMailMessage::RunError @ iState: %d, aError:%d", iState, aError);
    // ActiveSheduler Panics if this returns != KErrNone
    Cancel();
    //We have to return KLeaveExit(appui's exit code), otherwise our appui is not deleted
    if(aError == KLeaveExit)
        {
        return KLeaveExit;
        }
    TRAP_IGNORE( ShowErrorNoteL( aError ) );

    // exit from viewer app if OOM
    if( aError == KErrNoMemory )
        {
        return KLeaveExit;
        }

    // Try to continue loading
    TRAP_IGNORE( DoNextStateL() );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMailMessage::DoNextStateL
// -----------------------------------------------------------------------------
//
void CMailMessage::DoNextStateL()
    {
	// Step to next state
    if ( (!(iFlags & EFlagStopAfterLoad) ||
        (iState == EHtmlNotFound))
        && NextStateL() )
        {
        SetActive();
        }
    else
        {
		// message loaded
        NotifyObserverL(ELoadEnd);
        }
    }

// -----------------------------------------------------------------------------
// CMailMessage::DoGetMimeCharsetIdL
// -----------------------------------------------------------------------------
//
TUint CMailMessage::DoGetMimeCharsetIdL( CMsvEntry& aEntry )
    {
    //Try to read the character set from two places:
    // 1) Main entry's MIME headers
    // 2) Child entrys' MIME headers
    TUint characterSetId(0);

    // If the message store cannot be read, abort here
    CMsvStore* store = NULL;
    TRAPD( err, store = aEntry.ReadStoreL() );
    if( err != KErrNone )
        {
        return 0;
        }
    CleanupStack::PushL(store);

    //------------------------------------------------
    // Option 1. Read the MIME header from main entry
    //------------------------------------------------
    if (store->IsPresentL(KUidMsgFileMimeHeader))
        {
        //Main entry's mime header
    	CImMimeHeader* mime = CImMimeHeader::NewLC();
	    mime->RestoreL(*store);
	    characterSetId = DetermineCharactersetIdFromMimeHeaderL( aEntry, mime );
        CleanupStack::PopAndDestroy(mime);
        }

    //Store is no longer needed
 	CleanupStack::PopAndDestroy(store);

 	//If character set is valid stop now
 	if( characterSetId )
	 	{
	 	return characterSetId;
	 	}

     //--------------------------------------------------
    // Option 2. Read the MIME headers from child entrys'
    //---------------------------------------------------
    CMsvEntrySelection* entrySelection = iEntry->ChildrenL();
    CleanupStack::PushL(entrySelection);

    //Go through the children
	TInt count = entrySelection->Count();
	for (TInt counter = 0; counter < count; counter++)
		{
		//Get the child entry
		CMsvEntry* child = iEntry->ChildEntryL((*entrySelection)[counter]);
		CleanupStack::PushL(child);

		//Read child entry store
		CMsvStore* childStore = NULL;
		TRAPD( err, childStore = child->ReadStoreL() );

		//If error occurs in store read try the next children
		if( err )
			{
			CleanupStack::PopAndDestroy(child);
			continue;
			}

		//Read the childs MIME header if they are available
		CleanupStack::PushL(childStore);
		if (childStore->IsPresentL(KUidMsgFileMimeHeader))
			{
			CImMimeHeader* mime = CImMimeHeader::NewLC();
			mime->RestoreL( *childStore );
			characterSetId = DetermineCharactersetIdFromMimeHeaderL( *child, mime );
			CleanupStack::PopAndDestroy(mime);
			}
		CleanupStack::PopAndDestroy(childStore);
		CleanupStack::PopAndDestroy(child);	//This is brutal

		//If we got the characterSetId set from this child we can stop
		if( characterSetId )
			{
			break;
			}
		}
	CleanupStack::PopAndDestroy(); // entrySelection

    return characterSetId;
    }

// -----------------------------------------------------------------------------
// CMailMessage::DetermineCharactersetIdFromMimeHeader
// -----------------------------------------------------------------------------
//
TUint CMailMessage::DetermineCharactersetIdFromMimeHeaderL( CMsvEntry& aEntry,
															const CImMimeHeader* aMime )
	{
	TUint characterSetFromMimeHeader(0);
	TUint characterSetFromContentType(0);

	//Character can sometimes be read from MimeCharset
	characterSetFromMimeHeader = aMime->MimeCharset();

	//Determine character set from ContentTypeParams
	const CDesC8Array& array = aMime->ContentTypeParams();
    CCnvCharacterSetConverter* charconv =  CCnvCharacterSetConverter::NewLC();
    RFs& session = aEntry.Session().FileSession();

    for(TInt i = 0; i < array.Count() && characterSetFromContentType == 0; i++)
        {
        characterSetFromContentType = charconv->
        	ConvertStandardNameOfCharacterSetToIdentifierL( array[i], session);
        }
    CleanupStack::PopAndDestroy(); //charconv

    //Determine whitch one we use:
    //characterSetFromMimeHeader or characterSetFromContentType
    if( characterSetFromMimeHeader )
    	return characterSetFromMimeHeader;
    else if ( characterSetFromContentType )
    	return characterSetFromContentType;
    else
    	return 0;
	}

// -----------------------------------------------------------------------------
// CMailMessage::NextStateL
// -----------------------------------------------------------------------------
//
TBool CMailMessage::NextStateL()
    {
    TBool retValue(ETrue);
    switch(iState)
        {
        case ELoadMessageBody:
            // load attachment next
            LoadAttachmentsL();
            break;
        case EHtmlNotFound:
        	// Html content not found, so try plain content
            LoadBodyTextL();
            break;
        case ELoadAttachments:
			// FALLTROUGH
			// Load Attached messages only if clien requests them.
        default:
            // "ELoadAttachedMessages" or "ELoadLinkedHtmlContent"
            // or unknown state, so we stop
            retValue = EFalse;
            break;
        }
    return retValue;
    }

// -----------------------------------------------------------------------------
// CMailMessage::LoadBodyTextL
// -----------------------------------------------------------------------------
//
void CMailMessage::LoadBodyTextL()
    {
    iState = ELoadMessageBody;


    // delete the body text to avoid
    // appending the same text in different
    // character set at the end of the old text
    delete iBodyText;
    iBodyText = NULL;
    iBodyText = CRichText::NewL(
        iGlobalParaLayer,
        iGlobalCharLayer);

    iMessage->GetBodyTextL(iStatus,
        iMessageID,
        CImEmailMessage::EThisMessageOnly,
        *iBodyText,
        *iGlobalParaLayer,
        *iGlobalCharLayer);

    }

// -----------------------------------------------------------------------------
// CMailMessage::LoadHTMLContentL
// -----------------------------------------------------------------------------
//
void CMailMessage::LoadHTMLContentL()
    {
    iState = ELoadMessageBody;
    iMessage->FindFirstHTMLPageFileHandleL(iMessageID, iStatus);
    }

// -----------------------------------------------------------------------------
// CMailMessage::LoadLinkedHTMLContentL
// -----------------------------------------------------------------------------
//
void CMailMessage::LoadLinkedHTMLContentL(const TDesC& aBase, const TDesC& aURI)
    {
    iState = ELoadLinkedHtmlContent;

    iMessage->FindUniversalResourceIdentifierFileHandleL(
    	iMhtmlPartId,
    	aBase,
    	aURI,
    	iStatus);
    }

// -----------------------------------------------------------------------------
// CMailMessage::LoadAttachmentsL
// -----------------------------------------------------------------------------
//
void CMailMessage::LoadAttachmentsL()
    {
    iState = ELoadAttachments;
    iMessage->GetAttachmentsListL(
        iStatus, iMessageID,
        CImEmailMessage::EAllAttachments,
        CImEmailMessage::EThisMessageAndEmbeddedMessages);
    }

// -----------------------------------------------------------------------------
// CMailMessage::LoadAttachedMessagesL
// -----------------------------------------------------------------------------
//
void CMailMessage::LoadAttachedMessagesL()
    {
    iState = ELoadAttachedMessages;
    iMessage->GetMessageDigestEntriesL(iStatus, iMessageID);
    }

// ----------------------------------------------------------------------------
//  CMailMessage::NotifyObserverL()
// ----------------------------------------------------------------------------
//
void CMailMessage::NotifyObserverL(TInt aState)
    {
    iMessageObserver->MessageLoadingL(aState, *this);
    }

// ----------------------------------------------------------------------------
//  CMailMessage::IsLinkedItemL()
// ----------------------------------------------------------------------------
//
TBool CMailMessage::IsLinkedItemL( RFile& aHandle, TMsvAttachmentId aId )
	{
	TBool isLinkedItem(EFalse);
    const TInt linkedItems = iLinkedHtmlItems->Count();
    if ( linkedItems )
        {
        // linked html item?
        for( TInt index(0); index<linkedItems; ++index )
            {
            CLinkedHtmlItem* item = (*iLinkedHtmlItems)[index];
            if( item->iLinkedItemId == aId )
                {
                ASSERT( !iAsyncWait->IsActive() ); // nested calls illegal
                iMessage->FindUniversalResourceIdentifierFileHandleL(
                	iMhtmlPartId,
                	KNullDesC(),
                	*item->iUriAddress,
                	iAsyncWait->iStatus );
                iAsyncWait->Start();
                User::LeaveIfError( iAsyncWait->iStatus.Int() );

                User::LeaveIfError(
                    iMessage->GetUniversalResourceIdentifierFileHandle(
                    iLinkedfileId, aHandle) );
                isLinkedItem = ETrue;
                }
            }
        }
    return isLinkedItem;
	}

// ----------------------------------------------------------------------------
//  CMailMessage::ShowErrorNoteL()
// ----------------------------------------------------------------------------
//
void CMailMessage::ShowErrorNoteL( TInt aError )
    {
    CErrorUI* errorUI = CErrorUI::NewLC();
    errorUI->ShowGlobalErrorNoteL( aError ); // ignore return value
    CleanupStack::PopAndDestroy(); // errorUI
    }

// End Of File
