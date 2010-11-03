/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Loads html content
*
*/



// INCLUDE FILES
#include 	"MailLog.h"
#include 	"MailUtils.h"
#include    "MailLoadStateHtml.h"
#include    "MailLoadStateInline.h"
#include 	"MailLoadStateMachine.h"
#include 	"MsgMailViewerHtmlConv.h"
#include 	<MailPlainView.rsg>
#include    <MMailAppUiInterface.h>
#include    <CMailMessage.h>
#include 	<StringLoader.h>
#include 	<charconv.h>

_LIT8( KStartTag, "<html>" );
_LIT8( KHtmlHeader1, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=");
_LIT8( KHtmlHeader2, "\">\n");
_LIT8( KCharsetTag, "CHARSET" );
_LIT16( KCDrive, "C:" );
const TInt KPathNameLength( 100 );

// ============================ MEMBER FUNCTIONS ===============================

// load html file
// -----------------------------------------------------------------------------
// CLoadHtmlFile::MessageLoadingL
// -----------------------------------------------------------------------------
//
void CLoadHtmlFile::MessageLoadingL(TInt aStatus, CMailMessage& aMessage)
	{
	if( aStatus == CMailMessage::EBodyTextReady )
		{
        TRAPD( err, HandleBodyTextReadyL( aMessage ) );

        if( err )
            {
            // Complete request and leave if error occured
            CompleteRequest( err );
            User::LeaveIfError( err );
            }
		}
	}

// -----------------------------------------------------------------------------
// CLoadHtmlFile::CLoadHtmlFile
// -----------------------------------------------------------------------------
//
CLoadHtmlFile::CLoadHtmlFile(
	MMailAppUiInterface* aAppUI, MStateMachine& aStateMachine ) :
	CActive( CActive::EPriorityStandard )
	{
	CActiveScheduler::Add( this );
	iAppUI = aAppUI;
	iStateMachine = &aStateMachine;
	}

// -----------------------------------------------------------------------------
// CLoadHtmlFile::~CLoadHtmlFile
// -----------------------------------------------------------------------------
//
CLoadHtmlFile::~CLoadHtmlFile()
	{
	Cancel();
	delete iConverter;
	}

// -----------------------------------------------------------------------------
// CLoadHtmlFile::NextStateL
// -----------------------------------------------------------------------------
//
MLoadState* CLoadHtmlFile::NextStateL()
	{
	return new( ELeave ) CLoadInlineImages(
		iAppUI, *iStateMachine );
	}

// -----------------------------------------------------------------------------
// CLoadHtmlFile::MakeRequestL
// -----------------------------------------------------------------------------
//
void CLoadHtmlFile::MakeRequestL(
	TRequestStatus& aStatus,
	CMailMessage& aMessage )
	{
	iLoadStatus = &aStatus;
	iMessage = &aMessage;
	aMessage.LoadHtmlContentL( *this );
	aStatus = KRequestPending;
	}

// -----------------------------------------------------------------------------
// CLoadHtmlFile::HandleBodyTextReadyL
// -----------------------------------------------------------------------------
//
void CLoadHtmlFile::HandleBodyTextReadyL( CMailMessage& aMessage )
    {
	RFile handle = aMessage.HtmlContent();
	CleanupClosePushL( handle );

	if ( handle.SubSessionHandle() == 0 )
		{
		// if html content can not be found try to show plain text
		// content
		CRichText* bodyText = aMessage.MessageBody();
		// Partial loaded or corrupted message?
		if ( aMessage.MessageEntry().PartialDownloaded() )
			{
			HBufC* text = StringLoader::LoadLC(
				R_QTN_MAIL_REST_OF_MAIL_NOT_FETCHED, CCoeEnv::Static());

	        bodyText->InsertL(0, *text);
	        CleanupStack::PopAndDestroy(); // text
			}
		else // Corrupted?
			{
			MailUtils::InformationNoteL(
				R_MAIL_VIEWER_HTML_ERROR_DLG_TEXT );
			}
        iAppUI->SetBodyTextL(*aMessage.MessageBody());
        CompleteRequest( KErrNone );
		}
	else
		{
		// Add html content as attachment before conversion.
		if ( !aMessage.MessageEntry().PartialDownloaded() )
			{
			AddHtmlFileAttachmentAndCharsetL( aMessage, handle );
			}
        ParseHtmlFileL( aMessage, handle );
		}
    CleanupStack::PopAndDestroy(); // handle
    }

// -----------------------------------------------------------------------------
// CLoadHtmlFile::ShowHtmlErrorNoteIfNeededL
// -----------------------------------------------------------------------------
//
void CLoadHtmlFile::ShowHtmlErrorNoteIfNeededL( TInt aResult )
	{
    if ( aResult == KErrNotFound )
        {
		MailUtils::InformationNoteL( R_MAIL_VIEWER_HTML_ERROR_DLG_TEXT );
        }
    else if ( aResult == KErrCorrupt )
        {
		MailUtils::InformationNoteL( R_MAIL_VIEWER_HTML_CORRUPTED_NOTE_TEXT );
        }
    else if ( aResult == KErrEof )
        {
        // Successfully parsed. Do nothing.
        }
    else
        {
        User::LeaveIfError( aResult );
        }
	}

// -----------------------------------------------------------------------------
// CLoadHtmlFile::AddHtmlFileAttachmentL
// -----------------------------------------------------------------------------
//
void CLoadHtmlFile::AddHtmlFileAttachmentL(
    CMailMessage& aMessage, RFile aFile )
    {
	CMsvAttachment* htmlFileInfo = CMsvAttachment::NewL(
		CMsvAttachment::EMsvFile );
	CleanupStack::PushL( htmlFileInfo );

	TInt htmlSize(0);
	aFile.Size( htmlSize );
	htmlFileInfo->SetSize( htmlSize );
	TFileName filename;
	User::LeaveIfError( aFile.Name( filename ) );
	htmlFileInfo->SetAttachmentNameL( filename );
	htmlFileInfo->SetComplete( ETrue );
	htmlFileInfo->SetId( aMessage.MhtmlPartId() );

    iAppUI->AddAttachmentL( *htmlFileInfo, EFalse );
    CleanupStack::PopAndDestroy( htmlFileInfo );
    }

// -----------------------------------------------------------------------------
// CLoadHtmlFile::ParseHtmlFileL
// -----------------------------------------------------------------------------
//
void CLoadHtmlFile::ParseHtmlFileL(CMailMessage& aMessage, RFile aFile )
    {
    LOG( "CLoadHtmlFile::ParseHtmlFileL" );
    LOGHANDLESIZE( aFile );

    // Get the charset of the html part from the message. If the message
    // thinks it knows the html charset then force the conversion of the html
    // part using the specified character set decoding.
    //
    // Otherwise, if the html message part character set is unknown, then
    // we'll let the conversion object try to identify the character set during
    // the parsing process.
    const TUint htmlCharSetId = aMessage.HtmlCharsetIdL();
    const TBool forceCharSet = ( htmlCharSetId != 0 );

    delete iConverter;
    iConverter = NULL;
    iConverter = CMsgMailViewerHtmlConv::NewL(
            aFile,
            htmlCharSetId,
            forceCharSet );

    iConverter->Convert( &iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CLoadHtmlFile::RunL
// -----------------------------------------------------------------------------
//
void CLoadHtmlFile::RunL()
    {
    LOG( "CLoadHtmlFile::RunL" );
    ShowHtmlErrorNoteIfNeededL( iStatus.Int() );

    CRichText* bodyText = iMessage->MessageBody();
    bodyText->Reset();
    bodyText->InsertL(0, *(iConverter->GetText()));

    // If the conversion process successfully identified the characterset
    // of the html part, then update the message with this new charset
    // decoding value.
    if ( iConverter->AutoParsedCharSetNameAndIdentifier() )
        {
        const TUint autoParsedCharSetId = iConverter->AutoParsedCharSetIdentifier();
        iMessage->OverrideHtmlMessageCharset( autoParsedCharSetId );
        }

    iStateMachine->SetIdArray( iConverter->FileIdArray() );

    if ( iMessage->MessageEntry().PartialDownloaded() )
        {
        HBufC* text = StringLoader::LoadLC(
            R_QTN_MAIL_REST_OF_MAIL_NOT_FETCHED, CCoeEnv::Static());

        bodyText->InsertL(bodyText->DocumentLength(), CEditableText::ELineBreak);

        bodyText->InsertL(bodyText->DocumentLength(), *text);

        CleanupStack::PopAndDestroy(); // text
        }

    iAppUI->SetBodyTextL( *(iMessage->MessageBody()) );

    // All done
    CompleteRequest( KErrNone );
    }

// -----------------------------------------------------------------------------
// CLoadHtmlFile::RunError
// -----------------------------------------------------------------------------
//
TInt CLoadHtmlFile::RunError( TInt aError )
    {
    LOG( "CLoadHtmlFile::DoCancel" );
    CompleteRequest( aError );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLoadHtmlFile::DoCancel
// -----------------------------------------------------------------------------
//
void CLoadHtmlFile::DoCancel()
    {
    LOG( "CLoadHtmlFile::DoCancel" );

    if( iConverter )
        {
        iConverter->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CLoadHtmlFile::CompleteRequest
// -----------------------------------------------------------------------------
//
void CLoadHtmlFile::CompleteRequest( TInt aError )
    {
    LOG( "CLoadHtmlFile::CompleteRequest" );
    User::RequestComplete( iLoadStatus, aError );
    iAppUI->ViewMessageComplete();
    }

// -----------------------------------------------------------------------------
// CLoadHtmlFile::AddCharsetToHtmlAttachment
// -----------------------------------------------------------------------------
//
void CLoadHtmlFile::AddHtmlFileAttachmentAndCharsetL( CMailMessage& aMessage, RFile& aHandle )
	{
	// We have to check if the html body contains information about
	// charset used. If it does, we use original file. Otherwise
	// we create a new html file which has charset
	// information added

	// Get charset Id and handle for the html body content
	TUint htmlCharsetId = aMessage.HtmlCharsetIdL();

	// Convert charset Id into string
	RFs& fs = CCoeEnv::Static()->FsSession();
	CCnvCharacterSetConverter* charConv = CCnvCharacterSetConverter::NewLC();
	HBufC8* charsetBuf = charConv->
		ConvertCharacterSetIdentifierToStandardNameL(
				htmlCharsetId, fs);
	CleanupStack::PopAndDestroy( charConv ); // charConv
	CleanupStack::PushL( charsetBuf );
	TPtr8 charsetPtr = charsetBuf->Des();

	// Calculate buffer size for charsetRowBuf and also for
	// htmlContentBuf to avoid reallocation of memory
	TInt bufferSize( 0 );
	bufferSize += KHtmlHeader1().Length();
	bufferSize += charsetBuf->Length();
	bufferSize += KHtmlHeader2().Length();

	// Read html content from the html file
	TInt fileSize(0);
	aHandle.Size( fileSize );
	HBufC8* htmlContentBuf = HBufC8::NewLC( fileSize+bufferSize );
	TPtr8 htmlContentPtr = htmlContentBuf->Des();
	aHandle.Read( htmlContentPtr );
	TInt i( 0 );
	aHandle.Seek( ESeekStart, i );	// rewind the file

	// Try to find strings CHARSET and <html>
	// If the string is not found append information containing charset
	TInt charsetFoundAt( htmlContentBuf->Find( KCharsetTag ) );
	TInt htmlTagFoundAt( htmlContentBuf->Find( KStartTag ) );

	// If CHARSET is defined in the email but it is not found from the
	// html body part, we have to add it ourselves.
	// Else use the original html file
	if( charsetFoundAt <= 0
		&& htmlTagFoundAt >= 0 )
		{
		// Create a new hmtl line containing character set information
		// and append it.
		// Create new buffer for storing charset html info row and
		// insert it to the buffer containing html body
		HBufC8* charsetRowBuf = HBufC8::NewLC( bufferSize );
		TPtr8 charsetRowPtr = charsetRowBuf->Des();
		charsetRowPtr.Append( KHtmlHeader1 );
		charsetRowPtr.Append( charsetPtr );
		charsetRowPtr.Append( KHtmlHeader2 );
		htmlContentPtr.Insert( htmlTagFoundAt, charsetRowPtr );
		CleanupStack::PopAndDestroy( charsetRowBuf );

		// create mailviewer folder in c:\private
		HBufC* pathNameBuf = HBufC::NewLC( KPathNameLength );
		TPtr pathNamePtr = pathNameBuf->Des();
		fs.PrivatePath( pathNamePtr );
		pathNamePtr.Insert( 0, KCDrive );

		TInt makeDir = fs.MkDir( pathNamePtr );
		// It´s ok if directory already exists. If other errors,
		// use original html file
		if( makeDir != KErrAlreadyExists && makeDir < 0 )
			{
			AddHtmlFileAttachmentL( aMessage, aHandle );
			}
		else	// continue creating new html attachment
			{
			// create new html attachment
			RFile newHtmlFile;
			HBufC* newHtmlFileNameBuf = HBufC::NewLC( KPathNameLength );
			TPtr newHtmlFileNamePtr = newHtmlFileNameBuf->Des();
			aHandle.Name( newHtmlFileNamePtr );
			pathNamePtr.Append( newHtmlFileNamePtr );
			CleanupStack::PopAndDestroy( newHtmlFileNameBuf );

			TInt openResult = newHtmlFile.Replace( fs, pathNamePtr, EFileWrite );
			CleanupClosePushL( newHtmlFile );
			if( openResult >= KErrNone )
				{
				TInt writeResult = newHtmlFile.Write( 0, htmlContentPtr );
				if( writeResult >= KErrNone )
					{
					AddHtmlFileAttachmentL( aMessage, newHtmlFile );
					}
				else	// else use original html file
					{
					AddHtmlFileAttachmentL( aMessage, aHandle );
					}
				}
			CleanupStack::PopAndDestroy( &newHtmlFile );
			}
		CleanupStack::PopAndDestroy( pathNameBuf );
		}
	else	// charset found, use original html file
		{
		AddHtmlFileAttachmentL( aMessage, aHandle );
		}
	CleanupStack::PopAndDestroy( 2, charsetBuf );	// htmlContentBuf
	}


//  End of File
