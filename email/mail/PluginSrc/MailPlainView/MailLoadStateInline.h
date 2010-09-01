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



#ifndef PLAIN_MAILLOADSTATEINLINE_H
#define PLAIN_MAILLOADSTATEINLINE_H

//  INCLUDES
#include <e32base.h>
#include <MMessageLoadObserver.h>
#include "MMailState.h"

// FORWARD DECLARATIONS
class CLoadInlineImages;
class MStateMachine;
class MMailAppUiInterface;
class CMailMessage;
// CLASS DECLARATION
/**
*  CLoadInlineImages
*  Load and set mail message inline images
*
*  @since Series 60 3.0
*/	
class CLoadInlineImages : public CBase, public MLoadState
	{
	public:
		~CLoadInlineImages();
	private: // Functions from base classes
		// From: MMessageLoadObserver
		void MessageLoadingL(TInt aStatus, CMailMessage& aMessage);
		
		// From MLoadState
		MLoadState* NextStateL();
		void MakeRequestL( 
			TRequestStatus& aStatus, 
			CMailMessage& aMessage );
			
    private: // Implementation    
        void AddAttachmentsToAppUiL( CMailMessage& aMessage );
        
	public:
		/// Ctor
		CLoadInlineImages( 
			MMailAppUiInterface* aAppUI,
			MStateMachine& aStateMachine );			
	};


#endif      // PLAIN_MAILLOADSTATEINLINE_H   
            
// End of File
