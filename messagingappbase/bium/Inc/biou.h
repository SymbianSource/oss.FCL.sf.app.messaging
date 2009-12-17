/*
* Copyright (c) 1998 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  biou  declaration
*
*/



#if !defined(__BIOU_H__)
#define __BIOU_H__

#include <mtmuibas.h>
#include <bioscmds.h>           // for TBioProgress::EBiosProcessing
#include <biodb.h>          // MBifChangeObserver, CBifObserver...
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include <bifchangeobserver.h>
#endif
#include <MuiuMsgEditorLauncher.h> 

// forward declarations
class TBioData;
class CTextResolver;
class CMsvOperation;
class CMtmStore;

//////////////////////////////
// CBioMtmUi - BIO MTM UI   //
//////////////////////////////
NONSHARABLE_CLASS(CBioMtmUi) : public CBaseMtmUi, public MBifChangeObserver
    {
public:
    // --- Construction ---
    static CBioMtmUi* NewL(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll);
    //
    // --- Destruction ---
    virtual ~CBioMtmUi();
    //
    // --- Functions NOT dependent on the current context ---
    virtual CMsvOperation* CreateL(const TMsvEntry& aEntry, CMsvEntry& aParent, TRequestStatus& aStatus);// Context set to new entry
    //
    // --- Functions dependent on the current context ---
    virtual CMsvOperation* OpenL(TRequestStatus& aStatus); 
    virtual CMsvOperation* CloseL(TRequestStatus& aStatus);
    virtual CMsvOperation* EditL(TRequestStatus& aStatus);
    virtual CMsvOperation* ViewL(TRequestStatus& aStatus);
    //
    // --- Deletion ---
    // --- Deletes entries from the current context, which must be a folder or service of the relevant MTM ---
    virtual CMsvOperation* DeleteFromL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);//Deletes selection from current context
    // --- Deletes service, which need not be the current context ---
    virtual CMsvOperation* DeleteServiceL(const TMsvEntry& aService, TRequestStatus& aStatus); // default implementation simply delegates to the server
    //
    // --- Message responding ---
    // --- Default implementation calls CBaseMtm::ReplyL() or ForwardL(), followed by EditL() ---
    // --- Takes ownership of the passed entry (which the context is switched to ---
    virtual CMsvOperation* ReplyL(TMsvId aDestination, TMsvPartList aPartlist, TRequestStatus& aCompletionStatus);
    virtual CMsvOperation* ForwardL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus);
    //
    // --- Actions upon message selections ---
    // --- Selections must be in same folder and all of the correct MTM type ---
    // --- Context may change after calling these functions ---
    virtual CMsvOperation* OpenL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection); 
    virtual CMsvOperation* CloseL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);
    virtual CMsvOperation* EditL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);
    virtual CMsvOperation* ViewL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);
    virtual CMsvOperation* CancelL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);
    //  
    // --- RTTI functions ---
    virtual TInt QueryCapability(TUid aCapability, TInt& aResponse);
    virtual CMsvOperation* InvokeAsyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TRequestStatus& aCompletionStatus, TDes8& aParameter);
    virtual void InvokeSyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TDes8& aParameter);
    //  
    // --- These not relevant to BIO MTM - Leave with KErrNotSupported ---
    virtual CMsvOperation* CopyToL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);// Context should be MTM folder/service to copy to
    virtual CMsvOperation* MoveToL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);// Context should be MTM folder/service to move to
    virtual CMsvOperation* CopyFromL(const CMsvEntrySelection& aSelection, TMsvId aTargetId, TRequestStatus& aStatus);// Context should be MTM folder/service to copy from
    virtual CMsvOperation* MoveFromL(const CMsvEntrySelection& aSelection, TMsvId aTargetId, TRequestStatus& aStatus);// Context should be MTM folder/service to move from
    //
    // --- Interpret transfer progress
    virtual TInt GetProgress(const TDesC8& aProgress, TBuf<EProgressStringMaxLen>& aReturnString, TInt& aTotalEntryCount, TInt& aEntriesDone,
        TInt& aCurrentEntrySize, TInt& aCurrentBytesTrans) const;// Default implementation returns KErrNotSupported
    virtual TInt DisplayProgressSummary(const TDesC8& aProgress) const;// Used for display operation summary (usually completed operations). Default implementation returns KErrNotSupported.   

protected: 
    // --- specific - Integrity checking ---
    virtual void CheckEntryL(const TMsvEntry& aEntry) const;

protected: 
    // --- from CBaseMtmUi ---
    virtual void GetResourceFileName(TFileName& aFileName) const;
    
protected:
    CBioMtmUi(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll);
    virtual void ConstructL();

protected: // from MBifChangeObserver
    virtual void HandleBifChangeL(TBifChangeEvent aEvent, TUid aBioId);

protected: // new functions

    virtual void  ResetAndLoadBioDataL();
    virtual CMsvOperation* LaunchApplicationL(TRequestStatus& aStatus,TBool aReadOnly); 
    virtual TInt ProgressType(const TDesC8& aProgress) const;
    virtual TBool IsLocalProgress(TInt aProgressType) const;
    virtual TBool IsEngineProgress(TInt aProgressType) const;

    virtual CMsvOperation* ShowMessageInfoL(TRequestStatus& aCompletionStatus,TDes8& aParameter);
    virtual HBufC* ExtractTitleFromGMSBodyL(const CRichText& aBody, TInt aMaxLength);

    /**
     * Shows info note with qtn_mce_info_message_not_opened.
     * The note displaying is trapped, and the err code ignored.
     */
    virtual void ShowInvalidMsgNote() const;

    /**
     * Shows system error note. Uses CErrResolver for getting the text.
     * The note displaying is trapped, and the err code ignored.
     * @param aError The error code.
     */
    virtual void ShowSystemErrorNote(TInt aError) const;

    virtual HBufC* CreateFromTextLC(CMsvEntry& aEntry) const;
    
protected:
    CArrayFixFlat<TBioData>* iBioData;
    CBifChangeObserver* iBifObserver;
    CTextResolver*  iErrorResolver;
    };



/////////////////////////////////////////////////////////////////////////////////////
// Operation encapsulating the parse operation and the launch application operation.
/////////////////////////////////////////////////////////////////////////////////////

struct TParseAndEditParams
    {
    TBool iLaunchApp;
    TBool iPreferEmbedded;
    TFileName iEditorFileName;
    TEditorParameters iEditorParams;        // Includes message Id.
    };


class TBioUiProgress
    {
public:
    enum TBioUiType
        {
        EBioMtmUiEditing=TBioProgress::EBiosProcessing+1,
        EBioMtmUiParseAndEdit
        };

    TInt    iType;
    TInt    iError;
    };

// Internal class
NONSHARABLE_CLASS(CParseAndEditOp) : public CMsvOperation
    {
public:
    // --- Construction/destruction ---
    static CParseAndEditOp* NewL(CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus,
                                 const TParseAndEditParams& aParams);
    virtual ~CParseAndEditOp();

    // --- Setup and kick off operation ---
    void SetParseOpL(CMsvOperation* aParseOp);

    virtual const TDesC8& ProgressL();
    TRequestStatus& RequestStatus();

protected:
    // --- from CActive ----
    virtual void DoCancel();
    virtual void RunL();
    virtual TInt RunError(TInt aError);

private: // new functions

    // --- Construction---
    CParseAndEditOp(CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus,
                    const TParseAndEditParams& aParams);
    void ConstructL();

    void DoEditL();
    void CompleteObserver();

    /**
     * Sets the UNREAD and NEW properties of the entry to false.
     * @param The ID of the entry to be modified.
     */
    void SetEntryReadAndOldL(TMsvId aEntryId);

    /**
     * Deletes the parsed attachment from message store
     * @param The ID of the entry from which the attachment is to be deleted..
     */
    void DeleteMessageAttachmentL(TMsvId aEntryId);

private:
    // Data
    enum TParseAndEditState { EStateIdle, EStateParsing, EStateEditing, EStateComplete };
    TParseAndEditState iState;
    TParseAndEditParams iParams;
    CMsvOperation* iOperation;
    TBioUiProgress iLocalProgress;  
    TPckgBuf<TBioUiProgress> iLocalProgressPckg;
    CMtmStore* iMtmStore;
    };


#endif // __BIOU_H__