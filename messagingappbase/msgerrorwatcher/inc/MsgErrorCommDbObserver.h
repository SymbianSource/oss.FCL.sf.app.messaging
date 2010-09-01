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
*     CMsgErrorCommDbObserver declaration file
*
*/



#ifndef MSGERRORCOMMDBOBSERVER_H
#define MSGERRORCOMMDBOBSERVER_H

// FORWARD DECLARATIONS
class CMsgErrorWatcher;
class CCommsDatabase;

// CLASS DECLARATION

/**
* CMsgErrorCommDbObserver
*
* @lib msgerrorwatcher.dll
* @since 2.0
*/
class CMsgErrorCommDbObserver : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        static CMsgErrorCommDbObserver* NewL( CMsgErrorWatcher* aWatcher );
        
        /**
        * Destructor.
        */
        virtual ~CMsgErrorCommDbObserver();

        /**
        * For external reactivating the active object.
        */
        void Restart();

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Private C++ constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        CMsgErrorCommDbObserver( CMsgErrorWatcher* aWatcher );

    private: // from CActive

        /**
        * For starting the active object.
        */
        void Start();

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();

    private:    // Data

        CCommsDatabase*     iDb;
        CMsgErrorWatcher*   iWatcher;
    };

#endif      // MSGERRORCOMMDBOBSERVER_H
            
// End of File
