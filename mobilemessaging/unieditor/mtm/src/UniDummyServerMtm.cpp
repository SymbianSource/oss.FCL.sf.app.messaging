/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Dummy Server Mtm
*
*/



// INCLUDE FILES

#include <e32cmn.h>
#include <msventry.h>

#include "UniDummyServerMtm.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CUniDummyServerMtm::CUniDummyServerMtm(
    CRegisteredMtmDll& aRegisteredMtmDll,
    CMsvServerEntry* aInitialEntry )
    : CBaseServerMtm( aRegisteredMtmDll, aInitialEntry )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// Factory function
// ---------------------------------------------------------
//
EXPORT_C CUniDummyServerMtm* CUniDummyServerMtm::NewL(
    CRegisteredMtmDll& aRegisteredMtmDll,
    CMsvServerEntry* aInitialEntry )
    {
    
    CleanupStack::PushL( aInitialEntry ); // Take ownership of aInitialEntry
    CUniDummyServerMtm* self = new ( ELeave ) CUniDummyServerMtm(
        aRegisteredMtmDll, aInitialEntry );
    CleanupStack::Pop( aInitialEntry ); //  Now safely stored in member
    return self;
    }

    
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CUniDummyServerMtm::~CUniDummyServerMtm()
    {
    Cancel(); // cancel anything that may be pending...
    }


// ---------------------------------------------------------
// CUniDummyServerMtm::CopyToLocalL
// 
// ---------------------------------------------------------
//
void CUniDummyServerMtm::CopyToLocalL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aDestination*/,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ---------------------------------------------------------
// CUniDummyServerMtm::CopyFromLocalL
// 
// ---------------------------------------------------------
//
void CUniDummyServerMtm::CopyFromLocalL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aDestination*/,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ---------------------------------------------------------
// CUniDummyServerMtm::CopyWithinServiceL
// 
// ---------------------------------------------------------
//
void CUniDummyServerMtm::CopyWithinServiceL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aDestination*/,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ---------------------------------------------------------
// CUniDummyServerMtm::MoveToLocalL
// 
// ---------------------------------------------------------
//
void CUniDummyServerMtm::MoveToLocalL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aDestination*/,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ---------------------------------------------------------
// CUniDummyServerMtm::MoveFromLocalL
// 
// ---------------------------------------------------------
//
void CUniDummyServerMtm::MoveFromLocalL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aDestination*/,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ---------------------------------------------------------
// CUniDummyServerMtm::MoveWithinServiceL
// 
// ---------------------------------------------------------
//
void CUniDummyServerMtm::MoveWithinServiceL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aDestination*/,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ---------------------------------------------------------
// CUniDummyServerMtm::DeleteAllL
// 
// ---------------------------------------------------------
//
void CUniDummyServerMtm::DeleteAllL(
    const CMsvEntrySelection& /* aSelection */,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ---------------------------------------------------------
// CUniDummyServerMtm::CreateL
// 
// ---------------------------------------------------------
//
void CUniDummyServerMtm::CreateL(
    TMsvEntry /*aNewEntry*/,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ---------------------------------------------------------
// CUniDummyServerMtm::ChangeL
// 
// ---------------------------------------------------------
//
void CUniDummyServerMtm::ChangeL(
    TMsvEntry /* aNewEntry*/ ,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ---------------------------------------------------------
// CUniDummyServerMtm::StartCommandL
// 
// ---------------------------------------------------------
//
void CUniDummyServerMtm::StartCommandL(
    CMsvEntrySelection& /*aSelection*/,
    TInt /*aCommand*/,
    const TDesC8& /*aParameter*/,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );
    }

// ---------------------------------------------------------
// CUniDummyServerMtm::CommandExpected
// 
// ---------------------------------------------------------
//
TBool CUniDummyServerMtm::CommandExpected()
    {
    // so far we don't expect anything
    return EFalse;
    }

// ---------------------------------------------------------
// CUniDummyServerMtm::Progress
// 
// ---------------------------------------------------------
//
const TDesC8& CUniDummyServerMtm::Progress()
    {
    // should load in latest progress, if something is going on
    return KNullDesC8();
    }        

// ---------------------------------------------------------
// CUniDummyServerMtm::DoCancel
// 
// ---------------------------------------------------------
//
void CUniDummyServerMtm::DoCancel()
    {
    }

// ---------------------------------------------------------
// CUniDummyServerMtm::DoRunL
// Active object completion
// Run is used in this object to clean up after operations have finished.
//
// ---------------------------------------------------------
//
void CUniDummyServerMtm::DoRunL()
    {
    }
    
// ---------------------------------------------------------
// CUniDummyServerMtm::DoComplete
// 
// Active object complete
// ---------------------------------------------------------
//
void CUniDummyServerMtm::DoComplete( TInt /*aError*/ )
    {
    }
    
// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
