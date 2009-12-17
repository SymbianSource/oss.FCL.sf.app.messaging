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
* Description:  MsgEditorAppUiPanic  declaration
*
*/



#ifndef INC_MSGEDITORAPPUIPANIC_H
#define INC_MSGEDITORAPPUIPANIC_H

// ========== INCLUDE FILES ================================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

enum TMsgEditorAppUiPanic
{
    ENoValidCmdLine = -0x100,
//    ENoMessage,
//    EInvalidOperation,
    EInvalidArgument,
    ENullPointer1,
    EOverflow,
    EDeprecatedFunctionUsed,
    EMsgRepositorySettingFailure
};

// ========== FUNCTION PROTOTYPES ==========================

// ---------------------------------------------------------
// Panic
//
// ---------------------------------------------------------
//
GLREF_C void Panic (TMsgEditorAppUiPanic aPanic);

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

#endif

// End of File
