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
* Description:  
*     Application class for Mce.
*
*/

// system include files go here:

#include <iaupdate.h>
#include <iaupdateparameters.h>
#include <iaupdateresult.h>
#include <featmgr.h>

// user include files go here:
#include "mceui.h"
#include "mceiaupdateutils.h"
#include "MceLogText.h"

// CONSTANT 

// Defines the time span in hours for the next IAD update check event 
// which will be trigered.
const TInt KMceIADUpdateCheckRetryInterval = 24;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor.
// ---------------------------------------------------------------------------
//
CMceIAUpdateUtils::CMceIAUpdateUtils(CMceUi& aMceUi)
: CActive(EPriorityStandard),iUpdate( NULL ), iParameters( NULL ),iMceUi( aMceUi )
    {
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CMceIAUpdateUtils::ConstructL()
    {
    iUpdate = NULL;
    iParameters = NULL;
    // set it to current time.
    iPrevIADUpdateCheckTry.UniversalTime();
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMceIAUpdateUtils* CMceIAUpdateUtils::NewL(CMceUi& aMceUi)
    {
    CMceIAUpdateUtils* self = new( ELeave ) CMceIAUpdateUtils(aMceUi);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMceIAUpdateUtils::~CMceIAUpdateUtils()
    {
    Delete();
    Cancel();
    }

// ---------------------------------------------------------------------------
// Start IA update process.
// ---------------------------------------------------------------------------
//
void CMceIAUpdateUtils::DoStartL( const TUid aAppUid )
    {
    if(!iUpdate)
        {
        iUpdate = CIAUpdate::NewL( *this );
        }

    if( !iParameters )
        {
        iParameters = CIAUpdateParameters::NewL();
        }
        
    if( iUpdate && iParameters )
        {
        iParameters->SetUid( aAppUid );

        // Don't want any wait dialog.
        iParameters->SetShowProgress( EFalse );

        // Check the updates
        MCELOGGER_WRITE("StartL --- check updates");
        iUpdate->CheckUpdates( *iParameters );
        }
    else
        {
        Delete(); //Delete session with IAD server
        }
    }

// ---------------------------------------------------------------------------
// Cleanup function.
// ---------------------------------------------------------------------------
//
void CMceIAUpdateUtils::Delete()
    {
    if( iUpdate )
        {
        delete iUpdate;
        iUpdate = NULL;
        }
    if( iParameters )
        {
        delete iParameters;
        iParameters = NULL;
        }
    }

// -----------------------------------------------------------------------------
// From class MIAUpdateObserver.
// This callback function is called when the update checking operation has
// completed.
// -----------------------------------------------------------------------------
//
void CMceIAUpdateUtils::CheckUpdatesComplete( TInt aErrorCode,
        TInt aAvailableUpdates )
    {
    TBool bContinue = EFalse;
    
    if ( aErrorCode == KErrNone )
        {
        if ( aAvailableUpdates > 0 )
            {
                if((iMceUi.IsForeground())&&(iMceUi.MceViewActive(EMceMainViewActive)))
                    {
                    // There were some updates available.
                    MCELOGGER_WRITE("CheckUpdatesComplete --- updates available");
                    iUpdate->UpdateQuery();
                    bContinue = ETrue;
                    }
                else
                    {
                    MCELOGGER_WRITE("CheckUpdatesComplete --- But MessageView or Delivery Reports View is active");
                    }
            }
        else
            {
            // No updates available.
            MCELOGGER_WRITE("CheckUpdatesComplete --- no updates available");
            }
        }
    if(!bContinue)
        {
        Delete(); //Delete session with IAD server
        }
     
    }

// -----------------------------------------------------------------------------
// From class MIAUpdateObserver.
// This callback function is called when an update operation has completed.
// -----------------------------------------------------------------------------
//
void CMceIAUpdateUtils::UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResult )
    {
    if ( aErrorCode == KErrNone )
        {
        // The update process that the user started from IAUpdate UI is now
        // completed.
        // If the client application itself was updated in the update process,
        // this callback is never called, since the client is not running anymore.
        MCELOGGER_WRITE("UpdateComplete ---");
        TInt successCount = aResult->SuccessCount();
        }

    if( aResult )
        {
        // Ownership was transferred, so this must be deleted by the client.
        delete aResult;
        }

    // We do not need the client-server session anymore.
    Delete();
    }

// -----------------------------------------------------------------------------
// From class MIAUpdateObserver.
// This callback function is called when an update query operation has completed.
// -----------------------------------------------------------------------------
//
void CMceIAUpdateUtils::UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow )
    {
    TBool bContinue = EFalse;
    if ( aErrorCode == KErrNone )
        {
        if ( aUpdateNow )
            {
            // User choosed to update now, so let's launch the IAUpdate UI.
            MCELOGGER_WRITE("UpdateQueryComplete --- now");
            iUpdate->ShowUpdates( *iParameters );
            bContinue = ETrue;
            }
        else
            {
            // The answer was 'Later'.
            MCELOGGER_WRITE("UpdateQueryComplete --- later");
            }
        }
    if(!bContinue)
        {
        Delete(); //Delete session with IAD server
        }
    }
// -----------------------------------------------------------------------------
// From class MIAUpdateObserver.
// This callback function is called when an update query operation has completed.
// -----------------------------------------------------------------------------
//
void CMceIAUpdateUtils::StartL(const TUid aAppUid)
    {
    if (IsActive())
        {
            return;
         }
    iAppUid = aAppUid;
    CompleteSelf();
    }
// -----------------------------------------------------------------------------
// 
// For Setting the Active Object Active
// -----------------------------------------------------------------------------
//
void CMceIAUpdateUtils::CompleteSelf()
    {
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, KErrNone );
    }
// -----------------------------------------------------------------------------
// From class CActive.
// For Starting the update in a seperate Thread
// -----------------------------------------------------------------------------
//
void CMceIAUpdateUtils::RunL()
    {
    DoStartL( iAppUid );
    }
// -----------------------------------------------------------------------------
// From class CActive.
// Nothing to do here
// -----------------------------------------------------------------------------
//
void CMceIAUpdateUtils::DoCancel()
    {
    
    }

// ---------------------------------------------------------------------------
// Is IAD Update requried to do now
// YES, If the KMceIADUpdateCheckRetryInterval is over after the previous try
// NO, If the KMceIADUpdateCheckRetryInterval is not over after the previous retry 
// ---------------------------------------------------------------------------
//
TBool CMceIAUpdateUtils::IsUpdateRequired()
    {
    TBool required = EFalse;
    TTime currTime;
    TTimeIntervalHours hourInterval;
    TInt err = KErrNone;
    
    currTime.UniversalTime();
    
    err = currTime.HoursFrom(iPrevIADUpdateCheckTry,hourInterval);
    
    // KErrNone-> successful case, 
    // reset the iPrevIADUpdateCheckTry to current time, 
    // start IAD check update
    
    // KErrOverflow ->  if the calculated interval 
    // is too large for a 32-bit integer
    // reset the iPrevIADUpdateCheckTry to current time
    // start IAD check update
    
    // in all other cases don't do any thing.
    
    if(((err == KErrNone)&& 
        (hourInterval.Int() >= KMceIADUpdateCheckRetryInterval ))
        ||(err == KErrOverflow))        
        {
        iPrevIADUpdateCheckTry.UniversalTime();
        required = ETrue; 
        }
    
    return required;
    }
// EOF
