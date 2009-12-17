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
* Description:  State machine for mesage loading
*
*/



#ifndef PLAIN_MAILLOADSTATEHTML_H
#define PLAIN_MAILLOADSTATEHTML_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <MMessageLoadObserver.h>
#include "MMailState.h"

// FORWARD DECLARATIONS
class CLoadInlineImages;
class MStateMachine;
class MMailAppUiInterface;
class MLoadState;
class CMailMessage;
class CMsgMailViewerHtmlConv;

// CLASS DECLARATION

/**
*  CLoadHtmlFile
*  Load and set html message
*
*  @since Series 60 3.0
*/
class CLoadHtmlFile : public CActive, public MLoadState
	{
	public:
		
		~CLoadHtmlFile();	

	private: // Functions from base classes
		// From: MMessageLoadObserver
		void MessageLoadingL(TInt aStatus, CMailMessage& aMessage);
		// From MLoadState
		MLoadState* NextStateL();
        // From MLoadState
		void MakeRequestL( 
			TRequestStatus& aStatus,
			CMailMessage& aMessage );		

    protected: // From CActive
        void RunL();
        TInt RunError( TInt aError );
        void DoCancel();
        
	public:
		/// Ctor
		CLoadHtmlFile( 
			MMailAppUiInterface* aAppUI, 
			MStateMachine& aStateMachine );
	    
	private: // implementation
	
	    void HandleBodyTextReadyL( CMailMessage& aMessage );
		void ShowHtmlErrorNoteIfNeededL( TInt aResult );
		// Adds message html body part as an attachment
		void AddHtmlFileAttachmentL(CMailMessage& aMessage, RFile aFile );
		void ParseHtmlFileL(CMailMessage& aMessage, RFile aFile );
		void CompleteRequest( TInt aError );
		// Adds message html body part as an attachment and ads charset info if necessary
		void AddHtmlFileAttachmentAndCharsetL( CMailMessage& aMessage, RFile& aHandle );
		
	private: // Data 
	    // Own. HTML to Plain text converter
	    CMsgMailViewerHtmlConv* iConverter;
	    
	    // Not own. Email message 
	    CMailMessage* iMessage;
    
	};

#endif      // PLAIN_MAILLOADSTATEHTML_H   
            
// End of File
