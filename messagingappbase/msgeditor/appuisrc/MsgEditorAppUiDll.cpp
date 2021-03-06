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
* Description:  MsgEditorAppUiDll implementation
*
*/



// ========== INCLUDE FILES ================================

#include <e32std.h>                 // for GLDEF_C

#include "MsgEditorAppUiPanic.h"    // for MsgEditorModel panics

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ========== OTHER EXPORTED FUNCTIONS =====================

// ---------------------------------------------------------
// Panic
//
// ---------------------------------------------------------
//
GLDEF_C void Panic(TMsgEditorAppUiPanic aPanic)
{
    User::Panic(_L("MsgEditorAppUi"), aPanic);
}

//  End of File
