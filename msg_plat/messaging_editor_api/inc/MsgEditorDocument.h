/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgEditorDocument  declaration
*
*/



#ifndef INC_MSGEDITORDOCUMENT_H
#define INC_MSGEDITORDOCUMENT_H

// ========== INCLUDE FILES ================================

#include <eikdoc.h>
#include <msvids.h>             // KMsvDraftEntryId

#include <MuiuMsvSingleOpWatcher.h>

#include "MsgAttachmentModelObserver.h"

// ========== CONSTANTS ====================================

const TUint KReplyMessage(0x80000000);
const TUint KForwardMessage(0x40000000);
const TUint KReplyForwardMessage(KReplyMessage | KForwardMessage);  // for bwc

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

typedef CArrayPtrFlat < CMsvSingleOpWatcher > CSingleOpWatchers;
typedef TUint TMsvPartList;

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgEditorAppUi;
class CBaseMtmUi;
class CMsvSession;
class CBaseMtm;
class CMsgEditorModel;
class MMsgEditorLauncher;
class TEditorParameters;
class MMsgEditorModelObserver;
class CMsgAttachmentModel;
class CBaseMtmUiData;
class CFileStore;

// ========== CLASS DECLARATION ============================

/**
 * Base class for Message editor/viewer document module.
 *
 */
class CMsgEditorDocument : public CEikDocument, public MMsvSingleOpWatcher, public MMsgAttachmentModelObserver
{

public:    // new functions

    /**
     * Destructor.
     */
    IMPORT_C ~CMsgEditorDocument();

    /**
     * Set given entry to be current context.
     * @param aEntry
     */
    IMPORT_C void SetEntryWithoutNotificationL(TMsvId aEntry);

    /**
     * Set given entry to be current context and notifies after change.
     * @param aEntry
     */
    IMPORT_C void SetEntryL(TMsvId aEntry);

    /**
     * Return context.
     * @return
     */
    IMPORT_C const TMsvEntry& Entry() const;

    /**
     * Return session.
     * @return
     */
    IMPORT_C CMsvSession& Session() const;

    /**
     * Returns CMsvEntry of the current context. If just CMsvEntry of the
     * currently open message is needed, this function should be used instead
     * of CMsvSession::GetEntry(), as this does not create new CMsvEntry.
     * @return
     */
    IMPORT_C CMsvEntry& CurrentEntry() const;

    /**
     * Returns the current Mtm.
     * @return
     */
    IMPORT_C CBaseMtm& Mtm() const;

    /**
     * To be depracated
     * please use MtmUiL instead
     * @return
     */
    IMPORT_C CBaseMtmUi& MtmUi() const;

    /**
     * To be depracated
     * please use MtmUiDataL instead
     * @return
     */
    IMPORT_C CBaseMtmUiData& MtmUiData() const;

    /**
     * Returns the current MtmUi.
     * @return
     */
    IMPORT_C CBaseMtmUi& MtmUiL() const;

    /**
     * Returns the current MtmUiData.
     * @return
     */
    IMPORT_C CBaseMtmUiData& MtmUiDataL() const;

    
    /**
     * Loads client and ui mtm's.
     * @param aMtmType
     */
    IMPORT_C void PrepareMtmL(const TUid aMtmType);

public:    // from CEikDocument

    /**
     * Called (by the environment) when editor or viewer is opened embedded.
     * @param aStore
     * @param aStreamDic
     */
    IMPORT_C void RestoreL(const CStreamStore& aStore, const CStreamDictionary& aStreamDic);

public:    // new functions

    /**
     * Check if model is already created.
     * @return
     */
    IMPORT_C TBool HasModel() const;

    /**
     * Calls LaunchView() function after all pre-launch steps are completed.
     * @param aLauncher
     */
    IMPORT_C void PrepareToLaunchL(MMsgEditorLauncher* aLauncher);

    /**
     * Handle command line parameters.
     * @param aParameters
     */
    IMPORT_C void LaunchParametersL(const TEditorParameters& aParameters);

    /**
     * Checks if document has finished launching.
     * @return
     */
    inline TBool IsLaunched() const;

    /**
     * Returns launch flags that were passed to LaunchParametersL().
     * @return
     */
    inline TInt LaunchFlags() const;

    /**
     * Return the current (or latest known) state of the message storage.
     * If this function returns EFalse, then it's likely that operations
     * saving or restoring message entries will fail.
     * @return
     */
    IMPORT_C TBool MediaAvailable() const;

    /**
     * Define editor model observer. There can be only one editor model
     * observer defined at any time, so this function just changes the
     * current observer.
     * @param aObs
     */
    IMPORT_C void SetEditorModelObserver(MMsgEditorModelObserver* aObs);

    /**
     * Adds operation into operation queque.
     * @param aOperation
     * @param aSopWatch
     */
    IMPORT_C void AddSingleOperationL(CMsvOperation* aOperation, CMsvSingleOpWatcher* aSopWatch);

    /**
     *
     * @param aOperation
     * @return
     */
    IMPORT_C static TMsvId GetOperId(CMsvOperation& aOperation);

    /**
     * Returns reference to attachment model.
     * @return
     */
    IMPORT_C CMsgAttachmentModel& AttachmentModel() const;

    /**
     *
     * @param aDest
     * @param aParts
     * @param aStatus
     * @return
     */
    IMPORT_C virtual CMsvOperation* CreateReplyL(TMsvId aDest, TMsvPartList aParts, TRequestStatus& aStatus);

    /**
     *
     * @param aDest
     * @param aParts
     * @param aStatus
     * @return
     */
    IMPORT_C virtual CMsvOperation* CreateForwardL(TMsvId aDest, TMsvPartList aParts, TRequestStatus& aStatus);

public:    // pure virtuals

    /**
     *
     * @return
     */
    virtual TMsvId DefaultMsgFolder() const = 0;

    /**
     *
     * @return
     */
    virtual TMsvId DefaultMsgService() const = 0;

    /**
     *
     * @param aService
     * @param aFolder
     * @return
     */
    virtual TMsvId CreateNewL(TMsvId aService, TMsvId aFolder) = 0;

    /**
     * Called when new entry is set.
     */
    virtual void EntryChangedL() = 0;

protected: // new functions

    /**
     * Constructor.
     * @param aApp
     */
    IMPORT_C CMsgEditorDocument(CEikApplication& aApp);

    /**
     * 2nd phase constructor.
     */
    IMPORT_C void ConstructL();

    // from MMsvSingleOpWatcher

    /**
     * Is called when operation is completed. If operation is found, handler is called.
     * @param aOpWatcher
     * @param aCompletionCode
     */
    IMPORT_C void OpCompleted(CMsvSingleOpWatcher& aOpWatcher, TInt aCompletionCode);

private:   // from MMsgAttachmentModelObserver

    /**
     * This function is called when attachment model adds or removes
     * attachments from its list.
     *
     * This function must be implemented in the derived class.
     * Implementation in this base class panics with EMsgInheritedFunctionIsMissing.
     * @param aCommand
     */
    IMPORT_C void NotifyChanges(TMsgAttachmentCommand aCommand, CMsgAttachmentInfo* aAttachmentInfo);

    /**
     * This function is called when attachment model wants to get access
     * to an attachment file.
     *
     * This function shouldb be implemented in the derived class if it supports attachments.
     * @param aCommand
     */
    IMPORT_C RFile GetAttachmentFileL( TMsvAttachmentId aId );
    
protected:   // new functions

    /**
    * Handles reply and forward messages, change current message context to
    * be context of replied or forwarded message.
    * @param aParameters
    * @return
    */
    IMPORT_C virtual TMsvId ChangeContextL( const TEditorParameters& aParameters );

    /**
    * Handles existing messages (typically opened from Drafts).
    * This function can be overriden by inherited class if some
    * preprocessing is needed for the message.
    *
    * Default implementation just returns the given context.
    * @param aContext
    * @return
    */
    IMPORT_C virtual TMsvId PrepareContextL( const TMsvId aContext );

private:   // new functions

    /**
     * Creates new attachment model object.
     * @param aReadOnly
     * @return
     */
    IMPORT_C virtual CMsgAttachmentModel* CreateNewAttachmentModelL(TBool aReadOnly);

    /**
     * Sets message to read status
     * @return
     */
     void SetMsgAsReadL();

private:   // data

    TUid                 iTypeId;
    CMsgEditorModel*     iModel;

    CSingleOpWatchers*   iOpWatchers;
    CMsgAttachmentModel* iAttachmentModel;
    TBool                iLaunched;
    TInt                 iLaunchFlags;

};

inline TBool CMsgEditorDocument::IsLaunched() const
{
    return iLaunched;
}

inline TInt CMsgEditorDocument::LaunchFlags() const
{
    return iLaunchFlags;
}

#endif

// End of File
