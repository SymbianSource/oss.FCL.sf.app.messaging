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



#ifndef MMAILLOADSTATEMACHINE_H
#define MMAILLOADSTATEMACHINE_H

#include <e32cmn.h>

/**
*  MStateMachine
*  State machine call back API.
*
*  @since Series 60 3.0
*/
class MStateMachine
	{
	public: // new methods
		/**
        * SetIdArray.
        * @since Series 60 3.0
        * @param aIdArray Pointer array.
        * 	Takes ownership immediately.
        */
		virtual void SetIdArray( RPointerArray<HBufC>* aIdArray ) = 0;
		
		/**
        * GetIdArray.
        * @since Series 60 3.0
        * @return Pointer array
        * Ownership is not transferred.
        */	    
	    virtual RPointerArray<HBufC>* GetIdArray() = 0;
	    
	    /// Destructor
	    virtual ~MStateMachine() {};
	};

#endif      // MMAILLOADSTATEMACHINE_H 
            
// End of File
