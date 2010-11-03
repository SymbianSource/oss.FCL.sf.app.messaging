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
*     Email plugin class
*
*/


#ifndef __ALWAYSONLINEEMAILPLUGIN_H__
#define __ALWAYSONLINEEMAILPLUGIN_H__


#include <e32base.h>    // CBase
#include <msvstd.h>
#include <msvapi.h>
#include <AlwaysOnlineEComInterface.h>
#include <AlwaysOnlineManagerClient.h>
#include <MuiuMsvSingleOpWatcher.h>
#include "AlwaysOnlineEmailAgent.h"

//constants

/**
* class CAlwaysOnlineManagerServer;
*
*/
class CEComEmailPlugin : 
    public CAlwaysOnlineEComInterface, public MMsvSessionObserver
    {
    public:
        /**
        * NewL
        * Two phased constructor
        * @return CEComEmailPlugin*, self pointer
        */
        static CEComEmailPlugin* NewL();

        /**
        * ~CEComEmailPlugin
        * Destructor
        */
        virtual ~CEComEmailPlugin();

        /**
        * HandleServerCommandL
        * @param TInt, command Id
        * @param TDesC8*, parameter pack
        * @return TAny*, return value
        */
        virtual TAny* HandleServerCommandL( 
            TInt    aCommand, 
            TDesC8* aParameters );

        
    private:
        /**
        * CEComEmailPlugin
        */
        CEComEmailPlugin();

        /**
        * ConstructL
        * Second phase constructor
        */
        void ConstructL();

        /**
        * ActivateOnlineMailboxesL
        */
        void ActivateOnlineMailboxesL();

        /**
        * Turn mailbox on/off, depending on the parameter
        * @since Series60 2.6
        * @param aParameters Parameter to 
        * @param aSetState Turn mailbox on/off
        * @return KErrNone, if dispatching is ok
        */
        void HandleCmdMailboxL( 
            const TInt    aCommand,
            const TDesC8* aParameters );       

    public: //from MMsvSessionObserver

        /**
        * HandleSessionEventL
        * @param TMsvSessionEvent, session event
        * @param TAny* parameter 1
        * @param TAny* parameter 2
        * @param TAny* parameter 3
        */
        void HandleSessionEventL(
            TMsvSessionEvent    aEvent, 
            TAny*               aArg1, 
            TAny*               /*aArg2*/, 
            TAny*               /*aArg3*/);


        /**
        * static function for CPeriodic timer to start message server 
        * session after server termination
        * Calls LaunchSession function.
        * @since Series60 2.6
        * @param aSelf: pointer to CEComEmailPlugin object
        * @return error code
        */
        static TInt LaunchMsvSession( TAny* aSelf );

    private:

        /**
        * Tries to launch session and mail agents by calling 
        * ActivateOnlineMailboxesL function max 20 times.
        * If successfull deletes iSessionStarter
        * @since Series60 2.6
        * @return leave code trapped from function call ActivateOnlineMailboxesL.
        */
        TInt LaunchSession();

        /**
        * Deletes iSession and iEmailAgent and starts iSessionStarter
        * @since Series60 2.6
        */
        void HandleSessionTerminatedL();

        /**
        * Makes basic system health checks. Will initalize session and agent if 
        * they are not already initialized.
        * @since Series60 2.6
        */
        void VerifyPluginHealthL();

        //data
    private:
        CMsvSession*                iSession;
        CAlwaysOnlineEmailAgent*    iEmailAgent;
        TInt                        iError;
        TBool                       iPluginStarted;
        CPeriodic*                  iSessionStarter;
        TInt                        iLaunchCounter;
    };//CEComEmailPlugin

#endif
//EOF
