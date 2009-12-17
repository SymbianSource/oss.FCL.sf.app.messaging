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
*       Class declaration file
*
*/

#ifndef __SMTPCOPYMOVEOP_H__
#define __SMTPCOPYMOVEOP_H__

#include <ImumInternalApi.h>        // CImumInternalApi

#include "ImumOnlineOperation.h"
#include "SelectionOperationBase.h"
#include <smtpset.h>
class CMsvEmailConnectionProgressProvider;
class CMtmStore;

/**
* SMTP Sending operation
*/
class CSmtpCopyMoveOp :
    public CImumOnlineOperation
    {
    public:
        /**
        *
        */
        static CSmtpCopyMoveOp* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aTarget,
            const CMsvEntrySelection& aSel,
            TBool aMoving=EFalse );

        /**
        *
        */
        virtual ~CSmtpCopyMoveOp();

        /**
        *
        */
        virtual const TDesC8& ProgressL();

        /**
        *
        */
        virtual const TDesC8& GetErrorProgressL( TInt aError );

    protected:

        /**
        * From CActive
        */
        virtual void DoRunL();

    private:
        /**
        *
        */
        CSmtpCopyMoveOp(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aTarget,
            TBool aMoving );

        /**
        *
        */
        void ConstructL( const CMsvEntrySelection& aSel );

        /**
        *
        */
        TBool ValidateL( CImumInSettingsData& aSettings );

        /**
        * Prepares the operation to be ready for the sending
        * @since S60 3.0
        */
        void InitiateSendingOperationL();

        /**
        *
        * @since S60 3.0
        */
        TInt GetProgressErrorL();

        /**
        *
        * @since S60 3.0
        */
        TBool ContinueSendingL();

    private: // Data
        CMsvEntry*          iEntry;
        TMsvId              iTarget;
        CMsvEntrySelection* iSelection;
        CMsvEmailConnectionProgressProvider* iProgProvider;
        TPckgBuf<TImSmtpProgress> iProgress;
        TBool               iMoving;
        TBool               iValidIap;
        TInt                iLoginRetryCounter;
    };


#endif
