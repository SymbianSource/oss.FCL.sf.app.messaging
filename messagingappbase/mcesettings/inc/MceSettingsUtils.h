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
*     Offers utility functions.
*
*/



#ifndef MCESETTINGSUTILS_H
#define MCESETTINGSUTILS_H

// INCLUDES

#include <msvstd.h>     // TMsvId
#include <msvstd.hrh>   // KMsvNullIndexEntryIdValue
#include <msvreg.h>     // THumanReadableName
#include <ImumInHealthServices.h>

// CLASS DECLARATION

/**
* Helper class
* Includes static functions.
*/
class MceSettingsUtils
    {
    public:        

        /**
        * Returns ETrue if 'Memory in use' option is enabled in mce/settings               
        */
        static TBool MemoryInUseOptionL( );        

        /**
        * Returns ETrue if new mail indicator setting supported               
        */
        static TBool NewMailIndicatorL( );

        /**
        * Checks if csp is supported      
        */
        static TBool CspBitsL( );
        
        /**
        * Gets the list of valid mailboxes
        */
        static TInt GetHealthyMailboxListL( 
            const MImumInHealthServices& aHealthServices,
            MImumInHealthServices::RMailboxIdArray& aMailboxIdArray,                                          
            TBool aGetImap4,
            TBool aGetPop3,
            TBool aGetSyncML,
            TBool aGetOther );
    };


#endif

// End of file
