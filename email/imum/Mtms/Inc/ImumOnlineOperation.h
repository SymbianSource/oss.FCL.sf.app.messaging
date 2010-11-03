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
*       Common base class for email online operations
*
*/

#ifndef __IMUMONLINEOPERATION_H__
#define __IMUMONLINEOPERATION_H__

#include <msvstd.h>
#include <msvapi.h>
#include <eikenv.h>
#include <ImumInternalApi.h>            // CImumInternalApi
#include "PROGTYPE.H"

class CEikonEnv;
class CBaseMtmUi;
class CMsvEntrySelection;
class MMsvProgressReporter;

/**
* class CImumOnlineOperation
*
* Common base class for email online operations.
*/
class CImumOnlineOperation : public CMsvOperation
    {
    public:
        /**
        *
        */
        virtual ~CImumOnlineOperation();

        /**
        *
        */
        virtual const TDesC8& ProgressL();

        /**
        * For reporting if DoRunL leaves
        */
        virtual const TDesC8& GetErrorProgressL(TInt aError) = 0;

    protected:
        /**
        * C++ constructor
        */
        // Construction.
        CImumOnlineOperation(
            CImumInternalApi& aMailboxApi,
            TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter);

        /**
        * Base constructor
        */
        void BaseConstructL(TUid aMtmType);

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();

        /**
        * From CActive
        */
        virtual void DoRunL() = 0;

        /**
        * From CActive
        */
        virtual TInt RunError( TInt aError );

        /**
        * Completes observer with status aStatus
        * @param aStatus: Status of the operation.
        * Override if needed to complete with other status than KErrNone.
        */
        void CompleteObserver( TInt aStatus = KErrNone );

        /**
        *
        */
        void CompleteThis();

        /**
        * Sub-operation helpers
        */
        TInt GetOperationCompletionCodeL();

        /**
        *
        */
        void InvokeClientMtmAsyncFunctionL(
            TInt aFunctionId,
            TMsvId aEntryId,
            TMsvId aContextId,
            const TDesC8& aParams=KNullDesC8);

        /**
        *
        */
        void InvokeClientMtmAsyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSel,
            TMsvId aContextId,
            const TDesC8& aParams=KNullDesC8);

        /**
        * Progress reporter helpers.
        */
        void ReDisplayProgressL(TInt aTitleResourceId, TMsvId aServiceId);

        /**
        * Check free space from current drive
        * This function is used during RunL function, which is run after
        * a notification comes from fileserver.
        * @since S60 3.0
        * @return KErrNone, when everything is fine
        */
        virtual TInt DoDiskSpaceCheckL();

    protected:
        // Progress reporter mixin.
        MMsvProgressReporter& iReporter;
        // Internal API reference
        CImumInternalApi& iMailboxApi;
        // MTM UI. (Obtained from iReporter.MtmStore())
        CBaseMtmUi* iMtmUi;
        // The sub-operation.
        CMsvOperation* iOperation;
        // Status of current operation
        TInt iError;
        // Return this if iOperation==NULL.
        TBuf8<1> iDummyProg;
        // Uikon env for resource reading.
        CEikonEnv* iEikEnv;
    };


#endif
