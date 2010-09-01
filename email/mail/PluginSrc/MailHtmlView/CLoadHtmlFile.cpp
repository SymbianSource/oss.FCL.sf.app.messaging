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
#include    "CLoadHtmlFile.h"
#include 	"CHtmlLoadAttachments.h"
#include    "CMailHtmlBodyControl.h"
#include    <MMailAppUiInterface.h>
#include    <CMailMessage.h>
#include 	<stringloader.h>

// ============================ MEMBER FUNCTIONS ===============================

// load html file
// -----------------------------------------------------------------------------
// CLoadHtmlFile::MessageLoadingL
// -----------------------------------------------------------------------------
//	
void CLoadHtmlFile::MessageLoadingL( TInt aStatus, CMailMessage& aMessage )
	{
	if( aStatus == CMailMessage::EBodyTextReady ) 
		{
		LOG("CLoadHtmlFile::MessageLoadingL - EBodyTextReady");

        TRAPD( err, LoadHtmlFileL( aMessage ) );
        
        // Complete observer regardless of result	
		TRequestStatus* observer = iLoadStatus;
		User::RequestComplete( observer, err );
		
		// Then leave this method if error occurred
		User::LeaveIfError( err );
		}
	}

// -----------------------------------------------------------------------------
// CLoadHtmlFile::CLoadHtmlFile
// -----------------------------------------------------------------------------
//	
CLoadHtmlFile::CLoadHtmlFile( 
	MMailAppUiInterface* aAppUI,
	CMailHtmlBodyControl& aHtmlControl ):
	iHtmlControl( aHtmlControl )
	{
	iAppUI = aAppUI;
	}

// -----------------------------------------------------------------------------
// CLoadHtmlFile::~CLoadHtmlFile
// -----------------------------------------------------------------------------
//	
CLoadHtmlFile::~CLoadHtmlFile()
	{
	
	}

// -----------------------------------------------------------------------------
// CLoadHtmlFile::NextStateL
// -----------------------------------------------------------------------------
//		
MLoadState* CLoadHtmlFile::NextStateL()
	{
	return new( ELeave ) CHtmlLoadAttachments( iAppUI );
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
	aMessage.LoadHtmlContentL( *this );
	aStatus = KRequestPending;
	}

// -----------------------------------------------------------------------------
// CLoadHtmlFile::LoadHtmlFileL
// -----------------------------------------------------------------------------
//	
void CLoadHtmlFile::LoadHtmlFileL( CMailMessage& aMessage )
    {
	RFile handle = aMessage.HtmlContent();
	CleanupClosePushL( handle );
	if ( handle.SubSessionHandle() != 0 )
		{
        // load content to br control
	    TInt fileSize;
	    TInt error = handle.Size( fileSize );
	    if ( error == KErrNone )
	    	{
	    	iHtmlControl.BrowserControl().LoadFileL( handle );
	    	}
	    else
	        {
	        LOG1( "CLoadHtmlFile::LoadHtmlFileL error:%d", error );
	        User::Leave( error );
	        }       
		}
	else
		{
		// corrupted html file?
		User::Leave( KErrCorrupt );
		}    
    CleanupStack::PopAndDestroy(); // handle		
    }

//  End of File  
