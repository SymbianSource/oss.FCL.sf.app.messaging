/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: PostcardUiDll implementation
*
*/



// ========== INCLUDE FILES ================================

#include <e32std.h>                 // for GLDEF_C

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================
_LIT( KModuleName, "PostcardUi" );

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
GLDEF_C void Panic( TInt aPanic )
    {
    User::Panic( KModuleName, aPanic );
    }

//  End of File
