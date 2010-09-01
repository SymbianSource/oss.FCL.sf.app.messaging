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
* Description: SmilPlayerPauseIndicatorTimer  declaration
*
*/



#ifndef SMILPLAYERPAUSEINDICATORTIMER_H
#define SMILPLAYERPAUSEINDICATORTIMER_H

#include <e32base.h>

// FORWARD DECLARATIONS
class MSmilPlayerPauseObserver;

// DATA TYPES

// CLASS DECLARATION

// CLASS DECLARATION

/**
*  Class is used to indicate how long pause indicator is visible in screen.
*
* @lib smilplayer.lib
* @since 3.0
*/
NONSHARABLE_CLASS(CSmilPlayerPauseIndicatorTimer) : public CTimer
    {

    public: // Constructor and Destructor

        /**
        * Two-phased constructor.
        *
        * @param aPauseObserver Pause indicator timer observer..
        *
        * @return Pointer to created CSmilPlayerPauseIndicatorTimer object.
        */
        static CSmilPlayerPauseIndicatorTimer* NewL( MSmilPlayerPauseObserver* aPauseObserver  );
        
        /**
        * Destructor.
        */
        virtual ~CSmilPlayerPauseIndicatorTimer();

    public: // New functions

        /**
        * Starts the timer.
        *
        * @since 3.0
        */      
        void StartTimer();

    private: 

        /**
        * C++ default constructor.
        */
        CSmilPlayerPauseIndicatorTimer();
        
        /**
        * C++ constructor.
        */
        CSmilPlayerPauseIndicatorTimer( MSmilPlayerPauseObserver* aPauseObserver );
        
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected: // Functions from base classes
        
        /**
        * From CTimer 
        * See the documentation from the e32base.h
        */      
        void RunL();

    private: //

        // Observer
        MSmilPlayerPauseObserver* iPauseObserver;
    };        

#endif // SMILPLAYERPAUSEINDICATORTIMER_H

// End of File
