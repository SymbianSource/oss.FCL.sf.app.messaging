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
*     DLL internal functions and types implementation for Mce Engine
*     component.
*
*/



// INCLUDE FILES

#include "DllMain.h"

// LOCAL CONSTANTS AND MACROS
_LIT(KPanicText, "MceLE.dll");

// ================= MEMBER FUNCTIONS =======================

void LogsEnginePanic(
    TInt aCode) // panic code
{
  User::Panic(KPanicText, aCode);
}

// ==================== LOCAL FUNCTIONS ====================


// End of file
