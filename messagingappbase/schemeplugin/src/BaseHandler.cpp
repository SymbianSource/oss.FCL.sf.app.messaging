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
*      Implementation of Scheme handler base class
*
*/



// INCLUDE FILES

#include "BaseHandler.h"
#include "SchemeDispLogger.h"
#include <e32base.h>
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include <bautils.h>
#include <escapeutils.h>
#include <apparc.h>
#include <apgcli.h>
#include <s32mem.h>
#include <CMessageData.h>
#include <AknServerApp.h>

// ================= CONSTANTS =======================
LOCAL_D const TUid KUidSchemeApp = { SCHEME_APP_UID };

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CBaseHandler::CBaseHandler()
// ---------------------------------------------------------
//
CBaseHandler::CBaseHandler() : CSchemeHandler()
	{
	iSchemeDoc = NULL;
	}

// ---------------------------------------------------------
// CCtiHandler::~CCtiHandler()
// ---------------------------------------------------------
//
CBaseHandler::~CBaseHandler()
	{
	delete iParsedUrl;
	}


// ---------------------------------------------------------
// CBaseHandler::ConstructL()
// ---------------------------------------------------------
//
void CBaseHandler::BaseConstructL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CBaseHandler::BaseConstructL()" );

	iParsedUrl = EscapeUtils::EscapeDecodeL( aUrl );

	CLOG_LEAVEFN( "CBaseHandler::BaseConstructL()" );
	}

// ---------------------------------------------------------
// CBaseHandler::Observer()
// ---------------------------------------------------------
//
void CBaseHandler::Observer( MAknServerAppExitObserver* aSchemeDoc )
	{
	iSchemeDoc = aSchemeDoc;
	}

// ---------------------------------------------------------
// CBaseHandler::NotifyClient()
// ---------------------------------------------------------
//
void CBaseHandler::NotifyClient()
	{
	if( NULL !=iSchemeDoc )
		{
        iSchemeDoc->HandleServerAppExit( KErrNone );
		}
	}

// ---------------------------------------------------------
// CBaseHandler::SeparateAndAppendToAddressL
// ---------------------------------------------------------
//    
void CBaseHandler::SeparateAndAppendAddressL(
	const TDesC& aSeparator, 
	const TDesC& aString, 
	CMessageData& aMessageData,
	const TInt aRecipientType ) const
	{
	if ( aString.Length() == 0 )
        {
        return;
        }
    TInt endPos = 0;
    TInt startPos = 0;
    TInt tmp = 0;
    TInt tmp2 = 0;
    TInt length = aString.Length();
    TInt len = 0;
    while ( endPos < length )
        {
        tmp = aString.Right( length - endPos).FindF( aSeparator );
        if ( tmp > 0 )
            {
            endPos += tmp;
            }
        else // tmp == KErrNotFound
            {
            endPos = aString.Length();
            }
        tmp = startPos;
        tmp2 = endPos - 1;
        // remove spaces from the start         
        while ( aString[tmp] == ' ' && tmp < endPos )
            {
            ++tmp;
            }
        // remove spaces from the end
        while ( aString[tmp2] == ' ' && tmp2 > startPos )
            {
            --tmp2;
            }
        len = tmp2 - tmp + 1;
        // Append item to array without spaces
        switch( aRecipientType )
        	{
        	case KCcRecipient: aMessageData.AppendCcAddressL( aString.Mid( tmp, len ) );break;
        	default:; // fall through
        	case KToRecipient: aMessageData.AppendToAddressL( aString.Mid( tmp, len ) );break;
        	}
        
        ++endPos;
        startPos = endPos;
        }    
	}   

// ---------------------------------------------------------
// CBaseHandler::Minimum()
// ---------------------------------------------------------
//
TInt CBaseHandler::Minimum( RArray<TInt>& aArray ) const
    {
    // Sort array.
    aArray.Sort();
    // Return smallest
    return aArray[0];
    }


// ---------------------------------------------------------
// CBaseHandler::LaunchSchemeAppWithCommandLineL
// ---------------------------------------------------------
//
void CBaseHandler::LaunchSchemeAppWithCommandLineL()
    {
    // Try launching...
    RApaLsSession   appArcSession;
    User::LeaveIfError( appArcSession.Connect() );
    CleanupClosePushL<RApaLsSession>( appArcSession );
    TThreadId dummyId;

    TApaAppInfo appInfo;
    appArcSession.GetAppInfo( appInfo, KUidSchemeApp );

    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
    #if (defined(SYMBIAN_SUPPORT_UI_FRAMEWORKS_V1) || defined(__SERIES60_27__) || defined(__SERIES60_28__)) 
    cmdLine->SetLibraryNameL(appInfo.iFullName);
    #else
    cmdLine->SetExecutableNameL(appInfo.iFullName);
    #endif
    cmdLine->SetDocumentNameL( *iParsedUrl );
    cmdLine->SetCommandL( EApaCommandCreate );

    // Get the whole parameter list and put to the command line
    if ( iParamList )
        {
        TInt size = iParamList->Size();
        CBufFlat* store = CBufFlat::NewL( size );
        CleanupStack::PushL( store );
	    RBufWriteStream outStream;
		outStream.Open( *store );
        CleanupClosePushL<RBufWriteStream>( outStream );
		iParamList->ExternalizeL( outStream );
        cmdLine->SetTailEndL( store->Ptr( 0 ) );
        CleanupStack::PopAndDestroy( 2 );  // store, close outStream
        }

    User::LeaveIfError( appArcSession.StartApp( *cmdLine,dummyId) );
    CLOG_WRITE( "Launched SchemeApp succesfully" )
  
    CleanupStack::PopAndDestroy( 2 ); // close appArcSession, cmdLine
    }

// End of file
