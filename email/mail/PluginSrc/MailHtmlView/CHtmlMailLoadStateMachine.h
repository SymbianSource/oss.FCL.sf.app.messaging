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
* Description:  Async load state machine
*
*/



#ifndef CHTML_MAIL_LOADSTATEMACHINE_H
#define CHTML_MAIL_LOADSTATEMACHINE_H

//  INCLUDES
#include <e32base.h>
#include "CHtmlMailLoadStateMachine.h"

// FORWARD DECLARATIONS
class MMailAppUiInterface;
class CMailMessage;
class MLoadState;
class CMailHtmlBodyControl;

// CLASS DECLARATION

/**
*  CHtmlLoadStateMachine
*  Loads html mail message asynchronous parts.
*
*  @since Series 60 3.0
*/	
class CHtmlLoadStateMachine : public CActive
	{
	public:
		static CHtmlLoadStateMachine* NewL( 
			MMailAppUiInterface* aAppUI, 
			CMailMessage& aMessage,
			CMailHtmlBodyControl& aHtmlControl );
			
		// Start operation
		void StartL();
		/// Destructor
		~CHtmlLoadStateMachine();
	protected: // Functions from base classes	
		void DoCancel();
		void RunL(); // complete and select next state;
		TInt RunError(TInt aError);
	    
	private:
		/// Ctor
		CHtmlLoadStateMachine( 
			MMailAppUiInterface* aAppUI, 
			CMailMessage& aMessage,
			CMailHtmlBodyControl& aHtmlControl );
		void ConstructL();

	private: // Data
	    /// Ref: app UI call back
	    MMailAppUiInterface* iAppUI;
	    /// Ref to mail message
	    CMailMessage& iMessage; 
	    /// Ref to html control
	    CMailHtmlBodyControl& iHtmlControl; 	    
	    /// Own: current state
	    MLoadState* iState;		    
	};



#endif // CHTML_MAIL_LOADSTATEMACHINE_H   
            
// End of File
