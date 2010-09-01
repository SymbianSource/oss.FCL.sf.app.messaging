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
*       Copy or move a selection of messages that may or may not be complete
*
*/

#ifndef __IMAP4COPYMOVELOCALOP_H__
#define __IMAP4COPYMOVELOCALOP_H__

#include <imapset.h>
#include <imapcmds.h>
#include "IMAPPRGR.H"
#include "ImumDiskSpaceObserverOperation.h"
#include "Imap4MtmUi.hrh" // KImpuIMAPFolderNameLength

/**
* Copy or move a selection of messages that may or may not be complete.
* Always fetches entire messages.
*/
class CImap4CopyMoveLocalOp :
    public CImumDiskSpaceObserverOperation
    {
    public:
        /**
        *
        * @param aSession: Server session.
        * @param aStatus: Observer completion status.
        * @param aReporter: Progress reporter.
        * @param aSel: The message ids to get.
        * @param aTargetId: Where to fetch the messages to.
        * @param aCopy: Copying or moving.
        */
        static CImap4CopyMoveLocalOp* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aStatus,
            MMsvProgressReporter& aReporter,
            const CMsvEntrySelection& aSel,
            TMsvId aTargetId,
            TBool aCopy);

        /**
        *
        */
        virtual ~CImap4CopyMoveLocalOp();

        /**
        *
        */
        virtual const TDesC8& GetErrorProgressL(TInt aError);

    protected:

        /**
        *
        */
        virtual void DoRunL();
    private:

        /**
        * for explanation of parameters, see NewL
        */
        CImap4CopyMoveLocalOp(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aTargetId,
            TBool aCopy);

        /**
        *
        */
        void ConstructL(const CMsvEntrySelection& aSel);

        /**
        *
        * Sort messages into complete and incomplete.
        */
        void SortMessageSelectionL(const CMsvEntrySelection& aSel);

        /**
        *
        */
        void DoLocalCopyMoveL();

        /**
        *
        */
        void DoRemoteFetchL();

    private:
        enum TState { EIdle, ELocalMsgs, ERemoteMsgs };
        TState iState;
        const TBool iCopying;
        const TMsvId iTarget;
        CMsvEntrySelection* iLocalSel;      // Complete messages
        CMsvEntrySelection* iRemoteSel;     // Incomplete messages to be fetched.
        TPckgBuf<TImap4CompoundProgress> iProgressBuf;
    };


#endif

// End of File
