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
* Description:  Interface class of settings data collection
 *
*/

  
#ifndef M_IMUMDAMAILBOXUTILITIES_H
#define M_IMUMDAMAILBOXUTILITIES_H 

#include <msvstd.h>                     // TMsvId

/**
 * Utility class for mailboxes.
 *
 * This class provides methods to do minor tasks with mailboxes and 
 * their properties.
 *
 * @lib imumda.lib
 * @since S60 v3.2
 * @see from ImumDaErrorCodes.h enum TImumDaErrorCode for error codes
 */
NONSHARABLE_CLASS( MImumDaMailboxUtilities )
    {
    public:
    
        /** Flags to control mailboxes to be retrieved */
        enum TImumDaMailboxFlags
            {
            // Flags
            EFlagImap4      = 0x01,
            EFlagPop3       = 0x02,
            EFlagSmtp       = 0x04,
            EFlagSyncMl     = 0x08, // Deprecated since TB9.1
            // Default sets
            EFlagSetDefault = 0x0B,
            EFlagSetImapPop = 0x03
            };
        
        /** Request types of the mailbox entry */
        enum TImumDaMboxRequestType
            {
            EValueRequestCurrent = 0,
            EValueRequestSending,
            EValueRequestReceiving
            };
   
        /** Array of mailbox id's. */
        typedef RArray<TMsvId> RMailboxIdArray;
        
        /** Array of mailbox entries */
        typedef RArray<TMsvEntry> RMsvEntryArray;
        
                
        /**
         * Retrieves the id (sending) of the default mailbox in the system.
         *
         * @since S60 v3.2         
         * @return Id of the default mailbox
         */
        virtual TMsvId DefaultMailboxId() const = 0;
    
        /**
         * Checks if the provided mtm belongs to supported email mtm.
         *
         * @since S60 v3.2
         * @param aMtm, MTM type to be tested.
         * @param aAllowExtended, includes additional the protocols.
         * @return 
         */
        virtual TBool IsMailMtm( 
            const TUid& aMtm, 
            const TBool& aAllowExtended = EFalse ) const = 0;
        
        /**
         * Checks if the provided id/entry is mailbox.
         *
         * @since S60 v3.2
         * @param aMailboxId, Id to be tested.
         * @param aMailbox, Entry to be tested.
         * @return ETrue, if the entry belongs to mailbox.
         * @return EFalse, in failures or if the entry doesn't belong to mailbox.
         */
        virtual TBool IsMailbox( 
            const TMsvId aMailboxId ) const = 0;

        virtual TBool IsMailbox( 
            const TMsvEntry& aMailbox ) const = 0;

        /**
         * Gets the entry of the id and and checks that it is mailbox entry.
         *
         * @since S60 v3.2
         * @param aMailboxId, Id of the mailbox.
         * @param aType, Type of entry to be retrieved.
         * @return Entry of the mailbox.
         * @leave EEntryNotFound, if entry does not exist.
         * @leave EEntryNotMailbox, if the entry does not belong to mailbox.
         */
        virtual TMsvEntry GetMailboxEntryL( 
            const TMsvId aMailboxId, 
            const TImumDaMboxRequestType& aType = 
                EValueRequestCurrent ) const = 0;

        /**
         * Gets all the entries related to mailbox.
         *
         * @since S60 v3.2
         * @param aMailboxId, Id of the mailbox.
         * @return Mtm of the receiving entry.
         */
        virtual const TUid& GetMailboxEntriesL( 
            const TMsvId aMailboxId, 
            RMsvEntryArray& aEntries, 
            const TBool aResetArray = ETrue ) const = 0;

        /**
         * Checks, if the mailbox of specific id is functional.
         *
         * @since S60 v3.2
         * @param aMailboxId, Id of the mailbox to be checkd
         * @return ETrue, when mailbox is functional.
         * @return EFalse, when mailbox is faulty.
         */
        virtual TBool IsMailboxHealthy( const TMsvId aMailboxId ) const = 0;

        /**
         * Returns an array of availble mailboxes in the system.
         * *** NOTE: Client is responsible to clean the array after usage! ***
         *
         * @since S60 v3.2
         * @param aTypes, Type of the mailboxes
         * @return Array of id's of required mailboxes.
         */
        virtual RMailboxIdArray GetMailboxesL(
            const TInt64 aFlags ) const = 0;
    };



#endif /* M_IMUMDAMAILBOXUTILITIES_H */
