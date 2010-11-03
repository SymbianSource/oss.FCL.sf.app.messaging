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
* Description: 
*       Simple Timer Operation. Completes observer when time expires
*
*/

#ifndef __SMTPTIMEROP_H__
#define __SMTPTIMEROP_H__

#include <msvapi.h>

class CMsvEmailConnectionProgressProvider;
class CMtmStore;


/**
* Simple Timer Operation. Completes observer when time expires.
*/
class CSmtpTimerOp : public CMsvOperation
    {
    public:
        /**
        *
        */
        CSmtpTimerOp(
            CMsvSession& aMsvSession,
            TInt aPriority,
            TRequestStatus& aObserverRequestStatus );

        /**
        *
        */
        virtual ~CSmtpTimerOp();

        /**
        *
        */
        void ConstructL();

        /**
        *
        */
        virtual const TDesC8& ProgressL();

        /**
        *
        */
        void At(const TTime& aTime);

        /**
        *
        */
        void After(TTimeIntervalMicroSeconds32 aInterval);

    protected:

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();

    protected: // Data
        RTimer iTimer;
        TBuf8<1> iProgress;
    };



#endif
