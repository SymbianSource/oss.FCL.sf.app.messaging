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
*     Reqisters Mce to LogEng to get notification about log DB changes.
*
*/



#ifndef __Mce_Engine_CMceLogNotify_H__
#define __Mce_Engine_CMceLogNotify_H__

//  INCLUDES

#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATIONS

class CLogClient;
class CMceLogEngine;

// CLASS DECLARATION

/**
 * Observer for the log database. Issues request to the underlying
 * asyncronous service provider, which tells if changes in the DB
 * had happened.
 */
class CMceLogNotify : public CActive

    {
    public:     // Constructors.
        /**
         * Standard creation function. Creates and returns a new object of this
         * class.
         *
         * @param aObserver pointer to observer.
         * @param aDelay notification delay in microseconds.
         * @return  pointer to CMceLogNotify instance.
         */
        static CMceLogNotify* NewL( CMceLogEngine* aObserver,
                                  TTimeIntervalMicroSeconds32 aDelay );

        /**
         *  Destructor.
         */
        ~CMceLogNotify();


        /**
         *  Issues request to asyncronous service provider
         */
        void IssueNotifyRequest();


    private:    // Constructions.
        /**
         * Constructor, second phase.
         */
        void ConstructL();

        /**
         * Default Constructor
         */
        CMceLogNotify();

        /**
         * Constructor
         *
         * @param aObserver pointer to observer.
         * @param aDelay notification delay in microseconds.
         */
        CMceLogNotify( CMceLogEngine* aObserver,
                     TTimeIntervalMicroSeconds32 aDelay);

    private:     // Operations from CActive
        /**
         * This is called when a request to a log engine is completed.
         */
        void RunL();

        /**
         * Cancels active request.
         */
        void DoCancel();

    private: // data
        /// Ref: Messaging Centre log engine.
        CMceLogEngine* iObserver;
        /// Own: File System session
        RFs iFsSession;
        /// Own: Pointer to the log client.
        CLogClient* iLogClient;
        /// Own: Notification delay in microseconds.
        TTimeIntervalMicroSeconds32 iDelay;

    };

#endif // __Mce_Engine_CMceLogNotify_H__

// End of file
