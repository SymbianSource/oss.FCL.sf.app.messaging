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
* Description:  ImumDomainApiImpl.cpp
 *
*/


#include "ImumDomainApiImpl.h"
#include "ImumDaMailboxUtilitiesImpl.h"
#include "ImumDaMailboxServicesImpl.h"


// ============================ LOCAL FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CreateDomainApiL()
// ----------------------------------------------------------------------------
//
EXPORT_C CImumDomainApi* CreateDomainApiL( CMsvSession* aMsvSession )
    {
    return CImumDomainApiImpl::NewL( aMsvSession );
    }

// ----------------------------------------------------------------------------
// CreateDomainApiLC()
// ----------------------------------------------------------------------------
//
EXPORT_C CImumDomainApi* CreateDomainApiLC( CMsvSession* aMsvSession )
    {
    return CImumDomainApiImpl::NewLC( aMsvSession );
    }

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CImumDomainApiImpl::CImumDomainApiImpl()
// ----------------------------------------------------------------------------
//
CImumDomainApiImpl::CImumDomainApiImpl()
    :
    iApi( NULL ),
    iDaMailboxUtilities( NULL ),
    iDaMailboxServices( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CImumDomainApiImpl::~CImumDomainApiImpl()
// ----------------------------------------------------------------------------
//
CImumDomainApiImpl::~CImumDomainApiImpl()
    {
    delete iApi;
    iApi = NULL;
    delete iDaMailboxUtilities;
    iDaMailboxUtilities = NULL;
    delete iDaMailboxServices;
    iDaMailboxServices = NULL;
    }

// ----------------------------------------------------------------------------
// CImumDomainApiImpl::NewL()
// ----------------------------------------------------------------------------
//
CImumDomainApiImpl* CImumDomainApiImpl::NewL( CMsvSession* aMsvSession )
    {
    CImumDomainApiImpl* self = NewLC( aMsvSession );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumDomainApiImpl::NewLC()
// ----------------------------------------------------------------------------
//
CImumDomainApiImpl* CImumDomainApiImpl::NewLC( CMsvSession* aMsvSession )
    {
    CImumDomainApiImpl* self = new ( ELeave ) CImumDomainApiImpl();
    CleanupStack::PushL( self );
    self->ConstructL( aMsvSession );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumDomainApiImpl::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumDomainApiImpl::ConstructL( CMsvSession* aMsvSession )
    {
    iApi = CreateEmailApiL( aMsvSession );
    }

// ======================= INTERFACE IMPLEMENTATION ===========================

// ----------------------------------------------------------------------------
// From class CImumDomainApi
// CImumDomainApiImpl::MailboxServicesL()
// ----------------------------------------------------------------------------
//
MImumDaMailboxServices& CImumDomainApiImpl::MailboxServicesL()
    {
    if ( !iDaMailboxServices )
        {
        iDaMailboxServices = CImumDaMailboxServicesImpl::NewL( *iApi );
        }

    return *iDaMailboxServices;
    }

// ----------------------------------------------------------------------------
// From class CImumDomainApi
// CImumDomainApiImpl::MailboxUtilitiesL()
// ----------------------------------------------------------------------------
//
MImumDaMailboxUtilities& CImumDomainApiImpl::MailboxUtilitiesL()
    {
    if ( !iDaMailboxUtilities )
        {
        iDaMailboxUtilities = CImumDaMailboxUtilitiesImpl::NewL( *iApi );
        }

    return *iDaMailboxUtilities;
    }


