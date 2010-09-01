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
* Description: mtmstore implementation
*
*/



// INCLUDE FILES
#include <mtudreg.h>
#include <mtudcbas.h>
#include <mtclreg.h>
#include <mtclbase.h>
#include <mtuireg.h>
#include <mtmuibas.h>
#include <msvids.h>
#include "MTMStore.h"
#include "muiudomainpan.h"
#include "mtmstoreitem.h"
#include "mtmusagetimer.h"

// CONSTANTS
const TInt KMtmArrayGranularity = 5;


// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------
// CMtmStore::NewL
// ---------------------------------------------------------
//
EXPORT_C CMtmStore* CMtmStore::NewL( CMsvSession& aSession )
    {// static
    CMtmStore* self = new( ELeave ) CMtmStore( aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    return self;
    }



// ---------------------------------------------------------
// CMtmStore::CMtmStore
// ---------------------------------------------------------
//
CMtmStore::CMtmStore( CMsvSession& aSession )
:CBase(), iSession( aSession ), 
          iMtmUiInfoArray( KMtmArrayGranularity ), 
          iMtmUiDataArray( KMtmArrayGranularity )
    {
    }


// ---------------------------------------------------------
// CMtmStore::ConstructL
// ---------------------------------------------------------
//
void CMtmStore::ConstructL()
    {
    iBaseMtmRegistry = CClientMtmRegistry::NewL( iSession );
    iMtmUiRegistry = CMtmUiRegistry::NewL( iSession );
    iMtmUiDataRegistry = CMtmUiDataRegistry::NewL( iSession );
    }


// ---------------------------------------------------------
// CMtmStore::~CMtmStore
// ---------------------------------------------------------
//
EXPORT_C CMtmStore::~CMtmStore()
    {
    if ( iReleaser )
        {
        iReleaser->FreeOfObligations();
        }

    iMtmUiInfoArray.ResetAndDestroy();
    iMtmUiDataArray.ResetAndDestroy();
    delete iBaseMtmRegistry;
    delete iMtmUiRegistry;
    delete iMtmUiDataRegistry;
    }


// ---------------------------------------------------------
// CMtmStore::MtmUiDataL
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUiData& CMtmStore::MtmUiDataL( TUid aMtm )
    {
    TInt index = FindMtmUiDataIndex( aMtm );
    if ( index == KErrNotFound )
        {
        CBaseMtmUiData* uiData = iMtmUiDataRegistry->NewMtmUiDataLayerL( aMtm );
        if (!uiData)
            {
            User::Leave( KErrNotFound );
            }
        CleanupStack::PushL( uiData );
        index=iMtmUiDataArray.Count();
        iMtmUiDataArray.AppendL( uiData );
        CleanupStack::Pop( uiData );
        }
    
    __ASSERT_DEBUG( index != KErrNotFound, Panic( EMuiuMtmStoreMissingMtm ));
    return *iMtmUiDataArray[index];
    }

// ---------------------------------------------------------
// CMtmStore::ReleaseMtmUiData
// ---------------------------------------------------------
//
EXPORT_C void CMtmStore::ReleaseMtmUiData( TUid aMtm )
    {
    TInt index = FindMtmUiDataIndex( aMtm );

    if( index != KErrNotFound )
        {
        delete iMtmUiDataArray[index];
        iMtmUiDataArray.Delete( index );
        }
    }

// ---------------------------------------------------------
// CMtmStore::GetMtmUiLC
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUi& CMtmStore::GetMtmUiLC( TUid aMtm )
    {
    CBaseMtmUi& mtmUi = ClaimMtmUiL( aMtm );
    CMtmStoreMtmReleaser::CleanupReleaseMtmUiLC( *this, aMtm );
    return mtmUi;
    }


// ---------------------------------------------------------
// CMtmStore::GetMtmUiAndSetContextLC
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUi& CMtmStore::GetMtmUiAndSetContextLC( const TMsvEntry& aContext )
    {
    CBaseMtmUi& mtmUi = ClaimMtmUiAndSetContextL( aContext );
    CMtmStoreMtmReleaser::CleanupReleaseMtmUiLC( *this, aContext.iMtm );
    return mtmUi;
    }

// ---------------------------------------------------------
// CMtmStore::ClaimMtmUiL
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUi& CMtmStore::ClaimMtmUiL (TUid aMtm )
    {
    return DoClaimMtmUiL( aMtm ).MtmUi();
    }


// ---------------------------------------------------------
// CMtmStore::DoClaimMtmUiL
// ---------------------------------------------------------
//
CMtmUiInfoItem& CMtmStore::DoClaimMtmUiL( TUid aMtm )
    {
    TInt index = FindMtmUiIndex( aMtm );
    if ( index == KErrNotFound )
        {
        AddNewMtmUiHolderL( aMtm, index );
        }
    else
        {
        iMtmUiInfoArray[index]->IncreaseUsage();
        }
    __ASSERT_DEBUG( index != KErrNotFound, Panic( EMuiuMtmStoreMissingMtm ));
    return *iMtmUiInfoArray[index];
    }


// ---------------------------------------------------------
// CMtmStore::ClaimMtmUiAndSetContextL
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUi& CMtmStore::ClaimMtmUiAndSetContextL( const TMsvEntry& aContext )
    {
    CMtmUiInfoItem& mtmHolder = DoClaimMtmUiL( aContext.iMtm );
    CMsvEntry* contextEntry = iSession.GetEntryL( aContext.Id() );
    CleanupStack::PushL( contextEntry );

     // Takes ownership of contextEntry
    TRAPD( error, mtmHolder.Mtm().SetCurrentEntryL( contextEntry ) );  

    CleanupStack::Pop( contextEntry );

    if ( error )
        {
        ReleaseMtmUi( aContext.iMtm );
        User::Leave( error );
        }

    return mtmHolder.MtmUi();
    }


// ---------------------------------------------------------
// CMtmStore::AddNewMtmUiHolderL
// ---------------------------------------------------------
//
void CMtmStore::AddNewMtmUiHolderL( TUid aMtm, TInt& aNewIndex )
    {
    if ( !iBaseMtmRegistry->IsPresent( aMtm ) )
        {
        User::Leave( KErrNotFound );
        }
    
    CBaseMtm* baseMtm = iBaseMtmRegistry->NewMtmL( aMtm );
    CleanupStack::PushL( baseMtm );
    
    CBaseMtmUi* mtmUi = iMtmUiRegistry->NewMtmUiL( *baseMtm );
    CleanupStack::PushL( mtmUi );
    
    CMtmUsageTimer* timer = CMtmUsageTimer::NewLC( *this, aMtm );
    
    CMtmUiInfoItem* item = new( ELeave ) CMtmUiInfoItem( baseMtm, mtmUi, timer );
    CleanupStack::Pop( 3, baseMtm ); // timer, mtmUi, baseMtm
    
    CleanupStack::PushL( item );
    
    aNewIndex = iMtmUiInfoArray.Count();
    iMtmUiInfoArray.AppendL( item );
    CleanupStack::Pop( item );
    }


// ---------------------------------------------------------
// CMtmStore::ReleaseMtmUi
// ---------------------------------------------------------
//
EXPORT_C void CMtmStore::ReleaseMtmUi( TUid aMtm )
    {
    const TInt index = FindMtmUiIndex( aMtm );
    if ( index == KErrNotFound )
        {
#ifdef _DEBUG
        Panic( EMuiuMtmStoreMissingMtm );
#endif
        return;
        }
    
    iMtmUiInfoArray[index]->DecreaseUsage();
    // park entry if possible to save memory
    TRAPD( ignore, 
    iMtmUiInfoArray[index]->Mtm().SwitchCurrentEntryL( KMsvDeletedEntryFolderEntryIdValue ) );
    if( ignore )
        {
        ignore = KErrNone; //Prevent ARMV5 compilation varning
        }
    }


// ---------------------------------------------------------
// CMtmStore::MtmUsageTimedOut
// ---------------------------------------------------------
//
void CMtmStore::MtmUsageTimedOut( TUid aMtm )
    {
    const TInt index = FindMtmUiIndex( aMtm );
    if ( index == KErrNotFound )
        {
#ifdef _DEBUG
        Panic( EMuiuMtmStoreMissingMtm );
#endif
        return;
        }
    
    delete iMtmUiInfoArray[index];
    iMtmUiInfoArray.Delete( index );
    }


// ---------------------------------------------------------
// CMtmStore::FindMtmUiIndex
// ---------------------------------------------------------
//
TInt CMtmStore::FindMtmUiIndex( TUid aMtm ) const
    {
    for (TInt cc = iMtmUiInfoArray.Count(); --cc >= 0;)
        {
        if ( iMtmUiInfoArray[cc]->Mtm().Type() == aMtm )
            {
            return cc;
            }
        }
    return KErrNotFound;
    }


// ---------------------------------------------------------
// CMtmStore::FindMtmUiDataIndex
// ---------------------------------------------------------
//
TInt CMtmStore::FindMtmUiDataIndex( TUid aMtm ) const
    {
    for (TInt cc = iMtmUiDataArray.Count(); --cc >= 0;)
        {
        if ( iMtmUiDataArray[cc]->Type() == aMtm )
            {
            return cc;
            }
        }
    return KErrNotFound;
    }



// ---------------------------------------------------------
// CMtmStore::SetMtmStoreReleaser
// ---------------------------------------------------------
//
void CMtmStore::SetMtmStoreReleaser( CMtmStoreMtmReleaser* aReleaser )
    {
    iReleaser = aReleaser;
    }



// ---------------------------------------------------------
// CMtmStoreMtmReleaser::CleanupReleaseMtmUiLC
// ---------------------------------------------------------
//
EXPORT_C void CMtmStoreMtmReleaser::CleanupReleaseMtmUiLC( CMtmStore& aMtmStore, TUid aMtm )
    {
    CMtmStoreMtmReleaser* releaser = new( ELeave )CMtmStoreMtmReleaser( aMtmStore, aMtm );
    ASSERT( aMtmStore.ReleaserIsSet() == EFalse );
    aMtmStore.SetMtmStoreReleaser( releaser );
    CleanupStack::PushL( releaser );
    }



// ---------------------------------------------------------
// CMtmStoreMtmReleaser::CMtmStoreMtmReleaser
// ---------------------------------------------------------
//
CMtmStoreMtmReleaser::CMtmStoreMtmReleaser(CMtmStore& aMtmStore, TUid aMtm)
: CBase(), iMtmStore(&aMtmStore), iMtm(aMtm)
    {
    }


// ---------------------------------------------------------
// CMtmStoreMtmReleaser::~CMtmStoreMtmReleaser
// ---------------------------------------------------------
//
CMtmStoreMtmReleaser::~CMtmStoreMtmReleaser()
    {
    if ( iMtmStore )
        {
        iMtmStore->ReleaseMtmUi(iMtm);
        iMtmStore->SetMtmStoreReleaser( NULL );
        }
    //
    FreeOfObligations();
    }


// ---------------------------------------------------------
// CMtmStoreMtmReleaser::FreeOfObligations
// ---------------------------------------------------------
//
void CMtmStoreMtmReleaser::FreeOfObligations()
    {
    iMtmStore = NULL;
    }

// End of file
