/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides the interface to handle mailbox verifying functions
*
*/


#ifndef M_IMUMINHEALTHSERVICES_H
#define M_IMUMINHEALTHSERVICES_H

// INCLUDES
#include <msvstd.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class TMuiuFlags;
class MImumInHealthServices;

/*
* Tools API for the client
* @lib ImumUtils.lib
* @since S60 3.0
*/
class MImumInHealthServices
    {
    public:
    
        /** Array of mailbox id's */
        typedef RArray<TMsvId> RMailboxIdArray;
    
        /** Flags to control the data collection */
        enum TImumInMboxListRequest
            {
            // FLAGS (to set flags individually)
            
            // Includes protocol entries into array
            EFlagIncludeImap4           = 0x01,
            EFlagIncludePop3            = 0x02,
            EFlagIncludeSmtp            = 0x04,                            
            EFlagIncludeSyncMl          = 0x08, // Deprecated (inactive)
            EFlagIncludeOther           = 0x20,
            // Make a list of healthy mailboxes (on) or unhealthy (off)
            EFlagGetHealthy             = 0x10,
            
            // FLAG SETS (for initialization)
            
            // Includes healthy receiving protocols
            EFlagSetHealthyReceiving    = 0x33,
            // All flags set on
            EFlagSetAll                 = 0x37
            };
    
    public:
    
        /**
        * Returns array of fully operating mailboxes
        * @since S60 3.0
        * @param aMailboxArray Return parameter for array of mailboxes
        * @param aFlags Use TImumInMboxListRequest to include the MTMs
        * @param aClearArray By default, the array is cleared before use
        * @return KErrNone, when array has been constructed successfully
        */
        virtual TInt GetMailboxList(
            RMailboxIdArray& aMailboxArray,
            const TInt64 aFlags = EFlagSetHealthyReceiving,
            const TBool aClearArray = ETrue ) const = 0;

        // @deprecated
        virtual TInt GetHealthyMailboxList(
            RMailboxIdArray& aMailboxArray,
            const TInt64 aFlags = EFlagSetHealthyReceiving,
            const TBool aClearArray = ETrue ) const = 0;

        /**
        * Verifies the mailbox condition
        * @since S60 3.0
        * @param aMailboxId The Id of the mailbox for the check
        * @return ETrue/EFalse according to condition
        */
        virtual TBool IsMailboxHealthy(
            const TMsvId aMailboxId ) const = 0;
                            
        /**
        * Searches and destroys unhealthy mailboxes
        * @since S60 3.0
        */
        virtual void CleanupUnhealthyMailboxes() const = 0;    
    };

#endif /* M_IMUMINHEALTHSERVICES_H */
