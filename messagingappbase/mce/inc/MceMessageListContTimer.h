/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     List container key timer.
*
*/



#ifndef MCEMESSAGELISTCONTTIMER_H
#define MCEMESSAGELISTCONTTIMER_H

//  INCLUDES

// FORWARD DECLARATIONS
class CPtiEngine;

// CLASS DECLARATION

/**
*  Timer used by message view to find and select a message entry
*  Derived from CActive.
*/
class CMceMessageListContainerKeyTimer :
    public CActive
    {
    public:

        /**
        * Constructor.
        */        
        static CMceMessageListContainerKeyTimer* NewL( CPtiEngine& aT9Interface );
        
        /**
        * Destructor.
        */
        ~CMceMessageListContainerKeyTimer();
       
        /**
        * Starts key click timer
        */
        void Start();

    protected:

        /**
        * Cancels key click timer
        */
        virtual void DoCancel();
        
        /**
        * Runs key click timer
        */
        virtual void RunL();

    private:

        /**
        * Constructor is private.
        */       
        CMceMessageListContainerKeyTimer( CPtiEngine& aT9Interface );
        
        /**
        * Creates local key click timer
        */
        void ConstructL();

    private:
        RTimer      iTimer;        
        CPtiEngine& iT9Interface;

    };


#endif

// End of file
