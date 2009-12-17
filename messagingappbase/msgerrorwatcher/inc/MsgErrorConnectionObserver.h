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
*     CMsgErrorConnectionObserver declaration file
*
*/



#ifndef MSGERRORCONNECTIONOBSERVER_H
#define MSGERRORCONNECTIONOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <rconnmon.h>

// FORWARD DECLARATIONS
class CMsgErrorWatcher;

// CLASS DECLARATION

/**
* CMsgErrorConnectionObserver
*
* @lib msgerrorwatcher.dll
* @since 2.0
*/
class CMsgErrorConnectionObserver : public CBase,
                                    public MConnectionMonitorObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        static CMsgErrorConnectionObserver* NewL( CMsgErrorWatcher* aWatcher );
        
        /**
        * Destructor.
        */
        virtual ~CMsgErrorConnectionObserver();

        /**
        * Get number of open (packet data) connetions
        *
        * @return Number of open (packet data) connetions
        */
        TUint ConnectionsOpen();

        /**
        * Start listening to "connection deleted" events.
        */
        void StartL();

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
        CMsgErrorConnectionObserver( CMsgErrorWatcher* aWatcher );

    private:

        /**
        * From MConnectionMonitorObserver
        */
        virtual void EventL( const CConnMonEventBase &aConnMonEvent );

    private:    // Data

        RConnectionMonitor  iConnMon;
        CMsgErrorWatcher*   iWatcher;
        TBool               iStarted;
        TUint               iConnectionsOpenWhenStarted;
    };

#endif      // MSGERRORCONNECTIONOBSERVER_H
            
// End of File
