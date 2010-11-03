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
* Description:   Postcard query waiter for global msg query
*
*/



#ifndef POSTCARDQUERYWAITER_H
#define POSTCARDQUERYWAITER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

// Interface class for the PostcardQueryWaiter class
// This function is used when user has reacted to the query.
class MPostcardQueryWaiterCallback
	{
    public:

    /**
    * Callback function called when query is answered.
    * @since 3.0
    * @param aResult The button user pushed
    */
    virtual void QueryWaiterCallbackL( TInt aResult ) = 0;

    /**
    * Callback function called when QueryWaiterCallbackL() leaves.
    * Should perform cleanup. After QueryWaiterCallbackError has
    * been called the error is probagated to active scheduler.
    * @since 3.2
    */
    virtual void QueryWaiterCallbackError() = 0;

	};

/**
* Common utility class for Postcard global message queries
* @lib postcard.exe
* @since 3.0
*/
class CPostcardQueryWaiter : public CActive
    {
    public:
        CPostcardQueryWaiter( MPostcardQueryWaiterCallback* aCallback )
            : CActive( EPriorityStandard ), iCallback( aCallback )
            { CActiveScheduler::Add( this ); }
    public:
        // Deletes itself when the RunL is called.
        void SetActiveD() { SetActive(); }
    protected:
        void RunL() { 
                // First call callback function
            	iCallback->QueryWaiterCallbackL( iStatus.Int() );
            	// And delete itself.
	            delete this; }
        void DoCancel() {}
        TInt RunError(TInt aError) {
            iCallback->QueryWaiterCallbackError();
            delete this;
            return aError;
            }
    private:

    MPostcardQueryWaiterCallback* iCallback;
    };


#endif // POSTCARDQUERYWAITER

// End of file
