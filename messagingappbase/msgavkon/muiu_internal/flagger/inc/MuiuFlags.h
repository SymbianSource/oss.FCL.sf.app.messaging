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
* Description:   This file contains TMuiuFlags class declaration
*
*/



#ifndef TMuiuFlag_H
#define TMuiuFlag_H

#include <e32base.h>

/**
* Helper class for flags.
*/
class TMuiuFlags 
    {
    public:
        /**
        * constructor
        */
        IMPORT_C TMuiuFlags( const TUint64 aFlags = 0 );

        /**
        * Sets flag on
        * @since Series60 3.0
        * @param aFlag: flag to be set
        */
        inline void SetFlag( const TUint aFlag );

        /**
        * Clear flag on
        * @since Series60 3.0
        * @param aFlag: flag to be cleared
        */
        inline void ClearFlag( const TUint aFlag );

        /**
        * Query for the flag
        * @since Series60 3.0
        * @param aFlag: flag to be checked
        */
        inline TBool Flag( const TUint aFlag ) const;
        
        /**
        * Modifies the state of the flag
        * @since Series60 3.0
        */
        inline void ChangeFlag( 
            const TUint aFlag,
            const TBool aNewState );
           
        /**
        * Copies the flags
        * @since Series60 3.0
        */    
        IMPORT_C TMuiuFlags& operator=( const TMuiuFlags& aFlags );
        
        /**
        * Copies the flags
        * @since Series60 3.0
        */    
        IMPORT_C TMuiuFlags& operator=( const TUint64 aFlags );

    private:
        TUint64 iFlags;
    };
    
#include <muiuflags.inl>

#endif

// End of File

