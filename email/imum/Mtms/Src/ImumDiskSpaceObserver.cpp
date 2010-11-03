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
* Description:  ImumDiskSpaceObserver.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <MuiuMsvUiServiceUtilities.h>          // Muiu Utilities

#include "ImumDiskSpaceObserver.h"
#include "ImumMtmLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserver::CImumDiskSpaceObserver()
// ----------------------------------------------------------------------------
//
CImumDiskSpaceObserver::CImumDiskSpaceObserver(
    MImumDiskSpaceObserver& aObserver,
    RFs& aFileSession )
    :
    CActive( CActive::EPriorityStandard ),
    iDiskObserver( aObserver ),
    iFileSession( aFileSession )
    {
    IMUM_CONTEXT( CImumDiskSpaceObserver::CImumDiskSpaceObserver, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    }

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserver::~CImumDiskSpaceObserver()
// ----------------------------------------------------------------------------
//
CImumDiskSpaceObserver::~CImumDiskSpaceObserver()
    {
    IMUM_CONTEXT( CImumDiskSpaceObserver::~CImumDiskSpaceObserver, 0, KImumMtmLog );
    IMUM_IN();

    Cancel();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserver::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumDiskSpaceObserver::ConstructL()
    {
    IMUM_CONTEXT( CImumDiskSpaceObserver::ConstructL, 0, KImumMtmLog );
    IMUM_IN();

    CActiveScheduler::Add( this );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserver::NewL()
// ----------------------------------------------------------------------------
//
CImumDiskSpaceObserver* CImumDiskSpaceObserver::NewL(
    MImumDiskSpaceObserver& aObserver,
    RFs& aFileSession )
    {
    IMUM_STATIC_CONTEXT( CImumDiskSpaceObserver::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    CImumDiskSpaceObserver* self = NewLC( aObserver, aFileSession );
    CleanupStack::Pop( self );
    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserver::NewLC()
// ----------------------------------------------------------------------------
//
CImumDiskSpaceObserver* CImumDiskSpaceObserver::NewLC(
    MImumDiskSpaceObserver& aObserver,
    RFs& aFileSession )
    {
    IMUM_STATIC_CONTEXT( CImumDiskSpaceObserver::NewLC, 0, mtm, KImumMtmLog );
    IMUM_IN();

    CImumDiskSpaceObserver* self =
        new ( ELeave ) CImumDiskSpaceObserver( aObserver, aFileSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserver::DoCancel()
// ----------------------------------------------------------------------------
//
void CImumDiskSpaceObserver::DoCancel()
    {
    IMUM_CONTEXT( CImumDiskSpaceObserver::DoCancel, 0, KImumMtmLog );
    IMUM_IN();

    // Cancel notifications
    iFileSession.NotifyDiskSpaceCancel( iStatus );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserver::RunL()
// ----------------------------------------------------------------------------
//
void CImumDiskSpaceObserver::RunL()
    {
    IMUM_CONTEXT( CImumDiskSpaceObserver::RunL, 0, KImumMtmLog );
    IMUM_IN();

    iDiskObserver.HandleDiskSpaceEvent();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserver::NotifyDiskSpace()
// ----------------------------------------------------------------------------
//
void CImumDiskSpaceObserver::NotifyDiskSpace(
    const TInt64 aThreshold,
    const TInt aDrive )
    {
    IMUM_CONTEXT( CImumDiskSpaceObserver::NotifyDiskSpace, 0, KImumMtmLog );
    IMUM_IN();

    if ( !IsActive() )
        {
        iFileSession.NotifyDiskSpace( aThreshold, aDrive, iStatus );
        SetActive();
        }
    IMUM_OUT();
    }

// End of File

