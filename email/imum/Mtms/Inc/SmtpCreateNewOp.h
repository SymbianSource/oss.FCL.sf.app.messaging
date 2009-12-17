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
*       SMTP Create New Email operation
*
*/

#ifndef __SMTPCREATENEWOP_H__
#define __SMTPCREATENEWOP_H__

#include <MuiuMsgEditorLauncher.h>
#include "SmtpPreCreationOpBase.h"

class CMsvEmailConnectionProgressProvider;
class CMtmStore;


/**
* SMTP Create New Email operation
*/
class CSmtpCreateNewOp : public CSmtpPreCreationOpBase
    {
    public:

        /**
        *
        */
        static CSmtpCreateNewOp* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            const TEditorParameters& aParams,
            const TDesC& aAppName,
            const TDesC8& aEditorProgress );

        /**
        *
        */
        virtual ~CSmtpCreateNewOp();

    protected:

        /**
        * From CActive
        */
        virtual void RunL();

        /**
        * From CActive
        */
        void DoRunL();

        /**
        * From CActive
        */        
        virtual TInt RunError(TInt aError);

    private:

        /**
        *
        */
        CSmtpCreateNewOp(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            const TEditorParameters& aParams,
            const TDesC& aAppName );

        /**
        *
        */
        void ConstructL( const TDesC8& aEditorProgress );

        /**
        *
        */
        void LaunchEditorL();

        /**
        *
        */
        void WaitL();

        /**
        *
        */
        void CreateMessageL( TBool aLaunchEditorAfterCreate = EFalse);

    private: // Data
        enum TState {
            EStateIdle,
            EStateDeleteAll,
            EStateCreateNew,
            EStateLaunch,
            EStateWait,
            EStateCreate };
        
        TState iState;
        TEditorParameters iParams;
        TFileName iAppName;
        HBufC8* iEditorProgress;
        TMsvId iService;
    };


#endif
