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
* Description:  wait operation
*
*/

#ifndef C_MAILWAITOPERATION_H
#define C_MAILWAITOPERATION_H

#include <msvapi.h>

/**
 *  Mail viewer wait operation
 *
 *  Suspends operation execution when UI needs to execute time consuming operations.
 *
 *  @lib none
 *  @since S60 3.1
 */
class CMailWaitOperation : public CMsvOperation
    {
public:
    /**
     * NewL
     *
     * @since S60 3.1
     * @param aMsvSession Message server session.
     * @param aObserverRequestStatus Request status
     * @return ?description
     */
    static CMailWaitOperation* NewL( 
    	CMsvSession& aMsvSession, 
    	TRequestStatus& aObserverRequestStatus );


    ~CMailWaitOperation();

protected:

    /**
     * From CMsvOperation.
     */
    const TDesC8& ProgressL();
    
    /**
     * From CActive.
     */    
	void DoCancel();
	void RunL();
	
private:

    CMailWaitOperation(
    	CMsvSession& aMsvSession, 
    	TRequestStatus& aObserverRequestStatus );

    void ConstructL();
    };


#endif // C_MAILWAITOPERATION_H
