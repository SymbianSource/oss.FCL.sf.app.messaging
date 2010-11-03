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
* Description:  ImumDomainApiImpl.h
 *
*/


#ifndef C_IMUMDOMAINAPIIMPL_H
#define C_IMUMDOMAINAPIIMPL_H

#include "ImumDomainApi.h"
#include "ImumInternalApi.h"

// FORWARD DECLARATIONS
class CImumDaMailboxUtilitiesImpl;
class CImumDaMailboxServicesImpl;


NONSHARABLE_CLASS( CImumDomainApiImpl ) : public CImumDomainApi
    {
    public:  // Constructors and destructor

        /**
         * Create object from CImumDomainApiImpl
         *
         * @since S60 3.2
         * @param aMsvSession, session pointer.
         * @return, Constructed object
         */
        static CImumDomainApiImpl* NewL( CMsvSession* aMsvSession );

        /**
         * Creates object from CImumDomainApiImpl and leaves
         * it to cleanup stack
         *
         * @since S60 3.2
         * @param aMsvSession, session pointer.
         * @return, Constructed object
         */
        static CImumDomainApiImpl* NewLC( CMsvSession* aMsvSession );

        /**
        * Destructor
        * @since S60 3.2
        */
        virtual ~CImumDomainApiImpl();

    private:  // Constructors

        /**
         * Default constructor for CImumDomainApiImpl
         *
         * @since S60 3.2
         * @return, Constructed object
         */
        CImumDomainApiImpl();

        /**
         * Symbian 2-phase constructor
         * @since S60 3.2
         */
        void ConstructL( CMsvSession* aMsvSession );

    public:   // From class CImumDomainApi

        /**
         * Services to control mailboxes.
         *
         * @since S60 v3.2
         * @return Reference to mailbox services.
         */
        MImumDaMailboxServices& MailboxServicesL();

        /**
         * Utilities for using mailboxes.
         *
         * @since S60 v3.2
         * @return Reference to mailbox utilities.
         */
        MImumDaMailboxUtilities& MailboxUtilitiesL();

    private:

        // Owned: Pointer to internal api
        CImumInternalApi* iApi;
        // Owned: Pointer to domain api utilities
        CImumDaMailboxUtilitiesImpl* iDaMailboxUtilities;
        // Owned: Pointer to domain api services
        CImumDaMailboxServicesImpl* iDaMailboxServices;
    };



#endif /* C_IMUMDOMAINAPIIMPL_H */
