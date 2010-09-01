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
*     Deletes messages first locally and then from server.
*
*
*/


#ifndef __CIMUMDELETEMESSAGESFROMSERVER_H__
#define __CIMUMDELETEMESSAGESFROMSERVER_H__

//  INCLUDES
#include <ImumInternalApi.h>            // CImumInternalApi
#include <MsvPrgReporter.h>     // MMsvProgressDecoder
#include "IMAPPRGR.H"           // TImap4UiProgressBuf

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
* class CImumDeleteMessagesFromServer
* Encapsulates delete locally operation and delete from server operation.
* First deletes message locally and after it has been completed, deletes from the server
* Decodes its own progress.
*/
class CImumDeleteMessagesFromServer :
    public CMsvOperation,
    public MMsvProgressDecoder
    {
    public:
        /**
        * Constructor.
        */
        static CImumDeleteMessagesFromServer* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            CMsvEntrySelection& aDeletedEntries );
        /**
        * Destructor.
        */
        virtual ~CImumDeleteMessagesFromServer();
        /**
        *
        */
        virtual const TDesC8& ProgressL();

        /**
        * From MMsvProgressDecoder
        */
        virtual TInt DecodeProgress (
            const TDesC8& aProgress,
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans,
            TBool aInternal );

    private:
        enum ImumDeleteMessagesState {
            EImumDeletingMessagesStateLocally = 0,
            EImumDeletingMessagesStateFromServer
            };

        /**
        * Constructor.
        */
        CImumDeleteMessagesFromServer(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus );

        /**
        * Constructor.
        */
        void ConstructL( CMsvEntrySelection& aDeletedEntries );
        /**
        * From CActive
        */
        void DoCancel();
        /**
        * From CActive
        */
        void RunL();

        /**
        * Starts new local delete progress
        */
        void StartNextDeleteLocally();

        /**
        * Creates object of CImumDeleteMessagesLocally
        */
        void MakeNextDeleteLocallyL();

        /**
        * Starts entry delete after local delete is completed.
        */
        void StartDeleteFromServer();

        /**
        * Creates delete operation
        */
        void MakeDeleteFromServerL();

    private:
        CImumInternalApi&       iMailboxApi;
        CMsvOperation*          iOperation;
        CMsvEntry*              iEntry;
        CMsvEntrySelection*     iEntrySelection; // includes entries to be deleted
        TInt                    iEntryCount;
        TBuf8<1>                iBlank;
        HBufC*                  iProgressText;
        ImumDeleteMessagesState iState;
        TImap4UiProgressBuf     iUiProgress;
        TBool                   iDeleteProgress;
        // File Session
        RFs                     iFileSession;
        // Currently active drive
        TInt                    iDriveNo;
    };


#endif      // __CIMUMDELETEMESSAGESFROMSERVER_H__

// End of File
