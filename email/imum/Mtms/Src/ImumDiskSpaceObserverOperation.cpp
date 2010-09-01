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
* Description: 
*     Class for observing free disk space and if disk space goes under critical
*     level this cancels itself.

*
*/


// INCLUDE FILES
#include <e32base.h>                            // Basic Symbian Includes
#include <MuiuMsvUiServiceUtilities.h>          // Muiu Utilities
#include <ImumInternalApi.h>        // CImumInternalApi

#include "ImumDiskSpaceObserverOperation.h"
#include "ImumDiskSpaceObserver.h"              // CImumDiskSpaceObserver
#include "ImumMtmLogging.h"

// CONSTANTS
const TUint KImumCriticalThreshold = 131072;
const TUint KImumSafetyMargin = 204800;

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserverOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
CImumDiskSpaceObserverOperation::CImumDiskSpaceObserverOperation(
    CImumInternalApi& aMailboxApi,
    TInt aPriority,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter)
    :
    CImumOnlineOperation(
        aMailboxApi,
        aPriority,
        aObserverRequestStatus,
        aReporter)
    {
    IMUM_CONTEXT( CImumDiskSpaceObserverOperation::CImumDiskSpaceObserverOperation, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserverOperation::BaseConstructL()
// ----------------------------------------------------------------------------
void CImumDiskSpaceObserverOperation::BaseConstructL(TUid aMtmType)
    {
    IMUM_CONTEXT( CImumDiskSpaceObserverOperation::BaseConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    iDiskObserver = CImumDiskSpaceObserver::NewL(
        *this, iMsvSession.FileSession() );
    
    // Create own instance of API because ImumDiskSpaceObserver outlives
    // the owner of iMailboxApi reference in ImumOnlineOperation
    iEmailApi = CreateEmailApiL();
    
    CImumOnlineOperation::BaseConstructL( aMtmType );
    
    IMUM_OUT();
    }


// ----------------------------------------------------------------------------
// CImumDiskSpaceObserverOperation::~CImumDiskSpaceObserverOperation()
// ----------------------------------------------------------------------------
CImumDiskSpaceObserverOperation::~CImumDiskSpaceObserverOperation()
    {
    IMUM_CONTEXT( CImumDiskSpaceObserverOperation::~CImumDiskSpaceObserverOperation, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iEmailApi;
    delete iDiskObserver;
    iDiskObserver = NULL;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserverOperation::DoCancel()
// ----------------------------------------------------------------------------
//
void CImumDiskSpaceObserverOperation::DoCancel()
    {
    IMUM_CONTEXT( CImumDiskSpaceObserverOperation::DoCancel, 0, KImumMtmLog );
    IMUM_IN();
    
    iDiskObserver->Cancel();
    CImumOnlineOperation::DoCancel();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserverOperation::DoDiskSpaceCheckL()
// ----------------------------------------------------------------------------
//
TInt CImumDiskSpaceObserverOperation::DoDiskSpaceCheckL()
    {
    IMUM_CONTEXT( CImumDiskSpaceObserverOperation::DoDiskSpaceCheckL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Make sure we are operating in safe levels
    if( !MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelWithOverheadL(
    	iEmailApi->MsvSession(), 0, KImumSafetyMargin ) )
        {
        // Get current drive
        TInt currentDrive = iEmailApi->MsvSession().CurrentDriveL();
        TInt64 currentLimit = KImumCriticalThreshold + KImumSafetyMargin;

        // Enough space, issue new request
        iDiskObserver->Cancel();
        iDiskObserver->NotifyDiskSpace( currentLimit, currentDrive );
        IMUM_OUT();

        return KErrNone;
        }
    IMUM_OUT();

    // Disk full for operation, indicate with error
    return KErrDiskFull;
    }

// ----------------------------------------------------------------------------
// CImumDiskSpaceObserverOperation::HandleDiskSpaceEvent()
// ----------------------------------------------------------------------------
//
void CImumDiskSpaceObserverOperation::HandleDiskSpaceEvent()
    {
    IMUM_CONTEXT( CImumDiskSpaceObserverOperation::HandleDiskSpaceEvent, 0, KImumMtmLog );
    IMUM_IN();
    
    Cancel();
    IMUM_OUT();
    }

//  End of File


