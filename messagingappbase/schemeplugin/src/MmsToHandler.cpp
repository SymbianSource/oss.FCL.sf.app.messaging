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
*      for mmsto: scheme
*
*/



// INCLUDE FILES

#include "MmsToHandler.h"
#include "SchemeDefs.h"
#include "SchemeDispLogger.h"
#include "LocalAppHandler.h"
#include <ecom.h>		// For REComSession
#include <apgcli.h>
#include <apparc.h>
#include <eikdoc.h>
#include <eikproc.h>
#include <f32file.h>
#include <s32file.h> // RFileWriteStream
#include <sendui.h> // CSendUi
#include <SenduiMtmUids.h> 
#include <txtrich.h>
#include <CMessageData.h>


// ================= CONSTANTS =======================

   

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsToHandler::NewL()
// ---------------------------------------------------------
//
CMmsToHandler* CMmsToHandler::NewL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CMmsToHandler::NewL()" );

	CMmsToHandler* self = new( ELeave ) CMmsToHandler();
	CleanupStack::PushL( self );
	self->ConstructL( aUrl );
	CleanupStack::Pop( self );

	CLOG_LEAVEFN( "CMmsToHandler::NewL()" );

	return self;
	}

// ---------------------------------------------------------
// CMmsToHandler::~CMmsToHandler()
// ---------------------------------------------------------
//
CMmsToHandler::~CMmsToHandler()
	{
	CLOG_ENTERFN( "CMmsToHandler::~CMmsToHandler()" );

    delete iSendUi;
    iFs.Close();

	CLOG_LEAVEFN( "CMmsToHandler::~CMmsToHandler()" );
	}

// ---------------------------------------------------------
// CMmsToHandler::CMmsToHandler()
// ---------------------------------------------------------
//
CMmsToHandler::CMmsToHandler() : CBaseHandler()
	{
	// Deliberately do nothing here : 
	// See ConstructL() for initialisation completion.
	}

// ---------------------------------------------------------
// CMmsToHandler::ConstructL()
// ---------------------------------------------------------
//
void CMmsToHandler::ConstructL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CMmsToHandler::ConstructL()" );

	BaseConstructL( aUrl );
    
    User::LeaveIfError( iFs.Connect() );
	CLOG_LEAVEFN( "CMmsToHandler::ConstructL()" );
	}

// ---------------------------------------------------------
// CMmsToHandler::HandleUrlEmbeddedL()
// ---------------------------------------------------------
//
void CMmsToHandler::HandleUrlEmbeddedL()
	{
	CLOG_ENTERFN( "CMmsToHandler::HandleUrlEmbeddedL()" );
    HandleUrlL( ETrue ); 
    NotifyClient();
	CLOG_LEAVEFN( "CMmsToHandler::HandleUrlEmbeddedL()" );
	}

// ---------------------------------------------------------
// CMmsToHandler::HandleUrlStandaloneL()
// ---------------------------------------------------------
//
void CMmsToHandler::HandleUrlStandaloneL()
	{
	CLOG_ENTERFN( "CMmsToHandler::HandleUrlStandaloneL()" );
    LaunchSchemeAppWithCommandLineL();
	CLOG_LEAVEFN( "CMmsToHandler::HandleUrlStandaloneL()" );
	}

// ---------------------------------------------------------
// CMmsToHandler::HandleUrlL()
// ---------------------------------------------------------
//    
void CMmsToHandler::HandleUrlL(TBool aLaunchEmbedded)
    {
    // Parse url
	TPtrC mmsto = GetField( KMmsto );
    TPtrC to = GetField( KTo );
	TPtrC subject = GetField( KSubject );
	TPtrC msgBody = GetField( KBody );
	TPtrC cc = GetField( KCc );
	
	CMessageData* messageData = CMessageData::NewLC();

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

    // Add subject
    if ( subject.Length() )
        {
        messageData->SetSubjectL( &subject );
        }

    // Add addresses to messageData
    if( mmsto.Length() )
		{
		SeparateAndAppendAddressL(KComma, mmsto, *messageData);
		}
    else
        {
        }
    if ( to.Length() )
        {
        SeparateAndAppendAddressL(KComma, to, *messageData, KToRecipient);
        }
    if ( cc.Length() )
        {
        SeparateAndAppendAddressL(KComma, cc, *messageData, KCcRecipient );
        }
           
    if ( !iSendUi )
        {
        iSendUi = CSendUi::NewL();
        }

    iSendUi->CreateAndSendMessageL(
        KSenduiMtmMmsUid, messageData, KNullUid, aLaunchEmbedded);

    CleanupStack::PopAndDestroy(4, messageData);	
    }

// ---------------------------------------------------------
// CMmsToHandler::GetField()
// ---------------------------------------------------------
//
TPtrC CMmsToHandler::GetField(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CMmsToHandler::GetField()" );

	TPtrC path = iParsedUrl->Des();

	TInt start = 0;
	TInt end = 0;

	CMmsToHandler::TSchemeMmsToFields posHeader = GetHeaderPos( aHeader );
	// field start from; and end at 
	switch( posHeader )
		{
		case ESchemeMmsTo: // MmsTo
			{
			start = FieldStart( KMmsto );
			end = FieldEnd( KMmsto );
			break;
			}
		case ESchemeSubject: // Subject
			{
			start = FieldStart( KSubject );
			end = FieldEnd( KSubject );
			break;
			}
		case ESchemeMsgBody: // Msg Body
			{
			start = FieldStart( KBody );
			end = FieldEnd( KBody );
			break;
			}
		case ESchemeCc: // Cc
			{
			start = FieldStart( KCc );
			end = FieldEnd( KCc );
			break;
			}
        case ESchemeTo: // To
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

	CLOG_LEAVEFN( "CMmsToHandler::GetField()" );

	return path.Mid( start, end-start );
	}

// ---------------------------------------------------------
// CMmsToHandler::IsHeader()
// ---------------------------------------------------------
//
TBool CMmsToHandler::IsHeader(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CMmsToHandler::IsHeader()" );

	TBool retVal = EFalse;

	TPtrC path = iParsedUrl->Des();

	/* is the field in the mmsto sheme */
	if( KErrNotFound != path.FindF( aHeader ) )
		{
		retVal = ETrue;
		}

	CLOG_LEAVEFN( "CMmsToHandler::IsHeader()" );

	return retVal;
	}

// ---------------------------------------------------------
// CMmsToHandler::FieldStart()
// ---------------------------------------------------------
//
TInt CMmsToHandler::FieldStart(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CMmsToHandler::FieldStart()" );

	TPtrC path = iParsedUrl->Des();
	TInt retVal = path.Length();

	/* find the starting position of the specific filed */
	if( IsHeader( aHeader ) )
		{
		retVal = path.FindF( aHeader ) + aHeader.Length();
		}

	CLOG_LEAVEFN( "CMmsToHandler::FieldStart()" );

	return retVal;
	}

// ---------------------------------------------------------
// CMmsToHandler::FieldEnd()
// ---------------------------------------------------------
//
TInt CMmsToHandler::FieldEnd(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CMmsToHandler::FieldEnd()" );

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

	CLOG_LEAVEFN( "CMmsToHandler::FieldEnd()" );

	return retVal;
	}

// ---------------------------------------------------------
// CMmsToHandler::GetHeaderPos()
// ---------------------------------------------------------
//
CMmsToHandler::TSchemeMmsToFields CMmsToHandler::GetHeaderPos(const TDesC& aHeader)
	{
	CLOG_ENTERFN( "CMmsToHandler::GetHeaderPos()" );

	TSchemeMmsToFields retVal = ESchemeMmsTo;

	if( 0 == aHeader.Compare( KMmsto ) )
		{
		retVal = ESchemeMmsTo;
		}
	else if ( 0 == aHeader.Compare( KSubject ) )
		{
		retVal = ESchemeSubject;
		}
	else if ( 0 == aHeader.Compare( KBody ) )
		{
		retVal = ESchemeMsgBody;
		}
	else if ( 0 == aHeader.Compare( KCc ) )
		{
		retVal = ESchemeCc;
		}
    else if ( 0 == aHeader.Compare( KTo ) )
		{
		retVal = ESchemeTo;
		}

	CLOG_LEAVEFN( "CMmsToHandler::GetHeaderPos()" );

	return retVal;
	}

// ---------------------------------------------------------
// CMmsToHandler::GetNextField()
// ---------------------------------------------------------
//
TInt CMmsToHandler::GetNextField( TInt aStart )
    {
	TPtrC path = iParsedUrl->Des();
	TInt retVal = path.Length();
    TPtrC scheme;
    RArray<TInt> array;

    //KSubject KBody KCc KTo
    if( aStart < retVal )
        {
        scheme.Set( path.Right( retVal - aStart ) );
        }
    else
        {
        return retVal;
        }

    TInt subjPos = scheme.FindF( KSubject );
    subjPos = ( subjPos == KErrNotFound ) ? retVal : subjPos;
    array.Append( subjPos );

    TInt bodyPos = scheme.FindF( KBody );
    bodyPos = ( bodyPos == KErrNotFound ) ? retVal : bodyPos;
    array.Append( bodyPos );

    TInt ccPos = scheme.FindF( KCc );
    ccPos = ( ccPos == KErrNotFound ) ? retVal : ccPos;
    array.Append( ccPos );

    TInt toPos = scheme.FindF( KTo );
    toPos = ( toPos == KErrNotFound ) ? retVal : toPos;
    array.Append( toPos );    

    TInt temp = Minimum( array );
    retVal = ( temp < retVal) ? temp + aStart : retVal;

    return retVal;
    }

// End of file

