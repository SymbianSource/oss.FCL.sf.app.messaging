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
* Description: Handles OMA Email Notification messages.
*
*/


#ifndef ALWAYSONLINEEMAILEMNRESOLVER_H
#define ALWAYSONLINEEMAILEMNRESOLVER_H

// INCLUDES
#include <e32base.h>
#include "AlwaysOnlineEmailAgentBase.h"

/**
*  This class handles OMA Email Notification messages.
*
*  @since S60 v3.1
*/
class CAlwaysOnlineEmailEMNResolver : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Creates object from CAlwaysOnlineEmailEMNResolver and leaves it to 
        * cleanup stack
        * @since S60 v3.1
        * @return, Constructed object
        */
        static CAlwaysOnlineEmailEMNResolver* NewLC();

        /**
        * Creates object from CAlwaysOnlineEmailEMNResolver
        * @since S60 v3.1
        * @return, Constructed object
        */
        static CAlwaysOnlineEmailEMNResolver* NewL();

        /**
        * Destructor
        * @since S60 v3.1
        */
        virtual ~CAlwaysOnlineEmailEMNResolver();

    public: // New functions
    
        /**
        * @since S60 v3.1
        * @return mailbox agent, NULL if not found
        */
        CAlwaysOnlineEmailAgentBase* FindEMNMailbox( TDesC& aMailboxURI, 
            CAOEmailAgentArray& aMailAgentArray );
            
        /**
        * Dispatches TEMNElement struct
        * @since S60 v3.1
        * @param aParameters Received mailbox URI and timestamp
        * @param aElement Dispatched mailbox URI and timestamp
        * @return KErrNone, if dispatching is ok
        */
        TInt ParameterDispatchTEMNElement( 
            const TDesC8& aParameters,
            TEMNElement& aElement ) const;

    protected:  // Constructors

        /**
        * Default constructor for class CAlwaysOnlineEmailEMNResolver
        * @since S60 v3.1
        * @return, Constructed object
        */
        CAlwaysOnlineEmailEMNResolver();

        /**
        * Symbian 2-phase constructor
        * @since S60 v3.1
        */
        void ConstructL();

    private:  // New functions
        
        /**
        * Checks whether the given username and server is found from given URI
        * @since S60 v3.1
        * @param aURI, mailbox URI
        * @param aUsername
        * @param aServer
        * @return TBool, if aUsername and aServer is found from aURI
        */
        TBool HandleEmnImapUserURI(
            const TDesC& aURI,
            const TDesC8& aUsername,
            const TDesC& aServer ) const;

        /**
        * Checks whether the given username and server is found from given URI
        * @since S60 v3.1
        * @param aURI, mailbox URI
        * @param aUsername
        * @param aServer
        * @return TBool, if aUsername and aServer is found from aURI
        */
        TBool HandleEmnPopUserURI(
            const TDesC& aURI,
            const TDesC8& aUsername,
            const TDesC& aServer ) const;

        /**
        * Checks whether the given username and server is found from given URI
        * @since S60 v3.1
        * @param aURI, mailbox URI
        * @param aUsername
        * @param aServer
        * @return TBool, if aUsername and aServer is found from aURI
        */
        TBool HandleEmnPopNoAuthURI( 
            const TDesC& aURI,
            const TDesC8& aUsername,
            const TDesC& aServer ) const;
            
    };

#endif //  ALWAYSONLINEEMAILEMNRESOLVER_H
