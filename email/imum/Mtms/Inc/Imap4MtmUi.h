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
*       CImap4MtmUi - IMAP4 MTM UI
*
*/

#ifndef __IMAP4MTMUI_H__
#define __IMAP4MTMUI_H__

#include <imapset.h>
#include <mtmuibas.h>
#include <eikdialg.h>
#include <MsvPrgReporter.h>
#include "ImumMtmBaseMtmUi.h"   // CImumMtmBaseMtmUi

// forward declarations
class CErrorUI;
class TEditorParameters;
class CMsvProgressReporterOperation;
class CImap4ClientMtm;
class CMuiuFlags;

/**
* CImap4MtmUi - IMAP4 MTM UI
*/
class CImap4MtmUi : public CImumMtmBaseMtmUi
    {
    public:
        /**
        *
        */
        static CImap4MtmUi* NewL(
            CBaseMtm& aBaseMtm,
            CRegisteredMtmDll& aRegisteredMtmDll);

        /**
        * Destructor
        */
        virtual ~CImap4MtmUi();

        /**
        * From CBaseMtmUi
        * Function NOT dependent on the current context
        * Context set to new entry
        */
        virtual CMsvOperation* CreateL(
            const TMsvEntry& aEntry,
            CMsvEntry& aParent,
            TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* OpenL(TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* CloseL(TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* EditL(TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* ViewL(TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* CancelL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection);

        /**
        * From CBaseMtmUi
        * Deletes entries from the current context, which must be a folder or service of the relevant MTM
        */
        virtual CMsvOperation* DeleteFromL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Un-deletes entries from the current context, which must be a folder or service of the relevant MTM
        */
        virtual CMsvOperation* UnDeleteFromL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        */
        virtual CMsvOperation* OpenL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection);

        /**
        * From CBaseMtmUi
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        */
        virtual CMsvOperation* CloseL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection);

        /**
        * From CBaseMtmUi
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        */
        virtual CMsvOperation* EditL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection);

        /**
        * From CBaseMtmUi
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        */
        virtual CMsvOperation* ViewL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection);

        /**
        * From CBaseMtmUi
        */
        virtual TInt QueryCapability(TUid aCapability, TInt& aResponse);

        /**
        * From CBaseMtmUi
        */
        virtual CMsvOperation* InvokeAsyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus,
            TDes8& aParameter);

        /**
        * From CBaseMtmUi
        */
        virtual void InvokeSyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TDes8& aParameter);

        /**
        * From CBaseMtmUi
        * Context should be MTM folder/service to copy to
        */
        virtual CMsvOperation* CopyToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context should be MTM folder/service to move to
        */
        virtual CMsvOperation* MoveToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context should be MTM folder/service to copy from
        */
        virtual CMsvOperation* CopyFromL(
            const CMsvEntrySelection& aSelection,
            TMsvId aTargetId,
            TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context should be MTM folder/service to move from
        */
        virtual CMsvOperation* MoveFromL(
            const CMsvEntrySelection& aSelection,
            TMsvId aTargetId,
            TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        * Takes ownership of the passed entry (which the context is switched to )
        */
        virtual CMsvOperation* ReplyL(
            TMsvId aDestination,
            TMsvPartList aPartlist,
            TRequestStatus& aCompletionStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        * Takes ownership of the passed entry (which the context is switched to )
        */
        virtual CMsvOperation* ForwardL(
            TMsvId aDestination,
            TMsvPartList aPartList,
            TRequestStatus& aCompletionStatus);

        /**
        * From CBaseMtmUi
        * Interpret transfer progress
        */
        virtual TInt GetProgress(
            const TDesC8& aProgress,
            TBuf<EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans) const;

        /**
        * From CBaseMtmUi
        * Interpret transfer progress
        */
        virtual TInt DisplayProgressSummary(
            const TDesC8& aProgress) const;

        /**
        *
        */
        CMsvOperation* LaunchMultipleEditorApplicationsL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus);

        /**
        *
        */
        CMsvOperation* LaunchEditorApplicationL(TRequestStatus& aStatus);

        /**
        *
        */
        CMsvOperation* LaunchEditorApplicationL(
            TRequestStatus& aStatus,
            const TEditorParameters& aParams);

    public:
        /**
        *
        */
        inline CImap4ClientMtm* Imap4BaseMtm() const;

        /**
        *
        */
        TMsvId TrueServiceIdOfEntryL(TMsvId aEntryId);

    protected:
        /**
        *
        */
        TBool IsConnected(const TMsvId aService) const;

        /**
        *
        */
        CMsvEntrySelection* SingleEntrySelectionLC(TMsvId aId) const;

        /**
        *
        */
        TMsvId SelectionContainsInboxL(
            const CMsvEntry& aParentEntry,
            const CMsvEntrySelection& aSelection) const;

        /**
        *
        */
        TBool SelectionContainsGhostEntryL(
            const CMsvEntry& aParentEntry,
            const CMsvEntrySelection& aSelection);

        /**
        *
        */
        TBool SelectionContainsOnlyMessagesL(
            const CMsvEntry& aParentEntry,
            const CMsvEntrySelection& aSelection) const;

        /**
        *
        */
        CMsvOperation* EditImap4ServiceL(
            TRequestStatus& aStatus,
            TMsvId aServiceEntry);

        /**
        *
        */
        HBufC* GetEditorFileNameL();

        /**
        *
        */
        HBufC* GetViewerFileNameL();

        /**
        *
        */
        TBool WithinIMAPInboxL(TMsvId aEntry);

        /**
        *
        */
        CMsvOperation* CopyMoveToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus,
            TBool aCopy);

        /**
        *
        */
        CMsvOperation* ForwardOrReplyL(
            TMsvPartList aPartlist,
            TMsvId aDestination,
            TRequestStatus& aCompletionStatus,
            TBool aForward);

        /**
        *
        */
        CMsvEntrySelection* StripInvalidEntriesLC(
            const CMsvEntrySelection& aSelection) const;

        /**
        *
        */
        CMsvEntrySelection* StripBodyTextCompleteEntriesLC(
            const CMsvEntrySelection& aSelection) const;

        /**
        *
        */
        CMsvEntrySelection* StripCompleteEntriesLC(
            const CMsvEntrySelection& aSelection) const;

        /**
        *
        */
        void ValidateFolderNameL(
            const TDesC& aName,
            TDes8& aRetProgress);

        /**
        *
        */
        CMsvOperation* DeleteFoldersFromL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus,
            TMsvId aService,
            TBool aOnline);

        /**
        *
        */
        void CheckSelectionL(const CMsvEntrySelection& aSelection) const;

        /**
        *
        */
        void CheckEntryL(const TMsvEntry& aEntry) const;

        /**
        *
        */
        CMsvOperation* CompletedImap4OperationL(
            TMsvId aServiceId,
            TRequestStatus& aStatus);


        /**
        *
        */
        CMsvOperation* FetchMessagesL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection,
            TInt aFunctionId,
            TImImap4GetMailInfo& aGetMailInfo);

        /**
        * Initialises the parameters for Fetch New, Fetch All or Fetch Selected.
        * @param aSelection: array of messages to be selected
        * @param aGetMailInfo: get mail info to be filled
        * @param aInsertParentFolder: ETrue if parent needs to be inserted at the beginning of the array, Fetch New and Fetch All need that
        */
        void InitParametersForFetchCmdL(
            CMsvEntrySelection* aSelection,
            TImImap4GetMailInfo& aGetMailInfo,
            TBool aInsertParentFolder = ETrue ) const;


        /**
        *
        */
        void InitParametersForGetMailCmdL(
            CMsvEntrySelection& aSelection,
            TImImap4GetMailInfo& aGetMailInfo);

        /**
        *
        */
        TInt PopulateMessagesSyncL(const CMsvEntrySelection& aSelection);

        /**
        *
        */
        CMsvOperation* OpenMessageL(
            TRequestStatus& aCompletionStatus,
            const TEditorParameters& aEditorParams);

        /**
        *
        */
        CMsvOperation* PopulateMessagesL(
            TRequestStatus& aCompletionStatus,
            const CMsvEntrySelection& aSel);

        /**
        *
        */
        CMsvOperation* DoPopulateIncompleteMessageL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSel,
            const TEditorParameters& aEditorParams);

        /**
        *
        */
        CMsvOperation* InvokeRemoteFetchOpL(
            CMsvProgressReporterOperation& aReporter,
            TRequestStatus& aCompletionStatus,
            const CMsvEntrySelection& aSel,
            TInt aFunctionId,
            const TImImap4GetMailInfo& aGetMailInfo);

        /**
        *
        */
        CMsvOperation* InvokeCopyMoveOpL(
            CMsvProgressReporterOperation& aReporter,
            TRequestStatus& aCompletionStatus,
            const CMsvEntrySelection& aSel,
            TMsvId aTargetId,
            TBool aCopy);

    protected:

        /**
        * from CBaseMtmUi
        */
        virtual void GetResourceFileName(TFileName& aFileName) const;

    private:

        /**
        *
        */
        TBool RemoteLinkAbort();

        // --- Service state checking utilities
        enum TImapServiceInformationFlags
            {
            EIsIdle=0x0001,
            EIsDisconnected=0x0002,
            EIsConnecting=0x0004,
            EIsDisconnecting=0x0008,
            EIsLoaded=0x0010
            };

        /**
        *
        */
        inline TBool ServiceIsIdle(TInt aFlags) const;

        /**
        *
        */
        inline TBool ServiceIsDisconnected(TInt aFlags) const;

        /**
        *
        */
        inline TBool ServiceIsConnecting(TInt aFlags) const;

        /**
        *
        */
        inline TBool ServiceIsDisconnecting(TInt aFlags) const;

        /**
        *
        */
        inline TBool ServiceIsLoaded(TInt aFlags) const;

        /**
        *
        */
        TInt ServiceState(TMsvId aServiceId) const;

        /**
        *
        */
        CImap4MtmUi(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll);

        /**
        *
        */
        virtual void ConstructL();

        /**
        *
        */
        TInt ShowErrorDialog(TInt aErrorCode, TInt aErrorMessageRID=0) const;

        /**
        *
        */
        TInt GetUiProgress(
            const TDesC8& aProgress,
            TBuf<EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans) const;

        /**
        *
        */
        TInt GetSyncProgress(
            const TDesC8& aProgress,
            TBuf<EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans) const;

        /**
        *
        */
        TInt DisplayUiProgressSummary(const TDesC8& aProgress) const;

        /**
        *
        */
        TInt DisplayConnectionProgressSummary(const TDesC8& aProgress) const;

        /**
        *
        */
        TInt DisplaySyncProgressSummary(const TDesC8& aProgress) const;

        /**
        *
        */
        CMsvOperation* OfferSynchroniseL(
            TMsvId aService,
            TRequestStatus& aStatus);

        /**
        *
        */
        CMsvOperation* ShowMessageInfoL(
            TRequestStatus& aCompletionStatus,
            TDes8& aParameter);

        /**
        * InitParametersForFilteredPopulateL
        **/
        void InitParametersForFilteredPopulateL( CMsvEntrySelection& aSelection,
            TImImap4GetPartialMailInfo& aPartialGetMailInfo);

        /**
        * DoFilteredPopulateL
        */
        CMsvOperation* DoFilteredPopulateL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection,
            TInt aFunctionId,
            TImImap4GetPartialMailInfo& aPartialGetMailInfo);

    private:
        // --- Data ---
        HBufC*      iEditorFilename;
        HBufC*      iViewerFilename;
        CMsvEntry*  iEntry;
        TBuf<128>   iInboxName;
    };


#include "Imap4MtmUi.inl"

#endif // __IMAP4MTMUI_H__
