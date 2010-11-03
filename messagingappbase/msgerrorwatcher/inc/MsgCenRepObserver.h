/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     CMsgCenRepObserver declaration file
*
*/



#ifndef CMSGCENREPOBSERVER_H
#define CMSGCENREPOBSERVER_H

//  INCLUDES
#include    <e32base.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CRepository;
// CLASS DECLARATION

 

/**
* Class for observing CenRep if MMS Access Point in use changes
*/
class CMsgCenRepObserver : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aRConnection opened RConnection
        */
        static CMsgCenRepObserver* NewL(CMsgErrorWatcher* aWatcher);

        /**
        * Destructor.
        */
        virtual ~CMsgCenRepObserver();

    public: // New functions

        /**
        * Subscribe to a notification from CenRep
        */
        void SubscribeNotification();

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

        /**
        * Active object cancellation
        */
        void DoCancel();
        
        /**
        * If RunL leaves
        */
        TInt RunError( TInt aError );

    private:

        /**
        * C++ default constructor.
        */
        CMsgCenRepObserver( CMsgErrorWatcher* aWatcher );

        /**
        * By default Symbian OS constructor is private.
        * @param aRConnection opened RConnection
        */
        void ConstructL();

        /**
        * Completion
        */
        void RunL();

        // By default, prohibit copy constructor
        CMsgCenRepObserver( const CMsgCenRepObserver& );

        // Prohibit assignment operator
        CMsgCenRepObserver& operator= ( const CMsgCenRepObserver& );

    private:    // Data

        CMsgErrorWatcher*          iWatcher;
        CRepository*               iRepository;
        TUid                       iRepositoryUid;
        TInt32                     iKey;

    };

#endif      // CMSGCENREPOBSERVER_H

// End of File
