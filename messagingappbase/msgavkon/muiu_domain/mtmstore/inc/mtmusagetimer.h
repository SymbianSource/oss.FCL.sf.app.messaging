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
* Description: mtmusagetimer  declaration
*
*/



#if !defined(__MTMUSAGETIMER__)
#define __MTMUSAGETIMER__

//  INCLUDES
#ifndef __E32BASE_H__
#include <e32base.h>
#endif // __E32BASE_H__

// FORWARD DECLARATIONS
class MMtmUsageTimerObserver;


// CLASS DECLARATION

/**
* CMtmUsageTimer
*/
class CMtmUsageTimer: public CTimer
    {
    public:
        /**
        * Two phase constuctor
        */
        static CMtmUsageTimer* NewLC( MMtmUsageTimerObserver& aObserver, TUid aMtm );

        /**
        * 
        */ 
        void Start();

    private:
        /**
        * C++ constructor
        */
        CMtmUsageTimer( MMtmUsageTimerObserver& aObserver, TUid aMtm );

        /**
        * From CActive
        */
        virtual void RunL();

    private:
        MMtmUsageTimerObserver& iObserver;
        TUid                    iMtm;
    };

#endif // __MTMUSAGETIMER__

// End of File
