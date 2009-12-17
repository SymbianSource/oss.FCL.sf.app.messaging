/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SmilPlayerPanic implementation
*
*/



// ========== INCLUDE FILES ================================
#include <e32std.h>
#include "SmilPlayerPanic.h"

// ========== FUNCTION PROTOTYPES ==========================

// ---------------------------------------------------------
// Panic implements
// panic handling for SmilPlayer
// Returns: nothing
// ---------------------------------------------------------
//
GLDEF_C void Panic( TSmilPlayerDialogPanic aPanic) // enum for panic codes
    {
    _LIT( KSmilPlayerPanic, "SmilPlayer" );
    User::Panic( KSmilPlayerPanic, aPanic );
    }

//  End of File
