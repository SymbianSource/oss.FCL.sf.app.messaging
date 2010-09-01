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
*       POP3 MTM UI
*
*/

#ifndef __POP3MTMUI_H__
#define __POP3MTMUI_H__

#include <mtmuibas.h>
#include <popcmtm.h>

#include "ImumMtmBaseMtmUi.h"   // CImumMtmBaseMtmUi


// forward declarations
class TEditorParameters;
class CErrorUI;
class CMuiuFlags;

/**
* CPop3MtmUi - POP3 MTM UI
*/
class CPop3MtmUi : public CImumMtmBaseMtmUi
    {
    public:

        /**
        *
        */
        static CPop3MtmUi* NewL(
            CBaseMtm& aBaseMtm,
            CRegisteredMtmDll& aRegisteredMtmDll );

        /**
        * Destructor
        */
        virtual ~CPop3MtmUi();

        /**
        * From CBaseMtmUi
        * Function NOT dependent on the current context
        * Context set to new entry
        */
        virtual CMsvOperation* CreateL(
            const TMsvEntry& aEntry,
            CMsvEntry& aParent,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* OpenL( TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* CloseL( TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* EditL( TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* ViewL( TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Deletes entries from the current context, which must be a folder
        * or service of the relevant MTM
        */
        virtual CMsvOperation* DeleteFromL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Un-deletes entries from the current context, which must be a folder
        * or service of the relevant MTM
        */
        virtual CMsvOperation* UnDeleteFromL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Takes ownership of the passed entry (which the context is switched to )
        */
        virtual CMsvOperation* ReplyL(
            TMsvId aDestination,
            TMsvPartList aPartlist,
            TRequestStatus& aCompletionStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Takes ownership of the passed entry (which the context is switched to )
        */
        virtual CMsvOperation* ForwardL(
            TMsvId aDestination,
            TMsvPartList aPartList,
            TRequestStatus& aCompletionStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        */
        virtual CMsvOperation* OpenL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        */
        virtual CMsvOperation* CloseL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        */
        virtual CMsvOperation* EditL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        */
        virtual CMsvOperation* ViewL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        */
        virtual CMsvOperation* CancelL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );

        /**
        * From CBaseMtmUi
        */
        virtual TInt QueryCapability( TUid aCapability, TInt& aResponse );

        /**
        * From CBaseMtmUi
        */
        virtual CMsvOperation* InvokeAsyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus,
            TDes8& aParameter );

        /**
        * From CBaseMtmUi
        * not relevant to POP3 - Leave with not supported
        */
        virtual CMsvOperation* CopyToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * not relevant to POP3 - Leave with not supported
        */
        virtual CMsvOperation* MoveToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * not relevant to POP3 - Leave with not supported
        */
        virtual CMsvOperation* CopyFromL(
            const CMsvEntrySelection& aSelection,
            TMsvId aTargetId,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * not relevant to POP3 - Leave with not supported
        */
        virtual CMsvOperation* MoveFromL(
            const CMsvEntrySelection& aSelection,
            TMsvId aTargetId,
            TRequestStatus& aStatus );

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
            TInt& aCurrentBytesTrans ) const;

        /**
        * From CBaseMtmUi
        * Interpret transfer progress
        */
        virtual TInt DisplayProgressSummary(
            const TDesC8& aProgress ) const;

        /**
        * Specific to Pop3
        */
        CMsvOperation* LaunchEditorApplicationL(
            TRequestStatus& aStatus,
            const TEditorParameters& aParams );

    protected:
        /**
        *
        */
        TInt GetEngineProgress(
            const TDesC8& aProgress,
            TBuf<EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans ) const;

        /**
        *
        */
        TInt GetUiProgress(
            const TDesC8& aProgress,
            TBuf<EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans ) const;

        /**
        *
        */
        TInt GetConnectionProgress(
            const TDesC8& aProgress,
            TBuf<EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans ) const;

        /**
        *
        */
        TInt GetDeleteProgress(
            TInt aTotalMsgs,
            TInt aError,
            TBuf<EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans) const;

        /**
        *
        */
        TInt DisplayProgressErrorAlert( TInt aErrCode ) const;

        /**
        *
        */
        void FormatConnectingServerProgressTextL(
            TMsvId aServiceId,
            TDes& aReturnString ) const;

        /**
        *
        */
        CMsvEntrySelection* SingleEntrySelectionLC(
            TMsvId aId ) const;

        /**
        *
        */
        CMsvOperation* ViewMessageL( TRequestStatus& aStatus );

        /**
        *
        */
        CMsvOperation* EditSmtpServiceL(
            TRequestStatus& aStatus,
            TMsvId aServiceEntry ) const;

        /**
        *
        */
        CMsvOperation* RetrieveAndEditMessageL(
            TRequestStatus& aStatus,
            const TEditorParameters& aEditorParams );

        /**
        *
        */
        CMsvOperation* GetMailL( TRequestStatus& aStatus );

        /**
        *
        */
        CMsvOperation* FetchL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSel,
            TInt aFunctionId );

        /**
        *
        */
        TInt FetchSyncL( const CMsvEntrySelection& aSelection );

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
        TInt ProgressType( const TDesC8& aProgress ) const;

        /**
        *
        */
        const CImPop3Settings& GetAccountSettingsL( TMsvId aId ) const;

        /**
        *
        */
        void CheckSelectionL(
            const CMsvEntrySelection& aSelection,
            TMsvId& aParent ) const;

        /**
        *
        */
        void CheckEntryL( const TMsvEntry& aEntry ) const;

        /**
        *
        */
        CMsvEntrySelection* StripInvalidEntriesLC(
            const CMsvEntrySelection& aSelection ) const;

    protected:
        /**
        * From CBaseMtmUi
        */
        virtual void GetResourceFileName( TFileName& aFileName ) const;

    private:
        /**
        *
        */
        CPop3MtmUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll );
        /**
        *
        */
        virtual void ConstructL();
        /**
        *
        */
        inline CPop3ClientMtm& Pop3ClientMtm() const;

        /**
        * Shows message info dialog
        * Context should be the message which info is displayed.
        */
        CMsvOperation* ShowMessageInfoL(
            TRequestStatus& aCompletionStatus,
            TDes8& aParameter );

    private: // Data
        HBufC*      iEditorFilename;
        HBufC*      iViewerFilename;        

        struct TServerNameCache
            {
            TMsvId iService;
            TBuf<EProgressStringMaxLen> iConnectingServerTxt;
            };
        mutable TServerNameCache iServerNameCache;
    };


inline CPop3ClientMtm& CPop3MtmUi::Pop3ClientMtm() const
    {
    return STATIC_CAST(CPop3ClientMtm&, BaseMtm());
    }


#endif // __PP3U_H__
