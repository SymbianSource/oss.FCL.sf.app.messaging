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
*       Provides connection progress
*
*/


#include <eikenv.h>
#include <eikbtgpc.h>
#include <eikseced.h>
#include <iapprefs.h>
#include <smtpset.h>
#include <pop3set.h>
#include <imapset.h>

#include "EmailUtils.H"
#include "ComDbUtl.h"
#include "MsvConnectionValidation.h"
#include "MsvEmailConnectionProgressProvider.h"
#include <AknQueryDialog.h>
#include <SenduiMtmUids.h>
#include "PROGTYPE.H"       // Progress types.
#include "ImumPanic.h"
#include "ImumMtmLogging.h"

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
CMsvEmailConnectionProgressProvider* CMsvEmailConnectionProgressProvider::NewL(
    const TDesC& aIapName, TMsvId aService)
    {
    IMUM_STATIC_CONTEXT( CMsvEmailConnectionProgressProvider::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CMsvEmailConnectionProgressProvider* self =
        new(ELeave) CMsvEmailConnectionProgressProvider(aIapName, aService);
    IMUM_OUT();
    return self;
    }

// ----------------------------------------------------------------------------
// ~CMsvEmailConnectionProgressProvider
// ----------------------------------------------------------------------------
CMsvEmailConnectionProgressProvider::~CMsvEmailConnectionProgressProvider()
    {
    IMUM_CONTEXT( CMsvEmailConnectionProgressProvider::~CMsvEmailConnectionProgressProvider, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// GetConnectionProgressL
// ----------------------------------------------------------------------------
const TDesC8& CMsvEmailConnectionProgressProvider::GetConnectionProgressL(TInt /*aErrorCode*/)
    {
    IMUM_CONTEXT( CMsvEmailConnectionProgressProvider::GetConnectionProgressL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// CMsvEmailConnectionProgressProvider
// ----------------------------------------------------------------------------
CMsvEmailConnectionProgressProvider::CMsvEmailConnectionProgressProvider(
    const TDesC& aIapName, TMsvId aService)
    {
    IMUM_CONTEXT( CMsvEmailConnectionProgressProvider::CMsvEmailConnectionProgressProvider, 0, KImumMtmLog );
    IMUM_IN();
    
    TMsvEmailConnectionProgress& progress = iProgressBuf();
    progress.iType = EUiProgTypeConnecting;
    progress.iService = aService;
    iProgressBuf().iName = aIapName;
    IMUM_OUT();
    }

// End of File
