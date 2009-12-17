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
* Description:  State machine for message loading
*
*/



#ifndef HTML_LOAD_ATTACHED_MESSAGES_H
#define HTML_LOAD_ATTACHED_MESSAGES_H

//  INCLUDES
#include <e32base.h>
#include <MMessageLoadObserver.h>
#include "MMLoadState.h"
// FORWARD DECLARATIONS
class CLoadInlineImages;
class MStateMachine;
class MMailAppUiInterface;
class CMailMessage;
// CLASS DECLARATION

/**
*  CLoadAttachedMessages
*  Load and set attached messages
*
*  @since Series 60 3.0
*/	
class CLoadAttachedMessages : public CBase, public MLoadState
	{
	public:
		
		~CLoadAttachedMessages();	
	private: // Functions from base classes
		// From: MMessageLoadObserver
		void MessageLoadingL(TInt aStatus, CMailMessage& aMessage);
		// From MLoadState
		MLoadState* NextStateL();
		void MakeRequestL( 
			TRequestStatus& aStatus,
			CMailMessage& aMessage );
			
    private:
        void AddAttachmentsToAppUiL( CMailMessage& aMessage );
        
	public:
		/// Ctor
		CLoadAttachedMessages( MMailAppUiInterface* aAppUI );
	};
	

#endif      // HTML_LOAD_ATTACHED_MESSAGES_H   
            
// End of File
