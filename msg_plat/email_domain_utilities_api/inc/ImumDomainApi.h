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

 
#ifndef C_IMUMDOMAINAPI_H
#define C_IMUMDOMAINAPI_H 

#include <msvapi.h>                     // CMsvSession

// FORWARD DECLARATIONS
class CImumDomainApi;
class CMsvSession;
class MImumDaMailboxServices;
class MImumDaMailboxUtilities;

/**
 * Function to create the Domain API.
 * *** NOTE: Client is responsible to destroy the object after usage ***
 *
 * @since S60 v3.2
 * @return Pointer to Domain API object.
 */
IMPORT_C CImumDomainApi* CreateDomainApiL( CMsvSession* aMsvSession = NULL );
IMPORT_C CImumDomainApi* CreateDomainApiLC( CMsvSession* aMsvSession = NULL );

/**
 * Base factory class of mailbox services.
 *
 * This is the main class of the mailbox Domain API services for the clients.
 * 
 * Quick guide to use the Domain API:
 * @code
 *  // Create Domain API
 *  CImumDomainApi* domainApi = CreateDomainApiLC();
 *  // For mailbox creation
 *  MImumDaMailboxServices* services = &domainApi->MailboxServicesL();
 *  // For Utilities
 *  MImumDaMailboxUtilities* utils = &domainApi->MailboxUtilitiesL();
 *  
 *  // Your code...
 *
 *  // Remove the object
 *  CleanupStack::PopAndDestroy( domainApi );
 *  domainApi = NULL; 
 * @endcode
 *
 * @lib imumda.lib
 * @since S60 v3.2
 * @see from ImumDaErrorCodes.h enum TImumDaErrorCode for error codes
 */
NONSHARABLE_CLASS( CImumDomainApi ) : public CBase
    {
    public:
    
        /**
         * Services to control mailboxes.
         *
         * @since S60 v3.2
         * @return Reference to mailbox services.
         */
        virtual MImumDaMailboxServices& MailboxServicesL() = 0;

        /**
         * Utilities for using mailboxes.
         *
         * @since S60 v3.2
         * @return Reference to mailbox utilities.
         */
        virtual MImumDaMailboxUtilities& MailboxUtilitiesL() = 0;
    };



#endif // C_IMUMDOMAINAPI_H
