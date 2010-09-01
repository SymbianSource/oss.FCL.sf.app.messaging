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
* Description:  State for message loading
*
*/



#ifndef HTML_LOAD_HTML_FILE_H
#define HTML_LOAD_HTML_FILE_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <MMessageLoadObserver.h>
#include "MMLoadState.h"

// FORWARD DECLARATIONS
class MMailAppUiInterface;
class MLoadState;
class CMailMessage;
class CMailHtmlBodyControl;
// CLASS DECLARATION

/**
*  CLoadHtmlFile
*  Load and set html message
*
*  @since Series 60 3.0
*/
class CLoadHtmlFile : public CBase, public MLoadState
	{
	public:
		
		~CLoadHtmlFile();	
	private: // Functions from base classes
		// From: MMessageLoadObserver
		void MessageLoadingL(TInt aStatus, CMailMessage& aMessage);
		// From MLoadState
		MLoadState* NextStateL();
		void MakeRequestL( 
			TRequestStatus& aStatus,
			CMailMessage& aMessage );		
	public:
		/// Ctor
		CLoadHtmlFile( 
			MMailAppUiInterface* aAppUI, 
			CMailHtmlBodyControl& aHtmlControl );
			
	private: // implementation	
	    void LoadHtmlFileL( CMailMessage& aMessage );
	
	private: // data
	// Ref: to html control
	CMailHtmlBodyControl& iHtmlControl;
	};

#endif // HTML_LOAD_HTML_FILE_H   
            
// End of File
