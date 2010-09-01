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
* Description: ImumDaMailboxUtilitiesImpl.h
 *
*/


#ifndef C_IMUMDAMAIBOXUTILITIESIMPL_H
#define C_IMUMDAMAIBOXUTILITIESIMPL_H

#include "ImumDaMailboxUtilities.h"
#include "ImumInternalApi.h"
#include "ImumInHealthServices.h"

class MImumInMailboxUtilities;

NONSHARABLE_CLASS( CImumDaMailboxUtilitiesImpl ): public MImumDaMailboxUtilities
    {
    public:  // Constructors and destructor

        /**
         * Create object from CImumDaMailboxUtilitiesImpl
         *
         * @since S60 3.2
         * @param aMailboxApi, Reference to internal api.
         * @return, Constructed object
         */
        static CImumDaMailboxUtilitiesImpl* NewL( CImumInternalApi& aMailboxApi );

        /**
         * Creates object from CImumDaMailboxUtilitiesImpl and leaves it to cleanup stack
         *
         * @since S60 3.2
         * @param aMailboxApi, Reference to internal api.
         * @return, Constructed object
         */
        static CImumDaMailboxUtilitiesImpl* NewLC( CImumInternalApi& aMailboxApi );

        /**
         * Destructor
         * @since S60 3.2
         */
         virtual ~CImumDaMailboxUtilitiesImpl();

    protected:  // Constructors

        /**
         * Default constructor for CImumDaMailboxUtilitiesImpl
         *
         * @since S60 3.2
         * @param aMailboxApi, Reference to internal api.
         * @return, Constructed object
         */
        CImumDaMailboxUtilitiesImpl( CImumInternalApi& aMailboxApi );

        /**
        * Symbian 2-phase constructor
        * @since S60 3.2
        */
        void ConstructL();

    public:  //From class MImumDaMailboxUtilities

        /**
         * Retrieves the id (sending) of the default mailbox in the system.
         *
         * @since S60 v3.2
         * @return Id of the default mailbox
         */
        TMsvId DefaultMailboxId() const;

        /**
         * Checks if the provided mtm belongs to supported email mtm.
         *
         * @since S60 v3.2
         * @param aMtm, MTM type to be tested.
         * @param aAllowExtended, includes additional protocols.
         * @return
         */
        TBool IsMailMtm(
            const TUid& aMtm,
            const TBool& aAllowExtended = EFalse ) const;

        /**
         * Checks if the provided id/entry is mailbox.
         *
         * @since S60 v3.2
         * @param aMailboxId, Id to be tested.
         * @param aMailbox, Entry to be tested.
         * @return ETrue, if the entry belongs to mailbox.
         * @return EFalse, in failures or if the entry doesn't belong to mailbox.
         */
        TBool IsMailbox(
            const TMsvId aMailboxId ) const;

        TBool IsMailbox(
            const TMsvEntry& aMailbox ) const;

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
        TMsvEntry GetMailboxEntryL(
            const TMsvId aMailboxId,
            const TImumDaMboxRequestType& aType = EValueRequestCurrent ) const;

        /**
         * Gets all the entries related to mailbox.
         *
         * @since S60 v3.2
         * @param aMailboxId, Id of the mailbox.
         * @return Mtm of the receiving entry.
         */
        const TUid& GetMailboxEntriesL(
            const TMsvId aMailboxId,
            RMsvEntryArray& aEntries,
            const TBool aResetArray = ETrue ) const;

        /**
         * Checks, if the mailbox of specific id is functional.
         *
         * @since S60 v3.2
         * @param aMailboxId, Id of the mailbox to be checkd
         * @return ETrue, when mailbox is functional.
         * @return EFalse, when mailbox is faulty.
         */
        virtual TBool IsMailboxHealthy( const TMsvId aMailboxId ) const;

        /**
         * Returns an array of availble mailboxes in the system.
         * *** NOTE: Client is responsible to clean the array after usage! ***
         *
         * @since S60 v3.2
         * @param aTypes, Type of the mailboxes
         * @return Array of id's of required mailboxes.
         */
        RMailboxIdArray GetMailboxesL(
            const TInt64 aFlags =
                MImumInHealthServices::EFlagGetHealthy ) const;

    private:

        //Reference to internal Api
        CImumInternalApi &iMailboxApi;
        //not owned : shortcut to utilities
        MImumInMailboxUtilities *iUtils;
    };

#endif /* C_IMUMDAMAIBOXUTILITIESIMPL_H */
