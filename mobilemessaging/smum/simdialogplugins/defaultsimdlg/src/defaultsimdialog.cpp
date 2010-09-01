/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: default sim-dialog plugin implemntation
 *
 */

// INCLUDE FILES
#include <aknViewAppUi.h>               // iAvkonViewAppUi
#include "defaultsimdialog.h"
#include "smssimdialog.h"

#include <defaultsimdialog.rsg>                     // resource ids

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CDefaultSimDlg::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CDefaultSimDlg* CDefaultSimDlg::NewL( TSimDlgParams* aParams)
{
    CDefaultSimDlg* self = CDefaultSimDlg::NewLC( aParams);
    CleanupStack::Pop( self );

    return self;
}

// ----------------------------------------------------------------------------
// CDefaultSimDlg::NewLC
// Two Phase Construction
// ----------------------------------------------------------------------------
CDefaultSimDlg* CDefaultSimDlg::NewLC( TSimDlgParams* aParams)
{
    CDefaultSimDlg* self = new( ELeave ) CDefaultSimDlg(aParams);
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
}

// ----------------------------------------------------------------------------
// CDefaultSimDlg::CDefaultSimDlg
// Constructor
// ----------------------------------------------------------------------------
CDefaultSimDlg::CDefaultSimDlg(TSimDlgParams* aParams)
:iSmsMtmUi(aParams->iSmsMtmUi),iSession(aParams->iSession)
{
}

// ----------------------------------------------------------------------------
// CDefaultSimDlg::ConstructL
// 2nd phase constructor
// ----------------------------------------------------------------------------
void CDefaultSimDlg::ConstructL()
{
	
}

// ----------------------------------------------------------------------------
// CDefaultSimDlg::~CDefaultSimDlg
// Constructor
// ----------------------------------------------------------------------------
CDefaultSimDlg::~CDefaultSimDlg()
{
   
}


// ----------------------------------------------------------------------------
// CDefaultSimDlg::LaunchL
// 
// ----------------------------------------------------------------------------
void CDefaultSimDlg::LaunchL()
{
	TInt exitCode = KErrNone;
    CSmsSimDialog* simDialog = CSmsSimDialog::NewL( iSmsMtmUi, iSession, exitCode );
    simDialog->ExecuteLD( R_SMS_DEFAULT_SIM_DIALOG );
    if ( exitCode == EEikCmdExit )
        {
        iAvkonViewAppUi->ProcessCommandL( EAknCmdExit );
        }
}

