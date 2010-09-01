/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Mail operation base class
*
*/

#ifndef C_CMAILOPERATION_H
#define C_CMAILOPERATION_H

#include <msvapi.h>

/**
 *  Operation
 *
 *  Mail operation base class
 *
 *  @lib none, internal
 *  @since S60 v3.1
 */
class CMailOperation  : public CMsvOperation
    {

public:
    // See CMsvOperation
    CMailOperation( 
        CMsvSession& aMsvSession, 
        TInt aPriority, 
        TRequestStatus& aObserverRequestStatus )
        :CMsvOperation( 
        aMsvSession,
        aPriority,
        aObserverRequestStatus ){};
        
    ~CMailOperation(){};

    /**
     * Suspend ongoing operation
     *
     * @since S60 v3.1
     * @param aSetSuspend ETrue suspends operation
     */
    virtual void SuspendL( TBool aSetSuspend ) = 0;
    
    /**
     * Operation completed
     *
     * @since S60 v3.1
     * @param aCompletionCode completion code
     * @return ETrue if operation can be stopped.
     */
    virtual TBool CompletedL( TInt aCompletionCode ) = 0;    

    };

#endif //  C_CMAILOPERATION_H
