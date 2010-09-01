/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Message centre's settings session observer
*
*/



// INCLUDE FILES
#include <ErrorUI.h> // CErrorUI
#include <aknappui.h> // iAvkonAppUi
#include <avkon.mbg>

#include <mtudreg.h>
#include <MTMStore.h>
#include <MsgArrays.h> // CUidNameArray
#include <msvids.h>
#include <msvuids.h>

#include <MuiuMsvUiServiceUtilities.h> // MsvUiServiceUtilities
#include <MuiuMsvProgressReporterOperation.h>
#include <akninputblock.h> // CAknInputBlock
#include <SenduiMtmUids.h>

#include "MceSettingsSessionObserver.h"

// CONSTANTS

const TInt KMceArrayGranularity = 4;
const TInt KMceVisibleTextLength = KHumanReadableNameLength;

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CMceSettingsSessionObserver::Constructor
// ----------------------------------------------------
CMceSettingsSessionObserver::CMceSettingsSessionObserver( CMsvSession* aSession )
    :
    iSession( aSession ),
    iDeleteSession( EFalse )
    {
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::Constructor
// ----------------------------------------------------
void CMceSettingsSessionObserver::ConstructL()
    {
    if ( !iSession )
        {
        iSession = CMsvSession::OpenSyncL( *this );
        iDeleteSession = ETrue;
        }
    iUiRegistry = CMtmUiDataRegistry::NewL( *iSession );
    iMtmStore = CMtmStore::NewL( *iSession );
    iRootEntry=CMsvEntry::NewL(
        *iSession,
        KMsvRootIndexEntryId,
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::Constructor
// ----------------------------------------------------
EXPORT_C CMceSettingsSessionObserver* CMceSettingsSessionObserver::NewL( CMsvSession* aSession )
    {
    CMceSettingsSessionObserver* self = new ( ELeave ) CMceSettingsSessionObserver( aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::Destructor
// ----------------------------------------------------
EXPORT_C CMceSettingsSessionObserver::~CMceSettingsSessionObserver()
    {
    delete iRunningOperation;
    delete iUiRegistry;
    delete iMtmStore;
    delete iRootEntry;
    if ( iDeleteSession )
        {
        delete iSession;
        }
    }


// ----------------------------------------------------
// CMceSettingsSessionObserver::Session
// ----------------------------------------------------
EXPORT_C CMsvSession& CMceSettingsSessionObserver::Session()
    {
    return *iSession;
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::UiRegistry
// ----------------------------------------------------
EXPORT_C CMtmUiDataRegistry* CMceSettingsSessionObserver::UiRegistry() const
    {
    return iUiRegistry;
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::MtmStore
// ----------------------------------------------------
EXPORT_C CMtmStore* CMceSettingsSessionObserver::MtmStore() const
    {
    return iMtmStore;
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::HandleSessionEventL
// ----------------------------------------------------
void CMceSettingsSessionObserver::HandleSessionEventL( TMsvSessionEvent /*aEvent*/, 
                                                                                          TAny* /*aArg1*/, 
                                                                                          TAny* /*aArg2*/, 
                                                                                          TAny* /*aArg3*/ )
    {

    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::OpCompleted
// ----------------------------------------------------
void CMceSettingsSessionObserver::OpCompleted( CMsvSingleOpWatcher& aOpWatcher, 
                                                                            TInt aCompletionCode )
    {
    CMsvOperation* op=&aOpWatcher.Operation();
    TRAP_IGNORE( DoOperationCompletedL( op, aCompletionCode ) );  

    if ( iRunningOperation->Operation().Id() == op->Id() )
        {
        delete iRunningOperation;
        iRunningOperation = NULL;
        }
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::DoOperationCompletedL
// ----------------------------------------------------
void CMceSettingsSessionObserver::DoOperationCompletedL(
    CMsvOperation* aOperation, 
    TInt aCompletionCode )
    {
    if ( !aOperation )
        {
        return;
        }

    if ( aCompletionCode == EEikCmdExit )
        {
        iAvkonAppUi->ProcessCommandL( EAknCmdExit );
        return;
        }

    const TDesC8& progress = aOperation->ProgressL();

    if ( progress.Length() == 0 )
        {
        return;
        }

    if ( aOperation->Mtm() != KUidMsvLocalServiceMtm )
        {
        CAknInputBlock::NewLC();
        iMtmStore->GetMtmUiLC( aOperation->Mtm() ).DisplayProgressSummary( progress );            
        CleanupStack::PopAndDestroy( 2 );  // CAknInputBlock, release mtmUi      
        return;
        }
    
    TMsvLocalOperationProgress localprogress = 
        McliUtils::GetLocalProgressL( *aOperation );     

    if ( ( aCompletionCode==KErrNone || aCompletionCode==KErrCancel) &&
        ( localprogress.iError==KErrNone || localprogress.iError==KErrCancel ) )
        {
        return;
        }

    CErrorUI* errorUi = CErrorUI::NewL( *CCoeEnv::Static() );
    CleanupStack::PushL( errorUi );
    errorUi->ShowGlobalErrorNoteL( localprogress.iError?localprogress.iError:aCompletionCode );
    CleanupStack::PopAndDestroy( errorUi );
    }


// ----------------------------------------------------
// CMceSettingsSessionObserver::CanCreateNewAccountL
// ----------------------------------------------------
EXPORT_C TBool CMceSettingsSessionObserver::CanCreateNewAccountL( TUid aMtm )
    {
    TMsvEntry serviceEntry;
    serviceEntry.iType=KUidMsvServiceEntry;
    serviceEntry.iMtm=aMtm;
    TInt rid;
    CBaseMtmUiData& uiData = iMtmStore->MtmUiDataL( aMtm );
    return uiData.CanCreateEntryL( iRootEntry->Entry(), serviceEntry, rid );
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::DeleteAccountL
// ----------------------------------------------------
EXPORT_C void CMceSettingsSessionObserver::DeleteAccountL( TMsvId aId )
    {
    User::LeaveIfError( iRunningOperation ? KErrInUse : KErrNone );
    TMsvEntry entry;
    TMsvId serviceId;
    User::LeaveIfError( iSession->GetEntry( aId, serviceId, entry ) );
    const TUid mtm = entry.iMtm;
    CBaseMtmUi* ui = &iMtmStore->ClaimMtmUiL( mtm );
    if ( ui )
        {
        CMtmStoreMtmReleaser::CleanupReleaseMtmUiLC( *iMtmStore, mtm );
        }

    CMsvOperation* op = NULL;    

// Request free disk space to be implemented
    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );
    if ( ui )
        {
        ui->BaseMtm().SetCurrentEntryL( iSession->GetEntryL( KMsvRootIndexEntryId ) );
        CAknInputBlock::NewLC();
        op = ui->DeleteServiceL( entry, singleOpWatcher->iStatus );
        CleanupStack::PopAndDestroy(); //CAknInputBlock
        CleanupStack::PushL( op );
        }
    else
        {
        // could not load mtm, so delete as normal local entry
        CMsvProgressReporterOperation* progOp = CMsvProgressReporterOperation::NewL(
            *iSession, singleOpWatcher->iStatus, EMbmAvkonQgn_note_erased );
        CleanupStack::PushL( progOp );

        CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection();
        CleanupStack::PushL( selection );
        selection->AppendL( entry.Id() );
        CMsvOperation* subOp = iRootEntry->DeleteL( *selection, progOp->RequestStatus() );
        CleanupStack::PopAndDestroy( selection );
        progOp->SetOperationL( subOp ); // this takes ownership immediately, so no cleanupstack needed.
        op = progOp;
        CleanupStack::PushL( op );
        }

    CleanupStack::Pop( op );
    CleanupStack::Pop( singleOpWatcher );
    singleOpWatcher->SetOperation( op );
    iRunningOperation = singleOpWatcher;    

// Cancel the free disk space request to be implemented

    if ( ui )
        {
        CleanupStack::PopAndDestroy();// release mtmUi
        }
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::CreateNewAccountL
// ----------------------------------------------------
EXPORT_C TBool CMceSettingsSessionObserver::CreateNewAccountL( TUid aMessageType, 
                                                                           TMsvId aOldServiceId /* = KMsvNullIndexEntryId */)
    {
    LeaveIfDiskSpaceUnderCriticalLevelL();
    User::LeaveIfError ( ( iRunningOperation ? KErrInUse : KErrNone ) );

    TMsvEntry nentry;
    nentry.iMtm = aMessageType;
    nentry.iType.iUid = KUidMsvServiceEntryValue;
    nentry.iDate.HomeTime();
    nentry.iServiceId = aOldServiceId;

    // --- Get the MTM UI relevant to the message type ---
    CBaseMtmUi& mtmUi = iMtmStore->GetMtmUiLC( nentry.iMtm );
    CMsvEntry* centry = iSession->GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( centry );
    CAknInputBlock::NewLC();

    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );

    CMsvOperation* op=mtmUi.CreateL(
        nentry,
        *centry,
        singleOpWatcher->iStatus );

    CleanupStack::PushL( op );
    singleOpWatcher->SetOperation( op );
    iRunningOperation = singleOpWatcher;
    CleanupStack::Pop( op );
    CleanupStack::Pop( singleOpWatcher );

    TBool accountCreated = ( op->iStatus != KErrCancel );
    // if not created, imum returns completed operation with KErrCancel

    CleanupStack::PopAndDestroy(); // command absorb
    CleanupStack::PopAndDestroy( centry );
    CleanupStack::PopAndDestroy(); // mtm ui release
    return accountCreated;
    }


// ----------------------------------------------------
// CMceSettingsSessionObserver::EditAccountL
// ----------------------------------------------------
EXPORT_C void CMceSettingsSessionObserver::EditAccountL( TMsvId aId )
    {
    LeaveIfDiskSpaceUnderCriticalLevelL();
    User::LeaveIfError ( ( iRunningOperation ? KErrInUse : KErrNone ) );

    TMsvEntry entry;
    TMsvId serviceId;
    User::LeaveIfError( iSession->GetEntry( aId, serviceId, entry ) );

    if ( entry.iType == KUidMsvServiceEntry )
        {
        CBaseMtmUi& mtmUi=iMtmStore->GetMtmUiAndSetContextLC( entry );
        CAknInputBlock::NewLC();
        CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL( *this );
        CleanupStack::PushL( singleOpWatcher );
        CMsvOperation* op=mtmUi.EditL( singleOpWatcher->iStatus );
        CleanupStack::PushL( op );
        singleOpWatcher->SetOperation( op );
        iRunningOperation = singleOpWatcher;
        CleanupStack::Pop( op ); 
        CleanupStack::Pop( singleOpWatcher );
        CleanupStack::PopAndDestroy( 2 ); // CAknInputBlock, release mtmUi
        }
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::MtmAccountsL
// ----------------------------------------------------
EXPORT_C CUidNameArray* CMceSettingsSessionObserver::MtmAccountsL( TUid aType )
    {
    CUidNameArray* accounts = new (ELeave) CUidNameArray(
        KMceArrayGranularity );
    CleanupStack::PushL( accounts );

    CMsvEntrySelection* sel = NULL;

    if ( aType == KSenduiMtmSmtpUid )
        {
        sel = MsvUiServiceUtilities::GetListOfAccountsL(
            *iSession,
            ETrue );
        }
    else
        {
        sel = MsvUiServiceUtilities::GetListOfAccountsWithMTML(
            *iSession,
            aType,
            ETrue );
        }
    CleanupStack::PushL( sel );

    CMsvEntry* rootEntry=CMsvEntry::NewL(
        *iSession,
        KMsvRootIndexEntryId,
        TMsvSelectionOrdering(KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    delete iRootEntry;
    iRootEntry = rootEntry;

    TUid uid;
    TUid techType=iUiRegistry->TechnologyTypeUid( aType );
    const TInt numAccounts=sel->Count();
    for (TInt cc=0; cc<numAccounts; cc++)
        {
        uid.iUid=sel->At(cc);
        TMsvEntry tentry;
        TRAPD( err, ( tentry = iRootEntry->ChildDataL( uid.iUid ) ) );
        if ( err == KErrNone &&
            iUiRegistry->IsPresent( tentry.iMtm ) && 
            iUiRegistry->TechnologyTypeUid( tentry.iMtm ) == techType )
            {
            const TInt count = accounts->Count();
            TBool foundService = EFalse;
            //check that related service is not already added to array
            for ( TInt loop = 0; loop < count; loop ++ )
                {
                if ( tentry.iRelatedId == ( *accounts )[loop].iUid.iUid )
                    {
                    foundService = ETrue;
                    break;
                    }
                }
            if ( !foundService )
                {
                TUidNameInfo info( uid, tentry.iDetails.Left( KMceVisibleTextLength ) );
                accounts->AppendL( info );
                }
            }
        }
    CleanupStack::PopAndDestroy( sel );
    CleanupStack::Pop( accounts );
    return accounts;
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::IsPresent
// ----------------------------------------------------
EXPORT_C TBool CMceSettingsSessionObserver::IsPresent( TUid aMtm ) const
    {
    return iUiRegistry->IsPresent( aMtm );
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::LeaveIfDiskSpaceUnderCriticalLevelL
// ----------------------------------------------------
void CMceSettingsSessionObserver::LeaveIfDiskSpaceUnderCriticalLevelL( TInt aBytesToWrite ) const
    {
    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL( *iSession, aBytesToWrite) )
        {
        User::Leave( KErrDiskFull );
        }
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::CancelFreeDiskSpaceRequest
// Static function.
// ----------------------------------------------------
void CMceSettingsSessionObserver::CancelFreeDiskSpaceRequest(TAny* /*aAny*/)
    {
    // Cancel free disk space request to be implemented
    }

// ----------------------------------------------------
// CMceSettingsSessionObserver::MtmName
//
// ----------------------------------------------------
EXPORT_C THumanReadableName CMceSettingsSessionObserver::MtmName( TUid aMtm ) const
    {
    return iUiRegistry->RegisteredMtmDllInfo( aMtm ).HumanReadableName();
    }
    
// ----------------------------------------------------
// CMceSettingsSessionObserver::SetChangeMessageStore
//
// ----------------------------------------------------
EXPORT_C void CMceSettingsSessionObserver::SetChangeMessageStore( TBool /*aChangeEnded*/ )
    {
    // do nothing...
    }
