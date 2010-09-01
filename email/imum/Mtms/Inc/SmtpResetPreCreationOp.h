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
*       Used to reset a service's pre-created messages when the service changes
*
*/

#ifndef __SMTPRESETPRECREATIONOP_H__
#define __SMTPRESETPRECREATIONOP_H__

#include "SmtpPreCreationOpBase.h"

//
class CMsvEmailConnectionProgressProvider;
class CMtmStore;


/**
* CSmtpResetPreCreationOp
*
* Used to reset a service's pre-created messages when the service changes.
*/
class CSmtpResetPreCreationOp : public CSmtpPreCreationOpBase
    {
    public:
        /**
        *
        */
        static CSmtpResetPreCreationOp* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aServiceId );

        ~CSmtpResetPreCreationOp();

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
        CSmtpResetPreCreationOp(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aServiceId );

        /**
        *
        */
        void ConstructL();

        /**
        *
        */
        void DeleteMessageL();

        /**
        *
        */
        void CreateMessageL();

    private: // Data
        enum TState { EStateIdle, EStateDelete, EStateCreate };
        TState iState;
    };



#endif
