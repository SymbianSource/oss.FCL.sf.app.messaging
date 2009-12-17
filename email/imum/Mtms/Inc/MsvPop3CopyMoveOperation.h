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
*       Operation to copy or move messages
*
*/

#ifndef __MSVPOP3COPYMOVEOPERATION_H__
#define __MSVPOP3COPYMOVEOPERATION_H__

#include <pop3set.h>
#include <MsvPrgReporter.h>
#include "ImumDiskSpaceObserverOperation.h"

#include "ImumMtmLogging.h"

class CMsvEmailConnectionProgressProvider;

/**
* CMsvPop3CopyMoveOperation
* Operation to copy or move messages, supporting complete and incomplete messages.
*/
class CMsvPop3CopyMoveOperation :
    public CImumDiskSpaceObserverOperation
    {
    public:

        /**
        *
        */
        static CMsvPop3CopyMoveOperation* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aService, TMsvId aTarget,
            const CMsvEntrySelection& aSel,
            TBool aMoveMsgs=EFalse );

        /**
        *
        */
        virtual ~CMsvPop3CopyMoveOperation();

        /**
        *
        */
        virtual const TDesC8& GetErrorProgressL( TInt aError );

    protected:

        /**
        * From CActive
        */
        virtual void DoRunL();

        /**
        *
        */
        void SortMessageSelectionL( const CMsvEntrySelection& aSel );

        /**
        *
        */
        void DoLocalCopyMoveL();

        /**
        *
        */
        void DoRemoteFetchL();
    private:

        /**
        *
        */
        CMsvPop3CopyMoveOperation(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aService,
            TMsvId aTarget,
            TBool aMoveMsgs );

        /**
        *
        */
        void ConstructL( const CMsvEntrySelection& aSel );

    private: // Data
        enum TState { EIdle, ELocalMsgs, ERemoteMsgs };
        TState iState;
        TBool iMoving;
        TMsvId iTarget;
        CMsvEntrySelection* iLocalSel;      // Complete messages
        CMsvEntrySelection* iRemoteSel;     // Incomplete messages to be fetched.
        TPckgBuf<TPop3Progress> iErrorProgress;
    };

#endif

// End of File
