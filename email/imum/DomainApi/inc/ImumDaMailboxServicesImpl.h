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
* Description: ImumDaMailboxServicesImpl.h
 *
*/


#ifndef C_IMUMDAMAILBOXSERVICESIMPL_H
#define C_IMUMDAMAILBOXSERVICESIMPL_H

#include <msvstd.h>
#include "ImumDaMailboxServices.h"
#include "ImumInternalApi.h"

NONSHARABLE_CLASS( CImumDaMailboxServicesImpl ):
    public MImumDaMailboxServices
    {
    public:  // Constructors and destructor

        /**
         * Create object from CImumDaMailboxServicesImpl
         *
         * @since S60 3.2
         * @param aMailboxApi, Reference to internal api.
         * @return, Constructed object
         */
        static CImumDaMailboxServicesImpl* NewL( CImumInternalApi& aMailboxApi );

        /**
         * Creates object from CImumDaMailboxServicesImpl and leaves it to cleanup stack
         *
         * @since S60 3.2
         * @param aMailboxApi, Reference to internal api.
         * @return, Constructed object
         */
        static CImumDaMailboxServicesImpl* NewLC( CImumInternalApi& aMailboxApi );

        /**
         * Destructor
         * @since S60 3.2
         */
        virtual ~CImumDaMailboxServicesImpl();

    protected:  // Constructors

        /**
         * Default constructor for CImumDaMailboxServicesImpl
         *
         * @since S60 3.2
         * @param aMailboxApi, Reference to internal api.
         * @return, Constructed object
         */
        CImumDaMailboxServicesImpl( CImumInternalApi& aMailboxApi );

        /**
         * Symbian 2-phase constructor
         * @since S60 3.2
         */
        void ConstructL();

    public:  // From class MImumDaMailboxServices

        /**
         * Creates and prepares mailbox settings data object.
         * *** NOTE: Client is responsible to destroy the object after usage ***
         *
         * @since S60 v3.2
         * @param aProtocol, Any receiving protocol: Imap4 or Pop3.
         * @return Pointer to mailbox settings dataobject.
         * @leave EParamNotReceivingProtocol, If protocol receiving one.
         */
        CImumDaSettingsData* CreateSettingsDataL( const TUid& aProtocol ) const;

        /**
         * Validates the data and creates working mailbox to system.
         *
         * @since S60 v3.2
         * @param aSettingsData, Settings data object containing valid settings.
         */
        TMsvId CreateMailboxL( CImumDaSettingsData& aSettingsData ) const;

        /**
         * Sets the system's default mailbox (sending).
         *
         * @since S60 v3.2
         * @param aMailboxId, Id of the mailbox.
         * @leave EEntryNotFound, If the entry for the id can't be found.
         * @leave EEntryNotMailbox, If the id does not belong to mailbox.
         */
        void SetDefaultMailboxL( const TMsvId aMailboxId ) const;

    private:

        //Reference to internal Api
        CImumInternalApi &iMailboxApi;
        //not owned : shortcut to services
        MImumInMailboxServices* iServices;
};

#endif /* C_IMUMDAMAILBOXSERVICESIMPL_H */
