/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*      Implementation of Scheme handler interface implementation 
*      for sms:// scheme
*
*/



// INCLUDE FILES

#include "SmsHandler.h"
#include "SchemeDefs.h"
#include "SchemeDispLogger.h"
#include <ecom.h>		// For REComSession
#include <eikenv.h>
#include <DocumentHandler.h>
#include <apgcli.h>
#include <apparc.h>
#include <eikdoc.h>
#include <eikproc.h>
#include <f32file.h>
#include <sendui.h> // CSendUi
#include <SenduiMtmUids.h> 
#include <txtrich.h>
#include <CMessageData.h>


// ================= CONSTANTS =======================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSmsHandler::NewL()
// ---------------------------------------------------------
//
CSmsHandler* CSmsHandler::NewL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CSmsHandler::NewL()" );

	CSmsHandler* self = new( ELeave ) CSmsHandler();
	CleanupStack::PushL( self );
	self->ConstructL( aUrl );
	CleanupStack::Pop( self );

	CLOG_LEAVEFN( "CSmsHandler::NewL()" );

	return self;
	}

// ---------------------------------------------------------
// CSmsHandler::~CSmsHandler()
// ---------------------------------------------------------
//
CSmsHandler::~CSmsHandler()
	{
	CLOG_ENTERFN( "CSmsHandler::~CSmsHandler()" );
    delete iSendUi;

	CLOG_LEAVEFN( "CSmsHandler::~CSmsHandler()" );
	}

// ---------------------------------------------------------
// CSmsHandler::CSmsHandler()
// ---------------------------------------------------------
//
CSmsHandler::CSmsHandler() : CBaseHandler()
	{
	// Deliberately do nothing here : 
	// See ConstructL() for initialisation completion.
	}

// ---------------------------------------------------------
// CSmsHandler::ConstructL()
// ---------------------------------------------------------
//
void CSmsHandler::ConstructL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CSmsHandler::ConstructL()" );

	BaseConstructL( aUrl );
	
	CLOG_LEAVEFN( "CSmsHandler::ConstructL()" );
	}

// ---------------------------------------------------------
// CSmsHandler::HandleUrlEmbeddedL()
// ---------------------------------------------------------
//
void CSmsHandler::HandleUrlEmbeddedL()
	{
	CLOG_ENTERFN( "CSmsHandler::HandleUrlEmbeddedL()" );
    HandleUrlL( ETrue );    
	CLOG_LEAVEFN( "CSmsHandler::HandleUrlEmbeddedL()" );
	}

// ---------------------------------------------------------
// CSmsHandler::HandleUrlStandaloneL()
// ---------------------------------------------------------
//
void CSmsHandler::HandleUrlStandaloneL()
	{
	CLOG_ENTERFN( "CSmsHandler::HandleUrlStandaloneL()" );
    LaunchSchemeAppWithCommandLineL();   
	CLOG_LEAVEFN( "CSmsHandler::HandleUrlStandaloneL()" );
	}
    
// ---------------------------------------------------------
// CSmsHandler::HandleUrlL()
// ---------------------------------------------------------
//
void CSmsHandler::HandleUrlL(TBool aLaunchEmbedded)
    {
    CMessageData* messageData = CMessageData::NewLC();

    // Parse url
	TPtrC recipient = GetField( KSms );
    // To is supported because of localapp scheme
    TPtrC to = GetField( KTo );
	TPtrC msgBody = GetField( KBody );

    // Create richtext format for body
    CParaFormatLayer* paraFormat = CParaFormatLayer::NewL();
    CleanupStack::PushL( paraFormat );

    CCharFormatLayer* charFormat = CCharFormatLayer::NewL(); 
    CleanupStack::PushL( charFormat );

    CRichText* body = CRichText::NewL( paraFormat, charFormat );
    CleanupStack::PushL( body );
    if( msgBody.Length() )
	    {
        body->InsertL( 0, msgBody );
	    }
    else
	    {
        body->InsertL( 0, KNullDesC );
	    }
    messageData->SetBodyTextL(body);

    // Add addresses to messageData
    if( recipient.Length() )
		{
		SeparateAndAppendAddressL(KComma, recipient, *messageData, KToRecipient );
		}
    if( to.Length() )
        {
        SeparateAndAppendAddressL(KComma, to, *messageData, KToRecipient );
        }
	
    if ( !iSendUi )
        {
        iSendUi = CSendUi::NewL();
        }

    iSendUi->CreateAndSendMessageL(
        KSenduiMtmSmsUid, messageData, KNullUid, aLaunchEmbedded);

    CleanupStack::PopAndDestroy( 4, messageData );
    NotifyClient();
    }

// ---------------------------------------------------------
// CSmsHandler::GetField()
// ---------------------------------------------------------
//
TPtrC CSmsHandler::GetField(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CSmsHandler::GetField()" );

	TPtrC path = iParsedUrl->Des();

	TInt start = 0;
	TInt end = 0;

	CSmsHandler::TSchemeSmsFields posHeader = GetHeaderPos( aHeader );
	// field start from; and end at 
	switch( posHeader )
		{
		case ESchemeSms: // Sms
			{
			start = FieldStart( KSms );
			end = FieldEnd( KSms );
			break;
			}
		case ESchemeMsgBody: // Msg Body
			{
			start = FieldStart( KBody );
			end = FieldEnd( KBody );
			break;
			}
        case ESchemeTo:
            {
            start = FieldStart( KTo );
			end = FieldEnd( KTo );
			break;
            }
		case ESchemeNoMore:
			{
			break;
			}
		default:
			{
			break;
			}
		}

	CLOG_LEAVEFN( "CSmsHandler::GetField()" );

	return path.Mid( start, end-start );
	}

// ---------------------------------------------------------
// CSmsHandler::IsHeader()
// ---------------------------------------------------------
//
TBool CSmsHandler::IsHeader(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CSmsHandler::IsHeader()" );

	TBool retVal = EFalse;

	TPtrC path = iParsedUrl->Des();

	// is the field in the mmsto scheme 
	if( KErrNotFound != path.FindF( aHeader ) )
		{
		retVal = ETrue;
		}

	CLOG_LEAVEFN( "CSmsHandler::IsHeader()" );

	return retVal;
	}

// ---------------------------------------------------------
// CSmsHandler::FieldStart()
// ---------------------------------------------------------
//
TInt CSmsHandler::FieldStart(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CSmsHandler::FieldStart()" );

	TPtrC path = iParsedUrl->Des();
	TInt retVal = path.Length();

	// find the starting position of the specific filed
	if( IsHeader( aHeader ) )
		{
		retVal = path.FindF( aHeader ) + aHeader.Length();
		}

	CLOG_LEAVEFN( "CSmsHandler::FieldStart()" );

	return retVal;
	}

// ---------------------------------------------------------
// CSmsHandler::FieldEnd()
// ---------------------------------------------------------
//
TInt CSmsHandler::FieldEnd(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CSmsHandler::FieldEnd()" );

	TPtrC path = iParsedUrl->Des();
	TInt length = path.Length(); // length of the scheme
    TInt retVal = length;

    TInt startPos = FieldStart( aHeader );

    if( IsHeader( aHeader ) )
		{
        TInt temp = GetNextField( startPos );
        /* we need to subtract 1 if the result is 
           not equal to length because of the & or ? */
        retVal = ( temp == length ) ? length : ( temp - 1);
		}

	CLOG_LEAVEFN( "CSmsHandler::FieldEnd()" );

	return retVal;
	}

// ---------------------------------------------------------
// CSmsHandler::GetHeaderPos()
// ---------------------------------------------------------
//
CSmsHandler::TSchemeSmsFields CSmsHandler::GetHeaderPos(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CSmsHandler::GetHeaderPos()" );

	TSchemeSmsFields retVal = ESchemeSms;

	if( 0 == aHeader.Compare( KSms ) )
		{
		retVal = ESchemeSms;
		}
	else if ( 0 == aHeader.Compare( KBody ) )
		{
		retVal = ESchemeMsgBody;
		}
    else if ( 0 == aHeader.Compare( KTo ) )
		{
		retVal = ESchemeTo;
		}
    else
        {
        }

	CLOG_LEAVEFN( "CSmsHandler::GetHeaderPos()" );
	return retVal;
	}

// ---------------------------------------------------------
// CSmsHandler::GetNextField()
// ---------------------------------------------------------
//
TInt CSmsHandler::GetNextField( TInt aStart )
    {
	TPtrC path = iParsedUrl->Des();
	TInt retVal = path.Length();
    TPtrC scheme;
    RArray<TInt> array;

    if( aStart < retVal )
        {
        scheme.Set( path.Right( retVal - aStart ) );
        }
    else
        {
        return retVal;
        }

    TInt bodyPos = scheme.FindF( KBody );
    bodyPos = ( bodyPos == KErrNotFound ) ? retVal : bodyPos;
    array.Append( bodyPos );

    TInt toPos = scheme.FindF( KTo );
    toPos = ( toPos == KErrNotFound ) ? retVal : toPos;
    array.Append( toPos );   

    //retVal = ( bodyPos < retVal) ? bodyPos + aStart : retVal;
    TInt temp = Minimum( array );
    retVal = ( temp < retVal) ? temp + aStart : retVal;

    return retVal;
    }

//  End of File

