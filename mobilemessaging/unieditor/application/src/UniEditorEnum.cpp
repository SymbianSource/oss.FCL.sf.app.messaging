/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides Panic function for UniEditor application.
*
*/

 

// ========== INCLUDE FILES ================================

#include <e32std.h>         // for GLDEF_C
#include "UniEditorEnum.h"

// ---------------------------------------------------------
// Panic
// ---------------------------------------------------------
//
GLDEF_C void Panic( TUniEditorPanic aPanic )
    {
    _LIT( KPanicText,"UniEditor.exe" );
    User::Panic( KPanicText, aPanic );
    }   

//  End of File

