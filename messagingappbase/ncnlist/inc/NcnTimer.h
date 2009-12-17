/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*       This class defines functionality for small timer
*
*/



#ifndef __NCNTIMER_H__
#define __NCNTIMER_H__

//  INCLUDES
#include <e32base.h> // CTimer

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class MNcnTimer;

// CLASS DECLARATION

/**
*  CNcnTimer
*/
class CNcnTimer : public CTimer
    {
    public:
       /**
        * 2-phase constructor.
        */
        static CNcnTimer* NewL( MNcnTimer& aObserver );

        /**
        * Destructor.
        */
        virtual ~CNcnTimer();

    private:

        /**
        * From CActive
        */
        virtual void RunL();

       /**
        * C++ constructor.
        */
        CNcnTimer( MNcnTimer& aObserver );

        /**
        * Symbian OS constructor.
        */
        void ConstructL();

    private:
        // Object to be used for calling
        MNcnTimer& iObserver;
    };


#endif      // __MCEEMAILEDITORRESETTIMER_H__

// End of File
