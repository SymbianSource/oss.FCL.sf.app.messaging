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
* Description: SMSUMAIN.CPP*
*/



// INCLUDES
#include "smsui.pan"    				// for panics

// CONSTANTS
_LIT( KSMumMainPanicName, "SMSMtmUI" );

// Panic function
GLDEF_C void Panic( TSmsUiPanic aPanic )
    {
    User::Panic( KSMumMainPanicName, aPanic );
    }

//  End of File
