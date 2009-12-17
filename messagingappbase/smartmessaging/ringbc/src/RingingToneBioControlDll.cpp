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
*     Global Panic function.
*
*/



// INCLUDE FILES

#include <e32std.h>             // for GLDEF_C
#include "Ringbc.pan"
//This file defines the Panic function.

// constants

_LIT(KRingBC,"Ringbc");

// ==================== LOCAL FUNCTIONS ====================

/**
 * Panic function.
 */
GLDEF_C void Panic(TRingBcPanic aPanic)
    {
    User::Panic(KRingBC, aPanic);
    }

//  End of File
