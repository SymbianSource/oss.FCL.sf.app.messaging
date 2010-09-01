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
* Description:  State for loading message attachments
*
*/



#ifndef HTML_MAIL_LOAD_STATE_ATTACHMENTS_H
#define HTML_MAIL_LOAD_STATE_ATTACHMENTS_H

//  INCLUDES
#include <e32base.h>
#include <MMessageLoadObserver.h>
#include "MMLoadState.h"
// FORWARD DECLARATIONS
class MStateMachine;
class MMailAppUiInterface;
class CMailMessage;
class MMsvAttachmentManager;

// CLASS DECLARATION

/**
*  CHtmlLoadAttachments
*  Load and set message attachments
*
*  @since Series 60 3.0
*/	
class CHtmlLoadAttachments: public CBase, public MLoadState
	{
	public:
		
		~CHtmlLoadAttachments();	
	private: // Functions from base classes
		// From: MMessageLoadObserver
		void MessageLoadingL(TInt aStatus, CMailMessage& aMessage);
		// From MLoadState
		MLoadState* NextStateL();
		void MakeRequestL( 
			TRequestStatus& aStatus,
			CMailMessage& aMessage );
			
    private: // Implementation    
        void AddAttachmentsToAppUiL( MMsvAttachmentManager& aManager );
        					
	public:
		/// Ctor
		CHtmlLoadAttachments( MMailAppUiInterface* aAppUI );
	};
	

#endif      // HTML_MAIL_LOAD_STATE_ATTACHMENTS_H   
            
// End of File
