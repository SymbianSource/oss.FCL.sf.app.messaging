/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: An EMN handler WAP Push Plugin.
*		
*
*/


#include <push/ccontenthandlerbase.h>
#include "EMNHandler.h"
#include "EMNLogging.h"

#include <ecom/implementationproxy.h>    // for ECOM

#include <xml/parser.h>

_LIT( KReserved, "Reserved" );
_LIT( KEMNWBXMLMediaType, "application/vnd.wap.emn+wbxml" );
_LIT( KEMNXMLMediaType, "text/vnd.wap.emn+xml" );
_LIT8( KDictionaryUri, "-//WAPFORUM//DTD EMN 1.0//EN~0" );
_LIT8( KWBXMLMimeType, "text/wbxml");
_LIT8( KXMLMimeType, "text/xml");

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CEMNHandler::CPushHandlerBase_Reserved1()
	{
	User::Panic(KReserved, KErrNotSupported);
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CEMNHandler::CPushHandlerBase_Reserved2()
	{
	User::Panic(KReserved, KErrNotSupported);
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
CEMNHandler::CEMNHandler() : CPushHandlerBase()
	{
	CActiveScheduler::Add( this );
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CEMNHandler::ConstructL()
	{
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
CEMNHandler* CEMNHandler::NewL()
	{
	CEMNHandler* self = new (ELeave) CEMNHandler();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
CEMNHandler::~CEMNHandler()
	{
    KEMNLOGGER_FN1("CEMNHandler::~CEMNHandler()");

	delete iMessage;
    delete iBody;
    
    KEMNLOGGER_FN2("CEMNHandler::~CEMNHandler()");
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CEMNHandler::IdleComplete()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	SetActive();
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CEMNHandler::Done( TInt aError )
	{
	if ( iAsyncHandling )
	    {
		SignalConfirmationStatus( aError );
	    }
	// Time to commit suicide
	iPluginKiller->KillPushPlugin();
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CEMNHandler::HandleMessageL( CPushMessage* aPushMsg, TRequestStatus& aStatus )
	{
    KEMNLOGGER_FN1("CEMNHandler::HandleMessageL() 1");

	iAsyncHandling = ETrue;
	SetConfirmationStatus( aStatus );
	HandleMessageL( aPushMsg );

    KEMNLOGGER_FN2("CEMNHandler::HandleMessageL() 1");
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CEMNHandler::HandleMessageL( CPushMessage* aPushMsg )
	{
    KEMNLOGGER_FN1("CEMNHandler::HandleMessageL() 2");
    
    if ( iMessage )
        {
        delete iMessage;
        iMessage = NULL;
        }
	iMessage = aPushMsg;
	iState = EParsing;
	IdleComplete();

    KEMNLOGGER_FN2("CEMNHandler::HandleMessageL() 2");
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CEMNHandler::CancelHandleMessage()
	{
	Done( KErrCancel );
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CEMNHandler::DoCancel()
	{
    KEMNLOGGER_FN1("CEMNHandler::DoCancel()");

	Done( KErrCancel );

    KEMNLOGGER_FN2("CEMNHandler::DoCancel()");
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CEMNHandler::RunL()
	{
	switch ( iState )
		{
	case EParsing :
		iState = EProcessing;
		ParsePushMsgL();
		break;
		
	case EProcessing:
		iState = EDone;
		ProcessPushMsgL();
		break;
		
	case EDone:
		Done( KErrNone );
		break;
		
	default:
	    iState = EDone; // EMN silently discarded if something went wrong.
		break;
		}
	}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CEMNHandler::ParsePushMsgL()
	{
    KEMNLOGGER_FN1("CEMNHandler::ParsePushMsgL()");

    TPtrC8 body;
    TPtrC contentType;
    CParser* parser = NULL;
    CEMNXMLContentHandler* handler = NULL;
    
    // Check content type and leave if not correct
    iMessage->GetContentType( contentType );
    if ( contentType.FindF( KEMNXMLMediaType ) == KErrNotFound &&
         contentType.FindF( KEMNWBXMLMediaType ) == KErrNotFound )
        {
        User::Leave( KErrNotSupported );
        }
        
    if ( iMessage->GetMessageBody( body ) )
        {
        User::LeaveIfNull( iBody = body.Alloc() );
        }
    
    KEMNLOGGER_WRITE_FORMAT("CEMNHandler::ParsePushMsgL(): contentType = %s ", contentType.Ptr() );
    
    if ( iBody && 
        contentType.FindF( KEMNXMLMediaType ) != KErrNotFound )
        {
        // Ascii
        KEMNLOGGER_WRITE("CEMNHandler::ParsePushMsgL(): Ascii");
        
        // Create a new content handler
        handler = CEMNXMLContentHandler::NewLC( iElement, ETrue );

        // Create a new XML parser and push it to cleanup stack
        parser = CParser::NewLC( KXMLMimeType, *handler );
        parser->ParseBeginL();
        parser->ParseL( iBody->Des() );
        parser->ParseEndL();

        CleanupStack::PopAndDestroy( 2, handler ); // parser // CSI: 12,47 # nothing wrong
        }
    else if ( iBody && 
        contentType.FindF( KEMNWBXMLMediaType ) != KErrNotFound )
        {
        // Binary
        KEMNLOGGER_WRITE("CEMNHandler::ParsePushMsgL(): Binary");
        // Create a new content handler
        handler = CEMNXMLContentHandler::NewLC( iElement, EFalse );

        // Create a new WBXML parser and push it to cleanup stack
        parser = CParser::NewLC( KWBXMLMimeType, *handler );
        // Set correct String Dictionary into use
        parser->AddPreloadedDictionaryL( KDictionaryUri );
        // Start the actual parse operation.
        parser->ParseBeginL();
        parser->ParseL( iBody->Des() );
        parser->ParseEndL();
            
        CleanupStack::PopAndDestroy( 2, handler ); // parser // CSI: 12,47 # nothing wrong
        }
    else
        {
        KEMNLOGGER_WRITE("CEMNHandler::ParsePushMsgL(): Body missing or unknown content type -> discarding");
        }
    
    if ( iElement.mailbox.Length() == 0 )
        {
        // No need to call AOEmailPlugin if there was no email address in EMN.
        iState = EDone;
        }

    IdleComplete();
    KEMNLOGGER_FN2("CEMNHandler::ParsePushMsgL()");
    }

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CEMNHandler::ProcessPushMsgL()
	{
    KEMNLOGGER_FN1("CEMNHandler::ProcessPushMsgL()");

    // Create a command that server should execute
    TAlwaysOnlineServerAPICommands command = EServerAPIEmailEMNReceived;
    
    // Create a package descriptor from iElement, in order to be able to send it to server as a TDes8&
    TPckg<TEMNElement> parameters( iElement );

    // Connect client and close the connection after the message is sent
    TInt err = iAOClient.Connect();
    if ( err == KErrNone )
        {
        iAOClient.RelayCommandL( command, parameters );
        iAOClient.Close();
        }
    else
        {
        KEMNLOGGER_WRITE_FORMAT("CEMNHandler::ProcessPushMsgL(): err = %d", err );
        }

    IdleComplete();

    KEMNLOGGER_FN2("CEMNHandler::ProcessPushMsgL()");
    }

//-----------------------------------------------------------------------------
// ECOM PLUGIN LOADING STUFF
//-----------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
	{
    IMPLEMENTATION_PROXY_ENTRY( 0x1027508C, CEMNHandler::NewL )
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
	{
	aTableCount = 
	    sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}
	
