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
* Description:  Static helper methods related to CMailMessage.
*
*/


// INCLUDE FILES
#include "mailmessageutils.h"            // Own header
#include <CMailMessage.h>
#include <SendUiConsts.h>
#include <msvids.h>
#include <msvapi.h>

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// MailMessageUtils::IsOutgoingL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MailMessageUtils::IsOutgoingL( CMailMessage& aMessage )
    {
    TBool retVal( EFalse );
    if ( aMessage.MessageType() == KSenduiMtmSmtpUid )
        {
        retVal = ETrue;
        }
    
    return retVal;
    }
    	        
// End of File
