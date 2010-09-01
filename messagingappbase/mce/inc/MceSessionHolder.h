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
*     Keeps track on how many are using the same session to msgs.
*     When no one is using the session then the object of this class is deleted.
*
*/



#ifndef MCESESSIONHOLDER_H
#define MCESESSIONHOLDER_H

//  INCLUDES

#include <bldvariant.hrh>

// FORWARD DECLARATIONS
class CMsvSession;

// CLASS DECLARATION

/**
*  Keeps track on how many are using the same session to msgs.
*  When no one is using the session then the object of this class is deleted.
*/
class CMceSessionHolder :
    public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Creates session holder and returns pointer to created object.
        * @param aSessionObserver: reference to session observer
        */
        static CMceSessionHolder* NewL( MMsvSessionObserver& aSessionObserver );

        /**
        * Destructor.
        */
        virtual ~CMceSessionHolder();


    public:
        /**
        * New object starts usign session so number of session users
        * is incremented by 1
        */
        void AddClient();

        /**
        * Number of session users is decremented by 1. If no one is using the 
        * session after decrement delete session and this object.
        */
        void RemoveClient();

        /**
        * Returns number of session users.
        * @return number of session users.
        */
        TInt ClientCount() const;

        /* 
        * @return pointer to session object
        */
        CMsvSession* Session() const;

        /* 
        * Sets new session pointer
        *  @param aSession                 session pointer       
        */
        void SetSession( CMsvSession* aSession );

    private:
        /**
        * Constructor.
        */
        CMceSessionHolder();

        /**
        * Symbian OS Constructor.
        */
        void ConstructL( MMsvSessionObserver& aSessionObserver );



    private:    // Data
        CMsvSession*    iSession;
        TInt            iClientCount;
    };

#endif

// End of file
