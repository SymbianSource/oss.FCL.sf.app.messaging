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
*     CDMA Main settings dialog for Sms.
*
*/



// INCLUDE FILES
#include "smumsettingsdialogcdma.h"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSmumMainSettingsDialog::NewL
// Two-phased constructor.
// @TODO implement char set support for sending option for CDMA?
// -----------------------------------------------------------------------------
EXPORT_C CSmumMainSettingsDialog* CSmumMainSettingsDialog::NewL(
    CSmsSettings&   aSettings, 
    TInt            aTypeOfSettings,
    TInt&           aExitCode,
    TInt&           /*aCharSetSupportForSendingOptions*/,
    CSmsHeader*     aSmsHeader )
    {
    return CSmumMainSettingsDialogCDMA::NewL(
        aSettings, aTypeOfSettings, aExitCode, aSmsHeader);
    }

