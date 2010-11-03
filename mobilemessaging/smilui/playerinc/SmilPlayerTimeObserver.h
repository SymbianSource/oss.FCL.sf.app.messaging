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
* Description: SmilPlayerTimeObserver  declaration
*
*/



#ifndef SMILPLAYERTIMEOBSERVER_H
#define SMILPLAYERTIMEOBSERVER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// FORWARD DECLARATIONS

// DATA TYPES

// CLASS DECLARATION

/**
* This interface class is implemented by time indicator timer observer.
*
* @lib smilplayer.lib
* @since 3.0
*/
NONSHARABLE_CLASS(MSmilPlayerTimeObserver)
    {
    public:
        
        /**
        * Called by timer when time string has changed.
        *
        * @since 3.0
        *
        * @param aTimeString IN Changed time string.
        */
        virtual void TimeChangedL( const TDesC& aTimeString ) = 0;
        
        /**
        * Called by timer to retrieve current time.
        *
        * @since 3.0
        *
        * @return Current time on milliseconds.
        */
        virtual TInt CurrentTime() const = 0;
        
        /**
        * Called by timer to retrieve presentation duration.
        *
        * @since 3.0
        *
        * @return Presentation duration on milliseconds.
        */
        virtual TInt PresentationDuration() const = 0;
        
        /**
        * Called by timer check if presentation duration is finite.
        *
        * @since 3.0
        *
        * @return ETrue is duration is finite and EFalse if it is infinite.
        */
        virtual TBool IsDurationFinite() const = 0;
    };


#endif  // SMILPLAYERTIMEOBSERVER_H
            
// End of File  
