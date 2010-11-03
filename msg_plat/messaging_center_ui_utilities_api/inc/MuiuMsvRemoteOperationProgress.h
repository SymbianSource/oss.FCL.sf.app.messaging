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
*     Messaging progress watching classes
*
*/




#ifndef __MUIUMSVREMOTEOPERATIONPROGRESS_H__
#define __MUIUMSVREMOTEOPERATIONPROGRESS_H__


//  INCLUDES
#include <e32base.h>
#include <msvstd.h>


// CONSTANTS
const TInt KRemoteOpProgressDefaultPeriod = 1000000; // default update period every 1 second


// FORWARD DECLARATIONS
class CMsvOperation;


// CLASS DECLARATION

/**
* Mixin class MMsvRemoteOperationProgressObserver.
* Object of CMsvRemoteOperationProgress will call
* function UpdateRemoteOpProgressL in every x microseconds.
*/
class MMsvRemoteOperationProgressObserver
    {
    public:
        virtual void UpdateRemoteOpProgressL() = 0;
    };



/**
* CMsvRemoteOperationProgress:
* Remote operation progress observer
*
* Simple timer which signals back to observer every x
* microseconds (x defined in NewL()) to update the
* progress.
*/
class CMsvRemoteOperationProgress : public CTimer
    {
    public:
        /**
        * Two-phased constructor.
        * @param aObserver: referent to progress observer
        * @param aPeriod: refress time of the progress dialog, recommended to use KRemoteOpProgressDefaultPeriod
        * @return: object of CMsvRemoteOperationProgress
        */
        IMPORT_C static CMsvRemoteOperationProgress* NewL(
            MMsvRemoteOperationProgressObserver& aObserver,
            TInt aPeriod=KRemoteOpProgressDefaultPeriod );

        /**
        * Two-phased constructor.
        * @param aObserver: referent to progress observer
        * @param aPriority: priority for the timer.
        * @param aPeriod: refress time of the progress dialog, recommended to use KRemoteOpProgressDefaultPeriod
        * @return: object of CMsvRemoteOperationProgress
        */
        IMPORT_C static CMsvRemoteOperationProgress* NewL(
            MMsvRemoteOperationProgressObserver& aObserver,
            TInt aPriority,
            TInt aPeriod );

        /*
        * Destructor
        */
        virtual ~CMsvRemoteOperationProgress();

    private:
        /**
        * C++ constructor
        */
        CMsvRemoteOperationProgress(
            MMsvRemoteOperationProgressObserver& aObserver,
            TInt aPriority,
            TInt aPeriod );

        /**
        * Symbian OS constructor
        */
        void ConstructL();

        /**
        * Internal.
        * Starts new wait.
        */
        void IssueRequest();

        /**
        * From CActive
        */
        void RunL();

    private:
        MMsvRemoteOperationProgressObserver&    iObserver;
        TInt                                    iPeriod;
    };

#endif // __CMSVREMOTEOPERATIONPROGRESS_H__

// End of file
