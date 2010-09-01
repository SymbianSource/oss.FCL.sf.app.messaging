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
*       Common email MTM operation for launching editors
*
*
*/


#ifndef __IMUMEDITOPERATION_H__
#define __IMUMEDITOPERATION_H__

#include <msvstd.h>
#include <MuiuMsgEditorLauncher.h>
#include <ImumInternalApi.h>        // CImumInternalApi
#include "PROGTYPE.H"

class CEikonEnv;
class CBaseMtmUi;
class CMsvEntrySelection;
class MMsvProgressReporter;

struct TEmailEditParams
    {
    TUint iPreferences;
    TFileName iEditorFileName;
    TEditorParameters iEditorParams;        // Includes message Id.
    };

/**
* class CImumEditOperation
*
* Common email MTM operation for launching editors. Encapsulates creating a
* receipt if required.
*/
class CImumEditOperation : public CMsvOperation
    {
    public:
        /**
        *
        */
        static CImumEditOperation* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            TUid aMtmType,
            const TEmailEditParams& aParams,
            TBool aAckReceipts);

        /**
        * Destructor
        */
        virtual ~CImumEditOperation();

        /**
        * From CMsvOperation
        */
        virtual const TDesC8& ProgressL();

    protected:
        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();

    private:
        /**
        * C++ constructor
        */
        CImumEditOperation(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            TUid aMtmType,
            const TEmailEditParams& aParams );

        /**
        * Symbian OS constructor
        */
        void ConstructL(TBool aSendReceipts);


        /**
        *
        */
        void DoLaunchEditorL();

    private:
        enum TEditState { EStateIdle, EStateMakeReceipt, EStateLaunching };
        TEditState iState;
        TEmailEditParams iEditParams;
        CMsvOperation* iOperation;
        CMsvEntry* iMsgEntry;
        TUid iActualMtm;
        TPckgBuf<TEmailEditProgress> iProgress;
    };

#endif
