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
* Description:  
*     Wrapper to make SMIL parsing synchronous.
*
*/



#include <e32def.h>     // for basic types
#include <E32BASE.H>    // for CActiveSceduler
#include <eikenv.h>     // for CBase

#include <GMXMLDocument.h>
#include <GMXMLParser.h>

#include "SmilSyncParse.h"

// =========================================================
// CSmilSyncParse
// =========================================================

// ---------------------------------------------------------
// CSmilSyncParse::NewLC()
// ---------------------------------------------------------
//
CSmilSyncParse* CSmilSyncParse::NewLC()
    {
    CSmilSyncParse* self = new ( ELeave ) CSmilSyncParse();

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CSmilSyncParse::CSmilSyncParse
// ---------------------------------------------------------
//
CSmilSyncParse::CSmilSyncParse()
    {
    }

// ---------------------------------------------------------
// CSmilSyncParse::~CSmilSyncParse()
// ---------------------------------------------------------
//
CSmilSyncParse::~CSmilSyncParse()
    {
    if ( iWaiting )
        {
        (*iWaiting) = KErrAbort;
        iWaiting = NULL;
        }
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    delete iParser;
    }

// ---------------------------------------------------------
// CSmilSyncParse::StartL
// ---------------------------------------------------------
//
CMDXMLDocument* CSmilSyncParse::StartL( RFs aRFs, const TDesC& aFileName )
    {
    User::LeaveIfError( iParser->ParseFile(
        aRFs, const_cast<TDesC&>( aFileName ) ) );

    TInt waiting = KErrNone;
    iWaiting = &waiting;
    iWait.Start();
    iWaiting = NULL;
    
    User::LeaveIfError( waiting );

    return iParser->DetachXMLDoc();
    }

// ---------------------------------------------------------
// CSmilSyncParse::StartL
// ---------------------------------------------------------
//
CMDXMLDocument* CSmilSyncParse::StartL( RFile& aFile )
	{
    User::LeaveIfError( iParser->ParseFile( aFile ) );

    TInt waiting = KErrNone;
    iWaiting = &waiting;
    iWait.Start();
    iWaiting = NULL;
    
    User::LeaveIfError( waiting );

    return iParser->DetachXMLDoc();		
	}


// ---------------------------------------------------------
// CSmilSyncParse::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CSmilSyncParse::ConstructL()
    {
    iParser = CMDXMLParser::NewL( this );
    }


// ---------------------------------------------------------
// CSmilSyncParse::ParseFileCompleteL
//
// Parser callback.
// ---------------------------------------------------------
//
void CSmilSyncParse::ParseFileCompleteL()
    {
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// End of file



