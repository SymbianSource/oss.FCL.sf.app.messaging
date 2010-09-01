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
* Description: ImumDaMailboxUtilitiesImpl.cpp
*
*/


#include <ImumInHealthServices.h>
#include <ImumInMailboxUtilities.h>
#include <ImumInMailboxServices.h>

#include "ImumDaMailboxUtilitiesImpl.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CImumDaMailboxUtilitiesImpl::CImumDaMailboxUtilitiesImpl()
// ----------------------------------------------------------------------------
//
CImumDaMailboxUtilitiesImpl::CImumDaMailboxUtilitiesImpl(
    CImumInternalApi& aMailboxApi )
    :
    iMailboxApi( aMailboxApi ),
    iUtils( NULL )
    {
    }
// ----------------------------------------------------------------------------
// CImumDaMailboxUtilitiesImpl::~CImumDaMailboxUtilitiesImpl()
// ----------------------------------------------------------------------------
//
CImumDaMailboxUtilitiesImpl::~CImumDaMailboxUtilitiesImpl()
    {
    iUtils = NULL;
    }

// ----------------------------------------------------------------------------
// CImumDaMailboxUtilitiesImpl::NewL()
// ----------------------------------------------------------------------------
//
CImumDaMailboxUtilitiesImpl* CImumDaMailboxUtilitiesImpl::NewL(
    CImumInternalApi& aMailboxApi)
    {
    CImumDaMailboxUtilitiesImpl* self = NewLC( aMailboxApi );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumDaMailboxUtilitiesImpl::NewLC()
// ----------------------------------------------------------------------------
//
CImumDaMailboxUtilitiesImpl* CImumDaMailboxUtilitiesImpl::NewLC(
    CImumInternalApi& aMailboxApi)
    {
    CImumDaMailboxUtilitiesImpl* self =
        new ( ELeave ) CImumDaMailboxUtilitiesImpl( aMailboxApi );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumDaMailboxUtilitiesImpl::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumDaMailboxUtilitiesImpl::ConstructL()
    {
    iUtils = const_cast<MImumInMailboxUtilities*>(
        &iMailboxApi.MailboxUtilitiesL() );
    }

// ======================= INTERFACE IMPLEMENTATION ===========================

// ----------------------------------------------------------------------------
// From class MImumDaMailboxUtilities
// CImumDaMailboxUtilitiesImpl::DefaultMailboxId()
// ----------------------------------------------------------------------------
//
TMsvId CImumDaMailboxUtilitiesImpl::DefaultMailboxId() const
    {
    return iUtils->DefaultMailboxId( ETrue );
    }

// ----------------------------------------------------------------------------
// From class MImumDaMailboxUtilities
// CImumDaMailboxUtilitiesImpl::IsMailMtm()
// ----------------------------------------------------------------------------
//
TBool CImumDaMailboxUtilitiesImpl::IsMailMtm(
    const TUid& aMtm,
    const TBool& aAllowExtended ) const
    {
    return iUtils->IsMailMtm( aMtm, aAllowExtended );
    }

// ----------------------------------------------------------------------------
// From class MImumDaMailboxUtilities
// CImumDaMailboxUtilitiesImpl::IsMailbox()
// ----------------------------------------------------------------------------
//
TBool CImumDaMailboxUtilitiesImpl::IsMailbox(
    const TMsvId aMailboxId ) const
    {
    return iUtils->IsMailbox( aMailboxId );
    }

// ----------------------------------------------------------------------------
// From class MImumDaMailboxUtilities
// CImumDaMailboxUtilitiesImpl::IsMailbox()
// ----------------------------------------------------------------------------
//
TBool CImumDaMailboxUtilitiesImpl::IsMailbox(
    const TMsvEntry& aMailbox ) const
    {
    return iUtils->IsMailbox( aMailbox );
    }

// ----------------------------------------------------------------------------
// From class MImumDaMailboxUtilities
// CImumDaMailboxUtilitiesImpl::GetMailboxEntryL()
// ----------------------------------------------------------------------------
//
TMsvEntry CImumDaMailboxUtilitiesImpl::GetMailboxEntryL(
    const TMsvId aMailboxId,
    const TImumDaMboxRequestType& aType ) const
    {
    return iUtils->GetMailboxEntryL(
        aMailboxId,
        static_cast<MImumInMailboxUtilities::TImumInMboxRequest>( aType ) );
    }

// ----------------------------------------------------------------------------
// From class MImumDaMailboxUtilities
// CImumDaMailboxUtilitiesImpl::GetMailboxEntriesL()
// ----------------------------------------------------------------------------
//
const TUid& CImumDaMailboxUtilitiesImpl::GetMailboxEntriesL(
    const TMsvId aMailboxId,
    RMsvEntryArray& aEntries,
    const TBool aResetArray ) const
    {
    return iUtils->GetMailboxEntriesL(
        aMailboxId,
        aEntries,
        aResetArray );
    }

// ---------------------------------------------------------------------------
// From class MImumDaMailboxUtilities.
// CImumDaMailboxUtilitiesImpl::IsMailboxHealthy()
// ---------------------------------------------------------------------------
//
TBool CImumDaMailboxUtilitiesImpl::IsMailboxHealthy(
    const TMsvId aMailboxId ) const
    {
    TBool healthy = EFalse;
    TRAP_IGNORE( healthy = iMailboxApi.HealthServicesL().IsMailboxHealthy( aMailboxId ) );
    // If HealthServicesL should leave, healthy == EFalse;

    return healthy;
    }


// ----------------------------------------------------------------------------
// From class MImumDaMailboxUtilities
// CImumDaMailboxUtilitiesImpl::GetMailboxesL()
// ----------------------------------------------------------------------------
//
MImumDaMailboxUtilities::RMailboxIdArray
    CImumDaMailboxUtilitiesImpl::GetMailboxesL(
        const TInt64 aFlags) const
    {
    RMailboxIdArray aMailboxArray;

    iMailboxApi.HealthServicesL().GetMailboxList(
        aMailboxArray,
        ( aFlags | MImumInHealthServices::EFlagGetHealthy ),
        ETrue );

    return aMailboxArray;
    }
