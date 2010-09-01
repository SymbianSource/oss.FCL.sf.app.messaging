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
* Description:  MsgEditorModel implementation
*
*/



// ========== INCLUDE FILES ================================

#include <mtuireg.h>                 // MTM UI registery
#include <mtmuibas.h>                // MTM UI base
#include <mtudreg.h>                 // MTM UI data registry
#include <mtudcbas.h>                // MTM UI data base
#include <msvids.h>                  // NullEntryId

#include "MsgEditorCommon.h"
#include "MsgEditorModel.h"          // Module Header
#include "MsgEditorModelPanic.h"     // panic codes for model
#include "MsgEditorModelObserver.h"  // MMsgEditorModelObserver

#include "MsgEditorLogging.h"


//#define ASYNC_CONST


// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

const TInt KThingsToWaitBeforeLaunch = 2;

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgEditorModel::CMsgEditorModel
//
// Constructor.
// ---------------------------------------------------------
//
CMsgEditorModel::CMsgEditorModel() : iLaunchWait(KThingsToWaitBeforeLaunch)
{
}

// ---------------------------------------------------------
// CMsgEditorModel::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CMsgEditorModel::ConstructL()
{
    MEBLOGGER_ENTERFN("CMsgEditorModel::ConstructL");

#ifdef ASYNC_CONST

    MEBLOGGER_WRITE("OpenAsyncL");
    MEBLOGGER_WRITE_TIMESTAMP("");

    iSession = CMsvSession::OpenAsyncL(*this);

#else

    MEBLOGGER_WRITE("OpenSyncL");
    MEBLOGGER_WRITE_TIMESTAMP("");

    TRAPD(err, iSession = CMsvSession::OpenSyncL(*this));

    if (err != KErrNone)
    {
        iMediaAvailable = EFalse;
        if (iModelObserver)
        {
            // exits.
            iModelObserver->HandleServerFailedToStartL();
        }
        else
        {
            User::Leave(err);
        }
    }

    __ASSERT_DEBUG(iSession, Panic(ENoSession));

    iMediaAvailable = ETrue;

    CompleteConstructL();

#endif

    MEBLOGGER_LEAVEFN("CMsgEditorModel::ConstructL");
}

// ---------------------------------------------------------
// CMsgEditorModel::Wait
//
// Starts active scheduler to complete construction, if necessary.
// ---------------------------------------------------------
//
void CMsgEditorModel::Wait()
{
#ifdef ASYNC_CONST

    MEBLOGGER_ENTERFN("CMsgEditorModel::Wait");

    if (!iMediaAvailable)
    {
        iWait = ETrue;
        CActiveScheduler::Start();
    }
    iWait = EFalse;

    MEBLOGGER_LEAVEFN("CMsgEditorModel::Wait");

#endif
}

// ---------------------------------------------------------
// CMsgEditorModel::CompleteConstructL
//
// Creates client MTM registry and MTM UI registry when session is ready
// for use. This completes model construction and is called after 'server
// ready' event is received after async opening of CMsvSession.
// ---------------------------------------------------------
//
void CMsgEditorModel::CompleteConstructL()
{
    MEBLOGGER_ENTERFN("CMsgEditorModel::CompleteConstructL");
    MEBLOGGER_WRITE_TIMESTAMP("");

    __ASSERT_DEBUG(iMtmUiReg == NULL, Panic(ENullPointer));

    iMtmReg = CClientMtmRegistry::NewL(*iSession);
    //delay the construction of iMtmUiReg and iMtmUiDataReg until needed

    __ASSERT_DEBUG(iMtmReg, Panic(ENullPointer));


    MEBLOGGER_LEAVEFN("CMsgEditorModel::CompleteConstructL");
}

// ---------------------------------------------------------
// CMsgEditorModel::~CMsgEditorModel
//
// Destructor.
// ---------------------------------------------------------
//
CMsgEditorModel::~CMsgEditorModel()
{
    // note that iMtmReg can be deleted only after iMtm is deleted first
    delete iMtmUiData;
    delete iMtmUiDataReg;

    delete iMtmUi;
    delete iMtmUiReg;

    delete iMtm;
    delete iMtmReg;

    delete iSession;
    iModelObserver = NULL;   // not owned
}

// ---------------------------------------------------------
// CMsgEditorModel::SetEntryL
//
// Set up current message entry, creates a real connection between current
// editor/viewer session and an message entryy to be displayed or edited.
// ---------------------------------------------------------
//
void CMsgEditorModel::SetEntryL(TMsvId aEntryId)
    {
    MEBLOGGER_ENTERFN( "CMsgEditorModel::SetEntryL" );
    MEBLOGGER_WRITE_TIMESTAMP("");

    TMsvId service;
    TMsvEntry entry;
    User::LeaveIfError( Session().GetEntry( aEntryId, service, entry ) );

    if ( iMtm == NULL || iMtm->Type() != entry.iMtm )
        {
        delete iMtm;
        delete iMtmUi;
        delete iMtmUiData;

        iMtm   = NULL;
        iMtmUi = NULL;
        iMtmUiData = NULL;

        iMtm   = iMtmReg->NewMtmL( entry.iMtm );
        // delay the construction of iMtmUi and iMtmUiData until needed
        }

    if ( !iMtm->HasContext() || iMtm->Entry().EntryId() != aEntryId )
        {
        iMtm->SwitchCurrentEntryL( aEntryId );
        }

    MEBLOGGER_LEAVEFN( "CMsgEditorModel::SetEntryL" );
    }

// ---------------------------------------------------------
// CMsgEditorModel::PrepareMtmL
//
// Loads client and ui mtm's according to given message type.
// clientmtm context is set also (this prepares
// ---------------------------------------------------------
//
void CMsgEditorModel::PrepareMtmL(const TUid aMtmType)
{
    if (iMtm == NULL)
    {
        //delete iMtm;
        delete iMtmUi;
        delete iMtmUiData;

        //iMtm   = NULL;
        iMtmUi = NULL;
        iMtmUiData = NULL;

        iMtm   = iMtmReg->NewMtmL(aMtmType);
        //delay the construction of iMtmUi and iMtmUiData until needed

        // ClientMtm needs some context before CreateMessageL can be called.
        CMsvEntry* entry = Session().GetEntryL(KMsvRootIndexEntryId);

        CleanupStack::PushL(entry);

        // Note: Switch... makes copy of the given entry.
        //       Don't use SetCurrentEntry, because it does ownership transfer
        //       delete moves out of model.
        iMtm->SwitchCurrentEntryL(entry->EntryId());

        CleanupStack::PopAndDestroy();  // entry
    }
}

// ---------------------------------------------------------
// CMsgEditorModel::MtmUiL
// ---------------------------------------------------------
//
CBaseMtmUi& CMsgEditorModel::MtmUiL()
    {
    __ASSERT_DEBUG(iMtm, Panic(ENullPointer));
    if ( !iMtmUiReg )
        {
        iMtmUiReg = CMtmUiRegistry::NewL(*iSession);
        }       
    if ( !iMtmUi )
        {
        iMtmUi = iMtmUiReg->NewMtmUiL(*iMtm);
        }
    return *iMtmUi;        
    }

// ---------------------------------------------------------
// CMsgEditorModel::MtmUiDataL
// ---------------------------------------------------------
//
CBaseMtmUiData& CMsgEditorModel::MtmUiDataL()
    {
    __ASSERT_DEBUG(iMtm, Panic(ENullPointer));
    if ( !iMtmUiDataReg )
        {
        iMtmUiDataReg = CMtmUiDataRegistry::NewL(*iSession);
        }    
    if ( !iMtmUi )//incase MtmUi was not initialized
        {
        MtmUiL();
        }
    if ( !iMtmUiData )
        {
        iMtmUiData = iMtmUiDataReg->NewMtmUiDataLayerL(iMtmUi->Type());
        }                     
    return *iMtmUiData;
    }
    
// ---------------------------------------------------------
// CMsgEditorModel::HandleSessionEventL
//
// Handles session event observer, and call event handling functions in observer.
// ---------------------------------------------------------
//
void CMsgEditorModel::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3)
{
    MEBLOGGER_ENTERFN("CMsgEditorModel::HandleSessionEventL");
    MEBLOGGER_WRITE_TIMESTAMP("");

    switch (aEvent)
    {
    case EMsvEntriesCreated:
        MEBLOGGER_WRITE("EMsvEntriesCreated");
        // no action
        break;

    case EMsvEntriesChanged:
        {
            MEBLOGGER_WRITE("EMsvEntriesChanged");

            if (iMtm == NULL || !(iMtm->HasContext()))
                break; // no entry => ignore event

            // check if the current entry has been changed
            const CMsvEntrySelection* sel = static_cast<CMsvEntrySelection*>(aArg1);

            __ASSERT_DEBUG(sel, Panic(ENullPointer));

            const TInt index = sel->Find(Entry().Id());

            // if it has (and if there is model observer defined) ...
            if ((index != KErrNotFound) && iModelObserver)
            {
                iModelObserver->HandleEntryChangeL();
            }
        }
        break;

    case EMsvEntriesDeleted:
        {
            MEBLOGGER_WRITE("EMsvEntriesDeleted");

            if (iMtm == NULL || !(iMtm->HasContext()))
                break; // no entry => ignore event

            // check if the current entry has been deleted
            const CMsvEntrySelection* sel = static_cast<CMsvEntrySelection*>(aArg1);

            __ASSERT_DEBUG(sel, Panic(ENullPointer));

            const TInt index = sel->Find(Entry().Id());

            // notify the application, if necessary
            if ((index != KErrNotFound) && iModelObserver)
            {
                iModelObserver->HandleEntryDeletedL();
            }
        }
        break;

    case EMsvEntriesMoved:
        {
            MEBLOGGER_WRITE("EMsvEntriesMoved");

            if (iMtm == NULL || !(iMtm->HasContext()))
                break; // no entry => ignore event

            // check if the current entry has been moved
            const CMsvEntrySelection* sel = static_cast<CMsvEntrySelection*>(aArg1);
            const TMsvId newParent = reinterpret_cast<TMsvId>(aArg2);
            const TMsvId oldParent = reinterpret_cast<TMsvId>(aArg3);

            __ASSERT_DEBUG(sel,       Panic(ENullPointer));
            __ASSERT_DEBUG(newParent, Panic(ENullPointer));
            __ASSERT_DEBUG(oldParent, Panic(ENullPointer));

            const TInt index = sel->Find(Entry().Id());

            // notify the application, if necessary
            if ((index != KErrNotFound) && iModelObserver)
            {
                iModelObserver->HandleEntryMovedL(oldParent, newParent);
            }
        }
        break;

    case EMsvMtmGroupInstalled:
        MEBLOGGER_WRITE("EMsvMtmGroupInstalled");
        // no action
        break;

    case EMsvMtmGroupDeInstalled:
        {
            MEBLOGGER_WRITE("EMsvMtmGroupDeInstalled");

            if (iMtm == NULL || !(iMtm->HasContext()))
                break; // no entry => ignore event

            const TUid mtmId = TUid::Uid(reinterpret_cast<TInt>(aArg2));

            if (mtmId == Mtm().Type() && iModelObserver)
            {
                iModelObserver->HandleMtmGroupDeinstalledL();
            }
        }
        break;

    case EMsvGeneralError:
        {
            MEBLOGGER_WRITE("EMsvGeneralError");

            const TInt errorCode = reinterpret_cast<TInt>(aArg1);

            if (iModelObserver)
            {
                iModelObserver->HandleGeneralErrorL(errorCode);
            }
        }
        break;

    case EMsvCloseSession:
        MEBLOGGER_WRITE("EMsvCloseSession");

        if (iModelObserver)
        {
            iModelObserver->HandleCloseSessionL();
        }
        break;

    case EMsvServerReady:
        MEBLOGGER_WRITE("EMsvServerReady");

        iMediaAvailable = ETrue;
#ifdef ASYNC_CONST
        CompleteConstructL();
        if (iWait)
        {
            CActiveScheduler::Stop();
        }
#endif
        break;

    case EMsvServerFailedToStart:
        MEBLOGGER_WRITE("EMsvServerFailedToStart");

        iMediaAvailable = EFalse;
        if (iModelObserver)
        {
            iModelObserver->HandleServerFailedToStartL();
        }
        break;

    case EMsvServerTerminated:
        MEBLOGGER_WRITE("EMsvServerTerminated");

        iMediaAvailable = EFalse;
        if (iModelObserver)
        {
            iModelObserver->HandleServerTerminatedL();
        }
        break;

    case EMsvMediaChanged:
        MEBLOGGER_WRITE("EMsvMediaChanged");

        // media is available, although it may not be the same one any more
        iMediaAvailable = ETrue;
        if (iModelObserver)
        {
            iModelObserver->HandleMediaChangedL();
        }
        break;

    case EMsvMediaUnavailable:
        MEBLOGGER_WRITE("EMsvMediaUnavailable");

        iMediaAvailable = EFalse;
        if (iModelObserver)
        {
            iModelObserver->HandleMediaUnavailableL();
        }
        break;

    case EMsvMediaAvailable:
        MEBLOGGER_WRITE("EMsvMediaAvailable");

        iMediaAvailable = ETrue;
        if (iModelObserver)
        {
            iModelObserver->HandleMediaAvailableL();
        }
        break;

    case EMsvMediaIncorrect:
        MEBLOGGER_WRITE("EMsvMediaIncorrect");

        iMediaAvailable = EFalse;
        if (iModelObserver)
        {
            iModelObserver->HandleMediaIncorrectL();
        }
        break;

    case EMsvCorruptedIndexRebuilding:
        MEBLOGGER_WRITE("EMsvCorruptedIndexRebuilding");

        iMediaAvailable = EFalse;
        if (iModelObserver)
        {
            iModelObserver->HandleCorruptedIndexRebuildingL();
        }
        break;

    case EMsvCorruptedIndexRebuilt:
        MEBLOGGER_WRITE("EMsvCorruptedIndexRebuilt");

        // iMediaAvailable will be updated by EMsvServerReady event
        if (iModelObserver)
        {
            iModelObserver->HandleCorruptedIndexRebuiltL();
        }
        break;

    default:
        __ASSERT_DEBUG(EFalse, Panic(EUnknownSessionEvent));
        break;
    }

    MEBLOGGER_LEAVEFN("CMsgEditorModel::HandleSessionEventL");
}

// ========== OTHER EXPORTED FUNCTIONS =====================

//  End of File
