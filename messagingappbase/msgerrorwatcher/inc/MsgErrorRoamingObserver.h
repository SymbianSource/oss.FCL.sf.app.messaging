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
*     CMsgErrorRoamingObserver declaration file
*
*/



#ifndef MSGERRORROAMINGOBSERVER_H
#define MSGERRORROAMINGOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <etel.h>                   //RTelServer
#include <etelmm.h>

//#include <etelbgsm.h>               //RBasicGsmPhone

// FORWARD DECLARATIONS
class CMsgErrorWatcher;

// CLASS DECLARATION

/**
* CMsgErrorRoamingObserver
*
* @lib msgerrorwatcher.dll
* @since 2.0
*/
class CMsgErrorRoamingObserver : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        static CMsgErrorRoamingObserver* NewL( CMsgErrorWatcher* aWatcher );
        
        /**
        * Destructor.
        */
        virtual ~CMsgErrorRoamingObserver();

    private:

        /**
        * Checks whether roaming status has changed
        * from home network to roaming or vice versa.
        * If so, calls CMsgErrorWatcher::HandleRoamingEventL
        */
        void UpdateRoamingStatusL();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Private C++ constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        CMsgErrorRoamingObserver( CMsgErrorWatcher* aWatcher );

    private:

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

        CMsgErrorWatcher*   iWatcher;
        TBool               iRoaming;

        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/

        RMobilePhone        iMobilePhone;
        RTelServer          iETelServer;
        RMobilePhone::TMobilePhoneRegistrationStatus iRegStatus;
        TInt                iRequestId;

    };

#endif      // MSGERRORROAMINGOBSERVER_H
            
// End of File
