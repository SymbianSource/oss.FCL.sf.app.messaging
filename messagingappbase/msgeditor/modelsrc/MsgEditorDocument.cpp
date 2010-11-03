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
* Description:  MsgEditorDocument implementation
*
*/



// ========== INCLUDE FILES ================================

#include <mtclbase.h>
#include <mtmuibas.h>                   // MTM UI base
#include <mtmdef.h>

#include <MuiuMsgEditorLauncher.h>      // for TEditorParameters
#include <MuiuOperationWait.h>          // for CMuiuOperationWait

#include "MsgEditorDocument.h"          // module header
#include "MsgEditorModelPanic.h"        // Panics
#include "MsgEditorLauncher.h"
#include "MsgEditorModel.h"             // Model
#include "MsgEditorModelObserver.h"     // MMsgEditorModelObserver
#include "MsgAttachmentModel.h"
#include "MsgEditorAppUi.h"             // MsgEditorAppUi

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

const TInt KOperationGranularity = 4;

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgEditorDocument::CMsgEditorDocument
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgEditorDocument::CMsgEditorDocument( CEikApplication& aApp ) :
    CEikDocument( aApp )
    {
    }

// ---------------------------------------------------------
// CMsgEditorDocument::~CMsgEditorDocument
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgEditorDocument::~CMsgEditorDocument()
    {
    if ( iOpWatchers )
        {
        iOpWatchers->ResetAndDestroy();
        }
    delete iOpWatchers;
    delete iModel;
    delete iAttachmentModel;
    iAttachmentModel = NULL;
    }

// ---------------------------------------------------------
// CMsgEditorDocument::SetEntryWithoutNotificationL
//
// Set given entry to be current context.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorDocument::SetEntryWithoutNotificationL( TMsvId aId )
    {
    __ASSERT_DEBUG( ( &( iModel->Session() ) != NULL ), Panic( ENoSession ) );

    iModel->SetEntryL( aId );
    }

// ---------------------------------------------------------
// CMsgEditorDocument::SetEntryL
//
// Set given entry to be current context and notifies after change.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorDocument::SetEntryL( TMsvId aId )
    {
    SetEntryWithoutNotificationL( aId );
#ifdef RD_MSG_FAST_PREV_NEXT
    SetMsgAsReadL();
#endif
    EntryChangedL();
    }

// ---------------------------------------------------------
// CMsgEditorDocument::Entry
//
// Return context.
// ---------------------------------------------------------
//
EXPORT_C const TMsvEntry& CMsgEditorDocument::Entry() const
    {
    __ASSERT_DEBUG( HasModel(), Panic( ENoMessageEntry ) );

    return iModel->Entry();
    }

// ---------------------------------------------------------
// CMsgEditorDocument::Session
//
// Return session.
// ---------------------------------------------------------
//
EXPORT_C CMsvSession& CMsgEditorDocument::Session() const
    {
    __ASSERT_DEBUG( iModel, Panic( ENoSession ) );

    return iModel->Session();
    }

// ---------------------------------------------------------
// CMsgEditorDocument::CurrentEntry
//
// Returns CMsvEntry of the current context.
// ---------------------------------------------------------
//
EXPORT_C CMsvEntry& CMsgEditorDocument::CurrentEntry() const
    {
    __ASSERT_DEBUG( HasModel(), Panic( ENoMessageEntry ) );

    return iModel->Mtm().Entry();
    }

// ---------------------------------------------------------
// CMsgEditorDocument::Mtm
//
// Returns the current Mtm.
// ---------------------------------------------------------
//
EXPORT_C CBaseMtm& CMsgEditorDocument::Mtm() const
    {
    __ASSERT_DEBUG( HasModel(), Panic( ENoMessageEntry ) );

    return iModel->Mtm();
    }

// ---------------------------------------------------------
// CMsgEditorDocument::MtmUi
//
// to be deprecated
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUi& CMsgEditorDocument::MtmUi() const
    {
    __ASSERT_DEBUG( HasModel(), Panic( ENoMessageEntry ) );
    CBaseMtmUi* retVal = NULL;
    TRAP_IGNORE( retVal = &(iModel->MtmUiL() ) );
    return *retVal;
    }

// ---------------------------------------------------------
// CMsgEditorDocument::MtmUiData
//
// to be deprecated
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUiData& CMsgEditorDocument::MtmUiData() const
    {
    __ASSERT_DEBUG( HasModel(), Panic( ENoMessageEntry ) );
    CBaseMtmUiData* retVal = NULL;
    TRAP_IGNORE( retVal = &(iModel->MtmUiDataL() ) );
    return *retVal;
    }

// ---------------------------------------------------------
// CMsgEditorDocument::MtmUiL
//
// Returns the current MtmUi.
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUi& CMsgEditorDocument::MtmUiL() const
    {
    __ASSERT_DEBUG( HasModel(), Panic( ENoMessageEntry ) );

    return iModel->MtmUiL();
    }

// ---------------------------------------------------------
// CMsgEditorDocument::MtmUiDataL
//
// Returns the current MtmUiData.
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUiData& CMsgEditorDocument::MtmUiDataL() const
    {
    __ASSERT_DEBUG( HasModel(), Panic( ENoMessageEntry ) );

    return iModel->MtmUiDataL();
    }
    
// ---------------------------------------------------------
// CMsgEditorDocument::PrepareMtmL
//
// Loads client and ui mtm's. Set clietn mtm
// to have some context
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorDocument::PrepareMtmL( const TUid aMtmType )
    {
    iModel->PrepareMtmL( aMtmType );
    }

// ---------------------------------------------------------
// CMsgEditorDocument::RestoreL
//
// Called (by the environment) when editor or viewer is opened embedded.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorDocument::RestoreL(
        const CStreamStore& aStore,
        const CStreamDictionary& aStreamDic )
    {
    TEditorParameters params;
    RStoreReadStream stream;

    stream.OpenLC( aStore, aStreamDic.At( KUidMsvEditorParameterValue ) );
    params.InternalizeL( stream );
    CleanupStack::PopAndDestroy();  // stream

    LaunchParametersL( params );
    EntryChangedL();
    PrepareToLaunchL( static_cast<CMsgEditorAppUi*>( iAppUi ) );
    }

// ---------------------------------------------------------
// CMsgEditorDocument::HasModel
//
// Check if model is already created.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgEditorDocument::HasModel() const
    {
    return ( iModel && NULL != &( iModel->Mtm() ) );
    }

// ---------------------------------------------------------
// CMsgEditorDocument::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorDocument::ConstructL()
    {
    iModel = new ( ELeave ) CMsgEditorModel();
    iModel->ConstructL();
    iOpWatchers = new ( ELeave ) CSingleOpWatchers( KOperationGranularity );

    iAttachmentModel = CreateNewAttachmentModelL( EFalse );
    iAttachmentModel->SetObserver( this );
    }

// ---------------------------------------------------------
// CMsgEditorDocument::PrepareToLaunchL
//
// Calls LaunchView() function after all pre-launch steps are completed.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorDocument::PrepareToLaunchL(
    MMsgEditorLauncher* aLauncher )
    {
    __ASSERT_DEBUG( iModel, Panic( ENoMessageEntry ) );

    iModel->iLaunchWait--;

    if ( iModel->iLaunchWait <= 0 )
        {
        __ASSERT_DEBUG( aLauncher, Panic( EViewNotReady ) );

        aLauncher->LaunchViewL();
        iLaunched = ETrue;
        }
    }

// ---------------------------------------------------------
// CMsgEditorDocument::LaunchParametersL
//
// Handle command line parameters.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorDocument::LaunchParametersL(
    const TEditorParameters& aParameters )
    {
    iLaunchFlags = aParameters.iFlags;

    iModel->Wait();

    if ( aParameters.iFlags & EMsgCreateMessageToService )
        {
        // Create new message in given folder/service.
        const TMsvId id = CreateNewL( aParameters.iId, DefaultMsgFolder() );
        SetEntryWithoutNotificationL( id );
        }
    else if ( aParameters.iFlags & EMsgCreateNewMessage )
        {
        // create new in folder.
        const TMsvId id = CreateNewL(DefaultMsgService(), aParameters.iId);
        SetEntryWithoutNotificationL(id);
        }
    else if ( aParameters.iFlags & EMsgForwardMessage       ||
             aParameters.iFlags & EMsgReplyToMessageSender ||
             aParameters.iFlags & EMsgReplyToMessageAll )
        {
        
        const TMsvId id = ChangeContextL( aParameters );
        
         // Save priority of forwarded message from original entry as
         // entry is changed on SetEntryWithoutNotificationL
        TMsvPriority priority( EMsvMediumPriority );
        if( aParameters.iFlags & EMsgForwardMessage )
            {
            priority = Entry().Priority();
            }
        
        SetEntryWithoutNotificationL( id );

        TBool changeEntry = EFalse;
        TMsvEntry tEntry = Entry();
        
        // Set saved priority for new entry.
        tEntry.SetPriority( priority ); 
        
        if ( tEntry.Visible() )
            {
            tEntry.SetVisible( EFalse );
            changeEntry = ETrue;
            }

        if ( !tEntry.InPreparation() )
            {
            tEntry.SetInPreparation( ETrue );
            changeEntry = ETrue;
            }

        if ( changeEntry )
            {
            CurrentEntry().ChangeL( tEntry );
            }
        }
    else
        {
        // Open editor using the given message id.
        const TMsvId preparedId = PrepareContextL( aParameters.iId );
        SetEntryWithoutNotificationL( preparedId );
        }

    __ASSERT_DEBUG( HasModel(), Panic( ENoMessage ) );

    SetMsgAsReadL();
    }

// ---------------------------------------------------------
// CMsgEditorDocument::ChangeContextL
//
// Handles existing messages (typically opened from Drafts).
// This function can be overriden by inherited class if some
// preprocessing is needed for the message.
//
// Default implementation just returns the given context.
// ---------------------------------------------------------
//
EXPORT_C TMsvId CMsgEditorDocument::PrepareContextL( const TMsvId aContext )
    {
    return aContext;
    }
    
// ---------------------------------------------------------
// CMsgEditorDocument::ChangeContextL
//
// Handles reply and forward messages, change current message context to
// be context of replied or forwarded message.
// ---------------------------------------------------------
//
EXPORT_C TMsvId CMsgEditorDocument::ChangeContextL(
    const TEditorParameters& aParameters )
    {
    SetEntryWithoutNotificationL( aParameters.iId );

    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();

    CMsvOperation* oper = NULL;

    const TMsvPartList parts = aParameters.iPartList;
    const TMsvId dest = aParameters.iDestinationFolderId;

    if ( aParameters.iFlags & EMsgReplyToMessageSender ||
        aParameters.iFlags & EMsgReplyToMessageAll )
        {
        oper = CreateReplyL( dest, parts, wait->iStatus );
        }
    else if (aParameters.iFlags & EMsgForwardMessage)
        {
        oper = CreateForwardL( dest, parts, wait->iStatus );
        }

    __ASSERT_ALWAYS( oper != NULL, Panic( ENoMessage ) );

    CleanupStack::PushL( oper );

    wait->Start();

    const TMsvId id = GetOperId( *oper );

    if ( id < 0 )
        {
        User::Leave( id );
        }

    CleanupStack::PopAndDestroy( 2 );      // oper, wait

    return id;
    }

// ---------------------------------------------------------
// CMsgEditorDocument::CreateReplyL
//
//
// ---------------------------------------------------------
//
EXPORT_C CMsvOperation* CMsgEditorDocument::CreateReplyL(
    TMsvId aDest, TMsvPartList aParts, TRequestStatus& aStatus )
    {
    return Mtm().ReplyL( aDest, aParts, aStatus );
    }

// ---------------------------------------------------------
// CMsgEditorDocument::CreateForwardL
//
//
// ---------------------------------------------------------
//
EXPORT_C CMsvOperation* CMsgEditorDocument::CreateForwardL(
    TMsvId aDest, TMsvPartList aParts, TRequestStatus& aStatus )
    {
    return Mtm().ForwardL( aDest, aParts, aStatus );
    }

// ---------------------------------------------------------
// CMsgEditorDocument::AddSingleOperationL
//
// Adds operation into operation queque.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorDocument::AddSingleOperationL(
    CMsvOperation* aOperation, CMsvSingleOpWatcher* aSopWatch )
    {
    __ASSERT_DEBUG( aSopWatch != NULL, Panic( ENullPointer ) );
    __ASSERT_DEBUG( aOperation != NULL, Panic( ENullPointer ) );

    aSopWatch->SetOperation( aOperation );
    iOpWatchers->AppendL( aSopWatch );
    }

// ---------------------------------------------------------
// CMsgEditorDocument::OpCompleted
//
// Is called when operation is completed. If operation is found, handler is called.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorDocument::OpCompleted(
    CMsvSingleOpWatcher& aOpWatcher, TInt aCompletionCode )
    {
    __ASSERT_DEBUG( iOpWatchers != NULL, Panic( ENullPointer ) );

    const TInt count = iOpWatchers->Count();
    TInt i = 0;

    for ( ; i < count; i++ )
        {
        // looking for right operation in array.
        if ( ( (*iOpWatchers)[i] ) == &aOpWatcher )
            {
            break;
            }
        }

    if ( i >= count )
        {
        // Although invalid operation is an error, we do not
        // want to panic for that in product - since it not supposed
        // to seem anyhow to user (Is this true?)

        __ASSERT_DEBUG( EFalse, Panic( EInvalidOperation ) );
        return ;
        }

    if ( aCompletionCode != KErrNone )
        {
        __ASSERT_DEBUG( HasModel(), Panic( ENullPointer ) );

        TRAP_IGNORE( MtmUi().DisplayProgressSummary( (*iOpWatchers)[i]->Operation().ProgressL() ) );
        }

    // Operation has completed, remove watcher and operation (watcher owns operation)
    // NOTE: Have to delete the watcher first, as the array does not call delete.
    delete (*iOpWatchers)[i];

    iOpWatchers->Delete( i );
    iOpWatchers->Compress();
    }

// ---------------------------------------------------------
// CMsgEditorDocument::MediaAvailable
//
// Return the current (or latest known) state of the message storage.
// If this function returns EFalse, then it's likely that operations
// saving or restoring message entries will fail.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgEditorDocument::MediaAvailable() const
    {
    return iModel->MediaAvailable();
    }

// ---------------------------------------------------------
// CMsgEditorDocument::SetEditorModelObserver
//
// Define editor model observer. There can be only one editor model
// observer defined at any time, so this function just changes the
// current observer.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorDocument::SetEditorModelObserver(
    MMsgEditorModelObserver* aObs )
    {
    iModel->SetModelObserver( aObs );
    }

// ---------------------------------------------------------
// CMsgEditorDocument::GetOperId
//
// Returns TMsvId (new message id) of given CMsvOperation.
//
// Given operation class must return the id of the new
// message entry with FinalProgress().
// ---------------------------------------------------------
//
EXPORT_C TMsvId CMsgEditorDocument::GetOperId( CMsvOperation& aOperation )
    {
    TMsvId temp;
    TPckgC < TMsvId > paramPack( temp );
    const TDesC8& progress = aOperation.FinalProgress();
    paramPack.Set( progress );
    return paramPack();
    }

// ---------------------------------------------------------
// CMsgEditorDocument::AttachmentModel
//
// Returns reference to attachment model.
// ---------------------------------------------------------
//
EXPORT_C CMsgAttachmentModel& CMsgEditorDocument::AttachmentModel() const
    {
    return *iAttachmentModel;
    }

// ---------------------------------------------------------
// CMsgEditorDocument::NotifyChanges
//
// Default implementation panics with EMsgInheritedFunctionMissing.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorDocument::NotifyChanges(
    TMsgAttachmentCommand /*aCommand*/, CMsgAttachmentInfo* /*aAttachmentInfo*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EMsgInheritedFunctionMissing ) );
    }


// ---------------------------------------------------------
// CMsgEditorDocument::GetAttachmentFileL
//
// Default implementation leaves with KErrNotSupported
// ---------------------------------------------------------
//
EXPORT_C RFile CMsgEditorDocument::GetAttachmentFileL( TMsvAttachmentId /*aId*/ )
    {
    User::Leave( KErrNotSupported );
    RFile dummy;
    return dummy;
    }

// ---------------------------------------------------------
// CMsgEditorDocument::CreateNewAttachmentModelL
//
//
// ---------------------------------------------------------
//
EXPORT_C CMsgAttachmentModel* CMsgEditorDocument::CreateNewAttachmentModelL(
    TBool aReadOnly )
    {
    return CMsgAttachmentModel::NewL( aReadOnly );
    }

// ---------------------------------------------------------
// CMsgEditorDocument::SetMsgAsReadL
//
// ---------------------------------------------------------
//
void CMsgEditorDocument::SetMsgAsReadL()
    {
    iLaunchFlags &= ~EMsgUnreadMessage;
    
    // Unset unread flag
    // Can be done since viewers should not lock entries
    if ( Entry().Unread() || Entry().New() )
        {
        if ( Entry().Unread() )
            {
            iLaunchFlags |= EMsgUnreadMessage;
            }
        TMsvEntry entry = Entry();  // create a copy

        entry.SetUnread( EFalse );
        entry.SetNew( EFalse );

        // Asynchronous version used because entry may be remote.
        CMsvSingleOpWatcher* watch = CMsvSingleOpWatcher::NewLC( *this );
        CMsvOperation* op = CurrentEntry().ChangeL( entry, watch->iStatus );
        CleanupStack::Pop();  // watch;
        AddSingleOperationL( op, watch );
        }
    }

//  End of File
