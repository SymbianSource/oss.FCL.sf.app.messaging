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
* Description:  MsgEditorModelPanic  declaration
*
*/



#ifndef INC_MSGEDITORMODELPANIC_H
#define INC_MSGEDITORMODELPANIC_H

// ========== INCLUDE FILES ================================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

enum TMsgEditorModelPanic
{
    ENoMessageParameter = 0x100,
    ENoMessageEntry,
    ENoSettings,
    ENoRFs,
    ENoSession,
    ENoMessage,
    EUnknownSessionEvent,
    EModelObserverRefeined,
    EInvalidOperation,
    EOperationNotComplete,
    EViewNotReady,
    EMsgInheritedFunctionMissing,
    EMsgZeroLength,
    ENullPointer,
    EMsgLayoutUndetermined
};

// ========== FUNCTION PROTOTYPES ==========================

// ---------------------------------------------------------
// Panic
//
// ---------------------------------------------------------
//
GLREF_C void Panic (TMsgEditorModelPanic aPanic);

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

#endif

// End of File
