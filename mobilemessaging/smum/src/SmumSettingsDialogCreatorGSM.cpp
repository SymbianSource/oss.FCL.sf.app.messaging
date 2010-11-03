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
*     Main settings dialog for Sms.
*
*/



// INCLUDE FILES
#include "SmumSettingsDialogGSM.h"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSmumMainSettingsDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CSmumMainSettingsDialog* CSmumMainSettingsDialog::NewL(
    CSmsSettings&   aSettings, 
    TInt            aTypeOfSettings,
    TInt&           aExitCode,
    TInt&           aCharSetSupportForSendingOptions,
    CSmsHeader*     aSmsHeader )
    {
    return CSmumMainSettingsDialogGSM::NewL(
        aSettings, aTypeOfSettings, aExitCode, 
        aCharSetSupportForSendingOptions, aSmsHeader );
    }
//  End of File  

