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
* Description:  ImumDaMailboxServicesImpl.cpp
 *
*/


#include <ImumInMailboxServices.h>

#include <e32base.h>
#include "ImumDaMailboxServicesImpl.h"
#include "ImumDaSettingsDataImpl.h"
#include "ImumInSettingsData.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CImumDaMailboxServicesImpl::CImumDaMailboxServicesImpl()
// ----------------------------------------------------------------------------
//
CImumDaMailboxServicesImpl::CImumDaMailboxServicesImpl(
    CImumInternalApi& aMailboxApi)
    :
    iMailboxApi( aMailboxApi ),
    iServices( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CImumDaMailboxServicesImpl::~CImumDaMailboxServicesImpl()
// ----------------------------------------------------------------------------
//
CImumDaMailboxServicesImpl::~CImumDaMailboxServicesImpl()
    {
    iServices = NULL;
    }

// ---------------------------------------------------------------------------
// CImumDaMailboxServicesImpl::NewL()
// ---------------------------------------------------------------------------
//
CImumDaMailboxServicesImpl* CImumDaMailboxServicesImpl::NewL(
    CImumInternalApi& aMailboxApi)
    {
    CImumDaMailboxServicesImpl* self = NewLC( aMailboxApi );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CImumDaMailboxServicesImpl::NewLC()
// ---------------------------------------------------------------------------
//
CImumDaMailboxServicesImpl* CImumDaMailboxServicesImpl::NewLC(
    CImumInternalApi& aMailboxApi)
    {
    CImumDaMailboxServicesImpl* self =
        new ( ELeave ) CImumDaMailboxServicesImpl( aMailboxApi );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CImumDaMailboxServicesImpl::ConstructL()
// ---------------------------------------------------------------------------
//
void CImumDaMailboxServicesImpl::ConstructL()
    {
    iServices = const_cast<MImumInMailboxServices*>(
        &iMailboxApi.MailboxServicesL() );
    }

// ======================= INTERFACE IMPLEMENTATION ===========================

// ---------------------------------------------------------------------------
// From class MImumDaMailboxServices
// CImumDaMailboxServicesImpl::CreateSettingsDataL()
// ---------------------------------------------------------------------------
//
CImumDaSettingsData* CImumDaMailboxServicesImpl::CreateSettingsDataL(
    const TUid& aProtocol ) const
    {
    return CImumDaSettingsDataImpl::NewL( iMailboxApi, aProtocol );
    }

// ---------------------------------------------------------------------------
// From class MImumDaMailboxServices
// CImumDaMailboxServicesImpl::CreateMailboxL()
// ---------------------------------------------------------------------------
//
TMsvId CImumDaMailboxServicesImpl::CreateMailboxL(
    CImumDaSettingsData& aSettingsData ) const
    {
    CImumDaSettingsDataImpl *inData =
        static_cast<CImumDaSettingsDataImpl*>( &aSettingsData );

    return iServices->CreateMailboxL( inData->GetInData() );
    }

// ---------------------------------------------------------------------------
// From class MImumDaMailboxServices
// CImumDaMailboxServicesImpl::SetDefaultMailboxL()
// ---------------------------------------------------------------------------
//
void CImumDaMailboxServicesImpl::SetDefaultMailboxL(
    const TMsvId aMailboxId ) const
    {
    iServices->SetDefaultMailboxL( aMailboxId );
    }



