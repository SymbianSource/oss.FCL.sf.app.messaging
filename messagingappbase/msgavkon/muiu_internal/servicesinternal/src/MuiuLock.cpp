/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   The global lock class for messaging components
*
*/



#include <centralrepository.h>    // CRepository
#include "MuiuLock.h"             // Own header file

// ---------------------------------------------------------------------------
// CMuiuLock::NewL
// Runs the first and second phase constructors
// ---------------------------------------------------------------------------
//
EXPORT_C CMuiuLock* CMuiuLock::NewL( TUid aLockId )
    {
    CMuiuLock* self = new (ELeave) CMuiuLock;
    CleanupStack::PushL( self );
    self->ConstructL( aLockId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMuiuLock::~CMuiuLock
// Destructor. Releases the lock if needed.
// ---------------------------------------------------------------------------
//
CMuiuLock::~CMuiuLock()
    {
    if ( iLocked )
        {
        Release();
        }
    delete iRepository;
    }

// ---------------------------------------------------------------------------
// CMuiuLock::Reserve
// Reserves the lock for the client by opening the read/write transaction to
// the central repository
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CMuiuLock::Reserve()
    {
    TInt status( KErrNone );
    
    if ( !iLocked )
        {
        status = iRepository->StartTransaction( 
            CRepository::EReadWriteTransaction );
            
        if ( status == KErrNone )
            {
            iLocked = ETrue;
            }
        }
    else
        {
        status = KErrGeneral;
        }
        
    return status;
    }

// ---------------------------------------------------------------------------
// CMuiuLock::Release
// Releases the lock by cancelling the transaction opened by Reserve()
// ---------------------------------------------------------------------------
//
EXPORT_C void CMuiuLock::Release()
    {
    if ( iLocked )
        {
        iRepository->CancelTransaction();
        iLocked = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CMuiuLock::CMuiuLock
// First phase constructor. Just initialises the data members.
// ---------------------------------------------------------------------------
//
CMuiuLock::CMuiuLock() :
    iRepository ( NULL ),
    iLocked ( EFalse )
    {
    // none
    }

// ---------------------------------------------------------------------------
// CMuiuLock::ConstructL
// Second phase constructor. Creates the central repository handle.
// ---------------------------------------------------------------------------
//
void CMuiuLock::ConstructL( TUid aLockId )
    {
    iRepository = CRepository::NewL( aLockId );
    }
