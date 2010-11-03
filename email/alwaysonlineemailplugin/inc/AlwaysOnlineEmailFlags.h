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
*     Flags helper class for always online
*
*
*/


#ifndef ALWAYSONLINEEMAILFLAGS_H
#define ALWAYSONLINEEMAILFLAGS_H

//  INCLUDES

#include <e32def.h>



// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Helper class for flags.
*/
class TAlwaysOnlineEmailFlags
    {
    public:
        /**
        * constructor
        */
        TAlwaysOnlineEmailFlags();

        /**
        * Sets flag on
        * @param aFlag: flag to be set
        */
        inline void SetFlag( TUint32 aFlag );

        /**
        * Clear flag on
        * @param aFlag: flag to be cleared
        */
        inline void ClearFlag( TUint32 aFlag );

        /**
        * Query for the flag
        * @param aFlag: flag to be checked
        */
        inline TBool Flag( TUint32 aFlag ) const;

    private:
        TUint32 iFlags;

    };

#include "AlwaysOnlineEmailFlags.inl"

#endif

// End of file
