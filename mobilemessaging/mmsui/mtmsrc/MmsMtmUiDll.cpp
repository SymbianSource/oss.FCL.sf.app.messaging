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
* Description: MmsMtmUiDll implementation
*
*/



// ========== INCLUDE FILES ================================

#include <e32std.h>                 // for GLDEF_C

#include "MmsMtmUiPanic.h"    // for panic enums

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================
_LIT( KModuleName, "MmsMtmUi" );

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
GLDEF_C void Panic( TMmsMtmUiPanic aPanic )
    {
    User::Panic( KModuleName, aPanic );
    }

//  End of File
