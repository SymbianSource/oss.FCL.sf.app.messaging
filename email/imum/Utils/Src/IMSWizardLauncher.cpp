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
*       Class implementation file
*
*/


// INCLUDE FILES

#include "IMSWizardLauncher.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CIMSWizardLauncher::CIMSWizardLauncher( CIMSSettingsWizard& aWizard,
										CIMSSettingsUi& aSettingsUi,
										TInt aPriority )
    : CActive( aPriority ),
      iWizard( aWizard ),
      iSettingsUi ( aSettingsUi )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CIMSWizardLauncher* CIMSWizardLauncher::NewL(
    CIMSSettingsWizard& aWizard,
    CIMSSettingsUi& aSettingsUi
     )
    {
    CIMSWizardLauncher* self =
        new ( ELeave ) CIMSWizardLauncher( aWizard, aSettingsUi );
    CleanupStack::PushL( self );
    self->StartLauncher();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CIMSWizardLauncher::~CIMSWizardLauncher()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    }

// ---------------------------------------------------------------------------
// Starts active object and completes the request to make activescheduler to call
// the RunL-function immediately
// ---------------------------------------------------------------------------
//
void CIMSWizardLauncher::StartLauncher()
    {
    iWizard.WizRunStart();

    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Not needed
// ---------------------------------------------------------------------------
//
void CIMSWizardLauncher::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Starts the email wizard to skip the info screen in the beginning
// ---------------------------------------------------------------------------
//
void CIMSWizardLauncher::RunL()
    {
    TBool stepsLeft = iWizard.WizRunStepL();

    if( stepsLeft )
    	{
        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        SetActive();
    	}
    else
    	{
	    iSettingsUi.DoQuitL();
		}
    }

//End of file
