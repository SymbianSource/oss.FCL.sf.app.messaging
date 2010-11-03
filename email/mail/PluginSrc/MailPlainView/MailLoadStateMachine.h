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



#ifndef MAIL_LOADSTATEMACHINE_H
#define MAIL_LOADSTATEMACHINE_H

//  INCLUDES
#include <e32base.h>
#include "MMailStateMachine.h"
//#include <?include_file>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
class MMailAppUiInterface;
class CMailMessage;
class MLoadState;

// CLASS DECLARATION

/**
*  CLoadStateMachine
*  Loads mail message asynchronous parts.
*
*  @since Series 60 3.0
*/	
class CLoadStateMachine : public CActive, MStateMachine
	{
	public:
		static CLoadStateMachine* NewL( 
			MMailAppUiInterface* aAppUI, 
			CMailMessage& aMessage );
			
		// Start operation
		void StartL();
		/// Destructor
		~CLoadStateMachine();
	protected: // Functions from base classes	
		void DoCancel();
		void RunL(); // complete and select next state;
		TInt RunError(TInt aError);

		void SetIdArray( RPointerArray<HBufC>* aIdArray );
	    RPointerArray<HBufC>* GetIdArray();
	    
	private:
		/// Ctor
		CLoadStateMachine( 
			MMailAppUiInterface* aAppUI, 
			CMailMessage& aMessage );
		void ConstructL();

	private: // Data
	    /// Ref: app UI call back
	    MMailAppUiInterface* iAppUI;
	    /// Ref to mail message
	    CMailMessage& iMessage; 
	    /// Own: current state
	    MLoadState* iState;
		/// Own: id array
		RPointerArray<HBufC>* iIdArray;		    
	};



#endif      // MAIL_LOADSTATEMACHINE_H   
            
// End of File
