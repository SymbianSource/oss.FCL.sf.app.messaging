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
* Description: SmilPlayerPauseObserver  declaration
*
*/



#ifndef SMILPLAYERPAUSEOBSERVER_H
#define SMILPLAYERPAUSEOBSERVER_H

//  INCLUDES

// CONSTANTS

// FORWARD DECLARATIONS

// DATA TYPES

// CLASS DECLARATION

/**
* This interface class is implemented by pause indicator timer observer.
*
* @lib smilplayer.lib
* @since 3.0
*/
NONSHARABLE_CLASS(MSmilPlayerPauseObserver)
    {
    public:
        
        /**
        * Called by timer when pause indicator should be toggled.
        *
        * @since 3.0
        */
        virtual void TogglePauseIndicator() const = 0;
    };


#endif  // SMILPLAYERPAUSEOBSERVER_H
            
// End of File  
