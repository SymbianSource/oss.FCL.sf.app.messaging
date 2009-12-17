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
* Description:  state machine call back api
*
*/



#ifndef MMAIL_LOADSTATE_H_
#define MMAIL_LOADSTATE_H_

#include <MMessageLoadObserver.h>
#include <e32cmn.h>

// FORWARD DECLARATIONS
class MMailAppUiInterface;
class MStateMachine;
class CMailMessage;

/**
*  MLoadState
*  State API.
*
*  @since Series 60 3.0
*/
class MLoadState: public MMessageLoadObserver
	{	
	public: // new methods
        /**
        * NextStateL.
        * @since Series 60 3.0
        * @return Next state object. 
        * 	Ownership is transferred to the caller. 
        */				
		virtual MLoadState* NextStateL() = 0;
        
        /**
        * MakeRequestL.
        * @since Series 60 3.0
        * @param aStatus Async request status.
        * @return aMessage a ref to mail message.
        */
		virtual void MakeRequestL( 
			TRequestStatus& /*aStatus*/,
			CMailMessage& /*aMessage*/ ) = 0;
		
		/// Destructor	
		virtual ~MLoadState() {};
		
	protected: // data
		/// Ref: app UI call back
     	MMailAppUiInterface* iAppUI;
		/// Ref: Loading status
		TRequestStatus* iLoadStatus;
		/// Ref: to state machine
		MStateMachine* iStateMachine;
	};

#endif      // MMAIL_LOADSTATE_H_ 
            
// End of File
